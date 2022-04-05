import torch
import torch.nn as nn
from model.layers import DwsConvBlock
from math import ceil

import brevitas.nn as qnn
from .common import CommonActQuant, InputActQuant_TF, ScaledWeightQuant, InputActQuant_PySpeech
from .sinc_conv import SincConv
from brevitas.quant.binary import SignedBinaryWeightPerTensorConst, SignedBinaryActPerTensorConst
from brevitas.export.onnx.generic.manager import BrevitasONNXManager
from brevitas.quant.shifted_scaled_int import ShiftedUint8ActPerTensorFloat

from finn.util.inference_cost import inference_cost
import json
import numpy as np


class ModuleWithCost(nn.Module):
    """
    Parent class for supporting the calculation of the model size.
    """
    def __init__(self, in_features_shape):
        super().__init__()
        self._model_cost = None
        self._in_features_shape = in_features_shape

    def calculate_model_cost(self):
        # Calculate resource estimation for this particular model
        try:
            if self._model_cost is None:
                export_onnx_path = "tmp_model_export.onnx"
                final_onnx_path = "tmp_model_final.onnx"
                cost_dict_path = "tmp_model_cost.json"
                export_shape = (1, 1, *self._in_features_shape)
                BrevitasONNXManager.export(self.cpu(), input_t=torch.randn(export_shape), export_path=export_onnx_path)
                inference_cost(export_onnx_path, output_json=cost_dict_path, output_onnx=final_onnx_path,
                                                  preprocess=True, discount_sparsity=False)
                with open(cost_dict_path, 'r') as f:
                    self._model_cost = json.load(f)
                # Remove values, which are not supported as determined.ai evaluation return values
                del self._model_cost["discount_sparsity"]
                del self._model_cost["unsupported"]
            return self._model_cost
        except Exception as e:
            print("Received exception in model cost calculation. Skipping.")
            print("Exception is of type: ", type(e))
            print("And reads: ", e)
            return dict()


class DS_CNN(ModuleWithCost):
    """
    Depth-speraable convolutional network, based on this implementation: https://github.com/ARM-software/ML-KWS-for-MCU/blob/8151349b110f4d1c194c085fcc5b3535bdf7ce4a/models.py#L1066
    And DWS implementation from Brevitas: https://github.com/Xilinx/brevitas/blob/master/src/brevitas_examples/imagenet_classification/models/mobilenetv1.py#L43
    """
    def __init__(self, hparams, in_features_shape, data_config):
        super().__init__(in_features_shape)
        # Get the hyper parameters
        self.hparams = hparams
        in_channels = hparams["CNN_in_channels"]
        num_classes = hparams["num_classes"]
        t_dim = in_features_shape[1]
        f_dim = in_features_shape[0]

        # Build the network
        self.features = nn.ModuleList()

        # Conv block
        kernel_size = (4, 10)
        self.features.append(nn.Conv2d(
            in_channels=in_channels,
            out_channels=hparams["CNN_num_out_channels_1"],
            bias=False,
            kernel_size=kernel_size,
            stride=(hparams["CNN_strideFrequency_1"], hparams["CNN_strideTime_1"]),
        ))
        in_channels = hparams["CNN_num_out_channels_1"]
        self.features.append(nn.BatchNorm2d(num_features=in_channels))

        t_dim = (t_dim - kernel_size[1]) // hparams["CNN_strideTime_1"] + 1
        f_dim = (f_dim - kernel_size[0]) // hparams["CNN_strideFrequency_1"] + 1

        # DWS blocks
        for i in range(hparams["number_DS_layers"]):
            out_channels = hparams[f"DS_num_out_channels_{i+1}"]
            if i == 0:
                stride = hparams["DS_stride_1"]
            else:
                stride = 1
            self.features.append(DwsConvBlock(
                in_channels=in_channels,
                out_channels=out_channels,
                stride=stride
            ))
            in_channels = out_channels
            t_dim = ceil(t_dim / float(stride))
            f_dim = ceil(f_dim / float(stride))

        # Average pooling
        self.features.append(
            nn.AvgPool2d((f_dim, t_dim))
        )

        # Squeeze
        # We'll just flatten, that should do the trick instead.
        self.features.append(
            nn.Flatten()
        )

        # Output FC layer
        self.features.append(nn.Linear(
            in_features=out_channels,
            out_features=num_classes,
            bias=False))

    def forward(self, x):
        for mod in self.features:
            # print(mod, x.shape)
            x = mod(x)
        return x


class CNN(ModuleWithCost):
    """
    Convolutional network, based on this implementation: https://github.com/ARM-software/ML-KWS-for-MCU/blob/8151349b110f4d1c194c085fcc5b3535bdf7ce4a/models.py#L643
    """
    def __init__(self, hparams, in_features_shape, data_config):
        super().__init__(in_features_shape)
        # Get the hyper parameters
        self.hparams = hparams
        in_channels = hparams["CNN_in_channels"]
        conv_channels_list = [
            hparams["CNN_num_out_channels_1"],
            hparams["CNN_num_out_channels_2"],
        ]
        dropout = hparams["CNN_dropout"]
        num_fc_features = hparams["CNN_num_fc_features"]
        num_classes = hparams["num_classes"]

        t_dim = in_features_shape[1]
        f_dim = in_features_shape[0]

        # Build the network
        self.features = nn.ModuleList()

        # Conv blocks
        for i, out_channels in enumerate(conv_channels_list):
            kernel_size = (4, 10)
            stride = (1, i + 1)
            self.features.append(nn.Conv2d(
                in_channels=in_channels,
                out_channels=out_channels,
                bias=False,
                kernel_size=kernel_size,
                stride=stride,
            ))
            in_channels = out_channels
            self.features.append(nn.BatchNorm2d(num_features=in_channels))
            self.features.append(nn.ReLU())
            self.features.append(nn.Dropout(p=dropout))

            t_dim = ceil((t_dim - kernel_size[1]) / float(stride[1])) + 1
            f_dim = ceil((f_dim - kernel_size[0]) / float(stride[0])) + 1

        self.features.append(nn.Flatten())
        in_channels = t_dim * f_dim * in_channels

        # First FC layer
        self.features.append(nn.Linear(
            in_features=in_channels,
            out_features=num_fc_features,
            bias=False))
        self.features.append(nn.BatchNorm1d(num_features=num_fc_features))
        self.features.append(nn.ReLU())
        self.features.append(nn.Dropout(p=dropout))

        # Output FC layer
        self.features.append(nn.Linear(
            in_features=num_fc_features,
            out_features=num_classes,
            bias=False))

    def forward(self, x):
        for mod in self.features:
            # print(mod, x.shape)
            x = mod(x)
        return x


class QCNN(ModuleWithCost):
    """
    Quantized convolutional network, based on this implementation: https://github.com/ARM-software/ML-KWS-for-MCU/blob/8151349b110f4d1c194c085fcc5b3535bdf7ce4a/models.py#L643
    """
    def __init__(self, hparams, in_features_shape, data_config):
        super().__init__(in_features_shape)
        # Get the hyper parameters
        self.hparams = hparams
        self.data_config = data_config
        in_channels = hparams["CNN_in_channels"]
        conv_channels_list = [
            hparams["CNN_num_out_channels_1"],
            hparams["CNN_num_out_channels_2"],
        ]
        dropout = hparams["CNN_dropout"]
        num_fc_features = hparams["CNN_num_fc_features"]
        num_classes = hparams["num_classes"]
        conv_weight_bits_list = [hparams[f"Q_Conv{i}_weight_bits"] for i in range(1, len(conv_channels_list)+1)]
        conv_act_bits_list = [hparams[f"Q_Conv{i}_act_bits"] for i in range(1, len(conv_channels_list)+1)]

        t_dim = in_features_shape[1]
        f_dim = in_features_shape[0]

        # Build the network
        self.features = nn.ModuleList()

        self.features.append(qnn.QuantIdentity(act_quant=InputActQuant_TF, bit_width=hparams["Q_in_bits"]))

        # Conv blocks
        for i, out_channels in enumerate(conv_channels_list):
            kernel_size = (4, 10)
            stride = (1, i + 1)
            if conv_weight_bits_list[i] > 1:
                curr_weight_quant = ScaledWeightQuant.let(
                    scaling_init=hparams['Q_weight_scaling'],
                    scaling_per_output_channel=True)
            else:
                curr_weight_quant = SignedBinaryWeightPerTensorConst.let(
                    scaling_per_output_channel=True)
            if conv_act_bits_list[i] > 1:
                curr_act_quant = CommonActQuant
            else:
                curr_act_quant = SignedBinaryActPerTensorConst
            self.features.append(qnn.QuantConv2d(
                in_channels=in_channels,
                out_channels=out_channels,
                bias=False,
                kernel_size=kernel_size,
                stride=stride,
                weight_bit_width=conv_weight_bits_list[i],
                weight_quant=curr_weight_quant,
            ))
            in_channels = out_channels
            self.features.append(nn.BatchNorm2d(num_features=in_channels))
            self.features.append(qnn.QuantReLU(
                act_quant=curr_act_quant,
                bit_width=conv_act_bits_list[i],
            ))
            self.features.append(nn.Dropout(p=dropout))

            t_dim = ceil((t_dim - kernel_size[1]) / float(stride[1])) + 1
            f_dim = ceil((f_dim - kernel_size[0]) / float(stride[0])) + 1

        self.features.append(nn.Flatten())
        in_channels = t_dim * f_dim * in_channels
        # First FC layer
        if hparams['Q_FC1_weight_bits'] > 1:
            curr_weight_quant = ScaledWeightQuant.let(
                scaling_init=hparams['Q_weight_scaling'],
                scaling_per_output_channel=True
            )
        else:
            curr_weight_quant = SignedBinaryWeightPerTensorConst.let(
                scaling_per_output_channel=True
            )
        self.features.append(qnn.QuantLinear(
            in_features=in_channels,
            out_features=num_fc_features,
            bias=False,
            weight_bit_width=hparams['Q_FC1_weight_bits'],
            weight_quant=curr_weight_quant,
        ))
        self.features.append(nn.BatchNorm1d(num_features=num_fc_features))
        if hparams['Q_FC1_act_bits'] > 1:
            curr_act_quant = CommonActQuant
        else:
            curr_act_quant = SignedBinaryActPerTensorConst
        self.features.append(qnn.QuantReLU(
            act_quant=curr_act_quant,
            bit_width=hparams['Q_FC1_act_bits'],
        ))
        self.features.append(nn.Dropout(p=dropout))

        # Output FC layer
        if hparams["Q_out_bits"] > 1:
            curr_weight_quant = ScaledWeightQuant.let(
                scaling_init=hparams['Q_weight_scaling'],
                scaling_per_output_channel=False
            )
        else:
            curr_weight_quant = SignedBinaryWeightPerTensorConst.let(
                scaling_per_output_channel=False
            )
        self.features.append(qnn.QuantLinear(
            in_features=num_fc_features,
            out_features=num_classes,
            bias=False,
            weight_bit_width=hparams["Q_out_bits"],
            weight_quant=curr_weight_quant,
        ))

    def forward(self, x):
        for mod in self.features:
            # print(mod, x.shape)
            x = mod(x)
        return x


class MLP(ModuleWithCost):
    """
    MLP network, loosely adapted from the Brevitas bnn example, and very similar to the "Hello Edge" DNN.
    """
    def __init__(self, hparams, in_features_shape, data_config):
        super().__init__(in_features_shape)
        # Get the hyper parameters
        self.hparams = hparams
        in_features = in_features_shape[0] * in_features_shape[1]
        out_features_list = [
            hparams["MLP_num_out_features_1"],
            hparams["MLP_num_out_features_2"],
            hparams["MLP_num_out_features_3"],
        ]
        dropout = hparams["MLP_dropout"]
        num_classes = hparams["num_classes"]

        # Build the network
        self.features = nn.ModuleList()
        self.features.append(nn.Flatten())

        for out_features in out_features_list:
            self.features.append(nn.Linear(
                in_features=in_features,
                out_features=out_features,
                bias=False, ))
            in_features = out_features
            self.features.append(nn.BatchNorm1d(num_features=in_features))
            self.features.append(nn.ReLU())
            self.features.append(nn.Dropout(p=dropout))

        self.features.append(nn.Linear(
            in_features=in_features,
            out_features=num_classes,
            bias=False))

    def forward(self, x):
        for mod in self.features:
            # print(mod, x.shape)
            x = mod(x)
        return x

class QMLP(ModuleWithCost):
    """
    Quantized MLP network, loosely adapted from the Brevitas bnn example, and very similar to the "Hello Edge" DNN.
    """
    def __init__(self, hparams, in_features_shape, data_config):
        super().__init__(in_features_shape)
        # Get the hyper parameters
        self.hparams = hparams
        self.data_config = data_config
        in_features = in_features_shape[0] * in_features_shape[1]
        out_features_list = [hparams[f"MLP_num_out_features_{i}"] for i in range(1, 4)]
        weight_bits_list = [hparams[f"Q_L{i}_weight_bits"] for i in range(1, 4)]
        act_bits_list = [hparams[f"Q_L{i}_act_bits"] for i in range(1, 4)]
        dropout = hparams["MLP_dropout"]
        num_classes = hparams["num_classes"]

        # Build the network
        input_quantizer = None
        if hparams["Q_in_learned"]:
            input_quantizer = ShiftedUint8ActPerTensorFloat
        else:
            if self.data_config['dataset_preprocessor'] == "tf":
                if self.data_config['tf_feature_type'] == 'mfcc_py':
                    input_quantizer = InputActQuant_PySpeech
                elif self.data_config['tf_feature_type'] == 'mfcc':
                    input_quantizer = InputActQuant_TF
        if input_quantizer is None:
            raise ValueError("Could not determine the fitting input quantizer for this data config.")

        self.features = nn.ModuleList()
        self.features.append(qnn.QuantIdentity(act_quant=input_quantizer, bit_width=hparams["Q_in_bits"]))
        self.features.append(nn.Dropout(p=dropout))

        self.features.append(nn.Flatten())

        for i, out_features in enumerate(out_features_list):
            # Get quantizers for this loop
            if weight_bits_list[i] > 1:
                curr_weight_quant = ScaledWeightQuant.let(
                    scaling_init=hparams['Q_weight_scaling'],
                    scaling_per_output_channel=True)
            else:
                curr_weight_quant = SignedBinaryWeightPerTensorConst.let(
                    scaling_per_output_channel=True)
            if act_bits_list[i] > 1:
                curr_act_quant = CommonActQuant
            else:
                curr_act_quant = SignedBinaryActPerTensorConst

            # Extend layer stack
            self.features.append(qnn.QuantLinear(
                in_features=in_features,
                out_features=out_features,
                bias=False,
                weight_bit_width=weight_bits_list[i],
                weight_quant=curr_weight_quant,
                #weight_quant=ScaledWeightQuant_generator(hparams['Q_weight_scaling']),
                #weight_quant=CommonWeightQuant,
            ))
            in_features = out_features
            self.features.append(nn.BatchNorm1d(num_features=in_features))
            self.features.append(qnn.QuantReLU(
                act_quant=curr_act_quant,
                #act_quant=CommonActQuant,
                bit_width=act_bits_list[i],
            ))
            self.features.append(nn.Dropout(p=dropout))

        if hparams["Q_out_bits"] > 1:
            curr_weight_quant = ScaledWeightQuant.let(
                scaling_init=hparams['Q_weight_scaling'],
                scaling_per_output_channel=False
            )
        else:
            curr_weight_quant = SignedBinaryWeightPerTensorConst.let(
                scaling_per_output_channel=False
            )
        self.features.append(qnn.QuantLinear(
            in_features=in_features,
            out_features=num_classes,
            bias=False,
            weight_bit_width=hparams["Q_out_bits"],
            weight_quant=curr_weight_quant,
            #weight_quant=ScaledWeightQuant_generator(hparams['Q_weight_scaling']),
            #weight_quant=CommonWeightQuant,
        ))

    def forward(self, x):
        for mod in self.features:
            # print(mod, x.shape)
            x = mod(x)
        return x

class RNN(nn.Module):
    # ToDo: Convert to determined.ai hparams equivalent
    def __init__(self, hparams):
        super(RNN, self).__init__()
        self.hparams = hparams
        self.hidden_size = self.hparams.model_config.hidden_size
        if self.hparams.model_config.model_architecture == 'lstm':
            rnn = nn.LSTM
        else:
            rnn = nn.GRU
        dct_coefficient_count = self.hparams.preprocess_config.n_mfcc
        label_count = self.hparams.preprocess_config.label_count
        spectrogram_length = self.hparams.preprocess_config.spectrogram_length
        self.rnn = rnn(input_size=dct_coefficient_count, hidden_size=self.hidden_size,
                       batch_first=True)
        self.linear = nn.Linear(self.hidden_size, label_count)
        self.init_weights()
        self.input_frequency_size = dct_coefficient_count
        self.input_time_size = spectrogram_length

    def init_weights(self):
        for name, value in self.named_parameters():
            if 'weight' in name:
                torch.nn.init.xavier_uniform_(value.data)
            elif 'bias' in name:
                torch.nn.init.zeros_(value.data)
        if self.hparams.model_config.model_architecture == 'lstm':
            # Set bias of forget gate to 1
            self.rnn.bias_ih_l0.data[1 * self.hidden_size:2 * self.hidden_size] = torch.ones(self.hidden_size)
            self.rnn.bias_hh_l0.data[1 * self.hidden_size:2 * self.hidden_size] = torch.ones(self.hidden_size)

    def forward(self, input):
        input = input.view(-1, self.input_frequency_size, self.input_time_size)
        input = input.transpose(1, 2)
        output, _ = self.rnn(input)
        output = output[:, -1, :].squeeze(1)
        output = self.linear(output)
        return output


# class QRNN(nn.Module):
#     def __init__(self, hparams):
#         super(QRNN, self).__init__()
#         self.hparams = hparams
#         dct_coefficient_count = self.hparams.preprocess_config.n_mfcc
#         label_count = self.hparams.preprocess_config.label_count
#         spectrogram_length = self.hparams.preprocess_config.spectrogram_length
#         self.hidden_size = self.hparams.model_config.hidden_size
#
#         rnn_weight_config = dict(self.hparams.weight_config)
#         rnn_weight_config['weight_bit_width'] = self.hparams.bit_width_weight_config.rnn.bit_width
#
#         rnn_activation_config = dict(self.hparams.rnn_activation_config)
#         rnn_activation_config['bit_width'] = self.hparams.bit_width_act_config.rnn.bit_width
#
#         rnn_norm_scale_out_config = dict(self.hparams.rnn_norm_scale_out_config)
#         rnn_norm_scale_out_config['bit_width'] = self.hparams.bit_width_act_config.rnn.bit_width
#
#         rnn_norm_scale_hidden_config = dict(self.hparams.rnn_norm_scale_hidden_config)
#         rnn_norm_scale_hidden_config['bit_width'] = self.hparams.bit_width_act_config.rnn.bit_width
#
#         linear_weight = dict(self.hparams.linear_config)
#         linear_weight['weight_bit_width'] = self.hparams.bit_width_weight_config.linear.bit_width
#
#         if self.hparams.model_config.model_architecture == 'qlstm':
#             rnn = qnn.QuantLSTMLayer
#             # conf1 = rnn_norm_scale_out_config
#             # conf2 = rnn_norm_scale_hidden_config
#         else:
#             rnn = qnn.QuantGRULayer
#             # conf1 = rnn_norm_scale_hidden_config
#             # conf2 = rnn_norm_scale_out_config
#         self.rnn = rnn(input_size=dct_coefficient_count,
#                        hidden_size=self.hidden_size,
#                        batch_first=True,
#                        weight_config=rnn_weight_config,
#                        activation_config=rnn_activation_config,
#                        norm_scale_out_config=rnn_norm_scale_out_config,
#                        norm_scale_hidden_config=rnn_norm_scale_hidden_config)
#         self.linear = qnn.QuantLinear(self.hidden_size, label_count, bias=True,
#                                       **linear_weight)
#         if self.hparams.weight_init == 'tensorflow':
#             self.init_weights()
#         self.input_frequency_size = dct_coefficient_count
#         self.input_time_size = spectrogram_length
#
#         if self.hparams.tensornorm.value:
#             self.last = TensorNorm()
#         else:
#             self.last = nn.Identity()
#
#     def init_weights(self):
#         hidden_size = self.hidden_size
#         weight_hidden = torch.cat([self.rnn.weight_ch.t(), self.rnn.weight_nh.t(), self.rnn.weight_rh.t()], dim=0)
#         weight_input = torch.cat([self.rnn.weight_ci.t(), self.rnn.weight_ni.t(), self.rnn.weight_ri.t()], dim=0)
#         torch.nn.init.xavier_uniform_(weight_hidden)
#         torch.nn.init.xavier_uniform_(weight_input)
#         self.rnn.weight_ch = torch.nn.Parameter(weight_hidden[:hidden_size, :].t())
#         self.rnn.weight_nh = torch.nn.Parameter(weight_hidden[hidden_size:2 * hidden_size, :].t())
#         self.rnn.weight_rh = torch.nn.Parameter(weight_hidden[2 * hidden_size:, :].t())
#
#         self.rnn.weight_ci = torch.nn.Parameter(weight_input[:hidden_size, :].t())
#         self.rnn.weight_ni = torch.nn.Parameter(weight_input[hidden_size:2 * hidden_size, :].t())
#         self.rnn.weight_ri = torch.nn.Parameter(weight_input[2 * hidden_size:, :].t())
#
#         torch.nn.init.xavier_uniform_(self.linear.weight)
#         for name, value in self.named_parameters():
#             if 'bias' in name:
#                 torch.nn.init.zeros_(value.data)
#
#     def forward(self, input):
#         input = input.view(-1, self.input_frequency_size, self.input_time_size)
#         input = input.transpose(1, 2)
#         output, _ = self.rnn(input)
#         output = output[:, -1, :].squeeze(1)
#         output = self.linear(output)
#         output = self.last(output)
#         return output


class SincConv_MLP(ModuleWithCost):
    """
    MLP network with SincConvs for feature extraction.
    The MLP is loosely adapted from the Brevitas bnn example, and very similar to the "Hello Edge" DNN.
    """
    def __init__(self, hparams, in_features_shape, data_config):
        super().__init__(in_features_shape)
        # Get the hyper parameters
        self.hparams = hparams
        self.data_args = data_config

        # SincConv settings to match the MFCC settings
        sc_filters = self.hparams['sinc_filters']
        sc_kernel_size = self.hparams['sinc_kernel_size']
        sc_stride = self.hparams['sinc_stride']

        # MLP settings
        in_features = int(np.floor((self.data_args['tf_sample_rate'] - sc_kernel_size) / sc_stride) + 1) * sc_filters
        out_features_list = [
            hparams["MLP_num_out_features_1"],
            hparams["MLP_num_out_features_2"],
            hparams["MLP_num_out_features_3"],
        ]
        dropout = hparams["MLP_dropout"]
        num_classes = hparams["num_classes"]

        # Build the network
        self.features = nn.ModuleList()

        self.features.append(SincConv(1, sc_filters, sc_kernel_size, stride=sc_stride))

        self.features.append(nn.Flatten())

        for out_features in out_features_list:
            self.features.append(nn.Linear(
                in_features=in_features,
                out_features=out_features,
                bias=False, ))
            in_features = out_features
            self.features.append(nn.BatchNorm1d(num_features=in_features))
            self.features.append(nn.ReLU())
            self.features.append(nn.Dropout(p=dropout))

        self.features.append(nn.Linear(
            in_features=in_features,
            out_features=num_classes,
            bias=False))

    def forward(self, x):
        for mod in self.features:
            # print(mod, x.shape)
            x = mod(x)
        return x


available_models = {
    "MLP": MLP,
    "QMLP": QMLP,
    "RNN": RNN,
    "CNN": CNN,
    "QCNN": QCNN,
    "DS-CNN": DS_CNN,
    "SincConv_MLP": SincConv_MLP,
}
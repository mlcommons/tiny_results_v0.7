from typing import Any, Dict, Sequence, Tuple, Union, cast

import torch
from torch import nn

from determined.pytorch import DataLoader, PyTorchTrial, PyTorchTrialContext, PyTorchCallback
import determined

from data.preprocessing_module import PYPreprocessTransform
from data.gsc_dataset import KWSDatasetPy, KWSDatasetTf
import data.input_data_tf as input_data_tf
from model.models import available_models
from model.RMSProp import RMSpropTF
from torch.optim import lr_scheduler, RMSprop, Adam
from brevitas.export.onnx.generic.manager import BrevitasONNXManager
from sklearn.metrics import confusion_matrix
import numpy as np
from torch.utils.data import WeightedRandomSampler

TorchData = Union[Dict[str, torch.Tensor], Sequence[torch.Tensor], torch.Tensor]


class KWSTrial(PyTorchTrial):
    def __init__(self, context: PyTorchTrialContext) -> None:
        self.context = context
        self.hparams = self.context.get_hparams()
        self.data_config = self.context.get_data_config()
        if self.data_config["tf_feature_type"] == "td_samples" and self.data_config["dataset_preprocessor"] == "tf":
            self.in_features_shape = (self.data_config["tf_sample_rate"], )
        else:
            self.in_features_shape = self.data_config['preprocess_config_n_mfcc'], self.data_config['preprocess_config_spectrogram_length']

        # Create model
        nn_class = available_models[self.hparams['nn_to_train']]
        unwraped_model = nn_class(self.hparams, self.in_features_shape, self.data_config)
        # Calculate the models cost in advance
        self.model_cost = unwraped_model.calculate_model_cost()
        # Now hand over the model
        self.model = self.context.wrap_model(unwraped_model)

        learning_rate = self.hparams['learning_rate']
        # Create optimizer and scheduler
        if self.hparams['optimizer'] == "RMSpropTF":
            self.optimizer = self.context.wrap_optimizer(
                RMSpropTF(self.model.parameters(), lr=learning_rate)
            )
        elif self.hparams['optimizer'] == "RMSpropPy":
            # Note the hyperparameters were adjusted to reflect the default of RMSpropTF
            self.optimizer = self.context.wrap_optimizer(
                RMSprop(self.model.parameters(), lr=learning_rate, alpha=0.9, eps=1e-10)
            )
        elif self.hparams['optimizer'] == "Adam":
            self.optimizer = self.context.wrap_optimizer(
                Adam(self.model.parameters(), lr=learning_rate)
            )
        else:
            raise ValueError(f"Optimizer scheduler with name: {hparams['loss_function']} is currently not supported.")

        # Create LR scheduler
        if self.hparams['LR_scheduler'] == "CosineAnnealingLR":
            num_epochs = self.context.get_experiment_config()['searcher']['max_length']['epochs']
            self.lr_scheduler = self.context.wrap_lr_scheduler(
                lr_scheduler.CosineAnnealingLR(self.optimizer, num_epochs, eta_min=1e-8),
                step_mode=determined.pytorch._lr_scheduler.LRScheduler.StepMode.STEP_EVERY_EPOCH,
            )
        else:
            raise ValueError(f"Learning rate scheduler with name: {hparams['loss_function']} is currently not supported.")

        # Get training data
        if self.data_config['dataset_preprocessor'] == "torchaudio":
            self.audio_processor = PYPreprocessTransform(self.data_config)
            dataset_test = KWSDatasetPy(self.audio_processor, 'testing')
        elif self.data_config['dataset_preprocessor'] == "tf":
            self._ds_train, self._ds_test, self._ds_val = input_data_tf.get_training_data(self.data_config)
            # Immediately convert to datasets
            self._ds_train = KWSDatasetTf(self._ds_train, self.data_config)
            self._ds_test = KWSDatasetTf(self._ds_test, self.data_config)
            self._ds_val = KWSDatasetTf(self._ds_val, self.data_config)
        else:
            raise ValueError(
                f"Dataset preprocessor with name: {self.data_config['dataset_preprocessor']} is currently not supported.")
        # Create test DataLoader
        self.test_dataloader = DataLoader(self._ds_test, batch_size=self.context.get_per_slot_batch_size(),
                                          num_workers=5, shuffle=False)
        # Create balanced validation DataLoader
        # Compute weights to balance the validation dataset
        bins, edges = np.histogram(self._ds_val._label_array, 12)
        val_sample_draw_period = bins.sum() / bins
        sample_weights = np.zeros_like(self._ds_val._label_array, dtype=np.float32)
        for i, label in enumerate(self._ds_val._label_array):
            sample_weights[i] = val_sample_draw_period[label]
        # Balance dataset with the WeightedRandomSampler
        val_sampler = WeightedRandomSampler(sample_weights, len(sample_weights), replacement=True)
        self.balanced_val_dataloader = DataLoader(self._ds_val, batch_size=self.context.get_per_slot_batch_size(),
                                                  num_workers=5, shuffle=False, sampler=val_sampler)

        # Create loss function
        if self.hparams['weight_loss_function']:
            if self.data_config['dataset_preprocessor'] != "tf":
                raise ValueError("Weighting is currently not implemented for other dataset_preprocessor than tf.")
            # Compute weights to balance the training dataset
            bins, edges = np.histogram(self._ds_train._label_array, 12)
            class_density = bins / bins.sum()
            inverse_class_density = 1 - class_density
            # Further suppress the "unknown" label, since it is severely overrepresented in training data
            inverse_class_density[-1] /= self.hparams['label_suppression_unknown']
            inverse_class_density /= inverse_class_density.sum()
            inverse_class_density = inverse_class_density.astype(np.float32)
            loss_weight = torch.from_numpy(inverse_class_density).cuda()
        else:
            loss_weight = None
        if self.hparams['loss_function'] == "CrossEntropyLoss":
            self.loss_function = nn.CrossEntropyLoss(weight=loss_weight)
        elif self.hparams['loss_function'] == "MultiMarginLoss":
            self.loss_function = nn.MultiMarginLoss(p=2, weight=loss_weight)
        else:
            raise ValueError(f"Loss function with name: {hparams['loss_function']} is currently not supported.")

    def build_training_data_loader(self) -> DataLoader:
        if self.data_config['dataset_preprocessor'] == "torchaudio":
            dataset_train = KWSDatasetPy(self.audio_processor, 'training')
        elif self.data_config['dataset_preprocessor'] == "tf":
            dataset_train = self._ds_train
        return DataLoader(dataset_train, batch_size=self.context.get_per_slot_batch_size(), num_workers=5, shuffle=True)

    def build_validation_data_loader(self) -> DataLoader:
        if self.data_config['dataset_preprocessor'] == "torchaudio":
            dataset_val = KWSDatasetPy(self.audio_processor, 'validation')
        elif self.data_config['dataset_preprocessor'] == "tf":
            dataset_val = self._ds_val
        return DataLoader(dataset_val, batch_size=self.context.get_per_slot_batch_size(), num_workers=5, shuffle=False)

    def train_batch(self, batch: TorchData, epoch_idx: int, batch_idx: int) -> Dict[str, torch.Tensor]:
        # Unpack data
        batch = cast(Tuple[torch.Tensor, torch.Tensor], batch)
        data, labels = batch

        # Calculate output and loss
        output = self.model(data)
        loss = self.loss_function(output, labels)

        # Do backprop
        self.context.backward(loss)
        self.context.step_optimizer(self.optimizer)

        # Get Top1 accuracy
        pred = output.argmax(dim=1, keepdim=True)
        accuracy = pred.eq(labels.view_as(pred)).sum().item() / len(data)

        return_dict = {"loss": loss, "training_accuracy": accuracy}
        return return_dict

    def evaluate_batch(self, batch: TorchData) -> Dict[str, Any]:
        # Do basically the same as for training, just without backprop or gradient tracking (not visible here)
        batch = cast(Tuple[torch.Tensor, torch.Tensor], batch)
        data, labels = batch

        output = self.model(data)
        validation_loss = self.loss_function(output, labels).item()

        prediction = output.argmax(dim=1, keepdim=True)
        accuracy = prediction.eq(labels.view_as(prediction)).sum().item() / len(data)

        # Combine results and model cost
        validation_result = {"validation_loss": validation_loss, "validation_accuracy": accuracy}
        validation_result.update(self.model_cost)

        return validation_result

    def build_callbacks(self) -> Dict[str, PyTorchCallback]:
        hooks = {
            "balanced_val_eval_callback": EvaluateDatasetCallback(self.model, self.balanced_val_dataloader,
                                                                  self.loss_function, "balanced_validation"),
            "test_eval_callback": EvaluateDatasetCallback(self.model, self.test_dataloader, self.loss_function, "test"),
            "qonnx_checkpoint_callback": QONNXCheckpointCallback(self.model, self.in_features_shape),
        }
        return hooks


class QONNXCheckpointCallback(PyTorchCallback):
    def __init__(self, model, in_features_shape) -> None:
        self.model = model
        self._in_features_shape = in_features_shape
        super().__init__()

    def on_checkpoint_end(self, checkpoint_dir: str) -> None:
        # Export the model as QONNX
        export_path = checkpoint_dir + "/QONNX_model.onnx"
        in_shape = (1, 1, *self._in_features_shape)
        self.model = self.model.cpu()
        _ = BrevitasONNXManager.export(self.model, in_shape, export_path=export_path, opset_version=11)
        self.model = self.model.to("cuda:0")
        return None


class EvaluateDatasetCallback(PyTorchCallback):

    def __init__(self, model, dataloader, loss_function, dataset_name) -> None:
        self.model = model
        self.dataloader = dataloader
        self.loss_function = loss_function
        self.dataset_name = dataset_name
        super().__init__()

    def on_validation_end(self, metrics: Dict[str, Any]) -> None:
        # House keeping
        test_loss_sum = 0.
        test_acc_sum = 0.
        data_counter = 0

        # Get confusion matrix
        predlist = torch.zeros(0, dtype=torch.long, device='cpu')
        lbllist = torch.zeros(0, dtype=torch.long, device='cpu')

        # Run validation on the test dataset
        self.model.eval()
        with torch.no_grad():
            for data, labels in self.dataloader:
                data, labels = data.cuda(), labels.cuda()
                output = self.model(data)
                loss = self.loss_function(output, labels).item()
                test_loss_sum += loss * len(data)
                prediction = output.argmax(dim=1, keepdim=True)
                accuracy = prediction.eq(labels.view_as(prediction)).sum().item()
                test_acc_sum += accuracy
                data_counter += len(data)

                # Append batch prediction results
                predlist = torch.cat([predlist, prediction.view(-1).cpu()])
                lbllist = torch.cat([lbllist, labels.view(-1).cpu()])

        conf_mat = confusion_matrix(lbllist.numpy(), predlist.numpy())

        self.model.train()

        # Update the metrics dictionary
        metrics[f"{self.dataset_name}_loss"] = test_loss_sum / data_counter
        metrics[f"{self.dataset_name}_accuracy"] = test_acc_sum / data_counter
        metrics[f"{self.dataset_name}_confusion_matrix"] = conf_mat

        return None

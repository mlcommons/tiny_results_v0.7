from torch import nn


# DWS implementation from Brevitas: https://github.com/Xilinx/brevitas/blob/master/src/brevitas_examples/imagenet_classification/models/mobilenetv1.py#L43
class DwsConvBlock(nn.Module):
    def __init__(
            self,
            in_channels,
            out_channels,
            stride
    ):
        super(DwsConvBlock, self).__init__()
        self.dw_conv = ConvBlock(
            in_channels=in_channels,
            out_channels=in_channels,
            groups=in_channels,
            kernel_size=3,
            padding=1,
            stride=stride,)
        self.pw_conv = ConvBlock(
            in_channels=in_channels,
            out_channels=out_channels,
            kernel_size=1,
            padding=0,)

    def forward(self, x):
        x = self.dw_conv(x)
        x = self.pw_conv(x)
        return x


class ConvBlock(nn.Module):

    def __init__(
            self,
            in_channels,
            out_channels,
            kernel_size,
            stride=1,
            padding=0,
            groups=1,
            bn_eps=1e-5,
    ):
        super(ConvBlock, self).__init__()
        self.conv = nn.Conv2d(
            in_channels=in_channels,
            out_channels=out_channels,
            kernel_size=kernel_size,
            stride=stride,
            padding=padding,
            groups=groups,
            bias=False,)
        self.bn = nn.BatchNorm2d(num_features=out_channels, eps=bn_eps)
        self.activation = nn.ReLU()

    def forward(self, x):
        x = self.conv(x)
        x = self.bn(x)
        x = self.activation(x)
        return x

# class QConvBlock(nn.Module):
#
#     def __init__(
#             self,
#             in_channels,
#             out_channels,
#             kernel_size,
#             weight_bit_width,
#             act_bit_width,
#             stride=1,
#             padding=0,
#             groups=1,
#             bn_eps=1e-5,
#             activation_scaling_per_channel=False):
#         super(QConvBlock, self).__init__()
#         self.conv = QuantConv2d(
#             in_channels=in_channels,
#             out_channels=out_channels,
#             kernel_size=kernel_size,
#             stride=stride,
#             padding=padding,
#             groups=groups,
#             bias=False,
#             weight_quant=CommonIntWeightPerChannelQuant,
#             weight_bit_width=weight_bit_width)
#         self.bn = nn.BatchNorm2d(num_features=out_channels, eps=bn_eps)
#         self.activation = QuantReLU(
#             act_quant=CommonUintActQuant,
#             bit_width=act_bit_width,
#             per_channel_broadcastable_shape=(1, out_channels, 1, 1),
#             scaling_per_channel=activation_scaling_per_channel,
#             return_quant_tensor=True)
#
#     def forward(self, x):
#         x = self.conv(x)
#         x = self.bn(x)
#         x = self.activation(x)
#         return x

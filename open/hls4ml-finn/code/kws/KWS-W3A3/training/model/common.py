# MIT License
#
# Copyright (c) 2019 Xilinx
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

from dependencies import value

from brevitas.inject import ExtendedInjector
from brevitas.quant.solver import WeightQuantSolver, ActQuantSolver
from brevitas.core.bit_width import BitWidthImplType
from brevitas.core.quant import QuantType
from brevitas.core.restrict_val import RestrictValueType, FloatToIntImplType
from brevitas.core.scaling import ScalingImplType
from brevitas.core.zero_point import ZeroZeroPoint


class CommonQuant(ExtendedInjector):
    bit_width_impl_type = BitWidthImplType.CONST
    restrict_scaling_type = RestrictValueType.FP
    zero_point_impl = ZeroZeroPoint
    float_to_int_impl_type = FloatToIntImplType.ROUND
    narrow_range = True
    signed = True

    @value
    def quant_type(bit_width):
        if bit_width is None:
            return QuantType.FP
        elif bit_width == 1:
            return QuantType.BINARY
        else:
            return QuantType.INT


class ScaledWeightQuant(CommonQuant, WeightQuantSolver):
    scaling_impl_type = 'parameter'
    scaling_per_output_channel = True


# def ScaledWeightQuant_generator(scaling_init_ext):
#     class ScaledWeightQuant_intern(CommonQuant, WeightQuantSolver):
#         scaling_impl_type = 'parameter'
#         scaling_init = scaling_init_ext
#         scaling_per_output_channel = True
#     return ScaledWeightQuant_intern


# class CommonWeightQuant(CommonQuant, WeightQuantSolver):
#     scaling_impl_type = ScalingImplType.CONST
#     scaling_const = 0.001
#     #scaling_impl_type = 'parameter'
#     #scaling_init = 0.001
#     scaling_per_output_channel = True


class CommonActQuant(CommonQuant, ActQuantSolver):
    min_val = 0.0
    max_val = 1.0
    scaling_impl_type = ScalingImplType.CONST
    scaling_per_output_channel = False
    narrow_range = False
    signed = False


class InputActQuant_TF(CommonQuant, ActQuantSolver):
    """
    Based on the 99.-th percentile of the training data
    Note: With the current settings for the scaling and zero point,
    this will result in a symmetrical quantization between -42.8 and 42.8
    """
    min_val = -42.7662912750244
    max_val = 12.415506248474117
    scaling_impl_type = ScalingImplType.CONST
    scaling_per_output_channel = False


class InputActQuant_PySpeech(CommonQuant, ActQuantSolver):
    """
    Based on the 99.-th percentile of the training data
    Note: With the current settings for the scaling and zero point,
    this will result in a symmetrical quantization between -105.391 and 105.391
    """
    min_val = -105.391
    max_val = 16.351
    scaling_impl_type = ScalingImplType.CONST
    scaling_per_output_channel = False
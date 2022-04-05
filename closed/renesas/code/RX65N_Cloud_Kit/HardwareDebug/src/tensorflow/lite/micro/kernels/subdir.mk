################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/tensorflow/lite/micro/kernels/activations.cc \
../src/tensorflow/lite/micro/kernels/add.cc \
../src/tensorflow/lite/micro/kernels/arg_min_max.cc \
../src/tensorflow/lite/micro/kernels/ceil.cc \
../src/tensorflow/lite/micro/kernels/circular_buffer.cc \
../src/tensorflow/lite/micro/kernels/comparisons.cc \
../src/tensorflow/lite/micro/kernels/concatenation.cc \
../src/tensorflow/lite/micro/kernels/conv.cc \
../src/tensorflow/lite/micro/kernels/depthwise_conv.cc \
../src/tensorflow/lite/micro/kernels/dequantize.cc \
../src/tensorflow/lite/micro/kernels/elementwise.cc \
../src/tensorflow/lite/micro/kernels/ethosu.cc \
../src/tensorflow/lite/micro/kernels/floor.cc \
../src/tensorflow/lite/micro/kernels/fully_connected.cc \
../src/tensorflow/lite/micro/kernels/hard_swish.cc \
../src/tensorflow/lite/micro/kernels/kernel_runner.cc \
../src/tensorflow/lite/micro/kernels/kernel_util.cc \
../src/tensorflow/lite/micro/kernels/l2norm.cc \
../src/tensorflow/lite/micro/kernels/logical.cc \
../src/tensorflow/lite/micro/kernels/logistic.cc \
../src/tensorflow/lite/micro/kernels/maximum_minimum.cc \
../src/tensorflow/lite/micro/kernels/mul.cc \
../src/tensorflow/lite/micro/kernels/neg.cc \
../src/tensorflow/lite/micro/kernels/pack.cc \
../src/tensorflow/lite/micro/kernels/pad.cc \
../src/tensorflow/lite/micro/kernels/pooling.cc \
../src/tensorflow/lite/micro/kernels/prelu.cc \
../src/tensorflow/lite/micro/kernels/quantize.cc \
../src/tensorflow/lite/micro/kernels/reduce.cc \
../src/tensorflow/lite/micro/kernels/reshape.cc \
../src/tensorflow/lite/micro/kernels/resize_nearest_neighbor.cc \
../src/tensorflow/lite/micro/kernels/round.cc \
../src/tensorflow/lite/micro/kernels/shape.cc \
../src/tensorflow/lite/micro/kernels/softmax.cc \
../src/tensorflow/lite/micro/kernels/split.cc \
../src/tensorflow/lite/micro/kernels/split_v.cc \
../src/tensorflow/lite/micro/kernels/strided_slice.cc \
../src/tensorflow/lite/micro/kernels/sub.cc \
../src/tensorflow/lite/micro/kernels/svdf.cc \
../src/tensorflow/lite/micro/kernels/tanh.cc \
../src/tensorflow/lite/micro/kernels/unpack.cc 

CC_DEPS += \
./src/tensorflow/lite/micro/kernels/activations.d \
./src/tensorflow/lite/micro/kernels/add.d \
./src/tensorflow/lite/micro/kernels/arg_min_max.d \
./src/tensorflow/lite/micro/kernels/ceil.d \
./src/tensorflow/lite/micro/kernels/circular_buffer.d \
./src/tensorflow/lite/micro/kernels/comparisons.d \
./src/tensorflow/lite/micro/kernels/concatenation.d \
./src/tensorflow/lite/micro/kernels/conv.d \
./src/tensorflow/lite/micro/kernels/depthwise_conv.d \
./src/tensorflow/lite/micro/kernels/dequantize.d \
./src/tensorflow/lite/micro/kernels/elementwise.d \
./src/tensorflow/lite/micro/kernels/ethosu.d \
./src/tensorflow/lite/micro/kernels/floor.d \
./src/tensorflow/lite/micro/kernels/fully_connected.d \
./src/tensorflow/lite/micro/kernels/hard_swish.d \
./src/tensorflow/lite/micro/kernels/kernel_runner.d \
./src/tensorflow/lite/micro/kernels/kernel_util.d \
./src/tensorflow/lite/micro/kernels/l2norm.d \
./src/tensorflow/lite/micro/kernels/logical.d \
./src/tensorflow/lite/micro/kernels/logistic.d \
./src/tensorflow/lite/micro/kernels/maximum_minimum.d \
./src/tensorflow/lite/micro/kernels/mul.d \
./src/tensorflow/lite/micro/kernels/neg.d \
./src/tensorflow/lite/micro/kernels/pack.d \
./src/tensorflow/lite/micro/kernels/pad.d \
./src/tensorflow/lite/micro/kernels/pooling.d \
./src/tensorflow/lite/micro/kernels/prelu.d \
./src/tensorflow/lite/micro/kernels/quantize.d \
./src/tensorflow/lite/micro/kernels/reduce.d \
./src/tensorflow/lite/micro/kernels/reshape.d \
./src/tensorflow/lite/micro/kernels/resize_nearest_neighbor.d \
./src/tensorflow/lite/micro/kernels/round.d \
./src/tensorflow/lite/micro/kernels/shape.d \
./src/tensorflow/lite/micro/kernels/softmax.d \
./src/tensorflow/lite/micro/kernels/split.d \
./src/tensorflow/lite/micro/kernels/split_v.d \
./src/tensorflow/lite/micro/kernels/strided_slice.d \
./src/tensorflow/lite/micro/kernels/sub.d \
./src/tensorflow/lite/micro/kernels/svdf.d \
./src/tensorflow/lite/micro/kernels/tanh.d \
./src/tensorflow/lite/micro/kernels/unpack.d 

OBJS += \
./src/tensorflow/lite/micro/kernels/activations.o \
./src/tensorflow/lite/micro/kernels/add.o \
./src/tensorflow/lite/micro/kernels/arg_min_max.o \
./src/tensorflow/lite/micro/kernels/ceil.o \
./src/tensorflow/lite/micro/kernels/circular_buffer.o \
./src/tensorflow/lite/micro/kernels/comparisons.o \
./src/tensorflow/lite/micro/kernels/concatenation.o \
./src/tensorflow/lite/micro/kernels/conv.o \
./src/tensorflow/lite/micro/kernels/depthwise_conv.o \
./src/tensorflow/lite/micro/kernels/dequantize.o \
./src/tensorflow/lite/micro/kernels/elementwise.o \
./src/tensorflow/lite/micro/kernels/ethosu.o \
./src/tensorflow/lite/micro/kernels/floor.o \
./src/tensorflow/lite/micro/kernels/fully_connected.o \
./src/tensorflow/lite/micro/kernels/hard_swish.o \
./src/tensorflow/lite/micro/kernels/kernel_runner.o \
./src/tensorflow/lite/micro/kernels/kernel_util.o \
./src/tensorflow/lite/micro/kernels/l2norm.o \
./src/tensorflow/lite/micro/kernels/logical.o \
./src/tensorflow/lite/micro/kernels/logistic.o \
./src/tensorflow/lite/micro/kernels/maximum_minimum.o \
./src/tensorflow/lite/micro/kernels/mul.o \
./src/tensorflow/lite/micro/kernels/neg.o \
./src/tensorflow/lite/micro/kernels/pack.o \
./src/tensorflow/lite/micro/kernels/pad.o \
./src/tensorflow/lite/micro/kernels/pooling.o \
./src/tensorflow/lite/micro/kernels/prelu.o \
./src/tensorflow/lite/micro/kernels/quantize.o \
./src/tensorflow/lite/micro/kernels/reduce.o \
./src/tensorflow/lite/micro/kernels/reshape.o \
./src/tensorflow/lite/micro/kernels/resize_nearest_neighbor.o \
./src/tensorflow/lite/micro/kernels/round.o \
./src/tensorflow/lite/micro/kernels/shape.o \
./src/tensorflow/lite/micro/kernels/softmax.o \
./src/tensorflow/lite/micro/kernels/split.o \
./src/tensorflow/lite/micro/kernels/split_v.o \
./src/tensorflow/lite/micro/kernels/strided_slice.o \
./src/tensorflow/lite/micro/kernels/sub.o \
./src/tensorflow/lite/micro/kernels/svdf.o \
./src/tensorflow/lite/micro/kernels/tanh.o \
./src/tensorflow/lite/micro/kernels/unpack.o 


# Each subdirectory must supply rules for building sources it contributes
src/tensorflow/lite/micro/kernels/%.o: ../src/tensorflow/lite/micro/kernels/%.cc
	$(file > $@.in,-O3 -ffunction-sections -fdata-sections -fno-common -fsection-anchors -DEEMBCAD -DRX_DSP_INT8 -fno-use-cxa-atexit -fno-threadsafe-statics -fno-rtti -fno-exceptions -fdiagnostics-parseable-fixits -Wstack-usage=100 -g2 -mcpu=rx64m -misa=v2 -mlittle-endian-data -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\Core\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\NN\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\DSP\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\EEMBC" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\third_party\\flatbuffers\\include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\third_party\\gemmlowp" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_bsp" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_config" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\general" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_pincfg" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_MTU1" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_MTU2" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_PORT" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_SCI0" -DCPPAPP -Wa,-adlnh="$(basename $(notdir $<)).lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"  -c "$<" -o "$@")
	@echo 'rx-elf-gcc -O3 -ffunction-sections -fdata-sections -fno-common -fsection-anchors -DEEMBCAD -DRX_DSP_INT8 -fno-use-cxa-atexit -fno-threadsafe-statics -fno-rtti -fno-exceptions -fdiagnostics-parseable-fixits -Wstack-usage=100 -g2 -mcpu=rx64m -misa=v2 -mlittle-endian-data -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\Core\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\NN\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\DSP\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\EEMBC" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\third_party\\flatbuffers\\include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\third_party\\gemmlowp" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_bsp" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_config" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\general" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_pincfg" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_MTU1" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_MTU2" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_PORT" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_SCI0" -DCPPAPP -Wa,-adlnh="$(basename $(notdir $<)).lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"  -c "$<" -o "$@"'

	@rx-elf-gcc @"$@.in"


################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_accumulate_q7_to_q15.c \
../src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_add_q7.c \
../src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_padded_s8.c \
../src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_s8.c \
../src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_1x_s8.c \
../src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_4x_s8.c \
../src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s8.c \
../src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mult_q15.c \
../src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mult_q7.c \
../src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s8.c \
../src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_svdf_s8.c \
../src/CMSIS/NN/Source/NNSupportFunctions/arm_nntables.c \
../src/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_no_shift.c \
../src/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_reordered_no_shift.c \
../src/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_reordered_with_offset.c \
../src/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_with_offset.c 

C_DEPS += \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_accumulate_q7_to_q15.d \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_add_q7.d \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_padded_s8.d \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_s8.d \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_1x_s8.d \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_4x_s8.d \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s8.d \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mult_q15.d \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mult_q7.d \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s8.d \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_svdf_s8.d \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_nntables.d \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_no_shift.d \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_reordered_no_shift.d \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_reordered_with_offset.d \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_with_offset.d 

OBJS += \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_accumulate_q7_to_q15.o \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_add_q7.o \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_padded_s8.o \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_s8.o \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_1x_s8.o \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_4x_s8.o \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s8.o \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mult_q15.o \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mult_q7.o \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s8.o \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_svdf_s8.o \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_nntables.o \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_no_shift.o \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_reordered_no_shift.o \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_reordered_with_offset.o \
./src/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_with_offset.o 


# Each subdirectory must supply rules for building sources it contributes
src/CMSIS/NN/Source/NNSupportFunctions/%.o: ../src/CMSIS/NN/Source/NNSupportFunctions/%.c
	$(file > $@.in,-O3 -ffunction-sections -fdata-sections -fno-common -fsection-anchors -DEEMBCAD -DRX_DSP_INT8 -fno-use-cxa-atexit -fno-threadsafe-statics -fno-rtti -fno-exceptions -fdiagnostics-parseable-fixits -Wstack-usage=100 -g2 -mcpu=rx64m -misa=v2 -mlittle-endian-data -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\Core\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\NN\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\DSP\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\EEMBC" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\third_party\\flatbuffers\\include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\third_party\\gemmlowp" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_bsp" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_config" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\general" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_pincfg" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_MTU1" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_MTU2" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_PORT" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_SCI0" -DCPPAPP -Wa,-adlnh="$(basename $(notdir $<)).lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"  -c "$<" -o "$@")
	@echo 'rx-elf-gcc -O3 -ffunction-sections -fdata-sections -fno-common -fsection-anchors -DEEMBCAD -DRX_DSP_INT8 -fno-use-cxa-atexit -fno-threadsafe-statics -fno-rtti -fno-exceptions -fdiagnostics-parseable-fixits -Wstack-usage=100 -g2 -mcpu=rx64m -misa=v2 -mlittle-endian-data -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\Core\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\NN\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\DSP\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\EEMBC" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\third_party\\flatbuffers\\include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\third_party\\gemmlowp" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_bsp" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_config" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\general" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_pincfg" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_MTU1" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_MTU2" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_PORT" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_SCI0" -DCPPAPP -Wa,-adlnh="$(basename $(notdir $<)).lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"  -c "$<" -o "$@"'

	@rx-elf-gcc @"$@.in"


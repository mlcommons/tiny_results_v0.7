################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_mat_q7_vec_q15.c \
../src/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_mat_q7_vec_q15_opt.c \
../src/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_q15.c \
../src/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_q15_opt.c \
../src/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_q7.c \
../src/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_q7_opt.c \
../src/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_s8.c 

C_DEPS += \
./src/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_mat_q7_vec_q15.d \
./src/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_mat_q7_vec_q15_opt.d \
./src/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_q15.d \
./src/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_q15_opt.d \
./src/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_q7.d \
./src/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_q7_opt.d \
./src/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_s8.d 

OBJS += \
./src/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_mat_q7_vec_q15.o \
./src/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_mat_q7_vec_q15_opt.o \
./src/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_q15.o \
./src/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_q15_opt.o \
./src/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_q7.o \
./src/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_q7_opt.o \
./src/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_s8.o 


# Each subdirectory must supply rules for building sources it contributes
src/CMSIS/NN/Source/FullyConnectedFunctions/%.o: ../src/CMSIS/NN/Source/FullyConnectedFunctions/%.c
	$(file > $@.in,-O3 -ffunction-sections -fdata-sections -fno-common -fsection-anchors -DEEMBCAD -DRX_DSP_INT8 -fno-use-cxa-atexit -fno-threadsafe-statics -fno-rtti -fno-exceptions -fdiagnostics-parseable-fixits -Wstack-usage=100 -g2 -mcpu=rx64m -misa=v2 -mlittle-endian-data -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\Core\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\NN\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\DSP\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\EEMBC" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\third_party\\flatbuffers\\include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\third_party\\gemmlowp" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_bsp" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_config" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\general" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_pincfg" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_MTU1" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_MTU2" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_PORT" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_SCI0" -DCPPAPP -Wa,-adlnh="$(basename $(notdir $<)).lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"  -c "$<" -o "$@")
	@echo 'rx-elf-gcc -O3 -ffunction-sections -fdata-sections -fno-common -fsection-anchors -DEEMBCAD -DRX_DSP_INT8 -fno-use-cxa-atexit -fno-threadsafe-statics -fno-rtti -fno-exceptions -fdiagnostics-parseable-fixits -Wstack-usage=100 -g2 -mcpu=rx64m -misa=v2 -mlittle-endian-data -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\Core\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\NN\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\DSP\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\EEMBC" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\third_party\\flatbuffers\\include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\third_party\\gemmlowp" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_bsp" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_config" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\general" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_pincfg" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_MTU1" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_MTU2" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_PORT" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_SCI0" -DCPPAPP -Wa,-adlnh="$(basename $(notdir $<)).lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"  -c "$<" -o "$@"'

	@rx-elf-gcc @"$@.in"


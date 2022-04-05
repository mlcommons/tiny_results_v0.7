################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/EEMBC/monitor/th_api/th_lib.c \
../src/EEMBC/monitor/th_api/th_libc.c 

C_DEPS += \
./src/EEMBC/monitor/th_api/th_lib.d \
./src/EEMBC/monitor/th_api/th_libc.d 

OBJS += \
./src/EEMBC/monitor/th_api/th_lib.o \
./src/EEMBC/monitor/th_api/th_libc.o 


# Each subdirectory must supply rules for building sources it contributes
src/EEMBC/monitor/th_api/%.o: ../src/EEMBC/monitor/th_api/%.c
	$(file > $@.in,-O3 -ffunction-sections -fdata-sections -fno-common -fsection-anchors -DEEMBCAD -DRX_DSP_INT8 -fno-use-cxa-atexit -fno-threadsafe-statics -fno-rtti -fno-exceptions -fdiagnostics-parseable-fixits -Wstack-usage=100 -g2 -mcpu=rx64m -misa=v2 -mlittle-endian-data -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\Core\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\NN\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\DSP\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\EEMBC" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\third_party\\flatbuffers\\include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\third_party\\gemmlowp" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_bsp" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_config" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\general" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_pincfg" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_MTU1" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_MTU2" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_PORT" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_SCI0" -DCPPAPP -Wa,-adlnh="$(basename $(notdir $<)).lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"  -c "$<" -o "$@")
	@echo 'rx-elf-gcc -O3 -ffunction-sections -fdata-sections -fno-common -fsection-anchors -DEEMBCAD -DRX_DSP_INT8 -fno-use-cxa-atexit -fno-threadsafe-statics -fno-rtti -fno-exceptions -fdiagnostics-parseable-fixits -Wstack-usage=100 -g2 -mcpu=rx64m -misa=v2 -mlittle-endian-data -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\Core\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\NN\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\DSP\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\EEMBC" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\third_party\\flatbuffers\\include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\third_party\\gemmlowp" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_bsp" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_config" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\general" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_pincfg" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_MTU1" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_MTU2" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_PORT" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_SCI0" -DCPPAPP -Wa,-adlnh="$(basename $(notdir $<)).lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"  -c "$<" -o "$@"'

	@rx-elf-gcc @"$@.in"


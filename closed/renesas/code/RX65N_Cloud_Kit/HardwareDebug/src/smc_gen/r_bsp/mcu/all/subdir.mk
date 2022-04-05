################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../src/smc_gen/r_bsp/mcu/all/reset_program.S 

C_SRCS += \
../src/smc_gen/r_bsp/mcu/all/dbsct.c \
../src/smc_gen/r_bsp/mcu/all/lowlvl.c \
../src/smc_gen/r_bsp/mcu/all/lowsrc.c \
../src/smc_gen/r_bsp/mcu/all/mcu_locks.c \
../src/smc_gen/r_bsp/mcu/all/r_bsp_common.c \
../src/smc_gen/r_bsp/mcu/all/r_bsp_cpu.c \
../src/smc_gen/r_bsp/mcu/all/r_bsp_interrupts.c \
../src/smc_gen/r_bsp/mcu/all/r_bsp_locking.c \
../src/smc_gen/r_bsp/mcu/all/r_bsp_mcu_startup.c \
../src/smc_gen/r_bsp/mcu/all/r_bsp_software_interrupt.c \
../src/smc_gen/r_bsp/mcu/all/r_rx_intrinsic_functions.c \
../src/smc_gen/r_bsp/mcu/all/resetprg.c \
../src/smc_gen/r_bsp/mcu/all/sbrk.c 

C_DEPS += \
./src/smc_gen/r_bsp/mcu/all/dbsct.d \
./src/smc_gen/r_bsp/mcu/all/lowlvl.d \
./src/smc_gen/r_bsp/mcu/all/lowsrc.d \
./src/smc_gen/r_bsp/mcu/all/mcu_locks.d \
./src/smc_gen/r_bsp/mcu/all/r_bsp_common.d \
./src/smc_gen/r_bsp/mcu/all/r_bsp_cpu.d \
./src/smc_gen/r_bsp/mcu/all/r_bsp_interrupts.d \
./src/smc_gen/r_bsp/mcu/all/r_bsp_locking.d \
./src/smc_gen/r_bsp/mcu/all/r_bsp_mcu_startup.d \
./src/smc_gen/r_bsp/mcu/all/r_bsp_software_interrupt.d \
./src/smc_gen/r_bsp/mcu/all/r_rx_intrinsic_functions.d \
./src/smc_gen/r_bsp/mcu/all/resetprg.d \
./src/smc_gen/r_bsp/mcu/all/sbrk.d 

OBJS += \
./src/smc_gen/r_bsp/mcu/all/dbsct.o \
./src/smc_gen/r_bsp/mcu/all/lowlvl.o \
./src/smc_gen/r_bsp/mcu/all/lowsrc.o \
./src/smc_gen/r_bsp/mcu/all/mcu_locks.o \
./src/smc_gen/r_bsp/mcu/all/r_bsp_common.o \
./src/smc_gen/r_bsp/mcu/all/r_bsp_cpu.o \
./src/smc_gen/r_bsp/mcu/all/r_bsp_interrupts.o \
./src/smc_gen/r_bsp/mcu/all/r_bsp_locking.o \
./src/smc_gen/r_bsp/mcu/all/r_bsp_mcu_startup.o \
./src/smc_gen/r_bsp/mcu/all/r_bsp_software_interrupt.o \
./src/smc_gen/r_bsp/mcu/all/r_rx_intrinsic_functions.o \
./src/smc_gen/r_bsp/mcu/all/reset_program.o \
./src/smc_gen/r_bsp/mcu/all/resetprg.o \
./src/smc_gen/r_bsp/mcu/all/sbrk.o 

S_UPPER_DEPS += \
./src/smc_gen/r_bsp/mcu/all/reset_program.d 


# Each subdirectory must supply rules for building sources it contributes
src/smc_gen/r_bsp/mcu/all/%.o: ../src/smc_gen/r_bsp/mcu/all/%.c
	$(file > $@.in,-O3 -ffunction-sections -fdata-sections -fno-common -fsection-anchors -DEEMBCAD -DRX_DSP_INT8 -fno-use-cxa-atexit -fno-threadsafe-statics -fno-rtti -fno-exceptions -fdiagnostics-parseable-fixits -Wstack-usage=100 -g2 -mcpu=rx64m -misa=v2 -mlittle-endian-data -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\Core\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\NN\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\DSP\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\EEMBC" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\third_party\\flatbuffers\\include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\third_party\\gemmlowp" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_bsp" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_config" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\general" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_pincfg" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_MTU1" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_MTU2" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_PORT" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_SCI0" -DCPPAPP -Wa,-adlnh="$(basename $(notdir $<)).lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"  -c "$<" -o "$@")
	@echo 'rx-elf-gcc -O3 -ffunction-sections -fdata-sections -fno-common -fsection-anchors -DEEMBCAD -DRX_DSP_INT8 -fno-use-cxa-atexit -fno-threadsafe-statics -fno-rtti -fno-exceptions -fdiagnostics-parseable-fixits -Wstack-usage=100 -g2 -mcpu=rx64m -misa=v2 -mlittle-endian-data -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\Core\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\NN\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\DSP\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\EEMBC" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\third_party\\flatbuffers\\include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\third_party\\gemmlowp" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_bsp" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_config" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\general" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_pincfg" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_MTU1" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_MTU2" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_PORT" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_SCI0" -DCPPAPP -Wa,-adlnh="$(basename $(notdir $<)).lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"  -c "$<" -o "$@"'

	@rx-elf-gcc @"$@.in"
src/smc_gen/r_bsp/mcu/all/%.o: ../src/smc_gen/r_bsp/mcu/all/%.S
	$(file > $@.in,-O3 -ffunction-sections -fdata-sections -fno-common -fsection-anchors -DEEMBCAD -DRX_DSP_INT8 -fno-use-cxa-atexit -fno-threadsafe-statics -fno-rtti -fno-exceptions -fdiagnostics-parseable-fixits -Wstack-usage=100 -g2 -mcpu=rx64m -misa=v2 -mlittle-endian-data -x assembler-with-cpp -Wa,--gdwarf2 -DCPPAPP -I"C:/Users/a5116938/Desktop/e2_studio_workspace/EEMBC_final/RX65N_Cloud_Kit/src" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\Core\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\NN\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\DSP\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\EEMBC" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\third_party\\flatbuffers\\include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\third_party\\gemmlowp" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_MTU1" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_MTU2" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_PORT" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_SCI0" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_bsp\\mcu\\all" -Wa,-adlhn="$(basename $(notdir $<)).lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c "$<" -o "$@")
	@echo 'rx-elf-gcc -O3 -ffunction-sections -fdata-sections -fno-common -fsection-anchors -DEEMBCAD -DRX_DSP_INT8 -fno-use-cxa-atexit -fno-threadsafe-statics -fno-rtti -fno-exceptions -fdiagnostics-parseable-fixits -Wstack-usage=100 -g2 -mcpu=rx64m -misa=v2 -mlittle-endian-data -x assembler-with-cpp -Wa,--gdwarf2 -DCPPAPP -I"C:/Users/a5116938/Desktop/e2_studio_workspace/EEMBC_final/RX65N_Cloud_Kit/src" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\Core\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\NN\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\CMSIS\\DSP\\Include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\EEMBC" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\third_party\\flatbuffers\\include" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\third_party\\gemmlowp" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_MTU1" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_MTU2" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_PORT" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\Config_SCI0" -I"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\src\\smc_gen\\r_bsp\\mcu\\all" -Wa,-adlhn="$(basename $(notdir $<)).lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c "$<" -o "$@"'

	@rx-elf-gcc @"$@.in"


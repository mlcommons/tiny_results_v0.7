################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

-include makefile.init

RM := rm -rf *.lst *.lis *.lpp *.map

# All of the sources participating in the build are defined here
-include sources.mk
-include src/third_party/gemmlowp/subdir.mk
-include src/tensorflow/lite/schema/subdir.mk
-include src/tensorflow/lite/micro/memory_planner/subdir.mk
-include src/tensorflow/lite/micro/kernels/subdir.mk
-include src/tensorflow/lite/micro/examples/subdir.mk
-include src/tensorflow/lite/micro/subdir.mk
-include src/tensorflow/lite/kernels/internal/subdir.mk
-include src/tensorflow/lite/kernels/subdir.mk
-include src/tensorflow/lite/core/api/subdir.mk
-include src/tensorflow/lite/c/subdir.mk
-include src/smc_gen/r_pincfg/subdir.mk
-include src/smc_gen/r_bsp/mcu/rx65n/subdir.mk
-include src/smc_gen/r_bsp/mcu/all/subdir.mk
-include src/smc_gen/r_bsp/board/generic_rx65n/subdir.mk
-include src/smc_gen/general/subdir.mk
-include src/smc_gen/Config_SCI0/subdir.mk
-include src/smc_gen/Config_PORT/subdir.mk
-include src/smc_gen/Config_MTU2/subdir.mk
-include src/smc_gen/Config_MTU1/subdir.mk
-include src/EEMBC/profile/th_api/subdir.mk
-include src/EEMBC/profile/subdir.mk
-include src/EEMBC/monitor/th_api/subdir.mk
-include src/EEMBC/monitor/subdir.mk
-include src/EEMBC/subdir.mk
-include src/CMSIS/NN/Source/SoftmaxFunctions/subdir.mk
-include src/CMSIS/NN/Source/SVDFunctions/subdir.mk
-include src/CMSIS/NN/Source/ReshapeFunctions/subdir.mk
-include src/CMSIS/NN/Source/PoolingFunctions/subdir.mk
-include src/CMSIS/NN/Source/NNSupportFunctions/subdir.mk
-include src/CMSIS/NN/Source/FullyConnectedFunctions/subdir.mk
-include src/CMSIS/NN/Source/ConvolutionFunctions/subdir.mk
-include src/CMSIS/NN/Source/ConcatenationFunctions/subdir.mk
-include src/CMSIS/NN/Source/BasicMathFunctions/subdir.mk
-include src/CMSIS/NN/Source/ActivationFunctions/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_UPPER_DEPS)),)
-include $(CC_UPPER_DEPS)
endif
ifneq ($(strip $(C++_UPPER_DEPS)),)
-include $(C++_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_UPPER_DEPS)),)
-include $(CPP_UPPER_DEPS)
endif
ifneq ($(strip $(FSY_DEPS)),)
-include $(FSY_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(SRC_UPPER_DEPS)),)
-include $(SRC_UPPER_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(PP_UPPER_DEPS)),)
-include $(PP_UPPER_DEPS)
endif
ifneq ($(strip $(P_DEPS)),)
-include $(P_DEPS)
endif
ifneq ($(strip $(JMP_UPPER_DEPS)),)
-include $(JMP_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(SX_UPPER_DEPS)),)
-include $(SX_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_UPPER_DEPS)),)
-include $(CXX_UPPER_DEPS)
endif
ifneq ($(strip $(JMP_DEPS)),)
-include $(JMP_DEPS)
endif
ifneq ($(strip $(P_UPPER_DEPS)),)
-include $(P_UPPER_DEPS)
endif
ifneq ($(strip $(FSY_UPPER_DEPS)),)
-include $(FSY_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(SRC_DEPS)),)
-include $(SRC_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(PP_DEPS)),)
-include $(PP_DEPS)
endif
ifneq ($(strip $(SX_DEPS)),)
-include $(SX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LIBRARY_GENERATOR_OUTPUTTYPE_OUTPUTS += \
libRX65N_Cloud_Kit.a \

OBJCOPY += \
RX65N_Cloud_Kit.mot \

PRINT_SIZE += \
RX65N_Cloud_Kit.siz \


# All Target
all: RX65N_Cloud_Kit.elf secondary-outputs

# Tool invocations
RX65N_Cloud_Kit.elf: $(OBJS) $(USER_OBJS) $(LIBRARY_GENERATOR_OUTPUTTYPE_OUTPUTS) $(LINKER_SCRIPT)
	@	$(file > RX65N_Cloud_Kit.elf.in,-O3 -ffunction-sections -fdata-sections -fno-common -fsection-anchors -DEEMBCAD -DRX_DSP_INT8 -fno-use-cxa-atexit -fno-threadsafe-statics -fno-rtti -fno-exceptions -fdiagnostics-parseable-fixits -Wstack-usage=100 -g2 -mcpu=rx64m -misa=v2 -mlittle-endian-data -o "RX65N_Cloud_Kit.elf" $(OBJS) $(USER_OBJS) $(LIBS) -T "C:/Users/a5116938/Desktop/e2_studio_workspace/EEMBC_final/RX65N_Cloud_Kit/src/linker_script.ld" -L"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\HardwareDebug" -Wl,--start-group -lRX65N_Cloud_Kit -lgcc -lstdc++ -Wl,--end-group -nostartfiles -Wl,-e_PowerON_Reset -Wl,-M=RX65N_Cloud_Kit.map)
	@echo 'rx-elf-g++ -O3 -ffunction-sections -fdata-sections -fno-common -fsection-anchors -DEEMBCAD -DRX_DSP_INT8 -fno-use-cxa-atexit -fno-threadsafe-statics -fno-rtti -fno-exceptions -fdiagnostics-parseable-fixits -Wstack-usage=100 -g2 -mcpu=rx64m -misa=v2 -mlittle-endian-data -o "RX65N_Cloud_Kit.elf" $(OBJS) $(USER_OBJS) $(LIBS) -T "C:/Users/a5116938/Desktop/e2_studio_workspace/EEMBC_final/RX65N_Cloud_Kit/src/linker_script.ld" -L"C:\\Users\\a5116938\\Desktop\\e2_studio_workspace\\EEMBC_final\\RX65N_Cloud_Kit\\HardwareDebug" -Wl,--start-group -lRX65N_Cloud_Kit -lgcc -lstdc++ -Wl,--end-group -nostartfiles -Wl,-e_PowerON_Reset -Wl,-M=RX65N_Cloud_Kit.map'

	rx-elf-g++ @"RX65N_Cloud_Kit.elf.in"
libRX65N_Cloud_Kit.a: 
	rx-elf-libgen  --header-files=all --select-lib=optlib --compiler-options=-O3,-ffunction-sections,-fdata-sections,-fno-common,-fsection-anchors,-DEEMBCAD,-DRX_DSP_INT8,-fno-use-cxa-atexit,-fno-threadsafe-statics,-fno-rtti,-fno-exceptions,-mcpu=rx64m,-misa=v2,-mlittle-endian-data --assembler-options=-mlittle-endian-data -o "libRX65N_Cloud_Kit.a"
RX65N_Cloud_Kit.mot: RX65N_Cloud_Kit.elf
	rx-elf-objcopy "RX65N_Cloud_Kit.elf" -O srec -I elf32-rx-be-ns  "RX65N_Cloud_Kit.mot"
RX65N_Cloud_Kit.siz: RX65N_Cloud_Kit.elf
	rx-elf-size --format=berkeley "RX65N_Cloud_Kit.elf"
# Other Targets
clean:
ifneq ($(strip $(CC_UPPER_DEPS)),)
	-$(file > clean_CC_UPPER_DEPS.tmp, $(CC_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_CC_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_CC_UPPER_DEPS.tmp
endif
ifneq ($(strip $(PRINT_SIZE)),)
	-$(file > clean_PRINT_SIZE.tmp, $(PRINT_SIZE))
	-@ xargs -s 6000 -t -a clean_PRINT_SIZE.tmp $(RM)
	-@$(RM) clean_PRINT_SIZE.tmp
endif
ifneq ($(strip $(C++_UPPER_DEPS)),)
	-$(file > clean_C++_UPPER_DEPS.tmp, $(C++_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_C++_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_C++_UPPER_DEPS.tmp
endif
ifneq ($(strip $(CPP_UPPER_DEPS)),)
	-$(file > clean_CPP_UPPER_DEPS.tmp, $(CPP_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_CPP_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_CPP_UPPER_DEPS.tmp
endif
ifneq ($(strip $(FSY_DEPS)),)
	-$(file > clean_FSY_DEPS.tmp, $(FSY_DEPS))
	-@ xargs -s 6000 -t -a clean_FSY_DEPS.tmp $(RM)
	-@$(RM) clean_FSY_DEPS.tmp
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
	-$(file > clean_C_UPPER_DEPS.tmp, $(C_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_C_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_C_UPPER_DEPS.tmp
endif
ifneq ($(strip $(SRC_UPPER_DEPS)),)
	-$(file > clean_SRC_UPPER_DEPS.tmp, $(SRC_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_SRC_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_SRC_UPPER_DEPS.tmp
endif
ifneq ($(strip $(S_DEPS)),)
	-$(file > clean_S_DEPS.tmp, $(S_DEPS))
	-@ xargs -s 6000 -t -a clean_S_DEPS.tmp $(RM)
	-@$(RM) clean_S_DEPS.tmp
endif
ifneq ($(strip $(PP_UPPER_DEPS)),)
	-$(file > clean_PP_UPPER_DEPS.tmp, $(PP_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_PP_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_PP_UPPER_DEPS.tmp
endif
ifneq ($(strip $(P_DEPS)),)
	-$(file > clean_P_DEPS.tmp, $(P_DEPS))
	-@ xargs -s 6000 -t -a clean_P_DEPS.tmp $(RM)
	-@$(RM) clean_P_DEPS.tmp
endif
ifneq ($(strip $(OBJCOPY)),)
	-$(file > clean_OBJCOPY.tmp, $(OBJCOPY))
	-@ xargs -s 6000 -t -a clean_OBJCOPY.tmp $(RM)
	-@$(RM) clean_OBJCOPY.tmp
endif
ifneq ($(strip $(JMP_UPPER_DEPS)),)
	-$(file > clean_JMP_UPPER_DEPS.tmp, $(JMP_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_JMP_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_JMP_UPPER_DEPS.tmp
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
	-$(file > clean_ASM_UPPER_DEPS.tmp, $(ASM_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_ASM_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_ASM_UPPER_DEPS.tmp
endif
ifneq ($(strip $(CXX_DEPS)),)
	-$(file > clean_CXX_DEPS.tmp, $(CXX_DEPS))
	-@ xargs -s 6000 -t -a clean_CXX_DEPS.tmp $(RM)
	-@$(RM) clean_CXX_DEPS.tmp
endif
ifneq ($(strip $(SX_UPPER_DEPS)),)
	-$(file > clean_SX_UPPER_DEPS.tmp, $(SX_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_SX_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_SX_UPPER_DEPS.tmp
endif
ifneq ($(strip $(LIBRARY_GENERATOR_OUTPUTTYPE_OUTPUTS)),)
	-$(file > clean_LIBRARY_GENERATOR_OUTPUTTYPE_OUTPUTS.tmp, $(LIBRARY_GENERATOR_OUTPUTTYPE_OUTPUTS))
	-@ xargs -s 6000 -t -a clean_LIBRARY_GENERATOR_OUTPUTTYPE_OUTPUTS.tmp $(RM)
	-@$(RM) clean_LIBRARY_GENERATOR_OUTPUTTYPE_OUTPUTS.tmp
endif
ifneq ($(strip $(CXX_UPPER_DEPS)),)
	-$(file > clean_CXX_UPPER_DEPS.tmp, $(CXX_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_CXX_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_CXX_UPPER_DEPS.tmp
endif
ifneq ($(strip $(JMP_DEPS)),)
	-$(file > clean_JMP_DEPS.tmp, $(JMP_DEPS))
	-@ xargs -s 6000 -t -a clean_JMP_DEPS.tmp $(RM)
	-@$(RM) clean_JMP_DEPS.tmp
endif
ifneq ($(strip $(P_UPPER_DEPS)),)
	-$(file > clean_P_UPPER_DEPS.tmp, $(P_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_P_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_P_UPPER_DEPS.tmp
endif
ifneq ($(strip $(FSY_UPPER_DEPS)),)
	-$(file > clean_FSY_UPPER_DEPS.tmp, $(FSY_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_FSY_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_FSY_UPPER_DEPS.tmp
endif
ifneq ($(strip $(C_DEPS)),)
	-$(file > clean_C_DEPS.tmp, $(C_DEPS))
	-@ xargs -s 6000 -t -a clean_C_DEPS.tmp $(RM)
	-@$(RM) clean_C_DEPS.tmp
endif
ifneq ($(strip $(SRC_DEPS)),)
	-$(file > clean_SRC_DEPS.tmp, $(SRC_DEPS))
	-@ xargs -s 6000 -t -a clean_SRC_DEPS.tmp $(RM)
	-@$(RM) clean_SRC_DEPS.tmp
endif
ifneq ($(strip $(CC_DEPS)),)
	-$(file > clean_CC_DEPS.tmp, $(CC_DEPS))
	-@ xargs -s 6000 -t -a clean_CC_DEPS.tmp $(RM)
	-@$(RM) clean_CC_DEPS.tmp
endif
ifneq ($(strip $(PP_DEPS)),)
	-$(file > clean_PP_DEPS.tmp, $(PP_DEPS))
	-@ xargs -s 6000 -t -a clean_PP_DEPS.tmp $(RM)
	-@$(RM) clean_PP_DEPS.tmp
endif
ifneq ($(strip $(SX_DEPS)),)
	-$(file > clean_SX_DEPS.tmp, $(SX_DEPS))
	-@ xargs -s 6000 -t -a clean_SX_DEPS.tmp $(RM)
	-@$(RM) clean_SX_DEPS.tmp
endif
ifneq ($(strip $(C++_DEPS)),)
	-$(file > clean_C++_DEPS.tmp, $(C++_DEPS))
	-@ xargs -s 6000 -t -a clean_C++_DEPS.tmp $(RM)
	-@$(RM) clean_C++_DEPS.tmp
endif
ifneq ($(strip $(OBJS)),)
	-$(file > clean_OBJS.tmp, $(OBJS))
	-@ xargs -s 6000 -t -a clean_OBJS.tmp $(RM)
	-@$(RM) clean_OBJS.tmp
endif
ifneq ($(strip $(ASM_DEPS)),)
	-$(file > clean_ASM_DEPS.tmp, $(ASM_DEPS))
	-@ xargs -s 6000 -t -a clean_ASM_DEPS.tmp $(RM)
	-@$(RM) clean_ASM_DEPS.tmp
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
	-$(file > clean_S_UPPER_DEPS.tmp, $(S_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_S_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_S_UPPER_DEPS.tmp
endif
ifneq ($(strip $(CPP_DEPS)),)
	-$(file > clean_CPP_DEPS.tmp, $(CPP_DEPS))
	-@ xargs -s 6000 -t -a clean_CPP_DEPS.tmp $(RM)
	-@$(RM) clean_CPP_DEPS.tmp
endif
	-$(RM)  RX65N_Cloud_Kit.elf
	-@echo ' '

secondary-outputs: $(OBJCOPY) $(PRINT_SIZE)

.PHONY: all clean dependents  $(LIBRARY_GENERATOR_OUTPUTTYPE_OUTPUTS)

-include ../makefile.targets

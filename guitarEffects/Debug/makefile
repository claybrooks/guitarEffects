################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := E:/ti/ccsv6/tools/compiler/c2000_6.2.7

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./DSP2833x_headers/src/Xintf.obj" \
"./DSP2833x_headers/src/Watchdog.obj" \
"./DSP2833x_headers/src/SysCtrl.obj" \
"./DSP2833x_headers/src/SetDBGIER.obj" \
"./DSP2833x_headers/src/RFFT_f32_sincostable.obj" \
"./DSP2833x_headers/src/RFFT_f32_mag.obj" \
"./DSP2833x_headers/src/RFFT_adc_f32u.obj" \
"./DSP2833x_headers/src/RFFT_adc_f32.obj" \
"./DSP2833x_headers/src/PieVect_nonBIOS.obj" \
"./DSP2833x_headers/src/PieCtrl_nonBIOS.obj" \
"./DSP2833x_headers/src/Passwords.obj" \
"./DSP2833x_headers/src/Gpio.obj" \
"./DSP2833x_headers/src/Flash.obj" \
"./DSP2833x_headers/src/DelayUs.obj" \
"./DSP2833x_headers/src/DefaultIsr_nonBIOS.obj" \
"./DSP2833x_headers/src/DSP2833x_Spi.obj" \
"./DSP2833x_headers/src/DSP2833x_Mcbsp.obj" \
"./DSP2833x_headers/src/DSP2833x_I2C.obj" \
"./DSP2833x_headers/src/DSP2833x_GlobalVariableDefs.obj" \
"./DSP2833x_headers/src/DSP2833x_EPwm.obj" \
"./DSP2833x_headers/src/DSP2833x_CpuTimers.obj" \
"./DSP2833x_headers/src/DSP2833x_Adc.obj" \
"./DSP2833x_headers/src/DSP2833x_ADC_cal.obj" \
"./DSP2833x_headers/src/CodeStartBranch.obj" \
"./src/sys.obj" \
"./src/spi.obj" \
"./src/main.obj" \
"./src/lowpass.obj" \
"./src/lcd.obj" \
"./src/fft.obj" \
"./src/effect.obj" \
"./src/eeprom.obj" \
"./src/bp_iir.obj" \
"./src/autoWah.obj" \
"./src/adc.obj" \
"../cmd/F28335_nonBIOS_flash.cmd" \
"../DSP2833x_headers/cmd/DSP2833x_Headers_nonBIOS.cmd" \
-l"libc.a" \
-l"E:\ti\controlSUITE\libs\dsp\FPU\v130\lib\C28x_FPU_Lib.lib" \
-l"E:\ti\ccsv6\tools\compiler\c2000_6.2.7\lib\rts2800_fpu32.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir_vars.mk
-include cmd/subdir_vars.mk
-include DSP2833x_headers/src/subdir_vars.mk
-include DSP2833x_headers/include/subdir_vars.mk
-include DSP2833x_headers/cmd/subdir_vars.mk
-include src/subdir_rules.mk
-include cmd/subdir_rules.mk
-include DSP2833x_headers/src/subdir_rules.mk
-include DSP2833x_headers/include/subdir_rules.mk
-include DSP2833x_headers/cmd/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
guitarEffects.out \

EXE_OUTPUTS__QUOTED += \
"guitarEffects.out" \

BIN_OUTPUTS += \
guitarEffects.hex \

BIN_OUTPUTS__QUOTED += \
"guitarEffects.hex" \


# All Target
all: guitarEffects.out

# Tool invocations
guitarEffects.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: C2000 Linker'
	"E:/ti/ccsv6/tools/compiler/c2000_6.2.7/bin/cl2000" -v28 -ml -mt --float_support=fpu32 -Ooff -g --define=EXAMPLE_NONBIOS --define=EXAMPLE_FLASH --diag_warning=225 --display_error_number --diag_wrap=off -z -m"guitarEffects.map" --stack_size=0x300 --warn_sections -i"E:/ti/ccsv6/tools/compiler/c2000_6.2.7/lib" -i"E:/ti/ccsv6/tools/compiler/c2000_6.2.7/include" --reread_libs --display_error_number --diag_wrap=off --xml_link_info="guitarEffects_linkInfo.xml" --rom_model -o "guitarEffects.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

guitarEffects.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: C2000 Hex Utility'
	"E:/ti/ccsv6/tools/compiler/c2000_6.2.7/bin/hex2000"  -o "guitarEffects.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)$(BIN_OUTPUTS__QUOTED)
	-$(RM) "src\adc.pp" "src\autoWah.pp" "src\bp_iir.pp" "src\eeprom.pp" "src\effect.pp" "src\fft.pp" "src\lcd.pp" "src\lowpass.pp" "src\main.pp" "src\spi.pp" "src\sys.pp" "DSP2833x_headers\src\DSP2833x_Adc.pp" "DSP2833x_headers\src\DSP2833x_CpuTimers.pp" "DSP2833x_headers\src\DSP2833x_EPwm.pp" "DSP2833x_headers\src\DSP2833x_GlobalVariableDefs.pp" "DSP2833x_headers\src\DSP2833x_I2C.pp" "DSP2833x_headers\src\DSP2833x_Mcbsp.pp" "DSP2833x_headers\src\DSP2833x_Spi.pp" "DSP2833x_headers\src\DefaultIsr_nonBIOS.pp" "DSP2833x_headers\src\Flash.pp" "DSP2833x_headers\src\Gpio.pp" "DSP2833x_headers\src\PieCtrl_nonBIOS.pp" "DSP2833x_headers\src\PieVect_nonBIOS.pp" "DSP2833x_headers\src\RFFT_f32_sincostable.pp" "DSP2833x_headers\src\SysCtrl.pp" "DSP2833x_headers\src\Watchdog.pp" "DSP2833x_headers\src\Xintf.pp" 
	-$(RM) "src\adc.obj" "src\autoWah.obj" "src\bp_iir.obj" "src\eeprom.obj" "src\effect.obj" "src\fft.obj" "src\lcd.obj" "src\lowpass.obj" "src\main.obj" "src\spi.obj" "src\sys.obj" "DSP2833x_headers\src\CodeStartBranch.obj" "DSP2833x_headers\src\DSP2833x_ADC_cal.obj" "DSP2833x_headers\src\DSP2833x_Adc.obj" "DSP2833x_headers\src\DSP2833x_CpuTimers.obj" "DSP2833x_headers\src\DSP2833x_EPwm.obj" "DSP2833x_headers\src\DSP2833x_GlobalVariableDefs.obj" "DSP2833x_headers\src\DSP2833x_I2C.obj" "DSP2833x_headers\src\DSP2833x_Mcbsp.obj" "DSP2833x_headers\src\DSP2833x_Spi.obj" "DSP2833x_headers\src\DefaultIsr_nonBIOS.obj" "DSP2833x_headers\src\DelayUs.obj" "DSP2833x_headers\src\Flash.obj" "DSP2833x_headers\src\Gpio.obj" "DSP2833x_headers\src\Passwords.obj" "DSP2833x_headers\src\PieCtrl_nonBIOS.obj" "DSP2833x_headers\src\PieVect_nonBIOS.obj" "DSP2833x_headers\src\RFFT_adc_f32.obj" "DSP2833x_headers\src\RFFT_adc_f32u.obj" "DSP2833x_headers\src\RFFT_f32_mag.obj" "DSP2833x_headers\src\RFFT_f32_sincostable.obj" 
	-$(RM) "DSP2833x_headers\src\SetDBGIER.obj" "DSP2833x_headers\src\SysCtrl.obj" "DSP2833x_headers\src\Watchdog.obj" "DSP2833x_headers\src\Xintf.obj" 
	-$(RM) "DSP2833x_headers\src\CodeStartBranch.pp" "DSP2833x_headers\src\DSP2833x_ADC_cal.pp" "DSP2833x_headers\src\DelayUs.pp" "DSP2833x_headers\src\Passwords.pp" "DSP2833x_headers\src\RFFT_adc_f32.pp" "DSP2833x_headers\src\RFFT_adc_f32u.pp" "DSP2833x_headers\src\RFFT_f32_mag.pp" "DSP2833x_headers\src\SetDBGIER.pp" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets


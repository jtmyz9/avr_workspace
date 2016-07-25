################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include common_libs/systimer/subdir.mk
-include common_libs/i2c/subdir.mk
-include common_libs/fat/subdir.mk
-include common_libs/can/subdir.mk
-include common_libs/ZOUnet/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
FLASH_IMAGE += \
datalogger_old.hex \

SIZEDUMMY += \
sizedummy \


# All Target
all: datalogger_old.elf secondary-outputs

# Tool invocations
datalogger_old.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,datalogger_old.map -mmcu=at90can128 -o "datalogger_old.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

datalogger_old.hex: datalogger_old.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex datalogger_old.elf  "datalogger_old.hex"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: datalogger_old.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=at90can128 datalogger_old.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(FLASH_IMAGE)$(ELFS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) datalogger_old.elf
	-@echo ' '

secondary-outputs: $(FLASH_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

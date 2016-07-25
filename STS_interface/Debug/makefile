################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
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
LSS += \
STS_interface.lss \

FLASH_IMAGE += \
STS_interface.hex \

SIZEDUMMY += \
sizedummy \


# All Target
all: STS_interface.elf secondary-outputs

# Tool invocations
STS_interface.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,STS_interface.map -mmcu=atmega644p -o "STS_interface.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

STS_interface.lss: STS_interface.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S STS_interface.elf  >"STS_interface.lss"
	@echo 'Finished building: $@'
	@echo ' '

STS_interface.hex: STS_interface.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex STS_interface.elf  "STS_interface.hex"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: STS_interface.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega644p STS_interface.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) STS_interface.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

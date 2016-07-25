################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include fat/subdir.mk
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
DLGR.lss \

FLASH_IMAGE += \
DLGR.hex \

SIZEDUMMY += \
sizedummy \


# All Target
all: DLGR.elf secondary-outputs

# Tool invocations
DLGR.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,DLGR.map -mmcu=at90can128 -o "DLGR.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

DLGR.lss: DLGR.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S DLGR.elf  >"DLGR.lss"
	@echo 'Finished building: $@'
	@echo ' '

DLGR.hex: DLGR.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex DLGR.elf  "DLGR.hex"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: DLGR.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=at90can128 DLGR.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) DLGR.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

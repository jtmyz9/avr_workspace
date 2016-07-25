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
test_i2c.lss \

FLASH_IMAGE += \
test_i2c.hex \

SIZEDUMMY += \
sizedummy \


# All Target
all: test_i2c.elf secondary-outputs

# Tool invocations
test_i2c.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,test_i2c.map -mmcu=at90can128 -o "test_i2c.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

test_i2c.lss: test_i2c.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S test_i2c.elf  >"test_i2c.lss"
	@echo 'Finished building: $@'
	@echo ' '

test_i2c.hex: test_i2c.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex test_i2c.elf  "test_i2c.hex"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: test_i2c.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=at90can128 test_i2c.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) test_i2c.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../EEPROM_routines.c \
../RTC_EEPROM_main.c \
../RTC_routines.c \
../UART_routines.c \
../i2c_routines.c 

OBJS += \
./EEPROM_routines.o \
./RTC_EEPROM_main.o \
./RTC_routines.o \
./UART_routines.o \
./i2c_routines.o 

C_DEPS += \
./EEPROM_routines.d \
./RTC_EEPROM_main.d \
./RTC_routines.d \
./UART_routines.d \
./i2c_routines.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"C:\WinAVR-20100110\avr\include" -I"C:\WinAVR-20100110\lib\gcc\avr\4.3.3\include" -I"C:\WinAVR-20100110\lib\gcc\avr\4.3.3\include-fixed" -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=at90can128 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



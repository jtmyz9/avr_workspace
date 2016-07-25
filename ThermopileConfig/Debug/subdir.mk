################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ThermopileConfig.c \
../mlx90614.c \
../twimaster.c \
../uart.c \
../uart_extended.c 

OBJS += \
./ThermopileConfig.o \
./mlx90614.o \
./twimaster.o \
./uart.o \
./uart_extended.o 

C_DEPS += \
./ThermopileConfig.d \
./mlx90614.d \
./twimaster.d \
./uart.d \
./uart_extended.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega644p -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



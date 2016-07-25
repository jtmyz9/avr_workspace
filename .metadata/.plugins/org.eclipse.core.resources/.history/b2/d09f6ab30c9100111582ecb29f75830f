################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../fat/byteordering.c \
../fat/fat.c \
../fat/partition.c \
../fat/sd_raw.c \
../fat/uart.c 

OBJS += \
./fat/byteordering.o \
./fat/fat.o \
./fat/partition.o \
./fat/sd_raw.o \
./fat/uart.o 

C_DEPS += \
./fat/byteordering.d \
./fat/fat.d \
./fat/partition.d \
./fat/sd_raw.d \
./fat/uart.d 


# Each subdirectory must supply rules for building sources it contributes
fat/%.o: ../fat/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"C:\WinAVR-20100110\lib\gcc\avr\4.3.3\include" -I"C:\Users\Mark\Dropbox\Electronics\Software\avr_workspace\libs\include" -I"C:\WinAVR-20100110\avr\include" -I"C:\WinAVR-20100110\lib\gcc\avr\4.3.3\include-fixed" -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=at90can128 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



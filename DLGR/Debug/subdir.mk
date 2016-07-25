################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/Mark/Dropbox/Electronics/Software/avr_workspace/libs/can.c \
../datalogger.c \
../gps.c \
../log.c \
C:/Users/Mark/Dropbox/Electronics/Software/avr_workspace/libs/systimer.c 

OBJS += \
./can.o \
./datalogger.o \
./gps.o \
./log.o \
./systimer.o 

C_DEPS += \
./can.d \
./datalogger.d \
./gps.d \
./log.d \
./systimer.d 


# Each subdirectory must supply rules for building sources it contributes
can.o: C:/Users/Mark/Dropbox/Electronics/Software/avr_workspace/libs/can.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"C:\WinAVR-20100110\lib\gcc\avr\4.3.3\include" -I"C:\Users\Mark\Dropbox\Electronics\Software\avr_workspace\libs\include" -I"C:\WinAVR-20100110\avr\include" -I"C:\WinAVR-20100110\lib\gcc\avr\4.3.3\include-fixed" -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=at90can128 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"C:\WinAVR-20100110\lib\gcc\avr\4.3.3\include" -I"C:\Users\Mark\Dropbox\Electronics\Software\avr_workspace\libs\include" -I"C:\WinAVR-20100110\avr\include" -I"C:\WinAVR-20100110\lib\gcc\avr\4.3.3\include-fixed" -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=at90can128 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

systimer.o: C:/Users/Mark/Dropbox/Electronics/Software/avr_workspace/libs/systimer.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"C:\WinAVR-20100110\lib\gcc\avr\4.3.3\include" -I"C:\Users\Mark\Dropbox\Electronics\Software\avr_workspace\libs\include" -I"C:\WinAVR-20100110\avr\include" -I"C:\WinAVR-20100110\lib\gcc\avr\4.3.3\include-fixed" -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=at90can128 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



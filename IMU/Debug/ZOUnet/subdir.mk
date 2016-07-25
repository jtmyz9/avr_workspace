################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/Administrator/Dropbox/Electronics/Software/avr_workspace/common_libs/ZOUnet/ZOUnet.c 

OBJS += \
./ZOUnet/ZOUnet.o 

C_DEPS += \
./ZOUnet/ZOUnet.d 


# Each subdirectory must supply rules for building sources it contributes
ZOUnet/ZOUnet.o: C:/Users/Administrator/Dropbox/Electronics/Software/avr_workspace/common_libs/ZOUnet/ZOUnet.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"C:\Users\Administrator\Dropbox\Electronics\Software\avr_workspace\common_libs" -I"C:\WinAVR-20100110\avr\include" -I"C:\WinAVR-20100110\lib\gcc\avr\4.3.3\include" -I"C:\WinAVR-20100110\lib\gcc\avr\4.3.3\include-fixed" -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=at90can128 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



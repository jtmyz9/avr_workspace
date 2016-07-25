################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/Mark/Dropbox/Electronics/Software/avr_workspace/common_libs/fat/byteordering.c \
C:/Users/Mark/Dropbox/Electronics/Software/avr_workspace/common_libs/fat/fat.c \
C:/Users/Mark/Dropbox/Electronics/Software/avr_workspace/common_libs/fat/partition.c \
C:/Users/Mark/Dropbox/Electronics/Software/avr_workspace/common_libs/fat/sd_raw.c \
C:/Users/Mark/Dropbox/Electronics/Software/avr_workspace/common_libs/fat/uart.c 

OBJS += \
./common_libs/fat/byteordering.o \
./common_libs/fat/fat.o \
./common_libs/fat/partition.o \
./common_libs/fat/sd_raw.o \
./common_libs/fat/uart.o 

C_DEPS += \
./common_libs/fat/byteordering.d \
./common_libs/fat/fat.d \
./common_libs/fat/partition.d \
./common_libs/fat/sd_raw.d \
./common_libs/fat/uart.d 


# Each subdirectory must supply rules for building sources it contributes
common_libs/fat/byteordering.o: C:/Users/Mark/Dropbox/Electronics/Software/avr_workspace/common_libs/fat/byteordering.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -IC:/WinAVR-20100110/avr/include -I"C:\Users\Mark\Dropbox\Electronics\Software\avr_workspace\common_libs\include" -IC:/WinAVR-20100110/lib/gcc/avr/4.3.3/include -IC:/WinAVR-20100110/lib/gcc/avr/4.3.3/include-fixed -IC:/Users/Administrator/Dropbox/Electronics/Software/avr_workspace/common_libs/include -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=at90can128 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

common_libs/fat/fat.o: C:/Users/Mark/Dropbox/Electronics/Software/avr_workspace/common_libs/fat/fat.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -IC:/WinAVR-20100110/avr/include -I"C:\Users\Mark\Dropbox\Electronics\Software\avr_workspace\common_libs\include" -IC:/WinAVR-20100110/lib/gcc/avr/4.3.3/include -IC:/WinAVR-20100110/lib/gcc/avr/4.3.3/include-fixed -IC:/Users/Administrator/Dropbox/Electronics/Software/avr_workspace/common_libs/include -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=at90can128 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

common_libs/fat/partition.o: C:/Users/Mark/Dropbox/Electronics/Software/avr_workspace/common_libs/fat/partition.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -IC:/WinAVR-20100110/avr/include -I"C:\Users\Mark\Dropbox\Electronics\Software\avr_workspace\common_libs\include" -IC:/WinAVR-20100110/lib/gcc/avr/4.3.3/include -IC:/WinAVR-20100110/lib/gcc/avr/4.3.3/include-fixed -IC:/Users/Administrator/Dropbox/Electronics/Software/avr_workspace/common_libs/include -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=at90can128 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

common_libs/fat/sd_raw.o: C:/Users/Mark/Dropbox/Electronics/Software/avr_workspace/common_libs/fat/sd_raw.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -IC:/WinAVR-20100110/avr/include -I"C:\Users\Mark\Dropbox\Electronics\Software\avr_workspace\common_libs\include" -IC:/WinAVR-20100110/lib/gcc/avr/4.3.3/include -IC:/WinAVR-20100110/lib/gcc/avr/4.3.3/include-fixed -IC:/Users/Administrator/Dropbox/Electronics/Software/avr_workspace/common_libs/include -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=at90can128 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

common_libs/fat/uart.o: C:/Users/Mark/Dropbox/Electronics/Software/avr_workspace/common_libs/fat/uart.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -IC:/WinAVR-20100110/avr/include -I"C:\Users\Mark\Dropbox\Electronics\Software\avr_workspace\common_libs\include" -IC:/WinAVR-20100110/lib/gcc/avr/4.3.3/include -IC:/WinAVR-20100110/lib/gcc/avr/4.3.3/include-fixed -IC:/Users/Administrator/Dropbox/Electronics/Software/avr_workspace/common_libs/include -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=at90can128 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



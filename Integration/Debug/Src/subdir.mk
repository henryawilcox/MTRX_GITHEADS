################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/digital_io.c \
../Src/main.c \
../Src/serial.c \
../Src/serial_interrupt.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/timer.c 

OBJS += \
./Src/digital_io.o \
./Src/main.o \
./Src/serial.o \
./Src/serial_interrupt.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/timer.o 

C_DEPS += \
./Src/digital_io.d \
./Src/main.d \
./Src/serial.d \
./Src/serial_interrupt.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/timer.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F303VCTx -DSTM32 -DSTM32F3 -DSTM32F3DISCOVERY -c -I../Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/digital_io.cyclo ./Src/digital_io.d ./Src/digital_io.o ./Src/digital_io.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/serial.cyclo ./Src/serial.d ./Src/serial.o ./Src/serial.su ./Src/serial_interrupt.cyclo ./Src/serial_interrupt.d ./Src/serial_interrupt.o ./Src/serial_interrupt.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/timer.cyclo ./Src/timer.d ./Src/timer.o ./Src/timer.su

.PHONY: clean-Src


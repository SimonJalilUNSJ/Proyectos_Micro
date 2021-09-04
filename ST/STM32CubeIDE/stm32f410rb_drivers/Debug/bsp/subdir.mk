################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../bsp/ds1307.c \
../bsp/lcd.c 

OBJS += \
./bsp/ds1307.o \
./bsp/lcd.o 

C_DEPS += \
./bsp/ds1307.d \
./bsp/lcd.d 


# Each subdirectory must supply rules for building sources it contributes
bsp/ds1307.o: ../bsp/ds1307.c bsp/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F410RBTx -DSTM32 -DNUCLEO_F410RB -DSTM32F4 -c -I../Inc -I"/home/simon/Documentos/FACULTAD_linux/Microcontroladores/Curso-ST/My_workspace/target/stm32f410rb_drivers/bsp" -I"/home/simon/Documentos/FACULTAD_linux/Microcontroladores/Curso-ST/My_workspace/target/stm32f410rb_drivers/drivers/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"bsp/ds1307.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
bsp/lcd.o: ../bsp/lcd.c bsp/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F410RBTx -DSTM32 -DNUCLEO_F410RB -DSTM32F4 -c -I../Inc -I"/home/simon/Documentos/FACULTAD_linux/Microcontroladores/Curso-ST/My_workspace/target/stm32f410rb_drivers/bsp" -I"/home/simon/Documentos/FACULTAD_linux/Microcontroladores/Curso-ST/My_workspace/target/stm32f410rb_drivers/drivers/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"bsp/lcd.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"


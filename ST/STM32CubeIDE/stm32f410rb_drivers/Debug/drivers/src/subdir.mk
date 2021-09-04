################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/src/stm32f410rb_gpio_driver.c \
../drivers/src/stm32f410rb_i2c_driver.c \
../drivers/src/stm32f410rb_rcc_driver.c \
../drivers/src/stm32f410rb_spi_driver.c \
../drivers/src/stm32f410rb_usart_driver.c 

OBJS += \
./drivers/src/stm32f410rb_gpio_driver.o \
./drivers/src/stm32f410rb_i2c_driver.o \
./drivers/src/stm32f410rb_rcc_driver.o \
./drivers/src/stm32f410rb_spi_driver.o \
./drivers/src/stm32f410rb_usart_driver.o 

C_DEPS += \
./drivers/src/stm32f410rb_gpio_driver.d \
./drivers/src/stm32f410rb_i2c_driver.d \
./drivers/src/stm32f410rb_rcc_driver.d \
./drivers/src/stm32f410rb_spi_driver.d \
./drivers/src/stm32f410rb_usart_driver.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/src/stm32f410rb_gpio_driver.o: ../drivers/src/stm32f410rb_gpio_driver.c drivers/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F410RBTx -DSTM32 -DNUCLEO_F410RB -DSTM32F4 -c -I../Inc -I"/home/simon/Documentos/FACULTAD_linux/Microcontroladores/Curso-ST/My_workspace/target/stm32f410rb_drivers/bsp" -I"/home/simon/Documentos/FACULTAD_linux/Microcontroladores/Curso-ST/My_workspace/target/stm32f410rb_drivers/drivers/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"drivers/src/stm32f410rb_gpio_driver.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
drivers/src/stm32f410rb_i2c_driver.o: ../drivers/src/stm32f410rb_i2c_driver.c drivers/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F410RBTx -DSTM32 -DNUCLEO_F410RB -DSTM32F4 -c -I../Inc -I"/home/simon/Documentos/FACULTAD_linux/Microcontroladores/Curso-ST/My_workspace/target/stm32f410rb_drivers/bsp" -I"/home/simon/Documentos/FACULTAD_linux/Microcontroladores/Curso-ST/My_workspace/target/stm32f410rb_drivers/drivers/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"drivers/src/stm32f410rb_i2c_driver.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
drivers/src/stm32f410rb_rcc_driver.o: ../drivers/src/stm32f410rb_rcc_driver.c drivers/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F410RBTx -DSTM32 -DNUCLEO_F410RB -DSTM32F4 -c -I../Inc -I"/home/simon/Documentos/FACULTAD_linux/Microcontroladores/Curso-ST/My_workspace/target/stm32f410rb_drivers/bsp" -I"/home/simon/Documentos/FACULTAD_linux/Microcontroladores/Curso-ST/My_workspace/target/stm32f410rb_drivers/drivers/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"drivers/src/stm32f410rb_rcc_driver.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
drivers/src/stm32f410rb_spi_driver.o: ../drivers/src/stm32f410rb_spi_driver.c drivers/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F410RBTx -DSTM32 -DNUCLEO_F410RB -DSTM32F4 -c -I../Inc -I"/home/simon/Documentos/FACULTAD_linux/Microcontroladores/Curso-ST/My_workspace/target/stm32f410rb_drivers/bsp" -I"/home/simon/Documentos/FACULTAD_linux/Microcontroladores/Curso-ST/My_workspace/target/stm32f410rb_drivers/drivers/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"drivers/src/stm32f410rb_spi_driver.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
drivers/src/stm32f410rb_usart_driver.o: ../drivers/src/stm32f410rb_usart_driver.c drivers/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F410RBTx -DSTM32 -DNUCLEO_F410RB -DSTM32F4 -c -I../Inc -I"/home/simon/Documentos/FACULTAD_linux/Microcontroladores/Curso-ST/My_workspace/target/stm32f410rb_drivers/bsp" -I"/home/simon/Documentos/FACULTAD_linux/Microcontroladores/Curso-ST/My_workspace/target/stm32f410rb_drivers/drivers/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"drivers/src/stm32f410rb_usart_driver.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"


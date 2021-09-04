################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/data.c \
../Core/Src/it.c \
../Core/Src/main.c \
../Core/Src/msp.c \
../Core/Src/syscalls.c \
../Core/Src/system_stm32f4xx.c 

OBJS += \
./Core/Src/data.o \
./Core/Src/it.o \
./Core/Src/main.o \
./Core/Src/msp.o \
./Core/Src/syscalls.o \
./Core/Src/system_stm32f4xx.o 

C_DEPS += \
./Core/Src/data.d \
./Core/Src/it.d \
./Core/Src/main.d \
./Core/Src/msp.d \
./Core/Src/syscalls.d \
./Core/Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DUSE_HAL_DRIVER -DSTM32F410Rx -I"/home/simon/Documentos/FACULTAD_linux/Microcontroladores/Curso-ST/My_workspace/Workspace_SW/WFE_button/Core/Inc" -I"/home/simon/Documentos/FACULTAD_linux/Microcontroladores/Curso-ST/My_workspace/Workspace_SW/WFE_button/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/home/simon/Documentos/FACULTAD_linux/Microcontroladores/Curso-ST/My_workspace/Workspace_SW/WFE_button/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/home/simon/Documentos/FACULTAD_linux/Microcontroladores/Curso-ST/My_workspace/Workspace_SW/WFE_button/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/home/simon/Documentos/FACULTAD_linux/Microcontroladores/Curso-ST/My_workspace/Workspace_SW/WFE_button/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



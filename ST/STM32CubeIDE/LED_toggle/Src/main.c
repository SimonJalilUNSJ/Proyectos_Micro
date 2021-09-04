/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

#include <stdint.h>

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

int main(void)
{
	uint32_t *pClkCtrlReg = (uint32_t*) 0x40023830;
	uint32_t *pPortCModeReg = (uint32_t*) 0x40020800;
	uint32_t *pPortCOutReg = (uint32_t*) 0x40020814;

	//1) Habilitar el clock para periferico GPIOC en el AHB1ENR
	*pClkCtrlReg |= 0x08;

	//2) Configurar el modo del pin IO como salida
	//a. Limpiar los bit de la posicion 24 y 25
	*pPortCModeReg &= 0xFCFFFFFF;
	//b. Setear la posicion 24 a 1 (HIGH)
	*pPortCModeReg |= 0x01000000;

	while(1){
		//3) Setear el bit 12 del registro de dato de salida para hacer I/O pin-12 como salida
		*pPortCOutReg |= 0x1000;

		//Introducimos un pequeño delay 10k times
		for (uint32_t i = 0; i < 10000; i++);

		//Apagar el led
		*pPortCOutReg &= ~ (1 << 12);

		//Introducimos un pequeño delay 10k times
		for (uint32_t i = 0; i < 10000; i++);
	}
}

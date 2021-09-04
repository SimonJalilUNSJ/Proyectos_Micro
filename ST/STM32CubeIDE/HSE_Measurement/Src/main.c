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

#define RCC_BASE_ADDR		0x40023800UL
#define RCC_CR_REG_OFFSET	0x00UL
#define RCC_CFGR_REG_OFFSET	0x08UL
#define RCC_CR_REG_ADDR		(RCC_BASE_ADDR + RCC_CR_REG_OFFSET)
#define RCC_CFGR_REG_ADDR	(RCC_BASE_ADDR + RCC_CFGR_REG_OFFSET)
#define GPIOA_BASE_ADDR		0x40020000UL

int main(void)
{
	uint32_t *pRccCrReg 	= (uint32_t*)RCC_CR_REG_ADDR;
	uint32_t *pRccCfgrReg 	= (uint32_t*)RCC_CFGR_REG_ADDR;
	//1. Habilitar el clock HSE usando el bit HSEON (RCC_CR)
	*pRccCrReg |= (1 << 16);
	//2. Esperar hasta que el reloj HSE desde el cristal externo se estabilice.
	//Hay una bandera que me indica la estabilidad del cristal
	while(!(*pRccCrReg & (1 << 17)));

	//3. Cambiar el sistema de reloj a HSE(RCC_CFGR).
	*pRccCfgrReg |= (1 << 0);

	//***4. Hacer las configuraciones para medirlo.***
	//1. Configurar el registro RCC_CFGR
	*pRccCfgrReg |= (1 << 22); //Setear bit 22

	//Configurar el prescaler de MCO1 para dividirlo en 4
	*pRccCfgrReg |= (1 << 25);
	*pRccCfgrReg |= (1 << 26);

	//2. Configurar PA8 a modo AF0 para comportarse como señal MCO1
	//a. Habilitar el reloj periférico para el periférico GPIOA
	uint32_t *pRCCAhb1Enr = (uint32_t*)(RCC_BASE_ADDR + 0x30);
	*pRCCAhb1Enr |= (1 << 0);	//Habilito el reloj periférico GPIOA

	//b. Configurar el modo del pin 8 GPIOA como un modo de funcion alternativa.
	uint32_t *pGPIOAModeReg = (uint32_t*)(GPIOA_BASE_ADDR + 0x00);
	*pGPIOAModeReg &= ~(0x3 << 16); //Limpiar
	*pGPIOAModeReg |= (0x2 << 16);  //Setear

	//c. Configurar el registro de función alternativa para setear el modo 0 para PA8
	uint32_t *pGIOAAltFunHighReg = (uint32_t*)(GPIOA_BASE_ADDR + 0x24);
	*pGIOAAltFunHighReg &= ~(0xF << 0);


	/* Loop forever */
	for(;;);
}
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
#include <stdio.h>
#define USART3_IRQNO 39

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

int main(void)
{
	//1. Manualmente colocar el bit pendiente del USART3 del numero IRQ en NVIC.
	uint32_t *pISPR1 = (uint32_t*) 0XE000E200;
	*pISPR1 = (1 << (USART3_IRQNO % 32));

	//2. Habilitar el USART3 numero IRQ	en NVIC
	uint32_t *pISER1 = (uint32_t*)0xE000E100;
	*pISER1 |= (1 << (USART3_IRQNO % 32));



    /* Loop forever */
	for(;;);
}

//USART3 ISR
void USART3_IRQHandler(void){
 printf("En USART3 ISR\n");
}

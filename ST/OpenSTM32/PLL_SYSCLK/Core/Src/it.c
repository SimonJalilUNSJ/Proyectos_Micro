/*
 * it.c
 *
 *  Created on: 21 ago. 2021
 *      Author: simon
 */
#include "main.h"

void SysTick_Handler(){
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

/*
 * it.c
 *
 *  Created on: 21 ago. 2021
 *      Author: simon
 */
#include "main.h"

extern TIM_HandleTypeDef htimer5;

void SysTick_Handler(void){
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void TIM5_IRQHandler(void){
	HAL_TIM_IRQHandler(&htimer5);
}

/*
 * it.c
 *
 *  Created on: 21 ago. 2021
 *      Author: simon
 */
#include "main.h"

extern TIM_HandleTypeDef htimer6;

void SysTick_Handler(){
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void TIM6_DAC_IRQHandler(void){
	HAL_TIM_IRQHandler(&htimer6);
}

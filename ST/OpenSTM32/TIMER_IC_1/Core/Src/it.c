/*
 * it.c
 *
 *  Created on: 21 ago. 2021
 *      Author: simon
 */
#include "main.h"

extern TIM_HandleTypeDef htimer5;
extern TIM_HandleTypeDef htimer6;

void SysTick_Handler(){
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void TIM5_IRQHandler(){
	HAL_TIM_IRQHandler(&htimer5);
}

void TIM6_DAC_IRQHandler(){
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	HAL_TIM_IRQHandler(&htimer6);
}

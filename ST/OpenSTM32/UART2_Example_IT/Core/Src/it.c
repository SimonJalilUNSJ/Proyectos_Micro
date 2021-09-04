/*
 * it.c
 *
 *  Created on: 21 ago. 2021
 *      Author: simon
 */
#include "main.h"
extern UART_HandleTypeDef huart2;

void SysTick_Handler(){
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void USART2_IRQHandler(void){
	HAL_UART_IRQHandler(&huart2);
}

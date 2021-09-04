/*
 * it.c
 *
 *  Created on: 21 ago. 2021
 *      Author: simon
 */
#include "main.h"

extern UART_HandleTypeDef huart2;

void EXTI15_10_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

void USART2_IRQHandler(void){
	HAL_UART_IRQHandler(&huart2);
}

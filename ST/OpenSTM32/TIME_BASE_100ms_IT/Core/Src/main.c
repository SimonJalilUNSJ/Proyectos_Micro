/*
 * main.c
 *
 *  Created on: 21 ago. 2021
 *      Author: simon
 */
#include "stm32f4xx_hal.h"
#include "main.h"

void SystemClockConfig(void);
void Error_handler(void);
void Timer6_Init(void);
void GPIO_Init(void);

TIM_HandleTypeDef htimer6;

int main(void){
	HAL_Init();
	SystemClockConfig();
	GPIO_Init();
	Timer6_Init();

	//Comencemos el timer en modo interrupcion.
	HAL_TIM_Base_Start_IT(&htimer6);

	while(1);

	return 0;
}

void SystemClockConfig(void){

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_5);
}

void GPIO_Init(void){
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef ledgpio;
	ledgpio.Pin		= GPIO_PIN_5;
	ledgpio.Mode	= GPIO_MODE_OUTPUT_PP;
	ledgpio.Pull	= GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA,&ledgpio);
}

void Timer6_Init(void){
	htimer6.Instance 		= TIM6;
	htimer6.Init.Prescaler	= 24;
	htimer6.Init.Period 	= 64000 - 1;
	if(HAL_TIM_Base_Init(&htimer6) != HAL_OK){
		Error_handler();
	}

}

void Error_handler(void){
	while(1);
}

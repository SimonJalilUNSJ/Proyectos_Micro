/*
 * msp.c
 *
 *  Created on: 21 ago. 2021
 *      Author: simon
 */
#include "main.h"

void HAL_MspInit(void)
{
	//Aca vamos a incializar las especificaciones de bajo nivel del procesador.
	//1. Establecer el grupo de prioridad del procesador cortex mx
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	//2. Habilitar el sistema de excepciones requerido del procesador cortex arm mx
	SCB->SHCSR |= 0x7 << 16;	//usage fault, memoryfault y bus fault system exceptions

	//3. Configurar la prioridad para el sistema de excepciones.
	HAL_NVIC_SetPriority(MemoryManagement_IRQn,0,0);
	HAL_NVIC_SetPriority(BusFault_IRQn,0,0);
	HAL_NVIC_SetPriority(UsageFault_IRQn,0,0);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef gpio_uart;

	//Aqui vamos a hacer la inicializacion a bajo nivel del periferico USART2
	/*TIP: Cuando el periferico no trabaje de manera correcta, esta funcion
	 * es la primera a ser analizada. Ya que la mayoria de la gente
	 * comete errores en configurar los ajustes de bajo nivel del periferico*/

	//1. Habilitar el clock para el periferico USART2.
	__HAL_RCC_USART2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	//2. Hacer la configuracion del multiplexeo de pin
	gpio_uart.Pin 		= GPIO_PIN_2;
	gpio_uart.Mode 		= GPIO_MODE_AF_PP;
	gpio_uart.Pull 		= GPIO_PULLUP;
	gpio_uart.Speed 	= GPIO_SPEED_FREQ_LOW;
	gpio_uart.Alternate = GPIO_AF7_USART2; 	//UART2 Tx
	HAL_GPIO_Init(GPIOA,&gpio_uart);

	gpio_uart.Pin 		= GPIO_PIN_3;		//UART2 Rx
	HAL_GPIO_Init(GPIOA,&gpio_uart);

	//3. Habilitar el IRQ y establecer la prioridad (ajustes NVIC)
	HAL_NVIC_EnableIRQ(USART2_IRQn);
	HAL_NVIC_SetPriority(USART2_IRQn,15,0);
}

void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan){
	GPIO_InitTypeDef GPIO_InitStruct;

	/**CAN1 configuracion GPIO
	 * PA11	--> CAN1_RX
	 * PA12 --> CAN1_TX
	 */
	GPIO_InitStruct.Pin		= GPIO_PIN_11 | GPIO_PIN_12;
	GPIO_InitStruct.Mode 		= GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull		= GPIO_NOPULL;
	GPIO_InitStruct.Speed		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate	= GPIO_AF9_CAN1;
	HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);

	HAL_NVIC_SetPriority(CAN1_TX_IRQn,15,0);
	HAL_NVIC_SetPriority(CAN1_RX0_IRQn,15,0);
	HAL_NVIC_SetPriority(CAN1_RX1_IRQn,15,0);
	HAL_NVIC_SetPriority(CAN1_SCE_IRQn,15,0);

	HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
	HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
	HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
	HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
}

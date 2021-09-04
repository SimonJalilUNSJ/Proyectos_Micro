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


void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htimer){
	//1. Habilitar el reloj para el periferico TIM6
	__HAL_RCC_TIM6_CLK_ENABLE();

	//2.Habilitar el IRQ de TIM6
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

	//3. Establecer la prioridad para TIM6_DAC_IRQn
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn,15,0);



}

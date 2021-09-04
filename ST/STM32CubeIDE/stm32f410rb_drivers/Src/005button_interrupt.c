/*
 * 005button_interrupt.c
 *
 *  Created on: 29 jun. 2021
 *      Author: simon
 */
#include <string.h>
#include "stm32f410rb.h"

#define HIGH 			1
#define LOW 			0
#define BTN_PRESSED	 	LOW

void delay(void){
	//Esto introduce ~200ms de delay cuando systick es 16MHz
	for(uint32_t i = 0; i < 500000/2; i++);
}

int main(void){
	GPIO_Handle_t GpioLed, GpioBtn;
	memset(&GpioLed,0,sizeof(GpioLed));	//Esto incializa la estructura con ceros
	memset(&GpioBtn,0,sizeof(GpioBtn));	//Esto incializa la estructura con ceros

	//Esta es la configuracion del led GPIO
	GpioLed.pGPIOx = GPIOA;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber 		= GPIO_PIN_NO_5;
	GpioLed.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUTPUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_SPEED_LOW;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OP_TYPE_PP;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_NO_PUPD;

	GPIO_PeriClockControl(GPIOA, ENABLE);

	GPIO_Init(&GpioLed);


	//Esta es la configuracion del boton GPIO
	GpioBtn.pGPIOx = GPIOC;
	GpioBtn.GPIO_PinConfig.GPIO_PinNumber 		= GPIO_PIN_NO_0;
	GpioBtn.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_IT_FT;
	GpioBtn.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_SPEED_FAST;
	GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_NO_PUPD;

	GPIO_PeriClockControl(GPIOC, ENABLE);

	GPIO_Init(&GpioBtn);

	//Configuraciones IRQ
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI0, NVIC_IRQ_PRI15);
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI0, ENABLE);
	while(1);
}

void EXTI0_IRQHandler(void){
	delay(); //200ms
	GPIO_IRQHandling(GPIO_PIN_NO_0);
	GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
}

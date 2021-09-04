/*
 * uart_tx.c
 *
 *  Created on: Jan 22, 2019
 *      Author: admin
 */

#include<stdio.h>
#include<string.h>
#include "stm32f410rb.h"

//Tenemos 3 diferentes mensajes que transmitimos a arduino
//Se pueden agreagar mas mensajes
char *msg[3] = {"hihihihihihi123", "Hello How are you ?" , "Today is Monday !"};

//La respuesta desde arduino para almacenar mensajes
char rx_buf[1024] ;

USART_Handle_t usart2_handle;

//Esta bandera indica recepcion completada
uint8_t rxCmplt = RESET;

uint8_t g_data = 0;

extern void initialise_monitor_handles();

void USART2_Init(void)
{
	usart2_handle.pUSARTx = USART2;
	usart2_handle.USART_Config.USART_Baud = USART_STD_BAUD_115200;
	usart2_handle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
	usart2_handle.USART_Config.USART_Mode = USART_MODE_TXRX;
	usart2_handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
	usart2_handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
	usart2_handle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;
	USART_Init(&usart2_handle);
}

void 	USART2_GPIOInit(void)
{
	GPIO_Handle_t usart_gpios;

	usart_gpios.pGPIOx = GPIOA;
	usart_gpios.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	usart_gpios.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	usart_gpios.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	usart_gpios.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	usart_gpios.GPIO_PinConfig.GPIO_PinAltFunMode =7;

	usart_gpios.GPIO_PinConfig.GPIO_PinNumber  = GPIO_PIN_NO_2;
	GPIO_Init(&usart_gpios);

	usart_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_3;
	GPIO_Init(&usart_gpios);

}

void GPIO_ButtonInit(void)
{
	GPIO_Handle_t GPIOBtn,GpioLed;

	//Esta es la configuracion GPIO del boton
	GPIOBtn.pGPIOx = GPIOA;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_INPUT;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_Init(&GPIOBtn);

	//Esta es la configuracion del led gpio
	GpioLed.pGPIOx = GPIOA;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUTPUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_PeriClockControl(GPIOA,ENABLE);

	GPIO_Init(&GpioLed);

}

void delay(void)
{
	for(uint32_t i = 0 ; i < 500000/2 ; i ++);
}
int main(void)
{
	uint32_t cnt = 0;


	initialise_monitor_handles();

	USART2_GPIOInit();
    USART2_Init();

    USART_IRQInterruptConfig(IRQ_NO_USART2,ENABLE);

    USART_PeripheralControl(USART2,ENABLE);

    printf("La aplicacion se esta ejecutando\n");

    while(1)
    {
    	//Esperar hasta que el boton sea presionado
		while( ! GPIO_ReadFromInputPin(GPIOA,GPIO_PIN_NO_0) );

		//Para evitar el rebote del boton se espera un delay de 200ms
		delay();

		// Índice de mensaje siguiente; asegurarse de que el valor cnt no cruce 2
		cnt = cnt % 3;

		//Primero se habilita la recepcion en modo interrupcion
		//Este codigo habilita la interrupcion de recepcion
		while ( USART_ReceiveDataIT(&usart2_handle,rx_buf,strlen(msg[cnt])) != USART_READY );

		//Envia el indice de mensaje mediante cnt en modo blqueo
		USART_SendData(&usart2_handle,(uint8_t*)msg[cnt],strlen(msg[cnt]));

    	printf("Transmitted : %s\n",msg[cnt]);

    	//Ahora dejemos esperar hasta que todos los bytes son recibidos desde arduino
    	//Cuando todos los bytes son recividos rxCmplt seran seteados en applicactionCallback
    	while(rxCmplt != SET);

    	//Solo asegurarse que el ultimo byte debe ser null sino de otra manera %s falla mientras imprime
    	rx_buf[strlen(msg[cnt])+ 1] = '\0';

    	//Imprimir lo que recibimos desde el arduino
    	printf("Received    : %s\n",rx_buf);

    	//Se invalida la bandera
    	rxCmplt = RESET;

    	//Se mueve al siguiente mensaje indexado en msg[]
    	cnt ++;
    }

	return 0;
}


void USART2_IRQHandler(void)
{
	USART_IRQHandling(&usart2_handle);
}





void USART_ApplicationEventCallback( USART_Handle_t *pUSARTHandle,uint8_t ApEv)
{
   if(ApEv == USART_EVENT_RX_CMPLT)
   {
			rxCmplt = SET;

   }else if (ApEv == USART_EVENT_TX_CMPLT)
   {
	   ;
   }
}

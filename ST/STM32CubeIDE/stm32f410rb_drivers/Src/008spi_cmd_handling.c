/*
 * 008spi_cmd_handling.c
 *
 *  Created on: 29 jul. 2021
 *      Author: simon
 */
#include "string.h"
#include "stm32f410rb.h"
#include <stdio.h>

/*
 * PB14 -> SPI2_MISO
 * PB15 -> SPI2_MOSI
 * PB13 -> SPI2_SCK
 * PB12 -> SPI2_NSS
 * Alternate Funtion mode: AF5
 * */

//Comandos arduino
#define COMMAND_LED_CTRL	0x50
#define COMMAND_SENSOR_READ 0x51
#define COMMAND_LED_READ 	0x52
#define COMMAND_PRINT 		0x53
#define COMMAND_ID_READ 	0x54

#define LED_ON	1
#define LED_OFF 0

//Arduino pins analigicos
#define ANALOG_PIN0	0
#define ANALOG_PIN1 1
#define ANALOG_PIN2	2
#define ANALOG_PIN3	3
#define ANALOG_PIN4	4

//Arduino led
#define LED_PIN		9

void delay(void){
	for(uint32_t i = 0; i < 500000/2; i++);
}

void SPI2_GPIOInits(void){
	GPIO_Handle_t SPIPins;

	SPIPins.pGPIOx = GPIOB;
	SPIPins.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALTFN;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode	= 5;
	SPIPins.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OP_TYPE_PP;
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_NO_PUPD;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_SPEED_FAST;

	//SCLK
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIO_Init(&SPIPins);

	//MOSI
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	GPIO_Init(&SPIPins);

	//MISO
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	GPIO_Init(&SPIPins);

	//NSS
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIO_Init(&SPIPins);
}

void SPI2_Inits(void){
	SPI_Handle_t SPI2Handle;

	SPI2Handle.pSPIx = SPI2;
	SPI2Handle.SPIConfig.SPI_BusConfig	= SPI_BUS_CONFIG_FD;
	SPI2Handle.SPIConfig.SPI_DeviceMode	= SPI_DEVICE_MODE_MASTER;
	SPI2Handle.SPIConfig.SPI_SclkSpeed 	= SPI_SCLK_SPEED_DIV32;
	SPI2Handle.SPIConfig.SPI_CPOL 		= SPI_CPOL_LOW;
	SPI2Handle.SPIConfig.SPI_CPHA		= SPI_CPHA_LOW;
	SPI2Handle.SPIConfig.SPI_SSM		= SPI_SSM_DI;	//Hardware slave management habilitado para NSS pin

	SPI_Init(&SPI2Handle);
}

void GPIO_ButtonInit(void){
	GPIO_Handle_t GPIOBtn;

	GPIOBtn.pGPIOx = GPIOC;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_INPUT;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_Init(&GPIOBtn);
}

uint8_t SPI_VerifyResponse(uint8_t ackbyte){
	if(ackbyte == (uint8_t)0xF5){
		//ack
		return 1;
	}
	return 0;
}

int main(void){
	uint8_t dummy_write = 0xFF;
	uint8_t dummy_read;

	//Inicializar el manipulador de monitor
	printf("\nLa aplicación esta ejecutandose.");

	GPIO_ButtonInit();

	//Esta funcion es usada para inicializar los pins GPIO para manejarlos como SPI2  pins
	SPI2_GPIOInits();

	//Esta funcion es usada para inicializar los parametros del periferico SPI2
	SPI2_Inits();

	printf("SPI Init. done\n");

	/*
	 * Haciendo SSOE 1 hace que la salida NSS este habilitada
	 * El pin NSS es automaticamente manejado por el hardware
	 * i.e. cuando SPE = 1, NSSS sera pulleado a LOW
	 * y NSS pin sera HIGH cuando SPE = 0
	 * */
	SPI_SSOEConfig(SPI2, ENABLE);

	while(1){
		//Espera hasta que se presione el boton
		while(GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13));

		//Evita rebotes del boton con 200ms de espera aprox
		delay();

		//Habilitar el periferico SPI2
		SPI_PeripheralControl(SPI2, ENABLE);

		//1. CMD_LED_CTRL	<pin_no(1)>		<value(1)>
		uint8_t commandcode = COMMAND_LED_CTRL;
		uint8_t ackbyte;
		uint8_t args[2];

		//Enviar comando
		SPI_SendData(SPI2, &commandcode, 1);

		//hacer la lectura dummy para limpiar RXNE
		SPI_ReceiveData(SPI2, &dummy_read, 1);

		//Enviar dummy bytes para buscar respuesta desde el esclavo.
		SPI_SendData(SPI2, &dummy_write, 1);

		//Leer el ack byte recibido
		SPI_ReceiveData(SPI2, &ackbyte, 1);

		if(SPI_VerifyResponse(ackbyte)){
			//Enviar argumentos
			args[0] = LED_PIN;
			args[1] = LED_ON;
			SPI_SendData(SPI2, args, 2);
			SPI_ReceiveData(SPI2, args, 2);
			printf("COMMAND_LED_CTRL ejecutado\n");
		}
		//Fin del comando COMMAND_LED_CTRL

		//2.CMD_SENSOR_READ <analog pin number(1)>
		//Espera hasta que se presione el boton
		while(GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13));

		//Evita rebotes del boton con 200ms de espera aprox
		delay();

		commandcode = COMMAND_SENSOR_READ;

		//Enviar comando
		SPI_SendData(SPI2, &commandcode, 1);

		//hacer la lectura dummy para limpiar RXNE
		SPI_ReceiveData(SPI2, &dummy_read, 1);

		//Enviar dummy bytes para buscar respuesta desde el esclavo.
		SPI_SendData(SPI2, &dummy_write, 1);

		//Leer el ack byte recibido
		SPI_ReceiveData(SPI2, &ackbyte, 1);

		if(SPI_VerifyResponse(ackbyte)){
			//Enviar argumentos
			args[0] = ANALOG_PIN0;
			SPI_SendData(SPI2, args, 1);

			//hacer la lectura dummy para limpiar RXNE
			SPI_ReceiveData(SPI2, &dummy_read, 1);

			//Insertar un delay para que el esclavo este listo para enviar la informacion
			delay();

			//Enviar dummy bytes para buscar respuesta desde el esclavo.
			SPI_SendData(SPI2, &dummy_write, 1);

			uint8_t analog_read;
			SPI_ReceiveData(SPI2, &analog_read, 1);
			//Fin del comando

			//Confirmemos que el periferico no esta ocupado para deshabilitarlo.
			while(SPI_GetFlagStatus(SPI2,SPI_BUSY_FLAG));

			//Deshabilito el periferico SPI2
			SPI_PeripheralControl(SPI2, DISABLE);
			printf("COMMAND_SENSOR_READ %d\n",analog_read);
		}
		//Fin del comando COMMAND_SENSOR_READ

		//3.  CMD_LED_READ 	 <pin no(1) >
		//Esperar a que el boton sea presionado
		while( ! GPIO_ReadFromInputPin(GPIOA,GPIO_PIN_NO_0) );

		//delay para evadir problemas de rebote
		delay();

		commandcode = COMMAND_LED_READ;

		//enviar comando
		SPI_SendData(SPI2,&commandcode,1);

		//hacer una lectura dummy para eliminar el bit RXNE
		SPI_ReceiveData(SPI2,&dummy_read,1);

		//Enviar un dummy byte para buscar respuesta del esclavo
		SPI_SendData(SPI2,&dummy_write,1);

		//leer el ack byte recibido
		SPI_ReceiveData(SPI2,&ackbyte,1);

		if( SPI_VerifyResponse(ackbyte))
		{
			args[0] = LED_PIN;

			//enviar argumentos
			SPI_SendData(SPI2,args,1); //sending one byte of

			//hacer una lectura dummy para limpiar RXNE
			SPI_ReceiveData(SPI2,&dummy_read,1);

			//Insertar un delay para que el esclavo pueda tener listo el dato
			delay();

			//Enviar un byte dummy para buscar respuesta desde el esclavo
			SPI_SendData(SPI2,&dummy_write,1);

			uint8_t led_status;
			SPI_ReceiveData(SPI2,&led_status,1);
			printf("COMMAND_READ_LED %d\n",led_status);
		}
		//Fin del comando CMD_LED_READ

		//4. CMD_PRINT 		<len(2)>  <message(len) >
		//wait till button is pressed
		while( ! GPIO_ReadFromInputPin(GPIOA,GPIO_PIN_NO_0) );

		//to avoid button de-bouncing related issues 200ms of delay
		delay();

		commandcode = COMMAND_PRINT;

		//send command
		SPI_SendData(SPI2,&commandcode,1);

		//do dummy read to clear off the RXNE
		SPI_ReceiveData(SPI2,&dummy_read,1);

		//Send some dummy byte to fetch the response from the slave
		SPI_SendData(SPI2,&dummy_write,1);

		//read the ack byte received
		SPI_ReceiveData(SPI2,&ackbyte,1);

		uint8_t message[] = "Hello ! How are you ??";
		if( SPI_VerifyResponse(ackbyte))
		{
			args[0] = strlen((char*)message);

			//send arguments
			SPI_SendData(SPI2,args,1); //sending length

			//do dummy read to clear off the RXNE
			SPI_ReceiveData(SPI2,&dummy_read,1);

			delay();

			//send message
			for(int i = 0 ; i < args[0] ; i++){
				SPI_SendData(SPI2,&message[i],1);
				SPI_ReceiveData(SPI2,&dummy_read,1);
			}

			printf("COMMAND_PRINT Executed \n");
		}


		//5. CMD_ID_READ
		//wait till button is pressed
		while( ! GPIO_ReadFromInputPin(GPIOA,GPIO_PIN_NO_0) );

		//to avoid button de-bouncing related issues 200ms of delay
		delay();

		commandcode = COMMAND_ID_READ;

		//send command
		SPI_SendData(SPI2,&commandcode,1);

		//do dummy read to clear off the RXNE
		SPI_ReceiveData(SPI2,&dummy_read,1);

		//Send some dummy byte to fetch the response from the slave
		SPI_SendData(SPI2,&dummy_write,1);

		//read the ack byte received
		SPI_ReceiveData(SPI2,&ackbyte,1);

		uint8_t id[11];
		uint32_t i=0;
		if( SPI_VerifyResponse(ackbyte))
		{
			//read 10 bytes id from the slave
			for(  i = 0 ; i < 10 ; i++)
			{
				//send dummy byte to fetch data from slave
				SPI_SendData(SPI2,&dummy_write,1);
				SPI_ReceiveData(SPI2,&id[i],1);
			}

			id[10] = '\0';

			printf("COMMAND_ID : %s \n",id);

		}

		//lets confirm SPI is not busy
		while( SPI_GetFlagStatus(SPI2,SPI_BUSY_FLAG) );

		//Disable the SPI2 peripheral
		SPI_PeripheralControl(SPI2,DISABLE);

		printf("SPI Communication Closed\n");
	}
	return 0;
}



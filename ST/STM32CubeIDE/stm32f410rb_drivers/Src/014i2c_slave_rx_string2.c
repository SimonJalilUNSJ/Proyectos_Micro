/*
 * 012i2c_master_rx_testing.c
 *
 *  Created on: 10 ago. 2021
 *      Author: simon
 */
#include <stdio.h>
#include <string.h>
#include "stm32f410rb.h"

//Codigos de comando
#define COMMAND_LED_CTRL		0x50
#define	COMMAND_SENSOR_READ 	0x51
#define COMMAND_LED_READ 		0x52
#define COMMAND_PRINT			0x53
#define COMMAND_ID_READ			0x54

#define LED_ON		1
#define LED_OFF		0

//arduino
#define ANALOG_PIN0		0
#define ANALOG_PIN1 	1
#define ANALOG_PIN2		2
#define ANALOG_PIN3		3
#define ANALOG_PIN4		4

//Arduino led
#define LED_PIN			9

#define SLAVE_ADDR 		0x68

#define MY_ADDR 		SLAVE_ADDR

void delay(void){
	for(uint32_t i = 0; i < 500000/2; i++);
}

I2C_Handle_t I2C1Handle;

//Receive Buffer
uint8_t Tx_buf[32] = "STM32 Slave mode testing..";

/*
 * PB6-> SCL
 * PB9-> SDA
 * */

void I2C1_GPIOInits(void){
	GPIO_Handle_t I2CPins;

	I2CPins.pGPIOx = GPIOB;
	I2CPins.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALTFN;
	I2CPins.GPIO_PinConfig.GPIO_PinOPType	 	= GPIO_OP_TYPE_OD;
	I2CPins.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PIN_PU;
	I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode 	= 4;
	I2CPins.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_SPEED_FAST;

	//I2C
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_Init(&I2CPins);
}

void I2C1_Inits(void){
	I2C1Handle.pI2Cx = I2C1;
	I2C1Handle.I2C_Config.I2C_AckControl 	= I2C_ACK_ENABLE;
	I2C1Handle.I2C_Config.I2C_DeviceAddress = MY_ADDR;
	I2C1Handle.I2C_Config.I2C_FMDutyCycle	= I2C_FM_DUTY_2;
	I2C1Handle.I2C_Config.I2C_SCLSpeed		= I2C_SCL_SPEED_SM;

	I2C_Init(&I2C1Handle);
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

int main (void){

	GPIO_ButtonInit();

	//I2C pin inits
	I2C1_GPIOInits();

	//I2C configuracion de perifericos
	I2C1_Inits();

	//I2C IRQ configurations
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_EV, ENABLE);
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_ER, ENABLE);

	I2C_EnableDisableCallbackEvents(I2C1,ENABLE);

	//Habilitar el periferico I2C
	I2C_PeripheralControl(I2C1, ENABLE);

	//ack bit es 1 despues de PE = 1
	I2C_ManageAcking(I2C1, I2C_ACK_ENABLE);

	while(1);
}

void I2C1_EV_IRQHandler(void){
	I2C_EV_IRQHandling(&I2C1Handle);
}

void I2C1_ER_IRQHandler(void){
	I2C_ER_IRQHandling(&I2C1Handle);
}

void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle,uint8_t AppEv){
	static uint8_t commandCode = 0;
	static uint8_t Cnt = 0;

	if(AppEv == I2C_EV_DATA_REQ){
		//El maestro quiere enviar datos. El esclavo tiene que enviarlos
		if(commandCode == 0x51){
			//Enviar la longitud de informacion del maestro.
			I2C_SlaveSendData(pI2CHandle->pI2Cx, strlen((char *)Tx_buf));
		}else if(commandCode == 0x52){
			//Enviar el contenido del Tx_buf
			I2C_SlaveSendData(pI2CHandle->pI2Cx, Tx_buf[Cnt++]);
		}

	}else if(AppEv == I2C_EV_DATA_RCV){
		//Los datos estan esperando para que sean leidos por el esclavo. El esclavo tiene que leerlos.
		commandCode = I2C_SlaveReceiveData(pI2CHandle->pI2Cx);

	}else if(AppEv == I2C_ERROR_AF){
		//Esto ocurre solo durante la transmision de esclavo.
		//El maestro envio el NACK. Entonces el esclavo deberia entender que el maestro
		//no necesita mas datos.
		commandCode = 0xFF;
		Cnt = 0;

	}else if(AppEv == I2C_EV_STOP){
		//Esto ocurre durante la recepcion de esclavo.
		//El maestro termino la comunicacion I2C con el esclavo.

	}
}


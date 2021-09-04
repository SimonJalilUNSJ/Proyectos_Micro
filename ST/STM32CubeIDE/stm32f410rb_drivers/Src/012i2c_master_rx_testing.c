/*
 * 012i2c_master_rx_testing.c
 *
 *  Created on: 10 ago. 2021
 *      Author: simon
 */
#include <stdio.h>
#include <string.h>
#include "stm32f410rb.h"

extern void initialise_monitor_handles();

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

#define MY_ADDR			0x61

#define SLAVE_ADDR 		0x68

void delay(void){
	for(uint32_t i = 0; i < 500000/2; i++);
}

I2C_Handle_t I2C1Handle;

//Receive Buffer
uint8_t rcv_buf[32];

uint8_t rxComplt = RESET;

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

	//scl
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_Init(&I2CPins);

	//sda
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_9;
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

	uint8_t commandcode;

	uint8_t Len;

	initialise_monitor_handles();

	printf("La aplicación esta siendo ejecutada...\n");

	GPIO_ButtonInit();

	//I2C pin inits
	I2C1_GPIOInits();

	//I2C configuracion de perifericos
	I2C1_Inits();

	//I2C IRQ configurations
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_EV, ENABLE);
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_ER, ENABLE);

	//Habilitar el periferico I2C
	I2C_PeripheralControl(I2C1, ENABLE);

	//ack bit es 1 despues de PE = 1
	I2C_ManageAcking(I2C1, I2C_ACK_ENABLE);

	while(1){
		//Espera hasta que se presione el boton
		while(GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13));

		//Evita rebotes del boton con 200ms de espera aprox
		delay();

		//Envio comando para leer datos
		commandcode = 0x51;
		while(I2C_MasterSendDataIT(&I2C1Handle, &commandcode, 1, SLAVE_ADDR, I2C_ENABLE_SR) != I2C_READY);

		//Recibo la longitud del dato
		while(I2C_MasterReceiveDataIT(&I2C1Handle, &Len, 1, SLAVE_ADDR, I2C_ENABLE_SR) != I2C_READY);

		//Envio comando para que el esclavo envie datos
		commandcode = 0x52;
		while(I2C_MasterSendDataIT(&I2C1Handle, &commandcode, 1, SLAVE_ADDR, I2C_ENABLE_SR) != I2C_READY);

		while(I2C_MasterReceiveDataIT(&I2C1Handle, rcv_buf, Len, SLAVE_ADDR,I2C_DISABLE_SR) != I2C_READY);

		rxComplt = RESET;

		//Esperar hasta que rx se complete
		while(rxComplt != SET);

		rcv_buf[Len + 1] = '\0';

		printf("Data : %s", rcv_buf);

		rxComplt = RESET;
	}
}

void I2C1_EV_IRQHandler(void){
	I2C_EV_IRQHandling(&I2C1Handle);
}

void I2C1_ER_IRQHandler(void){
	I2C_ER_IRQHandling(&I2C1Handle);
}

void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle,uint8_t AppEv){
	if(AppEv == I2C_EV_TX_CMPLT){
		printf("Tx es completada\n");
	}else if(AppEv == I2C_EV_RX_CMPLT){
		printf("Rx es completada\n");
		rxComplt = SET;
	}else if(AppEv == I2C_ERROR_AF){
		printf("Error : Falla Ack");
		//En maestro la falla ack ocurre cuando el esclavo falla en enviar ack para el byte
		//enviado desde el maestro.
		I2C_CloseSendData(pI2CHandle);

		//Generar la condicion de STOP para liberar el bus
		I2C_GenerateStopCondition(I2C1);

		//Mantener en un loop infinito
		while(1);
	}
}


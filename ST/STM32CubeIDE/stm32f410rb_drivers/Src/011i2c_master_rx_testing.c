/*
 * 011i2c_master_rx_testing.c
 *
 *  Created on: 7 ago. 2021
 *      Author: simon
 */
#include <stdio.h>
#include <string.h>
#include "stm32f410rb.h"

extern void initialise_monitor_handles();

#define MY_ADDR			0x61

#define SLAVE_ADDR 		0x68

void delay(void){
	for(uint32_t i = 0; i < 500000/2; i++);
}

I2C_Handle_t I2C1Handle;

//Receive Buffer
uint8_t rcv_buf[32];

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

	//I2C-scl
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_Init(&I2CPins);

	//I2C-sda
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
		I2C_MasterSendData(&I2C1Handle, &commandcode, 1, SLAVE_ADDR, I2C_ENABLE_SR);

		//Recibo la longitud del dato
		I2C_MasterReceiveData(&I2C1Handle, &Len, 1, SLAVE_ADDR, I2C_ENABLE_SR);

		//Envio comando para que el esclavo envie datos
		commandcode = 0x52;
		I2C_MasterSendData(&I2C1Handle, &commandcode, 1, SLAVE_ADDR, I2C_ENABLE_SR);

		I2C_MasterReceiveData(&I2C1Handle, rcv_buf, Len, SLAVE_ADDR,I2C_DISABLE_SR);

		rcv_buf[Len + 1] = '\0';

		printf("Data : %s", rcv_buf);
	}
}


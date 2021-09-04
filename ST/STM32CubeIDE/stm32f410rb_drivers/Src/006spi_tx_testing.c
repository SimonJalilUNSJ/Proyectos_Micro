/*
 * 006spi_tx_testing.c
 *
 *  Created on: 12 jul. 2021
 *      Author: simon
 */

#include "string.h"
#include "stm32f410rb.h"

/*
 * PB14 -> SPI2_MISO
 * PB15 -> SPI2_MOSI
 * PB13 -> SPI2_SCK
 * PB12 -> SPI2_NSS
 * Alternate Funtion mode: AF5
 * */

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
	//SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	//GPIO_Init(&SPIPins);

	//NSS
	//SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	//GPIO_Init(&SPIPins);
}

void SPI2_Inits(void){
	SPI_Handle_t SPI2Handle;

	SPI2Handle.pSPIx = SPI2;
	SPI2Handle.SPIConfig.SPI_BusConfig	= SPI_BUS_CONFIG_FD;
	SPI2Handle.SPIConfig.SPI_DeviceMode	= SPI_DEVICE_MODE_MASTER;
	SPI2Handle.SPIConfig.SPI_SclkSpeed 	= SPI_SCLK_SPEED_DIV2;	//SCLK : 8MHz
	SPI2Handle.SPIConfig.SPI_DFF 		= SPI_DFF_8BITS;
	SPI2Handle.SPIConfig.SPI_CPOL 		= SPI_CPOL_HIGH;
	SPI2Handle.SPIConfig.SPI_CPHA		= SPI_CPHA_LOW;
	SPI2Handle.SPIConfig.SPI_SSM		= SPI_SSM_EN;	//Software slave management habilitado para NSS pin

	SPI_Init(&SPI2Handle);
}

int main(void){
	char user_data[] = "Hello World";

	//Esta funcion es usada para inicializar los pins GPIO para manejarlos como SPI2  pins
	SPI2_GPIOInits();

	//Esta funcion es usada para inicializar los parametros del periferico SPI2
	SPI2_Inits();

	//Esto hace que la señal interna de NSS sea HIGH y evita el error MODF
	SPI_SSIConfig(SPI2,ENABLE);

	//Habilitar el periferico SPI2
	SPI_PeripheralControl(SPI2, ENABLE);

	SPI_SendData(SPI2, (uint8_t*)user_data, strlen(user_data));

	//Confirmemos que el periferico no esta ocupado para deshabilitarlo.
	while(SPI_GetFlagStatus(SPI2,SPI_BUSY_FLAG));

	SPI_PeripheralControl(SPI2, DISABLE);
	while(1);
	return 0;
}

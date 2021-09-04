/*
 * stm32f410rb_spi_driver.c
 *
 *  Created on: 8 jul. 2021
 *      Author: simon
 */
#include "stm32f410rb_spi_driver.h"

static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle);

void SPI_PeriClockControl (SPI_RegDef_t *pSPIx, uint8_t EnorDi){
	if(EnorDi == ENABLE){
		if(pSPIx == SPI1){
			SPI1_PCLK_EN();
		}else if(pSPIx == SPI2){
			SPI2_PCLK_EN();
		}
	}else{
		if(pSPIx == SPI1){
			SPI1_PCLK_DI();
		}else if(pSPIx == SPI2){
			SPI2_PCLK_DI();
		}
	}
}

void SPI_Init(SPI_Handle_t *pSPIHandle){
	//Habilitar el clock de periferico
	SPI_PeriClockControl(pSPIHandle->pSPIx, ENABLE);

	//Primero configuramos el registro SPI_CR1
	uint32_t tempreg = 0;

	//1. Configuramos el modo de dispositivo
	tempreg	|= pSPIHandle->SPIConfig.SPI_DeviceMode << SPI_CR1_MSTR;

	//2. Configurar el bus
	if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD){
		//Modo bidireccional deberia limpiarse
		tempreg &= ~(1 << SPI_CR1_BIDIMODE);

	}else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD){
		//Modo bidireccional deberia ser seteado
		tempreg |= (1 << SPI_CR1_BIDIMODE);

	}else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY){
		//Modo bidireccional deberia limpiarse
		tempreg &= ~(1 << SPI_CR1_BIDIMODE);
		//RXONLY bit deberia setearse
		tempreg |= (1 << SPI_CR1_RXONLY);
	}

	//3. Configurar la velocidad del clock serial spi (baudrate)
	tempreg |= pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR;

	//4. Configurar el DFF
	tempreg |= pSPIHandle->SPIConfig.SPI_DFF << SPI_CR1_DFF;

	//5. Configurar el CPOL
	tempreg |= pSPIHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL;

	//6. Configurar el CPHA
	tempreg |= pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA;

	tempreg |= pSPIHandle->SPIConfig.SPI_SSM << SPI_CR1_SSM;

	pSPIHandle->pSPIx->CR1 = tempreg;
}

void SPI_DeInit(SPI_RegDef_t *pSPIx){
	uint32_t tempreg = 0;
	pSPIx->CR1 = tempreg;
}

void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len){
	while (Len > 0){
		//1. Esperar hasta que TXE sea seteado
		while(SPI_GetFlagStatus(pSPIx,SPI_TXE_FLAG) == FLAG_RESET);

		//2 Chequear el DFF bit en CR1
		if( (pSPIx->CR1 & (1 << SPI_CR1_DFF) ) ){
			//16 bit DFF
			//1. Cargar el dato en el registro DF
			pSPIx->DR = *((uint16_t*)pTxBuffer);
			Len--;
			Len--;
			(uint16_t*)pTxBuffer++;
		}else{
			//8 bit DFF
			pSPIx->DR = *(pTxBuffer);
			Len--;
			pTxBuffer++;
		}
	}
}

void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len){
	while (Len > 0){
		//1. Esperar hasta que RXE sea seteado
		while(SPI_GetFlagStatus(pSPIx,SPI_RXNE_FLAG) == FLAG_RESET);

		//2 Chequear el DFF bit en CR1
		if( (pSPIx->CR1 & (1 << SPI_CR1_DFF) ) ){
			//16 bit DFF
			//1. Cargar el dato desde DR a la direccion del Rxbuffer
			*((uint16_t*)pRxBuffer) = pSPIx->DR;
			Len--;
			Len--;
			(uint16_t*)pRxBuffer++;
		}else{
			//8 bit DFF
			*(pRxBuffer) = pSPIx->DR;
			Len--;
			pRxBuffer++;
		}
	}
}

void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi){
	if(EnOrDi == ENABLE){
		pSPIx->CR1 |= (1 << SPI_CR1_SPE);
	}else{
		pSPIx->CR1 &= ~(1 << SPI_CR1_SPE);
	}
}

void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi){
	if(EnOrDi == ENABLE){
		pSPIx->CR1 |= (1 << SPI_CR1_SSI);
	}else{
		pSPIx->CR1 &= ~(1 << SPI_CR1_SSI);
	}
}

void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi){
	if(EnOrDi == ENABLE){
		pSPIx->CR2 |= (1 << SPI_CR2_SSOE);
	}else{
		pSPIx->CR2 &= ~(1 << SPI_CR2_SSOE);
	}
}

uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName){
	if(pSPIx->SR & FlagName){
		return FLAG_SET;
	}
	return FLAG_RESET;
}

void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi){
	if(EnorDi == ENABLE){
		if(IRQNumber <= 31){
			//Programar el registro ISER0
			*NVIC_ISER0 |= (1 << IRQNumber);
			*NVIC_ICER0 &= ~(1 << IRQNumber);

		}else if(IRQNumber > 31 && IRQNumber < 64){
			//Programar el registro ISER1	//32 a 63
			*NVIC_ISER1 |= (1 << IRQNumber % 32);

		}else if(IRQNumber >= 64 && IRQNumber < 96){
			//Programar el registro ISER2 	//64 a 95
			*NVIC_ISER3 |= (1 << IRQNumber % 64);
		}
	}else{
		if(IRQNumber <= 31){
			//Programar el registro ISER0
			*NVIC_ICER0 |= (1 << IRQNumber);

		}else if(IRQNumber > 31 && IRQNumber < 64){
			//Programar el registro ISER1	//32 a 63
			*NVIC_ICER1 |= (1 << IRQNumber % 32);

		}else if(IRQNumber >= 64 && IRQNumber < 96){
			//Programar el registro ISER2 	//64 a 95
			*NVIC_ICER3 |= (1 << IRQNumber % 64);
		}
	}
}

void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority){
	//1. Primero encontrar el registro ipr
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section = IRQNumber %4;
	uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);
	*(NVIC_PR_BASE_ADDR + iprx)	|= (IRQPriority << shift_amount);
}

uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t Len){
	uint8_t state = pSPIHandle->TxState;
	if(state != SPI_BUSY_IN_TX){
		//1. Guardar la direccion buffer Tx y la informacion Len en algunas variables globales
		pSPIHandle->pTxBuffer 	= pTxBuffer;
		pSPIHandle->TxLen 		= Len;

		//2. Marcar el estado SPI como ocupado en transmision
		//de esa manera ningun otro codigo puede tomar el mismo periferico hasta que la transmision finalice
		pSPIHandle->TxState = SPI_BUSY_IN_TX;

		//3. Habilitar el control de bit TXEIE para tener la interrupcion cuando el flag TXE es seteado en SR.
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXEIE);
	}
	return state;
}

uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len){
	uint8_t state = pSPIHandle->RxState;
	if(state != SPI_BUSY_IN_RX){
		//1. Guardar la direccion buffer Tx y la informacion Len en algunas variables globales
		pSPIHandle->pRxBuffer 	= pRxBuffer;
		pSPIHandle->RxLen 		= Len;

		//2. Marcar el estado SPI como ocupado en transmision
		//de esa manera ningun otro codigo puede tomar el mismo periferico hasta que la transmision finalice
		pSPIHandle->RxState = SPI_BUSY_IN_RX;

		//3. Habilitar el control de bit TXEIE para tener la interrupcion cuando el flag TXE es seteado en SR.
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE);
	}
	return state;
}

void SPI_IRQHandling(SPI_Handle_t *pHandle){
	uint8_t temp1, temp2;

	//Primero chequeemos para TXE
	temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_TXE);
	temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_TXEIE);

	if(temp1 && temp2)
		//Manipular TXE
		spi_txe_interrupt_handle(pHandle);


	//chequeemos para RXNE
	temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_RXNE);
	temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_RXNEIE);

	if(temp1 && temp2)
		//Manipular TXE
		spi_rxne_interrupt_handle(pHandle);

	//chequeemos para OVR flag
	temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_OVR);
	temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_ERRIE);

	if(temp1 && temp2)
		//Manipular TXE
		spi_ovr_err_interrupt_handle(pHandle);
}


//Implementaciones de algunas funciones auxuiliares
static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle){
	//Chequear el DFF bit en CR1
	if( (pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF) ) ){
		//16 bit DFF
		//1. Cargar el dato en el registro DF
		pSPIHandle->pSPIx->DR = *((uint16_t*)pSPIHandle->pTxBuffer);
		pSPIHandle->TxLen--;
		pSPIHandle->TxLen--;
		(uint16_t*)pSPIHandle->pTxBuffer++;
	}else{
		//8 bit DFF
		pSPIHandle->pSPIx->DR = *pSPIHandle->pTxBuffer;
		pSPIHandle->TxLen--;
		pSPIHandle->pTxBuffer++;
	}

	if(! pSPIHandle->TxLen){
		//TxLen es cero, entonces cerrar la transmision spi e informar a la aplicacion
		//que TX termino.

		//Esto prevee a la interrupcion del seteo de TXE flag.
		SPI_CloseTransmission(pSPIHandle);
		SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_TX_CMPLT);
	}
}

static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle){
	//do rxing as per the dff
	if(pSPIHandle->pSPIx->CR1 & ( 1 << 11))
	{
		//16 bit
		*((uint16_t*)pSPIHandle->pRxBuffer) = (uint16_t) pSPIHandle->pSPIx->DR;
		pSPIHandle->RxLen -= 2;
		pSPIHandle->pRxBuffer++;
		pSPIHandle->pRxBuffer++;

	}else
	{
		//8 bit
		*(pSPIHandle->pRxBuffer) = (uint8_t) pSPIHandle->pSPIx->DR;
		pSPIHandle->RxLen--;
		pSPIHandle->pRxBuffer++;
	}

	if(! pSPIHandle->RxLen)
	{
		//reception is complete
		SPI_CloseReception(pSPIHandle);
		SPI_ApplicationEventCallback(pSPIHandle,SPI_EVENT_RX_CMPLT);
	}
}

static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle){
	uint8_t temp;
	//1. Limpiar el flag ovr
	if(pSPIHandle->TxState != SPI_BUSY_IN_TX){
		temp = pSPIHandle->pSPIx->DR;
		temp = pSPIHandle->pSPIx->SR;
	}
	(void) temp;
	//2. Informar a la aplicacion
	SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_OVR_ERR);
}


void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle){
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_TXEIE);
	pSPIHandle->pTxBuffer = NULL;
	pSPIHandle->TxLen = 0;
	pSPIHandle->TxState = SPI_READY;
}

void SPI_CloseReception(SPI_Handle_t *pSPIHandle){
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_RXNEIE);
	pSPIHandle->pRxBuffer = NULL;
	pSPIHandle->RxLen = 0;
	pSPIHandle->RxState = SPI_READY;
}

void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx){
	uint8_t temp;
	temp = pSPIx->DR;
	temp = pSPIx->SR;
	(void)temp;
}

__attribute__((weak)) void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle,uint8_t AppEv){
	//Esta es una implementacion weak. El usuario de la aplicacion puede anular esta funcion.
}


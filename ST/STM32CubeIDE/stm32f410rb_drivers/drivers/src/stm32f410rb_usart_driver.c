/*
 * stm32f410rb_usart_driver.c
 *
 *  Created on: 11 ago. 2021
 *      Author: simon
 */
#include "stm32f410rb_usart_driver.h"


/*********************************************************************
 * @fn      		  - I2C_PeriClockControl
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */
void USART_PeriClockControl(USART_RegDef_t *pUSARTx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pUSARTx == USART1)
		{
			USART1_PCLK_EN();
		}else if (pUSARTx == USART2)
		{
			USART2_PCLK_EN();
		}else if (pUSARTx == USART6)
		{
			USART6_PCLK_EN();
		}
	}
	else
	{
		//TODO
	}

}


/*********************************************************************
 * @fn      		  - USART_Init
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */
void USART_Init(USART_Handle_t *pUSARTHandle)
{

	//Variable temporal
	uint32_t tempreg=0;

/******************************** Configuracion de CR1******************************************/

	//Implementar el codigo para habilitar el clock para un periferico UART dado
	USART_PeriClockControl(pUSARTHandle->pUSARTx,ENABLE);

	//Habilitar el motor de Tx y Rx USART de acuerdo al item de modo de configuracion USART
	if ( pUSARTHandle->USART_Config.USART_Mode == USART_MODE_ONLY_RX)
	{
		//Implementar el codigo para habilitar el campo de bit del receptor
		tempreg |= (1 << USART_CR1_RE);
	}else if (pUSARTHandle->USART_Config.USART_Mode == USART_MODE_ONLY_TX)
	{
		//Implementar el codigo para habilitar el campo de bit del transmisor
		tempreg |= ( 1 << USART_CR1_TE );
	}else if (pUSARTHandle->USART_Config.USART_Mode == USART_MODE_TXRX)
	{
		//Implementar el codigo para habilitar ambos campos de bit tanto del transmisor como receptor
		tempreg |= ( ( 1 << USART_CR1_RE) | ( 1 << USART_CR1_TE) );
	}

	//Implementar el código para configurar el elemento de configuración de longitud de palabra
	tempreg |= pUSARTHandle->USART_Config.USART_WordLength << USART_CR1_M ;

	//Configuracion de campo de bits de control de paridad
	if ( pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_EN_EVEN)
	{
		//Implementar el codigo para habilitar el control de paridad
		tempreg |= ( 1 << USART_CR1_PCE);

		//Implementar el codigo para habilitar la paridad par
		//No se requiere porque por default la paridad par esta seleccionada una vez que se habilita el control de paridad

	}else if (pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_EN_ODD )
	{
		//Implementar el codigo para habilitar la paridad de control
	    tempreg |= ( 1 << USART_CR1_PCE);

	    //Implementar el codigo para habilitar la paridad impar
	    tempreg |= ( 1 << USART_CR1_PS);
	}

	//Programar el registro CR1
	pUSARTHandle->pUSARTx->CR1 = tempreg;

/******************************** Configuracion de CR2******************************************/

	tempreg = 0;

	//Implementar el codigo para configurar el numero de bits de stop insertados durante una transmision de USART frame
	tempreg |= pUSARTHandle->USART_Config.USART_NoOfStopBits << USART_CR2_STOP;

	//Programar el registro CR2
	pUSARTHandle->pUSARTx->CR2 = tempreg;

/******************************** Configuracion de CR3******************************************/

	tempreg=0;

	//Configuracion del control de flujo del hardware USART
	if ( pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_CTS)
	{
		//Implementar el codigo para habilitar el control de flujo de CTS
		tempreg |= ( 1 << USART_CR3_CTSE);

	}else if (pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_RTS)
	{
		//Implementar el codigo para habilitar el control de flujo RTS
		tempreg |= (1 << USART_CR3_RTSE);

	}else if (pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_CTS_RTS)
	{
		//Implementar el codigo para habilitar ambos controles de flujo CTS y RTS
		tempreg |= (1 << USART_CR3_CTSE);
		tempreg |= (1 << USART_CR3_RTSE);
	}

	pUSARTHandle->pUSARTx->CR3 = tempreg;

/******************************** Configuracion de BRR(Registro de Baudrate)******************************************/

	//Implementar el codigo para configurar el baud rate
	USART_SetBaudRate(pUSARTHandle->pUSARTx,pUSARTHandle->USART_Config.USART_Baud);
}



/*********************************************************************
 * @fn      		  - USART_SendData
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              - Resolve all the TODOs

 */
void USART_SendData(USART_Handle_t *pUSARTHandle, uint8_t *pTxBuffer, uint32_t Len)
{

	uint16_t *pdata;

	//Bucle hast que un numero "Len" de bytes fueron transmitidos
	for(uint32_t i = 0 ; i < Len; i++)
	{
		//Implementar el codigo para esperar hasta que el flag TXE sea seteado en el SR
		while(! USART_GetFlagStatus(pUSARTHandle->pUSARTx,USART_FLAG_TXE));

		//Chequear el item de tamaño de palabra USART para 9BIT o 8BIT en un frame
		if(pUSARTHandle->USART_Config.USART_WordLength == USART_WORDLEN_9BITS)
		{
			//Si es 9BIT, cargue el DR con 2bytes enmascarando los bits que no sean los primeros 9 bits
			pdata = (uint16_t*) pTxBuffer;
			pUSARTHandle->pUSARTx->DR = (*pdata & (uint16_t)0x01FF);

			//Chequear USART_ParityControl
			if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
			{
				//Ninguna paridad es usada en esta transferencia. Entones, 9bits de data de usuario seran enviados.
				//Implementar el codigo para incrementar pTxBuffer dos veces
				pTxBuffer++;
				pTxBuffer++;
			}
			else
			{
				//Bit de paridad es usado en esta transferencia. Entonces, los 8bits de dato de usuario seran enviados
				//El 9° bit sera reemplazado por el bit de paridad mediante hardware.
				pTxBuffer++;
			}
		}
		else
		{
			//Este es la transferencia de datos 8Bit
			pUSARTHandle->pUSARTx->DR = (*pTxBuffer  & (uint8_t)0xFF);

			//Implementar el codigo para incrementar la direccion de buffer.
			pTxBuffer++;
		}
	}

	//Implementar el codigo para esperar hasta que la bandera TC sea seteada en el SR
	while( ! USART_GetFlagStatus(pUSARTHandle->pUSARTx,USART_FLAG_TC));
}


/*********************************************************************
 * @fn      		  - USART_ReceiveData
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */

void USART_ReceiveData(USART_Handle_t *pUSARTHandle, uint8_t *pRxBuffer, uint32_t Len)
{
	//Repetir hasta que se transfiera el numero de bytes "Len"
	for(uint32_t i = 0 ; i < Len; i++)
	{
		//Implementar el codigo para esperar hasta que la bandera RXNE sea seteada en el SR
		while(! USART_GetFlagStatus(pUSARTHandle->pUSARTx,USART_FLAG_RXNE));

		//Verificar USART_WordLength para decidir si vamos a recibir 9 bits de datos en una trama o de 8 bits
		if(pUSARTHandle->USART_Config.USART_WordLength == USART_WORDLEN_9BITS)
		{
			//Vamos a recibir 9bit de datos en trama

			//Verificar si estamos usando control USART_ParityControl o no
			if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
			{
				//No se usa paridad. Entonces, todos los 9 bits seran datos de usuario

				//Leer solo los primeros 9 bits. Entonces, enmascarar el DR con 0x01FF
				*((uint16_t*) pRxBuffer) = (pUSARTHandle->pUSARTx->DR  & (uint16_t)0x01FF);

				//Ahora incrementar el pRxBuffer dos veces
				pRxBuffer++;
				pRxBuffer++;
			}
			else
			{
				//La paridad es usada. Entonces 8bits de datos de usuario seran recibidos y 1 bit de paridad
				*pRxBuffer = (pUSARTHandle->pUSARTx->DR  & (uint8_t)0xFF);

				//Incrementar el pRxBuffer
				pRxBuffer++;
			}
		}
		else
		{
			//Ahora vamos a revibir 8bits de datos en la trama

			//Verificar si estamos usando control USART_ParityControl o no
			if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
			{
				//Ninguna paridad usada, entonces todos los 8bits seran datos de usuario
				//Leer los 8 bits desde DR
				*pRxBuffer = (uint8_t) (pUSARTHandle->pUSARTx->DR  & (uint8_t)0xFF);
			}

			else
			{
				//Paridad usada, entonces, 7bits seran datos de usuario y 1 bit de paridad

				//Solo lectura 7 bits, por eso se enmascara el DR con 0x7F
				*pRxBuffer = (uint8_t) (pUSARTHandle->pUSARTx->DR  & (uint8_t)0x7F);

			}

			//Se incrementa el pRxBuffer
			pRxBuffer++;
		}
	}

}

/*********************************************************************
 * @fn      		  - USART_SendDataWithIT
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */
uint8_t USART_SendDataIT(USART_Handle_t *pUSARTHandle,uint8_t *pTxBuffer, uint32_t Len)
{
	uint8_t txstate = pUSARTHandle->TxBusyState;

	if(txstate != USART_BUSY_IN_TX)
	{
		pUSARTHandle->TxLen = Len;
		pUSARTHandle->pTxBuffer = pTxBuffer;
		pUSARTHandle->TxBusyState = USART_BUSY_IN_TX;

		//Implementar el codigo para habilitar la interrupcion para TXE
		pUSARTHandle->pUSARTx->CR1 |= ( 1 << USART_CR1_TXEIE);

		//Implementar el codigo para habilitar la interrupcion para TC
		pUSARTHandle->pUSARTx->CR1 |= ( 1 << USART_CR1_TCIE);

	}

	return txstate;
}


/*********************************************************************
 * @fn      		  - USART_ReceiveDataIT
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */
uint8_t USART_ReceiveDataIT(USART_Handle_t *pUSARTHandle,uint8_t *pRxBuffer, uint32_t Len)
{
	uint8_t rxstate = pUSARTHandle->RxBusyState;

	if(rxstate != USART_BUSY_IN_RX)
	{
		pUSARTHandle->RxLen = Len;
		pUSARTHandle->pRxBuffer = pRxBuffer;
		pUSARTHandle->RxBusyState = USART_BUSY_IN_RX;

		//Implementar el codigo para habilitar la interrupcion para RXNE
		pUSARTHandle->pUSARTx->CR1 |= ( 1 << USART_CR1_RXNEIE);

	}

	return rxstate;

}

/*********************************************************************
 * @fn      		  - USART_IRQInterruptConfig
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */
void USART_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{

	if(EnorDi == ENABLE)
	{
		if(IRQNumber <= 31)
		{
			//Programar el registro ISER0
			*NVIC_ISER0 |= ( 1 << IRQNumber );

		}else if(IRQNumber > 31 && IRQNumber < 64 ) //32 to 63
		{
			//Programar el registro ISER1
			*NVIC_ISER1 |= ( 1 << (IRQNumber % 32) );
		}
		else if(IRQNumber >= 64 && IRQNumber < 96 )
		{
			//PRogramar el registro ISER2 //64 a 95
			*NVIC_ISER3 |= ( 1 << (IRQNumber % 64) );
		}
	}else
	{
		if(IRQNumber <= 31)
		{
			//Programar el registro ICER0
			*NVIC_ICER0 |= ( 1 << IRQNumber );
		}else if(IRQNumber > 31 && IRQNumber < 64 )
		{
			//Programar el registro ICER1
			*NVIC_ICER1 |= ( 1 << (IRQNumber % 32) );
		}
		else if(IRQNumber >= 6 && IRQNumber < 96 )
		{
			//Programar el registro ICER2
			*NVIC_ICER3 |= ( 1 << (IRQNumber % 64) );
		}
	}

}

/*********************************************************************
 * @fn      		  - USART_IRQPriorityConfig
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */
void USART_IRQPriorityConfig(uint8_t IRQNumber,uint32_t IRQPriority)
{
	//1. Primero encontrar el registro ipr
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section  = IRQNumber %4 ;

	uint8_t shift_amount = ( 8 * iprx_section) + ( 8 - NO_PR_BITS_IMPLEMENTED) ;

	*(  NVIC_PR_BASE_ADDR + iprx ) |=  ( IRQPriority << shift_amount );

}


/*********************************************************************
 * @fn      		  - USART_IRQHandler
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */
void USART_IRQHandling(USART_Handle_t *pUSARTHandle)
{

	uint32_t temp1 , temp2, temp3;

	uint16_t *pdata;

	/*************************Verificar la bandera TC********************************************/

	//Implementar el codigo para verificar el estado del bit TC en el SR
	temp1 = pUSARTHandle->pUSARTx->SR & ( 1 << USART_SR_TC);

	//Implementar el codigo para verificar el estado de bit TCEIE
	temp2 = pUSARTHandle->pUSARTx->CR1 & ( 1 << USART_CR1_TCIE);

	if(temp1 && temp2 )
	{
		//Esta es la interrupcion causada por TC
		//Cerrar la transmision y llamar a la aplicacion de callback si TxLen es cero
		if ( pUSARTHandle->TxBusyState == USART_BUSY_IN_TX)
		{
			//Verificar el TxLen. Si es cero entonces se cierra la transmision de datos
			if(! pUSARTHandle->TxLen )
			{
				//Implementar el codigo para limpiar la bandera TC
				pUSARTHandle->pUSARTx->SR &= ~( 1 << USART_SR_TC);

				//Implementar el codigo para limpiar el bit de control TCIE

				//Resetear el estado de la aplicacion
				pUSARTHandle->TxBusyState = USART_READY;

				//Resetear la direccion de buffer a NULL
				pUSARTHandle->pTxBuffer = NULL;

				//Resetear la longitud a cero
				pUSARTHandle->TxLen = 0;

				//Llamar al call back de aplicacion si hay un evento USART_TX_CMPLT
				USART_ApplicationEventCallback(pUSARTHandle,USART_EVENT_TX_CMPLT);
			}
		}
	}

	/*************************Verificar la bandera TXE********************************************/

	//Implementar el codigo para verificar el estado del bit TXE en el SR
	temp1 = pUSARTHandle->pUSARTx->SR & ( 1 << USART_SR_TXE);

	//Implementar el codigo para verificar el estado del bit TXEIE en CR1
	temp2 = pUSARTHandle->pUSARTx->CR1 & ( 1 << USART_CR1_TXEIE);


	if(temp1 && temp2 )
	{
		//Esta interrupcion es debido a TXE
		if(pUSARTHandle->TxBusyState == USART_BUSY_IN_TX)
		{
			//Seguir enviando datos hasta que TxLen llegue a cero
			if(pUSARTHandle->TxLen > 0)
			{
				//Verificar el USART_WordLength item para 9BIT o 8BIT en un frame
				if(pUSARTHandle->USART_Config.USART_WordLength == USART_WORDLEN_9BITS)
				{
					//Si 9BIT se cargan en DR con 2 bytes se enmascara los primeros 9 bits
					pdata = (uint16_t*) pUSARTHandle->pTxBuffer;
					pUSARTHandle->pUSARTx->DR = (*pdata & (uint16_t)0x01FF);

					//Verificar el USART_ParityControl
					if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
					{
						//Ninguna paridad es usada en esta transferencia, entonces 9bits de usuario son usados para ser enviados
						//Implementar el codigo para incrementar el pTxBuffer dos veces
						pUSARTHandle->pTxBuffer++;
						pUSARTHandle->pTxBuffer++;
						pUSARTHandle->TxLen-=2;
					}
					else
					{
						//El bit de paridad es usado en esta transferencia, entonces 8bits de datos de usuario seran enviados
						//El noveno bit sera reemplazado por el de paridad mediante hardware
						pUSARTHandle->pTxBuffer++;
						pUSARTHandle->TxLen-=1;
					}
				}
				else
				{
					//Este es el 8bit de transferencia de datos
					pUSARTHandle->pUSARTx->DR = (*pUSARTHandle->pTxBuffer  & (uint8_t)0xFF);

					//Implementar el codigo para incrementear la direccion de buffer
					pUSARTHandle->pTxBuffer++;
					pUSARTHandle->TxLen-=1;
				}

			}
			if (pUSARTHandle->TxLen == 0 )
			{
				//TxLen es cero
				//Implementar el codigo para limpiar el bit TXEIE (deshabilitando interrupcion para la bandera TXE)
				pUSARTHandle->pUSARTx->CR1 &= ~( 1 << USART_CR1_TXEIE);
			}
		}
	}

	/*************************Verificar la bandera RXNE********************************************/

	temp1 = pUSARTHandle->pUSARTx->SR & ( 1 << USART_SR_RXNE);
	temp2 = pUSARTHandle->pUSARTx->CR1 & ( 1 << USART_CR1_RXNEIE);


	if(temp1 && temp2 )
	{
		//Esta interrupcion es debido a rxne
		if(pUSARTHandle->RxBusyState == USART_BUSY_IN_RX)
		{
			if(pUSARTHandle->RxLen > 0)
			{
				//Verificar el USART_WordLength para decidir que cantidad de datos vamos a recibir si 9 bits o 8 en un frame
				if(pUSARTHandle->USART_Config.USART_WordLength == USART_WORDLEN_9BITS)
				{
					//Ahora vamos a revibir 9bits de datos en un frame
					//Ahora verificamos que estamos usando control USART_ParityControl o no
					if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
					{
						//Ninguna paridad es usada, entonces todos los 9 bits seran usados para datos de usuario

						//Leer solo los primeros 9 bits para enmascarar el DR con 0x01FF
						*((uint16_t*) pUSARTHandle->pRxBuffer) = (pUSARTHandle->pUSARTx->DR  & (uint16_t)0x01FF);

						//Ahora incrementamos el pRxBuffer dos veces
						pUSARTHandle->pRxBuffer++;
						pUSARTHandle->pRxBuffer++;
						pUSARTHandle->RxLen-=2;
					}
					else
					{
						//La paridad es usada, entonces 8 bits son dedatos y 1 de paridad
						 *pUSARTHandle->pRxBuffer = (pUSARTHandle->pUSARTx->DR  & (uint8_t)0xFF);
						 pUSARTHandle->pRxBuffer++;
						 pUSARTHandle->RxLen-=1;
					}
				}
				else
				{
					//Ahora vamos a recibir 8bits de datos en un frame
					//Ahora verificamos si estamos usando el control USART_ParityControl o no
					if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
					{
						//Ninugna paridad es usada, entonces todos los 8 bits seran usados como datos de usuario
						//Leer 8 bits desde DR
						*pUSARTHandle->pRxBuffer = (uint8_t) (pUSARTHandle->pUSARTx->DR  & (uint8_t)0xFF);

					}

					else
					{
						//Parity is used, so , 7 bits will be of user data and 1 bit is parity

						//read only 7 bits , hence mask the DR with 0X7F
						 *pUSARTHandle->pRxBuffer = (uint8_t) (pUSARTHandle->pUSARTx->DR  & (uint8_t)0x7F);

					}

					//Ahora, incrementar el pRxBuffe
					pUSARTHandle->pRxBuffer++;
					 pUSARTHandle->RxLen-=1;
				}


			}//if of >0

			if(! pUSARTHandle->RxLen)
			{
				//Deshabilitado para RXNE
				pUSARTHandle->pUSARTx->CR1 &= ~( 1 << USART_CR1_RXNEIE );
				pUSARTHandle->RxBusyState = USART_READY;
				USART_ApplicationEventCallback(pUSARTHandle,USART_EVENT_RX_CMPLT);
			}
		}
	}


	/*************************Verificar para la bandera CTS********************************************/
	//Nota: El feature CTS no es aplicable para UART4 y UART5
	//Implementar el codigo para verificar el estado del bit CTS en el SR
	temp1 = pUSARTHandle->pUSARTx->SR & ( 1 << USART_SR_CTS);

	//Implementar el codigo para verificar el estado de CTSE bit en CR1
	temp2 = pUSARTHandle->pUSARTx->CR3 & ( 1 << USART_CR3_CTSE);

	//Implementar el codigo para verificar el estado de CTSIE bit en CR3 (Este bit no esta disponible en UART4 y UART5)
	temp3 = pUSARTHandle->pUSARTx->CR3 & ( 1 << USART_CR3_CTSIE);


	if(temp1  && temp2 )
	{
		//Implementar el codigo para limpiar el flag CTS en SR
		pUSARTHandle->pUSARTx->SR &=  ~( 1 << USART_SR_CTS);

		//Esta interrupcion es debido a cts
		USART_ApplicationEventCallback(pUSARTHandle,USART_EVENT_CTS);
	}

	/*************************Verificar la deteccion de bandera IDLE********************************************/

	//Implementar el codigo para verificar el estado de la bandera IDLE en el SR
	temp1 = pUSARTHandle->pUSARTx->SR & ( 1 << USART_SR_IDLE);

	//Implementar el codigo para verificar el estado de bit de IDLEIE en CR1
	temp2 = pUSARTHandle->pUSARTx->CR1 & ( 1 << USART_CR1_IDLEIE);


	if(temp1 && temp2)
	{
		//Implementar el codigo para limpiar la bandera IDLE. Referirse a RM para entender la secuencia de limpiado
		temp1 = pUSARTHandle->pUSARTx->SR &= ~( 1 << USART_SR_IDLE);

		//Esta interrupcion es debido a idle
		USART_ApplicationEventCallback(pUSARTHandle,USART_EVENT_IDLE);
	}

	/*************************Verificar la deteccion de bandera Overrun********************************************/

	//Implementar el codigo para chequear el estado de ORE flag en el SR
	temp1 = pUSARTHandle->pUSARTx->SR & USART_SR_ORE;

	//Implementar el codigo para verificar el estad RXNEIE bit en el CR1
	temp2 = pUSARTHandle->pUSARTx->CR1 & USART_CR1_RXNEIE;


	if(temp1  && temp2 )
	{
		//No se necesita limpiar la bandera ORE aqui, en cambio se da un API para limpiar la bandera ORE flag.

		//Esta interrupcion es debida al error Overrun
		USART_ApplicationEventCallback(pUSARTHandle,USART_ERR_ORE);
	}



	/*************************Verificar la bandera de error********************************************/

	//Bandera de ruido, error overrun y error de trama en comunicacion multibuffer
	//El siguiente codigo sera ejecutado si solo el modo multibuffer es usado

	temp2 =  pUSARTHandle->pUSARTx->CR3 & ( 1 << USART_CR3_EIE) ;

	if(temp2 )
	{
		temp1 = pUSARTHandle->pUSARTx->SR;
		if(temp1 & ( 1 << USART_SR_FE))
		{
			/*
				Este bit es seteado mediante hardware cuando hay una desincronizacion, a causa de ecesivo ruido o un caracter roto
				es detectado. Es limpiado mediante una secuencia de software (una lectura al registro USART_SR seguido por una lectura al registro USART_DR)
			*/
			USART_ApplicationEventCallback(pUSARTHandle,USART_ERR_FE);
		}

		if(temp1 & ( 1 << USART_SR_NF) )
		{
			/*
			 	Este bit es establecido mediante hardware cuando el ruido es detectado sobre una trama recibida. Este es limpiado mediante
			 	una secuencia de hardware (una lectura al registro USART_SR seguido de una lectura al registro USART_DR)
			*/
			USART_ApplicationEventCallback(pUSARTHandle,USART_ERR_NE);
		}

		if(temp1 & ( 1 << USART_SR_ORE) )
		{
			USART_ApplicationEventCallback(pUSARTHandle,USART_ERR_ORE);
		}
	}
}


/*********************************************************************
 * @fn      		  - USART_SetBaudRate
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -  Resolve all the TODOs

 */
void USART_SetBaudRate(USART_RegDef_t *pUSARTx, uint32_t BaudRate)
{

	//Variable para mantener el APB clock
	uint32_t PCLKx;

	uint32_t usartdiv;

	//Variables para mantener los valores de mantisa y fraccion
	uint32_t M_part,F_part;

	uint32_t tempreg=0;

	//Obtener el valor del clock del bus APB en una variable PCLKx
	if(pUSARTx == USART1 || pUSARTx == USART6)
	{
		//USART1 y USART8 son mantenidos por el bus APB2
		PCLKx = RCC_GetPCLK2Value();
	}else
	{
	   PCLKx = RCC_GetPCLK1Value();
	}

	//Verificar para OVER8 la configuracion de bit
	if(pUSARTx->CR1 & (1 << USART_CR1_OVER8))
	{
		//OVER = 1, sobremuestreo por 8
	   usartdiv = ((25 * PCLKx) / (2 *BaudRate));
	}else
	{
	  //Sobremuestreo por 16
		usartdiv = ((25 * PCLKx) / (4 *BaudRate));
	}

	//Calcular la mantisa
	M_part = usartdiv/100;

	//Colocar la mantisa en la apropiada posicion de bit. referido a USART_BRR
	tempreg |= M_part << 4;

	//Extraer la parte de fraccion
	F_part = (usartdiv - (M_part * 100));

	//Calcular el final fraccional
	if(pUSARTx->CR1 & ( 1 << USART_CR1_OVER8))
	{
		//OVER8 = 1, sobremuestreo por 8
		F_part = ((( F_part * 8)+ 50) / 100)& ((uint8_t)0x07);

	}else
	{
	   //Sobremuestreo por 16
	   F_part = ((( F_part * 16)+ 50) / 100) & ((uint8_t)0x0F);

	}

	//Colocar la parte fraccional en una apropiada posicion de bit, referido a USART_BRR
	tempreg |= F_part;

	//Copiar el valor de tempreg en el registro BRR
	pUSARTx->BRR = tempreg;
}


/*********************************************************************
 * @fn      		  - USART_PeripheralControl
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */
void USART_PeripheralControl(USART_RegDef_t *pUSARTx, uint8_t Cmd)
{
	if(Cmd == ENABLE)
	{
		pUSARTx->CR1 |= (1 << 13);
	}else
	{
		pUSARTx->CR1 &= ~(1 << 13);
	}

}

/*********************************************************************
 * @fn      		  - USART_GetFlagStatus
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */
uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSARTx, uint8_t StatusFlagName)
{
    if(pUSARTx->SR & StatusFlagName)
    {
    	return SET;
    }

   return RESET;
}


/*********************************************************************
 * @fn      		  - USART_ClearFlag
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              - Applicable to only USART_CTS_FLAG , USART_LBD_FLAG
 * USART_TC_FLAG,USART_TC_FLAG flags
 *

 */

void USART_ClearFlag(USART_RegDef_t *pUSARTx, uint16_t StatusFlagName)
{
	pUSARTx->SR &= ~( StatusFlagName);

}


/*********************************************************************
 * @fn      		  - USART_ApplicationEventCallback
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */
__attribute__((weak)) void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle,uint8_t event)
{

}

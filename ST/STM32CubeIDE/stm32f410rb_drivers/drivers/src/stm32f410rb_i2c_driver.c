/*
 * stm32f410rb_i2c_driver.c
 *
 *  Created on: 2 ago. 2021
 *      Author: simon
 */
#include <stm32f410rb.h>

static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx);
static void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx,uint8_t SlaveAddr);
static void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx,uint8_t SlaveAddr);
static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle);

static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx){
	pI2Cx->CR1 |= (1 << I2C_CR1_START);
}

static void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx,uint8_t SlaveAddr){
	SlaveAddr = SlaveAddr << 1;
	SlaveAddr &= ~(1);		//SlaveAddr es la direccion de esclavo + r/nw bit = 0
	pI2Cx->DR = SlaveAddr;
}

static void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx,uint8_t SlaveAddr){
	SlaveAddr = SlaveAddr << 1;
	SlaveAddr |= 1;		//SlaveAddr es la direccion de esclavo + r/nw bit = 1
	pI2Cx->DR = SlaveAddr;
}

static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle){
	uint32_t dummy_read;
	//Chequear para el modo de dispositivo
	if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL)){
		//El dispositivo esta en el modo maestro
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX){
			if(pI2CHandle->RxSize == 1){
				//Deshabilitar el ACK
				I2C_ManageAcking(pI2CHandle->pI2Cx, DISABLE);

				//Limpiar el flag ADDR (Leer SR1, Leer SR2)
				dummy_read = pI2CHandle->pI2Cx->SR1;
				dummy_read = pI2CHandle->pI2Cx->SR2;
				(void)dummy_read;
			}
		}else{
			//Limpiar el flag ADDR (Leer SR1, Leer SR2)
			dummy_read = pI2CHandle->pI2Cx->SR1;
			dummy_read = pI2CHandle->pI2Cx->SR2;
			(void)dummy_read;
		}
	}else{
		//El dispositivo esta en el modo esclavo
		//Limpiar el flag ADDR (Leer SR1, Leer SR2)
		dummy_read = pI2CHandle->pI2Cx->SR1;
		dummy_read = pI2CHandle->pI2Cx->SR2;
		(void)dummy_read;
	}
}

void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx){
	pI2Cx->CR1 |= (1 << I2C_CR1_STOP);
}

/*********************************************************************
 * @fn      		  - I2C_EnableDisableCallbackEvents
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
void I2C_EnableDisableCallbackEvents(I2C_RegDef_t *pI2Cx,uint8_t EnorDi){
	if(EnorDi == ENABLE){
		pI2Cx->CR2 |= (1 << I2C_CR2_ITEVTEN);
		pI2Cx->CR2 |= (1 << I2C_CR2_ITBUFEN);
		pI2Cx->CR2 |= (1 << I2C_CR2_ITERREN);
	}else{
		pI2Cx->CR2 &= ~(1 << I2C_CR2_ITEVTEN);
		pI2Cx->CR2 &= ~(1 << I2C_CR2_ITBUFEN);
		pI2Cx->CR2 &= ~(1 << I2C_CR2_ITERREN);
	}
}


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
void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi){
	if(EnorDi == ENABLE){
		if(pI2Cx == I2C1){
			I2C1_PCLK_EN();
		}
		else if(pI2Cx == I2C2){
			I2C2_PCLK_EN();
		}
		else if (pI2Cx == I2C4){
			I2C4_PCLK_EN();
		}
	}
}


/*********************************************************************
 * @fn      		  - I2C_Init
 *
 * @brief             -	Configura los registros del periferico I2C
 *
 * @param[in]         -	Puntero a periferico
 *
 * @return            -
 *
 * @Note              -

 */
void I2C_Init(I2C_Handle_t *pI2CHandle){
	uint32_t tempreg = 0;

	//Habilitar el clock del periferico I2Cx
	I2C_PeriClockControl(pI2CHandle->pI2Cx,ENABLE);

	//ack control bit
	tempreg |= pI2CHandle->I2C_Config.I2C_AckControl << 10;
	pI2CHandle->pI2Cx->CR1 = tempreg;

	//Comfigurar el campo de FREQ de CR2
	tempreg = 0;
	tempreg |= RCC_GetPCLK1Value() / 1000000U;
	pI2CHandle->pI2Cx->CR2 = (tempreg & 0x3F);

	//Programar la direccion del dispositivo en caso que se comporte como esclavo
	tempreg = 0;
	tempreg |= pI2CHandle->I2C_Config.I2C_DeviceAddress << 1;
	tempreg |= (1 << 14);
	pI2CHandle->pI2Cx->OAR1 = tempreg;

	//Calculo CCR
	uint16_t ccr_value = 0;
	tempreg = 0;
	if(pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM){
		//Modo en standard mode
		ccr_value = (RCC_GetPCLK1Value() / (2 * pI2CHandle->I2C_Config.I2C_SCLSpeed));
		tempreg |= (ccr_value & 0xFFF);
	}else{
		//Mode is fast mode
		tempreg |= (1 << 15);
		tempreg |= (pI2CHandle->I2C_Config.I2C_FMDutyCycle << 14);
		if(pI2CHandle->I2C_Config.I2C_FMDutyCycle == I2C_FM_DUTY_2){
			ccr_value = (RCC_GetPCLK1Value() / (3 * pI2CHandle->I2C_Config.I2C_SCLSpeed));
		}else{
			ccr_value = (RCC_GetPCLK1Value() / (25 * pI2CHandle->I2C_Config.I2C_SCLSpeed));
		}
		tempreg |= (ccr_value & 0xFFF);
	}
	pI2CHandle->pI2Cx->CCR = tempreg;

	//Configuracion TRISE
	if(pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM){
		//Modo es standard mode
		tempreg = (RCC_GetPCLK1Value() / 1000000U) + 1;
	}else{
		//Modo es fast mode
		tempreg = (RCC_GetPCLK1Value() * 300) / 1000000000U + 1;
	}
	pI2CHandle->pI2Cx->TRISE = (tempreg & 0x3F);
}


/*********************************************************************
 * @fn      		  - I2C_DeInit
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
void I2C_DeInit(I2C_RegDef_t *pI2Cx){

}

/*********************************************************************
 * @fn      		  - I2C_GetFlagStatus
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
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagName){
	if(pI2Cx->SR1 & FlagName){
		return FLAG_SET;
	}
	return FLAG_RESET;
}

/*********************************************************************
 * @fn      		  - I2C_IRQInterruptConfig
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
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{

	if(EnorDi == ENABLE)
	{
		if(IRQNumber <= 31)
		{
			//program ISER0 register
			*NVIC_ISER0 |= ( 1 << IRQNumber );

		}else if(IRQNumber > 31 && IRQNumber < 64 ) //32 to 63
		{
			//program ISER1 register
			*NVIC_ISER1 |= ( 1 << (IRQNumber % 32) );
		}
		else if(IRQNumber >= 64 && IRQNumber < 96 )
		{
			//program ISER2 register //64 to 95
			*NVIC_ISER3 |= ( 1 << (IRQNumber % 64) );
		}
	}else
	{
		if(IRQNumber <= 31)
		{
			//program ICER0 register
			*NVIC_ICER0 |= ( 1 << IRQNumber );
		}else if(IRQNumber > 31 && IRQNumber < 64 )
		{
			//program ICER1 register
			*NVIC_ICER1 |= ( 1 << (IRQNumber % 32) );
		}
		else if(IRQNumber >= 6 && IRQNumber < 96 )
		{
			//program ICER2 register
			*NVIC_ICER3 |= ( 1 << (IRQNumber % 64) );
		}
	}

}


/*********************************************************************
 * @fn      		  - I2C_IRQPriorityConfig
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
void I2C_IRQPriorityConfig(uint8_t IRQNumber,uint32_t IRQPriority)
{
	//1. first lets find out the ipr register
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section  = IRQNumber %4 ;

	uint8_t shift_amount = ( 8 * iprx_section) + ( 8 - NO_PR_BITS_IMPLEMENTED) ;

	*(  NVIC_PR_BASE_ADDR + iprx ) |=  ( IRQPriority << shift_amount );

}


/*********************************************************************
 * @fn      		  - I2C_MasterSendData
 *
 * @brief             -	Proceso de enviado de datos por el master
 *
 * @param[in]         -	Manipulador de I2C
 * @param[in]         -	Buffer Tx
 * @param[in]         -	Longitud de dato
 * @param[in]         -	Direccion de esclavo
 *
 * @return            -
 *
 * @Note              -

 */
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr){
	//1. Genera la condicion de START
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	//2. Confirmar que la generacion del comienzo es completada mediante el
	//chequeo del flag SB en el SR1.
	//Nota: Antes que SB sea limpiada SCL sera estirada (pulled down)
	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB));

	//3. Enviar la direccion del esclavo con r/nw bit seteado a w(0) (total 8 bits)
	I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, SlaveAddr);

	//4. Confirmar que la direccion es completada mediante el chequeo del
	//flag ADDR en SR1
	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR));

	//5. Limpiar el flag ADDR de acuerdo a la secuencia de software
	//Nota: Hasta que ADDR sea limpiada SCL sera estirada (pulled to low)
	I2C_ClearADDRFlag(pI2CHandle);

	//6. Enviar datos antes que len sea 0
	while(Len > 0){
		while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE));	//Esperar hasta que TXE es seteado
		pI2CHandle->pI2Cx->DR = *pTxBuffer;
		pTxBuffer++;
		Len--;
	}

	//7. Cuando Len sea cero esperar para TXE = 1 y BTF = 1 antes de generar la condicion de STOP
	//Nota: TXE=1, BTF=1, significa que ambos SR y DR estan vacios y la siguiente transmision deberia comenzar cuando
	//BTF=1 SCL sera estirado (pulled down)
	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE));

	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_BTF));

	//8. Genera la condicion de STOP y el maestro no necesita esperar para completar la condicion de STOP
	//Nota: Generar STOP, automaticamente limpia el BTF
	if(Sr == I2C_DISABLE_SR)
		I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
}


/*********************************************************************
 * @fn      		  - I2C_MasterReceiveData
 *
 * @brief             -	Proceso de recibido de datos por el master
 *
 * @param[in]         -	Manipulador de I2C
 * @param[in]         -	Buffer Rx
 * @param[in]         -	Longitud de datos
 * @param[in]         -	Direccion de esclavo
 *
 * @return            -
 *
 * @Note              -

 */
void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint8_t Len, uint8_t SlaveAddr, uint8_t Sr){
	//1. Generar la condicion de START
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	//2. Confirmar que la generacion de START es completada mediante la bandera SB en el SR1
	//Nota: Hasta que el SB sea limpiado el SCL debe estirarse (pulled to LOW)
	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB));

	//3. Enviar la direccion de esclavo con el bit r/nw bit seteado a R(1) (total de 8 bits)
	I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, SlaveAddr);

	//4. Esperar hasta que la fase de direccion sea completada mediante el chequeo del flag ADDR en el SR1
	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR));

	//5. Proceder a leer solo 1 byte desde el esclavo
	if(Len == 1){
		//Deshabilitar Acking
		I2C_ManageAcking(pI2CHandle->pI2Cx,I2C_ACK_DISABLE);

		//Limpiar el flag ADDR
		I2C_ClearADDRFlag(pI2CHandle);

		//Esperar hasta que RXNE sea 1
		while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE));

		//Generar la condicion de STOP
		if(Sr == I2C_DISABLE_SR)
			I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

		//Leer datos del buffer
		*pRxBuffer = pI2CHandle->pI2Cx->DR;
	}

	//Procedimiento para leer datos desde el esclavo para longitudes > 1
	if(Len > 1){
		//Limpiar el ADDR flag
		I2C_ClearADDRFlag(pI2CHandle);

		//Leer datos hasta que Len sea cero
		for(uint32_t i = Len; i > 0; i--){
			//Esperar hasta que RXNE sea 1
			while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE));


			if(i == 2){	//Si faltan los dos ultimos bytes
				//Limpiar el ack bit
				I2C_ManageAcking(pI2CHandle->pI2Cx,I2C_ACK_DISABLE);

				//Generar la condicion de STOP
				if(Sr == I2C_DISABLE_SR)
					I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
			}

			//Leer datos del buffer
			*pRxBuffer = pI2CHandle->pI2Cx->DR;

			//Incrementar la direccion del buffer
			pRxBuffer++;
		}
	}

	//Re-habilitar el acking
	if(pI2CHandle->I2C_Config.I2C_AckControl == I2C_ACK_ENABLE){
		I2C_ManageAcking(pI2CHandle->pI2Cx,I2C_ACK_ENABLE);
	}
}

/*********************************************************************
 * @fn      		  - I2C_MasterSendDataIT
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -  Complete the below code . Also include the function prototype in header file

 */
uint8_t  I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pTxBuffer, uint32_t Len,uint8_t SlaveAddr,uint8_t Sr)
{

	uint8_t busystate = pI2CHandle->TxRxState;

	if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pTxBuffer = pTxBuffer;
		pI2CHandle->TxLen = Len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_TX;
		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->Sr = Sr;

		//Implementar el código para generar la condición de START
		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

		//Implementar el código para habilitar el bit de control ITBUFEN
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN);

		//Implementar el código para habilitar el bit de control ITEVFEN
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITEVTEN);

		//Implementar el código para habilitar el bit de control ITERREN
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITERREN);
	}

	return busystate;

}


/*********************************************************************
 * @fn      		  - I2C_MasterReceiveDataIT
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              - Complete the below code . Also include the fn prototype in header file

 */
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pRxBuffer, uint8_t Len, uint8_t SlaveAddr,uint8_t Sr)
{
	uint8_t busystate = pI2CHandle->TxRxState;

	if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pRxBuffer = pRxBuffer;
		pI2CHandle->RxLen = Len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_RX;
		pI2CHandle->RxSize = Len;
		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->Sr = Sr;

		//Implementar código para generar la condicion de START
		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

		//Implementar código para habilitar el bit de control ITBUFEN
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN);

		//Implementar el código para habilitar el bit de control ITEVFEN
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITEVTEN);

		//Implementar el código para habilitar el bit de control ITERREN
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITERREN);
	}

	return busystate;
}


/*********************************************************************
 * @fn      		  - I2C_CloseReceiveData
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -  Interrupt handling for different I2C events (refer SR1)

 */
void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle){
	//Implementar el codigo para deshabilitar el bit de control ITBUFEN
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITBUFEN);

	//Implementar el codigo para deshabilitar el bit de control ITEVFEN
	pI2CHandle->pI2Cx->CR2 & ~(1 << I2C_CR2_ITEVTEN);

	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pRxBuffer = NULL;
	pI2CHandle->RxLen = 0;
	pI2CHandle->RxSize = 0;

	if(pI2CHandle->I2C_Config.I2C_AckControl == I2C_ACK_ENABLE){
		I2C_ManageAcking(pI2CHandle->pI2Cx, ENABLE);
	}
}

/*********************************************************************
 * @fn      		  - I2C_CloseSendData
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -  Interrupt handling for different I2C events (refer SR1)

 */
void I2C_CloseSendData(I2C_Handle_t *pI2CHandle){
	//Implementar el codigo para deshabilitar el bit de control ITBUFEN
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITBUFEN);

	//Implementar el codigo para deshabilitar el bit de control ITEVFEN
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITEVTEN);

	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pTxBuffer = NULL;
	pI2CHandle->RxLen = 0;
}


static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle){
	if(pI2CHandle->TxLen > 0){
		//1. Cargar los datos en DR
		pI2CHandle->pI2Cx->DR = *(pI2CHandle->pTxBuffer);

		//2. Decrementar TxLen
		pI2CHandle->TxLen--;

		//3. Incrementar la direccion del buffer
		pI2CHandle->pTxBuffer++;
	}
}

static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle){
	//Tenemos que hacer la recepccion de datos
	if(pI2CHandle->RxSize == 1){
		*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
		pI2CHandle->RxLen--;
	}

	if(pI2CHandle->RxSize > 1){
		if(pI2CHandle->RxLen == 2){
			//Limpiar el ACK bit
			I2C_ManageAcking(pI2CHandle->pI2Cx, DISABLE);
		}

		//Leer DR
		*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
		pI2CHandle->pRxBuffer++;
		pI2CHandle->RxLen--;
	}

	if(pI2CHandle->RxLen == 0){
		//Cerrar la recepcion de datos y notificar a la aplicacion

		//1. Generar la condicion de STOP
		if(pI2CHandle->Sr == I2C_ACK_DISABLE)
			I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

		//2. Cerrar el rx I2C
		I2C_CloseReceiveData(pI2CHandle);

		//3. Notificar a la aplicacion
		I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_RX_CMPLT);
	}
}


/*********************************************************************
 * @fn      		  - I2C_SlaveSendData
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -  Interrupt handling for different I2C events (refer SR1)

 */
void I2C_SlaveSendData(I2C_RegDef_t *pI2C, uint8_t data){
	pI2C->DR = data;
}

/*********************************************************************
 * @fn      		  - I2C_SlaveReceiveData
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -  Interrupt handling for different I2C events (refer SR1)

 */
uint8_t I2C_SlaveReceiveData(I2C_RegDef_t *pI2C){
	return (uint8_t) pI2C -> DR;
}

/*********************************************************************
 * @fn      		  - I2C_EV_IRQHandling
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -  Interrupt handling for different I2C events (refer SR1)

 */
void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle)
{
	//Manipulador de interrupcion para ambos modos de maestro esclavo del dispositivo
	uint32_t temp1, temp2, temp3;

	temp1 = pI2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITEVTEN);
	temp2 = pI2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITBUFEN);

	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_SB);
	//1. Manipulador para interrupcion generada por un evento SB.
	// Nota: El flag SB es solo aplicable en el modo maestro
	if(temp1 && temp3){
		//SB flag es seteado
		//Este bloque no se ejecutara en el modo esclavo ya que para el esclavo BTF siempre es cero
		//En este bloque ejecutemos la direccion
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX){
			I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
		}else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX){
			I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
		}
	}

	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_ADDR);
	//2. Manipulador para interrupcion generado por un evento ADDR
	//Nota: Cuando es el modo maestro : La direccion es enviada
	//Nota: Cuando es el modo esclavo : La direccion debe coincidir con su propia direccion
	if(temp1 && temp3){
		//ADDR flag es seteado
		I2C_ClearADDRFlag(pI2CHandle);

	}

	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_BTF);
	//3. Manipulador para interrupcion generada por evento BTF (Byte Transfer Finished)
	if(temp1 && temp3){
		//BTF flag es seteado
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX){
			//Asegurarse que TXE este también seteado
			if(pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_TxE)){
				//BTF y TXE = 1
				if(pI2CHandle->TxLen == 0){
					//1. Generar la condicion de STOP
					if(pI2CHandle->Sr == I2C_DISABLE_SR)
						I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

					//2. Resetear todos los miembros elementos de la estructura de manipulacion
					I2C_CloseSendData(pI2CHandle);

					//3. Notificar a la aplicacion acerca de la transmision completa
					I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_TX_CMPLT);
				}
			}
		}else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX){
			;
		}
	}

	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_STOPF);
	//4. Manipulador para interrupcion generada por un evento STOPF
	// Nota: El flag de deteccion Stop es aplicable solo en el modo esclavo. Para el maestro este flag no sera seteado nunca
	if(temp1 && temp3){
		//STOPF flag es seteado
		//Clear the STOPF (por ej 1) leer SR1 2) Escribir a CR1 )
		pI2CHandle->pI2Cx->CR1 |= 0x0000;

		//Notificar a la aplicacion que un STOP fue detectado
		I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_STOP);
	}

	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_TxE);
	//5. Manipulador para interrupcion generada por un evento TXE
	if(temp1 && temp2 && temp3){
		//Chequear el modo de dispositivo
		if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL)){
			//TxE flag es seteado
			//Tenemos que hacer la transmision de datos
			if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX){
				I2C_MasterHandleTXEInterrupt(pI2CHandle);
			}
		}else{
			//Esclavo
			//Nos aseguramos que el esclavo esta realmente en modo transmision
			if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_TRA)){
				I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_DATA_REQ);
			}
		}
	}

	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_RxNE);
	//6. Manipulador para interrupcion generada por un evento RXNE
	if(temp1 && temp2 && temp3){
		//Chequear el modo de dispositivo
		if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL)){
			//El dispositivo es maestro
			//RxNE flag es seteado
			if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX){
				I2C_MasterHandleRXNEInterrupt(pI2CHandle);
			}
		}else{
			//Esclavo
			//Asegurarse que el esclavo esta en modo recepcion
			if(!(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_TRA))){
				I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_DATA_RCV);
			}
		}
	}
}


/*********************************************************************
 * @fn      		  - I2C_ER_IRQHandling
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              - Complete the code also define these macros in the driver
						header file
						#define I2C_ERROR_BERR  3
						#define I2C_ERROR_ARLO  4
						#define I2C_ERROR_AF    5
						#define I2C_ERROR_OVR   6
						#define I2C_ERROR_TIMEOUT 7

 */

void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle)
{

	uint32_t temp1,temp2;

	//Conocer el estado del bit de control ITERREN en CR2
	temp2 = (pI2CHandle->pI2Cx->CR2) & ( 1 << I2C_CR2_ITERREN);


/***********************Chequear error en bus************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1<< I2C_SR1_BERR);
	if(temp1  && temp2 )
	{
		//Este es un error de bus

		//Implementar el codigo para limpiar el flag de erro en bus
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_BERR);

		//Implementar el codigo para notificar la aplicacion acerca del error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_BERR);
	}

/***********************Verificar error de arbitraje perdido************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_ARLO );
	if(temp1  && temp2)
	{
		//Este es una perdida de error de arbitraje

		//Implementar el codigo para limpiar la bandera de error de perdida de arbitraje
		pI2CHandle->pI2Cx->SR1 &= ~(1 << I2C_SR1_ARLO);

		//Implementar el codigo para notificar a la aplicacion acerca del error
		I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_ARLO);
	}

/***********************Verifique el error de falla de ACK************************************/

	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_AF);
	if(temp1  && temp2)
	{
		//Este es un error de falla por ACK

		//Implementar el codigo para limpiar la bandera de error de falla del ack
		pI2CHandle->pI2Cx->SR1 &= ~(1 << I2C_SR1_AF);

		//Implementar el codigo para notificar a la aplicacion acerca de este error
		I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_AF);
	}

/***********************Compruebe si hay un error de rebasamiento / insuficiencia de datos************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_OVR);
	if(temp1  && temp2)
	{
		//Este es un error de rebasamiento

		//Implementar el codigo para limpiar la bandera de error rebasamiento
		pI2CHandle->pI2Cx->SR1 &= ~(1 << I2C_SR1_OVR);

		//Implementar el codigo para notificar a la aplicacion acerca del error
		I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_OVR);
	}

/***********************Verifique el error de tiempo de espera************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_TIMEOUT);
	if(temp1  && temp2)
	{
		//Este es un error de tiempo de espera

		//Implementar el codigo para limpiar la bandera de error de tiempo de espera
		pI2CHandle->pI2Cx->SR1 &= ~(1 << I2C_SR1_TIMEOUT);

		//Implementar el codigo para notificar a la aplicacion acerca del error
		I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_TIMEOUT);
	}

}


/*********************************************************************
 * @fn      		  - I2C_PeripheralControl
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
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi){
	if(EnOrDi == ENABLE)
		pI2Cx->CR1 |= (1 << I2C_CR1_PE);
	else
		pI2Cx->CR1 &= ~(1 << 0);
}

/*********************************************************************
 * @fn      		  - I2C_ManageAcking
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
void I2C_ManageAcking(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi){
	if(EnOrDi == I2C_ACK_ENABLE){
		//Habilitar el Ack
		pI2Cx->CR1 |= (1 << I2C_CR1_ACK);
	}else{
		//Deshabilitar el Ack
		pI2Cx->CR1 &= ~(1 << I2C_CR1_ACK);
	}
}

/*
 * stm32f410rb_gpio_driver.h
 *
 *  Created on: Jun 23, 2021
 *      Author: simon
 */

#ifndef INC_STM32F410RB_GPIO_DRIVER_H_
#define INC_STM32F410RB_GPIO_DRIVER_H_

#include "stm32f410rb.h"

//Esta es una estructura para configurar el pin GPIO
typedef struct{
	uint8_t GPIO_PinNumber;
	uint8_t GPIO_PinMode;
	uint8_t GPIO_PinSpeed;
	uint8_t GPIO_PinPuPdControl;
	uint8_t GPIO_PinOPType;
	uint8_t GPIO_PinAltFunMode;
}GPIO_PinConfig_t;

//Esta es una estructura para manejar el pin de GPIO
typedef struct{
	//Puntero para mantener la direccion base del periferico GPIO
	GPIO_RegDef_t 		*pGPIOx;	//Esto mantiene la direccion base del puerto GPIO al cual el pin pertenece
	GPIO_PinConfig_t	GPIO_PinConfig;	//Esto mantiene las configuraciones del pin GPIO
}GPIO_Handle_t;

//Modos posibles de pines GPIO
#define GPIO_MODE_INPUT 	0
#define GPIO_MODE_OUTPUT	1
#define GPIO_MODE_ALTFN		2
#define GPIO_MODE_ANALOG	3
#define GPIO_MODE_IT_FT		4	//Modo interrupcion flanco descendente
#define GPIO_MODE_IT_RT		5	//Idem anterior flanco ascendente
#define GPIO_MODE_IT_RFT	6	//Idem anterior ambos flancos

//Tipos posibles de salida en pin GPIO
#define GPIO_OP_TYPE_PP		0	//Push pull
#define GPIO_OP_TYPE_OD		1	//Open drain

//Tipos posibles de velocidad en pin GPIO
#define GPIO_SPEED_LOW		0
#define GPIO_SPEED_MEDIUM	1
#define GPIO_SPEED_FAST		2
#define GPIO_SPEED_HIGH		3

//macros de configuracion GPIO pin pullup y pulldown
#define GPIO_NO_PUPD		0
#define GPIO_PIN_PU			1
#define GPIO_PIN_PD			2

//Numeros de pin GPIO
#define GPIO_PIN_NO_0 		0
#define GPIO_PIN_NO_1		1
#define GPIO_PIN_NO_2		2
#define GPIO_PIN_NO_3		3
#define GPIO_PIN_NO_4		4
#define GPIO_PIN_NO_5		5
#define GPIO_PIN_NO_6		6
#define GPIO_PIN_NO_7		7
#define GPIO_PIN_NO_8		8
#define GPIO_PIN_NO_9		9
#define GPIO_PIN_NO_10		10
#define GPIO_PIN_NO_11		11
#define GPIO_PIN_NO_12		12
#define GPIO_PIN_NO_13		13
#define GPIO_PIN_NO_14		14
#define GPIO_PIN_NO_15		15

/******************************************************************************************
 *								APIs soportadas por este driver
 ******************************************************************************************/

/*
 * Peripheral Clock setup
 */
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi);

/*
 * Init and De-init
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle);
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx);

/*
 * Data read and write
 */
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx);
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value);
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value);
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);


/*
 * IRQ Configuration and ISR handling
 */
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void GPIO_IRQHandling(uint8_t PinNumber);

#endif /* INC_STM32F410RB_GPIO_DRIVER_H_ */

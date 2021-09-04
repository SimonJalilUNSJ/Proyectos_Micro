/*
 * stm32f410rb_rcc_driver.h
 *
 *  Created on: 12 ago. 2021
 *      Author: simon
 */

#ifndef INC_STM32F410RB_RCC_DRIVER_H_
#define INC_STM32F410RB_RCC_DRIVER_H_

#include "stm32f410rb.h"

//Este retorna el valor de clock de APB1
uint32_t RCC_GetPCLK1Value(void);

//Este retorna el valor de clock de APB2
uint32_t RCC_GetPCLK2Value(void);

uint32_t RCC_GetPLLOutputClock(void);

#endif /* INC_STM32F410RB_RCC_DRIVER_H_ */

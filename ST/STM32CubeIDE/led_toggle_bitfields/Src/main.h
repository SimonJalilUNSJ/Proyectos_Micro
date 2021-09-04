/*
 * main.h
 *
 *  Created on: May 3, 2021
 *      Author: simon
 */

#ifndef MAIN_H_
#define MAIN_H_

typedef struct{
	uint32_t gpioa_en	:1;
	uint32_t gpiob_en	:1;
	uint32_t gpioc_en	:1;
	uint32_t reservado	:4;
	uint32_t gpioh_en	:1;
	uint32_t reservado2 :4;
	uint32_t crcen		:1;
	uint32_t reservado3 :8;
	uint32_t dma1_en 	:1;
	uint32_t dma2_en 	:1;
	uint32_t reservado4 :8;
	uint32_t rng_en		:1;
}RCC_AHB1ENR_t;

typedef struct{
	uint32_t pin_0	:2;
	uint32_t pin_1	:2;
	uint32_t pin_2	:2;
	uint32_t pin_3	:2;
	uint32_t pin_4	:2;
	uint32_t pin_5	:2;
	uint32_t pin_6	:2;
	uint32_t pin_7	:2;
	uint32_t pin_8	:2;
	uint32_t pin_9	:2;
	uint32_t pin_10	:2;
	uint32_t pin_11	:2;
	uint32_t pin_12	:2;
	uint32_t pin_13	:2;
	uint32_t pin_14	:2;
	uint32_t pin_15	:2;
}GPIOx_MODE_t;

typedef struct{
	uint32_t pin_0		:1;
	uint32_t pin_1		:1;
	uint32_t pin_2		:1;
	uint32_t pin_3		:1;
	uint32_t pin_4		:1;
	uint32_t pin_5		:1;
	uint32_t pin_6		:1;
	uint32_t pin_7		:1;
	uint32_t pin_8		:1;
	uint32_t pin_9		:1;
	uint32_t pin_10		:1;
	uint32_t pin_11		:1;
	uint32_t pin_12		:1;
	uint32_t pin_13		:1;
	uint32_t pin_14		:1;
	uint32_t pin_15		:1;
	uint32_t reservado 	:16;
}GPIOx_ODR_t;

#endif /* MAIN_H_ */

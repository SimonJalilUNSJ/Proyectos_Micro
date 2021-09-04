/*
 * stm32f410rb.h
 *
 *  Created on: Jun 23, 2021
 *      Author: simon
 */

#ifndef INC_STM32F410RB_H_
#define INC_STM32F410RB_H_

#include <stdint.h>
#include <stddef.h>

/*******Detalles especificos del procesador********/
//Direcciones de registro NVIC ISERx en procesadores ARM Cortex Mx
#define NVIC_ISER0	((volatile uint32_t*)0xE000E100)
#define NVIC_ISER1	((volatile uint32_t*)0xE000E104)
#define NVIC_ISER2	((volatile uint32_t*)0xE000E108)
#define NVIC_ISER3	((volatile uint32_t*)0xE000E10C)
#define NVIC_ISER4	((volatile uint32_t*)0xE000E110)
#define NVIC_ISER5	((volatile uint32_t*)0xE000E114)
#define NVIC_ISER6	((volatile uint32_t*)0xE000E118)
#define NVIC_ISER7	((volatile uint32_t*)0xE000E11C)

//Direcciones de registro NVIC ICERx en procesadores ARM Cortex Mx
#define NVIC_ICER0	((volatile uint32_t*)0XE000E180)
#define NVIC_ICER1	((volatile uint32_t*)0XE000E184)
#define NVIC_ICER2	((volatile uint32_t*)0XE000E188)
#define NVIC_ICER3	((volatile uint32_t*)0XE000E18C)
#define NVIC_ICER4	((volatile uint32_t*)0XE000E190)
#define NVIC_ICER5	((volatile uint32_t*)0XE000E194)
#define NVIC_ICER6	((volatile uint32_t*)0XE000E198)
#define NVIC_ICER7	((volatile uint32_t*)0XE000E19C)

//Calculo de direccion de registro de prioridad del procesador ARM Cortex MX
#define NVIC_PR_BASE_ADDR	((volatile uint32_t*)0xE000E400)

//Numero de bits de prioridad implementados en el registro de prioridad del ARM Cortex Mx
#define NO_PR_BITS_IMPLEMENTED 4

#define FLASH_BASE_ADDR			0x80000000U
#define SRAM1_BASE_ADDR			0x20000000U	//32Kbytes
#define ROME_BASEADDR			0x1FFF0000
#define SRAM 					SRAM1_BASE_ADDR

#define PERIPH_BASE 			0x40000000U
#define APB1PERIPH_BASE			PERIPH_BASE
#define APB2PERIPH_BASE			0x40010000
#define AHB1PERIPH_BASE			0x40020000

/*Direcciones base de perifericos que son manejados por AHB1*/
#define GPIOA_BASEAADR			(AHB1PERIPH_BASE + 0x0000)
#define GPIOB_BASEAADR			(AHB1PERIPH_BASE + 0x0400)
#define GPIOC_BASEADDR			(AHB1PERIPH_BASE + 0x0800)
#define GPIOH_BASEADDR			(AHB1PERIPH_BASE + 0x1C00)
#define RCC_BASEADDR			(AHB1PERIPH_BASE + 0x3800)

/*Direcciones base de perifericos que son manejados por APB1*/
#define I2C1_BASEADDR			(APB1PERIPH_BASE + 0x5400)
#define I2C2_BASEADDR			(APB1PERIPH_BASE + 0x5800)
#define I2C4_BASEADDR 			(APB1PERIPH_BASE + 0x6000)
#define SPI2_BASEADDR			(APB1PERIPH_BASE + 0x3800)
#define USART2_BASEADDR			(APB1PERIPH_BASE + 0x4400)

/*Direcciones base de perifericos que son manejados por APB2*/
#define EXTI_BASEADDR			(APB2PERIPH_BASE + 0x3C00)
#define USART1_BASEADDR			(APB2PERIPH_BASE + 0x1000)
#define USART6_BASEADDR			(APB2PERIPH_BASE + 0x1400)
#define SPI1_BASEADDR			(APB2PERIPH_BASE + 0x3000)
#define SYSCFG_BASEADRR			(APB2PERIPH_BASE + 0x3800)

/*Nota: Registros de los perifericos que son especificos del MCU*/
//Usamos volatile ya que el registro de entrada del GPIO se actualiza con cada
//flanco del bus AHB1
typedef struct{
	volatile uint32_t MODER;		//GPIO port mode register				Address offset: 0x00
	volatile uint32_t OTYPER;		//GPIO port output type register		Address offset: 0x04
	volatile uint32_t OSPEEDER;		//GPIO port output speed register		Address offset: 0x08
	volatile uint32_t PUPDR;		//GPIO port pull-up/pull-down register	Address offset: 0x0C
	volatile uint32_t IDR;			//GPIO port input data register			Address offset: 0x10
	volatile uint32_t ODR;			//GPIO port output data register		Address offset: 0x14
	volatile uint32_t BSRR;			//GPIO port bit set/reset register		Address offset: 0x18
	volatile uint32_t LCKR;			//GPIO port configuration lock register	Address offset: 0x1C
	volatile uint32_t AFR[2];		//GPIO alternate function low (Address offset: 0x20) and high (Address offset: 0x24) register
}GPIO_RegDef_t;

/*Definicion de estructura para registros de perifericos para RCC*/
typedef struct{
	volatile uint32_t CR;			//RCC clock control register									Address offset: 0x00
	volatile uint32_t PLLCFGR;		//RCC PLL configuration register								Address offset: 0x04
	volatile uint32_t CFGR;			//RCC clock configuration register								Address offset: 0x08
	volatile uint32_t CIR;			//RCC clock interrupt register									Address offset: 0x0C
	volatile uint32_t AHB1RSTR;		//RCC AHB1 peripheral reset register							Address offset: 0x10
	uint32_t reservado0[3];			//Reservado 0x14 a 0x1C
	volatile uint32_t APB1RSTR;		//RCC APB1 peripheral reset register							Address offset: 0x20
	volatile uint32_t APB2RSTR;		//RCC APB2 peripheral reset register							Address offset: 0x24
	uint32_t reservado1[2];			//Reservado 0x28 a 0x2C
	volatile uint32_t AHB1ENR;		//RCC AHB1 peripheral clock enable register						Address offset: 0x30
	uint32_t reservado2[3];			//Reservado 0x34 a 0x3C
	volatile uint32_t APB1ENR;		//RCC APB1 peripheral clock enable register						Address offset: 0x40
	volatile uint32_t APB2ENR;		//RCC APB2 peripheral clock enable register						Address offset: 0x44
	uint32_t reservado3[2];			//Reservado 0x48 a 0x4C
	volatile uint32_t AHB1LPENR;	//RCC AHB1 peripheral clock enable in low power mode register	Address offset: 0x50
	uint32_t reservado4[3];			//Reservado 0x54 a 0x5C
	volatile uint32_t APB1LPENR;	//RCC APB1 peripheral clock enable in low power mode register	Address offset: 0x60
	volatile uint32_t APB2LPENR;	//RCC APB2 peripheral clock enabled in low power mode register	Address offset: 0x64
	uint32_t reservado5[2];			//Reservado 0x68 a 0x6C
	volatile uint32_t BDCR;			//RCC Backup domain control register							Address offset: 0x70
	volatile uint32_t CSR;			//RCC clock control & status register							Address offset: 0x74
	uint32_t reservado6[2];			//Reservado 0x78 a 0x7C
	volatile uint32_t SSCGR;		//RCC spread spectrum clock generation register					Address offset: 0x80
	uint32_t reservado7[2];			//Reservado 0x84 a 0x88
	volatile uint32_t DCKCFGR;		//RCC Dedicated Clocks Configuration Register					Address offset: 0x8C
	uint32_t reservado8;			//Reservado 0x90
	volatile uint32_t DCKCFGR2;		//RCC dedicated Clocks Configuration Register 2					Address offset: 0x94
}RCC_RegDef_t;

/*Definicion de estructura para registros de perifericos para EXTI*/
typedef struct{
	volatile uint32_t IMR;			//Address Offset: 0x00
	volatile uint32_t EMR;			//Address Offset: 0x04
	volatile uint32_t RTSR;			//Address Offset: 0x08
	volatile uint32_t FTSR;			//Address Offset: 0x0C
	volatile uint32_t SWIER; 		//Address Offset: 0x10
	volatile uint32_t PR;			//Address Offset: 0x14
}EXTI_RegDef_t;

/*Definicion de estructura para la definicion de registro de periferico para SPI*/
typedef struct{
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t SR;
	volatile uint32_t DR;
	volatile uint32_t CRCPR;
	volatile uint32_t RXCRCR;
	volatile uint32_t TXCRCR;
	volatile uint32_t I2SCFGR;
	volatile uint32_t I2SPR;
}SPI_RegDef_t;

/*Definicion de estructura para registros de perifericos para SYSCFG*/
typedef struct{
	volatile uint32_t MEMRMP;		//Address Offset: 0x00
	volatile uint32_t PMC;			//Address Offset: 0x04
	volatile uint32_t EXTICR[4];	//Address Offset: 0x08 a 0xx14
	volatile uint32_t reservado;	//Address Offset: 0x18
	volatile uint32_t CFGR2;		//Address Offset: 0x1C
	volatile uint32_t CMPCR;		//Address Offset: 0x20
	volatile uint32_t CFGR;			//Address Offset: 0x24
}SYSCFG_RegDef_t;


/*Definicion de estructura para registros de perifericos para I2C*/
typedef struct{
	volatile uint32_t CR1;			//Address offset: 0x00
	volatile uint32_t CR2;			//Address offset: 0x04
	volatile uint32_t OAR1;			//Address offset: 0x08
	volatile uint32_t OAR2;			//Address offset: 0x0C
	volatile uint32_t DR;			//Address offset: 0x10
	volatile uint32_t SR1;			//Address offset: 0x14
	volatile uint32_t SR2;			//Address offset: 0x18
	volatile uint32_t CCR;			//Address offset: 0x1C
	volatile uint32_t TRISE;		//Address offset: 0x20
	volatile uint32_t FLTR;			//Address offset: 0x24
}I2C_RegDef_t;

typedef struct{
	volatile uint32_t SR;
	volatile uint32_t DR;
	volatile uint32_t BRR;
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t CR3;
	volatile uint32_t GTPR;
}USART_RegDef_t;

/*Definiciones de periféricos (Direccioens base de periféricos typecasted a xxx_RegDef_t)*/
#define GPIOA		((GPIO_RegDef_t*)GPIOA_BASEAADR)
#define GPIOB		((GPIO_RegDef_t*)GPIOB_BASEAADR)
#define GPIOC		((GPIO_RegDef_t*)GPIOC_BASEADDR)
#define GPIOH		((GPIO_RegDef_t*)GPIOH_BASEADDR)

#define RCC 		((RCC_RegDef_t*)RCC_BASEADDR)
#define EXTI 		((EXTI_RegDef_t*)EXTI_BASEADDR)
#define SYSCFG 		((SYSCFG_RegDef_t*)SYSCFG_BASEADRR)

#define SPI1		((SPI_RegDef_t*)SPI1_BASEADDR)
#define SPI2		((SPI_RegDef_t*)SPI2_BASEADDR)

#define I2C1		((I2C_RegDef_t*)I2C1_BASEADDR)
#define I2C2 		((I2C_RegDef_t*)I2C2_BASEADDR)
#define I2C4		((I2C_RegDef_t*)I2C4_BASEADDR)

#define USART1 		((USART_RegDef_t*)USART1_BASEADDR)
#define USART2 		((USART_RegDef_t*)USART2_BASEADDR)
#define USART6		((USART_RegDef_t*)USART6_BASEADDR)

/*Macros de habilitacion de clock para perifericos GPIOx*/
#define GPIOA_PCLK_EN()	( RCC->AHB1ENR |= ( 1 << 0 ) )
#define GPIOB_PCLK_EN()	( RCC->AHB1ENR |= ( 1 << 1 ) )
#define GPIOC_PCLK_EN()	( RCC->AHB1ENR |= ( 1 << 2 ) )
#define GPIOH_PCLK_EN()	( RCC->AHB1ENR |= ( 1 << 7 ) )

/*Macros de habilitacion de clock para perifericos I2C*/
#define I2C1_PCLK_EN() 	( RCC->APB1ENR |= ( 1 << 21 ) )
#define I2C2_PCLK_EN() 	( RCC->APB1ENR |= ( 1 << 22 ) )
#define I2C4_PCLK_EN() 	( RCC->APB1ENR |= ( 1 << 24 ) )

/*Macros de habilitacion de clock para perifericos SPIx*/
#define SPI1_PCLK_EN() 	( RCC->APB2ENR |= ( 1 << 12 ) )
#define SPI2_PCLK_EN() 	( RCC->APB1ENR |= ( 1 << 14 ) )

/*Macros de habilitacion de clock para perifericos USART*/
#define USART2_PCLK_EN()	( RCC->APB1ENR |= ( 1 << 17 ) )
#define USART1_PCLK_EN()	( RCC->APB2ENR |= ( 1 << 4 ) )
#define USART6_PCLK_EN()	( RCC->APB2ENR |= ( 1 << 5 ) )

/*Macros de habilitacion de clock para perifericos SYSCFG*/
#define SYSCFG_PCLK_EN() (RCC->APB2ENR |= (1 << 14))


/*Macros de deshabilitacion de clock para perifericos GPIOx*/
#define GPIOA_PCLK_DI()	( RCC->AHB1ENR &= ~( 1 << 0 ) )
#define GPIOB_PCLK_DI()	( RCC->AHB1ENR &= ~( 1 << 1 ) )
#define GPIOC_PCLK_DI()	( RCC->AHB1ENR &= ~( 1 << 2 ) )
#define GPIOH_PCLK_DI()	( RCC->AHB1ENR &= ~( 1 << 7 ) )

/*Macros de deshabilitacion de clock para perifericos I2C*/
#define I2C1_PCLK_DI() 	( RCC->APB1ENR &= ~( 1 << 21 ) )
#define I2C2_PCLK_DI() 	( RCC->APB1ENR &= ~( 1 << 22 ) )
#define I2C4_PCLK_DI() 	( RCC->APB1ENR &= ~( 1 << 24 ) )

/*Macros de deshabilitacion de clock para perifericos SPIx*/
#define SPI1_PCLK_DI() 	( RCC->APB2ENR &= ~( 1 << 12 ) )
#define SPI2_PCLK_DI() 	( RCC->APB1ENR &= ~( 1 << 14 ) )

/*Macros de deshabilitacion de clock para perifericos USART*/
#define USART2_PCLK_DI()	( RCC->APB1ENR &= ~( 1 << 17 ) )
#define USART1_PCLK_DI()	( RCC->APB2ENR &= ~( 1 << 4 ) )
#define USART6_PCLK_DI()	( RCC->APB2ENR &= ~( 1 << 5 ) )

/*Macros de deshabilitacion de clock para perifericos SYSCFG*/
#define SYSCFG_PCLK_DI() (RCC->APB2ENR &= ~(1 << 14))

//Macros para resetear los perifericos GPIOx
#define GPIOA_REG_RESET()	do{(RCC->AHB1RSTR |= (1 << 0)); (RCC->AHB1RSTR &= ~(1 << 0)); }while(0)
#define GPIOB_REG_RESET()	do{(RCC->AHB1RSTR |= (1 << 0)); (RCC->AHB1RSTR &= ~(1 << 0)); }while(0)
#define GPIOC_REG_RESET()	do{(RCC->AHB1RSTR |= (1 << 0)); (RCC->AHB1RSTR &= ~(1 << 0)); }while(0)
#define GPIOH_REG_RESET()	do{(RCC->AHB1RSTR |= (1 << 0)); (RCC->AHB1RSTR &= ~(1 << 0)); }while(0)

//Retorna el codigo de puerto dado por la direccion base GPIOx
#define GPIO_BASEADDR_TO_CODE(x)	((x == GPIOA) ? 0 :\
									(x == GPIOB) ? 1 :\
									(x == GPIOC) ? 2 :\
									(x == GPIOH) ? 7 :0)

//IRQ(Interrupt request) Numero de MCU stm32f410rb
#define IRQ_NO_EXTI0		6
#define IRQ_NO_EXTI1		7
#define IRQ_NO_EXTI2		8
#define IRQ_NO_EXTI3		9
#define IRQ_NO_EXTI4		10
#define IRQ_NO_EXTI9_5		23
#define IRQ_NO_EXTI15_10	40
//IRQ SPI
#define IRQ_SPI1			35
#define IRQ_SPI2			36
#define IRQ_SPI5			85
//IRQ I2C
#define IRQ_NO_I2C1_EV		31
#define IRQ_NO_I2C1_ER		32
#define IRQ_NO_I2C2_EV		33
#define IRQ_NO_I2C2_ER		34
#define IRQ_NO_I2C4_EV		95
#define IRQ_NO_I2C4_ER		96
//IRQ USART
#define IRQ_NO_USART1 		37
#define IRQ_NO_USART2		38
#define IRQ_NO_USART6		71

//Prioridades IRQs
#define NVIC_IRQ_PRI0		0
#define NVIC_IRQ_PRI1		1
#define NVIC_IRQ_PRI2		2
#define NVIC_IRQ_PRI3		3
#define NVIC_IRQ_PRI4		4
#define NVIC_IRQ_PRI5		5
#define NVIC_IRQ_PRI6		6
#define NVIC_IRQ_PRI7		7
#define NVIC_IRQ_PRI8		8
#define NVIC_IRQ_PRI9		9
#define NVIC_IRQ_PRI10		10
#define NVIC_IRQ_PRI11		11
#define NVIC_IRQ_PRI12		12
#define NVIC_IRQ_PRI13		13
#define NVIC_IRQ_PRI14		14
#define NVIC_IRQ_PRI15		15

//Algunas macros genericas
#define ENABLE 			1
#define DISABLE 		0
#define SET 			ENABLE
#define RESET 			DISABLE
#define GPIO_PIN_SET 	SET
#define GPIO_PIN_RESET 	RESET
#define FLAG_RESET 		RESET
#define FLAG_SET 		SET

/*Definiciones de posiciones de bit del periferico SPI*/
//SPI_CR1
#define SPI_CR1_CPHA     				 0
#define SPI_CR1_CPOL      				 1
#define SPI_CR1_MSTR     				 2
#define SPI_CR1_BR   					 3
#define SPI_CR1_SPE     				 6
#define SPI_CR1_LSBFIRST   			 	 7
#define SPI_CR1_SSI     				 8
#define SPI_CR1_SSM      				 9
#define SPI_CR1_RXONLY      		 	10
#define SPI_CR1_DFF     			 	11
#define SPI_CR1_CRCNEXT   			 	12
#define SPI_CR1_CRCEN   			 	13
#define SPI_CR1_BIDIOE     			 	14
#define SPI_CR1_BIDIMODE      			15

//SPI_CR2
#define SPI_CR2_RXDMAEN		 			0
#define SPI_CR2_TXDMAEN				 	1
#define SPI_CR2_SSOE				 	2
#define SPI_CR2_FRF						4
#define SPI_CR2_ERRIE					5
#define SPI_CR2_RXNEIE				 	6
#define SPI_CR2_TXEIE					7

//SPI_SR
#define SPI_SR_RXNE						0
#define SPI_SR_TXE				 		1
#define SPI_SR_CHSIDE				 	2
#define SPI_SR_UDR					 	3
#define SPI_SR_CRCERR				 	4
#define SPI_SR_MODF					 	5
#define SPI_SR_OVR					 	6
#define SPI_SR_BSY					 	7
#define SPI_SR_FRE					 	8


/*Definiciones de posiciones de bit del periferico I2C*/
//I2C_CR1
#define I2C_CR1_PE						0
#define I2C_CR1_SMBUS					1
#define I2C_CR1_SMBTYPE					3
#define I2C_CR1_ENARP					4
#define I2C_CR1_ENPEC					5
#define I2C_CR1_ENGC					6
#define I2C_CR1_NOSTRETCH				7
#define I2C_CR1_START 					8
#define I2C_CR1_STOP					9
#define I2C_CR1_ACK						10
#define I2C_CR1_POS						11
#define I2C_CR1_PEC						12
#define I2C_CR1_ALERT					13
#define I2C_CR1_SWRST 					15

//I2C_CR2
#define I2C_CR2_FREQ					0
#define I2C_CR2_ITERREN					8
#define I2C_CR2_ITEVTEN					9
#define I2C_CR2_ITBUFEN					10
#define I2C_CR2_DMAEN					11
#define I2C_CR2_LAST 					12

//I2C_OAR1
#define I2C_OAR1_ADD0					0
#define I2C_OAR1_ADD71					1
#define I2C_OAR1_ADD98					8
#define I2C_OAR1_ADDMODE				15

//I2C_SR1
#define I2C_SR1_SB						0
#define I2C_SR1_ADDR					1
#define I2C_SR1_BTF						2
#define I2C_SR1_ADD10					3
#define I2C_SR1_STOPF					4
#define I2C_SR1_RxNE					6
#define I2C_SR1_TxE						7
#define I2C_SR1_BERR					8
#define I2C_SR1_ARLO 					9
#define I2C_SR1_AF						10
#define I2C_SR1_OVR						11
#define I2C_SR1_PECERR					12
#define I2C_SR1_TIMEOUT					14
#define I2C_SR1_SMBALERT 				15

//I2C_SR2
#define I2C_SR2_MSL 					0
#define I2C_SR2_BUSY					1
#define I2C_SR2_TRA						2
#define I2C_SR2_GENCALL					4
#define I2C_SR2_SMBDEFAULT				5
#define I2C_SR2_SMBHOST					6
#define I2C_SR2_DUALF					7
#define I2C_SR2_PEC						8

//I2C_CCR
#define I2C_CCR_CCR						0
#define I2C_CCR_DUTY					14
#define I2C_CCR_FS						15

/*Definiciones de posiciones de bit del periferico USART*/
//USART_SR
#define USART_SR_PE		0
#define USART_SR_FE		1
#define USART_SR_NF		2
#define USART_SR_ORE	3
#define USART_SR_IDLE	4
#define USART_SR_RXNE 	5
#define USART_SR_TC		6
#define USART_SR_TXE 	7
#define USART_SR_LBD	8
#define USART_SR_CTS 	9

//USART_DR
#define USART_DR		0

//USART_BRR
#define USART_BRR_DIV_Fraction	0
#define USART_BRR_DIV_Mantissa	4

//USART_CR1
#define USART_CR1_SBK		0
#define USART_CR1_RWU		1
#define USART_CR1_RE		2
#define USART_CR1_TE		3
#define USART_CR1_IDLEIE	4
#define USART_CR1_RXNEIE	5
#define USART_CR1_TCIE		6
#define USART_CR1_TXEIE		7
#define USART_CR1_PEIE		8
#define USART_CR1_PS		9
#define USART_CR1_PCE		10
#define USART_CR1_WAKE		11
#define USART_CR1_M			12
#define USART_CR1_UE		13
#define USART_CR1_OVER8		15

//USART_CR2
#define USART_CR2_ADD		0
#define USART_CR2_LBDL		5
#define USART_CR2_LBDIE		6
#define USART_CR2_LBCL		8
#define USART_CR2_CPHA		9
#define USART_CR2_CPOL		10
#define USART_CR2_CLKEN		11
#define USART_CR2_STOP		12
#define USART_CR2_LINEN		14

//USART_CR3
#define USART_CR3_EIE		0
#define USART_CR3_IREN		1
#define USART_CR3_IRLP		2
#define USART_CR3_HDSEL		3
#define USART_CR3_NACK 		4
#define USART_CR3_SCEN		5
#define USART_CR3_DMAR		6
#define USART_CR3_DMAT		7
#define USART_CR3_RTSE		8
#define USART_CR3_CTSE		9
#define USART_CR3_CTSIE		10
#define USART_CR3_ONEBIT	11

//USART_GTPR
#define USART_GTPR_PSC		0
#define USART_GTPR_GT		8

#include "stm32f410rb_gpio_driver.h"
#include "stm32f410rb_spi_driver.h"
#include "stm32f410rb_i2c_driver.h"
#include "stm32f410rb_usart_driver.h"
#include "stm32f410rb_rcc_driver.h"

#endif /* INC_STM32F410RB_H_ */

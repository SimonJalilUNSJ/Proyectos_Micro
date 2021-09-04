/*
 * main-1.c
 *
 *  Created on: 23 may. 2021
 *      Author: simon
 */
#include <stdint.h>
#include <stdio.h>


#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

void fun_divide(int x, int y){
	return x/y;
}

int main(void)
{
	//1. Habilitar todas las excepciones configurables como error de uso,
	//error de manejo de memoria y error de bus
	uint32_t *pSHCSR = (uint32_t*)0xE000ED24;

	*pSHCSR |= (1 << 16);	//manejo de memoria
	*pSHCSR |= (1 << 17);	//error de bus
	*pSHCSR |= (1 << 18);	//error de uso

	//2. Habilitar la trampa de division por cero.
	uint32_t *pCCR = (uint32_t*) 0xE000ED14;
	*pCCR |= (1 << 4);

	//3. Intento de division por cero
	fun_divide(0, 0);

	/* Loop forever */
	for(;;);
}

//2.Implementar los manipuladores de errores.
void HardFault_Handler(void){
	printf("Excepción : Hardfault\n");
	while(1);
}

void MemManage_Handler(void){
	printf("Excepción : Manejo de memoria\n");
	while(1);
}

void BusFault_Handler(void){
	printf("Excepción : Error de bus\n");
	while(1);
}

__attribute__((naked)) void UsageFault_Handler(void){
	//Aqui pudimos extraer el valor del MSP cuando fue
	//la direccion base del stack frame(thread mode) que tuvo guardado durante
	//la entrada de excepcion desde thread mode a handler mode
	__asm ("MRS r0,MSP");
	__asm ("B UsageFault_Handler_c");
}

void UsageFault_Handler_c(uint32_t *pBaseStackFrame){

	//El termino "register" sirve para indicarle al compilador
	//que la variable no se guarde en el stack sino en un registro
	//register uint32_t msp_value __asm("r0");
	//uint32_t *pMSP = (uint32_t*)msp_value;

	uint32_t *pUFSR = (uint32_t*)0xE000ED2A;
	printf("Excepción : Error de uso\n");
	printf("UFSR = %lx\n",(*pUFSR) & 0xFFFF);
	printf("pBaseStackFrame = %p\n",pBaseStackFrame);
	printf("Valor de R0 = %lx\n",pBaseStackFrame[0]);
	printf("Valor de R1 = %lx\n",pBaseStackFrame[1]);
	printf("Valor de R2 = %lx\n",pBaseStackFrame[2]);
	printf("Valor de R3 = %lx\n",pBaseStackFrame[3]);
	printf("Valor de R12 = %lx\n",pBaseStackFrame[4]);
	printf("Valor de LR = %lx\n",pBaseStackFrame[5]);
	printf("Valor de PC = %lx\n",pBaseStackFrame[6]);
	printf("Valor de XPSR = %lx\n",pBaseStackFrame[7]);

	while(1);
}


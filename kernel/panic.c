/*
 * panic.c
 *
 *  Implementation of function called when critical error occurred
 *  Created on: 16.06.2019
 *      Author: Paweł Wieczorek
 */

#include "panic.h"
#include "stm32f10x_gpio.h"
#include "core_cm3.h"
#include <stdio.h>

// Configure GPIO of LED
static inline void configureLEDGPIO(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

// Dump registers - not implemented yet
static inline void dumpKernelContext(void) {
	static const char* registersNames[] = {	"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8",
						"R9", "R10", "R11", "R12", "SP", "LR", "PC"};

	// TODO: implement registers dumping
	for(int i = 0; i < 16; i++)
		printf("\t%s: %p\n", registersNames[i], 0x0);
}

// Main kernel panic function
void kernelPanic(char* reason) {
	__disable_fault_irq();			// Disable all interrupts

	puts("======= Kernel panic =======\0");
	puts("Reason:\0");
	puts(reason);
	puts("Dump of kernel context\0");
	dumpKernelContext();

#ifdef KERNEL_PANIC_HALT
	puts("System halted...\0");
#else
	puts("System will now reboot\0");
#endif

	puts("============ END ===========\n\0");


#ifdef KERNEL_PANIC_HALT
	configureLEDGPIO();

	while(1) {
		// Blink LED
		GPIO_WriteBit(KERNEL_PANIC_LED_PORT, KERNEL_PANIC_LED_PIN,
				1 - GPIO_ReadInputDataBit(KERNEL_PANIC_LED_PORT, KERNEL_PANIC_LED_PIN) );
		for(int i = 0; i < SystemCoreClock / 30; i++);		// Primitive delay
	}
#else
	NVIC_SystemReset();
#endif
}

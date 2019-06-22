/*
 * panic.c
 *
 *  Implementation of function called when critical error occurred
 *  Created on: 16.06.2019
 *      Author: Paweł Wieczorek
 */

#include "panic.h"
#include "thread.h"
#include "../generic/core.h"
#include "../generic/io.h"

extern Thread* activeThread;

#ifdef KERNEL_PANIC_HALT
#include "stm32f10x_gpio.h"

// Configure GPIO of LED
static inline void configureLEDGPIO(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
#endif

// Dump registers - not implemented yet
static inline void dumpKernelContext(void) {
	static const char* registersNames[] = {	"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8",
						"R9", "R10", "R11", "R12", "SP", "LR", "PC"};

	// TODO: implement registers dumping
	for(int i = 0; i < 16; i++)
		_puts(registersNames[i]);
}

// Main kernel panic function
void kernelPanic(char* reason) {
	__core_disable_irq();			// Disable all interrupts

	_puts("======= Kernel panic =======");
	_puts("Reason:");
	_puts(reason);
	_puts("Active thread:");
	_puts(activeThread->name);
	_puts("Dump of kernel context");
	dumpKernelContext();

#ifdef KERNEL_PANIC_HALT
	_puts("System halted...");
#elif defined KERNEL_PANIC_BREAKPOINT
	_puts("System will enter DEBUG state");
#else
	_puts("System will now reboot");
#endif

	_puts("============ END ===========\n");


#ifdef KERNEL_PANIC_HALT
	configureLEDGPIO();

	while(1) {
		// Blink LED
		GPIO_WriteBit(KERNEL_PANIC_LED_PORT, KERNEL_PANIC_LED_PIN,
				1 - GPIO_ReadInputDataBit(KERNEL_PANIC_LED_PORT, KERNEL_PANIC_LED_PIN) );
		for(int i = 0; i < SystemCoreClock / 30; i++);		// Primitive delay
	}
#elif defined KERNEL_PANIC_BREAKPOINT
	__core_dbg_brk();
	for(;;);
#else
	for(int i = 0; i < SystemCoreClock / 50; i++);		// Give USART time to flush Tx buffer
	NVIC_SystemReset();
#endif
}

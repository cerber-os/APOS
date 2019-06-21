/*
 * main.c
 *
 *  Created on: 16.06.2019
 *      Author: Paweł Wieczorek
 */

#include <stdio.h>
#include <stm32f10x.h>

#include "kernel/apos.h"
#include "device/periphConfig.h"

void entry_KernelThread(void);
void entry_Thread1(void);
void entry_Thread2(void);

// Structures holding configuration of used threads
CREATE_THREAD_CONFIG(KernelThread, entry_KernelThread, 2000, THREAD_FLAG_IS_KERNEL_THREAD);
CREATE_THREAD_CONFIG(Thread1, entry_Thread1, 2000, THREAD_FLAG_NORMAL);
CREATE_THREAD_CONFIG(Thread2, entry_Thread2, 2000, THREAD_FLAG_NORMAL);


int main(void)
{
	__core_disable_irq();
	ConfigRCC();
	ConfigNVIC();
	ConfigGPIO();
	ConfigUSART();

	puts("Low level initialization finished. Starting kernel");

	StartKernel(&KernelThread);
	for(;;);
}

// Entry function of kernel thread
void entry_KernelThread(void) {
	puts("Hello world from Kernel Thread\0");

	// Create Thread #1
	SVC_OSCreateThread(&Thread1);
	puts("Created thread #1\0");

	// Create Thread #2
	SVC_OSCreateThread(&Thread2);
	puts("Created thread #2\0");

	// Blink LED
	while(1) {
		;
	}
}

void recursiveFunc(void) {
	SVC_OSSuspendThread(&Thread1, 1);
	uint32_t stack;
	asm volatile ("mov %0, sp;" : "=r"(stack));
	printf("Cur sp: %x\n", stack);
	if(KernelThread.stackPtr == 0x342524)		// Just to make sure GCC won't modify recursion
		return;
	else
		recursiveFunc();
}

// Entry function of thread #1
void entry_Thread1(void) {
	puts("Hello world from Thread 1\0");
	recursiveFunc();
}

// Entry function of thread #2
void entry_Thread2(void) {
	puts("Hello world from Thread 2\0");
	while(1) {
		GPIO_WriteBit(GPIOB, GPIO_Pin_12, 1 - GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) );
		SVC_OSSuspendThread(&Thread2, 25);
	}
}

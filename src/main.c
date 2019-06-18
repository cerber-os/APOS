/*
 * main.c
 *
 *  Created on: 16.06.2019
 *      Author: Paweł Wieczorek
 */

#include <stdio.h>

#include "stm32f10x.h"
#include "../kernel/apos.h"

void ConfigRCC(void);
void ConfigNVIC(void);
void ConfigGPIO(void);
void ConfigUSART(void);

void entry_KernelThread(void);
void entry_Thread1(void);
void entry_Thread2(void);

// Structures holding configuration of used threads
Thread Thread2 = {
		.entryPoint = entry_Thread2,
		.stackSize = 2000,
		.flags = THREAD_FLAG_NORMAL
};

Thread Thread1 = {
		.entryPoint = entry_Thread1,
		.stackSize = 2000,
		.flags = THREAD_FLAG_NORMAL
};

Thread KernelThread = {
		.entryPoint = entry_KernelThread,
		.stackSize = 2000,
		.flags = THREAD_FLAG_IS_KERNEL_THREAD
};

//*******************************//
int main(void)
{
	__core_disable_irq();
	ConfigRCC();
	SysTick_Config(SystemCoreClock / 25);
	ConfigNVIC();
	ConfigGPIO();
	ConfigUSART();

	puts("Low level initialization finished. Starting kernel");

	StartKernel(&KernelThread);
	for(;;);
}

void ConfigRCC(void) {
	// Clock configuration is done by System_Init() function.
	// We only need to configure our peripherals

	// Enable clock on peripherals
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
}


void ConfigGPIO(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	// Led 8-15
	GPIO_InitStructure.GPIO_Pin = 0xFF00;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB, 0xFF00);

	// PA9 - USART Tx  (alternative function pull-up)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// PA10 - USART Rx (alternative function floating)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void ConfigNVIC(void) {
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	NVIC_InitStructure.NVIC_IRQChannel = PendSV_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = SysTick_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 14;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = SVCall_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 13;
	NVIC_Init(&NVIC_InitStructure);
}

void ConfigUSART(void) {
	USART_InitTypeDef USART_InitStructure;

	// Set USART1 configuration (38400b/s, no parity, 8b length)
	USART_InitStructure.USART_BaudRate = 38400;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
			USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitStructure);
	// Start USART1
	USART_Cmd(USART1, ENABLE);
}

// Entry function of kernel thread
void entry_KernelThread(void) {
	puts("Hello world from Kernel Thread\0");
	SVC_OSCreateThread(&Thread1);
	puts("Created thread #1");

	SVC_OSCreateThread(&Thread2);
	puts("Created thread #2");
	while(1) {
			//	Blink LED
			GPIO_WriteBit(GPIOB, GPIO_Pin_10,
					1 - GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) );
			for(int i = 0; i < SystemCoreClock / 80; i++);		// Primitive delay
		}
}

// Entry function of thread #1
void entry_Thread1(void) {
	puts("Hello world from Thread 1\0");
		while(1) {
				//	Blink LED
				GPIO_WriteBit(GPIOB, GPIO_Pin_11,
						1 - GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) );
				for(int i = 0; i < SystemCoreClock / 80; i++);		// Primitive delay
			}
}

// Entry function of thread #2
void entry_Thread2(void) {
	puts("Hello world from Thread 2\0");
		while(1) {
				//	Blink LED
				GPIO_WriteBit(GPIOB, GPIO_Pin_12,
						1 - GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) );
				for(int i = 0; i < SystemCoreClock / 80; i++);		// Primitive delay
			}
}

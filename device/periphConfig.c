/*
 * periphConfig.c
 *
 *  Configuration of all peripherals used in project
 *  Created on: 19.06.2019
 *      Author: Paweł Wieczorek
 */

#include "../device/periphConfig.h"

#include "stm32f10x.h"

void ConfigRCC(void) {
	// Clock configuration is done by System_Init() function.
	// We only need to configure our peripherals
	SysTick_Config(SystemCoreClock / 25);

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

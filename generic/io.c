/*
 * io.c
 *
 *  Declarations of input/output wrappers used by stdlib functions, like puts or printf
 *  Created on: 16.06.2019
 *      Author: Paweł Wieczorek
 */

#include "io.h"
#include "stm32f10x_usart.h"

// Print char to the console via USART1
int __io_putchar(int ch) {
	 while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	 USART_SendData(USART1, ch);

	 return ch;
}

/*
 * panic.h
 *
 *  Implementation of function called when critical error occurred
 *  Created on: 16.06.2019
 *      Author: Paweł Wieczorek
 */

#ifndef PANIC_H_
#define PANIC_H_

// Kernel panic configuration
// 	KERNEL_PANIC_HALT:		should system be halted after panic occurred
//	KERNEL_PANIC_RESET:		should system be restarted after panic occurred
//	KERNEL_PANIC_BREAKPOINT:	should system enter DEBUG state after panic occurred

#define KERNEL_PANIC_HALT
//#define KERNEL_PANIC_RESET
//#define KERNEL_PANIC_BREAKPOINT

// Configuration of LED used to signalize kernel panic
#define KERNEL_PANIC_LED_PORT	GPIOB
#define KERNEL_PANIC_LED_PIN	GPIO_Pin_8

// Functions declarations
void kernelPanic(char* reason) __attribute__ ((noreturn));

#endif /* PANIC_H_ */

/*
 * core.h
 *  Inline C wrappers to some ARM assembly instructions used in program
 *
 *  Created on: 18.06.2019
 *      Author: Paweł Wieczorek
 */

#ifndef CORE_H_
#define CORE_H_

#include <stdint.h>

// Constants for use with control register
#define SET_PSP_THREAD_STACK 		(1 << 1)
#define SET_UNPRIVILEGED_MODE		(1 << 0)

// Wrappers declarations
static inline void 		__core_enable_irq	() 			__attribute__((always_inline));
static inline void 		__core_disable_irq	() 			__attribute__((always_inline));
static inline void 		__core_ISB		() 			__attribute__((always_inline));
static inline void 		__core_set_control	(uint32_t flag) 	__attribute__((always_inline));
static inline void 		__core_set_psp		(uint32_t* addr)	__attribute__((always_inline));
static uint32_t* 		__core_get_psp		() 			__attribute__((naked));

// Wrappers definitions

// Enable interrupts
static inline void __core_enable_irq() {
	asm volatile("CPSIE I");
}

// Disable interrupts
static inline void __core_disable_irq() {
	asm volatile("CPSID I");
}

// Flush the pipeline of processor
static inline void __core_ISB() {
	asm volatile("ISB");
}

// Set CONTROL register
static inline void __core_set_control(uint32_t flag) {
	asm volatile ("MSR CONTROL, %0" : : "r" (flag) );
}

// Set PSP register
static inline void __core_set_psp(uint32_t* addr) {
	asm volatile ("MSR PSP, %0\n\t"  : : "r" (addr) );
}

// Get current PSP register value
static uint32_t* __core_get_psp() {
	uint32_t res;
	asm volatile (	"MRS	%0,	PSP;"
			"MOV	R0,	%0;"
			"BX	LR;" : "=&r"(res));
	return (uint32_t*)res;
}

#endif /* CORE_H_ */

/*
 * apos.c
 *
 *	File grouping APOS includes
 *  Created on: 18.06.2019
 *      Author: Paweł Wieczorek
 */

#include "apos.h"

// Array used as temporary stack for kernel start up
uint32_t tempStack[30];

void StartKernel(Thread* kernelThread) {
	__core_set_psp(&tempStack[29]);
	__core_set_control(SET_PSP_THREAD_STACK);
	__core_ISB();
	__core_enable_irq();

	SVC_OSStart(kernelThread);
}

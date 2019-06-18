/*
 * interrupts.c
 *
 *  Definition of interrupts used by kernel
 *  Created on: 18.06.2019
 *      Author: Paweł Wieczorek
 */

#include "interrupts.h"
#include "thread.h"
#include "syscall.h"
#include "panic.h"

void SVC_Handler(void) {
	asm volatile("PUSH  {LR};"
				 // Dump context of current thread
				 "MRS   R12,  PSP;"
				 "STMDB R12!, {R4-R11};"
				 "MSR	PSP,  R12;"
				 // Call SVC handler C function
				 "BL	SVC_Handler_Body;"
				 "POP	{LR};"

				 // Trigger PendSV interrupt
				 "LDR 	R0,   =0xe000ed04;"
				 "LDR	R1,   [R0];"
				 "ORR   R1,   #0x10000000;"
			     "STR	R1,	  [R0];"

				 // Return
				 "BX 	LR;");
}

void SysTick_Handler(void) {
	asm volatile ("MOV  r0, %0;"
				  "SVC  0;"
				  "BX LR;" : : "i" (SVC_TASK_SWITCH));
}

void PendSV_Handler(void) {
	asm volatile ("MRS	 R0, PSP;"
				  "LDMFD R0!, {R4-R11};"
				  "MSR   PSP, R0;"
				  "BX 	 LR;");
}

void HardFault_Handler(void) {
	kernelPanic("Hard fault occurred!");
}


/*
 * syscall.c
 *
 *  Implementation of SVC handlers
 *  Created on: 17.06.2019
 *      Author: Paweł Wieczorek
 */

#include "syscall.h"
#include "panic.h"
#include "../kernel/thread.h"
#include "../generic/core.h"
#include <stdint.h>

// Currently active thread
extern Thread* activeThread;

// Create new thread
static void SVCCreateThread(Thread* thread) {
	__core_disable_irq();

	if(createThread(thread))
		while(1);		// TODO: add better error handling

	__core_enable_irq();
	__core_ISB();
}

// Start kernel
static inline void SVCSystemStart(Thread* thread) {
	__core_disable_irq();

	thread->flags |= THREAD_FLAG_IS_KERNEL_THREAD;		// The first thread is always kernel thread
	SVCCreateThread(thread);
	__core_set_psp(activeThread->stackPtr);			// Set PSP pointer

	__core_enable_irq();
	__core_ISB();
}

// Switch to the next task
static inline void SVCTaskSwitch(void) {
	__core_disable_irq();

	uint32_t* pspStack = __core_get_psp();		// Get current PSP pointer
	activeThread->stackPtr = pspStack;		// Save PSP pointer

	Thread* nextThread = (Thread*)CList_Next((CList_t*)activeThread);	// Get next thread
	__core_set_psp(nextThread->stackPtr);		// Set PSP pointer
	activeThread = nextThread;

	__core_enable_irq();
	__core_ISB();
}

// Kill thread
static inline void SVCKillThread(Thread* thread) {
	// Check if such task exists
	Thread* curThread = activeThread;
	do {
		if(curThread == thread)
			break;
		curThread = (Thread*) curThread->list.next;
	} while(curThread != activeThread);

	if(curThread != thread)
		kernelPanic("Attempted to kill not existing thread");
	else if(curThread->flags == THREAD_FLAG_IS_KERNEL_THREAD)
		kernelPanic("Attempted to kill kernel thread");
	else {
		if(curThread == activeThread)	// Active thread cannot be killed - first switch to another one
			SVCTaskSwitch();
		CList_Remove((CList_t*) curThread);
		curThread->flags |= THREAD_FLAG_KILLED;
	}
}

// C body of SVC interrupt handler
void SVC_Handler_Body(uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3) {
    switch(r0)    {
        case SVC_OS_START:
		SVCSystemStart((Thread*) r1);
		break;
        case SVC_TASK_SWITCH:
		SVCTaskSwitch();
		break;
        case SVC_CREATE_THREAD:
		SVCCreateThread((Thread*) r1);
		break;
        case SVC_KILL_THREAD:
        	SVCKillThread((Thread*) r1);
        	break;
        default:
		kernelPanic("Unknown syscall requested");
		break;
    }
}


// C wrappers for calling syscalls

// Start system
void SVC_OSStart(Thread* r0) {
	asm volatile(	"MOV	R1,	R0;"
			"MOV	R0,	%0;"
			"SVC	0;"
			"BX	LR;": : "i" (SVC_OS_START));
}

// Switch task
void SVC_OSTaskSwitch(void) {
	asm volatile(	"MOV	R0, %0;"
			"SVC	0;"
			"BX	LR;" : : "i" (SVC_TASK_SWITCH));
}

// Create new task
void SVC_OSCreateThread(Thread* r0) {
	asm volatile(	"MOV	R1,	R0;"
			"MOV	R0,	%0;"
			"SVC	0;"
			"BX	LR;" : : "i" (SVC_CREATE_THREAD));
}

// Kill task
void SVC_OSKillThread(Thread* r0) {
	asm volatile(	"MOV	R1,	R0;"
			"MOV	R0,	%0;"
			"SVC	0;"
			"BX	LR;" : : "i" (SVC_KILL_THREAD));
}

// Kill task that called this function
void SVC_OSKillSelf(void) {
	SVC_OSKillThread(activeThread);
}

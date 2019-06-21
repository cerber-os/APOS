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
	if(CList_Find((CList_t*)activeThread, (CList_t*) thread) != 0x0)
		kernelPanic("Attempted to create already existing thread");

	if(createThread(thread))
		kernelPanic("Unable to create thread - out of memory");
}

// Start kernel
static inline void SVCSystemStart(Thread* thread) {
	thread->flags |= THREAD_FLAG_IS_KERNEL_THREAD;		// The first thread is always kernel thread
	SVCCreateThread(thread);
	__core_set_psp(activeThread->stackPtr);			// Set PSP pointer
}

// Switch to the next task
static inline void SVCTaskSwitch(void) {
	uint32_t* pspStack = __core_get_psp();		// Get current PSP pointer
	activeThread->stackPtr = pspStack;		// Save PSP pointer

	if(verifyStackCanaries())			// Verify canaries at stack pool
		kernelPanic("Stack overflow detected - invalid canary");

	Thread* nextThread = (Thread*)activeThread->list.next;
	while(nextThread->sleep > 0)
		nextThread = (Thread*)nextThread->list.next;

	__core_set_psp(nextThread->stackPtr);		// Set PSP pointer
	activeThread = nextThread;
}

// Kill thread
static inline void SVCKillThread(Thread* thread) {
	// Check if such task exists
	Thread* found = (Thread*)CList_Find((CList_t*)activeThread, (CList_t*)thread);

	if(found == 0x0)
		kernelPanic("Attempted to kill not existing thread");
	else if(found->flags & THREAD_FLAG_IS_KERNEL_THREAD)
		kernelPanic("Attempted to kill kernel thread");
	else {
		if(found == activeThread)	// Active thread cannot be killed - first switch to another one
			SVCTaskSwitch();
		CList_Remove((CList_t*) found);
		found->flags |= THREAD_FLAG_KILLED;
	}
}

// Suspend thread
static inline void SVCSuspendThread(Thread* thread, uint32_t delay) {
	// Check if such task exists
	Thread* found = (Thread*)CList_Find((CList_t*)activeThread, (CList_t*)thread);
	if(found == 0x0)
		kernelPanic("Attempted to delay not existing thread");
	else if(found->flags & THREAD_FLAG_IS_KERNEL_THREAD)
		kernelPanic("Attempted to delay kernel thread");

	found->sleep = delay;
	found->flags |= THREAD_FLAG_SUSPENDED;
	if(found == activeThread)
		SVCTaskSwitch();
}

// C body of SVC interrupt handler
void SVC_Handler_Body(uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3) {
	__core_disable_irq();
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
        	case SVC_SUSPEND_THREAD:
        		SVCSuspendThread((Thread*) r1, (int32_t) r2);
        		break;
        	default:
        		kernelPanic("Unknown syscall requested");
        		break;
	}
	__core_enable_irq();
	__core_ISB();
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

// Suspend thread
void SVC_OSSuspendThread(Thread* r0, int32_t r1) {
	asm volatile(	"MOV	R2,	R1;"
			"MOV	R1,	R0;"
			"MOV	R0,	%0;"
			"SVC	0;"
			"BX	LR;" : : "i" (SVC_SUSPEND_THREAD));
}

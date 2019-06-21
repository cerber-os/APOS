/*
 * thread.c
 *
 *	Implementation of threads
 *  Created on: 16.06.2019
 *      Author: Paweł Wieczorek
 */

#include "thread.h"
#include "syscall.h"
#include <stdint.h>
#include <stdio.h>

// Currently active thread
Thread* activeThread = 0x0;
// Area where stacks of threads will be held
uint32_t StackPool[THREAD_STACK_POOL_SIZE / sizeof(uint32_t)];

// Function that will be called when thread ends
void thread_exit(void);

// Initialize thread stack frame
static void initThread(Thread* thread) {
	// Initialize list header
	thread->list.next = (CList_t*) thread;
	thread->list.prev = (CList_t*) thread;

	// memset(thread->stackPtr - thread->stackSize, 0, thread->stackSize);
	uint32_t* stack = thread->stackPtr;

	// Build basic stack frame
	*(--stack) = (uint32_t) 0x1000000;			// PSR
	*(--stack) = (uint32_t) thread->entryPoint & ARM_STATE_ADDRESS_MASK;// PC
	*(--stack) = (uint32_t) thread_exit;			// LR - function to call after thread ends
	*(--stack) = (uint32_t) 0x0;				// R12
	*(--stack) = (uint32_t) 0x0;				// R3
	*(--stack) = (uint32_t) 0x0;				// R2
	*(--stack) = (uint32_t) 0x0;				// R1
	*(--stack) = (uint32_t) 0x0;				// R0
	*(--stack) = (uint32_t) 0x0;				// R11
	*(--stack) = (uint32_t) 0x0;				// R10
	*(--stack) = (uint32_t) 0x0;				// R9
	*(--stack) = (uint32_t) 0x0;				// R8
	*(--stack) = (uint32_t) 0x0;				// R7
	*(--stack) = (uint32_t) 0x0;				// R6
	*(--stack) = (uint32_t) 0x0;				// R5
	*(--stack) = (uint32_t) 0x0;				// R4

	// Update stack pointer
	thread->stackPtr = stack;

	// Insert canary at the end of stack area
	SET_CANARY_AT_STACK_END(thread->startOfStack, thread->stackSize);
}

// Try to find free space for new thread stack
static uint32_t* findFreeSpaceInStackPool(uint32_t stackSize) {
	Thread* currentThread = activeThread;
	uint8_t found = 0;

	stackSize /= sizeof(uint32_t);
	do {
		uint32_t* newStackStart = currentThread->stackPtr - currentThread->stackSize / sizeof(uint32_t) - 8;
		if(newStackStart < StackPool) {	// New stack won't fit
			currentThread = (Thread*) currentThread->list.next;
			continue;
		}

		Thread* checkThread = currentThread;
		found = 1;
		do {
			if((newStackStart - stackSize / 4 >= checkThread->startOfStack)
			 ||(newStackStart <= checkThread->startOfStack - currentThread->stackSize / 4))
				{}
			else
				found = 0;

			checkThread = (Thread*) checkThread->list.next;
		} while(checkThread != currentThread);

		if(found)
			return newStackStart;

		currentThread = (Thread*) currentThread->list.next;
	} while(currentThread != activeThread);

	return 0x0;
}

// Error message passed by inline assembly to kernel panic function
char* createThreadErrMsg = "Attempted to create thread from another thread";

// Create new thread
int createThread(Thread* thread) {
	uint32_t reg1, reg2;

	// Check if function is called from handler mode
	asm volatile(	"MRS	%0,	PSP;"
			"MOV	%1,	SP;"
			"TST 	%1,	%0;"
			"ITT	EQ;"
			"MOVEQ 	R0, 	createThreadErrMsg;"
			"BEQ   	kernelPanic;" : "=r"(reg1), "=r"(reg2) : : "cc");

	// Check if any thread exists so far
	if(activeThread == 0x0) {
		// This is first thread
		if(thread->stackSize > THREAD_STACK_POOL_SIZE)
			return 1;				// Error: requested stack size exceeds pool size

		// Build stack configuration structure
		thread->stackPtr = StackPool + sizeof(StackPool) / sizeof(uint32_t) - 1;		// Last 4byte of stack
		thread->startOfStack = thread->stackPtr;
		initThread(thread);		// Build initial stack frame

		activeThread = thread;

		return 0;				// We are ready to launch
	} else {
		// Find free stack space for thread
		uint32_t* stackPtr = findFreeSpaceInStackPool(thread->stackSize);
		if(stackPtr == 0x0)
			return 1;				// There is no free space left for thread stack

		// Build stack configuration structure
		thread->stackPtr = stackPtr;
		thread->startOfStack = thread->stackPtr;
		initThread(thread);		// Build initial stack frame

		CList_Add((CList_t*) activeThread, (CList_t*) thread);

		return 0;
	}
}

// Called when thread exits
void thread_exit(void) {
	printf("[Thread_Exit] Thread %s has exited.\n", activeThread->name);
	SVC_OSKillThread(activeThread);
	for(;;);
}

void DecrementThreadsSuspendTime(void) {
	// Decrement delay in all threads
	Thread* it = activeThread;
	do {
		it->sleep -= (it->sleep > 0) & 1;	// If greater than 0, decrement
		it = (Thread*) it->list.next;
	} while(it != activeThread);
}

int verifyStackCanaries(void) {
	Thread* it = activeThread;
	do {
		if(STACK_POOL_CANARY != CANARY_AT_STACK_END(it->startOfStack, it->stackSize))
			return 1;
		it = (Thread*) it->list.next;
	} while(it != activeThread);
	return 0;
}

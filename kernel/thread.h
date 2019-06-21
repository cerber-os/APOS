/*
 * thread.h
 *
 *	Implementation of threads
 *  Created on: 14.06.2019
 *      Author: Paweł Wieczorek
 */

#ifndef THREAD_H_
#define THREAD_H_

#include "../generic/cList.h"
#include <stdint.h>

// Thread structure
struct Thread {
	CList_t list;					// Circular list header

	uint8_t* name;					// Name of the thread
	void* entryPoint;				// Pointer to entry point of thread
	uint16_t flags;					// Flags of thread
	int32_t sleep;					// Number of SysTicks thread should be sleeping

	uint32_t* startOfStack;				// Pointer to the base of stack
	uint32_t* stackPtr;				// Pointer to thread stack
	uint32_t stackSize;				// Size of stack in bytes
};
typedef struct Thread Thread;

// Flags of thread
#define THREAD_FLAG_NORMAL		(0)
#define	THREAD_FLAG_KILLED		(1 << 0)
#define THREAD_FLAG_SUSPENDED		(1 << 1)
#define THREAD_FLAG_IS_KERNEL_THREAD	(1 << 2)

// Threads global configuration
#define THREAD_STACK_SIZE		500
#define THREAD_STACK_POOL_SIZE		8000

// Bit masks
#define ARM_STATE_ADDRESS_MASK		0xFFFFFFFE

// Canary used as a primitive stack overflow protection
#define STACK_POOL_CANARY		0x6AA4DC7F
#define SET_CANARY_AT_STACK_END(PTR, SIZE)	\
		*(PTR - SIZE / sizeof(uint32_t)) = STACK_POOL_CANARY
#define CANARY_AT_STACK_END(PTR, SIZE)	*(PTR - SIZE / sizeof(uint32_t))

// Macro used to create thread structure
#define	CREATE_THREAD_CONFIG(NAME, ENTRY, SIZE, FLAGS)	\
				Thread NAME = {				\
					.name=(uint8_t*)#NAME,		\
					.entryPoint=ENTRY,		\
					.stackSize=SIZE,		\
					.flags=FLAGS,			\
				}

// Functions declarations
int 	createThread			(Thread* thread);
void 	DecrementThreadsSuspendTime	(void);
int 	verifyStackCanaries		(void);

#endif /* THREAD_H_ */

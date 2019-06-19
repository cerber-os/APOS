/*
 * syscall.h
 *
 *  Implementation of SVC handlers
 *  Created on: 17.06.2019
 *      Author: Paweł Wieczorek
 */

#ifndef SYSCALL_H_
#define SYSCALL_H_

#include "thread.h"

// Available syscalls
#define		SVC_OS_START		0
#define		SVC_TASK_SWITCH		1
#define		SVC_CREATE_THREAD	2

// Declaration of syscalls C wrappers
void SVC_OSStart	(Thread*)	__attribute__((naked));
void SVC_OSTaskSwitch	(void)		__attribute__((naked));
void SVC_OSCreateThread	(Thread*)	__attribute__((naked));

// Syscall interrupt C body
void SVC_Handler_Body(uint32_t, uint32_t, uint32_t, uint32_t);

#endif /* SYSCALL_H_ */

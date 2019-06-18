/*
 * apos.h
 *
 *	File grouping APOS includes
 *  Created on: 18.06.2019
 *      Author: Paweł Wieczorek
 */

#ifndef APOS_H_
#define APOS_H_

#include "../generic/core.h"
#include "../kernel/thread.h"
#include "../kernel/syscall.h"
#include "../kernel/interrupts.h"

// Declaration of basic kernel functions
void StartKernel(Thread* kernelThread);

#endif /* APOS_H_ */

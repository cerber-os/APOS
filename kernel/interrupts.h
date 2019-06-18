/*
 * interrupts.h
 *
 *  Definition of interrupts used by kernel
 *  Created on: 18.06.2019
 *      Author: Paweł Wieczorek
 */

#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

void SVC_Handler		(void) 	__attribute__((naked));
void SysTick_Handler	(void) 	__attribute__((naked));
void PendSV_Handler		(void) 	__attribute__((naked));

#endif /* INTERRUPTS_H_ */

/*
 * io.h
 *
 *  Declarations of input/output wrappers used by libc functions, like puts or printf
 *  Created on: 16.06.2019
 *      Author: Paweł Wieczorek
 */

#ifndef IO_H_
#define IO_H_

#include <sys/stat.h>

#define	HEAP_SIZE		1400

int 		_close		(int);
int 		_read 		(int, char*, int);
int 		_isatty		(int);
int 		_lseek		(int, int, int);
int 		_write		(int, char*, int);
int 		_fstat		(int, struct stat*);
uint8_t* 	_sbrk		(int);

/*************************************
 * 	Higher level IO functions
 *************************************/
void 		_puts		(const char*);

#endif /* IO_H_ */

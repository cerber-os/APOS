/*
 * io.c
 *
 *  Declarations of input/output wrappers used by libc functions, like puts or printf
 *  Created on: 16.06.2019
 *      Author: Paweł Wieczorek
 */

#include "io.h"

#include "stm32f10x_usart.h"
#include <sys/stat.h>
#include <stdint.h>

int _close(int file) { return -1; }
int _read (int file, char *ptr, int len) { return len; }
int _isatty(int file) { return 1; };
int _lseek(int file, int ptr, int dir) { return 0; };

int _write(int file, char *ptr, int len) {
	for (int i = 0; i < len; i++) {
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		USART_SendData(USART1, *ptr++);
	}

	return len;
}

int _fstat(int file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}


// Primitive heap support
uint8_t heap[HEAP_SIZE];
uint8_t* _sbrk(int incr)
{
	static uint8_t *heap_end;
	uint8_t *prev_heap_end;

	if (heap_end == 0)
		heap_end = heap;

	prev_heap_end = heap_end;
	if (heap_end - heap + incr >= HEAP_SIZE) {
		_write(0, "Heap overflow\n", 14);
		return (uint8_t*)-1;
	}

	heap_end += incr;
	return prev_heap_end;
}


/*************************************
 * 	Higher level IO functions
 *************************************/
void _puts(const char* str) {
	while(*str)
		USART_SendData(USART1, *str++);
	USART_SendData(USART1, '\n');
}

#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	uint32_t capacity;
	uint32_t cursor;
	volatile uint32_t length;
	void *ptr;
	volatile bool lock;
}cfifo;

void cfifo_init(cfifo *cfifo, void *buffer, uint32_t capacity);

int cfifo_isfull(cfifo *cfifo);

int cfifo_isempty(cfifo *cfifo);

bool cfifo_push(cfifo *cfifo, uint8_t *value);

bool cfifo_pop(cfifo *cfifo, uint8_t* byte);

#ifdef __cplusplus
}
#endif

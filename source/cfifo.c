#include "cfifo.h"
#include <string.h>

void cfifo_init(cfifo *cfifo, void *buffer, uint32_t capacity)
{
	cfifo->capacity = capacity;
	cfifo->cursor = cfifo->length = 0;
	cfifo->ptr = buffer;
}

int cfifo_isfull(cfifo *cfifo)
{
	return cfifo->length == cfifo->capacity;
}

int cfifo_isempty(cfifo *cfifo)
{
	return cfifo->length == 0;
}

bool cfifo_push(cfifo *cfifo, uint8_t *value)
{
	if(cfifo->lock)
	{
		return false;
	}

	cfifo->lock = true;
	uint8_t *pos;

	if (cfifo->length == cfifo->capacity)
	{
		pos = (uint8_t *)cfifo->ptr + cfifo->cursor;
		cfifo->cursor++;
		if (cfifo->cursor == cfifo->capacity)
			cfifo->cursor = 0;
	}
	else
	{
		uint32_t offset = (cfifo->cursor + cfifo->length) % cfifo->capacity;
		pos = (uint8_t *)cfifo->ptr + offset;
		cfifo->length++;
	}

	*pos = *value;

	cfifo->lock = false;

	return true;
}

bool cfifo_pop(cfifo *cfifo, uint8_t* byte)
{
	if (cfifo->length == 0 || cfifo->lock)
	{
		return false;
	}
	cfifo->lock = true;
	*byte = *((uint8_t *)cfifo->ptr + cfifo->cursor);
	cfifo->cursor++;
	if (cfifo->cursor == cfifo->capacity)
	{
		cfifo->cursor = 0;
	}
		
	cfifo->length--;
	cfifo->lock = false;
	return true;
}

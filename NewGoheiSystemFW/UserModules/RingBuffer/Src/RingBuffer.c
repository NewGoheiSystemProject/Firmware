/*
 * RingBuffer.c
 *
 *  Created on: 2018/10/02
 *      Author: Shuji
 */

#include "RingBuffer.h"

typedef struct{
	uint32_t Size;
	uint32_t Front;
	uint32_t Back;
	uint32_t Count;
	uint32_t* buffer;
}RingBuffer_t;

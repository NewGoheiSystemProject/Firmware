/*
 * RingBuffer.c
 *
 *  Created on: 2018/10/02
 *      Author: Shuji
 */

#include "RingBuffer.h"


#define BUFFER_CREATE_COUNT 100
#define BUFFER_FULL_STATE (int)-1

typedef enum{
	TRUE,
	FALSE
}bool_t;

typedef struct{
	uint32_t Size;
	uint32_t Front;
	uint32_t Back;
	uint32_t Count;
	uint32_t* buffer;
}RingBuffer_t;

static bool_t isUsed[BUFFER_CREATE_COUNT] = { FALSE };
static RingBuffer_t* bufferPointers[BUFFER_CREATE_COUNT];

static int findMinimumHandle();

RingBufferHandle_t CreateRingBuffer(uint32_t size)
{
	int minimumUnused = findMinimumHandle();

	if(minimumUnused != BUFFER_FULL_STATE){
		bufferPointers[minimumUnused] = (RingBuffer_t*)malloc(sizeof(RingBuffer_t));
		bufferPointers[minimumUnused]->buffer = (uint32_t*)malloc(size * sizeof(uint32_t));
		isUsed[minimumUnused] = TRUE;
	}

	return (RingBufferHandle_t)minimumUnused;
}
void DisposeRingBuffer(RingBufferHandle_t handle)
{
	if(isUsed[handle] == TRUE){
		free(bufferPointers[handle]->buffer);
		free(bufferPointers[handle]);
		isUsed[handle] = FALSE;
	}
}
RING_BUFFER_STATE_t GetData(RingBufferHandle_t handle, uint32_t* buffer)
{
	RING_BUFFER_STATE_t result = RING_BUFFER_SUCCESS;

	if(isUsed[handle] == FALSE){
		result = RING_BUFFER_ERROR;
	}

	if(result == RING_BUFFER_SUCCESS){
		if(bufferPointers[handle]->Count <= 0){
			result = RING_BUFFER_ERROR;
		}
	}

	if(result == RING_BUFFER_SUCCESS){
		int index = bufferPointers[handle]->Front;
		*buffer = bufferPointers[handle]->buffer[index];
		bufferPointers[handle]->Front = (++index) % bufferPointers[handle]->Size;
		bufferPointers[handle]->Count--;
	}

	return result;
}
RING_BUFFER_STATE_t AddData(RingBufferHandle_t handle, uint32_t data)
{

}
uint32_t GetCount(RingBufferHandle_t handle)
{

}
void ClearBuffer(RingBufferHandle_t handle)
{

}
static int findMinimumHandle()
{
	int result = BUFFER_FULL_STATE;

	int cnt = 0;

	for(cnt = 0; cnt < BUFFER_CREATE_COUNT; cnt++){
		if(isUsed[cnt] == FALSE){
			result = cnt;
			break;
		}
	}

	return result;
}

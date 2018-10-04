/*
 * RingBufferTest.c
 *
 *  Created on: 2018/10/05
 *      Author: Shuji
 */
#include "RingBufferTest.h"
#include "RingBuffer.h"


void RingBufferReadWriteTest()
{
	RingBufferHandle_t bufferHandle = CreateRingBuffer(30);

	RING_BUFFER_STATE_t result = AddData(bufferHandle, 1);

	if(result == RING_BUFFER_SUCCESS){
		result = AddData(bufferHandle, 2);
	}

	if(result == RING_BUFFER_SUCCESS){
		result = AddData(bufferHandle, 3);
	}

	if(result == RING_BUFFER_SUCCESS){
		result = AddData(bufferHandle, 4);
	}

	if(result == RING_BUFFER_SUCCESS){
		result = AddData(bufferHandle, 5);
	}

	if(result == RING_BUFFER_SUCCESS){
		result = AddData(bufferHandle, 6);
	}

	if(result == RING_BUFFER_SUCCESS){
		result = AddData(bufferHandle, 7);
	}

	uint32_t count = GetCount(bufferHandle);

	uint32_t readoutResult = 0;

	if(result == RING_BUFFER_SUCCESS){
		result = GetData(bufferHandle, &readoutResult);
		count = GetCount(bufferHandle);
	}

	if(result == RING_BUFFER_SUCCESS){
		result = GetData(bufferHandle, &readoutResult);
		count = GetCount(bufferHandle);
	}

	if(result == RING_BUFFER_SUCCESS){
		result = GetData(bufferHandle, &readoutResult);
		count = GetCount(bufferHandle);
	}

	if(result == RING_BUFFER_SUCCESS){
		result = GetData(bufferHandle, &readoutResult);
		count = GetCount(bufferHandle);
	}

	if(result == RING_BUFFER_SUCCESS){
		result = GetData(bufferHandle, &readoutResult);
		count = GetCount(bufferHandle);
	}

	if(result == RING_BUFFER_SUCCESS){
		DisposeRingBuffer(bufferHandle);
	}
}

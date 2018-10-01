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

RingBufferHandle_t CreateRingBuffer(uint32_t size)
{

}
void DisposeRingBuffer(RingBufferHandle_t handle)
{

}
RING_BUFFER_STATE_t GetData(RingBufferHandle_t handle, uint32_t* buffer)
{

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

/*
 * RingBuffer.h
 *
 *  Created on: 2018/10/02
 *      Author: Shuji
 */

#ifndef RINGBUFFER_INC_RINGBUFFER_H_
#define RINGBUFFER_INC_RINGBUFFER_H_

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;

typedef unsigned long RingBufferHandle_t;


typedef enum{
	RING_BUFFER_SUCCESS,
	RING_BUFFER_ERROR
}RING_BUFFER_STATE_t;


RingBufferHandle_t CreateRingBuffer(uint32_t size);
void DisposeRingBuffer(RingBufferHandle_t handle);
RING_BUFFER_STATE_t GetData(RingBufferHandle_t handle, uint32_t* buffer);
RING_BUFFER_STATE_t AddData(RingBufferHandle_t handle, uint32_t data);
uint32_t GetCount(RingBufferHandle_t handle);
void ClearBuffer(RingBufferHandle_t handle);

#endif /* RINGBUFFER_INC_RINGBUFFER_H_ */

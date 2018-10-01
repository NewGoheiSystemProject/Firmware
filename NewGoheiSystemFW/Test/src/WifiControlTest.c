/*
 * WifiControlTest.c
 *
 *  Created on: 2018/10/01
 *      Author: Shuji
 */
#include "stm32f4xx_hal.h"
#include "usart.h"

#define DMA_BUFFER_SIZE 1024
#define RESET_PULSE_WIDTH_MS 500
#define MESSAGE_BUFFER_SINZE 10

//UartRingBuffer
static uint8_t uartRxBuf[DMA_BUFFER_SIZE] = {0};
static uint16_t readPos = 0;
static uint16_t writePos = 0;
static uint16_t uartRingCount = 0;

//StringRingBuffer
static uint8_t receivedStrings[MESSAGE_BUFFER_SINZE][DMA_BUFFER_SIZE] = {0};
static uint16_t stringLength[MESSAGE_BUFFER_SINZE] = {0};
static uint8_t stringReadPos = 0;
static uint8_t stringWritePos = 0;
static uint8_t stringRingCount = 0;

static void stringBufferingTask();
static int searchString(uint8_t* stringToSearch, uint16_t length);

void WifiModuleBootTest()
{
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);
    HAL_UART_Receive_DMA(&huart3, uartRxBuf, DMA_BUFFER_SIZE);
	HAL_Delay(RESET_PULSE_WIDTH_MS);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);

	while(1){
		stringBufferingTask();
	}
}
void stringBufferingTask()
{
	if(__HAL_DMA_GET_COUNTER(&hdma_usart3_rx) < DMA_BUFFER_SIZE){
		writePos = DMA_BUFFER_SIZE - 1 - __HAL_DMA_GET_COUNTER(&hdma_usart3_rx);
		int cnt = 0;
		while(readPos < writePos){
			if(uartRxBuf[readPos] != '\r' && uartRxBuf[readPos] != '\n'){
				receivedStrings[stringWritePos][cnt] = uartRxBuf[readPos];
				cnt++;
				stringLength[stringWritePos]++;
			}
			else if(uartRxBuf[readPos] == '\r' && uartRxBuf[readPos + 1] == '\n'){
				stringWritePos++;
				cnt = 0;
			}
			else{
				cnt = 0;
			}
			readPos++;
		}
	}
}
int searchString(uint8_t* stringToSearch, uint16_t length)
{
	int result = -1;

	while(stringReadPos < stringWritePos){

	}
}

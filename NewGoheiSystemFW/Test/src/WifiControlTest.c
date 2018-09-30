/*
 * WifiControlTest.c
 *
 *  Created on: 2018/10/01
 *      Author: Shuji
 */
#include "stm32f4xx_hal.h"
#include "usart.h"

void WifiModuleBootTest()
{
	volatile uint8_t uartRxBuf[1024] = {0};
	volatile uint16_t readPos = 0;
	volatile uint16_t writePos = 0;

    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);
    HAL_UART_Receive_DMA(&huart3, uartRxBuf, 1024);
	HAL_Delay(500);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);

	volatile uint8_t receivedStrings[10][1000] = {0};
	volatile uint16_t stringLength[10] = {0};
	volatile uint8_t stringReadPos = 0;
	volatile uint8_t stringWritePos = 0;
	while(1){
		if(__HAL_DMA_GET_COUNTER(&hdma_usart3_rx) < 1024){
			writePos = 1023 - __HAL_DMA_GET_COUNTER(&hdma_usart3_rx);
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
}


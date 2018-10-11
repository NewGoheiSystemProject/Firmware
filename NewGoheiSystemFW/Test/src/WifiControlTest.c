/*
 * WifiControlTest.c
 *
 *  Created on: 2018/10/01
 *      Author: Shuji
 */
#include "stm32f4xx_hal.h"
#include "usart.h"
#include <string.h>

#define DMA_BUFFER_SIZE 1024
#define RESET_PULSE_WIDTH_MS 500
#define MESSAGE_BUFFER_SINZE 10

#define EVENT_RCV_READY          0x0001
#define EVENT_RCV_WIFI_CONNECTED 0x0002
#define EVENT_RCV_WIFI_GOT_IP    0x0004
#define EVENT_OK                 0x0008

#define EVENT_TIMEOUT            0x8000



static uint16_t eventStatus = 0;
const uint8_t STR_READY[] = "ready";
const uint8_t STR_WIFI_CONNECTED[] = "WIFI CONNECTED";
const uint8_t STR_WIFI_GOT_IP[] = "WIFI GOT IP";
const uint8_t STR_OK[] = "OK";

const uint8_t STR_CONNECTION_START_FRONT[] = "AT+CIPSTART=\"UDP\",\"";
const uint8_t STR_NTPSERVER_NAME[] = "ntp.nict.jp";
const uint8_t STR_CONNECTION_START_REAR[] = "\",123,,0";



//UartRingBuffer
static uint8_t uartRxBuf[DMA_BUFFER_SIZE] = {0};
static uint16_t readPos = 0;
static uint16_t writePos = 0;
static uint16_t uartCount = 0;

//StringRingBuffer
static uint8_t receivedStrings[MESSAGE_BUFFER_SINZE][DMA_BUFFER_SIZE] = {0};
static uint16_t stringLength[MESSAGE_BUFFER_SINZE] = {0};
static uint8_t stringReadPos = 0;
static uint8_t stringWritePos = 0;
static uint8_t stringRingCount = 0;

static int rcvCnt = 0;

static void stringBufferingTask();
static void eventCheckTask();
static void checkEventState(uint8_t* checkStr, uint16_t length);
static void clearEvent();
static void wait4Event(uint16_t eventFlag, uint32_t timeout);

void WifiModuleBootTest()
{
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);
    HAL_UART_Receive_DMA(&huart3, uartRxBuf, DMA_BUFFER_SIZE);
	HAL_Delay(RESET_PULSE_WIDTH_MS);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);

	//ready待ち
	if(!(eventStatus & EVENT_TIMEOUT)){
		wait4Event(EVENT_RCV_READY, 5000);
	}

	//wifi connect待ち
	if(!(eventStatus & EVENT_RCV_WIFI_CONNECTED)){
		wait4Event(EVENT_RCV_READY, 5000);
	}

	//wifi connect待ち
	if(!(eventStatus & EVENT_RCV_WIFI_GOT_IP)){
		wait4Event(EVENT_RCV_READY, 5000);
	}

	clearEvent();

	while(1);
}
void stringBufferingTask()
{
	if(__HAL_DMA_GET_COUNTER(&hdma_usart3_rx) < DMA_BUFFER_SIZE){
		writePos = DMA_BUFFER_SIZE - 1 - __HAL_DMA_GET_COUNTER(&hdma_usart3_rx);
		if(readPos < writePos){
			uartCount = writePos - readPos;
			rcvCnt = 0;
		}
		else if(rcvCnt > 0){
			uartCount = DMA_BUFFER_SIZE - readPos + writePos - 1;
		}

		static int cnt = 0;
		while(uartCount > 0){
			if(stringRingCount >= MESSAGE_BUFFER_SINZE){
				stringWritePos = 0;
			}
			if(uartRxBuf[readPos] != '\r' && uartRxBuf[readPos] != '\n'){
				receivedStrings[stringWritePos][cnt] = uartRxBuf[readPos];
				cnt++;
				stringLength[stringWritePos]++;
			}
			else if(uartRxBuf[readPos] == '\r' && uartRxBuf[readPos + 1] == '\n'){
				stringWritePos = (stringWritePos + 1) % MESSAGE_BUFFER_SINZE;
				stringRingCount++;
				cnt = 0;
			}
			readPos = (readPos + 1) % DMA_BUFFER_SIZE;
			uartCount --;
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
	if(huart == &huart3){
		rcvCnt++;
	}
}

void checkEventState(uint8_t* checkStr, uint16_t length)
{
	if(length == sizeof(STR_READY) - 1){
		if(memcmp(checkStr, STR_READY, length) == 0){
			eventStatus |= EVENT_RCV_READY;
		}
	}

	if(length == sizeof(STR_WIFI_CONNECTED) - 1){
		if(memcmp(checkStr, STR_WIFI_CONNECTED, length) == 0){
			eventStatus |= EVENT_RCV_WIFI_CONNECTED;
		}
	}

	if(length == sizeof(STR_WIFI_GOT_IP) - 1){
		if(memcmp(checkStr, STR_WIFI_GOT_IP, length) == 0){
			eventStatus |= EVENT_RCV_WIFI_GOT_IP;
		}
	}

	if(length == sizeof(STR_OK) - 1){
		if(memcmp(checkStr, STR_OK, length) == 0){
			eventStatus |= EVENT_OK;
		}
	}
}

void clearEvent()
{
	eventStatus = 0;
}
void eventCheckTask()
{
	if(stringRingCount > 0){
		checkEventState(receivedStrings[stringReadPos], stringLength[stringReadPos]);
		stringReadPos = (stringReadPos + 1) % MESSAGE_BUFFER_SINZE;
		stringRingCount--;
	}
}
void wait4Event(uint16_t eventFlag, uint32_t timeout)
{
	uint32_t currentTick = HAL_GetTick();

	while(!(eventStatus & eventFlag)){
		//uartrecv
		stringBufferingTask();

		//event check
		eventCheckTask();

		//check Tick
		if(HAL_GetTick() > currentTick + timeout){
			eventStatus |= EVENT_TIMEOUT;
			break;
		}
	}
}

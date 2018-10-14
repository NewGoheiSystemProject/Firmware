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
#define EVENT_CONNECTED          0x0010
#define EVENT_RECEIVE_MESSAGE    0x0020
#define EVENT_SEND_OK            0x0040

#define EVENT_TIMEOUT            0x8000



static uint16_t eventStatus = 0;
const uint8_t STR_READY[] = "ready";
const uint8_t STR_WIFI_CONNECTED[] = "WIFI CONNECTED";
const uint8_t STR_WIFI_GOT_IP[] = "WIFI GOT IP";
const uint8_t STR_OK[] = "OK";
const uint8_t STR_CONNECTED[] = ",CONNECT";
const uint8_t STR_SEND_OK[] = "SEND OK";
const uint8_t STR_RECEIVE_FROM_SERVER = "+IPD,";

const uint8_t STR_CONNECTION_START_FRONT[] = "AT+CIPSTART=\"UDP\",\"";
const uint8_t STR_NTPSERVER_NAME[] = "ntp.nict.jp";
const uint8_t STR_CONNECTION_START_REAR[] = "\",123,,0";
const uint8_t STR_CONNECTION_START_NICT[] = "AT+CIPSTART=\"UDP\",\"ntp.nict.jp\",123,,0";
const uint8_t STR_SENDCOMMAND_NICT[] = "AT+CIPSEND=48";
const uint8_t STR_NEWLINE[] = "\r\n";
const uint8_t STR_ACTUAL_COMMAND_NICT[] = { 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


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

static int connectionHandleAsClient = 0;

static int receivedDataCnt = 0;

static uint8_t receivedData[200];

static void stringBufferingTask();
static void eventCheckTask();
static void checkEventState(uint8_t* checkStr, uint16_t length);
static void clearEvent();
static void wait4Event(uint16_t eventFlag, uint32_t timeout);

static void sendMessage(uint8_t* message, uint16_t length);

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
void WifiNTPTest()
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
	if(!(eventStatus & EVENT_TIMEOUT)){
		wait4Event(EVENT_RCV_WIFI_CONNECTED, 5000);
	}

	//wifi connect待ち
	if(!(eventStatus & EVENT_TIMEOUT)){
		wait4Event(EVENT_RCV_WIFI_GOT_IP, 5000);
	}


	if(!(eventStatus & EVENT_TIMEOUT)){
		//nict.jpのNTPに接続
		sendMessage((uint8_t*)STR_CONNECTION_START_NICT, sizeof(STR_CONNECTION_START_NICT) - 1);
	}

	if(!(eventStatus & EVENT_TIMEOUT)){
		//nictへの接続成功待ち
		wait4Event(EVENT_CONNECTED, 5000);
	}

	if(!(eventStatus & EVENT_TIMEOUT)){
		//OK待ち
		wait4Event(EVENT_OK, 5000);
	}

	if(!(eventStatus & EVENT_TIMEOUT)){
		//nictへのコマンド送信準備
		sendMessage(STR_SENDCOMMAND_NICT, sizeof(STR_SENDCOMMAND_NICT) - 1);
	}

	if(!(eventStatus & EVENT_TIMEOUT)){
		//OK待ち
		wait4Event(EVENT_OK, 5000);
	}

	if(!(eventStatus & EVENT_TIMEOUT)){
		//nictへのコマンド送信
		sendMessage(STR_ACTUAL_COMMAND_NICT, sizeof(STR_ACTUAL_COMMAND_NICT));
	}

	clearEvent();
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

	if(strstr((const char*)checkStr, (const char*)STR_CONNECTED) != NULL){
		eventStatus |= EVENT_CONNECTED;
		char* strOfHandle = strtok((char*)checkStr, ",");

		if(strOfHandle != NULL){//数値に変換
			connectionHandleAsClient = atoi((const char*)strOfHandle);
		}
	}

	if(strstr((const char*)checkStr, (const char*)STR_RECEIVE_FROM_SERVER) != NULL){
		eventStatus |= EVENT_RECEIVE_MESSAGE;
		//文字数取得
		char* commaPos = strtok((char*)checkStr, ",");
		char* colonPos = strtok((char*)commaPos, ":");

		char countChar[10];
		int countLength = (int)(colonPos - commaPos);

		int cnt = 0;
		for(cnt = 0; cnt < countLength; cnt++){
			countChar[cnt] = commaPos[cnt];
		}
		countChar[cnt] = '\0';
		receivedDataCnt = atoi((const char*)countChar);

		//文字列取得
		receivedData = (uint8_t*)colonPos;
	}

	if(length == sizeof(STR_SEND_OK) - 1){
		if(memcmp(checkStr, STR_SEND_OK, length) == 0){
			eventStatus |= EVENT_SEND_OK;
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
void sendMessage(uint8_t* message, uint16_t length)
{
	HAL_StatusTypeDef result = HAL_UART_Transmit(&huart3, message, length, 1000);

	if(result == HAL_OK){
		result = HAL_UART_Transmit(&huart3, STR_NEWLINE, sizeof(STR_NEWLINE) - 1, 1000);
	}

	if(result != HAL_OK){
		eventStatus |= EVENT_TIMEOUT;
	}
}

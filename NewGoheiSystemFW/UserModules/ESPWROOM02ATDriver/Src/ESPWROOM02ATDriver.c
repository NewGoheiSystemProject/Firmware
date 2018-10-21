/*
 * ESPWROOM02ATDriver.c
 *
 *  Created on: 2018/10/20
 *      Author: Shuji
 */
#include "ESPWROOM02ATDriver.h"
#include "ESPWROOM02EVENTDef.h"
#include "ESPWROOM02ATCommands.h"
#include <string.h>

#define MESSAGE_BUFFER_SIZE 10
#define MESSAGE_MAX_LENGTH 1000

#define DMA_BUFFER_SIZE 1024

static uint8_t receivedStrings[MESSAGE_BUFFER_SIZE][MESSAGE_MAX_LENGTH] = {0};
static uint16_t stringLength[MESSAGE_BUFFER_SIZE] = {0};
static uint8_t stringReadPos = 0;
static uint8_t stringWritePos = 0;
static uint8_t stringRingCount = 0;

static int ipdReceivedCount = 0;
static uint8_t ipdReceivedString[MESSAGE_MAX_LENGTH];

static uint16_t eventStatus = 0;
static int connectionHandleAsClient = 0;

//StringRingBuffer
static void stringBufferingTask();
static void eventCheckTask();
static void checkEventState(uint8_t* checkStr, uint16_t length);
static void clearEvent();
static void wait4Event(uint16_t eventFlag, uint32_t timeout);

void Reboot()
{
	//ピンを下ろす
	AssertResetPin();

	//パルス幅分待つ
	Wait4ResetPulse();

	//ピンを上げる
	NegateResetPin();
}
void EnableServer()
{

}
void DisableServer()
{

}
void EnableClient()
{

}
void DisableClient()
{

}
void ConnectToAP(const char* APName, const char* APPass)
{

}
void stringBufferingTask()
{
	//UartRingBuffer
	static uint8_t uartRxBuf[DMA_BUFFER_SIZE] = {0};
	static uint16_t readPos = 0;

	//最新データポジションの取得
	uint16_t writePos = GetUartDMAPosition();

	//データサイズの計算
	uint16_t uartCount = 0;

	if(readPos < writePos){//バッファ境界を跨いでいないときは普通に減算
		uartCount = writePos - readPos + 1;
	}
	else if(readPos == writePos){
		uartCount = 0;
	}
	else{//バッファ境界を跨いだ場合はその計算
		uartCount = DMA_BUFFER_SIZE - (readPos - writePos + 1);
	}

	if(uartCount > 0){//DMAにデータがあるときのみ
		static int stringBufCharPos = 0;
		static int ipdmodeFlag = 0;
		static int ipdCountGotFlag = 0;
		static int ipdStringLength = 0;
		static int ipdHeaderStringLength = 0;



		//現在DMAがバッファに格納済みの分を全て処理
		int cnt = 0;
		for(cnt = 0; cnt < uartCount; cnt++){
			if(uartRxBuf[readPos] != '\r' && uartRxBuf[readPos] != '\n'){//改行コード以外は文字列バッファに格納
				receivedStrings[stringWritePos][stringBufCharPos++] = uartRxBuf[readPos];
			}
			else if(uartRxBuf[readPos] == '\r' && uartRxBuf[readPos + 1] == '\n'){
				stringLength[stringWritePos] = stringBufCharPos;//文字列長格納
				stringWritePos = (stringWritePos + 1) % MESSAGE_BUFFER_SIZE;
				stringRingCount++;
				stringBufCharPos = 0;
			}

			if(stringBufCharPos == 5 &&
				receivedStrings[stringWritePos][0] == '+' &&
				receivedStrings[stringWritePos][1] == 'I' &&
				receivedStrings[stringWritePos][2] == 'P' &&
				receivedStrings[stringWritePos][3] == 'D' &&
				receivedStrings[stringWritePos][4] == ','){//+IPDの後の受信状態に入る
				ipdmodeFlag = 1;
			}

			if(ipdmodeFlag == 1 && ipdCountGotFlag == 0){//IPDモードで, IPD文字列のカウントが取得できていない場合
				if(receivedStrings[stringWritePos][stringBufCharPos - 1] == ':'){//最初の':'を発見したら
					//ヘッダ文字列長を取得
					ipdHeaderStringLength = stringBufCharPos;

					//IPD文字列長を示す文字列から数値を算出
					char str4IpdmodeCount[10];
					int k = 0;
					for(k = 0; k < ipdHeaderStringLength - 1 - 5; k++){//+IPD,と:の分
						str4IpdmodeCount[k] = receivedStrings[stringWritePos][5 + k];
					}
					str4IpdmodeCount[k] = '\0';
					ipdStringLength = atoi((const char*)str4IpdmodeCount);

					ipdCountGotFlag = 1;
				}
			}

			if(ipdmodeFlag == 1 && stringBufCharPos == ipdHeaderStringLength + ipdStringLength){//IPDModeでの文字列受信終了判定
				stringLength[stringWritePos] = stringBufCharPos;//文字列長格納
				stringWritePos = (stringWritePos + 1) % MESSAGE_BUFFER_SIZE;
				stringRingCount++;
				stringBufCharPos = 0;

				//IPD受信モードから抜ける
				ipdmodeFlag = 0;
				ipdCountGotFlag = 0;
				ipdStringLength = 0;
				ipdHeaderStringLength = 0;
			}
			readPos = (readPos + 1) % DMA_BUFFER_SIZE;
		}
		uartCount = 0;
	}
}
void checkEventState(uint8_t* checkStr, uint16_t length)
{
	if(memcmp((const char*)checkStr, REP_READY, length) == 0){//ready受信イベント
		eventStatus |= EVENT_RCV_READY;
	}

	if(memcmp((const char*)checkStr, REP_WIFI_CONNECTED, length) == 0){//WIFI CONNECTEDイベント
		eventStatus |= EVENT_RCV_WIFI_CONNECTED;
	}

	if(memcmp((const char*)checkStr, REP_WIFI_GOT_IP, length) == 0){//WIFI GOTIPイベント
		eventStatus |= EVENT_RCV_WIFI_GOT_IP;
	}

	if(memcmp((const char*)checkStr, REP_OK, length) == 0){//OKイベント
		eventStatus |= EVENT_OK;
	}


	if(strstr((const char*)checkStr, REP_CONNECTED) != NULL){
		eventStatus |= EVENT_CONNECTED;
		char* strOfHandle = strtok((char*)checkStr, ",");

		if(strOfHandle != NULL){//数値に変換
			connectionHandleAsClient = atoi((const char*)strOfHandle);
		}
	}

	if(strstr((const char*)checkStr, REP_RECEIVE_FROM_SERVER) != NULL){
		eventStatus |= EVENT_RECEIVE_MESSAGE;
		//文字数取得
		char str4Count[10];

		int startPos = 0;
		int endPos = 0;
		int i = 0;
		while(i < length){
			if(checkStr[i] == ','){
				startPos = i + 1;
			}

			if(checkStr[i] == ':'){
				endPos = i - 1;
			}

			if(endPos > startPos){
				break;
			}
			i++;
		}

		int j = 0;
		for(j = 0; j < endPos - startPos + 1; j++){
			str4Count[j] = checkStr[startPos + j];
		}
		str4Count[j] = '\0';
		ipdReceivedCount = atoi((const char*)str4Count);

		//文字列取得
		int cnt = 0;
		for(cnt = 0; cnt < ipdReceivedCount; cnt++){
			ipdReceivedString[cnt] = checkStr[endPos + 1 + 1 + cnt];
		}
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
		stringReadPos = (stringReadPos + 1) % MESSAGE_BUFFER_SIZE;
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

	if(!(eventStatus & EVENT_TIMEOUT)){
		eventStatus &= (~eventFlag);
	}
}
void sendMessage(uint8_t* message, uint16_t length)
{
	HAL_StatusTypeDef result = HAL_UART_Transmit(&huart3, message, length, 1000);

	if(result != HAL_OK){
		eventStatus |= EVENT_TIMEOUT;
	}
}

__attribute__((weak)) void AssertResetPin()
{

}
__attribute__((weak)) void NegateResetPin()
{

}
__attribute__((weak)) void Wait4ResetPulse()
{

}
__attribute__((weak)) uint32_t GetUartDMAPosition()
{
	return 0;
}
uint32_t GetUartDMABufferSize()
{
	return DMA_BUFFER_SIZE;
}

/*
 * BoardControler.c
 *
 *  Created on: 2018/01/22
 *      Author: Shuji
 */
#include "BoardControler.h"
#include "gpio.h"
#include "adc.h"
#include "tim.h"

#define RELAY1_ANODE_PORT (GPIOA)
#define RELAY2_ANODE_PORT (GPIOA)
#define RELAY3_ANODE_PORT (GPIOA)
#define RELAY1_ANODE_PIN (GPIO_PIN_1)
#define RELAY2_ANODE_PIN (GPIO_PIN_2)
#define RELAY3_ANODE_PIN (GPIO_PIN_3)
#define RELAY_STATE_ON GPIO_PIN_SET
#define RELAY_STATE_OFF GPIO_PIN_RESET

typedef GPIO_PinState Relay_State_t;


static void setRelay1State(Relay_State_t relayState);
static void toggleRelay1State();
static void setRelay2State(Relay_State_t relayState);
static void toggleRelay2State();
static void setRelay3State(Relay_State_t relayState);
static void toggleRelay3State();

static void HeaterTimerCallBack();
#define HEATER_FLAG_OFF 0
#define HEATER_FLAG_ON 1

static void FanTimerCallBack();
#define FAN_FLAG_OFF 0
#define FAN_FLAG_ON 1

static uint16_t HeaterCounter = 0;
static int HeaterFlag = 0;

static uint16_t FanCounter = 0;
static int FanFlag = 0;

#define STATE_CONTROLING 1
#define STATE_NO_CONTROLING 0
static int isControling();

#define MAX_CONTROL_DURATION 60//最大を60sec制御とする

static double Coef_prop = 0.5;//暫定
static double Coef_int = 0.2;//暫定
static double Coef_dif =0.3;//暫定


static void pidControl();
#define ROOM_TEMP (25 * 0xFFF / 2)
static uint32_t SetTemperature = ROOM_TEMP;
static int32_t IntegralVal = 0;

static void tempControlCallBack();

void tempControlCallBack()
{
	HeaterTimerCallBack();

	FanTimerCallBack();
}

int isControling()
{
	int result = STATE_NO_CONTROLING;

	if(HeaterCounter > 0 || FanCounter > 0){
		result = STATE_CONTROLING;
	}

	return result;
}

void HeaterTimerCallBack()
{
	if(HeaterCounter > 0){
		HeaterCounter--;
	}

	if(HeaterCounter == 0){
		HeaterFlag = HEATER_FLAG_OFF;
		TIM3Stop();
	}

}
void FanTimerCallBack()
{
	if(FanCounter > 0){
		FanCounter--;
	}

	if(FanCounter == 0){
		FanFlag = FAN_FLAG_OFF;
		TIM3Stop();
	}

}

void setRelay1State(Relay_State_t relayState)
{
	HAL_GPIO_WritePin(RELAY1_ANODE_PORT, RELAY1_ANODE_PIN, relayState);
}
void toggleRelay1State()
{
	HAL_GPIO_TogglePin(RELAY1_ANODE_PORT, RELAY1_ANODE_PIN);
}
void setRelay2State(Relay_State_t relayState)
{
	HAL_GPIO_WritePin(RELAY2_ANODE_PORT, RELAY2_ANODE_PIN, relayState);
}
void toggleRelay2State()
{
	HAL_GPIO_TogglePin(RELAY2_ANODE_PORT, RELAY2_ANODE_PIN);
}
void setRelay3State(Relay_State_t relayState)
{
	HAL_GPIO_WritePin(RELAY3_ANODE_PORT, RELAY3_ANODE_PIN, relayState);
}
void toggleRelay3State()
{
	HAL_GPIO_TogglePin(RELAY3_ANODE_PORT, RELAY3_ANODE_PIN);
}
void UVOn()
{
	setRelay1State(RELAY_STATE_ON);
}
void UVOff()
{
	setRelay1State(RELAY_STATE_OFF);
}
void HeaterSet(uint16_t duration_sec)
{
	HeaterCounter = duration_sec;
	HeaterFlag = HEATER_FLAG_ON;
	TIM3Start();
}
void NaturalCoolingSet(uint16_t duration_sec)
{
	FanCounter = duration_sec;
	TIM3Start();
}
void HeaterOff()
{
	setRelay2State(RELAY_STATE_OFF);
}
void HeaterOn()
{
	setRelay2State(RELAY_STATE_ON);
}
void FanSet(uint16_t duration_sec)
{

}
void FanOff()
{
	setRelay3State(RELAY_STATE_OFF);
}
void FanOn()
{
	setRelay3State(RELAY_STATE_ON);
}
void BoardInitialize()
{
	TIM3IRQAttach(tempControlCallBack);
}
void pidControl(uint32_t newTempData, int32_t diff)
{
	//比例値
	int32_t prop = ((int32_t)SetTemperature - (int32_t)newTempData);

	//積分偏差を計算
	IntegralVal += diff;

	//計算
	double pidCal = (Coef_prop * (double)prop + Coef_int * (double)IntegralVal + Coef_dif * diff) / (double)SetTemperature;

	if(pidCal > 1){//MAXにさちらせる
		pidCal = 1;
	}
	else if(pidCal < -1){//MINにさちらせる
		pidCal = -1;
	}

	if(pidCal > 0){//温める方向ならヒータ
		HeaterSet((uint16_t)(pidCal * MAX_CONTROL_DURATION));
	}
	else if(pidCal == -1){//冷ます方向にさちったならファン
		FanSet((uint16_t)(-pidCal * MAX_CONTROL_DURATION));
	}
	else{//冷ます方向なら放置
		NaturalCoolingSet((uint16_t)(-pidCal * MAX_CONTROL_DURATION));
	}
}
void BoardTask()
{
	//温度測定
	static uint32_t prevData = 0;
	ADC1_StartConv();
	while(isADC1_Finished() == ADC_CONVERTING);
	uint32_t data = ADC1_GetData();

	//動作判定
	if(isControling() == STATE_NO_CONTROLING){//制御中でなければ
		pidControl(data, prevData - data);
	}

	//前回温度の更新
	prevData = data;

	//ヒータ制御
	if(HeaterFlag == HEATER_FLAG_ON){
		HeaterOn();
	}
	else{
		HeaterOff();
	}

	//Fan制御
	if(HeaterFlag == HEATER_FLAG_ON){
		HeaterOn();
	}
	else{
		HeaterOff();
	}

	//表示更新
}

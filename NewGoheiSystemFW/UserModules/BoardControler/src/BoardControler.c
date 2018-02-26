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
#include "ThermistorCalc.h"
#include "LCDDisplayDriver.h"
#include <string.h>

#define RELAY1_ANODE_PORT (GPIOA)
#define RELAY2_ANODE_PORT (GPIOA)
#define RELAY3_ANODE_PORT (GPIOA)
#define RELAY1_ANODE_PIN (GPIO_PIN_1)
#define RELAY2_ANODE_PIN (GPIO_PIN_2)
#define RELAY3_ANODE_PIN (GPIO_PIN_3)
#define RELAY_STATE_ON GPIO_PIN_SET
#define RELAY_STATE_OFF GPIO_PIN_RESET

#define UVSWITCH_PORT (GPIOC)
#define UVSWITCH_PIN (GPIO_PIN_2)

#define SETTINGSWITCH_PORT (GPIOD)
#define SETTINGSWITCH_PIN (GPIO_PIN_7)

#define SETTING_TEMP_UP_PORT (GPIOD)
#define SETTING_TEMP_UP_PIN (GPIO_PIN_3)

#define SETTING_TEMP_DOWN_PORT (GPIOD)
#define SETTING_TEMP_DOWN_PIN (GPIO_PIN_1)

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

static double Coef_prop = 0.8;//暫定
static double Coef_int = 0.1;//暫定
static double Coef_dif =0.1;//暫定


static void pidControl(double newTempData, double diff);
#define ROOM_TEMP (double)25.0
static double SetTemperature = ROOM_TEMP;
static double IntegralVal = 0;

static void tempControlCallBack();

static void indicateTemperature(double temperature);
static void indicateAction(char* actionName);

#define USB_COMMAND_LENGTH 5

static void usbCommandAction(uint8_t* command);

#define UV_ON 1
#define UV_OFF 0

#define TEMP_UP 1
#define TEMP_DOWN 0

static int LCDUpdateFlag = 1;

static int readUVSwitch();

static int readSettingSwitch();

static void buttonTempUpCallBack();
static void buttonTempDownCallBack();

static int antiChatteringFlag = 0;
static void antiChatteringCallBack();

static int currentMode = 0;

void lcdControlCallBack()
{
	LCDUpdateFlag = 1;
}
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
		if(HeaterCounter == 0){
			HeaterFlag = HEATER_FLAG_OFF;
			TIM3Stop();
		}
	}
}
void FanTimerCallBack()
{
	if(FanCounter > 0){
		FanCounter--;
		if(FanCounter == 0){
			FanFlag = FAN_FLAG_OFF;
			TIM3Stop();
		}
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
	FanCounter = duration_sec;
	TIM3Start();
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
	initialize_LCDDisplayDriver();

	TIM2IRQAttach(lcdControlCallBack);

	TIM4IRQAttach(antiChatteringCallBack);

	IRQAttach_GPIO(SETTING_TEMP_DOWN_PIN, buttonTempDownCallBack);
	IRQAttach_GPIO(SETTING_TEMP_UP_PIN, buttonTempUpCallBack);
}
void pidControl(double newTempData, double diff)
{
	//比例値
	double prop = SetTemperature - newTempData;

	//積分偏差を計算
	IntegralVal += prop;

	//計算
	double pidCal = (Coef_prop * prop + Coef_int * IntegralVal + Coef_dif * diff) / SetTemperature;

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
	if(readSettingSwitch() == 0){
		currentMode = 0;

		//温度測定
		static double prevTemperature = 0;
		uint32_t adcData = ADC1_OneshotConv();
		double thermistorResistance = (double)adcData / ((double)0xFFF - (double)adcData + 1.0) * GetSeriesResistance();
		double temperatureData = CalcurateTemperature_ThermistorCalc(thermistorResistance);


		//動作判定
		if(isControling() == STATE_NO_CONTROLING){//制御中でなければ
			pidControl(temperatureData, prevTemperature - temperatureData);
		}

		//前回温度の更新
		prevTemperature = temperatureData;

		//ヒータ制御
		if(HeaterFlag == HEATER_FLAG_ON){
			HeaterOn();
		}
		else{
			HeaterOff();
		}

		//Fan制御
		if(FanFlag == FAN_FLAG_ON){
			FanOn();
		}
		else{
			FanOff();
		}

		//表示更新
		if(LCDUpdateFlag == 1){
			clearChar_LCDDisplayDriver();
			indicateTemperature(temperatureData);
			char actionName[256];
			if(HeaterFlag == HEATER_FLAG_ON){
				sprintf(actionName, "Heating");
			}
			else if(FanFlag == FAN_FLAG_ON){
				sprintf(actionName, "Fan Cooling");
			}
			else{
				sprintf(actionName, "Natural Cooling");
			}
			indicateAction(actionName);

			LCDUpdateFlag = 0;
			TIM2Start();
		}

		//UVSwitch
		if(readUVSwitch() == 1){
			UVOn();
		}
		else{
			UVOff();
		}
	}

}
void indicateTemperature(double temperature)
{
	//整数部抽出
	int frontOfPoint = (int)temperature;

	//少数部2桁まで抽出
	int backOfPoint = (int)((temperature - (double)frontOfPoint) * 100);
	if(backOfPoint < 0){
		backOfPoint = -backOfPoint;
	}

	//文字列作成
	char str[100];
	sprintf(str, "%d.%d", frontOfPoint, backOfPoint);

	//バイト列作成
	uint8_t bytesToIndicate[100];
	int count = 0;
	while(str[count] != '\0'){
		bytesToIndicate[count] = (uint8_t)str[count];
		count++;
	}
	bytesToIndicate[count++] = 0xF2;//上の小丸
	bytesToIndicate[count++] = 'C';//度CのC

	//表示
	setChar_LCDDisplayDriver(bytesToIndicate, count, 1);
}
void indicateAction(char* actionName)
{
	uint8_t bytesToIndicate[100];

	int cnt = 0;
	for(cnt = 0; cnt < strlen(actionName); cnt++){
		bytesToIndicate[cnt] = (uint8_t)actionName[cnt];
	}

	setChar_LCDDisplayDriver(bytesToIndicate, strlen(actionName), 2);
}
void USBTask()
{

}
void usbCommandAction(uint8_t* command)
{

}

int readUVSwitch()
{
	int result = 0;

	if(HAL_GPIO_ReadPin(UVSWITCH_PORT, UVSWITCH_PIN) == GPIO_PIN_SET){
		result = 1;
	}

	return result;
}
void SettingTask()
{
	if(readSettingSwitch() == 1){
		currentMode = 1;
		//表示更新
		if(LCDUpdateFlag == 1){
			clearChar_LCDDisplayDriver();
			indicateTemperature(SetTemperature);

			LCDUpdateFlag = 0;
			TIM2Start();
		}
	}

}
int readSettingSwitch()
{
	int result = 0;

	if(HAL_GPIO_ReadPin(SETTINGSWITCH_PORT, SETTINGSWITCH_PIN) == GPIO_PIN_SET){
		result = 1;
	}

	return result;
}
void buttonTempUpCallBack()
{
	if(currentMode == 1){
		if(antiChatteringFlag == 0){
			if(SetTemperature <= 30){
				SetTemperature++;
			}
			antiChatteringFlag = 1;
			TIM4Start();
		}
	}

}
void buttonTempDownCallBack()
{
	if(currentMode == 1)
	{
		if(antiChatteringFlag == 0){
			if(SetTemperature >= 22){
				SetTemperature--;
			}
			antiChatteringFlag = 1;
			TIM4Start();
		}
	}
}
void antiChatteringCallBack()
{
	antiChatteringFlag = 0;
	TIM4Stop();
}

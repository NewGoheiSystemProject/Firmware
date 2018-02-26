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

/***********************型定義**********************************/
typedef enum{
	TRUE,
	FALSE
}bool;
typedef enum{
	MEASURING_MODE,
	SETTING_MODE
}mode;

/************************Pinの定義******************************/
#define UV_CONTROL_PORT (GPIOA)
#define UV_CONTROL_PIN (GPIO_PIN_1)

#define HEATER_CONTROL_PORT (GPIOA)
#define HEATER_CONTROL_PIN (GPIO_PIN_2)

#define FAN_CONTROL_PORT (GPIOA)
#define FAN_CONTROL_PIN (GPIO_PIN_3)

#define UVSWITCH_PORT (GPIOC)
#define UVSWITCH_PIN (GPIO_PIN_2)

#define SETTINGSWITCH_PORT (GPIOD)
#define SETTINGSWITCH_PIN (GPIO_PIN_7)

#define SETTING_TEMP_UP_PORT (GPIOD)
#define SETTING_TEMP_UP_PIN (GPIO_PIN_3)

#define SETTING_TEMP_DOWN_PORT (GPIOD)
#define SETTING_TEMP_DOWN_PIN (GPIO_PIN_1)

/*******************定数**************************/
#define MAX_CONTROL_DURATION 60//最大を60sec制御とする
#define ROOM_TEMP (double)25.0
#define SET_TEMP_LOWER_LIMIT 22
#define SET_TEMP_UPPER_LIMIT 30

/*******************static変数************************/
static uint16_t HeaterCounter = 0;
static bool HeaterFlag = FALSE;
static uint16_t FanCounter = 0;
static bool FanFlag = FALSE;
static double Coef_prop = 0.8;//暫定
static double Coef_int = 0.0;//暫定
static double Coef_dif = 0.0;//暫定
static double SetTemperature = ROOM_TEMP;
static double IntegralVal = 0;
static bool LCDUpdateFlag = TRUE;
static bool antiChatteringFlag = FALSE;
static mode currentMode = 0;

static void HeaterTimerCallBack();
static void FanTimerCallBack();
static bool isControling();
static void pidControl(double newTempData, double diff);
static void tempControlCallBack();
static void indicateTemperature(double temperature);
static void indicateAction(char* actionName);
static bool readUVSwitch();
static bool readSettingSwitch();
static void buttonTempUpCallBack();
static void buttonTempDownCallBack();
static void antiChatteringCallBack();
static void lcdControlCallBack();



void lcdControlCallBack()
{
	LCDUpdateFlag = TRUE;
}
void tempControlCallBack()
{
	HeaterTimerCallBack();

	FanTimerCallBack();
}
bool isControling()
{
	bool result = FALSE;

	if(HeaterCounter > 0 || FanCounter > 0){
		result = TRUE;
	}

	return result;
}
void HeaterTimerCallBack()
{
	if(HeaterCounter > 0){
		HeaterCounter--;
		if(HeaterCounter == 0){
			HeaterFlag = FALSE;
			TIM3Stop();
		}
	}
}
void FanTimerCallBack()
{
	if(FanCounter > 0){
		FanCounter--;
		if(FanCounter == 0){
			FanFlag = FALSE;
			TIM3Stop();
		}
	}
}
void UVOn()
{
	HAL_GPIO_WritePin(UV_CONTROL_PORT, UV_CONTROL_PIN, GPIO_PIN_SET);
}
void UVOff()
{
	HAL_GPIO_WritePin(UV_CONTROL_PORT, UV_CONTROL_PIN, GPIO_PIN_RESET);
}
void HeaterSet(uint16_t duration_sec)
{
	HeaterCounter = duration_sec;
	HeaterFlag = TRUE;
	TIM3Start();
}
void NaturalCoolingSet(uint16_t duration_sec)
{
	FanCounter = duration_sec;
	TIM3Start();
}
void HeaterOff()
{
	HAL_GPIO_WritePin(HEATER_CONTROL_PORT, HEATER_CONTROL_PIN, GPIO_PIN_RESET);
}
void HeaterOn()
{
	HAL_GPIO_WritePin(HEATER_CONTROL_PORT, HEATER_CONTROL_PIN, GPIO_PIN_SET);
}
void FanSet(uint16_t duration_sec)
{
	FanCounter = duration_sec;
	TIM3Start();
}
void FanOff()
{
	HAL_GPIO_WritePin(FAN_CONTROL_PORT, FAN_CONTROL_PIN, GPIO_PIN_RESET);
}
void FanOn()
{
	HAL_GPIO_WritePin(FAN_CONTROL_PORT, FAN_CONTROL_PIN, GPIO_PIN_SET);
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
	if(readSettingSwitch() == FALSE){
		currentMode = MEASURING_MODE;

		//温度測定
		static double prevTemperature = 0;
		uint32_t adcData = ADC1_OneshotConv();
		double thermistorResistance = (double)adcData / ((double)0xFFF - (double)adcData + 1.0) * GetSeriesResistance();
		double temperatureData = CalcurateTemperature_ThermistorCalc(thermistorResistance);


		//動作判定
		if(isControling() == FALSE){//制御中でなければ
			pidControl(temperatureData, prevTemperature - temperatureData);
		}

		//前回温度の更新
		prevTemperature = temperatureData;

		//ヒータ制御
		if(HeaterFlag == TRUE){
			HeaterOn();
		}
		else{
			HeaterOff();
		}

		//Fan制御
		if(FanFlag == TRUE){
			FanOn();
		}
		else{
			FanOff();
		}

		//表示更新
		if(LCDUpdateFlag == TRUE){
			clearChar_LCDDisplayDriver();
			indicateTemperature(temperatureData);
			char actionName[256];
			if(HeaterFlag == TRUE){
				sprintf(actionName, "Heating");
			}
			else if(FanFlag == TRUE){
				sprintf(actionName, "Fan Cooling");
			}
			else{
				sprintf(actionName, "Natural Cooling");
			}
			indicateAction(actionName);

			LCDUpdateFlag = FALSE;
			TIM2Start();
		}

		//UVSwitch
		if(readUVSwitch() == TRUE){
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
bool readUVSwitch()
{
	bool result = FALSE;

	if(HAL_GPIO_ReadPin(UVSWITCH_PORT, UVSWITCH_PIN) == GPIO_PIN_SET){
		result = TRUE;
	}

	return result;
}
void SettingTask()
{
	if(readSettingSwitch() == TRUE){
		currentMode = SETTING_MODE;
		//表示更新
		if(LCDUpdateFlag == TRUE){
			clearChar_LCDDisplayDriver();
			indicateTemperature(SetTemperature);

			LCDUpdateFlag = FALSE;
			TIM2Start();
		}
	}

}
bool readSettingSwitch()
{
	bool result = FALSE;

	if(HAL_GPIO_ReadPin(SETTINGSWITCH_PORT, SETTINGSWITCH_PIN) == GPIO_PIN_SET){
		result = TRUE;
	}

	return result;
}
void buttonTempUpCallBack()
{
	if(currentMode == SETTING_MODE){
		if(antiChatteringFlag == FALSE){
			if(SetTemperature <= SET_TEMP_UPPER_LIMIT){
				SetTemperature++;
			}
			antiChatteringFlag = TRUE;
			TIM4Start();
		}
	}
}
void buttonTempDownCallBack()
{
	if(currentMode == SETTING_MODE)
	{
		if(antiChatteringFlag == FALSE){
			if(SetTemperature >= SET_TEMP_LOWER_LIMIT){
				SetTemperature--;
			}
			antiChatteringFlag = TRUE;
			TIM4Start();
		}
	}
}
void antiChatteringCallBack()
{
	antiChatteringFlag = FALSE;
	TIM4Stop();
}

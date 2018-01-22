/*
 * ThermistorCalc.c
 *
 *  Created on: 2018/01/22
 *      Author: Shuji
 */
#include "ThermistorCalc.h"
#include "math.h"

//! サーミスタB定数
static const double B_CONSTANT = 3380;

//! サーミスタに直列に入る抵抗値
static const double RP = 10000;

//! サーミスタのROOM_TEMPERATUREでの抵抗値
static const double R0 = 10000;

//! 絶対零度の摂氏温度
static const double ZERO_KELVIN = -273.15;

//! R0測定時の温度
static const double ROOM_TEMPERATURE = 25.0;

double CalcurateTemperature_ThermistorCalc(double thermistorResistance)
{
	double temp = 0;

	double thermistorVoltRatio = 0;
	thermistorVoltRatio = thermistorResistance / (RP + thermistorResistance);

	temp = 1 / ((log(thermistorVoltRatio/(1 - thermistorVoltRatio))
			+ log(RP / R0)) / B_CONSTANT + 1 / (ROOM_TEMPERATURE - ZERO_KELVIN)) + ZERO_KELVIN;

	return temp;
}

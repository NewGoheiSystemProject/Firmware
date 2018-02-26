/*
 * ThermistorCalc.h
 *
 *  Created on: 2018/01/22
 *      Author: Shuji
 */

#ifndef THERMISTORCALC_INC_THERMISTORCALC_H_
#define THERMISTORCALC_INC_THERMISTORCALC_H_

double CalcurateTemperature_ThermistorCalc(double thermistorResistance);
double CalcurateResistance_ThermistorCalc(double temperatureData);
double GetSeriesResistance();
#endif /* THERMISTORCALC_INC_THERMISTORCALC_H_ */

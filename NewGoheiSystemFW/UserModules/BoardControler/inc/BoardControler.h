/*
 * BoardControler.h
 *
 *  Created on: 2018/01/22
 *      Author: Shuji
 */

#ifndef BOARDCONTROLER_INC_BOARDCONTROLER_H_
#define BOARDCONTROLER_INC_BOARDCONTROLER_H_

#include "gpio.h"

void BoardInitialize();
void UVOn();
void UVOff();
void HeaterSet(uint16_t duration_sec);
void NaturalCoolingSet(uint16_t duration_sec);
void HeaterOff();
void HeaterOn();
void FanSet(uint16_t duration_sec);
void FanOff();
void FanOn();
void BoardTask();
void USBTask();
#endif /* BOARDCONTROLER_INC_BOARDCONTROLER_H_ */

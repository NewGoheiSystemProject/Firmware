/*
 * LCDDisplayDriver.h
 *
 *  Created on: 2018/02/10
 *      Author: Shuji
 */

#ifndef LCDDISPLAYDRIVER_INC_LCDDISPLAYDRIVER_H_
#define LCDDISPLAYDRIVER_INC_LCDDISPLAYDRIVER_H_

void initialize_LCDDisplayDriver();
void setChar_LCDDisplayDriver(char* str, uint8_t line);
void clearChar_LCDDisplayDriver();
#endif /* LCDDISPLAYDRIVER_INC_LCDDISPLAYDRIVER_H_ */

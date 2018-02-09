/*
 * LCDDriverTest.c
 *
 *  Created on: 2018/02/10
 *      Author: Shuji
 */

#include "LCDDriverTest.h"
#include "LCDDisplayDriver.h"

void LCDDriverTest()
{
	initialize_LCDDisplayDriver();

	uint8_t testData[] = {'t', 'e', 's', 't'};
	setChar_LCDDisplayDriver(testData, sizeof(testData),1);
}

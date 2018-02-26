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

	uint8_t testData2[] = {'t', 'e', 's', 't', '2'};
	setChar_LCDDisplayDriver(testData2, sizeof(testData2),2);

	uint8_t testData3[] = {'t', 'e', 's', 't', '3'};
	setChar_LCDDisplayDriver(testData3, sizeof(testData3),1);

	uint8_t testData4[] = {'2', '5', '.', '5', 0xF2, 'C'};
	setChar_LCDDisplayDriver(testData4, sizeof(testData4),1);
}

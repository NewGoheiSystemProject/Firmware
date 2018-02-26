/*
 * boardControlTest.c
 *
 *  Created on: 2018/02/01
 *      Author: Shuji
 */
#include "boardControlTest.h"

#include "BoardControler.h"

void boardControlTaskTest()
{
	BoardInitialize();

	while(1){
		BoardTask();
		SettingTask();
	}
}

/*
 * LCDDisplayDriver.c
 *
 *  Created on: 2018/02/10
 *      Author: Shuji
 */
#include "LCDDisplayDriver.h"
#include "i2c.h"

#define DEVICE_ADDRESS 0x7C
#define COMMAND_ADDRESS 0x00
#define TIME_OUT 100


void initialize_LCDDisplayDriver()
{
	HAL_StatusTypeDef writeResult = I2C1_Master_MEM_WriteByte(DEVICE_ADDRESS, COMMAND_ADDRESS, 0x38, TIME_OUT);

	if(writeResult == HAL_OK){
		writeResult = I2C1_Master_MEM_WriteByte(DEVICE_ADDRESS, COMMAND_ADDRESS, 0x39, TIME_OUT);
	}

	if(writeResult == HAL_OK){
		writeResult = I2C1_Master_MEM_WriteByte(DEVICE_ADDRESS, COMMAND_ADDRESS, 0x14, TIME_OUT);
	}

	if(writeResult == HAL_OK){
		writeResult = I2C1_Master_MEM_WriteByte(DEVICE_ADDRESS, COMMAND_ADDRESS, 0x73, TIME_OUT);
	}

	if(writeResult == HAL_OK){
		writeResult = I2C1_Master_MEM_WriteByte(DEVICE_ADDRESS, COMMAND_ADDRESS, 0x56, TIME_OUT);
	}

	if(writeResult == HAL_OK){
		writeResult = I2C1_Master_MEM_WriteByte(DEVICE_ADDRESS, COMMAND_ADDRESS, 0x6C, TIME_OUT);
	}

	if(writeResult == HAL_OK){
		writeResult = I2C1_Master_MEM_WriteByte(DEVICE_ADDRESS, COMMAND_ADDRESS, 0x38, TIME_OUT);
	}

	if(writeResult == HAL_OK){
		writeResult = I2C1_Master_MEM_WriteByte(DEVICE_ADDRESS, COMMAND_ADDRESS, 0x01, TIME_OUT);
	}

	if(writeResult == HAL_OK){
		writeResult = I2C1_Master_MEM_WriteByte(DEVICE_ADDRESS, COMMAND_ADDRESS, 0x0C, TIME_OUT);
	}
}
void setChar_LCDDisplayDriver(char* str, uint8_t line)
{

}
void clearChar_LCDDisplayDriver()
{

}

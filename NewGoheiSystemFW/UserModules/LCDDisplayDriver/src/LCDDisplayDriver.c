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

#define DATA_ADDRESS 0x40

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
		writeResult = I2C1_Master_MEM_WriteByte(DEVICE_ADDRESS, COMMAND_ADDRESS, 0x7A, TIME_OUT);
	}

	if(writeResult == HAL_OK){
		writeResult = I2C1_Master_MEM_WriteByte(DEVICE_ADDRESS, COMMAND_ADDRESS, 0x54, TIME_OUT);
	}

	if(writeResult == HAL_OK){
		writeResult = I2C1_Master_MEM_WriteByte(DEVICE_ADDRESS, COMMAND_ADDRESS, 0x6C, TIME_OUT);
		HAL_Delay(200);
	}

	if(writeResult == HAL_OK){
		writeResult = I2C1_Master_MEM_WriteByte(DEVICE_ADDRESS, COMMAND_ADDRESS, 0x38, TIME_OUT);
	}

	if(writeResult == HAL_OK){
		writeResult = I2C1_Master_MEM_WriteByte(DEVICE_ADDRESS, COMMAND_ADDRESS, 0x0C, TIME_OUT);
	}

	if(writeResult == HAL_OK){
		writeResult = I2C1_Master_MEM_WriteByte(DEVICE_ADDRESS, COMMAND_ADDRESS, 0x01, TIME_OUT);
	}
	HAL_Delay(2);

	if(writeResult != HAL_OK){
		MX_I2C1_Init();
	}
}
void setChar_LCDDisplayDriver(uint8_t* str, uint8_t length, uint8_t line)
{
	HAL_StatusTypeDef writeResult = HAL_OK;

	if(line == 1){
		writeResult = I2C1_Master_MEM_WriteByte(DEVICE_ADDRESS, COMMAND_ADDRESS, 0x80, TIME_OUT);
	}
	else if(line == 2){
		writeResult = I2C1_Master_MEM_WriteByte(DEVICE_ADDRESS, COMMAND_ADDRESS, 0x40 | 0x80, TIME_OUT);
	}

	if(writeResult == HAL_OK){
		I2C1_Master_MEM_WriteBytes(DEVICE_ADDRESS, DATA_ADDRESS, str, length, TIME_OUT);
	}
	else{
		MX_I2C1_Init();
		initialize_LCDDisplayDriver();
	}

}
void clearChar_LCDDisplayDriver()
{

	HAL_StatusTypeDef writeResult = I2C1_Master_MEM_WriteByte(DEVICE_ADDRESS, COMMAND_ADDRESS, 0x01, TIME_OUT);
	HAL_Delay(2);

	if(writeResult != HAL_OK){
		MX_I2C1_Init();
		initialize_LCDDisplayDriver();
	}
}

/*
 * USB_CDC_Test.c
 *
 *  Created on: 2018/02/21
 *      Author: Shuji
 */
#include "USB_CDC_Test.h"
#include "usbd_cdc_if.h"


void USB_CDC_Test()
{
	uint8_t buffer[100];
	uint8_t bufferTx[] = {'t','e','s','t'};
	uint8_t bufferTx2[] = {'t','e','s','t','2'};


	while(1){
		uint16_t len = CDC_GetReceivedLength();
		if(len > 0){
			CDC_GetReceivedData(buffer, len);
			CDC_Transmit_FS(bufferTx, sizeof(bufferTx));
		}
		else
		{
			//CDC_Transmit_FS(bufferTx2, sizeof(bufferTx2));
			CDC_Transmit_FS(bufferTx2, 0);
		}
		HAL_Delay(500);
	}
}

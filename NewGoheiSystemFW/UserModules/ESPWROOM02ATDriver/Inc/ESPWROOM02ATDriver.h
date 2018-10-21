/*
 * ESPWROOM02ATDriver.h
 *
 *  Created on: 2018/10/20
 *      Author: Shuji
 */

#ifndef ESPWROOM02ATDRIVER_INC_ESPWROOM02ATDRIVER_H_
#define ESPWROOM02ATDRIVER_INC_ESPWROOM02ATDRIVER_H_

typedef unsigned long uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

//API function
void Reboot();
void EnableServer();
void DisableServer();
void EnableClient();
void DisableClient();
void ConnectToAP(const char* APName, const char* APPass);

//Device related Weak Function
void AssertResetPin();
void NegateResetPin();
void Wait4ResetPulse();
uint32_t GetUartDMAPosition();
uint32_t GetUartDMABufferSize();
uint8_t* GetUartBufferAddress();

#endif /* ESPWROOM02ATDRIVER_INC_ESPWROOM02ATDRIVER_H_ */

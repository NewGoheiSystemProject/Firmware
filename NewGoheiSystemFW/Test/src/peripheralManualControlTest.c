/*
 * peripheralManualControlTest.c
 *
 *  Created on: 2018/01/18
 *      Author: Shuji
 */

#include "peripheralManualControlTest.h"
#include "gpio.h"
#include "adc.h"

#define RELAY1_ANODE_PORT (GPIOA)
#define RELAY2_ANODE_PORT (GPIOA)
#define RELAY3_ANODE_PORT (GPIOA)
#define RELAY1_ANODE_PIN (GPIO_PIN_1)
#define RELAY2_ANODE_PIN (GPIO_PIN_2)
#define RELAY3_ANODE_PIN (GPIO_PIN_3)
#define RELAY_STATE_ON GPIO_PIN_SET
#define RELAY_STATE_OFF GPIO_PIN_RESET

typedef GPIO_PinState Relay_State_t;

static void (*CallBackFunc)() = NULL;

static void callBack();



static void setRelay1State(Relay_State_t relayState)
{
	HAL_GPIO_WritePin(RELAY1_ANODE_PORT, RELAY1_ANODE_PIN, relayState);
}
static void toggleRelay1State()
{
	HAL_GPIO_TogglePin(RELAY1_ANODE_PORT, RELAY1_ANODE_PIN);
}
static void setRelay2State(Relay_State_t relayState)
{
	HAL_GPIO_WritePin(RELAY2_ANODE_PORT, RELAY2_ANODE_PIN, relayState);
}
static void toggleRelay2State()
{
	HAL_GPIO_TogglePin(RELAY2_ANODE_PORT, RELAY2_ANODE_PIN);
}
static void setRelay3State(Relay_State_t relayState)
{
	HAL_GPIO_WritePin(RELAY3_ANODE_PORT, RELAY3_ANODE_PIN, relayState);
}
static void toggleRelay3State()
{
	HAL_GPIO_TogglePin(RELAY3_ANODE_PORT, RELAY3_ANODE_PIN);
}
void GPIO_relay_anode_control_test()
{
	setRelay1State(RELAY_STATE_ON);
	HAL_Delay(2000);
	setRelay1State(RELAY_STATE_OFF);

	setRelay2State(RELAY_STATE_ON);
	HAL_Delay(2000);
	setRelay2State(RELAY_STATE_OFF);

	setRelay3State(RELAY_STATE_ON);
	HAL_Delay(2000);
	setRelay3State(RELAY_STATE_OFF);
}
void GPIO_relay_drain_interrupt_test()
{
	CallBackFunc = callBack;
}
/*
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(CallBackFunc != NULL)
	{
		CallBackFunc();
	}
}
*/
void callBack()
{
	toggleRelay1State();
	toggleRelay2State();
	toggleRelay3State();
}
void ADC_DataGet_test()
{
	ADC1_StartConv();
	while(isADC1_Finished() == ADC_CONVERTING);
	uint32_t data = ADC1_GetData();

	printf("%d¥r¥n", (int)data);
}

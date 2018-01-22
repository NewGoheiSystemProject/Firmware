/*
 * BoardControler.c
 *
 *  Created on: 2018/01/22
 *      Author: Shuji
 */
#include "BoardControler.h"
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


static void setRelay1State(Relay_State_t relayState);
static void toggleRelay1State();
static void setRelay2State(Relay_State_t relayState);
static void toggleRelay2State();
static void setRelay3State(Relay_State_t relayState);
static void toggleRelay3State();

void setRelay1State(Relay_State_t relayState)
{
	HAL_GPIO_WritePin(RELAY1_ANODE_PORT, RELAY1_ANODE_PIN, relayState);
}
void toggleRelay1State()
{
	HAL_GPIO_TogglePin(RELAY1_ANODE_PORT, RELAY1_ANODE_PIN);
}
void setRelay2State(Relay_State_t relayState)
{
	HAL_GPIO_WritePin(RELAY2_ANODE_PORT, RELAY2_ANODE_PIN, relayState);
}
void toggleRelay2State()
{
	HAL_GPIO_TogglePin(RELAY2_ANODE_PORT, RELAY2_ANODE_PIN);
}
void setRelay3State(Relay_State_t relayState)
{
	HAL_GPIO_WritePin(RELAY3_ANODE_PORT, RELAY3_ANODE_PIN, relayState);
}
void toggleRelay3State()
{
	HAL_GPIO_TogglePin(RELAY3_ANODE_PORT, RELAY3_ANODE_PIN);
}
void UVOn()
{
	setRelay1State(RELAY_STATE_ON);
}
void UVOff()
{
	setRelay1State(RELAY_STATE_OFF);
}
void HeaterSet(uint8_t percentLevel)
{

}
void HeaterOff()
{
	setRelay2State(RELAY_STATE_OFF);
}
void FanSet(uint8_t percentLevel)
{

}
void FanOff()
{
	setRelay3State(RELAY_STATE_OFF);
}

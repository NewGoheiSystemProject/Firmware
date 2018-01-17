/*
 * peripheralManualControlTest.c
 *
 *  Created on: 2018/01/18
 *      Author: Shuji
 */

#include "peripheralManualControlTest.h"
#include "gpio.h"

#define RELAY1_ANODE_PORT (GPIOA)
#define RELAY2_ANODE_PORT (GPIOA)
#define RELAY3_ANODE_PORT (GPIOA)
#define RELAY1_ANODE_PIN (GPIO_PIN_1)
#define RELAY2_ANODE_PIN (GPIO_PIN_2)
#define RELAY3_ANODE_PIN (GPIO_PIN_3)
#define RELAY_STATE_ON GPIO_PIN_SET
#define RELAY_STATE_OFF GPIO_PIN_RESET

typedef GPIO_PinState Relay_State_t;

static void setRelay1State(Relay_State_t relayState)
{
	HAL_GPIO_WritePin(RELAY1_ANODE_PORT, RELAY1_ANODE_PIN, relayState);
}
static void setRelay2State(Relay_State_t relayState)
{
	HAL_GPIO_WritePin(RELAY2_ANODE_PORT, RELAY2_ANODE_PIN, relayState);
}
static void setRelay3State(Relay_State_t relayState)
{
	HAL_GPIO_WritePin(RELAY3_ANODE_PORT, RELAY3_ANODE_PIN, relayState);
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

}

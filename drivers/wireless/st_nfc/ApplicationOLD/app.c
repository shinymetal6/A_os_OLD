/*
 * app.c
 *
 *  Created on: Aug 4, 2023
 *      Author: fil
 */

#include "main.h"
#include "app.h"
#include "../Platform/Platform_STM32H7xx.h"

SystemTypeDef System;

void app_init(void)
{
	platformLedOn(PLATFORM_LED_A_PORT, PLATFORM_LED_A_PIN);
	HAL_Delay(100);
	platformLedOff(PLATFORM_LED_A_PORT, PLATFORM_LED_A_PIN);
	platformLedOn(PLATFORM_LED_B_PORT, PLATFORM_LED_B_PIN);
	HAL_Delay(100);
	platformLedOff(PLATFORM_LED_B_PORT, PLATFORM_LED_B_PIN);
	platformLedOn(PLATFORM_LED_F_PORT, PLATFORM_LED_F_PIN);
	HAL_Delay(100);
	platformLedOff(PLATFORM_LED_F_PORT, PLATFORM_LED_F_PIN);
	platformLedOn(PLATFORM_LED_V_PORT, PLATFORM_LED_V_PIN);
	HAL_Delay(100);
	platformLedOff(PLATFORM_LED_V_PORT, PLATFORM_LED_V_PIN);
	platformLedOn(PLATFORM_LED_AP2P_PORT, PLATFORM_LED_AP2P_PIN);
	HAL_Delay(100);
	platformLedOff(PLATFORM_LED_AP2P_PORT, PLATFORM_LED_AP2P_PIN);
	HAL_Delay(100);
	ftm_init();
	log_Init();
	HAL_TIM_Base_Start_IT(&TIM_TICK_10MSEC);
}

void app_led_check(void)
{
	platformLedOn(PLATFORM_LED_A_PORT, PLATFORM_LED_A_PIN);
	platformLedOn(PLATFORM_LED_B_PORT, PLATFORM_LED_B_PIN);
	platformLedOn(PLATFORM_LED_F_PORT, PLATFORM_LED_F_PIN);
	platformLedOn(PLATFORM_LED_V_PORT, PLATFORM_LED_V_PIN);
	platformLedOn(PLATFORM_LED_AP2P_PORT, PLATFORM_LED_AP2P_PIN);
	HAL_Delay(1000);
	platformLedOff(PLATFORM_LED_A_PORT, PLATFORM_LED_A_PIN);
	platformLedOff(PLATFORM_LED_B_PORT, PLATFORM_LED_B_PIN);
	platformLedOff(PLATFORM_LED_F_PORT, PLATFORM_LED_F_PIN);
	platformLedOff(PLATFORM_LED_V_PORT, PLATFORM_LED_V_PIN);
	platformLedOff(PLATFORM_LED_AP2P_PORT, PLATFORM_LED_AP2P_PIN);
	HAL_Delay(1000);
}

void app_loop(void)
{
	//if (( System.tick_flags & TICK_10MSEC) == TICK_10MSEC)
	{
		if (( System.system_flags & SYSTEM_NFCSTARTED) == SYSTEM_NFCSTARTED)
		{
			if ( ftm_messages() )
			{
				System.poller_state = START_DISCOVERY;
				System.system_flags &= ~SYSTEM_NFCSTARTED;
				HAL_Delay(1000);
			}
			//HAL_Delay(10);
		}
		else
		{
			if ( ftm_cycle() != NFC_FOUND)
			{
				System.tick_cntr++;
				if ( System.tick_cntr > TICK_TIMEOUT )
				{
					ftm_init();
					System.tick_cntr = 0;
				}
			}
			else
			{
				System.system_flags |= SYSTEM_NFCSTARTED;
				System.tick_cntr = 0;
			}
		}
	}
}

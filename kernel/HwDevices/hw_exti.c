/* 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Project : A_os
*/
/*
 * hw_exti.c
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */

#include "main.h"
#include "hw_spi.h"
#include "../A.h"
#include "../scheduler.h"
#include "../A_exported_functions.h"
#include "../hwmanager.h"
#include "../system_default.h"

extern	HWMngr_t		HWMngr[PERIPHERAL_NUM];
extern	PCB_t 		process[MAX_PROCESS];
extern	Asys_t		Asys;
extern	void encoder_button_irq_callback(void);
extern	void encoder_clock_irq_callback(void);

#if defined ST25R391X_INT_Pin
extern	void st25r3916Isr( void );
#endif

#ifdef ENCODER_CLOCK_Pin
extern	void irq0_callback(void);
#endif
#if defined ENCODER_BTN_Pin
extern	void irq1_callback(void);
#endif

#ifndef STM32H743xx
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
#if defined ST25R391X_INT_Pin
	if ( GPIO_Pin == ST25R391X_INT_Pin)
		st25r3916Isr();
#endif
}
#endif

#if defined SX126X_DIO1_PIN_NUMBER
extern	void RadioOnDioIrq( void );
#endif

uint32_t	flag_a,flag_b;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
#if defined ST25R391X_INT_Pin
	if ( GPIO_Pin == ST25R391X_INT_Pin)
		st25r3916Isr();
#endif

#if defined EXTI4_NRFIRQ_Pin
	if(GPIO_Pin == EXTI4_NRFIRQ_Pin)
	{
		activate_process(1,WAKEUP_FROM_NRF24L01_IRQ,0);
	}
#endif
#if defined SX126X_DIO1_PIN_NUMBER
	if(GPIO_Pin == SX126X_DIO1_PIN_NUMBER)
	{
		//RadioOnDioIrq();
	}
#endif
#ifdef ENCODER_BTN_Pin
	if(GPIO_Pin == ENCODER_BTN_Pin)
	{
		encoder_button_irq_callback();
	}
#endif
#ifdef ENCODER_CLOCK_Pin
	if(GPIO_Pin == ENCODER_CLOCK_Pin)
	{
		encoder_clock_irq_callback();
	}
#endif
}

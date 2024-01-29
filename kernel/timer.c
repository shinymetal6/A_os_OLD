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
 * timer.c
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */

#include "main.h"
#include "A.h"
#include "system_default.h"
#include "scheduler.h"
#include "A_exported_functions.h"
#include "kernel_opt.h"

extern	PCB_t 		process[MAX_PROCESS];
extern	Asys_t		Asys;

extern	__IO uint32_t uwTick;

void update_global_tick_count(void)
{
	Asys.g_tick_count++;
	// update the HAL timer, if someone need it
	uwTick++;
	HAL_UART_RxTimeoutCheckCallback();
}

int32_t A_GetTick(void)
{
	return Asys.g_tick_count;
}

uint32_t HAL_GetTick(void)
{
	if ( Asys.g_os_started )
		return Asys.g_tick_count;
	else
		return uwTick;
}

void task_delay(uint32_t tick_count)
{
	__disable_irq();
	if(Asys.current_process)
	{
		process[Asys.current_process].delay_value = Asys.g_tick_count + tick_count;
		process[Asys.current_process].current_state &= ~PROCESS_READY_STATE;
		process[Asys.current_process].wait_event = SUSPEND_ON_DELAY;
		schedule();
	}
	__enable_irq();
}

void check_timers(void)
{
register uint8_t	i,j;

	for( i = 1 ; i < MAX_PROCESS ; i++)
	{
		if((process[i].wait_event & SUSPEND_ON_DELAY) == SUSPEND_ON_DELAY)
		{
			if(Asys.g_tick_count >= process[i].delay_value)
				process[i].current_state |= PROCESS_READY_STATE;
		}

		if((process[i].wait_event & SUSPEND_ON_TIMER) == SUSPEND_ON_TIMER)
		{
			for( j = 0 ; j < MAX_TIMERS ; j++)
			{
				if((process[i].timer_flags[j] & TIMERFLAGS_IN_USE ) == TIMERFLAGS_IN_USE)
				{
					if((process[i].timer_flags[j] & TIMERFLAGS_ENABLED ) == TIMERFLAGS_ENABLED)
					{
						if(Asys.g_tick_count >= process[i].current_timer[j] )
						{
							process[i].current_state |= PROCESS_READY_STATE;
							if ((process[i].timer_flags[j] & TIMERFLAGS_FOREVER ) == TIMERFLAGS_FOREVER)
								process[i].current_timer[j] = Asys.g_tick_count + process[i].timer_value[j];
							process[i].timer_expired |= (1<<j);
							activate_process(i,WAKEUP_FROM_TIMER,1<<j);
						}
					}
					else
						process[i].current_timer[j]++;
				}
			}
		}
	}
}

void  SysTick_Handler(void)
{
	if ( Asys.g_os_started )
	{
		update_global_tick_count();
		check_timers();
		check_semaphores();
		//pend the pendsv exception
		SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
	}
	else
		HAL_IncTick();
}

uint32_t create_timer(uint8_t timer_id,uint32_t tick_count,uint8_t flags)
{
uint8_t timer_index = 0;
	if (( process[Asys.current_process].timer_flags[timer_id] & TIMERFLAGS_IN_USE ) == TIMERFLAGS_IN_USE)
		return 1;
	switch(timer_id)
	{
	case	TIMER_ID_0:	timer_index = 0; break;
	case	TIMER_ID_1:	timer_index = 1; break;
	case	TIMER_ID_2:	timer_index = 2; break;
	case	TIMER_ID_3:	timer_index = 3; break;
	case	TIMER_ID_4:	timer_index = 4; break;
	case	TIMER_ID_5:	timer_index = 5; break;
	case	TIMER_ID_6:	timer_index = 6; break;
	case	TIMER_ID_7:	timer_index = 7; break;
	default : return 1;
	}
	__disable_irq();
	process[Asys.current_process].timer_flags[timer_index] = (flags & TIMERFLAGS_USERMASK) | TIMERFLAGS_IN_USE;
	process[Asys.current_process].timer_value[timer_index] = tick_count;
	process[Asys.current_process].current_timer[timer_index] = Asys.g_tick_count + tick_count;
	__enable_irq();
	return 0;
}

uint32_t start_timer(uint8_t timer_id)
{
uint8_t timer_index = 0;
	if (( process[Asys.current_process].timer_flags[timer_id] & TIMERFLAGS_IN_USE ) == TIMERFLAGS_IN_USE)
		return 1;
	switch(timer_id)
	{
	case	TIMER_ID_0:	timer_index = 0; break;
	case	TIMER_ID_1:	timer_index = 1; break;
	case	TIMER_ID_2:	timer_index = 2; break;
	case	TIMER_ID_3:	timer_index = 3; break;
	case	TIMER_ID_4:	timer_index = 4; break;
	case	TIMER_ID_5:	timer_index = 5; break;
	case	TIMER_ID_6:	timer_index = 6; break;
	case	TIMER_ID_7:	timer_index = 7; break;
	default : return 1;
	}
	process[Asys.current_process].timer_flags[timer_index] |= TIMERFLAGS_ENABLED;
	return 0;
}

uint32_t restart_timer(uint8_t timer_id,uint32_t tick_count,uint8_t flags)
{
	if (( process[Asys.current_process].timer_flags[timer_id] & TIMERFLAGS_IN_USE ) != TIMERFLAGS_IN_USE)
		return 1;
	process[Asys.current_process].timer_flags[timer_id] &= ~TIMERFLAGS_IN_USE;
	create_timer(timer_id,tick_count,flags);
	return 0;
}

uint32_t stop_timer(uint8_t timer_id)
{
	process[Asys.current_process].timer_flags[timer_id] &= ~TIMERFLAGS_ENABLED;
	return 0;
}

uint32_t destroy_timer(uint8_t timer_id)
{
	__disable_irq();
	process[Asys.current_process].timer_flags[timer_id] = 0;
	__enable_irq();
	return 0;
}

uint8_t get_timer_expired(void)
{
uint8_t tim_exp = process[Asys.current_process].timer_expired;
	process[Asys.current_process].timer_expired &= ~tim_exp;
	return tim_exp;
}


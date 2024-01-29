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
 * Project : A_os_U575_devel 
*/
/*
 * semaphores.c
 *
 *  Created on: Sep 19, 2023
 *      Author: fil
 */
#include "main.h"
#include "A.h"
#include "scheduler.h"
#include "A_exported_functions.h"
#include "kernel_opt.h"

extern	Semaphores_t	Semaphores;
extern	PCB_t 			process[MAX_PROCESS];
extern	Asys_t			Asys;

uint32_t destroy_semaphore(uint8_t semaphore_id)
{
	Semaphores.semaphore[semaphore_id] = 0;
	if ( Semaphores.semaphore_waiting_process[semaphore_id] )
		activate_process(Semaphores.semaphore_waiting_process[semaphore_id],WAKEUP_FROM_SEMAPHORE,WAKEUP_FLAGS_SEMAPHORE);
	return 0;
}

uint32_t remove_from_waiting_semaphore(uint8_t semaphore_id)
{
	Semaphores.semaphore_waiting_process[semaphore_id] = 0;
	return 0;
}

uint32_t	sem_err = 0;

uint32_t get_semaphore(uint8_t semaphore_id,uint8_t semaphore_flag,uint32_t semaphore_timeout)
{
uint32_t	ret_val = SEMAPHORE_RED;
uint32_t	wakeup;

	__disable_irq();
	if ( (semaphore_flag & SEMAPHORE_SUSPEND_IF_RED) != SEMAPHORE_SUSPEND_IF_RED)
	{
		if (Semaphores.semaphore[semaphore_id] == SEMAPHORE_FREE )
		{
			Semaphores.semaphore[semaphore_id] =  Asys.current_process;
			ret_val = SEMAPHORE_GREEN;
		}
	}
	else
	{
		if ((Semaphores.semaphore[semaphore_id] ==  SEMAPHORE_FREE ) || (Semaphores.semaphore[semaphore_id] ==  Asys.current_process ))
		{
			Semaphores.semaphore[semaphore_id] = Asys.current_process;
			Semaphores.semaphore_timeout[semaphore_id] = Asys.g_tick_count + semaphore_timeout;
			ret_val = SEMAPHORE_GREEN;
		}
		else
		{
			if ( Semaphores.semaphore_waiting_process[semaphore_id] == 0 )
			{
				if ( semaphore_timeout )
				{
					Semaphores.semaphore_timeout[semaphore_id] = Asys.g_tick_count + semaphore_timeout;
					Semaphores.semaphore_waiting_process[semaphore_id] = Asys.current_process;
					wakeup = 0;
					while(1)
					{
						while ( (wakeup & (WAKEUP_FROM_SEMAPHORE | WAKEUP_FROM_SEMAPHORE_TIMEOUT)) == 0)
						{
							if ( wakeup & SEMAPHORE_ERRORS_MASK)
								process[Asys.current_process].semaphore_flags = wakeup >> 28;
							wakeup = wait_event( EVENT_SEMAPHORE | EVENT_SEMAPHORE_TIMEOUT );
						}
						if (( wakeup & WAKEUP_FROM_SEMAPHORE_TIMEOUT) == WAKEUP_FROM_SEMAPHORE_TIMEOUT )
						{
							Semaphores.semaphore_waiting_process[semaphore_id] = 0;
							ret_val = SEMAPHORE_TIMEOUT;
							break;
						}
						else if (( wakeup & WAKEUP_FROM_SEMAPHORE) == WAKEUP_FROM_SEMAPHORE )
						{
							Semaphores.semaphore_waiting_process[semaphore_id] = 0;
							ret_val = SEMAPHORE_GREEN;
							break;
						}
						else
							sem_err++;
					}
				}
				else
				{
					Semaphores.semaphore_waiting_process[semaphore_id] = Asys.current_process;
					wakeup = wait_event( EVENT_SEMAPHORE );
					if (( wakeup & WAKEUP_FROM_SEMAPHORE) == WAKEUP_FROM_SEMAPHORE )
					{
						Semaphores.semaphore_waiting_process[semaphore_id] = 0;
						ret_val = SEMAPHORE_GREEN;
					}
					else
						sem_err++;
				}
				Semaphores.semaphore[semaphore_id] = Asys.current_process;
			}
			else if ( Semaphores.semaphore_waiting_process[semaphore_id] == Asys.current_process )
				ret_val = SEMAPHORE_ALREADY_SET;
			else
				ret_val = SEMAPHORE_UNAVAILABLE;
		}
	}
	__enable_irq();
	return ret_val;
}

void check_semaphores(void)
{
register uint8_t	i,j;

	__disable_irq();
	for( i = 1 ; i < MAX_PROCESS ; i++)
	{
		if((process[i].wait_event & SUSPEND_ON_SEMAPHORE) == SUSPEND_ON_SEMAPHORE)
		{
			for( j = 0 ; j < MAX_SEMAPHORES ; j++)
			{
				if ((Semaphores.semaphore[j] == i ) && (Asys.g_tick_count >= Semaphores.semaphore_timeout[j] ))
				{
					process[i].current_state |= PROCESS_READY_STATE;
					activate_process(i,WAKEUP_FROM_SEMAPHORE_TIMEOUT,WAKEUP_FLAGS_SEMAPHORE);
				}
			}
		}
	}
	__enable_irq();
}


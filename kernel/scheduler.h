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
 * scheduler.h
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */

#ifndef KERNEL_SCHEDULER_H_
#define KERNEL_SCHEDULER_H_

#include "A.h"

typedef struct
{
	uint32_t 	psp_value;
	uint32_t	delay_value;
	uint8_t  	current_state;
	uint32_t	wait_event;
	uint8_t		*mbx[MAILBOX_NUM];
	uint32_t	mbx_size[MAILBOX_NUM];
	uint8_t		mbx_number;
	uint32_t	wakeup_rsn;
	uint32_t	wakeup_flags;
	uint8_t		semaphore_flags;
	uint32_t	current_timer[MAX_TIMERS];
	uint32_t	timer_value[MAX_TIMERS];
	uint8_t		timer_flags[MAX_TIMERS];
	uint8_t		timer_expired;
	void (*task_handler)(void);
}PCB_t;

/* timer_flags */
#define	TIMERFLAGS_ONESHOT			0x00
#define	TIMERFLAGS_FOREVER			0x01
#define	TIMERFLAGS_ENABLED			0x02
#define	TIMERFLAGS_IN_USE			0x80
#define	TIMERFLAGS_DISABLED			0x00
#define	TIMERFLAGS_USERMASK			0x0f

#define MAX_TASKS   5
/* the following is a nonsense, it's here just to clarify that a process with bit 0 @ 0 is waiting */
#define PROCESS_WAITING_STATE		0x00
#define PROCESS_READY_STATE			0x01
#define PROCESS_USAGEFAULT_FLAG		0X04
#define PROCESS_BUSFAULT_FLAG		0X08
#define PROCESS_MEMMANAGE_FLAG		0X10
#define PROCESS_HARDFAULT_FLAG		0X20
#define PROCESS_FINISHED_FLAG		0X40
#define PROCESS_KILLED_STATE		0X80

extern	void unblock_tasks(void);
extern	void switch_sp_to_psp(void);
extern	uint32_t activate_process(uint8_t dest_process,uint32_t rsn , uint32_t flags);



#endif /* KERNEL_SCHEDULER_H_ */

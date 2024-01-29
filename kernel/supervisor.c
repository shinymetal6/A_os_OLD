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
 * supervisor.c
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */

#include "main.h"
#include "A.h"
#include "system_default.h"
#include "scheduler.h"
#include "kernel_opt.h"

#ifdef ETH_ENABLED
extern	void MX_LWIP_Process(void);
#endif

#ifdef USB_ENABLED
extern	void MX_USB_HOST_Process(void);
#endif

extern	void supervisor_callback(void);
extern	USRprcs_t	UserProcesses[USR_PROCESS_NUMBER];
extern	PCB_t 		process[MAX_PROCESS];

void supervisor(void)
{
	while(1)
	{
		if ((( Asys.system_flags & SYS_MEM_DEFRAG_REQUEST ) == SYS_MEM_DEFRAG_REQUEST) & (Asys.num_buf_in_use == 0))
			defrag_mem();
		if ( Asys.failed_process )
		{
			reset_orphaned_chunks(Asys.failed_process);
			Asys.failed_process = Asys.fail_rsn = 0;
		}
#ifdef ETH_ENABLED
		MX_LWIP_Process();
#endif
#ifdef USB_ENABLED
		MX_USB_HOST_Process();
#endif
		supervisor_callback();
		/*
#ifdef STM32H743xx
	#ifdef POWERSAVE_ENABLED
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON,PWR_SLEEPENTRY_WFI);
	#endif
#endif
*/
	}
}

void supervisor_process1(void)
{
	UserProcesses[0].user_process(1);
	process[Asys.current_process].current_state = (PROCESS_KILLED_STATE | PROCESS_FINISHED_FLAG);
	while(1);
}

void supervisor_process2(void)
{
	UserProcesses[1].user_process(2);
	process[Asys.current_process].current_state = (PROCESS_KILLED_STATE | PROCESS_FINISHED_FLAG);
	while(1);
}

void supervisor_process3(void)
{
	UserProcesses[2].user_process(3);
	process[Asys.current_process].current_state = (PROCESS_KILLED_STATE | PROCESS_FINISHED_FLAG);
	while(1);
}

void supervisor_process4(void)
{
	UserProcesses[3].user_process(4);
	process[Asys.current_process].current_state = (PROCESS_KILLED_STATE | PROCESS_FINISHED_FLAG);
	while(1);
}

/* faults management */
void HardFault_Handler(void)
{
	process[Asys.current_process].current_state = (PROCESS_KILLED_STATE | PROCESS_HARDFAULT_FLAG);
	Asys.failed_process = Asys.current_process;
	Asys.fail_rsn = PROCESS_HARDFAULT_FLAG;
	Asys.process_hard_fault[Asys.current_process]++;
	schedule();
}

void MemManage_Handler(void)
{
	process[Asys.current_process].current_state = (PROCESS_KILLED_STATE | PROCESS_MEMMANAGE_FLAG);
	Asys.failed_process = Asys.current_process;
	Asys.fail_rsn = PROCESS_HARDFAULT_FLAG;
	Asys.process_mem_manage_fault[Asys.current_process]++;
	schedule();
}

void BusFault_Handler(void)
{
	process[Asys.current_process].current_state = (PROCESS_KILLED_STATE | PROCESS_BUSFAULT_FLAG);
	Asys.failed_process = Asys.current_process;
	Asys.fail_rsn = PROCESS_HARDFAULT_FLAG;
	Asys.process_bus_fault[Asys.current_process]++;
	schedule();
}

void UsageFault_Handler(void)
{
	process[Asys.current_process].current_state = (PROCESS_KILLED_STATE | PROCESS_USAGEFAULT_FLAG);
	Asys.failed_process = Asys.current_process;
	Asys.fail_rsn = PROCESS_HARDFAULT_FLAG;
	Asys.process_usage_fault[Asys.current_process]++;
	schedule();
}


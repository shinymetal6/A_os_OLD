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
 * weak_processes.c
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */
#include "main.h"
#include "../../A_os/kernel/A.h"

/* These processes waste only time, they are defined to have something of useless to schedule */
__weak void weak_process1(uint32_t process_id)
{
	while(1);
}
__weak void weak_process2(uint32_t process_id)
{
	while(1);
}
__weak void weak_process3(uint32_t process_id)
{
	while(1);
}
__weak void weak_process4(uint32_t process_id)
{
	while(1);
}

/* This processes table is define as weak so can be overridden by a user defined processes table */
__weak USRprcs_t	UserProcesses[USR_PROCESS_NUMBER] =
{
		{
				.user_process = weak_process1,
				.stack_size = 256,
		},
		{
				.user_process = weak_process2,
				.stack_size = 256,
		},
		{
				.user_process = weak_process3,
				.stack_size = 256,
		},
		{
				.user_process = weak_process4,
				.stack_size = 256,
		}
};




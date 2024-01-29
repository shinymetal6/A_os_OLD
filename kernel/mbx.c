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
 * mbx.c
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */
#include "main.h"
#include "A.h"
#include "scheduler.h"
#include "A_exported_functions.h"
#include "kernel_opt.h"

extern	uint8_t	current_process;
extern	PCB_t process[MAX_PROCESS];

void mbx_send(uint8_t process_number,uint8_t mailbox_number,uint8_t *mbx_ptr,uint32_t mbx_size)
{
	process[process_number].mbx[mailbox_number] = mbx_ptr;
	process[process_number].mbx_size[mailbox_number] = mbx_size;
	activate_process(process_number,WAKEUP_FROM_MBX,WAKEUP_FLAGS_MBX);
}

uint32_t mbx_receive(uint8_t mailbox_number,uint8_t *buf_ptr)
{
	A_memcpy(buf_ptr,process[current_process].mbx[mailbox_number] ,process[current_process].mbx_size[mailbox_number]);
	process[current_process].wakeup_rsn &= ~WAKEUP_FROM_MBX;
	return process[current_process].mbx_size[mailbox_number];
}



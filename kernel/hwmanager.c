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
 * hwmanager.c
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */

#include "main.h"
#include "A.h"
#include "scheduler.h"
#include "A_exported_functions.h"
#include "kernel_opt.h"

extern	HWMngr_t		HWMngr[PERIPHERAL_NUM];
extern	Asys_t			Asys;

#ifdef	USE_QUEUES

extern	HWMngr_queue_t	HwQueues[PERIPHERAL_NUM];

uint32_t create_queue(HWMngr_queue_t *queue,uint32_t peripheral)
{
	if ( queue->peripheral == 0 )
	{
		queue->peripheral = peripheral;
		queue->process = Asys.current_process;
		return 0;
	}
	return 1;
}

uint32_t destroy_queue(HWMngr_queue_t *queue,uint32_t peripheral)
{
	if (( queue->peripheral == peripheral ) && (queue->process == Asys.current_process))
	{
		A_bzero((uint8_t *)queue,HW_MNGR_QUEUE_LEN);
		return 0;
	}
	return 1;
}

uint32_t get_queue_len(HWMngr_queue_t *queue)
{
	return (uint32_t )queue->queued_bufs;
}

uint32_t queue_insert(HWMngr_queue_t *queue, uint8_t *buf, uint16_t len)
{
uint32_t	ret_val , empty = 0;
	__disable_irq();
	if (queue->process == Asys.current_process)
	{
		if ( queue->queued_bufs < HWMAN_QUEUE_LEN ) // are there at least a free entry ?
		{
			if ( queue->queued_bufs == 0 )	// queue was empty
				empty = HW_MNGR_QUEUE_WAS_EMPTY;
			queue->queued_bufs++;
			queue->ptr[queue->insertion_index] = buf;
			queue->len[queue->insertion_index] = len;
			queue->insertion_index++;
			queue->insertion_index &= (HWMAN_QUEUE_LEN-1);
			ret_val = (uint32_t )len | empty;
		}
		else
			ret_val = HW_MNGR_QUEUE_FULL;
	}
	else
		ret_val = HW_MNGR_QUEUE_NOT_OWNED;
 	__enable_irq();
 	return ret_val;
}

/* returns:
 * 0 : last queued buffer or no buffers in queue
 * 1..16 : still some buffers in queue
 */
uint8_t *queue_extract(HWMngr_queue_t *queue, uint8_t *numbuf, uint16_t *len)
{
uint8_t *buf = NULL;
	__disable_irq();
	if ( queue->queued_bufs > 0 )
	{
		queue->queued_bufs--;
		queue->extraction_index++;
		queue->extraction_index &= (HWMAN_QUEUE_LEN-1);
		buf = queue->ptr[queue->extraction_index];
		*len = queue->len[queue->extraction_index];
		queue->len[queue->extraction_index] = 0;
		*numbuf = queue->queued_bufs;
	}
	else
		*numbuf = 0;
 	__enable_irq();
 	return buf;
}
#endif

uint32_t allocate_hw(uint32_t peripheral,uint8_t config)
{
	if ( HWMngr[peripheral].process )
		return 0;
	HWMngr[peripheral].process = Asys.current_process;
	HWMngr[peripheral].status = HWMAN_STATUS_ALLOCATED | config;
#ifdef	USE_QUEUES
	create_queue(&HwQueues[peripheral],peripheral);
#endif
	return peripheral;
}

uint32_t deallocate_hw(uint32_t peripheral)
{
	if ( HWMngr[peripheral].process != Asys.current_process )
		return 0;
	HWMngr[peripheral].process = 0;
	HWMngr[peripheral].status = HWMAN_STATUS_FREE;
#ifdef	USE_QUEUES
	destroy_queue(&HwQueues[peripheral],peripheral);
#endif
	return peripheral;
}

uint16_t get_rx_len(uint32_t peripheral)
{
	if ( HWMngr[peripheral].process != Asys.current_process )
		return 0;
	return HWMngr[HW_USB_DEVICE].rxlen;
}

uint32_t get_peripheral_flags(uint32_t peripheral)
{
	if ( HWMngr[peripheral].process != Asys.current_process )
		return 0;
	return HWMngr[HW_USB_DEVICE].flags;
}


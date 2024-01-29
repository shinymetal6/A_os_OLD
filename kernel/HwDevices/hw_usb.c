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
 * hw_usb.c
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */

#include "main.h"
#include "../A.h"
#include "../scheduler.h"
#include "../A_exported_functions.h"
#include "../hwmanager.h"
#include "../system_default.h"

extern	HWMngr_t	HWMngr[PERIPHERAL_NUM];
extern	Asys_t		Asys;
extern	HWMngr_queue_t	HwQueues[PERIPHERAL_NUM];

uint32_t hw_set_usb_rx_buffer(uint8_t *rx_buf)
{
	if ( HWMngr[HW_USB_DEVICE].process == Asys.current_process )
	{
		HWMngr[HW_USB_DEVICE].rx_buf = rx_buf;
		return 0;
	}
	return 255;
}

uint32_t hw_UsbPktReceived(uint8_t* Buf, uint32_t Len)
{
uint8_t	*dest_ptr = HWMngr[HW_USB_DEVICE].rx_buf;
	if ( dest_ptr != NULL )
	{
		HWMngr[HW_USB_DEVICE].rxlen = Len;
		A_memcpy(dest_ptr,Buf,Len);
		dest_ptr[Len] = 0;
		activate_process(HWMngr[HW_USB_DEVICE].process,WAKEUP_FROM_USB_DEVICE_IRQ,WAKEUP_FLAGS_HW_USB_RX_COMPLETE);
		return	Len;
	}
	return 0;
}

void hw_UsbControlCallback(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
}


extern	uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);

uint32_t hw_send_usb(uint8_t* ptr, uint16_t len)
{
#ifdef	USE_QUEUES
	if (( HWMngr[HW_USB_DEVICE].process == Asys.current_process ) && ( len != 0 ))
	{
		if ( (queue_insert(&HwQueues[HW_USB_DEVICE],ptr,len) & HW_MNGR_QUEUE_WAS_EMPTY ) == HW_MNGR_QUEUE_WAS_EMPTY )
		{
			return (uint32_t )CDC_Transmit_FS(ptr, len);
		}
	}
	return 0xffffffff;
#else
	return (uint32_t )CDC_Transmit_FS(ptr, len);
#endif
}


void USB_TxCpltCallback(void)
{
#ifdef	USE_QUEUES
	uint8_t		numbuf,*ptr;
	uint16_t 	len;
	if ( HWMngr[HW_USB_DEVICE].status == HWMAN_STATUS_ALLOCATED)
	{
		ptr = queue_extract(&HwQueues[HW_USB_DEVICE], &numbuf, &len);
		if (  numbuf )
		{
			CDC_Transmit_FS(ptr, len);
		}
	}
#endif
}




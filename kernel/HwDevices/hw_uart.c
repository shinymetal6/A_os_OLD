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
 * hw_uart.c
 *
 *  Created on: Sep 4, 2023
 *      Author: fil
 */


#include "main.h"
#include "../A.h"
#include "../scheduler.h"
#include "../A_exported_functions.h"
#include "../hwmanager.h"
#include "../system_default.h"

#ifdef CONSOLE
extern	HWMngr_t		HWMngr[PERIPHERAL_NUM];
extern	Asys_t			Asys;
extern	HWMngr_queue_t	HwQueues[PERIPHERAL_NUM];
uint8_t	rx_char;
#endif

uint32_t hw_send_uart(uint32_t uart,uint8_t *ptr,uint16_t len)
{

#ifdef CONSOLE
#ifdef	USE_QUEUES
	if ( HWMngr[uart].process == Asys.current_process )
	{
		if ( (queue_insert(&HwQueues[uart],ptr,len) & HW_MNGR_QUEUE_WAS_EMPTY ) == HW_MNGR_QUEUE_WAS_EMPTY )
		{
			return  HAL_UART_Transmit_IT(&CONSOLE, ptr, len);
		}
	}
#else
	return  HAL_UART_Transmit_IT(&CONSOLE, ptr, len);
#endif
#endif
	return 255;
}


uint32_t hw_receive_uart(uint32_t uart,uint8_t *rx_buf,uint16_t rx_buf_max_len,uint8_t timeout)
{
#ifdef CONSOLE
	if ( HWMngr[uart].process == Asys.current_process )
	{
		HWMngr[uart].rx_buf = rx_buf;
		HWMngr[uart].rx_buf_max_len = rx_buf_max_len;
		if ( timeout )
		{
			HWMngr[uart].timeout = HWMngr[uart].timeout_reload_value = timeout;
			HWMngr[uart].flags |= HWMAN_TIMEOUT_ENABLED;
		}
		HWMngr[uart].rx_buf_index = 0;
		HAL_UART_Receive_IT(&CONSOLE, &rx_char, 1);
		return 0;
	}
#endif
	return 255;
}

uint16_t hw_get_uart_receive_len(uint32_t uart)
{
#ifdef CONSOLE
	if ( HWMngr[uart].status == HWMAN_STATUS_ALLOCATED)
		if ( HWMngr[uart].process == Asys.current_process )
			return HWMngr[uart].rxlen;
	return 0;
#endif
	return 0;
}

#ifdef CONSOLE
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
#ifdef	USE_QUEUES
uint8_t		numbuf,*ptr;
uint16_t 	len;
	if ( HWMngr[HW_UART1].status == HWMAN_STATUS_ALLOCATED)
	{
		ptr = queue_extract(&HwQueues[HW_UART1], &numbuf, &len);
		if (  numbuf )
		{
			HAL_UART_Transmit_IT(&CONSOLE, ptr, len);
		}
	}
	if ( HWMngr[HW_UART2].status == HWMAN_STATUS_ALLOCATED)
	{
		ptr = queue_extract(&HwQueues[HW_UART2], &numbuf, &len);
		if (  numbuf )
		{
			HAL_UART_Transmit_IT(&CONSOLE, ptr, len);
		}
	}
	if ( HWMngr[HW_UART3].status == HWMAN_STATUS_ALLOCATED)
	{
		ptr = queue_extract(&HwQueues[HW_UART3], &numbuf, &len);
		if (  numbuf )
		{
			HAL_UART_Transmit_IT(&CONSOLE, ptr, len);
		}
	}
#else
#endif
}

uint8_t trxbuf[32],t_index=0;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	trxbuf[t_index] = rx_char;
	t_index++;
	t_index &= 0x1f;

	if ( HWMngr[HW_UART1].status == HWMAN_STATUS_ALLOCATED)
	{
		if ( HWMngr[HW_UART1].rx_buf != NULL )
		{
			HWMngr[HW_UART1].rx_buf[HWMngr[HW_UART1].rx_buf_index] = rx_char;
			HWMngr[HW_UART1].rx_buf_index++;
			if ( HWMngr[HW_UART1].rx_buf_index == HWMngr[HW_UART1].rx_buf_max_len)
			{
				HWMngr[HW_UART1].rxlen = HWMngr[HW_UART1].rx_buf_index;
				HWMngr[HW_UART1].rx_buf_index = 0;
				activate_process(HWMngr[HW_UART1].process,WAKEUP_FROM_UART1_IRQ,WAKEUP_FLAGS_UART);
			}
		}
	}
	if ( HWMngr[HW_UART2].status == HWMAN_STATUS_ALLOCATED)
	{
		if ( HWMngr[HW_UART2].rx_buf != NULL )
		{
			HWMngr[HW_UART2].rx_buf[HWMngr[HW_UART2].rx_buf_index] = rx_char;
			HWMngr[HW_UART2].rx_buf_index++;
			if ( HWMngr[HW_UART2].rx_buf_index == HWMngr[HW_UART2].rx_buf_max_len)
			{
				HWMngr[HW_UART2].rxlen = HWMngr[HW_UART2].rx_buf_index;
				HWMngr[HW_UART2].rx_buf_index = 0;
				activate_process(HWMngr[HW_UART2].process,WAKEUP_FROM_UART2_IRQ,WAKEUP_FLAGS_UART);
			}
		}
	}
	if ( HWMngr[HW_UART3].status == HWMAN_STATUS_ALLOCATED)
	{
		if ( HWMngr[HW_UART3].rx_buf != NULL )
		{
			HWMngr[HW_UART3].rx_buf[HWMngr[HW_UART3].rx_buf_index] = rx_char;
			HWMngr[HW_UART3].rx_buf_index++;
			if ( HWMngr[HW_UART3].rx_buf_index == HWMngr[HW_UART3].rx_buf_max_len)
			{
				HWMngr[HW_UART3].rxlen = HWMngr[HW_UART3].rx_buf_index;
				HWMngr[HW_UART3].rx_buf_index = 0;
				activate_process(HWMngr[HW_UART3].process,WAKEUP_FROM_UART3_IRQ,WAKEUP_FLAGS_UART);
			}
		}
	}
	HAL_UART_Receive_IT(&CONSOLE, &rx_char, 1);
}

void HAL_UART_RxTimeoutCheckCallback(void)
{
	if ( HWMngr[HW_UART1].status == HWMAN_STATUS_ALLOCATED)
	{
		if ( HWMngr[HW_UART1].rx_buf != NULL )
		{
			if ( HWMngr[HW_UART1].rx_buf_index != 0 )
			{
				if (( HWMngr[HW_UART1].flags & HWMAN_TIMEOUT_ENABLED) == HWMAN_TIMEOUT_ENABLED)
				{
					if ( HWMngr[HW_UART1].timeout )
					{
						HWMngr[HW_UART1].timeout --;
						if ( HWMngr[HW_UART1].timeout == 0 )
						{
							HWMngr[HW_UART1].timeout = HWMngr[HW_UART1].timeout_reload_value;
							HWMngr[HW_UART1].rxlen = HWMngr[HW_UART1].rx_buf_index;
							HWMngr[HW_UART1].rx_buf_index = 0;
							activate_process(HWMngr[HW_UART1].process,WAKEUP_FROM_UART1_IRQ,WAKEUP_FLAGS_UART);
						}
					}
				}
			}
		}
	}
	if ( HWMngr[HW_UART2].status == HWMAN_STATUS_ALLOCATED)
	{
		if ( HWMngr[HW_UART2].rx_buf != NULL )
		{
			if ( HWMngr[HW_UART2].rx_buf_index != 0 )
			{
				if (( HWMngr[HW_UART2].flags & HWMAN_TIMEOUT_ENABLED) == HWMAN_TIMEOUT_ENABLED)
				{
					if ( HWMngr[HW_UART2].timeout )
					{
						HWMngr[HW_UART2].timeout --;
						if ( HWMngr[HW_UART2].timeout == 0 )
						{
							HWMngr[HW_UART2].timeout = HWMngr[HW_UART2].timeout_reload_value;
							HWMngr[HW_UART2].rxlen = HWMngr[HW_UART2].rx_buf_index;
							HWMngr[HW_UART2].rx_buf_index = 0;
							activate_process(HWMngr[HW_UART2].process,WAKEUP_FROM_UART2_IRQ,WAKEUP_FLAGS_UART);
						}
					}
				}
			}
		}
	}
	if ( HWMngr[HW_UART3].status == HWMAN_STATUS_ALLOCATED)
	{
		if ( HWMngr[HW_UART3].rx_buf != NULL )
		{
			if ( HWMngr[HW_UART3].rx_buf_index != 0 )
			{
				if (( HWMngr[HW_UART3].flags & HWMAN_TIMEOUT_ENABLED) == HWMAN_TIMEOUT_ENABLED)
				{
					if ( HWMngr[HW_UART3].timeout )
					{
						HWMngr[HW_UART3].timeout --;
						if ( HWMngr[HW_UART3].timeout == 0 )
						{
							HWMngr[HW_UART3].timeout = HWMngr[HW_UART3].timeout_reload_value;
							HWMngr[HW_UART3].rxlen = HWMngr[HW_UART3].rx_buf_index;
							HWMngr[HW_UART3].rx_buf_index = 0;
							activate_process(HWMngr[HW_UART3].process,WAKEUP_FROM_UART3_IRQ,WAKEUP_FLAGS_UART);
						}
					}
				}
			}
		}
	}
}

#endif

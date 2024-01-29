/*
 * logger.c
 *
 *  Created on: Aug 22, 2023
 *      Author: fil
 */

#include "main.h"
#include "rfal_platform.h"
#include "app.h"
#include <stdio.h>
#include <string.h>

uint8_t current_counter = 55;
uint8_t	err;
uint8_t	temp_line[512];

void log_data(uint8_t pktlen)
{
int	pkt_counter,header_size;
uint8_t	pnum,data_index = 0,i,k,start_data;
uint8_t pkt_type=0;

	System.uart_txlen = 0;
	while(data_index == 0 )
	{
		if ( System.mbx_rxbuf[k] == '<') // data message
		{
			data_index = k;
			break;
		}
		else
		{
			k++;
			if ( k > 32 )
				return;
		}
	}
	pnum = sscanf((char *)&System.mbx_rxbuf[data_index],"<%d*%d%c",(int *)&pkt_counter,(int *)&header_size , &pkt_type);
	if (pnum == 3 )
	{
		bzero(System.uart_txbuf,UART_TX_SIZE);
		if ( current_counter != pkt_counter)
		{
			if ( pkt_type == '!' ) // env sensors
			{
				for (i = 0; i <header_size; i++)
				{
					System.uart_txbuf[i] = System.mbx_rxbuf[i+1];
				}
				System.uart_txlen = strlen((char *)System.uart_txbuf);
				current_counter = pkt_counter;
			}
			if ( pkt_type == '#' ) // accelerometer , 32 * 6 bytes -> 196
			{
				start_data = 0;
				for (start_data = 0; start_data < pktlen; start_data++)
				{
					if (( System.mbx_rxbuf[start_data] == '#') &&( System.mbx_rxbuf[start_data+1] == 'A')&&( System.mbx_rxbuf[start_data+2] == ':'))
						break;
				}
				start_data +=4;
				for (i = 0; i < 192; i++)
				{
				  sprintf((char *)&temp_line[i*2],"%02x", (unsigned int )System.mbx_rxbuf[start_data+i]);
				}
				sprintf((char *)System.uart_txbuf,"\n\r<%d*%d%c rxlen=%d--",pkt_counter,header_size , pkt_type,pktlen);
				strcat((char *)System.uart_txbuf,(char *)temp_line);
				sprintf((char *)temp_line,">\n\r\n\r");
				strcat((char *)System.uart_txbuf,(char *)temp_line);
				System.uart_txlen = strlen((char *)System.uart_txbuf);
				current_counter = pkt_counter;
			}
		}
	}
	else
		err++;

	if ( System.uart_txlen )
		HAL_UART_Transmit_IT(&LOG_UART, System.uart_txbuf, System.uart_txlen);
	platformLedOff(PLATFORM_LED_CARDFOUND_PORT, PLATFORM_LED_CARDFOUND_PIN);
}

void log_Init(void)
{
	sprintf((char *)System.uart_txbuf,"Rollon Logger\n\r");
	System.uart_txlen = strlen((char *)System.uart_txbuf);
	HAL_UART_Transmit_IT(&LOG_UART, System.uart_txbuf, System.uart_txlen);
	System.reached_len = 0;
}

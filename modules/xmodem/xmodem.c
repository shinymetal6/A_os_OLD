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
 * xmodem.c
 *
 *  Created on: Jan 31, 2024
 *      Author: fil
 */

#include "main.h"
#include "xmodem.h"
#include <string.h>

uint8_t 	xmodem_sm;
xmodem_t	xmodem_struct;
uint8_t		xmodem_line[1024];

static uint8_t xmodem_calc_csum(uint8_t *buf)
{
uint16_t	calc_csum=0,i;
	for(i=0;i<XMODEM_LEN;i++)
		calc_csum += buf[i+3];
	return ( calc_csum & 0xff ) - buf[XMODEM_LEN+3];
}

static uint8_t xmodem_calc_crc(uint8_t *buf)
{
	return 0;
}

uint8_t xmodem_line_parser(uint8_t *buf)
{
	if ( buf[0] == X_SOH)
		xmodem_struct.data_len = XMODEM_LEN;
	else if ( buf[0] == X_STX)
		xmodem_struct.data_len = XMODEM1K_LEN;
	else
		return 1;
	if ( (buf[XMODEM_ADDR] + buf[XMODEM_ADDRI]) != 0xff)
		return 1;
	xmodem_struct.addr = buf[XMODEM_ADDR];
	xmodem_struct.addri = buf[XMODEM_ADDRI];
	if ( xmodem_struct.data_len == XMODEM_LEN )
		xmodem_struct.cs = buf[XMODEM_CS];
	memcpy(xmodem_line,&buf[3],xmodem_struct.data_len);
	if ( buf[0] == X_SOH)
		return xmodem_calc_csum(buf);
	else if ( buf[0] == X_STX)
		return xmodem_calc_crc(buf);
	else
		return 1;
}

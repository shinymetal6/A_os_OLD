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

uint8_t 	xmodem_sm;
xmodem_t	xmodem_struct;
uint8_t		xmodem_line[1024];

uint8_t xmodem_receive(uint8_t *buf , uint16_t len)
{
	xmodem_struct.data_len = len;
	xmodem_struct.addr = buf[1];
	xmodem_struct.addri = buf[2];
	xmodem_struct.cs = buf[131];
	return 0;
}

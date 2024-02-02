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
 * xmodem.h
 *
 *  Created on: Jan 31, 2024
 *      Author: fil
 */

#ifndef MODULES_XMODEM_XMODEM_H_
#define MODULES_XMODEM_XMODEM_H_

#define X_SOH	0x01
#define X_STX 	0x02
#define X_EOT	0x04
#define X_ACK	0x06
#define X_NAK	0x15
#define X_CAN	0x18
#define X_C		'C'  /* notify the host we can use CRC16. */

#define IDLE			0
#define	RECV_SOH		1
#define	RECV_ADDR		2
#define	RECV_ADDRI		3
#define	DATA_PHASE		4
#define	CS_PHASE		5
#define	CRCH_PHASE		6
#define	CRCL_PHASE		7
#define	RECV_LOOP		8
#define	RECV_EOT		9

#define	XMODEM_LEN		128
#define	XMODEM1K_LEN	1024

typedef struct
{
	uint16_t	data_len;
	uint8_t		addr;
	uint8_t		addri;
	uint8_t		cs;
	uint8_t		crch;
	uint8_t		crcl;
}xmodem_t;


#endif /* MODULES_XMODEM_XMODEM_H_ */

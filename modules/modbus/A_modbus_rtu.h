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
 * Project : u575_modbus 
*/
/*
 * A_modbus_rtu.h
 *
 *  Created on: Oct 3, 2023
 *      Author: fil
 */
/*
 * For CRC16 modbus computation, some are using : 0x8005 polynomial (reversal of 0xA001),
 * input reversed, output reversed.
 * */

#ifndef A_MODBUS_RTU_H_
#define A_MODBUS_RTU_H_

#define	A_MODBUS_RTU_ADDRESS_PHASE			0x00
#define	A_MODBUS_RTU_FC_PHASE				0x01
#define	A_MODBUS_RTU_DATA_PHASE				0x02

#define	A_MODBUS_RTU_ERR_ILLEGAL_FC			0x00
#define	A_MODBUS_RTU_ERR_ILLEGAL_ADDRESS	0x01
#define	A_MODBUS_RTU_ERR_ILLEGAL_DATA		0x02
#define	A_MODBUS_RTU_ERR_SLAVE_ERROR		0x03

#define	A_MODBUS_RTU_INFO					"A_ModBus "
#define	A_MODBUS_RTU_INFO_VER				"RTU V1.0 "
#define	A_MODBUS_RTU_MBFW_VER				"\n FW : V1.0 \n Author : FV\n"
#define	A_MODBUS_RTU_MBFW_DATE				__DATE__
#define	A_MODBUS_RTU_MBFW_TIME				__TIME__
#define	A_MODBUS_RTU_RUN_INDICATOR			0xff

extern	uint32_t A_rtu_modbus_process(uint8_t *buf, uint16_t len);


#endif /* A_MODBUS_H_ */

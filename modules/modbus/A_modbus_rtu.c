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
https://www.fernhillsoftware.com/help/drivers/modbus/modbus-protocol.html#writeSingleCoil*/
/*
 * A_modbus_rtu.c
 *
 *  Created on: Oct 3, 2023
 *      Author: fil
 */

#include "main.h"
#include "A_modbus.h"
#include "A_modbus_rtu.h"
#include "../../kernel/system_default.h"	/* for BOARD_NAME variable only */
#include <stdio.h>
#include <string.h>

extern	A_modbus_t			A_modbus;
extern	A_modbus_inout_t	A_modbus_inout;
extern	uint32_t hw_send_uart(uint32_t uart,uint8_t *ptr,uint16_t len);

// weak functions for external interface
__weak uint8_t A_write_coil_from_modbus(uint16_t coil_index, uint8_t coil_value)
{
	return 0;
}

__weak uint8_t A_write_reg_from_modbus(uint16_t register_index, uint8_t register_value)
{
	return 0;
}

uint8_t A_set_in_to_modbus(uint16_t discrete_in_index, uint8_t discrete_in_value)
{
uint16_t		byte_index,temp_in_index;
	if ( discrete_in_index >= A_MAX_DISCRETE_IN)
		return 1;
	byte_index = discrete_in_index/8;
	temp_in_index = discrete_in_index - byte_index*8;
	if ( discrete_in_value )
		A_modbus_inout.discrete_input[byte_index] |= 1 << temp_in_index;
	else
		A_modbus_inout.discrete_input[byte_index] &= ~(1 << temp_in_index);
	return 0;
}

uint8_t A_get_reg_from_modbus(uint8_t register_index)
{
	if ( register_index < A_MAX_REGS)
		return A_modbus_inout.holding_registers[register_index];
	return 0;
}

uint8_t A_set_reg_to_modbus(uint8_t register_index, uint8_t register_value)
{
	if ( register_index < A_MAX_REGS)
		A_modbus_inout.holding_registers[register_index] = register_value;
	return 0;
}

uint16_t	crc(uint8_t *data,uint32_t len)
{
#ifdef A_MODBUS_SW_CRC
uint16_t crc = 0xffff,i,pos;

	for (pos = 0; pos < len; pos++)
	{
		crc ^= data[pos];
		for (i = 8; i != 0; i--)
		{
			if ((crc & 0x0001) != 0)
			{
				crc >>= 1;
				crc ^= 0xa001;
			}
			else
				crc >>= 1;
		}
	}
	  return crc;
#else
	return (uint16_t )HAL_CRC_Calculate(&MODBUS_CRC, (uint32_t *)data, len);
#endif
}

/* Code */

uint8_t A_rtu_modbus_exception(uint8_t *buf,uint8_t	exception_code)
{
	A_modbus.tx_packet[0] = A_modbus.modbus_addr;
	A_modbus.tx_packet[1] = buf[1] | 0x80;
	A_modbus.tx_packet[2] = exception_code;
	A_modbus.crc = crc( A_modbus.tx_packet, 3);
	A_modbus.tx_packet[3] = A_modbus.crc;
	A_modbus.tx_packet[4] = A_modbus.crc>>8;
	A_modbus.tx_packet_len = 5;
	return 0;
}

uint8_t get_coils_discrete_in(uint8_t *ptr,uint16_t start_coil,uint16_t number_of_coil)
{
uint16_t		i,j,k;
uint16_t		byte_index;
uint16_t		temp_coils_index=0;

	byte_index = start_coil/8;
	j=start_coil - (byte_index*8);
	for(i=start_coil,k=0;i<start_coil+number_of_coil;i++,j++,k++)
	{
		if ( k == 8)
		{
			k = 0;
			temp_coils_index++;
		}
		if ( j == 8 )
		{
			j=0;
			byte_index++;
		}
		if ( ptr[byte_index] & (1 << (i & 0x07)))
			A_modbus_inout.temp_coils_icoils[temp_coils_index] |= 1 << j;
		else
			A_modbus_inout.temp_coils_icoils[temp_coils_index] &= ~(1 << j);
	}
	return temp_coils_index+1;
}

static uint8_t A_rtu_modbus_read_coils_discrete_in(uint8_t *buf)
{
uint8_t		*ptr = A_modbus_inout.coils;
uint16_t	number_of_bytes,bit_index,coil_discrete_number,i;

	switch(buf[1])
	{
	case	A_MODBUS_READ_COIL :
		ptr = A_modbus_inout.coils;
		break;
	case	A_MODBUS_READ_DISCRETE_INPUT :
		ptr = A_modbus_inout.discrete_input;
		break;
	default:
		return 1;
	}

	bit_index = (buf[2] <<8 ) | buf[3];
	coil_discrete_number = (buf[4] <<8 ) | buf[5];
	if ( bit_index > A_MAX_COILS )
		return A_rtu_modbus_exception(buf,A_MODBUS_RTU_ERR_ILLEGAL_ADDRESS);
	if ((bit_index+coil_discrete_number) > A_MAX_COILS )
		return A_rtu_modbus_exception(buf,A_MODBUS_RTU_ERR_ILLEGAL_DATA);

	number_of_bytes = get_coils_discrete_in(ptr,bit_index,coil_discrete_number);

	A_modbus.tx_packet[0] = A_modbus.modbus_addr;
	A_modbus.tx_packet[1] = buf[1];
	A_modbus.tx_packet[2] = number_of_bytes;
	for(i=0;i<number_of_bytes;i++)
	{
		A_modbus.tx_packet[i+3] = A_modbus_inout.temp_coils_icoils[i];
	}
	A_modbus.crc = crc( A_modbus.tx_packet, number_of_bytes+3);
	A_modbus.tx_packet[number_of_bytes+3] = A_modbus.crc;
	A_modbus.tx_packet[number_of_bytes+4] = A_modbus.crc>>8;
	A_modbus.tx_packet_len = number_of_bytes+5;
	return 0;
}

static uint8_t A_rtu_modbus_read_regs_iregs(uint8_t *buf)
{
uint8_t		*ptr = A_modbus_inout.input_registers;
uint16_t	number_of_bytes,reg_index,i;

	reg_index = (buf[2]<<8 ) | buf[3];
	reg_index *=2;
	number_of_bytes = (buf[4]<<8 ) | buf[5];
	if ( reg_index > A_MAX_REGS )
		return A_rtu_modbus_exception(buf,A_MODBUS_RTU_ERR_ILLEGAL_ADDRESS);
	if ((reg_index+number_of_bytes) > A_MAX_REGS )
		return A_rtu_modbus_exception(buf,A_MODBUS_RTU_ERR_ILLEGAL_DATA);
	switch(buf[1])
	{
	case	A_MODBUS_READ_REGISTER :
		ptr = A_modbus_inout.holding_registers;
		break;
	case	A_MODBUS_READ_INPUT_REGISTER :
		ptr = A_modbus_inout.input_registers;
		break;
	default:	return 1;
	}
	A_modbus.tx_packet[0] = A_modbus.modbus_addr;
	A_modbus.tx_packet[1] = buf[1];
	A_modbus.tx_packet[2] = (uint8_t )number_of_bytes*2;
	for(i=0;i<number_of_bytes*2;i+=2)
	{
		A_modbus.tx_packet[i+3] 	= ptr[i+reg_index];
		A_modbus.tx_packet[i+3+1] 	= ptr[i+reg_index+1];
	}
	A_modbus.crc = crc( A_modbus.tx_packet, number_of_bytes*2+3);
	A_modbus.tx_packet[number_of_bytes*2+3] = A_modbus.crc;
	A_modbus.tx_packet[number_of_bytes*2+4] = A_modbus.crc>>8;
	A_modbus.tx_packet_len = number_of_bytes*2+5;
	return 0;
}

static uint8_t A_rtu_modbus_report_slave_info(uint8_t *buf)
{
uint16_t	number_of_bytes;
	A_modbus.tx_packet[0] = A_modbus.modbus_addr;
	A_modbus.tx_packet[1] = buf[1];
	sprintf((char *)&A_modbus.tx_packet[3],"%x%s %s %s %s %s\nBoard : %s",A_MODBUS_RTU_RUN_INDICATOR,A_MODBUS_RTU_INFO,A_MODBUS_RTU_INFO_VER,A_MODBUS_RTU_MBFW_VER,A_MODBUS_RTU_MBFW_DATE,A_MODBUS_RTU_MBFW_TIME,BOARD_NAME);
	number_of_bytes = strlen((char *)&A_modbus.tx_packet[3]);
	A_modbus.tx_packet[2] = number_of_bytes;
	A_modbus.crc = crc( A_modbus.tx_packet, number_of_bytes+3);
	A_modbus.tx_packet[number_of_bytes+3] = A_modbus.crc;
	A_modbus.tx_packet[number_of_bytes+4] = A_modbus.crc>>8;
	A_modbus.tx_packet_len = number_of_bytes+5;
	return 0;
}

static void A_rtu_modbus_write_reply(uint8_t *buf)
{
uint16_t	i;
	for(i=0;i<6;i++)
		A_modbus.tx_packet[i] = buf[i];
	A_modbus.crc = crc( A_modbus.tx_packet, 6);
	A_modbus.tx_packet[6] = A_modbus.crc;
	A_modbus.tx_packet[7] = A_modbus.crc>>8;
	A_modbus.tx_packet_len = 8;
}

static uint8_t A_rtu_write_mb_register(uint8_t *buf)
{
uint16_t	reg_index;
	reg_index = (buf[2]<<8 ) | buf[3];
	if ( reg_index > A_MAX_REGS )
		return A_rtu_modbus_exception(buf,A_MODBUS_RTU_ERR_ILLEGAL_ADDRESS);
	reg_index *= 2;
	A_modbus_inout.holding_registers[reg_index] = buf[4];
	A_modbus_inout.holding_registers[reg_index+1] = buf[5];
	A_rtu_modbus_write_reply(buf);
	return A_write_reg_from_modbus(reg_index,(buf[4]<<8) | buf[5]);
}

static uint8_t A_rtu_modbus_write_multiple_regs(uint8_t *buf)
{
uint16_t	reg_start,byte_count,i,k;
	reg_start = (buf[2]<<8 ) | buf[3];
	byte_count = buf[6];
	if ( reg_start > A_MAX_REGS )
		return A_rtu_modbus_exception(buf,A_MODBUS_RTU_ERR_ILLEGAL_ADDRESS);
	if ((reg_start+byte_count) > A_MAX_REGS )
		return A_rtu_modbus_exception(buf,A_MODBUS_RTU_ERR_ILLEGAL_DATA);

	for(i=0,k=reg_start;i<byte_count;i++,k++)
	{
		A_modbus_inout.holding_registers[k] = buf[i+7];
		A_write_reg_from_modbus(k,A_modbus_inout.holding_registers[k]);
	}
	A_rtu_modbus_write_reply(buf);
	return 0;
}

static uint8_t A_rtu_modbus_set_coils(uint8_t *buf,uint16_t	coil_start, uint16_t coil_num)
{
uint16_t	i,j,rx_index,byte_index,bit_offset,byte_count,byte_op,byte_rx;

	byte_count = coil_start+coil_num;
	byte_index = coil_start / 8;
	bit_offset = coil_start - (byte_index*8);
	rx_index = 0;
	for(i=coil_start,j=bit_offset;i<byte_count;i++,j++)
	{
		if ( j == 8 )
		{
			j = 0;
			rx_index++;
			byte_index++;
		}
		if ( coil_num == 1 )
			byte_rx = buf[rx_index+4];
		else
			byte_rx = buf[rx_index+7];
		byte_op = (byte_rx & (1 << j));
		A_modbus_inout.coils[byte_index] &= ~byte_op;
		A_modbus_inout.coils[byte_index] |= byte_op;
		A_write_coil_from_modbus(i,byte_op>>j);
		A_rtu_modbus_write_reply(buf);
	}
	return 0;
}

static uint8_t A_rtu_modbus_write_multiple_coils(uint8_t *buf)
{
uint16_t	coil_start,coil_num;
	coil_start = (buf[2]<<8 ) | buf[3];
	coil_num = (buf[4]<<8 ) | buf[5];
	if ( coil_start > A_MAX_COILS )
		return A_rtu_modbus_exception(buf,A_MODBUS_RTU_ERR_ILLEGAL_ADDRESS);
	if ((coil_start+coil_num) > A_MAX_COILS )
		return A_rtu_modbus_exception(buf,A_MODBUS_RTU_ERR_ILLEGAL_DATA);

	A_rtu_modbus_set_coils(buf,coil_start,coil_num);
	A_rtu_modbus_write_reply(buf);
	return 0;
}

static uint8_t A_rtu_modbus_write_reg_coil(uint8_t *buf)
{
	switch(buf[1])
	{
	case	A_MODBUS_WRITE_REGISTER :
		A_rtu_write_mb_register(buf);
		return 0;
		break;
	case	A_MODBUS_WRITE_COIL :
		A_rtu_modbus_set_coils(buf,(buf[2] <<8 ) | buf[3],1);
		return 0;
		break;
	}
	return A_rtu_modbus_exception(buf,A_MODBUS_RTU_ERR_ILLEGAL_FC);
}

uint32_t A_rtu_modbus_process(uint8_t *buf, uint16_t len)
{
uint8_t	ret_val = 1;

	if ( crc( buf, len) == 0 )
	{
		if ( buf[0] !=  A_modbus.modbus_addr)
			return 1;
		switch(buf[1])
		{
		case	A_MODBUS_READ_COIL					:
		case	A_MODBUS_READ_DISCRETE_INPUT			:
			ret_val = A_rtu_modbus_read_coils_discrete_in(buf);
			break;
		case	A_MODBUS_READ_REGISTER				:
		case	A_MODBUS_READ_INPUT_REGISTER		:
			ret_val = A_rtu_modbus_read_regs_iregs(buf);
			break;
		case	A_MODBUS_WRITE_COIL					:
		case	A_MODBUS_WRITE_REGISTER				:
			ret_val = A_rtu_modbus_write_reg_coil(buf);
			break;
		case	A_MODBUS_WRITE_MULTIPLE_COILS		:
			ret_val = A_rtu_modbus_write_multiple_coils(buf);
			break;
		case	A_MODBUS_WRITE_MULTIPLE_REGISTERS	:
			ret_val = A_rtu_modbus_write_multiple_regs(buf);
			break;
		case A_MODBUS_FC_REPORT_SLAVE_ID:
			ret_val = A_rtu_modbus_report_slave_info(buf);
			break;
		default	:
			ret_val = 1;
			break;
		}
	}
	if ( ret_val  )
		A_rtu_modbus_exception(buf,A_MODBUS_RTU_ERR_ILLEGAL_FC);
	hw_send_uart(A_modbus.modbus_uart,A_modbus.tx_packet, A_modbus.tx_packet_len);
	return ret_val;
}


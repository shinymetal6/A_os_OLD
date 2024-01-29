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
 * A_modbus.c
 *
 *  Created on: Oct 5, 2023
 *      Author: fil
 */

#include "main.h"
#include "A_modbus.h"
#include "A_modbus_rtu.h"

A_modbus_t			A_modbus;
A_modbus_inout_t	A_modbus_inout;

#ifndef A_MODBUS_SW_CRC
// Internal init to be agnostic about IDE setup
uint8_t A_modbus_CRC_Init(void)
{
	hcrc.Instance = CRC;
	hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_DISABLE;
	hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_DISABLE;
	hcrc.Init.GeneratingPolynomial = 32773;
	hcrc.Init.CRCLength = CRC_POLYLENGTH_16B;
	hcrc.Init.InitValue = 0xffff;
	hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_BYTE;
	hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_ENABLE;
	hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
	return HAL_CRC_Init(&hcrc);
}
#endif

void A_modbus_init(uint8_t address,uint8_t mode,uint32_t uart)
{
#ifndef A_MODBUS_SW_CRC
	HAL_CRC_DeInit(&hcrc);
	A_modbus_CRC_Init();
#endif
	A_modbus.modbus_state = mode;
	A_modbus.modbus_addr = address;
	A_modbus.modbus_uart = uart;
}

uint8_t A_modbus_process(uint8_t *buf, uint16_t len)
{
	if (( A_modbus.modbus_state & MODBUS_RTU_MODE) == MODBUS_RTU_MODE )
		return A_rtu_modbus_process(buf,len);
	return 0;
}

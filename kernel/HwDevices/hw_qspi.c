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
 * Project : A_os_F746gdisco_devel 
*/
/*
 * hw_qspi.c
 *
 *  Created on: Sep 26, 2023
 *      Author: fil
 */

#include "main.h"
#include "../A.h"
#include "../scheduler.h"
#include "../A_exported_functions.h"
#include "../hwmanager.h"
#include "../system_default.h"
#include "hw_qspi.h"

#if defined QSPI_ENABLED

extern	HWMngr_t	HWMngr[PERIPHERAL_NUM];
extern	Asys_t		Asys;

uint8_t A_qspi_enable_write(void)
{
	if ( HWMngr[HW_QSPI].process != Asys.current_process )
		return HW_QSPI_ERROR_HW_NOT_OWNED;
	 return qspi_WriteEnable();
}

uint8_t A_qspi_disable_write(void)
{
	if ( HWMngr[HW_QSPI].process != Asys.current_process )
		return HW_QSPI_ERROR_HW_NOT_OWNED;
	 return qspi_WriteDisable();
}

uint8_t A_qspi_erase_block(uint32_t BlockAddress)
{
	if ( HWMngr[HW_QSPI].process != Asys.current_process )
		return HW_QSPI_ERROR_HW_NOT_OWNED;
	 return qspi_Erase_Block(BlockAddress);
}

uint8_t A_qspi_erase_chip(void)
{
	if ( HWMngr[HW_QSPI].process != Asys.current_process )
		return HW_QSPI_ERROR_HW_NOT_OWNED;
	 return qspi_Erase_Chip();
}

uint8_t A_qspi_write(uint8_t* data,uint32_t addr, uint32_t size)
{
	if ( HWMngr[HW_QSPI].process != Asys.current_process )
		return HW_QSPI_ERROR_HW_NOT_OWNED;
	return qspi_Write(data, addr, size);
}

uint8_t A_qspi_read(uint8_t* data,uint32_t addr, uint32_t size)
{
	if ( HWMngr[HW_QSPI].process != Asys.current_process )
		return HW_QSPI_ERROR_HW_NOT_OWNED;
	return qspi_Read(data, addr, size);
}

QSPI_HandleTypeDef *A_qspi_init(void)
{
	if ( HWMngr[HW_QSPI].process != Asys.current_process )
		return 0;
	return qspi_init();
}

#endif





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
 * Project : A_Libraries
*/
/*
 * qspi.c
 *
 *  Created on: Sep 26, 2023
 *      Author: fil
 */
#include "main.h"
#include "../../kernel/A.h"
#include "../../kernel/system_default.h"

#if defined QSPI_ENABLED

#include "qspi.h"
QSPI_Info	QSPI_memory;

extern	Asys_t			Asys;

#include "w25q.h"

QSPI_HandleTypeDef *qspi_init(void)
{
#ifdef QSPI_WINBOND
	Asys.qspi_id = w25q_Init();
	QSPI_memory.FlashSize = MEM_FLASH_SIZE*1024*1024;
	QSPI_memory.EraseSectorSize = MEM_SECTOR_SIZE*1024;
	QSPI_memory.EraseSectorsNumber = SECTOR_COUNT;
	QSPI_memory.ProgPageSize = MEM_PAGE_SIZE;
	QSPI_memory.ProgPagesNumber = PAGE_COUNT;
#endif
	return &HQSPI1;
}

uint8_t qspi_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
{
#ifdef QSPI_WINBOND
	return w25q_ReadRaw(pData, Size, ReadAddr);
#endif
}

uint8_t qspi_WriteEnable(void)
{
#ifdef QSPI_WINBOND
	return w25q_WriteEnable(1);
#endif
}

uint8_t qspi_WriteDisable(void)
{
#ifdef QSPI_WINBOND
	return w25q_WriteEnable(0);
#endif
}

uint8_t qspi_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size)
{
#ifdef QSPI_WINBOND
	return w25q_ProgramRaw(pData, Size, WriteAddr);
#endif
}

uint8_t qspi_Erase_Block(uint32_t BlockAddress)
{
#ifdef QSPI_WINBOND
	return w25q_EraseBlock(BlockAddress, MEM_BLOCK_SIZE);
#endif
}

uint8_t qspi_Erase_Sector(uint32_t SectorAddress)
{
#ifdef QSPI_WINBOND
	return w25q_EraseSector(SectorAddress);
#endif
}

uint8_t qspi_Erase_Chip(void)
{
#ifdef QSPI_WINBOND
	return w25q_EraseChip();
#endif
}

#endif
/* micron */
/*
QSPI_CommandTypeDef qspi_command;
static void qspi_init_parameters(void)
{
	qspi_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	qspi_command.Instruction       = 0;
	qspi_command.AddressMode       = QSPI_ADDRESS_4_LINES;
	qspi_command.AddressSize       = QSPI_ADDRESS_24_BITS;
	qspi_command.Address           = 0;
	qspi_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	qspi_command.DataMode          = QSPI_DATA_4_LINES;
	qspi_command.DummyCycles       = N25Q128A_DUMMY_CYCLES_READ_QUAD;
	qspi_command.NbData            = 0;
	qspi_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	qspi_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	qspi_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
}

uint8_t qspi_WriteEnable(QSPI_HandleTypeDef *QSPIHandle)
{
	QSPI_CommandTypeDef     qspi_command;
	QSPI_AutoPollingTypeDef s_config;

	// Enable write operations
	qspi_command.Instruction       = WRITE_ENABLE_CMD;
	qspi_command.AddressMode       = QSPI_ADDRESS_NONE;
	qspi_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	qspi_command.DataMode          = QSPI_DATA_NONE;
	qspi_command.DummyCycles       = 0;

	if (HAL_QSPI_Command(QSPIHandle, &qspi_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return 1;

	// Configure automatic polling mode to wait for write enabling
	s_config.Match           = N25Q128A_SR_WREN;
	s_config.Mask            = N25Q128A_SR_WREN;
	s_config.MatchMode       = QSPI_MATCH_MODE_AND;
	s_config.StatusBytesSize = 1;
	s_config.Interval        = 0x10;
	s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

	qspi_command.Instruction    = READ_STATUS_REG_CMD;
	qspi_command.DataMode       = QSPI_DATA_1_LINE;

	return HAL_QSPI_AutoPolling(QSPIHandle, &qspi_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
}

uint8_t qspi_WriteDisable(QSPI_HandleTypeDef *QSPIHandle)
{
	QSPI_CommandTypeDef     qspi_command;
	QSPI_AutoPollingTypeDef s_config;

	// Enable write operations
	qspi_command.Instruction       = WRITE_DISABLE_CMD;
	qspi_command.AddressMode       = QSPI_ADDRESS_NONE;
	qspi_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	qspi_command.DataMode          = QSPI_DATA_NONE;
	qspi_command.DummyCycles       = 0;

	if (HAL_QSPI_Command(QSPIHandle, &qspi_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return 1;

	// Configure automatic polling mode to wait for write enabling
	s_config.Match           = N25Q128A_SR_WREN;
	s_config.Mask            = N25Q128A_SR_WREN;
	s_config.MatchMode       = QSPI_MATCH_MODE_AND;
	s_config.StatusBytesSize = 1;
	s_config.Interval        = 0x10;
	s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

	qspi_command.Instruction    = READ_STATUS_REG_CMD;
	qspi_command.DataMode       = QSPI_DATA_1_LINE;

	return HAL_QSPI_AutoPolling(QSPIHandle, &qspi_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
}

static uint8_t qspi_AutoPollingMemReady(QSPI_HandleTypeDef *QSPIHandle, uint32_t Timeout)
{
  QSPI_AutoPollingTypeDef s_config;

  // Configure automatic polling mode to wait for memory ready
  qspi_command.Instruction       = READ_STATUS_REG_CMD;
  qspi_command.AddressMode       = QSPI_ADDRESS_NONE;
  qspi_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  qspi_command.DataMode          = QSPI_DATA_1_LINE;
  qspi_command.DummyCycles       = 0;

  s_config.Match           = 0;
  s_config.Mask            = N25Q128A_SR_WIP;
  s_config.MatchMode       = QSPI_MATCH_MODE_AND;
  s_config.StatusBytesSize = 1;
  s_config.Interval        = 0x10;
  s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

  return HAL_QSPI_AutoPolling(QSPIHandle, &qspi_command, &s_config, Timeout);
}

uint8_t qspi_Read(QSPI_HandleTypeDef *QSPIHandle,uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
{
	// Initialize the read command
	qspi_command.AddressMode       = QSPI_ADDRESS_4_LINES;
	qspi_command.Instruction       = QUAD_INOUT_FAST_READ_CMD;
	qspi_command.Address           = ReadAddr;
	qspi_command.NbData            = Size;
	qspi_command.DummyCycles       = N25Q128A_DUMMY_CYCLES_READ_QUAD;

	// Configure the command
	if (HAL_QSPI_Command(QSPIHandle, &qspi_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return 1;
	// Set S# timing for Read command
	MODIFY_REG(QSPIHandle->Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_3_CYCLE);
	// Reception of the data
	if (HAL_QSPI_Receive(QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return 1;
	// Restore S# timing for nonRead commands
	MODIFY_REG(QSPIHandle->Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_6_CYCLE);
	return 0;
}

uint8_t qspi_Write(QSPI_HandleTypeDef *QSPIHandle,uint8_t* pData, uint32_t WriteAddr, uint32_t Size)
{
uint32_t end_addr, current_size, current_addr;

	// Calculation of the size between the write address and the end of the page
	current_size = N25Q128A_PAGE_SIZE - (WriteAddr % N25Q128A_PAGE_SIZE);

	// Check if the size of the data is less than the remaining place in the page
	if (current_size > Size)
		current_size = Size;
	// Initialize the adress variables
	current_addr = WriteAddr;
	end_addr = WriteAddr + Size;
	// Initialize the program command
	qspi_command.Instruction       = EXT_QUAD_IN_FAST_PROG_CMD;
	qspi_command.AddressMode       = QSPI_ADDRESS_4_LINES;
	qspi_command.AddressSize       = QSPI_ADDRESS_24_BITS;
	qspi_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	qspi_command.DataMode          = QSPI_DATA_4_LINES;
	qspi_command.DummyCycles       = 0;

	// Perform the write page by page
	do
	{
		qspi_command.Address = current_addr;
		qspi_command.NbData  = current_size;

		// Enable write operations
		if (qspi_WriteEnable(QSPIHandle) != 0)
			return 1;
		// Configure the command
		if (HAL_QSPI_Command(QSPIHandle, &qspi_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
			return 1;
		// Transmission of the data
		if (HAL_QSPI_Transmit(QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
			return 1;
		// Configure automatic polling mode to wait for end of program
		if (qspi_AutoPollingMemReady(QSPIHandle, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != 0)
			return 1;
		// Update the address and size variables for next page programming
		current_addr += current_size;
		pData += current_size;
		current_size = ((current_addr + N25Q128A_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : N25Q128A_PAGE_SIZE;
	} while (current_addr < end_addr);
	return 0;
}

uint8_t qspi_Erase_Block(QSPI_HandleTypeDef *QSPIHandle,uint32_t BlockAddress)
{
	// Initialize the erase command
	qspi_command.Instruction       = SUBSECTOR_ERASE_CMD;
	qspi_command.AddressMode       = QSPI_ADDRESS_1_LINE;
	qspi_command.AddressSize       = QSPI_ADDRESS_24_BITS;
	qspi_command.Address           = BlockAddress;
	qspi_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	qspi_command.DataMode          = QSPI_DATA_NONE;
	qspi_command.DummyCycles       = 0;

	// Enable write operations
	if (qspi_WriteEnable(QSPIHandle) != 0)
		return 1;
	// Send the command
	if (HAL_QSPI_Command(QSPIHandle, &qspi_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return 1;
	// Configure automatic polling mode to wait for end of erase
	return qspi_AutoPollingMemReady(QSPIHandle, N25Q128A_SUBSECTOR_ERASE_MAX_TIME);
}

uint8_t qspi_Erase_Chip(QSPI_HandleTypeDef *QSPIHandle)
{
	// Initialize the erase command
	qspi_command.Instruction       = BULK_ERASE_CMD;
	qspi_command.AddressMode       = QSPI_ADDRESS_NONE;
	qspi_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	qspi_command.DataMode          = QSPI_DATA_NONE;
	qspi_command.DummyCycles       = 0;

	// Enable write operations
	if (qspi_WriteEnable(QSPIHandle) != 0)
		return 1;
	// Send the command
	if (HAL_QSPI_Command(QSPIHandle, &qspi_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return 1;
	// Configure automatic polling mode to wait for end of erase
	return qspi_AutoPollingMemReady(QSPIHandle, N25Q128A_BULK_ERASE_MAX_TIME);
}


static uint8_t qspi_ResetMemory(QSPI_HandleTypeDef *QSPIHandle)
{
	// Initialize the reset enable command
	qspi_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	qspi_command.Instruction       = RESET_ENABLE_CMD;
	qspi_command.AddressMode       = QSPI_ADDRESS_NONE;
	qspi_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	qspi_command.DataMode          = QSPI_DATA_NONE;
	qspi_command.DummyCycles       = 0;
	// Send the command
	if (HAL_QSPI_Command(QSPIHandle, &qspi_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return 1;
	// Send the reset memory command
	qspi_command.Instruction = RESET_MEMORY_CMD;
	if (HAL_QSPI_Command(QSPIHandle, &qspi_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return 1;
	// Configure automatic polling mode to wait the memory is ready
	return qspi_AutoPollingMemReady(QSPIHandle, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
}


QSPI_HandleTypeDef *qspi_init(void)
{
	qspi_init_parameters();
	qspi_ResetMemory(&A_qspi);
	return &A_qspi;
}
*/

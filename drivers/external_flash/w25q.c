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
 * Project : fy-201023-00 
*/
/*
 * w25q.c
 *
 *  Created on: Dec 27, 2023
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/A.h"
#include "../../kernel/system_default.h"

extern	void task_delay(uint32_t tick_count);
extern	void A_memcpy(uint8_t *dest,uint8_t *source,uint16_t size);

#ifdef QSPI_ENABLED
#ifdef QSPI_WINBOND

#include "w25q.h"

static void set_com(QSPI_CommandTypeDef *com , uint8_t instruction,uint32_t instruction_mode, uint32_t address, uint32_t address_mode, uint32_t address_size, uint32_t nbdata,uint32_t datamode)
{
	com->InstructionMode = instruction_mode; 	// Instruction
	com->Instruction = instruction;	 				// Command
	com->AddressMode = address_mode;
	com->AddressSize = address_size;
	com->Address = address;
	com->AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	com->AlternateBytes = QSPI_ALTERNATE_BYTES_NONE;
	com->AlternateBytesSize = QSPI_ALTERNATE_BYTES_NONE;
	com->DummyCycles = 0;
	com->DataMode = datamode;
	com->NbData = nbdata;
	com->DdrMode = QSPI_DDR_MODE_DISABLE;
	com->DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	com->SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
}

static uint32_t page_to_addr(uint32_t pageNum, uint8_t pageShift)
{
	return pageNum * MEM_PAGE_SIZE + pageShift;
}

static uint8_t w25q_ReadStatusReg(uint8_t *reg_data, uint8_t reg_num)
{
QSPI_CommandTypeDef com;
uint32_t	instruction;

	if (reg_num == 1)
		instruction = W25Q_READ_SR1;
	else if (reg_num == 2)
		instruction = W25Q_READ_SR2;
	else if (reg_num == 3)
		instruction = W25Q_READ_SR3;
	else
		return W25Q_PARAM_ERR;
	set_com(&com , instruction,QSPI_INSTRUCTION_1_LINE, 0, QSPI_ADDRESS_NONE, QSPI_ADDRESS_NONE, 1,QSPI_DATA_1_LINE);

	if (HAL_QSPI_Command(&HQSPI1, &com, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return W25Q_SPI_ERR;
	if (HAL_QSPI_Receive(&HQSPI1, reg_data, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return W25Q_SPI_ERR;
	return W25Q_OK;
}
static uint8_t w25q_IsBusy(void)
{
uint8_t sr = 0;

	if (w25q_ReadStatusReg(&sr, 1) != W25Q_OK)
		return W25Q_CHIP_ERR;

	Asys.qspi_status &= ~ASYS_QSPI_BUSY;
	if ( sr & 0x01 )
		Asys.qspi_status |= ASYS_QSPI_BUSY;
	return sr & 0x01;
}

uint8_t w25q_WriteEnable(uint8_t enable)
{
QSPI_CommandTypeDef com;
uint32_t	instruction;

	instruction = enable ? W25Q_WRITE_ENABLE : W25Q_WRITE_DISABLE;
	set_com(&com , instruction,QSPI_INSTRUCTION_1_LINE, 0, QSPI_ADDRESS_NONE, QSPI_ADDRESS_NONE, 0,QSPI_DATA_NONE);

	if (HAL_QSPI_Command(&HQSPI1, &com, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return W25Q_SPI_ERR;
	task_delay(1); // Give a little time to sleep

	Asys.qspi_status |= ASYS_QSPI_WEL;

	return W25Q_OK;
}

static uint8_t w25q_WriteStatusReg(uint8_t reg_data, uint8_t reg_num)
{
QSPI_CommandTypeDef com;
uint32_t	instruction;

	while (w25q_IsBusy() == W25Q_BUSY)
		task_delay(1);

	if (w25q_WriteEnable(1) != W25Q_OK)
		return W25Q_CHIP_ERR;

	if (reg_num == 1)
		instruction = W25Q_WRITE_SR1;
	else if (reg_num == 2)
		instruction = W25Q_WRITE_SR2;
	else if (reg_num == 3)
		instruction = W25Q_WRITE_SR3;
	else
		return W25Q_PARAM_ERR;

	set_com(&com, instruction, QSPI_INSTRUCTION_1_LINE, 0, QSPI_ADDRESS_NONE, QSPI_ADDRESS_NONE, 1, QSPI_DATA_1_LINE);

	if (HAL_QSPI_Command(&HQSPI1, &com, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
		return W25Q_SPI_ERR;
	}
	if (HAL_QSPI_Transmit(&HQSPI1, &reg_data, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
		return W25Q_SPI_ERR;
	}
	return W25Q_OK;
}

uint8_t w25q_ReadStatusStruct(void)
{
uint8_t status_reg = 0;

	Asys.qspi_status = ASYS_QSPI_FOUND;

	// first portion
	if (w25q_ReadStatusReg(&status_reg, 1) != W25Q_OK)
		return 1;
	if ( status_reg & 0x01)
		Asys.qspi_status |= ASYS_QSPI_BUSY;
	if ( status_reg & 0x02)
		Asys.qspi_status |= ASYS_QSPI_WEL;

	// second portion
	if (w25q_ReadStatusReg(&status_reg, 2) != W25Q_OK)
		return 1;
	if ( status_reg & 0x02)
		Asys.qspi_status |= ASYS_QSPI_QE;
	if ( status_reg & 0x80)
		Asys.qspi_status |= ASYS_QSPI_SUS;

	// third portion
	if (w25q_ReadStatusReg(&status_reg, 3) != W25Q_OK)
		return 1;
	if ( status_reg & 0x01)
		Asys.qspi_status |= ASYS_QSPI_ADS;
	if ( status_reg & 0x02)
		Asys.qspi_status |= ASYS_QSPI_ADP;
	return 0;
}

uint8_t w25q_ReadID(uint8_t *buf)
{
QSPI_CommandTypeDef com;
	set_com(&com, W25Q_DEVID, QSPI_INSTRUCTION_1_LINE, 0, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, 1, QSPI_DATA_1_LINE);

	if (HAL_QSPI_Command(&HQSPI1, &com, HAL_QSPI_TIMEOUT_DEFAULT_VALUE)	!= HAL_OK)
		return W25Q_SPI_ERR;
	if (HAL_QSPI_Receive(&HQSPI1, buf, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return W25Q_SPI_ERR;
	return W25Q_OK;
}

uint8_t w25q_ReadRaw(uint8_t *buf, uint16_t data_len, uint32_t rawAddr)
{
QSPI_CommandTypeDef com;
uint32_t	Instruction,AddressSize;
uint8_t		ret_val;

	if (data_len > 256 || data_len == 0)
		return W25Q_PARAM_ERR;

	while (w25q_IsBusy() == W25Q_BUSY)
		task_delay(1);

#if MEM_FLASH_SIZE > 128U
	Instruction = W25Q_FAST_READ_QUAD_IO_4B;
	com.AddressSize = QSPI_ADDRESS_32_BITS;
#else
	Instruction = W25Q_FAST_READ_QUAD_IO;
	AddressSize = QSPI_ADDRESS_24_BITS;
#endif
	set_com(&com, Instruction, QSPI_INSTRUCTION_1_LINE, rawAddr, QSPI_ADDRESS_4_LINES, AddressSize, data_len, QSPI_DATA_4_LINES);
	com.DummyCycles = 6;

	ret_val = 0;
	if (HAL_QSPI_Command(&hqspi, &com, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) == HAL_OK)
		if (HAL_QSPI_Receive(&hqspi, buf, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) == HAL_OK)
			ret_val = W25Q_OK;
	ret_val = W25Q_SPI_ERR;
	com.DummyCycles = 0;
	return ret_val;
}

uint8_t w25q_ProgramRaw(uint8_t *buf, uint16_t data_len, uint32_t rawAddr)
{
QSPI_CommandTypeDef com;
uint32_t			Instruction,AddressSize;
uint8_t				state;

	if (data_len > 256 || data_len == 0)
		return W25Q_PARAM_ERR;

	while (w25q_IsBusy() == W25Q_BUSY)
		task_delay(1);

	state = w25q_WriteEnable(1);
	if (state != W25Q_OK)
		return state;

#if MEM_FLASH_SIZE > 128U
	Instruction = W25Q_PAGE_PROGRAM_QUAD_INP_4B;	 // Command
	AddressSize = QSPI_ADDRESS_32_BITS;
#else
	Instruction = W25Q_PAGE_PROGRAM_QUAD_INP;	 // Command
	AddressSize = QSPI_ADDRESS_24_BITS;
#endif

	set_com(&com, Instruction, QSPI_INSTRUCTION_1_LINE, rawAddr, QSPI_ADDRESS_1_LINE, AddressSize, data_len, QSPI_DATA_4_LINES);
	if (HAL_QSPI_Command(&hqspi, &com, HAL_QSPI_TIMEOUT_DEFAULT_VALUE)
			!= HAL_OK)
		return W25Q_SPI_ERR;

	if (HAL_QSPI_Transmit(&hqspi, buf, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return W25Q_SPI_ERR;

	while (w25q_IsBusy() == W25Q_BUSY)
		task_delay(1);

	return W25Q_OK;
}

uint8_t w25q_ReadData(uint8_t *buf, uint16_t len, uint8_t pageShift, uint32_t pageNum)
{
uint32_t rawAddr;
	if (pageNum >= PAGE_COUNT || len == 0 || len > 256 || pageShift > 256 - len)
		return W25Q_PARAM_ERR;
	rawAddr = page_to_addr(pageNum, pageShift);
	return w25q_ReadRaw(buf, len, rawAddr);
}

uint8_t w25q_EraseSector(uint32_t SectAddr)
{
QSPI_CommandTypeDef com;
uint32_t			Instruction,AddressSize;
uint32_t			rawAddr;
uint8_t				state;
	if (SectAddr >= SECTOR_COUNT)
		return W25Q_PARAM_ERR;

	while (w25q_IsBusy() == W25Q_BUSY)
		task_delay(1);

	state = w25q_WriteEnable(1);
	if (state != W25Q_OK)
		return state;
	rawAddr = SectAddr * MEM_SECTOR_SIZE * 1024U;

#if MEM_FLASH_SIZE > 128U
	Instruction = W25Q_SECTOR_ERASE_4B;	 // Command
	AddressSize = QSPI_ADDRESS_32_BITS;
#else
	Instruction = W25Q_SECTOR_ERASE;	 // Command
	AddressSize = QSPI_ADDRESS_24_BITS;
#endif
	set_com(&com, Instruction, QSPI_INSTRUCTION_1_LINE, rawAddr, QSPI_ADDRESS_1_LINE, AddressSize, 0, QSPI_DATA_NONE);

	if (HAL_QSPI_Command(&hqspi, &com, HAL_QSPI_TIMEOUT_DEFAULT_VALUE)
			!= HAL_OK)
		return W25Q_SPI_ERR;

	while (w25q_IsBusy() == W25Q_BUSY)
		task_delay(1);
	return W25Q_OK;
}

uint8_t w25q_EraseBlock(uint32_t BlockAddr, uint8_t size)
{
QSPI_CommandTypeDef com;
uint32_t			Instruction,AddressSize;
uint32_t			rawAddr;
uint8_t				state;
	if (size != 32 && size != 64)
		return W25Q_PARAM_ERR;
	if ((size == 64 && BlockAddr >= BLOCK_COUNT) || (size == 32 && BlockAddr >= BLOCK_COUNT * 2))
		return W25Q_PARAM_ERR;

	while (w25q_IsBusy() == W25Q_BUSY)
		task_delay(1);

	rawAddr = BlockAddr * MEM_SECTOR_SIZE * 1024U * 16;
	if (size == 32)
		rawAddr /= 2;

	state = w25q_WriteEnable(1);
	if (state != W25Q_OK)
		return state;

#if MEM_FLASH_SIZE > 128U
	Instruction = W25Q_64KB_BLOCK_ERASE_4B;	 // Command
	AddressSize = QSPI_ADDRESS_32_BITS;
#else
	Instruction = W25Q_32KB_BLOCK_ERASE;	 // Command
	AddressSize = QSPI_ADDRESS_24_BITS;
#endif

	set_com(&com, Instruction, QSPI_INSTRUCTION_1_LINE, rawAddr, QSPI_ADDRESS_1_LINE, AddressSize, 0, QSPI_DATA_NONE);
	if (HAL_QSPI_Command(&hqspi, &com, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return W25Q_SPI_ERR;

	while (w25q_IsBusy() == W25Q_BUSY)
		task_delay(1);

	return W25Q_OK;
}

uint8_t w25q_EraseChip(void)
{
QSPI_CommandTypeDef com;
uint8_t				state;

	while (w25q_IsBusy() == W25Q_BUSY)
		task_delay(1);

	state = w25q_WriteEnable(1);
	if (state != W25Q_OK)
		return state;

	set_com(&com, W25Q_CHIP_ERASE, QSPI_INSTRUCTION_1_LINE, 0, QSPI_ADDRESS_NONE, QSPI_ADDRESS_NONE, 0, QSPI_DATA_NONE);

	if (HAL_QSPI_Command(&hqspi, &com, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return W25Q_SPI_ERR;

	while (w25q_IsBusy() == W25Q_BUSY)
		task_delay(1);

	return W25Q_OK;
}

uint8_t w25q_Sleep(void)
{
QSPI_CommandTypeDef com;

	set_com(&com, W25Q_POWERDOWN, QSPI_INSTRUCTION_1_LINE, 0, QSPI_ADDRESS_NONE, QSPI_ADDRESS_NONE, 0, QSPI_DATA_NONE);
	if (HAL_QSPI_Command(&hqspi, &com, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
		return W25Q_SPI_ERR;
	}
	task_delay(1);
	Asys.qspi_status |= ASYS_QSPI_SLEEP;
	return W25Q_OK;
}

uint8_t w25q_WakeUP(void)
{
QSPI_CommandTypeDef com;

	set_com(&com, W25Q_POWERUP, QSPI_INSTRUCTION_1_LINE, 0, QSPI_ADDRESS_NONE, QSPI_ADDRESS_NONE, 0, QSPI_DATA_NONE);
	if (HAL_QSPI_Command(&hqspi, &com, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return W25Q_SPI_ERR;
	task_delay(1);
	Asys.qspi_status &= ~ASYS_QSPI_SLEEP;
	return W25Q_OK;
}

uint8_t w25q_Init(void)
{
uint8_t id = 0 , buf_reg = 0;

	// read id
	if (w25q_ReadID(&id) != W25Q_OK)
		return 0;

	// read chip's state to private lib's struct
	if (w25q_ReadStatusStruct() != W25Q_OK)
		return 0;

	if (!( Asys.qspi_status & ASYS_QSPI_QE))
	{
		if (w25q_ReadStatusReg(&buf_reg, 2) != W25Q_OK)
			return 0;
		buf_reg |= 0b10;
		if (w25q_WriteStatusReg(buf_reg, 2) != W25Q_OK)
			return 0;
	}

	// make another read
	if (w25q_ReadStatusStruct() != W25Q_OK)
		return 0;
	// return communication status
	return id;
}

#endif
#endif

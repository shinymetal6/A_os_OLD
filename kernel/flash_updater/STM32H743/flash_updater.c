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
 * flash_updater.c
 *
 *  Created on: Jan 31, 2024
 *      Author: fil
 */

#include "main.h"

#ifdef	STM32H743xx
#include "../../system_default.h"
#include "flash_updater.h"

extern	uint8_t	*_fdata_start,*_fdata_end,*_d2ram_start;

uint32_t	flash_uwTick=0;
DTCM_VECTORS_DATA	uint32_t	VectorTable_DTCM[VECTOR_TABLE_SIZE];

ITCM_AREA_CODE uint8_t WaitForLastFlashOperation(uint32_t Timeout, uint32_t Bank)
{
uint32_t bsyflag = FLASH_FLAG_QW_BANK1;
uint32_t errorflag = 0;

	if (Bank == FLASH_BANK_2)
		bsyflag = FLASH_FLAG_QW_BANK2;

	while(__HAL_FLASH_GET_FLAG(bsyflag))
	{
		flash_uwTick = 0;
		if(flash_uwTick > Timeout)
				return 1;
	}

	if (Bank == FLASH_BANK_1)
		errorflag = FLASH->SR1 & FLASH_FLAG_ALL_ERRORS_BANK1;
	else
		errorflag = (FLASH->SR2 & FLASH_FLAG_ALL_ERRORS_BANK2) | 0x80000000U;

	if((errorflag & 0x7FFFFFFFU) != 0U)
		return 1;

	if(Bank == FLASH_BANK_1)
	{
		if (__HAL_FLASH_GET_FLAG_BANK1(FLASH_FLAG_EOP_BANK1))
			__HAL_FLASH_CLEAR_FLAG_BANK1(FLASH_FLAG_EOP_BANK1);
	}
	else
	{
		if (__HAL_FLASH_GET_FLAG_BANK2(FLASH_FLAG_EOP_BANK2))
			__HAL_FLASH_CLEAR_FLAG_BANK2(FLASH_FLAG_EOP_BANK2);
	}
	return 0;
}

ITCM_AREA_CODE uint8_t Flash_Erase_Sector(uint32_t Sector, uint32_t Banks)
{
  if((Banks & FLASH_BANK_1) == FLASH_BANK_1)
  {
    /* Reset Program/erase VoltageRange and Sector Number for Bank1 */
    FLASH->CR1 &= ~(FLASH_CR_PSIZE | FLASH_CR_SNB);
    FLASH->CR1 |= (FLASH_CR_SER | FLASH_VOLTAGE_RANGE_3 | (Sector << FLASH_CR_SNB_Pos) | FLASH_CR_START);
  }
  if((Banks & FLASH_BANK_2) == FLASH_BANK_2)
  {
    FLASH->CR2 &= ~(FLASH_CR_PSIZE | FLASH_CR_SNB);
    FLASH->CR2 |= (FLASH_CR_SER | FLASH_VOLTAGE_RANGE_3  | (Sector << FLASH_CR_SNB_Pos) | FLASH_CR_START);
  }
  return WaitForLastFlashOperation((uint32_t)FLASH_TIMEOUT_VALUE, Banks);
}

ITCM_AREA_CODE uint8_t flash_unlock(void)
{
	if(READ_BIT(FLASH->CR1, FLASH_CR_LOCK) != 0U)
	{
		/* Authorize the FLASH Bank1 Registers access */
		WRITE_REG(FLASH->KEYR1, FLASH_KEY1);
		WRITE_REG(FLASH->KEYR1, FLASH_KEY2);

		/* Verify Flash Bank1 is unlocked */
		if (READ_BIT(FLASH->CR1, FLASH_CR_LOCK) != 0U)
			return 1;
	}

	if(READ_BIT(FLASH->CR2, FLASH_CR_LOCK) != 0U)
	{
		/* Authorize the FLASH Bank2 Registers access */
		WRITE_REG(FLASH->KEYR2, FLASH_KEY1);
		WRITE_REG(FLASH->KEYR2, FLASH_KEY2);

		/* Verify Flash Bank2 is unlocked */
		if (READ_BIT(FLASH->CR2, FLASH_CR_LOCK) != 0U)
			return 1;
	}
	return 0;
}

ITCM_AREA_CODE uint8_t flash_Lock(void)
{
	SET_BIT(FLASH->CR1, FLASH_CR_LOCK);

	if (READ_BIT(FLASH->CR1, FLASH_CR_LOCK) == 0U)
		return 1;

	/* Set the LOCK Bit to lock the FLASH Bank2 Control Register access */
	SET_BIT(FLASH->CR2, FLASH_CR_LOCK);

	/* Verify Flash Bank2 is locked */
	if (READ_BIT(FLASH->CR2, FLASH_CR_LOCK) == 0U)
		return 1;

	return 0;
}

ITCM_AREA_CODE uint8_t FLASH_32B_data(uint32_t TypeProgram, uint32_t FlashAddress, uint32_t DataAddress)
{
HAL_StatusTypeDef status;
__IO uint32_t *dest_addr = (__IO uint32_t *)FlashAddress;
__IO uint32_t *src_addr = (__IO uint32_t*)DataAddress;
uint32_t bank;
uint8_t row_index = FLASH_NB_32BITWORD_IN_FLASHWORD;

	if(IS_FLASH_PROGRAM_ADDRESS_BANK1(FlashAddress))
		bank = FLASH_BANK_1;
	else
		bank = FLASH_BANK_2;
	/* Wait for last operation to be completed */
	status = WaitForLastFlashOperation((uint32_t)FLASH_TIMEOUT_VALUE, bank);

	if(bank == FLASH_BANK_1)
		SET_BIT(FLASH->CR1, FLASH_CR_PG);
	else
		SET_BIT(FLASH->CR2, FLASH_CR_PG);
	__ISB();
	__DSB();

	/* Program the flash word */
	do
	{
		*dest_addr = *src_addr;
		dest_addr++;
		src_addr++;
		row_index--;
	} while (row_index != 0U);
	__ISB();
	__DSB();

	/* Wait for last operation to be completed */
	status = WaitForLastFlashOperation((uint32_t)FLASH_TIMEOUT_VALUE, bank);
	if( status )
		return 1;

	if(bank == FLASH_BANK_1)
		CLEAR_BIT(FLASH->CR1, FLASH_CR_PG);
	else
		CLEAR_BIT(FLASH->CR2, FLASH_CR_PG);
	return 0;
}

ITCM_AREA_CODE uint32_t flash_write(uint8_t const *src, uint8_t *dst,uint32_t size)
{
uint32_t FlashAddress = (uint32_t)dst , Sector, Banks;
uint32_t	i;

	Sector = (size / FLASH_SECTOR_SIZE)+1;

	if (FlashAddress & (32-1))
		return 1; // bad alignment
	if ( flash_unlock() )
		return 1;
	if(IS_FLASH_PROGRAM_ADDRESS_BANK1(FlashAddress))
		Banks = FLASH_BANK_1;
	else if(IS_FLASH_PROGRAM_ADDRESS_BANK2(FlashAddress))
		Banks = FLASH_BANK_2;
	else
		return 1;
	for(i=0;i<Sector;i++)
	{
		if ( Flash_Erase_Sector(i, Banks) )
			return 1;
	}
	for(i=0,FlashAddress = (uint32_t)dst;i<size;i+=32,FlashAddress+=32)
	{
		if (  FLASH_32B_data(FLASH_TYPEPROGRAM_FLASHWORD, FlashAddress, (uint32_t)(src + i)) != 0 )
			return 1;
	}
	if ( flash_Lock() )
		return 1;
	return 0;
}

ITCM_AREA_CODE void  FlashIrq_Error_Handler(void)
{
    __disable_irq();
    while(1);	// hangs badly
}

void  ITCM_AREA_CODE FlashSysTick_Handler(void)
{
	flash_uwTick += (uint32_t)HAL_TICK_FREQ_DEFAULT;
}

static void relocate_vtable_systick(void)
{
uint32_t i;
	// 1 -  sync with irqs and disable all
	HAL_Delay(1);
    __disable_irq();
	for(i=0;i<8;i++)
		NVIC->ICER[i] = 0xffffffff;
    __enable_irq();
	HAL_Delay(10);
    __disable_irq();
    // 2 - disable caches if enabled
    __DSB();
    __ISB();
    SCB->CCR &= ~(uint32_t)SCB_CCR_IC_Msk;  /* disable I-Cache */
    SCB->ICIALLU = 0UL;                     /* invalidate I-Cache */
    __DSB();
    __ISB();
    SCB->CCR &= ~(uint32_t)SCB_CCR_DC_Msk;  /* disable D-Cache */
    __DSB();
    __ISB();
	// 3 - compile a new vector table with only systick enabled
	for( i = 0; i < VECTOR_TABLE_SIZE; i++)
		VectorTable_DTCM[i] = (uint32_t )&(*FlashIrq_Error_Handler);
	VectorTable_DTCM[SYSTICK_VECTOR] = (uint32_t )&(*FlashSysTick_Handler);
	// 4 - set new vector table
    SCB->VTOR = ((uint32_t) &VectorTable_DTCM);
    // 5 - now only systick should be enabled, wait for some ticks after enabling irqs
    flash_uwTick=0;
    __enable_irq();
	while ( flash_uwTick < 4);
	// 7 - vectors relocated
}

ITCM_AREA_CODE void flash_update(uint8_t *flash_data,uint32_t size)
{
uint8_t	status;
	relocate_vtable_systick();
	status = flash_write(flash_data,&_fdata_start,size); // ADDR_FLASH_SECTOR_0_BANK2
    if ( status  )
    	{ while(1);	}	// error so loop forever
	CLEAR_BIT(FLASH->CR1, FLASH_CR_PG);
    // All done, restart
	NVIC_SystemReset();
}


ITCM_AREA_CODE uint32_t get_flash_storage_ptr(void)
{
	return (uint32_t )&_d2ram_start;
}

ITCM_AREA_CODE uint32_t get_flash_size(void)
{
uint32_t size = (&_fdata_end - &_fdata_start) + 32;
	return size * 4;
}

#define TEST_FLASH
#ifdef TEST_FLASH
#include <string.h>

void copy_code(uint8_t *storage_ptr)
{
uint32_t size = &_fdata_end - &_fdata_start;

	memcpy(storage_ptr,(uint8_t *)&_fdata_start,size*4);
}
#endif // #ifdef TEST_FLASH

#endif

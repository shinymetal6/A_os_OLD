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
 * qspi.h
 *
 *  Created on: Sep 26, 2023
 *      Author: fil
 */
#ifndef __QSPI_H
#define __QSPI_H

#ifdef __cplusplus
 extern "C" {
#endif 

#if defined QSPI_ENABLED

#include "main.h"
//#include "n25q128a.h"

extern	QSPI_HandleTypeDef 	hqspi;
#define	A_qspi				hqspi

/* N25Q128A13EF840E Micron memory */
/* Size of the flash */
#define QSPI_FLASH_SIZE            23     /* Address bus width to access whole memory space */
#define QSPI_PAGE_SIZE             256

/* This alias is added as the name of Memory mapped fucntion changed */   
#define BSP_QSPI_MemoryMappedMode  BSP_QSPI_EnableMemoryMappedMode   

typedef struct {
  uint32_t FlashSize;          /*!< Size of the flash */
  uint32_t EraseSectorSize;    /*!< Size of sectors for the erase operation */
  uint32_t EraseSectorsNumber; /*!< Number of sectors for the erase operation */
  uint32_t ProgPageSize;       /*!< Size of pages for the program operation */
  uint32_t ProgPagesNumber;    /*!< Number of pages for the program operation */
} QSPI_Info;


#ifdef __cplusplus
}
#endif
#endif	/* QSPI_ENABLED */
#endif	/* __QSPI_H */

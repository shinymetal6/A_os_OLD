/**
  ******************************************************************************
  * @file           : pac.c   (Persistent Analog Config)
  * @version        : v2.0_Cube
  * @brief          : This file implements the Persistent Analog Config management on flash
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V.
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/

#include "pac.h"
#include "rfal_analogConfig.h"
#include "flash_driver.h"
#include "rfal_crc.h"


/* Private typedef -----------------------------------------------------------*/

/* Configuration of Persistent Analog Config */
typedef struct
{
    const uint8_t  * table;               /* Current Analog Config Table          */
    uint32_t         crc32;               /* CRC32 of the current Analog Config   */
    uint32_t         crc32_neg;           /* !CRC32 of the current Analog Config  */
    uint32_t         crc32_orig;          /* CRC32 of the original Analog Config  */
    uint32_t         configLength;        /* Current Analog Config Table Length   */
    uint32_t         unused[3];           /* Padding to reach multiple of double-words (8 bytes alignment)
                                             and quad-words (16 bytes alignment)*/
} t_AnalogConfigPage;


/* Private define ------------------------------------------------------------*/

#define ANALOG_CONFIG_CRC_INITVAL        (0x6363)     /* Initial value to compute CRC */

#ifndef ANALOG_CONFIG_OFFSET
#define ANALOG_CONFIG_OFFSET             (0x00018000) /* Analog config offset in Flash memory */
#endif
#define ANALOG_CONFIG_ADDRESS            (FLASH_BASE + ANALOG_CONFIG_OFFSET)

#define ANALOG_CONFIG_FLASH_PAGE_NUM     ((ANALOG_CONFIG_OFFSET) / (FLASH_PAGE_SIZE))

#ifndef ANALOG_CONFIG_FLASH_PAGE_COUNT
#define ANALOG_CONFIG_FLASH_PAGE_COUNT   ((RFAL_ANALOG_CONFIG_TBL_SIZE + (FLASH_PAGE_SIZE - 1)) / (FLASH_PAGE_SIZE))
#endif

#define ANALOG_CONFIG_FLASH_SIZE         ((FLASH_PAGE_SIZE) * ANALOG_CONFIG_FLASH_PAGE_COUNT)

#define ANALOG_CONFIG_HEADER_ADDRESS     (ANALOG_CONFIG_ADDRESS)

#define ANALOG_CONFIG_DATA_ADDRESS       (ANALOG_CONFIG_ADDRESS    + sizeof(t_AnalogConfigPage))
#define ANALOG_CONFIG_DATA_LENGTH        (ANALOG_CONFIG_FLASH_SIZE - sizeof(t_AnalogConfigPage))


#define ANALOG_CONFIG_CRC_MARKER_BAD     0xbadd
#define ANALOG_CONFIG_CRC_MARKER_DEAD    0xdead
#define ANALOG_CONFIG_CRC_MARKER_CACA    0xcaca


/* Reserve an entire page of Flash memory */

#if defined( __CC_ARM ) || defined(__ARMCC_VERSION) /* compiled with Keil v5 or v6 */

#define ANALOG_CONFIG_HEADER_ADDR 0x08018000
#define ANALOG_CONFIG_DATA_ADDR   0x08018020

/* Trick to check the value manually set matches the computed defines */
extern bool checker_header_addr[ ANALOG_CONFIG_HEADER_ADDRESS == ANALOG_CONFIG_HEADER_ADDR ? 1 : -1 ];
extern bool checker_data_addr  [ ANALOG_CONFIG_DATA_ADDRESS   == ANALOG_CONFIG_DATA_ADDR   ? 1 : -1 ];

#define str(address)            #address
#define sectionAt(address)      ".ARM.__at_" str(address)

const t_AnalogConfigPage __attribute__(( section(sectionAt(ANALOG_CONFIG_HEADER_ADDR)) )) PersistentAnalogConfig  = {.table = NULL, .crc32 = ANALOG_CONFIG_CRC_MARKER_BAD, .crc32_neg = ANALOG_CONFIG_CRC_MARKER_DEAD, .crc32_orig = ANALOG_CONFIG_CRC_MARKER_CACA, .configLength = 0};
const uint8_t            __attribute__(( section(sectionAt(ANALOG_CONFIG_DATA_ADDR))   )) PersistentAnalogConfigTable[ANALOG_CONFIG_DATA_LENGTH] = {0};

#elif defined( __ICCARM__ ) /* compiled with IAR */

const t_AnalogConfigPage PersistentAnalogConfig @ ANALOG_CONFIG_HEADER_ADDRESS = {.table = NULL, .crc32 = ANALOG_CONFIG_CRC_MARKER_BAD, .crc32_neg = ANALOG_CONFIG_CRC_MARKER_DEAD, .crc32_orig = ANALOG_CONFIG_CRC_MARKER_CACA, .configLength= 0};
const uint8_t            PersistentAnalogConfigTable[ANALOG_CONFIG_DATA_LENGTH] @ ANALOG_CONFIG_DATA_ADDRESS = {0};

#elif defined( __GNUC__ ) /* compiled with GCC */

const t_AnalogConfigPage __attribute__((section(".ST25R_PAC"))) PersistentAnalogConfig  = {.table = NULL, .crc32 = ANALOG_CONFIG_CRC_MARKER_BAD, .crc32_neg = ANALOG_CONFIG_CRC_MARKER_DEAD, .crc32_orig = ANALOG_CONFIG_CRC_MARKER_CACA, .configLength= 0};
const uint8_t            __attribute__((section(".ST25R_PAC"))) PersistentAnalogConfigTable[ANALOG_CONFIG_DATA_LENGTH] = {0};

#endif


/* Private function prototypes -----------------------------------------------*/
bool persistentAnalogConfig_IsValid     (const t_AnalogConfigPage * cfg);
bool persistentAnalogConfig_IsInstalled (const t_AnalogConfigPage * cfg);


/*!
 *****************************************************************************
 * \brief Get the state Persistent Analog Config table
 *
 * return true when all Persistent Analog Config parameters are correct.
 *
 *****************************************************************************
 */
bool persistentAnalogConfig_IsValid(const t_AnalogConfigPage * cfg)
{
    /* Check if persistent Analog config table is valid */
    if (   (cfg->configLength == 0)
        || (cfg->configLength > sizeof(PersistentAnalogConfigTable))
        || (cfg->configLength > RFAL_ANALOG_CONFIG_TBL_SIZE)
        || (cfg->table == NULL)
        || ((cfg->crc32_neg ^ PersistentAnalogConfig.crc32) != 0xFFFFFFFFul) )
    {
        return(false);
    }

    return(true);
}

/*!
 *****************************************************************************
 * \brief Tell if Persistent Analog Config table has been installed
 *
 * return true when Persistent Analog Config parameters has been initialised. Initialised doesn't mean valid!
 *
 *****************************************************************************
 */
bool persistentAnalogConfig_IsInstalled(const t_AnalogConfigPage * cfg)
{
    /* Check if persistent Analog config table is valid */
    if (   (cfg->configLength == 0)
        && (cfg->crc32        == ANALOG_CONFIG_CRC_MARKER_BAD)
        && (cfg->crc32_neg    == ANALOG_CONFIG_CRC_MARKER_DEAD)
        && (cfg->crc32_orig   == ANALOG_CONFIG_CRC_MARKER_CACA)
        && (cfg->table        == NULL) )
    {
        return(false);
    }

    return(true);
}

/*!
 *****************************************************************************
 * \brief Enable Persistent Analog Config
 *
 * Install the Persistent Analog Config table if required or initialise the RAM table at startup
 *
 *****************************************************************************
 */
bool persistentAnalogConfig_Enable(void)
{
    /* Access to Permanent Analog Config needs to be protected from potential 
       compiler optimization.
       Data located in Flash memory can be considered unalterable by the 
       compiler, and access optimized, whereas its content is modified during 
       runtime (which the compiler can't predict) */
    t_AnalogConfigPage cfg = PersistentAnalogConfig;
    bool ret = true;
    
    if ( persistentAnalogConfig_IsInstalled(&cfg) == false)
    {
        /* The persistent table is empty or invalid, create it */
        ret = (persistentAnalogConfig_Program() == ERR_NONE);
    }
    else
    {
        rfalAnalogConfigListWriteRaw(cfg.table, cfg.configLength);
    }
    
    return ret;
}

/*!
 *****************************************************************************
 * \brief Calculate CRC16
 *
 * Calculate the CRC of the given buffer
 *
 *****************************************************************************
 */
uint16_t persistentAnalogConfig_CalcCrc16( const uint8_t *buf, uint16_t bufLen )
{
    return rfalCrcCalculateCcitt( ANALOG_CONFIG_CRC_INITVAL, buf, bufLen );
}

/*!
 *****************************************************************************
 * \brief Calculate CRC32
 *
 * Calculate the CRC of the given buffer
 *
 *****************************************************************************
 */
uint32_t persistentAnalogConfig_CalcHash( const uint8_t * buf, uint16_t bufLen )
{
    /* Compute CRC32_FNV-1a */
    const uint32_t  init = ((uint32_t)0x811c9dc5ul);
    uint32_t        crc  = init;
    const uint8_t * end  = buf + bufLen;
    while ( buf < end )
    {
        /* xor the bottom with the current octet */
        crc ^= ( uint32_t ) * buf;
        /* multiply by the 32 bit FNV magic prime mod 2^32 FNV_32_PRIME  = 0x01000193 --> 0001 0000 0000 0000 0001 1001 0011 */
        crc += ( crc << 1 ) + ( crc << 4 ) + ( crc << 7 ) + ( crc << 8 ) + ( crc << 24 );/* crc *= FNV_32_PRIME; */
        ++ buf;
    }
    return ( crc );
}

/*!
 *****************************************************************************
 * \brief Get CRC
 *
 * Retrieve the CRCs of the original and currently stored Analog Config table
 *
 *****************************************************************************
 */
void persistentAnalogConfig_GetHash( uint32_t *origCrc, uint32_t *curCrc )
{
    /* Access to Permanent Analog Config needs to be protected from potential 
       compiler optimization.
       Data located in Flash memory can be considered unalterable by the 
       compiler, and access optimized, whereas its content is modified during 
       runtime (which the compiler can't predict) */
    t_AnalogConfigPage cfg = PersistentAnalogConfig;

    if( origCrc != NULL )
    {
        *origCrc = cfg.crc32_orig;
    }
    
    if( curCrc != NULL )
    {
        *curCrc = cfg.crc32;
    }
}

/*!
 *****************************************************************************
 * \brief Program Persistent Analog Config page
 *
 * Retrieve actual Analog Configs in RAM and write it permanently to dedicated flash page
 *
 *****************************************************************************
 */

#if ((RFAL_ANALOG_CONFIG_TBL_SIZE) % 16) != 0
#error "RFAL Analog Config table size must be multiple of quad words due to HAL_FLASH_Program() pace"
#endif

/* Allocate uint64_t array to align on 64-bit boundary ( size / sizeof(uint64_t) to get size in bytes) */
static uint64_t tmpAC[(RFAL_ANALOG_CONFIG_TBL_SIZE) / sizeof(uint64_t)];

stError persistentAnalogConfig_Program( void )
{
    stError             err = ERR_NONE;
    ReturnCode          ret;
    t_AnalogConfigPage  AC_Config;
    uint16_t            sizeAC;

    /* Retrieve current RFAL AC Table from RAM */
    ret = rfalAnalogConfigListReadRaw( (uint8_t*)tmpAC, sizeof(tmpAC), & sizeAC );
    if (ret != RFAL_ERR_NONE)
    {
        return ERR_IO;
    }

    /* Compute checksum */
    uint32_t checksum32 = persistentAnalogConfig_CalcHash( (uint8_t*)tmpAC, sizeAC );

#if defined(STM32L4) || defined(STM32G0)
    uint32_t pad_count = sizeof(uint64_t) - (sizeAC % sizeof(uint64_t));
#elif defined(STM32U5)
    uint32_t pad_count = (2*sizeof(uint64_t)) - (sizeAC % (2*sizeof(uint64_t)));
#endif
    if (pad_count != 0)
    {
        for (uint32_t i = 0; i < pad_count; i++)
        {
            tmpAC[sizeAC + i] = 0;
        }
    }

    /* Construct header info */
    memcpy(& AC_Config, & PersistentAnalogConfig, sizeof(AC_Config));
    AC_Config.table        = PersistentAnalogConfigTable; /* point to persistent page */
    AC_Config.configLength = (uint32_t )sizeAC;           /* change the original size */
    AC_Config.crc32        = (uint32_t )checksum32;       /* store computed crc */
    AC_Config.crc32_neg    = (uint32_t )~checksum32;      /* store crc complement */
    if (AC_Config.crc32_orig == ANALOG_CONFIG_CRC_MARKER_CACA)
    {
        AC_Config.crc32_orig = checksum32; /* Set original value */
    }

    /* Erase page(s) */
    err = flash_PageErase(ANALOG_CONFIG_FLASH_PAGE_NUM, ANALOG_CONFIG_FLASH_PAGE_COUNT);
    if (err != ERR_NONE)
    {
        return ERR_IO;
    }

    /* Program data */
    err = flash_Program((uint32_t) & PersistentAnalogConfigTable, tmpAC, sizeAC);
    if (err != ERR_NONE)
    {
        err = ERR_WRITE;
    }

    /* Compute the CRC really in flash */
    AC_Config.crc32_neg = (uint32_t )~ persistentAnalogConfig_CalcHash( PersistentAnalogConfigTable, sizeAC );
    
    /* Program the header now */
    if (flash_Program((uint32_t) & PersistentAnalogConfig, (uint64_t*) & AC_Config, sizeof(t_AnalogConfigPage)) != ERR_NONE)
    {
        /* Don't overwrite previous error */
        if (err == ERR_NONE)
        {
            err = ERR_WRITE;
        }
    }

    return err;
}

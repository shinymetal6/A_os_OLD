/**
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  * 1. Redistributions of source code must retain the above copyright notice,
  * this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  * this list of conditions and the following disclaimer in the documentation
  * and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of its contributors
  * may be used to endorse or promote products derived from this software
  * without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
*/

/*! \file
 *
 *  \author
 *
 *  \brief routine for programming/erasing flash.
 *
 */

/* Includes ------------------------------------------------------------------*/

#include "flash_driver.h"
#include "main.h"


/*!
 *****************************************************************************
 * \brief Erase given page(s) in Flash
 *
 * Erase page(s) in flash. Programming is done from RAM to allow making changes in code page
 *
 *****************************************************************************
 */
stError flash_PageErase(uint32_t pageStart, uint32_t nb)
{
    uint32_t               pageErr;
    HAL_StatusTypeDef      status;

    FLASH_EraseInitTypeDef flashEraseParam =
    {
      .TypeErase = FLASH_TYPEERASE_PAGES,
      .Banks     = (pageStart >= 256)? FLASH_BANK_2 : FLASH_BANK_1,
      .Page      = pageStart,
      .NbPages   = nb,
    };

    /* Disable interrupts to avoid any interruption during the loop */
    __disable_irq();
    /* Unlock Flash */
    HAL_FLASH_Unlock();
    /* Restart from a clean state */
#if defined(STM32L4) || defined(STM32U5)
        __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
#elif defined(STM32G0)
        FLASH->SR = FLASH_SR_ERRORS;
#endif
    /* Mass Erase whole bank 2 */
    status = HAL_FLASHEx_Erase(& flashEraseParam, & pageErr);
    /* Lock Flash */
    HAL_FLASH_Lock();
    /* Re-enable the interrupts */
    __enable_irq();

    if (status != HAL_OK)
    {
        return ERR_INTERNAL;
    }
    return ERR_NONE;
}

/*!
 *****************************************************************************
 * \brief program 64 bits blocks in flash
 *
 * copy 64 bits memory blocks to flash.
 *
 *****************************************************************************
 */
stError flash_Program(uint32_t flashAddr, const uint64_t* data, uint32_t length)
{
    stError           err = ERR_NONE;
    HAL_StatusTypeDef status;

    /* Disable interrupts to avoid any interruption during the loop */
    __disable_irq();
    /* Unlock Flash */
    HAL_FLASH_Unlock();

#if defined(STM32L4) || defined(STM32G0)
    /* round to next double-word (i.e. 8 bytes, 64-bit) boundary */
    uint32_t count = length + (sizeof(uint64_t) - 1);
    count = count / sizeof(uint64_t);
#elif defined(STM32U5)
    /* round to next quad-word (i.e. 16 bytes, 128-bit) boundary */
    uint32_t count = length + (2 * sizeof(uint64_t) - 1);
    count = count / (2 * sizeof(uint64_t));
#endif

    /* Program data */
    for (uint32_t i = 0; i < count; i++)
    {
#if defined(STM32L4) || defined(STM32G0)
        /* Program 64-bit word */
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, flashAddr, *data);
        data++;       /* + 8 bytes */
        flashAddr += (sizeof(uint64_t));
#elif defined(STM32U5)
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, flashAddr, (uint32_t)data);
        data += 2;   /* + 16 bytes */
        flashAddr += (2 * sizeof(uint64_t));
#endif
        if (status != HAL_OK)
        {
            err = ERR_WRITE;
#if defined(STM32L4) || defined(STM32U5)
            __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
#elif defined(STM32G0)
            FLASH->SR = FLASH_SR_ERRORS;
#endif
        }
    }

    /* Lock Flash */
    status = HAL_FLASH_Lock();
    /* Re-enable the interrupts */
    __enable_irq();

    if (status != HAL_OK && err == ERR_NONE)
    {
        err = ERR_IO;
    }
    return err;
}

/*
 * Platform_STM32H5xx.c
 *
 *  Created on: Aug 4, 2023
 *      Author: fil
 */

#include "main.h"
#include "../RFAL/st25r3916/st25r3916_irq.h"
#include "Platform_STM32H7xx.h"
#include "../../../../RollonProcesses/Application/app.h"
#include <string.h>

uint8_t globalCommProtectCnt = 0;


void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
	st25r3916Isr();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if ( htim == &TIM_TICK_10MSEC )
	{
		System.tick_flags |= TICK_10MSEC;
	}
}

int32_t BSP_SPI1_Init(void)
{
	return HAL_OK;
}

int32_t BSP_SPI1_Send(const uint8_t * const pData, const uint16_t Length)
{
  HAL_StatusTypeDef status;
  uint8_t   tx[256];
  int32_t ret = BSP_ERROR_NONE;

  if (pData != NULL)
  {
    (void)memcpy(tx, pData, Length);
  }

  status = HAL_SPI_Transmit(&ST25R3915_SPI, tx, Length, BUS_SPI1_TIMEOUT);

  /* Check the communication status */
  if (status != HAL_OK)
  {
    /* Execute user timeout callback */
    ret = BSP_SPI1_Init();
  }

  return ret;
}

int32_t BSP_SPI1_Recv(uint8_t * const pData, const uint16_t Length)
{
  HAL_StatusTypeDef status;
  uint8_t   rx[256];
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  status = HAL_SPI_Receive(&ST25R3915_SPI, (pData != NULL) ? (uint8_t *)pData : rx, Length, BUS_SPI1_TIMEOUT);

  /* Check the communication status */
  if (status != HAL_OK)
  {
    /* Execute user timeout callback */
    ret = BSP_SPI1_Init();
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

int32_t BSP_SPI1_SendRecv(const uint8_t * const pTxData, uint8_t * const pRxData, uint16_t Length)
{
  HAL_StatusTypeDef status;
  int32_t ret = BSP_ERROR_NONE;

  if((pTxData != NULL)&&(pRxData != NULL))
  {
    status = HAL_SPI_TransmitReceive(&ST25R3915_SPI, (uint8_t *)pTxData, (uint8_t *)pRxData, Length, BUS_SPI1_TIMEOUT);
  } else if ((pTxData != NULL)&&(pRxData == NULL)) {
    status = HAL_SPI_Transmit(&ST25R3915_SPI, (uint8_t *)pTxData, Length, BUS_SPI1_TIMEOUT);
  } else if ((pTxData == NULL)&&(pRxData != NULL)) {
    status = HAL_SPI_Receive(&ST25R3915_SPI, (uint8_t *)pRxData, Length, BUS_SPI1_TIMEOUT);
  }
  /* Check the communication status */
  if (status != HAL_OK)
  {
    /* Execute user timeout callback */
    ret = BSP_SPI1_Init();
  }

  return ret;
}

/**
  * @brief  SPI error treatment function
  * @param  None
  * @return BSP status
  */
int32_t BSP_SPI1_Error(void)
{
  /* De-initialize the SPI communication BUS */
  (void)HAL_SPI_DeInit(&ST25R3915_SPI);

  /* Re-Initiaize the SPI communication BUS */
  return BSP_SPI1_Init();
}

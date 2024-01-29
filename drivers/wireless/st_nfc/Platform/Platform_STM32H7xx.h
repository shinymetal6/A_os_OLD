/*
 * Platform_STM32H5xx.h
 *
 *  Created on: Aug 4, 2023
 *      Author: fil
 */

#ifndef BSP_PLATFORM_STM32H5XX_PLATFORM_STM32H5XX_H_
#define BSP_PLATFORM_STM32H5XX_PLATFORM_STM32H5XX_H_

#include "../../../../kernel/system_default.h"
extern	ETH_TxPacketConfig		TxConfig;
extern	ETH_DMADescTypeDef		DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
extern	ETH_DMADescTypeDef		DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */
//extern	ADC_HandleTypeDef		hadc1;
extern	CRC_HandleTypeDef 		hcrc;
extern	ETH_HandleTypeDef		heth;
extern	UART_HandleTypeDef		hlpuart1;
extern	UART_HandleTypeDef		huart3;
//extern	SPI_HandleTypeDef		hspi1;
//#define	ST25R3915_SPI			hspi1
extern	TIM_HandleTypeDef		htim6;
extern	PCD_HandleTypeDef		hpcd_USB_DRD_FS;

#ifdef NO_KERNEL
#define BSP_ERROR_NONE                     0
#define BSP_ERROR_NO_INIT                 -1
#define BSP_ERROR_WRONG_PARAM             -2
#define BSP_ERROR_BUSY                    -3
#define BSP_ERROR_PERIPH_FAILURE          -4
#define BSP_ERROR_COMPONENT_FAILURE       -5
#define BSP_ERROR_UNKNOWN_FAILURE         -6
#define BSP_ERROR_UNKNOWN_COMPONENT       -7
#define BSP_ERROR_BUS_FAILURE             -8
#define BSP_ERROR_CLOCK_FAILURE           -9
#define BSP_ERROR_MSP_FAILURE             -10
#define BSP_ERROR_FEATURE_NOT_SUPPORTED   -11

#define BUS_SPI1_TIMEOUT        2000U /* baud rate of SPI1 = 5 Mbps*/

#define ST25R_SS_PIN             ST25R391X_SS_Pin              /*!< GPIO pin used for ST25R SPI SS                */
#define ST25R_SS_PORT            ST25R391X_SS_GPIO_Port        /*!< GPIO port used for ST25R SPI SS port          */

#define ST25R_INT_PIN            ST25R391X_INT_Pin            /*!< GPIO pin used for ST25R External Interrupt    */
#define ST25R_INT_PORT           ST25R391X_INT_GPIO_Port      /*!< GPIO port used for ST25R External Interrupt   */
#endif

#endif /* BSP_PLATFORM_STM32H5XX_PLATFORM_STM32H5XX_H_ */

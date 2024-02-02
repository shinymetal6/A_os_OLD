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
 * STM32H743ZI2_NUCLEO.h
 *
 *  Created on: Jan 24, 2024
 *      Author: fil
 */

#ifndef BOARDS_STM32H743ZI2_NUCLEO_H_
#define BOARDS_STM32H743ZI2_NUCLEO_H_

/* Memories */
#define	POOL_START			    0x38000000
/* Note : SRAM_START must be equal to osSegment in ld file */
#define SRAM_START               0x38000000
#define SRAM_SIZE                65536

/* I/O */
#define	DEBUG_GPIOPORT			PG6_Debug_GPIO_Port
#define	DEBUG_GPIOBIT			PG6_Debug_Pin
#define	LED_1_GPIOPORT			LD1_GPIO_Port
#define	LED_1_GPIOBIT			LD1_Pin
#define	LED_2_GPIOPORT			LD2_GPIO_Port
#define	LED_2_GPIOBIT			LD2_Pin
#define	LED_3_GPIOPORT			LD3_GPIO_Port
#define	LED_3_GPIOBIT			LD3_Pin
#define	BUTTON_GPIOPORT			B1_GPIO_Port
#define	BUTTON_GPIOBIT			B1_Pin
#define	ST25R3916B				1

/* Clock */
#define TICK_HZ 				1000U
#define HSI_CLOCK         		480000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK
/* Others */
#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		12
#define	ASSIGNED				1

extern	UART_HandleTypeDef 	huart3;
#define	CONSOLE				huart3

//#define	ST_NFC_ENABLED
#ifdef	ST_NFC_ENABLED

	extern	SPI_HandleTypeDef		hspi1;
	#define	ST25R3915_SPI			hspi1

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

	/*
#define DMA_NOCACHE_RAM		__attribute__((section(".dmaNoCache")))   __attribute__ ((aligned (32)))
#define OSCILLATORS_RAM		__attribute__((section(".oscillators"))) __attribute__ ((aligned (32)))
#define AUDIOBUFS_RAM		__attribute__((section(".audiobufs")))   __attribute__ ((aligned (32)))
*/
#define	ITCM_AREA_CODE		__attribute__((section(".RamITCMFunc"))) __attribute__ ((aligned (32)))
#define DTCM_VECTORS_DATA	__attribute__((section(".dtcm_data")))   __attribute__ ((aligned (32)))

#define	BOARD_NAME			"STM32H743ZI2_NUCLEO"

#endif /* BOARDS_STM32H743ZI2_NUCLEO_H_ */

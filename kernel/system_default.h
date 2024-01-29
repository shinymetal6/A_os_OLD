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
 * system_default.h
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */

#ifndef KERNEL_SYSTEM_DEFAULT_H_
#define KERNEL_SYSTEM_DEFAULT_H_

//#define	FY_201023_00	1
//#define	STM32H743ZI2_NUCLEO	1
//#define	BB2xx_743			1

#ifdef	STM32H743xx
	#ifdef STM32H743ZI2_NUCLEO
		#include "../boards/STM32H743ZI2_NUCLEO.h"
	#endif
	#ifdef	FY_201023_00
		#include "../boards/FY_201023_00.h"
	#endif
	#ifdef	BB2xx_743
		#include "../boards/BB2xx_743.h"
	#endif

#ifdef INTERNAL_DEFS
	#ifdef	FY_201023_00
		/* Memories */
		#define	POOL_START			    0x38000000
		/* Note : SRAM_START must be equal to osSegment in ld file */
		#define SRAM_START               0x38000000
		#define SRAM_SIZE                65536

		/* I/O */
		#define	DEBUG_GPIOPORT			PG6_Debug_GPIO_Port
		#define	DEBUG_GPIOBIT			PG6_Debug_Pin
		#define	LED_1_GPIOPORT			LED_GPIO_Port
		#define	LED_1_GPIOBIT			LED_Pin

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

		#define	QSPI_ENABLED			1
		#define	WIRELESS_ENABLED		1
		#define	WIRELESS_NRF24L01		1
		#define	LORA_ENABLED			1
		#define	ETH_ENABLED				1
		#define	USB_ENABLED				1
		#undef 	LWIP_DHCP


		#ifdef QSPI_ENABLED
			#define	QSPI_WINBOND			1
			extern	QSPI_HandleTypeDef 	hqspi;
			#define	HQSPI1				hqspi
		#endif // #ifdef QSPI_ENABLED
		#ifdef WIRELESS_ENABLED
			#ifdef WIRELESS_NRF24L01
				extern	SPI_HandleTypeDef 					hspi1;
				#define NRF24L01_SPI						hspi1
				#define NRF24L01_SPI_TIMEOUT				2000

				#define NRF24L01_SS_PIN_PORT				ISM_SS_GPIO_Port
				#define NRF24L01_SS_PIN_NUMBER				ISM_SS_Pin

				#define NRF24L01_CE_PIN_PORT				ISM_CE_GPIO_Port
				#define NRF24L01_CE_PIN_NUMBER				ISM_CE_Pin

				#define NRF24L01_IRQ_PIN_PORT				ISM_IRQ_GPIO_Port
				#define NRF24L01_IRQ_PIN_NUMBER				ISM_IRQ_Pin
			#endif // #ifdef WIRELESS_NRF24L01
			#ifdef LORA_ENABLED
				extern	SPI_HandleTypeDef 					hspi1;
				#define SX126X_SPI							hspi1
				#define SX126X_SPI_TIMEOUT					2000

				#define SX126X_SS_PIN_PORT					SX126x_SS_GPIO_Port
				#define SX126X_SS_PIN_NUMBER				SX126x_SS_Pin

				#define SX126X_BUSY_PIN_PORT				SX126x_BUSY_GPIO_Port
				#define SX126X_BUSY_PIN_NUMBER				SX126x_BUSY_Pin

				#define SX126X_DIO1_PIN_PORT				SX126x_DIO1_GPIO_Port
				#define SX126X_DIO1_PIN_NUMBER				SX126x_DIO1_Pin

				#define SX126X_RESET_GPIO_Port				SX126x_RESET_GPIO_Port
				#define SX126X_RESET_Pin					SX126x_RESET_Pin
			#endif // #ifdef LORA_ENABLED
		#endif // #ifdef WIRELESS_ENABLED
		#define	BOARD_NAME			"FY-201023-00"
	#endif
	#ifdef BB2xx_743
		/* Memories */
		#define	POOL_START			    0x38000000
		/* Note : SRAM_START must be equal to osSegment in ld file */
		#define SRAM_START               0x38000000
		#define SRAM_SIZE                65536

		/* I/O */
			/*
		#define	DEBUG_GPIOPORT			PG6_Debug_GPIO_Port
		#define	DEBUG_GPIOBIT			PG6_Debug_Pin
		*/
		#define	LED_1_GPIOPORT			LED_GPIO_Port
		#define	LED_1_GPIOBIT			LED_Pin

		/* Clock */
		#define TICK_HZ 				1000U
		#define HSI_CLOCK         		480000000U
		#define SYSTICK_TIM_CLK   		HSI_CLOCK
		/* Others */
		#define	ASSIGNED				1

		extern	UART_HandleTypeDef 	huart3;
		#define	CONSOLE				huart3

		#define	POWERSAVE_ENABLED		1
		#define	QSPI_ENABLED			1
		#define	SDCARD_ENABLED			1
		//#define	WIRELESS_ENABLED		1
		#define	LCD_096_ENABLED			1
		#define	ENCODER_ENABLED			1
		#define	CODEC_ENABLED			1
		#define	CODEC_NAU88C22			1
		#define	HWRANDOM_GEN			1
		#define	ADC_ENABLED				1
		#define	DAC_ENABLED				1
		#define	USB_ENABLED				1
		#define	I2STX_PRIORITY			7
		#define	I2SRX_PRIORITY			7

		#ifdef USB_ENABLED
			//#define	USB_CDC				1
			#define	USB_MIDI			1
		#endif // #ifdef QSPI_ENABLED


		#ifdef QSPI_ENABLED
			#define	QSPI_WINBOND			1
			extern	QSPI_HandleTypeDef 	hqspi;
			#define	HQSPI1				hqspi
		#endif // #ifdef QSPI_ENABLED


		#ifdef SDCARD_ENABLED
			#define	MICROSD_DETECT_IOPORT	SD_DETECT_GPIO_Port
			#define	MICROSD_DETECT_PIN		SD_DETECT_Pin
		#endif // #ifdef SDCARD_ENABLED

		#ifdef ADC_ENABLED
				extern	ADC_HandleTypeDef 	hadc1;
				extern	ADC_HandleTypeDef 	hadc2;
				extern	TIM_HandleTypeDef 	htim6;
				#define	CONTROL_ADC1		hadc1
				#define	ANALOG_IN_ADC2		hadc2
				#define	CONTROL_TIMER		htim6
		#endif // #ifdef ADC_DAC_ENABLED

		#ifdef DAC_ENABLED
			extern	DAC_HandleTypeDef hdac1;
		#endif // #ifdef DAC_ENABLED

		#ifdef LCD_096_ENABLED
			extern SPI_HandleTypeDef hspi1;
			#define ST7735_SPI_PORT 					hspi1
			#define ST7735_SPI_TIMEOUT					2000

			#define ST7735_CS_GPIO_Port					LCD_SS_GPIO_Port
			#define ST7735_CS_Pin						LCD_SS_Pin

			#define ST7735_DC_GPIO_Port					LCD_DC_GPIO_Port
			#define ST7735_DC_Pin						LCD_DC_Pin

			#define ST7735_RES_GPIO_Port				LCD_RESET_GPIO_Port
			#define ST7735_RES_Pin						LCD_RESET_Pin
		#endif // #ifdef LCD_096_ENABLED

		#ifdef ENCODER_ENABLED
			#define ENCODER_BTN_Port					ENCODER1_BUTTON_GPIO_Port
			#define ENCODER_BTN_Pin						ENCODER1_BTN_Pin

			#define ENCODER_CLOCK_Port					ENCODER1_PHASE_GPIO_Port
			#define ENCODER_CLOCK_Pin					ENCODER1_PHASE_Pin

			#define ENCODER_DATA_Port					ENCODER1_DATA_GPIO_Port
			#define ENCODER_DATA_Pin					ENCODER1_DATA_Pin
		#endif // #ifdef ENCODER_ENABLED

		#ifdef CODEC_ENABLED
			#define AUDIO_BUF_SIZE 			256
			#ifdef CODEC_NAU88C22
				#define SYNTH_ENGINE_ENABLE	1
				#define NAU88C22_ADDR		(0x1a << 1)
				#define NAU88C22_TIMEOUT	(1000)
				extern	I2C_HandleTypeDef 	hi2c1;
				#define	NAU88C22_I2C		hi2c1
				extern	I2S_HandleTypeDef 	hi2s2;
				#define	NAU88C22_I2S		hi2s2
				#define	EXT_FREQ_12_288MHZ	1
			#endif // #ifdef CODEC_NAU88C22
		#endif // #ifdef CODEC_ENABLED

		#define DMA_NOCACHE_RAM		__attribute__((section(".dma_nocache_Segment"))) __attribute__ ((aligned (32)))

		#define	BOARD_NAME			"BB2xx_743"
		#define	MACHINE_NAME		"BB201_VCO"
		#define	MACHINE_VERSION		"A"
	#endif



#endif //#ifdef EXTERNAL_DEFS

#endif	//#ifdef	STM32H743xx

#ifdef	STM32H563xx
/* Memories */
#define	POOL_START			    0x20070000
/* Note : SRAM_START must be equal to osSegment in ld file */
#define SRAM_START               0x20080000
#define SRAM_SIZE                32768

/* I/O */

#define	DEBUG_GPIOPORT			PG6_Debug_GPIO_Port
#define	DEBUG_GPIOBIT			PG6_Debug_Pin
#define	LED_1_GPIOPORT			LED3_RED_GPIO_Port
#define	LED_1_GPIOBIT			LED3_RED_Pin
#define	LED_2_GPIOPORT			LED1_GREEN_GPIO_Port
#define	LED_2_GPIOBIT			LED1_GREEN_Pin
#define	LED_3_GPIOPORT			LED2_YELLOW_GPIO_Port
#define	LED_3_GPIOBIT			LED2_YELLOW_Pin
#define	BUTTON_GPIOPORT			USER_BUTTON_GPIO_Port
#define	BUTTON_GPIOBIT			USER_BUTTON_Pin

#define TICK_HZ 				1000U
#define HSI_CLOCK         		250000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK

#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		12
#define	ASSIGNED				1

extern	UART_HandleTypeDef 	huart3;
#define	CONSOLE				huart3

#define	BOARD_NAME			"STM32H563ZI-NUCLEO"

#endif


#ifdef	STM32U575xx

/* Memories */
#define	POOL_START			     0x20080000
/* Note : SRAM_START must be equal to osSegment in ld file */
#define SRAM_START               0x20090000
#define SRAM_SIZE                131072

/* I/O */
#define	DEBUG_GPIOPORT			PG6_Debug_GPIO_Port
#define	DEBUG_GPIOBIT			PG6_Debug_Pin
#define	LED_1_GPIOPORT			LED_RED_GPIO_Port
#define	LED_1_GPIOBIT			LED_RED_Pin
#define	LED_2_GPIOPORT			LED_GREEN_GPIO_Port
#define	LED_2_GPIOBIT			LED_GREEN_Pin
#define	LED_3_GPIOPORT			LED_BLUE_GPIO_Port
#define	LED_3_GPIOBIT			LED_BLUE_Pin
#define	BUTTON_GPIOPORT			USER_BUTTON_GPIO_Port
#define	BUTTON_GPIOBIT			USER_BUTTON_Pin

/* Clock */
#define TICK_HZ 				1000U
#define HSI_CLOCK         		250000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK

/* Others */
#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		12
#define	ASSIGNED				1

extern	UART_HandleTypeDef 	huart1;
#define	CONSOLE				huart1

#define	BOARD_NAME			"STM32U575-NUCLEO"

#endif

#ifdef	STM32F746xx
/* Memories */
#define	POOL_START			     0x20040000
/* Note : SRAM_START must be equal to osSegment in ld file */
#define SRAM_START               0x20048000
#define SRAM_SIZE                32768

/* I/O */
#define	DEBUG_GPIOPORT			PG6_Debug_GPIO_Port
#define	DEBUG_GPIOBIT			PG6_Debug_Pin

#define TICK_HZ 				1000U
#define HSI_CLOCK         		216000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK

#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		12
#define	ASSIGNED				1

extern	UART_HandleTypeDef 	huart1;
#define	CONSOLE				huart1

#define	QSPI_ENABLED			1

#define	BOARD_NAME			"STM32F746G-DISCO"
// Uncomment this out if you want a fixed ip
#undef LWIP_DHCP

#endif

#ifdef	STM32F446xx
/* Memories */
#define	POOL_START			     0x20004000
/* Note : SRAM_START must be equal to osSegment in ld file */
#define SRAM_START               0x20008000
#define SRAM_SIZE                32768

/* I/O */
#define	DEBUG_GPIOPORT			PG6_Debug_GPIO_Port
#define	DEBUG_GPIOBIT			PG6_Debug_Pin

#define TICK_HZ 				1000U
#define HSI_CLOCK         		180000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK

#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		12
#define	ASSIGNED				1

extern	UART_HandleTypeDef 	huart2;
#define	CONSOLE				huart2

#define	BOARD_NAME			"STM32F446RE-NUCLEO"
// Uncomment this out if you want a fixed ip
#undef LWIP_DHCP

#endif

#ifdef	STM32L152xE
/* Memories */
#define	POOL_START			     0x20004000
/* Note : SRAM_START must be equal to osSegment in ld file */
#define SRAM_START               0x20008000
#define SRAM_SIZE                32768

/* I/O */
#define	DEBUG_GPIOPORT			PG6_Debug_GPIO_Port
#define	DEBUG_GPIOBIT			PG6_Debug_Pin
#define	LED_GPIOPORT			LD2_LED_GPIO_Port
#define	LED_GPIOBIT				LD2_LED_Pin

#define TICK_HZ 				1000U
#define HSI_CLOCK         		180000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK

#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		12
#define	ASSIGNED				1

extern	UART_HandleTypeDef 	huart2;
#define	CONSOLE				huart2

#define	WIRELESS_ENABLED				1

#ifdef WIRELESS_ENABLED
	extern	SPI_HandleTypeDef 					hspi2;
	#define NRF24L01_SPI						hspi2
	#define NRF24L01_SPI_TIMEOUT				2000

	#define NRF24L01_SPI_CS_PIN_PORT			SPI2_SS_GPIO_Port
	#define NRF24L01_SPI_CS_PIN_NUMBER			SPI2_SS_Pin

	#define NRF24L01_CE_PIN_PORT				NRF24_CE_GPIO_Port
	#define NRF24L01_CE_PIN_NUMBER				NRF24_CE_Pin

	#define NRF24L01_IRQ_PIN_PORT				EXTI4_NRFIRQ_GPIO_Port
	#define NRF24L01_IRQ_PIN_NUMBER				EXTI4_NRFIRQ_Pin
#endif // #ifdef WIRELESS_ENABLED
#define	BOARD_NAME			"STM32L152RE-NUCLEO"
// Uncomment this out if you want a fixed ip
#undef LWIP_DHCP

#endif

#ifndef ASSIGNED
#error "Processor not implemented"
#endif

/* common for all processors */
#define	POOL_CHUNK_SIZE		    256
#define	POOL_NUM			    32
#define	POOL_SIZE			    (POOL_CHUNK_SIZE*POOL_NUM)
#define	POOL_END			    (POOL_START+POOL_SIZE)
#define SRAM_END                 ((SRAM_START) + (SRAM_SIZE) )
#define SIZE_SCHED_STACK         4096U
#define SCHED_STACK_START        SRAM_END
#define IDLE_STACK_START         (SCHED_STACK_START - SIZE_SCHED_STACK)
#define SIZE_IDLE_STACK          4096U
#define	FIRST_PRC_STACK_START	 (IDLE_STACK_START - SIZE_IDLE_STACK)
#define SYSTEM_RAM				__attribute__((section(".osSegment"))) __attribute__ ((aligned (32)))
#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		12
#define	Exti0_PRIORITY			13
#define	Exti1_PRIORITY			13
#define	Exti2_PRIORITY			13


#endif /* KERNEL_SYSTEM_DEFAULT_H_ */

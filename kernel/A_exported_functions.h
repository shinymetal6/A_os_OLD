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
 * A_exported_functions.h
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */

#ifndef KERNEL_A_EXPORTED_FUNCTIONS_H_
#define KERNEL_A_EXPORTED_FUNCTIONS_H_

/* System */
extern	/* uint32_t */ void wait_event(uint32_t events);

extern	uint8_t get_current_process(void);
extern	uint32_t get_activation_flags(void);
extern	uint32_t get_wakeup_rsn(void);
extern	uint32_t get_wakeup_flags(uint32_t *reason, uint32_t *flags );

extern	void A_TimeDebug_High(void);
extern	void A_TimeDebug_Low(void);

/* Memory */
extern	uint8_t *mem_get(uint32_t size );
extern	uint32_t mem_release(uint8_t *data_ptr);

/* timer */
extern	void task_delay(uint32_t tick_count);
extern	uint32_t create_timer(uint8_t timer_id,uint32_t tick_count,uint8_t flags);
extern	uint32_t start_timer(uint8_t timer_id);
extern	uint32_t restart_timer(uint8_t timer_id,uint32_t tick_count,uint8_t flags);
extern	uint32_t stop_timer(uint8_t timer_id);
extern	uint32_t destroy_timer(uint8_t timer_id);
extern	uint8_t get_timer_expired(void);
extern	int32_t A_GetTick(void);

/* qspi */
#if defined QSPI_ENABLED
extern	QSPI_HandleTypeDef *A_qspi_init(void);
extern	uint8_t A_qspi_erase_block(uint32_t BlockAddress);
extern	uint8_t A_qspi_erase_chip(void);
extern	uint8_t A_qspi_write(uint8_t* data,uint32_t addr, uint32_t size);
extern	uint8_t A_qspi_read( uint8_t* data,uint32_t addr, uint32_t size);
extern	uint8_t A_qspi_enable_write(void);
extern	uint8_t A_qspi_disable_write(void);

#endif

/* compatibility */
extern	void 	A_Error_Handler(char * file, int line);

/* timer id */
#define	TIMER_ID_0		0x01
#define	TIMER_ID_1		0x02
#define	TIMER_ID_2		0x04
#define	TIMER_ID_3		0x08
#define	TIMER_ID_4		0x10
#define	TIMER_ID_5		0x20
#define	TIMER_ID_6		0x40
#define	TIMER_ID_7		0x80

/* timer flags */
#define	TIMERFLAGS_ONESHOT			0x00
#define	TIMERFLAGS_FOREVER			0x01
#define	TIMERFLAGS_ENABLED			0x02
#define	TIMERFLAGS_IN_USE			0x80
#define	TIMERFLAGS_DISABLED			0x00
#define	TIMERFLAGS_USERMASK			0x0f

/* hwmanager */
extern	uint32_t allocate_hw(uint32_t peripheral,uint8_t config);
extern	uint32_t deallocate_hw(uint32_t peripheral);

/* hwmanager : usb */
extern	uint32_t hw_set_usb_rx_buffer(uint8_t *rx_buf);
extern	uint32_t hw_send_usb(uint8_t* ptr, uint16_t len);

/* hwmanager : uart */
extern	uint32_t hw_send_uart(uint32_t uart,uint8_t *ptr,uint16_t len);
extern	uint32_t hw_receive_uart(uint32_t uart,uint8_t *rx_buf,uint16_t rx_buf_max_len,uint16_t timeout);
extern	uint32_t hw_receive_uart_sentinel(uint32_t uart,uint8_t *rx_buf,uint16_t rx_buf_max_len,uint8_t sentinel_start, uint8_t sentinel_end,uint16_t timeout);
extern	uint32_t hw_receive_uart_sentinel_clear(uint32_t uart);

extern	void HAL_UART_RxTimeoutCheckCallback(void);
extern	uint16_t hw_get_uart_receive_len(uint32_t uart);

/* support functions */
extern	uint32_t A_bit_index_to_num(uint32_t bit_index );



/* audio */
extern	uint32_t *InitAudioBuffers(void);
extern	uint8_t StartAudioBuffers(int16_t* audio_in_buffer,int16_t* audio_out_buffer);

/* xmodem */
extern	uint8_t xmodem_receive(uint8_t *buf , uint16_t len);


/* svc ops */
extern	int32_t call_svc(int8_t svc_index,int32_t param1 , int32_t param2 , int32_t param3);


/* peripherals , maximum index is 27 , bit 28 to 31 are for anomalies on the semaphores ( actually used 3 ) */
#define	HW_DELAY					0
#define	HW_TIMER					1
#define	HW_MBX						2
#define	HW_UART1					5
#define	HW_UART2					6
#define	HW_UART3					7
#define	HW_UART4					8
#define	HW_I2C1						9
#define	HW_I2C2						10
#define	HW_SPI1						11
#define	HW_SPI2						12
#define	HW_QSPI						13
#define	HW_I2S1						14
#define	HW_I2S2						15
#define	HW_TIM						16
#define	HW_EXT_INT					17
#define	HW_ADC1						18
#define	HW_ADC2						18
#define	HW_DAC						20
#define	HW_NRF24L01					28
#define	HW_USB_DEVICE				29
#define	HW_USB_HOST					30

/* event to wait */
#define	EVENT_DELAY						(1<<HW_DELAY)
#define	EVENT_TIMER						(1<<HW_TIMER)
#define	EVENT_MBX						(1<<HW_MBX)
#define	EVENT_UART1_IRQ					(1<<HW_UART1)
#define	EVENT_UART2_IRQ					(1<<HW_UART2)
#define	EVENT_UART3_IRQ					(1<<HW_UART3)
#define	EVENT_UART4_IRQ					(1<<HW_UART4)
#define	EVENT_I2C1_IRQ					(1<<HW_I2C1)
#define	EVENT_I2C2_IRQ					(1<<HW_I2C2)
#define	EVENT_SPI1_IRQ					(1<<HW_SPI1)
#define	EVENT_SPI2_IRQ					(1<<HW_SPI2)
#define	EVENT_QSPI_IRQ					(1<<HW_QSPI)
#define	EVENT_I2S1_IRQ					(1<<HW_I2S1)
#define	EVENT_I2S2_IRQ					(1<<HW_I2S2)
#define	EVENT_TIM_IRQ					(1<<HW_TIM)
#define	EVENT_EXT_INT_IRQ				(1<<HW_EXT_INT)
#define	EVENT_ADC1_IRQ					(1<<HW_ADC1)
#define	EVENT_ADC2_IRQ					(1<<HW_ADC2)
#define	EVENT_DAC_IRQ					(1<<HW_DAC)
#define	EVENT_NRF24L01_IRQ				(1<<HW_NRF24L01)
#define	EVENT_USB_DEVICE_IRQ			(1<<HW_USB_DEVICE)
#define	EVENT_USB_IRQ					(1<<HW_USB_HOST)
/* suspend_mode */
#define	SUSPEND_ON_DELAY				EVENT_DELAY
#define	SUSPEND_ON_TIMER				EVENT_TIMER
#define	SUSPEND_ON_MBX					EVENT_MBX
#define	SUSPEND_ON_UART1_IRQ			EVENT_UART1_IRQ
#define	SUSPEND_ON_UART2_IRQ			EVENT_UART2_IRQ
#define	SUSPEND_ON_UART3_IRQ			EVENT_UART3_IRQ
#define	SUSPEND_ON_UART4_IRQ			EVENT_UART4_IRQ
#define	SUSPEND_ON_I2C1_IRQ				EVENT_I2C1_IRQ
#define	SUSPEND_ON_I2C2_IRQ				EVENT_I2C2_IRQ
#define	SUSPEND_ON_SPI1_IRQ				EVENT_SPI1_IRQ
#define	SUSPEND_ON_SPI2_IRQ				EVENT_SPI2_IRQ
#define	SUSPEND_ON_QSPI_IRQ				EVENT_QSPI_IRQ
#define	SUSPEND_ON_I2S1_IRQ				EVENT_I2S1_IRQ
#define	SUSPEND_ON_I2S2_IRQ				EVENT_I2S2_IRQ
#define	SUSPEND_ON_TIM_IRQ				EVENT_TIM_IRQ
#define	SUSPEND_ON_EXT_INT_IRQ			EVENT_EXT_INT_IRQ
#define	SUSPEND_ON_ADC1_IRQ				EVENT_ADC1_IRQ
#define	SUSPEND_ON_ADC2_IRQ				EVENT_ADC2_IRQ
#define	SUSPEND_ON_DAC_IRQ				EVENT_DAC_IRQ
#define	SUSPEND_ON_NRF24L01_IRQ			EVENT_NRF24L01_IRQ
#define	SUSPEND_ON_USB_DEVICE_IRQ		EVENT_USB_DEVICE_IRQ
#define	SUSPEND_ON_USB_HOST_IRQ			EVENT_USB_HOST_IRQ
/* wakeup_flags */
#define	WAKEUP_FROM_DELAY				SUSPEND_ON_DELAY
#define	WAKEUP_FROM_TIMER				SUSPEND_ON_TIMER
#define	WAKEUP_FROM_MBX					SUSPEND_ON_MBX
#define	WAKEUP_FROM_UART1_IRQ			SUSPEND_ON_UART1_IRQ
#define	WAKEUP_FROM_UART2_IRQ			SUSPEND_ON_UART2_IRQ
#define	WAKEUP_FROM_UART3_IRQ			SUSPEND_ON_UART3_IRQ
#define	WAKEUP_FROM_UART4_IRQ			SUSPEND_ON_UART4_IRQ
#define	WAKEUP_FROM_I2C1_IRQ			SUSPEND_ON_I2C1_IRQ
#define	WAKEUP_FROM_I2C2_IRQ			SUSPEND_ON_I2C2_IRQ
#define	WAKEUP_FROM_SPI1_IRQ			SUSPEND_ON_SPI1_IRQ
#define	WAKEUP_FROM_SPI2_IRQ			SUSPEND_ON_SPI2_IRQ
#define	WAKEUP_FROM_QSPI_IRQ			SUSPEND_ON_QSPI_IRQ
#define	WAKEUP_FROM_I2S1_IRQ			SUSPEND_ON_I2S1_IRQ
#define	WAKEUP_FROM_I2S2_IRQ			SUSPEND_ON_I2S2_IRQ
#define	WAKEUP_FROM_TIM_IRQ				SUSPEND_ON_TIM_IRQ
#define	WAKEUP_FROM_EXT_INT_IRQ			SUSPEND_ON_EXT_INT_IRQ
#define	WAKEUP_FROM_ADC1_IRQ			SUSPEND_ON_ADC1_IRQ
#define	WAKEUP_FROM_ADC2_IRQ			SUSPEND_ON_ADC2_IRQ
#define	WAKEUP_FROM_DAC_IRQ				SUSPEND_ON_DAC_IRQ
#define	WAKEUP_FROM_NRF24L01_IRQ		SUSPEND_ON_NRF24L01_IRQ
#define	WAKEUP_FROM_USB_DEVICE_IRQ		SUSPEND_ON_USB_DEVICE_IRQ
#define	WAKEUP_FROM_USB_HOST_IRQ		SUSPEND_ON_USB_HOST_IRQ
/* device_flags */
#define	DEVICE_DELAY					HW_DELAY
#define	DEVICE_TIMER					HW_TIMER
#define	DEVICE_MBX						HW_MBX
#define	DEVICE_UART1					HW_UART1
#define	DEVICE_UART2					HW_UART2
#define	DEVICE_UART3					HW_UART3
#define	DEVICE_UART4					HW_UART4
#define	DEVICE_I2C1						HW_I2C1
#define	DEVICE_I2C2						HW_I2C2
#define	DEVICE_SPI1						HW_SPI1
#define	DEVICE_SPI2						HW_SPI2
#define	DEVICE_QSPI						HW_QSPI
#define	DEVICE_I2S1						HW_I2S1
#define	DEVICE_I2S2						HW_I2S2
#define	DEVICE_TIM						HW_TIM
#define	DEVICE_EXT_INT					HW_EXT_INT
#define	DEVICE_ADC1						HW_ADC1
#define	DEVICE_ADC2						HW_ADC2
#define	DEVICE_DAC						HW_DAC
#define	DEVICE_NRF24L01					HW_NRF24L01
#define	DEVICE_USB_DEVICE				HW_USB_DEVICE
#define	DEVICE_USB_HOST					HW_USB_HOST

/* wakeup_flags */
#define	WAKEUP_FLAGS_TIM_ID0				0x00000001
#define	WAKEUP_FLAGS_TIM_ID1				0x00000002
#define	WAKEUP_FLAGS_TIM_ID2				0x00000004
#define	WAKEUP_FLAGS_TIM_ID3				0x00000008
#define	WAKEUP_FLAGS_TIM_ID4				0x00000010
#define	WAKEUP_FLAGS_TIM_ID5				0x00000020
#define	WAKEUP_FLAGS_TIM_ID6				0x00000040
#define	WAKEUP_FLAGS_TIM_ID7				0x00000080
#define	WAKEUP_FLAGS_UART					0x00000100
#define	WAKEUP_FLAGS_UART_TO				0x00000200
#define	WAKEUP_FLAGS_EXTI_D0				0x00000400
#define	WAKEUP_FLAGS_EXTI_D1				0x00000800
#define	WAKEUP_FLAGS_EXTI					0x00001000
#define	WAKEUP_FLAGS_TEST					0x10000000
#define	WAKEUP_FLAGS_MBX					0x40000000
#define	WAKEUP_FLAGS_HW_USB_RX_COMPLETE		0x80000000

#ifdef WIRELESS_NRF24L01
#include "../drivers/wireless/nrf24l01/nrf24l01.h"
#endif
#ifdef CODEC_ENABLED
	#include "audio.h"
	#ifdef CODEC_NAU88C22
		#include "../drivers/codec/nau88c22.h"
	#endif
#endif // #ifdef CODEC_ENABLED
#ifdef LCD_096_ENABLED
	#include "../drivers/lcd_st7735/st7735.h"
	#include "../drivers/lcd_st7735/lcd_7735.h"
	extern	void Draw_Logo(uint16_t *logo);
#endif // #ifdef LCD_096_ENABLED
#ifdef ADC_ENABLED
	#include "../drivers/internal_adc/internal_adc.h"
	extern	uint8_t IntAdc_Start(void);

#endif // #ifdef ADC_ENABLED

#endif /* KERNEL_A_EXPORTED_FUNCTIONS_H_ */

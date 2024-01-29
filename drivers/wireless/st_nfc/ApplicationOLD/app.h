/*
 * app.h
 *
 *  Created on: Aug 4, 2023
 *      Author: fil
 */

#ifndef APPLICATION_APP_H_
#define APPLICATION_APP_H_

#include "../Platform/Platform_STM32H7xx.h"
#include "ftm.h"
#include "logger.h"
#include "rfal_platform.h"
#include "../RFAL/rfal_nfcv.h"

extern	SPI_HandleTypeDef 		hspi1;
#define	SPI_3911				hspi1
extern	TIM_HandleTypeDef 		htim6;
#define	TIM_TICK_10MSEC			htim6
extern	UART_HandleTypeDef 		huart3;
#define	LOG_UART				huart3

#define	MBX_SIZE				256
#define	RX_MBX_SIZE				280
#define	UART_TX_SIZE			4096

typedef struct  _SystemTypeDef
{
	uint8_t			system_flags;
	uint8_t			tick_flags;
	uint8_t			tick_cntr;
	uint8_t			poller_state;
	uint8_t			mbx_rxbuf[RX_MBX_SIZE];
	uint16_t		mbx_rxbuf_len;
	uint8_t			rx_msg_count;
	uint8_t			mbx_txbuf[MBX_SIZE];
	uint16_t		mbx_txbuf_len;
	uint8_t			uart_txbuf[UART_TX_SIZE];
	uint16_t		uart_txlen;
	uint32_t		reached_len;
	uint8_t			uart_rxchar;
	uint8_t			uart_rxbuf[MBX_SIZE];
	uint16_t		uart_rxindex;
}SystemTypeDef;

extern	SystemTypeDef System;
/* system_flags */
#define	SYSTEM_NFCSTARTED	0x80

/* tick_flags */
#define	TICK_10MSEC			0x01

/* poller state machine */
#define NOTINIT               	0     /*!< State:  Not initialized        */
#define START_DISCOVERY       	1     /*!< State:  Start Discovery        */
#define DISCOVERY             	2     /*!< State:  Discovery              */
#define NFC_FOUND             	3     /*!< State:  Device Found           */
#define USE_SELECT_MODE     	true  /*!< NFCV select mode   			  */
#define TICK_TIMEOUT			2000

extern	void app_init();
extern	void app_loop();
extern	void st25r3916Isr( void );

#endif /* APPLICATION_APP_H_ */

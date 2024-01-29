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
 * Project : fy-201023-00 
*/
/*
 * nrf24l01.c
 *
 *  Created on: Dec 28, 2023
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/system_default.h"

#ifdef	ISM_ENABLED
#include "nrf24l01.h"
extern	void task_delay(uint32_t tick_count);

void nrf24l01_cs_high(void)
{
	HAL_GPIO_WritePin(NRF24L01_SS_PIN_PORT, NRF24L01_SS_PIN_NUMBER, GPIO_PIN_SET);
}

void nrf24l01_cs_low(void)
{
	HAL_GPIO_WritePin(NRF24L01_SS_PIN_PORT, NRF24L01_SS_PIN_NUMBER, GPIO_PIN_RESET);
}

void nrf24l01_ce_high(void)
{
	HAL_GPIO_WritePin(NRF24L01_CE_PIN_PORT, NRF24L01_CE_PIN_NUMBER, GPIO_PIN_SET);
}

void nrf24l01_ce_low(void)
{
	HAL_GPIO_WritePin(NRF24L01_CE_PIN_PORT, NRF24L01_CE_PIN_NUMBER, GPIO_PIN_RESET);
}

uint8_t nrf24l01_read_register(uint8_t reg)
{
uint8_t command = NRF24L01_CMD_R_REGISTER | reg;
uint8_t status;
uint8_t read_val;

	nrf24l01_cs_low();
	HAL_SPI_TransmitReceive(&NRF24L01_SPI, &command, &status, 1, NRF24L01_SPI_TIMEOUT);
	HAL_SPI_Receive(&NRF24L01_SPI, &read_val, 1, NRF24L01_SPI_TIMEOUT);
	nrf24l01_cs_high();
	return read_val;
}

uint8_t nrf24l01_write_register(uint8_t reg, uint8_t value)
{
uint8_t command = NRF24L01_CMD_W_REGISTER | reg;
uint8_t status;
uint8_t write_val = value;

	nrf24l01_cs_low();
	HAL_SPI_TransmitReceive(&NRF24L01_SPI, &command, &status, 1, NRF24L01_SPI_TIMEOUT);
	HAL_SPI_Transmit(&NRF24L01_SPI, &write_val, 1, NRF24L01_SPI_TIMEOUT);
	nrf24l01_cs_high();
	return write_val;
}

uint8_t nrf24l01_read_multiple_register(uint8_t reg, uint8_t *values,uint8_t reg_num)
{
uint8_t command = NRF24L01_CMD_R_REGISTER | reg;
uint8_t status;

	nrf24l01_cs_low();
	HAL_SPI_TransmitReceive(&NRF24L01_SPI, &command, &status, 1, NRF24L01_SPI_TIMEOUT);
	HAL_SPI_Receive(&NRF24L01_SPI,values, reg_num, NRF24L01_SPI_TIMEOUT);
	nrf24l01_cs_high();
	return status;
}

uint8_t nrf24l01_write_multiple_register(uint8_t reg, uint8_t *values,uint8_t reg_num)
{
uint8_t command = NRF24L01_CMD_W_REGISTER | reg;
uint8_t status;

	nrf24l01_cs_low();
	HAL_SPI_TransmitReceive(&NRF24L01_SPI, &command, &status, 1, NRF24L01_SPI_TIMEOUT);
	HAL_SPI_Transmit(&NRF24L01_SPI, values, reg_num, NRF24L01_SPI_TIMEOUT);
	nrf24l01_cs_high();
	return 0;
}

void nrf24l01_flush_rx_fifo()
{
uint8_t command = NRF24L01_CMD_FLUSH_RX;
uint8_t status;

	nrf24l01_cs_low();
	HAL_SPI_TransmitReceive(&NRF24L01_SPI, &command, &status, 1, NRF24L01_SPI_TIMEOUT);
	nrf24l01_cs_high();
}

void nrf24l01_flush_tx_fifo()
{
uint8_t command = NRF24L01_CMD_FLUSH_TX;
uint8_t status;

	nrf24l01_cs_low();
	HAL_SPI_TransmitReceive(&NRF24L01_SPI, &command, &status, 1, NRF24L01_SPI_TIMEOUT);
	nrf24l01_cs_high();
}

uint8_t nrf24l01_read_rx_fifo(uint8_t* rx_payload)
{
uint8_t command = NRF24L01_CMD_R_RX_PAYLOAD;
uint8_t status;

	nrf24l01_cs_low();
	HAL_SPI_TransmitReceive(&NRF24L01_SPI, &command, &status, 1, NRF24L01_SPI_TIMEOUT);
	HAL_SPI_Receive(&NRF24L01_SPI, rx_payload, NRF24L01_PAYLOAD_LENGTH, NRF24L01_SPI_TIMEOUT);
	nrf24l01_cs_high();
	return status;
}

uint8_t nrf24l01_write_tx_fifo(uint8_t* tx_payload)
{
uint8_t command = NRF24L01_CMD_W_TX_PAYLOAD;
uint8_t status;

	nrf24l01_flush_tx_fifo();
	nrf24l01_cs_low();
	HAL_SPI_TransmitReceive(&NRF24L01_SPI, &command, &status, 1, NRF24L01_SPI_TIMEOUT);
	HAL_SPI_Transmit(&NRF24L01_SPI, tx_payload, NRF24L01_PAYLOAD_LENGTH, NRF24L01_SPI_TIMEOUT);
	nrf24l01_cs_high();
	return status;
}

uint8_t nrf24l01_rx(uint8_t* rx_payload )
{
uint8_t nrf24l01_status;
	nrf24l01_read_rx_fifo(rx_payload);
	nrf24l01_status = nrf24l01_read_register(NRF24L01_REG_STATUS);
	nrf24l01_write_register(NRF24L01_REG_STATUS, nrf24l01_status |= 0x40);
	return nrf24l01_status;
}

uint8_t nrf24l01_set_rx_address(uint8_t* rx_address )
{
uint8_t nrf24l01_status;
	nrf24l01_write_multiple_register(NRF24L01_REG_RX_ADDR_P0,rx_address,5);
	nrf24l01_status = nrf24l01_read_register(NRF24L01_REG_STATUS);
	nrf24l01_write_register(NRF24L01_REG_STATUS, nrf24l01_status |= 0x40);
	return nrf24l01_status;
}

uint8_t nrf24l01_get_tx_irq_goto_rx(void)
{
uint8_t nrf24l01_status;
	nrf24l01_status = nrf24l01_read_register(NRF24L01_REG_STATUS);
	nrf24l01_write_register(NRF24L01_REG_STATUS, 0x70);
	nrf24l01_ce_low();
	nrf24l01_write_register(NRF24L01_REG_CONFIG, 0x00);						// power down
	nrf24l01_write_register(NRF24L01_REG_CONFIG, 0x3b);						// go to rx : pup, crc en 1 bytes,rx, txdr & maxrt irq disabled
	nrf24l01_ce_high();
	return nrf24l01_status;
}

uint8_t nrf24l01_get_status(void)
{
	return nrf24l01_read_register(NRF24L01_REG_STATUS);
}

uint8_t nrf24l01_tx(uint8_t* tx_payload , uint8_t* tx_address)
{
uint8_t nrf24l01_status;
	nrf24l01_ce_low();
	nrf24l01_write_register(NRF24L01_REG_CONFIG, 0x00);						// power down
	nrf24l01_write_register(NRF24L01_REG_CONFIG, 0x4a);						// go to tx : pup, crc en 1 bytes,tx, rx dr irq disabled
	task_delay(1);															// need to wait at least 130uSec for radio switch
	nrf24l01_write_register(NRF24L01_REG_STATUS, 0x70);						// clear irqs
	nrf24l01_write_multiple_register(NRF24L01_REG_TX_ADDR,tx_address,5);
	nrf24l01_write_multiple_register(NRF24L01_REG_RX_ADDR_P0,tx_address,5);
	nrf24l01_status = nrf24l01_read_register(NRF24L01_REG_STATUS);
	nrf24l01_write_register(NRF24L01_REG_STATUS, nrf24l01_status & 0x40);
	nrf24l01_write_tx_fifo(tx_payload);
	nrf24l01_ce_high();
	return nrf24l01_read_register(NRF24L01_REG_STATUS);
}

uint8_t nrf24l01_init(uint16_t MHz, uint8_t bps , uint8_t mode , uint8_t* nrf_address)
{
uint8_t nrf24l01_status;
	nrf24l01_ce_low();
	nrf24l01_write_register(NRF24L01_REG_CONFIG, 0x08);
	nrf24l01_write_register(NRF24L01_REG_EN_AA, 0x3f);
	nrf24l01_write_register(NRF24L01_REG_EN_RXADDR, 0x3f);
	nrf24l01_write_register(NRF24L01_REG_SETUP_AW, 0x03);
	nrf24l01_write_register(NRF24L01_REG_SETUP_RETR, 0xff);	// 4000 uS , 15 retransmit
	nrf24l01_write_register(NRF24L01_REG_RF_CH, MHz - 2400);
	nrf24l01_write_register(NRF24L01_REG_RF_SETUP, 0x07 | ((bps << 3) & 0x08 ));
	nrf24l01_write_register(NRF24L01_REG_DYNPD, 0x00);
	nrf24l01_write_register(NRF24L01_REG_FEATURE, 0x00);

	nrf24l01_write_multiple_register(NRF24L01_REG_TX_ADDR,nrf_address,5);
	nrf24l01_write_multiple_register(NRF24L01_REG_RX_ADDR_P0,nrf_address,5);
	nrf24l01_write_multiple_register(NRF24L01_REG_RX_ADDR_P5,nrf_address,5);

	/* */
	nrf24l01_write_register(NRF24L01_REG_RX_PW_P0, 32);
	nrf24l01_write_register(NRF24L01_REG_RX_PW_P1, 32);
	nrf24l01_write_register(NRF24L01_REG_RX_PW_P2, 32);
	nrf24l01_write_register(NRF24L01_REG_RX_PW_P3, 32);
	nrf24l01_write_register(NRF24L01_REG_RX_PW_P4, 32);
	nrf24l01_write_register(NRF24L01_REG_RX_PW_P5, 32);

	nrf24l01_write_multiple_register(NRF24L01_REG_TX_ADDR,nrf_address,5);
	nrf24l01_write_multiple_register(NRF24L01_REG_RX_ADDR_P0,nrf_address,5);
	nrf24l01_write_multiple_register(NRF24L01_REG_RX_ADDR_P5,nrf_address,5);
	/* */

	nrf24l01_flush_rx_fifo();
	nrf24l01_flush_tx_fifo();

	if ( mode == NRF24L01_MODE_TX )
	{
		nrf24l01_write_register(NRF24L01_REG_CONFIG, 0x4a);						// pup, crc en 1 bytes,tx, rx dr irq disabled
	}
	if ( mode == NRF24L01_MODE_RX )
	{
		nrf24l01_write_register(NRF24L01_REG_CONFIG, 0x3b);						// pup, crc en 1 bytes,rx, txdr & maxrt irq disabled
	}
	nrf24l01_write_register(NRF24L01_REG_STATUS, 0x70);


	nrf24l01_status = nrf24l01_read_register(NRF24L01_REG_STATUS);

	if ( mode == NRF24L01_MODE_RX )
		nrf24l01_ce_high();
	return nrf24l01_status;
}

#endif	//#ifdef	ISM_ENABLED

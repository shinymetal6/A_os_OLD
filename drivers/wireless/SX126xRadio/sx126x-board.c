/*
  ______                              _
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: SX126x driver specific target board functions implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#include "main.h"
#include "../../../kernel/system_default.h"

#ifdef	LORA_ENABLED

#include "radio.h"
#include "sx126x.h"
#include "sx126x-board.h"

uint8_t pTxData[256];
uint8_t pRxData[256];

void SX126xReset( void )
{
    task_delay( 10 );
    HAL_GPIO_WritePin( SX126X_RESET_GPIO_Port, SX126X_RESET_Pin,GPIO_PIN_RESET);
    task_delay( 20 );
    HAL_GPIO_WritePin( SX126X_RESET_GPIO_Port, SX126X_RESET_Pin,GPIO_PIN_SET);
    task_delay( 10 );
}

void SX126xWaitOnBusy( void )
{
   while(HAL_GPIO_ReadPin(SX126X_BUSY_PIN_PORT,SX126X_BUSY_PIN_NUMBER)==GPIO_PIN_SET);
}

void SX126x_select(void)
{
	HAL_GPIO_WritePin( SX126X_SS_PIN_PORT, SX126X_SS_PIN_NUMBER,GPIO_PIN_RESET);
	HAL_GPIO_WritePin( NRF24L01_SS_PIN_PORT, NRF24L01_SS_PIN_NUMBER, GPIO_PIN_RESET);
}

void SX126x_deselect(void)
{
    HAL_GPIO_WritePin( SX126X_SS_PIN_PORT, SX126X_SS_PIN_NUMBER,GPIO_PIN_SET);
    HAL_GPIO_WritePin( NRF24L01_SS_PIN_PORT, NRF24L01_SS_PIN_NUMBER, GPIO_PIN_SET);
}

void SX126xWakeup( void )
{
	pTxData[0]=RADIO_GET_STATUS;
	pTxData[1]=0x00;
	pTxData[2]=0x00;
	SX126x_select();
    HAL_SPI_TransmitReceive(&SX126X_SPI, pTxData, pRxData,3, SX126X_SPI_TIMEOUT);
    SX126x_deselect();
    // Wait for chip to be ready.
    SX126xWaitOnBusy( );
}

void SX126xWriteCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{
uint8_t	i;
    SX126xCheckDeviceReady( );

    pTxData[0] = command;
    for(i=0;i<size;i++)
    	pTxData[i+1] = buffer[i];
    SX126x_select();
    HAL_SPI_TransmitReceive(&SX126X_SPI, pTxData, buffer,size, SX126X_SPI_TIMEOUT);
    SX126x_deselect();
    
    if( command != RADIO_SET_SLEEP )
    {
        SX126xWaitOnBusy( );
    }
}

void SX126xReadCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{

    SX126xCheckDeviceReady( );

    SX126x_select();
    pTxData[0] = command;
    pTxData[1] = 0;
    HAL_SPI_TransmitReceive(&SX126X_SPI, pTxData, buffer,size, SX126X_SPI_TIMEOUT);
    SX126x_deselect();

    SX126xWaitOnBusy( );
}

void SX126xWriteRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{
	
    SX126xCheckDeviceReady( );

    SX126x_select();
    pTxData[0] = RADIO_WRITE_REGISTER;
    pTxData[1] = ( address & 0xFF00 ) >> 8 ;
    pTxData[2] = address & 0x00FF;
    HAL_SPI_TransmitReceive(&SX126X_SPI, pTxData, buffer,size, SX126X_SPI_TIMEOUT);
    SX126x_deselect();

    SX126xWaitOnBusy( );
}

void SX126xWriteRegister( uint16_t address, uint8_t value )
{
    SX126xWriteRegisters( address, &value, 1 );
}

void SX126xReadRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{
uint8_t		i;

    SX126xCheckDeviceReady( );

    SX126x_select();
    pTxData[0] = RADIO_READ_REGISTER;
    pTxData[1] = ( address & 0xFF00 ) >> 8 ;
    pTxData[2] = address & 0x00FF;
    pTxData[3] = 0;
    HAL_SPI_TransmitReceive(&SX126X_SPI, pTxData, pRxData,size+4, SX126X_SPI_TIMEOUT);
    SX126x_deselect();
    for(i=0;i<size;i++)
    	buffer[i] = pRxData[i+3];

    SX126xWaitOnBusy( );
}

uint8_t SX126xReadRegister( uint16_t address )
{
    uint8_t data;
    SX126xReadRegisters( address, &data, 1 );
    return data;
}


uint8_t	crc_sxsx[2];
void SX126xGetCRCRegs( void )
{
	SX126xReadRegisters(REG_LR_CRCPOLYBASEADDR,crc_sxsx,2);
}

void SX126xWriteBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
    SX126xCheckDeviceReady( );

    SX126x_select();
    pTxData[0] = RADIO_WRITE_BUFFER;
    pTxData[1] = offset ;
    HAL_SPI_TransmitReceive(&SX126X_SPI, pTxData, buffer,size, SX126X_SPI_TIMEOUT);
    SX126x_deselect();

    SX126xWaitOnBusy( );
}

void SX126xReadBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
    SX126xCheckDeviceReady( );

    SX126x_select();
    pTxData[0] = RADIO_READ_BUFFER;
    pTxData[1] = offset ;
    HAL_SPI_TransmitReceive(&SX126X_SPI, pTxData, buffer,size, SX126X_SPI_TIMEOUT);
    SX126x_deselect();
    SX126x_select();
    
    SX126xWaitOnBusy( );
}

void SX126xSetRfTxPower( int8_t power )
{
    SX126xSetTxParams( power, RADIO_RAMP_40_US );
}

uint8_t SX126xGetPaSelect( uint32_t channel )
{
//    if( GpioRead( &DeviceSel ) == 1 )
//    {
//        return SX1261;
//    }
//    else
//    {
//        return SX1262;
//    }
  
  return SX1262;
}

void SX126xAntSwOn( void )
{
    //GpioInit( &AntPow, ANT_SWITCH_POWER, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1 );
}

void SX126xAntSwOff( void )
{
   // GpioInit( &AntPow, ANT_SWITCH_POWER, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
}

bool SX126xCheckRfFrequency( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supported
    return true;
}
#endif //#ifdef	LORA_ENABLED

/*!
 * \file      radio.c
 *
 * \brief     Radio driver API definition
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#include "main.h"
#include <math.h>
#include <string.h>
#include <stdbool.h>

#ifdef	LORA_ENABLED

#include "radio.h"
#include "sx126x.h"
#include "sx126x-board.h"

/*
void RadioInit( RadioEvents_t *events );
RadioState_t RadioGetStatus( void );
void RadioSetModem( RadioModems_t modem );
void RadioSetChannel( uint32_t freq );
bool RadioIsChannelFree( RadioModems_t modem, uint32_t freq, int16_t rssiThresh, uint32_t maxCarrierSenseTime );
uint32_t RadioRandom( void );
void RadioSetRxConfig( RadioModems_t modem, uint32_t bandwidth,
                          uint32_t datarate, uint8_t coderate,
                          uint32_t bandwidthAfc, uint16_t preambleLen,
                          uint16_t symbTimeout, bool fixLen,
                          uint8_t payloadLen,
                          bool crcOn, bool FreqHopOn, uint8_t HopPeriod,
                          bool iqInverted, bool rxContinuous );

void RadioSetTxConfig( RadioModems_t modem, int8_t power, uint32_t fdev,
                          uint32_t bandwidth, uint32_t datarate,
                          uint8_t coderate, uint16_t preambleLen,
                          bool fixLen, bool crcOn, bool FreqHopOn,
                          uint8_t HopPeriod, bool iqInverted, uint32_t timeout );

bool RadioCheckRfFrequency( uint32_t frequency );
uint32_t RadioTimeOnAir( RadioModems_t modem, uint8_t pktLen );
void RadioSend( uint8_t *buffer, uint8_t size );
void RadioSleep( void );
void RadioStandby( void );
void RadioRx( uint32_t timeout );
void RadioStartCad( void );
void RadioSetTxContinuousWave( uint32_t freq, int8_t power, uint16_t time );
void RadioWrite( uint16_t addr, uint8_t data );
uint8_t RadioRead( uint16_t addr );
void RadioWriteBuffer( uint16_t addr, uint8_t *buffer, uint8_t size );
void RadioReadBuffer( uint16_t addr, uint8_t *buffer, uint8_t size );
void RadioSetMaxPayloadLength( RadioModems_t modem, uint8_t max );
void RadioSetPublicNetwork( bool enable );
uint32_t RadioGetWakeupTime( void );
void RadioIrqProcess( void );
void RadioRxBoosted( uint32_t timeout );
void RadioSetRxDutyCycle( uint32_t rxTime, uint32_t sleepTime );
*/
/*
const struct Radio_s Radio =
{
    RadioInit,
    RadioGetStatus,
    RadioSetModem,
    RadioSetChannel,
    RadioIsChannelFree,
    RadioRandom,
    RadioSetRxConfig,
    RadioSetTxConfig,
    RadioCheckRfFrequency,
    RadioTimeOnAir,
    RadioSend,
    RadioSleep,
    RadioStandby,
    RadioRx,
    RadioStartCad,
    RadioSetTxContinuousWave,
    RadioRssi,
    RadioWrite,
    RadioRead,
    RadioWriteBuffer,
    RadioReadBuffer,
    RadioSetMaxPayloadLength,
    RadioSetPublicNetwork,
    RadioGetWakeupTime,
    RadioIrqProcess,
    // Available on SX126x only
    RadioRxBoosted,
    RadioSetRxDutyCycle
};
*/
/*
 * Local types definition
 */


 /*!
 * FSK bandwidth definition
 */
typedef struct
{
    uint32_t bandwidth;
    uint8_t  RegValue;
}FskBandwidth_t;

/*!
 * Precomputed FSK bandwidth registers values
 */
const FskBandwidth_t FskBandwidths[] =
{
    { 4800  , 0x1F },
    { 5800  , 0x17 },
    { 7300  , 0x0F },
    { 9700  , 0x1E },
    { 11700 , 0x16 },
    { 14600 , 0x0E },
    { 19500 , 0x1D },
    { 23400 , 0x15 },
    { 29300 , 0x0D },
    { 39000 , 0x1C },
    { 46900 , 0x14 },
    { 58600 , 0x0C },
    { 78200 , 0x1B },
    { 93800 , 0x13 },
    { 117300, 0x0B },
    { 156200, 0x1A },
    { 187200, 0x12 },
    { 234300, 0x0A },
    { 312000, 0x19 },
    { 373600, 0x11 },
    { 467000, 0x09 },
    { 500000, 0x00 }, // Invalid Bandwidth
};

const RadioLoRaBandwidths_t Bandwidths[] = { LORA_BW_125, LORA_BW_250, LORA_BW_500 };

//                                          SF12    SF11    SF10    SF9    SF8    SF7
static double RadioLoRaSymbTime[3][6] = {{ 32.768, 16.384, 8.192, 4.096, 2.048, 1.024 },  // 125 KHz
                                         { 16.384, 8.192,  4.096, 2.048, 1.024, 0.512 },  // 250 KHz
                                         { 8.192,  4.096,  2.048, 1.024, 0.512, 0.256 }}; // 500 KHz

uint8_t MaxPayloadLength = 0xFF;

uint32_t TxTimeout = 0;
uint32_t RxTimeout = 0;

bool RxContinuous = false;


PacketStatus_t RadioPktStatus;
uint8_t RadioRxPayload[255];

bool IrqFired = false;

/*
 * SX126x DIO IRQ callback functions prototype
 */

/*!
 * \brief DIO 0 IRQ callback
 */
void RadioOnDioIrq( void );

/*!
 * \brief Tx timeout timer callback
 */
void RadioOnTxTimeoutIrq( void );

/*!
 * \brief Rx timeout timer callback
 */
void RadioOnRxTimeoutIrq( void );

/*
 * Private global variables
 */


/*!
 * Holds the current network type for the radio
 */
typedef struct
{
    bool Previous;
    bool Current;
}RadioPublicNetwork_t;

static RadioPublicNetwork_t RadioPublicNetwork = { false };

/*!
 * Radio callbacks variable
 */
static RadioEvents_t* RadioEvents;

/*
 * Public global variables
 */

/*!
 * Radio hardware and global parameters
 */
SX126x_t SX126x;

/*!
 * Tx and Rx timers
 */
//TimerEvent_t TxTimeoutTimer;
//TimerEvent_t RxTimeoutTimer;

/*!
 * Returns the known FSK bandwidth registers value
 *
 * \param [IN] bandwidth Bandwidth value in Hz
 * \retval regValue Bandwidth register value.
 */
static uint8_t RadioGetFskBandwidthRegValue( uint32_t bandwidth )
{
    uint8_t i;

    if( bandwidth == 0 )
    {
        return( 0x1F );
    }

    for( i = 0; i < ( sizeof( FskBandwidths ) / sizeof( FskBandwidth_t ) ) - 1; i++ )
    {
        if( ( bandwidth >= FskBandwidths[i].bandwidth ) && ( bandwidth < FskBandwidths[i + 1].bandwidth ) )
        {
            return FskBandwidths[i+1].RegValue;
        }
    }
    // ERROR: Value not found
    while( 1 );
}

//void RadioInit( RadioEvents_t *events )
void RadioInit( void )
{
    //RadioEvents = events;
    
    SX126xInit(  );
    SX126xSetStandby( STDBY_RC );
    SX126xSetRegulatorMode( USE_DCDC );
    
    SX126xSetBufferBaseAddress( 0x00, 0x00 );
    SX126xSetTxParams( 0, RADIO_RAMP_200_US );
    SX126xSetDioIrqParams( IRQ_RADIO_ALL, IRQ_RADIO_ALL, IRQ_RADIO_NONE, IRQ_RADIO_NONE );
    
    //Initialize driver timeout timers
    //TimerInit( &TxTimeoutTimer, RadioOnTxTimeoutIrq );
    //TimerInit( &RxTimeoutTimer, RadioOnRxTimeoutIrq );
    
    IrqFired = false;
}

RadioState_t RadioGetStatus( void )
{
    switch( SX126xGetOperatingMode( ) )
    {
        case MODE_TX:
            return RF_TX_RUNNING;
        case MODE_RX:
            return RF_RX_RUNNING;
        case RF_CAD:
            return RF_CAD;
        default:
            return RF_IDLE;
    }
}

void RadioSetModem( RadioModems_t modem )
{
    switch( modem )
    {
    default:
    case MODEM_FSK:
        SX126xSetPacketType( PACKET_TYPE_GFSK );
        // When switching to GFSK mode the LoRa SyncWord register value is reset
        // Thus, we also reset the RadioPublicNetwork variable
        RadioPublicNetwork.Current = false;
        break;
    case MODEM_LORA:
        SX126xSetPacketType( PACKET_TYPE_LORA );
        // Public/Private network register is reset when switching modems
        if( RadioPublicNetwork.Current != RadioPublicNetwork.Previous )
        {
            RadioPublicNetwork.Current = RadioPublicNetwork.Previous;
            RadioSetPublicNetwork( RadioPublicNetwork.Current );
        }
        break;
    }
}

void RadioSetChannel( uint32_t freq )
{
    SX126xSetRfFrequency( freq );
}

bool RadioIsChannelFree( RadioModems_t modem, uint32_t freq, int16_t rssiThresh, uint32_t maxCarrierSenseTime )
{
    bool status = true;
   // int16_t rssi = 0;
   // uint32_t carrierSenseTime = 0;

    RadioSetModem( modem );

    RadioSetChannel( freq );

    RadioRx( 0 );

    task_delay( 1 );

    //carrierSenseTime = TimerGetCurrentTime( );

    
     //Perform carrier sense for maxCarrierSenseTime
//    while( TimerGetElapsedTime( carrierSenseTime ) < maxCarrierSenseTime )
//    {
//        rssi = RadioRssi( modem );
//
//        if( rssi > rssiThresh )
//        {
//            status = false;
//            break;
//        }
//    }
    RadioSleep( );
    return status;
}

uint32_t RadioRandom( void )
{
    uint8_t i;
    uint32_t rnd = 0;

    /*
     * Radio setup for random number generation
     */
    // Set LoRa modem ON
    RadioSetModem( MODEM_LORA );

    // Set radio in continuous reception
    SX126xSetRx( 0 );

    for( i = 0; i < 32; i++ )
    {
    	task_delay( 1 );
        // Unfiltered RSSI value reading. Only takes the LSB value
        rnd |= ( ( uint32_t )SX126xGetRssiInst( ) & 0x01 ) << i;
    }

    RadioSleep( );

    return rnd;
}

void RadioSetRxConfig( RadioModems_t modem, uint32_t bandwidth,
                         uint32_t datarate, uint8_t coderate,
                         uint32_t bandwidthAfc, uint16_t preambleLen,
                         uint16_t symbTimeout, bool fixLen,
                         uint8_t payloadLen,
                         bool crcOn, bool freqHopOn, uint8_t hopPeriod,
                         bool iqInverted, bool rxContinuous )
{
	uint8_t syncWordByte[] ={ 0xC1, 0x94, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00 };
	
    RxContinuous = rxContinuous;

    if( fixLen == true )
    {
        MaxPayloadLength = payloadLen;
    }
    else
    {
        MaxPayloadLength = 0xFF;
    }

    switch( modem )
    {
        case MODEM_FSK:
            SX126xSetStopRxTimerOnPreambleDetect( false );
            SX126x.ModulationParams.PacketType = PACKET_TYPE_GFSK;

            SX126x.ModulationParams.Params.Gfsk.BitRate = datarate;
            SX126x.ModulationParams.Params.Gfsk.ModulationShaping = MOD_SHAPING_G_BT_1;
            SX126x.ModulationParams.Params.Gfsk.Bandwidth = RadioGetFskBandwidthRegValue( bandwidth );

            SX126x.PacketParams.PacketType = PACKET_TYPE_GFSK;
            SX126x.PacketParams.Params.Gfsk.PreambleLength = ( preambleLen << 3 ); // convert byte into bit
            SX126x.PacketParams.Params.Gfsk.PreambleMinDetect = RADIO_PREAMBLE_DETECTOR_08_BITS;
            SX126x.PacketParams.Params.Gfsk.SyncWordLength = 3 << 3; // convert byte into bit
            SX126x.PacketParams.Params.Gfsk.AddrComp = RADIO_ADDRESSCOMP_FILT_OFF;
            SX126x.PacketParams.Params.Gfsk.HeaderType = ( fixLen == true ) ? RADIO_PACKET_FIXED_LENGTH : RADIO_PACKET_VARIABLE_LENGTH;
            SX126x.PacketParams.Params.Gfsk.PayloadLength = MaxPayloadLength;
            if( crcOn == true )
            {
                SX126x.PacketParams.Params.Gfsk.CrcLength = RADIO_CRC_2_BYTES_CCIT;
            }
            else
            {
                SX126x.PacketParams.Params.Gfsk.CrcLength = RADIO_CRC_OFF;
            }
            SX126x.PacketParams.Params.Gfsk.DcFree = RADIO_DC_FREE_OFF;

            RadioStandby( );
            RadioSetModem( ( SX126x.ModulationParams.PacketType == PACKET_TYPE_GFSK ) ? MODEM_FSK : MODEM_LORA );
            SX126xSetModulationParams( &SX126x.ModulationParams );
            SX126xSetPacketParams( &SX126x.PacketParams );
            SX126xSetSyncWord( syncWordByte );
            SX126xSetWhiteningSeed( 0x01FF );

            RxTimeout = ( uint32_t )( symbTimeout * ( ( 1.0 / ( double )datarate ) * 8.0 ) * 1000 );
            break;

        case MODEM_LORA:
            SX126xSetStopRxTimerOnPreambleDetect( false );
            SX126xSetLoRaSymbNumTimeout( symbTimeout );
            SX126x.ModulationParams.PacketType = PACKET_TYPE_LORA;
            SX126x.ModulationParams.Params.LoRa.SpreadingFactor = ( RadioLoRaSpreadingFactors_t )datarate;
            SX126x.ModulationParams.Params.LoRa.Bandwidth = Bandwidths[bandwidth];
            SX126x.ModulationParams.Params.LoRa.CodingRate = ( RadioLoRaCodingRates_t )coderate;

            if( ( ( bandwidth == 0 ) && ( ( datarate == 11 ) || ( datarate == 12 ) ) ) ||
            ( ( bandwidth == 1 ) && ( datarate == 12 ) ) )
            {
                SX126x.ModulationParams.Params.LoRa.LowDatarateOptimize = 0x01;
            }
            else
            {
                SX126x.ModulationParams.Params.LoRa.LowDatarateOptimize = 0x00;
            }

            SX126x.PacketParams.PacketType = PACKET_TYPE_LORA;

            if( ( SX126x.ModulationParams.Params.LoRa.SpreadingFactor == LORA_SF5 ) ||
                ( SX126x.ModulationParams.Params.LoRa.SpreadingFactor == LORA_SF6 ) )
            {
                if( preambleLen < 12 )
                {
                    SX126x.PacketParams.Params.LoRa.PreambleLength = 12;
                }
                else
                {
                    SX126x.PacketParams.Params.LoRa.PreambleLength = preambleLen;
                }
            }
            else
            {
                SX126x.PacketParams.Params.LoRa.PreambleLength = preambleLen;
            }

            SX126x.PacketParams.Params.LoRa.HeaderType = ( RadioLoRaPacketLengthsMode_t )fixLen;

            SX126x.PacketParams.Params.LoRa.PayloadLength = MaxPayloadLength;
            SX126x.PacketParams.Params.LoRa.CrcMode = ( RadioLoRaCrcModes_t )crcOn;
            SX126x.PacketParams.Params.LoRa.InvertIQ = ( RadioLoRaIQModes_t )iqInverted;

            RadioSetModem( ( SX126x.ModulationParams.PacketType == PACKET_TYPE_GFSK ) ? MODEM_FSK : MODEM_LORA );
            SX126xSetModulationParams( &SX126x.ModulationParams );
            SX126xSetPacketParams( &SX126x.PacketParams );

            // Timeout Max, Timeout handled directly in SetRx function
             RxTimeout = 0xFFFF;

            break;
    }
}

void RadioSetTxConfig( RadioModems_t modem, int8_t power, uint32_t fdev,
                        uint32_t bandwidth, uint32_t datarate,
                        uint8_t coderate, uint16_t preambleLen,
                        bool fixLen, bool crcOn, bool freqHopOn,
                        uint8_t hopPeriod, bool iqInverted, uint32_t timeout )
{
	uint8_t syncWordByte[] ={ 0xC1, 0x94, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00 };
	
    switch( modem )
    {
        case MODEM_FSK:
            SX126x.ModulationParams.PacketType = PACKET_TYPE_GFSK;
            SX126x.ModulationParams.Params.Gfsk.BitRate = datarate;

            SX126x.ModulationParams.Params.Gfsk.ModulationShaping = MOD_SHAPING_G_BT_1;
            SX126x.ModulationParams.Params.Gfsk.Bandwidth = RadioGetFskBandwidthRegValue( bandwidth );
            SX126x.ModulationParams.Params.Gfsk.Fdev = fdev;

            SX126x.PacketParams.PacketType = PACKET_TYPE_GFSK;
            SX126x.PacketParams.Params.Gfsk.PreambleLength = ( preambleLen << 3 ); // convert byte into bit
            SX126x.PacketParams.Params.Gfsk.PreambleMinDetect = RADIO_PREAMBLE_DETECTOR_08_BITS;
            SX126x.PacketParams.Params.Gfsk.SyncWordLength = 3 << 3 ; // convert byte into bit
            SX126x.PacketParams.Params.Gfsk.AddrComp = RADIO_ADDRESSCOMP_FILT_OFF;
            SX126x.PacketParams.Params.Gfsk.HeaderType = ( fixLen == true ) ? RADIO_PACKET_FIXED_LENGTH : RADIO_PACKET_VARIABLE_LENGTH;

            if( crcOn == true )
            {
                SX126x.PacketParams.Params.Gfsk.CrcLength = RADIO_CRC_2_BYTES_CCIT;
            }
            else
            {
                SX126x.PacketParams.Params.Gfsk.CrcLength = RADIO_CRC_OFF;
            }
            SX126x.PacketParams.Params.Gfsk.DcFree = RADIO_DC_FREEWHITENING;

            RadioStandby( );
            RadioSetModem( ( SX126x.ModulationParams.PacketType == PACKET_TYPE_GFSK ) ? MODEM_FSK : MODEM_LORA );
            SX126xSetModulationParams( &SX126x.ModulationParams );
            SX126xSetPacketParams( &SX126x.PacketParams );
            SX126xSetSyncWord( syncWordByte );
            SX126xSetWhiteningSeed( 0x01FF );
            break;

        case MODEM_LORA:
            SX126x.ModulationParams.PacketType = PACKET_TYPE_LORA;
            SX126x.ModulationParams.Params.LoRa.SpreadingFactor = ( RadioLoRaSpreadingFactors_t ) datarate;
            SX126x.ModulationParams.Params.LoRa.Bandwidth =  Bandwidths[bandwidth];
            SX126x.ModulationParams.Params.LoRa.CodingRate= ( RadioLoRaCodingRates_t )coderate;

            if( ( ( bandwidth == 0 ) && ( ( datarate == 11 ) || ( datarate == 12 ) ) ) ||
            ( ( bandwidth == 1 ) && ( datarate == 12 ) ) )
            {
                SX126x.ModulationParams.Params.LoRa.LowDatarateOptimize = 0x01;
            }
            else
            {
                SX126x.ModulationParams.Params.LoRa.LowDatarateOptimize = 0x00;
            }

            SX126x.PacketParams.PacketType = PACKET_TYPE_LORA;

            if( ( SX126x.ModulationParams.Params.LoRa.SpreadingFactor == LORA_SF5 ) ||
                ( SX126x.ModulationParams.Params.LoRa.SpreadingFactor == LORA_SF6 ) )
            {
                if( preambleLen < 12 )
                {
                    SX126x.PacketParams.Params.LoRa.PreambleLength = 12;
                }
                else
                {
                    SX126x.PacketParams.Params.LoRa.PreambleLength = preambleLen;
                }
            }
            else
            {
                SX126x.PacketParams.Params.LoRa.PreambleLength = preambleLen;
            }

            SX126x.PacketParams.Params.LoRa.HeaderType = ( RadioLoRaPacketLengthsMode_t )fixLen;
            SX126x.PacketParams.Params.LoRa.PayloadLength = MaxPayloadLength;
            SX126x.PacketParams.Params.LoRa.CrcMode = ( RadioLoRaCrcModes_t )crcOn;
            SX126x.PacketParams.Params.LoRa.InvertIQ = ( RadioLoRaIQModes_t )iqInverted;

            RadioStandby( );
            RadioSetModem( ( SX126x.ModulationParams.PacketType == PACKET_TYPE_GFSK ) ? MODEM_FSK : MODEM_LORA );
            SX126xSetModulationParams( &SX126x.ModulationParams );
            SX126xSetPacketParams( &SX126x.PacketParams );
            break;
    }
    SX126xSetRfTxPower( power );
    TxTimeout = timeout;
}

bool RadioCheckRfFrequency( uint32_t frequency )
{
    return true;
}

uint32_t RadioTimeOnAir( RadioModems_t modem, uint8_t pktLen )
{
    uint32_t airTime = 0;

    switch( modem )
    {
    case MODEM_FSK:
        {
           airTime = rint( ( 8 * ( SX126x.PacketParams.Params.Gfsk.PreambleLength +
                                     ( SX126x.PacketParams.Params.Gfsk.SyncWordLength >> 3 ) +
                                     ( ( SX126x.PacketParams.Params.Gfsk.HeaderType == RADIO_PACKET_FIXED_LENGTH ) ? 0.0 : 1.0 ) +
                                     pktLen +
                                     ( ( SX126x.PacketParams.Params.Gfsk.CrcLength == RADIO_CRC_2_BYTES ) ? 2.0 : 0 ) ) /
                                     SX126x.ModulationParams.Params.Gfsk.BitRate ) * 1e3 );
        }
        break;
    case MODEM_LORA:
        {
            double ts = RadioLoRaSymbTime[SX126x.ModulationParams.Params.LoRa.Bandwidth - 4][12 - SX126x.ModulationParams.Params.LoRa.SpreadingFactor];
            // time of preamble
            double tPreamble = ( SX126x.PacketParams.Params.LoRa.PreambleLength + 4.25 ) * ts;
            // Symbol length of payload and time
            double tmp = ceil( ( 8 * pktLen - 4 * SX126x.ModulationParams.Params.LoRa.SpreadingFactor +
                                 28 + 16 * SX126x.PacketParams.Params.LoRa.CrcMode -
                                 ( ( SX126x.PacketParams.Params.LoRa.HeaderType == LORA_PACKET_FIXED_LENGTH ) ? 20 : 0 ) ) /
                                 ( double )( 4 * ( SX126x.ModulationParams.Params.LoRa.SpreadingFactor -
                                 ( ( SX126x.ModulationParams.Params.LoRa.LowDatarateOptimize > 0 ) ? 2 : 0 ) ) ) ) *
                                 ( ( SX126x.ModulationParams.Params.LoRa.CodingRate % 4 ) + 4 );
            double nPayload = 8 + ( ( tmp > 0 ) ? tmp : 0 );
            double tPayload = nPayload * ts;
            // Time on air
            double tOnAir = tPreamble + tPayload;
            // return milli seconds
            airTime = floor( tOnAir + 0.999 );
        }
        break;
    }
    return airTime;
}

void RadioSend( uint8_t *buffer, uint8_t size )
{
    SX126xSetDioIrqParams( IRQ_TX_DONE | IRQ_RX_TX_TIMEOUT,
                           IRQ_TX_DONE | IRQ_RX_TX_TIMEOUT,
                           IRQ_RADIO_NONE,
                           IRQ_RADIO_NONE );

    if( SX126xGetPacketType( ) == PACKET_TYPE_LORA )
    {
        SX126x.PacketParams.Params.LoRa.PayloadLength = size;
    }
    else
    {
        SX126x.PacketParams.Params.Gfsk.PayloadLength = size;
    }
    SX126xSetPacketParams( &SX126x.PacketParams );

    SX126xSendPayload( buffer, size, 0 );
//    TimerSetValue( &TxTimeoutTimer, TxTimeout );
//    TimerStart( &TxTimeoutTimer );
}

void RadioSleep( void )
{
    SleepParams_t params = { 0 };

    params.Fields.WarmStart = 1;
    SX126xSetSleep( params );

    task_delay( 2 );
}

void RadioStandby( void )
{
    SX126xSetStandby( STDBY_RC );
}

void RadioRx( uint32_t timeout )
{
    SX126xSetDioIrqParams( IRQ_RADIO_ALL, //IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT,
                           IRQ_RADIO_ALL, //IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT,
                           IRQ_RADIO_NONE,
                           IRQ_RADIO_NONE );
    

    if( RxContinuous == true )
    {
        SX126xSetRx( 0xFFFFFF ); // Rx Continuous
    }
    else
    {
        SX126xSetRx( timeout << 6 );
    }
}

void RadioRxBoosted( uint32_t timeout )
{
    SX126xSetDioIrqParams( IRQ_RADIO_ALL, //IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT,
                           IRQ_RADIO_ALL, //IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT,
                           IRQ_RADIO_NONE,
                           IRQ_RADIO_NONE );


    if( RxContinuous == true )
    {
        SX126xSetRxBoosted( 0xFFFFFF ); // Rx Continuous
    }
    else
    {
        SX126xSetRxBoosted( timeout << 6 );
    }
}

void RadioSetRxDutyCycle( uint32_t rxTime, uint32_t sleepTime )
{
    SX126xSetRxDutyCycle( rxTime, sleepTime );
}

void RadioStartCad( void )
{
    SX126xSetCad( );
}

void RadioTx( uint32_t timeout )
{
    SX126xSetTx( timeout << 6 );
}

void RadioSetTxContinuousWave( uint32_t freq, int8_t power, uint16_t time )
{
    SX126xSetRfFrequency( freq );
    SX126xSetRfTxPower( power );
    SX126xSetTxContinuousWave( );

//    TimerSetValue( &RxTimeoutTimer, time  * 1e3 );
//    TimerStart( &RxTimeoutTimer );
}

int16_t RadioRssi( RadioModems_t modem )
{
    return SX126xGetRssiInst( );
}

void RadioWrite( uint16_t addr, uint8_t data )
{
    SX126xWriteRegister( addr, data );
}

uint8_t RadioRead( uint16_t addr )
{
    return SX126xReadRegister( addr );
}

void RadioWriteBuffer( uint16_t addr, uint8_t *buffer, uint8_t size )
{
    SX126xWriteRegisters( addr, buffer, size );
}

void RadioReadBuffer( uint16_t addr, uint8_t *buffer, uint8_t size )
{
    SX126xReadRegisters( addr, buffer, size );
}

void RadioWriteFifo( uint8_t *buffer, uint8_t size )
{
    SX126xWriteBuffer( 0, buffer, size );
}

void RadioReadFifo( uint8_t *buffer, uint8_t size )
{
    SX126xReadBuffer( 0, buffer, size );
}

void RadioSetMaxPayloadLength( RadioModems_t modem, uint8_t max )
{
    if( modem == MODEM_LORA )
    {
        SX126x.PacketParams.Params.LoRa.PayloadLength = MaxPayloadLength = max;
        SX126xSetPacketParams( &SX126x.PacketParams );
    }
    else
    {
        if( SX126x.PacketParams.Params.Gfsk.HeaderType == RADIO_PACKET_VARIABLE_LENGTH )
        {
            SX126x.PacketParams.Params.Gfsk.PayloadLength = MaxPayloadLength = max;
            SX126xSetPacketParams( &SX126x.PacketParams );
        }
    }
}

void RadioSetPublicNetwork( bool enable )
{
    RadioPublicNetwork.Current = RadioPublicNetwork.Previous = enable;

    RadioSetModem( MODEM_LORA );
    if( enable == true )
    {
        // Change LoRa modem SyncWord
        SX126xWriteRegister( REG_LR_SYNCWORD, ( LORA_MAC_PUBLIC_SYNCWORD >> 8 ) & 0xFF );
        SX126xWriteRegister( REG_LR_SYNCWORD + 1, LORA_MAC_PUBLIC_SYNCWORD & 0xFF );
    }
    else
    {
        // Change LoRa modem SyncWord
        SX126xWriteRegister( REG_LR_SYNCWORD, ( LORA_MAC_PRIVATE_SYNCWORD >> 8 ) & 0xFF );
        SX126xWriteRegister( REG_LR_SYNCWORD + 1, LORA_MAC_PRIVATE_SYNCWORD & 0xFF );
    }
}

uint32_t RadioGetWakeupTime( void )
{
    return( RADIO_TCXO_SETUP_TIME + RADIO_WAKEUP_TIME );
}

void RadioOnTxTimeoutIrq( void )
{
    if( ( RadioEvents != NULL ) && ( RadioEvents->TxTimeout != NULL ) )
    {
        RadioEvents->TxTimeout( );
    }
}

void RadioOnRxTimeoutIrq( void )
{
    if( ( RadioEvents != NULL ) && ( RadioEvents->RxTimeout != NULL ) )
    {
        RadioEvents->RxTimeout( );
    }
}

void RadioOnDioIrq( void )
{
    IrqFired = true;
}

void RadioIrqProcess( void )
{
	uint16_t irqRegs=0;
   // if( IrqFired == true )
    if(HAL_GPIO_ReadPin(SX126X_DIO1_PIN_PORT, SX126X_DIO1_PIN_NUMBER)==1)
    {
        IrqFired = false;

        irqRegs = SX126xGetIrqStatus( );
        SX126xClearIrqStatus( IRQ_RADIO_ALL );
        
        if( ( irqRegs & IRQ_TX_DONE ) == IRQ_TX_DONE )
        {
 
            if( ( RadioEvents != NULL ) && ( RadioEvents->TxDone != NULL ) )
            {
                RadioEvents->TxDone( );
            }
        }

        if( ( irqRegs & IRQ_RX_DONE ) == IRQ_RX_DONE )
        {
            uint8_t size;

            SX126xGetPayload( RadioRxPayload, &size , 255 );
            SX126xGetPacketStatus( &RadioPktStatus );
            if( ( RadioEvents != NULL ) && ( RadioEvents->RxDone != NULL ) )
            {
                RadioEvents->RxDone( RadioRxPayload, size, RadioPktStatus.Params.LoRa.RssiPkt, RadioPktStatus.Params.LoRa.SnrPkt );
            }
        }

        if( ( irqRegs & IRQ_CRC_ERROR ) == IRQ_CRC_ERROR )
        {
            if( ( RadioEvents != NULL ) && ( RadioEvents->RxError ) )
            {
                RadioEvents->RxError( );
            }
        }

        if( ( irqRegs & IRQ_CAD_DONE ) == IRQ_CAD_DONE )
        {
            if( ( RadioEvents != NULL ) && ( RadioEvents->CadDone != NULL ) )
            {
                RadioEvents->CadDone( ( ( irqRegs & IRQ_CAD_ACTIVITY_DETECTED ) == IRQ_CAD_ACTIVITY_DETECTED ) );
            }
        }

        if( ( irqRegs & IRQ_RX_TX_TIMEOUT ) == IRQ_RX_TX_TIMEOUT )
        {
            if( SX126xGetOperatingMode( ) == MODE_TX )
            {
                if( ( RadioEvents != NULL ) && ( RadioEvents->TxTimeout != NULL ) )
                {
                    RadioEvents->TxTimeout( );
                }
            }
            else if( SX126xGetOperatingMode( ) == MODE_RX )
            {
 
                if( ( RadioEvents != NULL ) && ( RadioEvents->RxTimeout != NULL ) )
                {
                    RadioEvents->RxTimeout( );
                }
            }
        }

        if( ( irqRegs & IRQ_PREAMBLE_DETECTED ) == IRQ_PREAMBLE_DETECTED )
        {
            //__NOP( );
        }

        if( ( irqRegs & IRQ_SYNCWORD_VALID ) == IRQ_SYNCWORD_VALID )
        {
            //__NOP( );
        }

        if( ( irqRegs & IRQ_HEADER_VALID ) == IRQ_HEADER_VALID )
        {
            //__NOP( );
        }

        if( ( irqRegs & IRQ_HEADER_ERROR ) == IRQ_HEADER_ERROR )
        {
            if( ( RadioEvents != NULL ) && ( RadioEvents->RxTimeout != NULL ) )
            {
                RadioEvents->RxTimeout( );
            }
        }
    }
}
#endif //#ifdef	LORA_ENABLED

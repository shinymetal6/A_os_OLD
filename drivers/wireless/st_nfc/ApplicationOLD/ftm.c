/*
 * ftm.c
 *
 *  Created on: Aug 7, 2023
 *      Author: fil
 */

#include "main.h"
#include "ftm.h"
#include "app.h"
#include "../ST25FTM/st25ftm_protocol.h"
#include "st25ftm_config.h"
#include "../RFAL/st25r3916/st25r3916_com.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



static rfalNfcDiscoverParam 	discParam;

uint8_t					*uid = NULL;

static uint8_t ftm_setup(rfalNfcvListenDevice *nfcvDev)
{
uint8_t err;
static uint8_t        pwd[8] = {0,0,0,0,0,0,0,0};
uint8_t               pwdLen = 8;
uint8_t               pwdNum = 0;
uint8_t               pointer;
uint8_t               reg;

    err = rfalNfcvPollerSelect(RFAL_NFCV_REQ_FLAG_DEFAULT, nfcvDev->InvRes.UID );
	/* Open RF configuration security session */
	err = rfalST25xVPollerPresentPassword(RFAL_NFCV_REQ_FLAG_DEFAULT, uid, pwdNum, pwd, pwdLen);
	if ( err )
		return 1;
	/* set MB_MODE to 1: Enabling fast transfer mode is authorized.*/
	pointer = ST25DV04K_SYSTEM_CONFIG_MB_MODE;
	reg = ST25DV04K_REG_MB_MODE_FTM_AUTHORIZED;
	err = rfalST25xVPollerWriteConfiguration(RFAL_NFCV_REQ_FLAG_DEFAULT, uid, pointer, reg);
	if ( err )
		return 1;
	/* set MB_EN to 1: Enable FTM. Read, update and write */
	pointer = ST25DV04K_DYN_REG_MB_CTRL;
	err = rfalST25xVPollerFastReadDynamicConfiguration(RFAL_NFCV_REQ_FLAG_DEFAULT, uid, pointer, &reg);
	if ( err )
		return 1;
	reg |= ST25DV04K_REG_MB_CTRL_DYN_MB_EN;
	err = rfalST25xVPollerFastWriteDynamicConfiguration(RFAL_NFCV_REQ_FLAG_DEFAULT, uid, pointer, reg);
	if ( err )
		return 1;
	return err;
}

uint8_t max_retry_len=0,max_retry_msg=0;
#define	MAX_RETRY_LEN	15
#define	MAX_RETRY_MSG	15
#define	DELAY_RETRY		10

uint8_t ftm_messages(void)
{
uint8_t retry = 0;

	uint8_t err = 1;
	//rfalNfcWorker();                                    /* Run RFAL worker periodically */

	bzero(System.mbx_rxbuf,RX_MBX_SIZE);
	while ( retry < 40 )
	{
		err = rfalST25xVPollerReadMessage( RFAL_NFCV_REQ_FLAG_DEFAULT, NULL, 0, 0, System.mbx_rxbuf, ST25FTM_BUFFER_LENGTH, &System.mbx_rxbuf_len );
		if ( err == 0 )
		{
			if ( System.mbx_rxbuf_len != 0 )
			{
				System.rx_msg_count++;
				log_data(System.mbx_rxbuf_len);
				//err = rfalST25xVPollerWriteMessage( RFAL_NFCV_REQ_FLAG_DEFAULT, NULL, 2, msgData, System.mbx_txbuf, 2 );

				return 0;
			}
		}
		else
			retry++;
	}

	platformLedOn(PLATFORM_LED_ERROR_PORT, PLATFORM_LED_ERROR_PIN);
	platformLedOff(PLATFORM_LED_CARDFOUND_PORT, PLATFORM_LED_CARDFOUND_PIN);
	return 1;

}

static void notifier( rfalNfcState st )
{
}

uint16_t ftm_init( void )
{
uint16_t err;

    err = rfalNfcInitialize();
    if( err == RFAL_ERR_NONE )
    {
        rfalNfcDefaultDiscParams( &discParam );
        discParam.devLimit			= 1U;
        discParam.notifyCb			= notifier;
        discParam.totalDuration		= 100U;
        discParam.techs2Find		= RFAL_NFC_POLL_TECH_V;
        System.poller_state 		= START_DISCOVERY;
        err							= 0;
        platformLedOff(PLATFORM_LED_V_PORT, PLATFORM_LED_V_PIN);
        platformLedOff(PLATFORM_LED_ERROR_PORT, PLATFORM_LED_ERROR_PIN);
        platformLedOff(PLATFORM_LED_CARDFOUND_PORT, PLATFORM_LED_CARDFOUND_PIN);
        platformLedOn(PLATFORM_LED_NOCARD_PORT, PLATFORM_LED_NOCARD_PIN);
    }
    return err;
}

static rfalNfcDevice *nfcDevice;
uint8_t ftm_cycle( void )
{
    rfalNfcWorker();                                    /* Run RFAL worker periodically */
    switch( System.poller_state )
    {
        case START_DISCOVERY:
			rfalNfcDeactivate( false );
			rfalNfcDiscover( &discParam );
			System.poller_state = DISCOVERY;
			platformLedOff(PLATFORM_LED_V_PORT, PLATFORM_LED_V_PIN);
	        platformLedOff(PLATFORM_LED_ERROR_PORT, PLATFORM_LED_ERROR_PIN);
	        platformLedOff(PLATFORM_LED_CARDFOUND_PORT, PLATFORM_LED_CARDFOUND_PIN);
	        platformLedOn(PLATFORM_LED_NOCARD_PORT, PLATFORM_LED_NOCARD_PIN);
         break;
        case DISCOVERY:
            if( rfalNfcIsDevActivated( rfalNfcGetState() ) )
            {
                rfalNfcGetActiveDevice( &nfcDevice );
                switch( nfcDevice->type )
                {
                     case RFAL_NFC_LISTEN_TYPE_NFCV:
                        {
                            platformLedOn(PLATFORM_LED_V_PORT, PLATFORM_LED_V_PIN);
                            ftm_setup(&nfcDevice->dev.nfcv);
                            System.poller_state = NFC_FOUND;
                	        platformLedOff(PLATFORM_LED_NOCARD_PORT, PLATFORM_LED_NOCARD_PIN);
                        }
                        break;
                    default:
                        break;
                }
                /*
                rfalNfcDeactivate( false );
                platformDelay( 500 );
                System.poller_state = DEMO_ST_START_DISCOVERY;
                */
            }
            break;
        case	NFC_FOUND:
        	break;
        case NOTINIT:
        default:
            System.poller_state 						= START_DISCOVERY;
            break;
    }
    return System.poller_state;
}

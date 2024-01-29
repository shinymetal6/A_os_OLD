

#ifndef RFAL_PLATFORM_H
#define RFAL_PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

//#include "stm32h5xx_hal.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

#include "main.h"
#include "../Platform/Platform_STM32H7xx.h"
//#include "nfc08a1_conf.h"



/*
******************************************************************************
* GLOBAL DEFINES
******************************************************************************
*/

#define PLATFORM_LED_A_PIN			LED_A_Pin             /*!< GPIO pin used for LED A    */
#define PLATFORM_LED_A_PORT         LED_A_GPIO_Port       /*!< GPIO port used for LED A   */
#define PLATFORM_LED_B_PIN          LED_B_Pin             /*!< GPIO pin used for LED B    */
#define PLATFORM_LED_B_PORT         LED_B_GPIO_Port       /*!< GPIO port used for LED B   */
#define PLATFORM_LED_F_PIN          LED_F_Pin             /*!< GPIO pin used for LED F    */
#define PLATFORM_LED_F_PORT         LED_F_GPIO_Port       /*!< GPIO port used for LED F   */
#define PLATFORM_LED_V_PIN          LED_V_Pin             /*!< GPIO pin used for LED V    */
#define PLATFORM_LED_V_PORT         LED_V_GPIO_Port       /*!< GPIO port used for LED V   */
#define PLATFORM_LED_AP2P_PIN       LED_AP2P_Pin          /*!< GPIO pin used for LED AP2P */
#define PLATFORM_LED_AP2P_PORT      LED_AP2P_GPIO_Port    /*!< GPIO port used for LED AP2P*/
#define PLATFORM_LED_FIELD_PIN		LED_FIELD_Pin         /*!< GPIO pin used as field LED */
#define PLATFORM_LED_FIELD_PORT     LED_FIELD_GPIO_Port   /*!< GPIO port used as field LED*/
#define PLATFORM_LED_ERROR_PIN		LD3_Pin          /*!< GPIO pin used as error LED */
#define PLATFORM_LED_ERROR_PORT     LD3_GPIO_Port    /*!< GPIO port used as error LED*/
#define PLATFORM_LED_NOCARD_PIN		LD2_Pin       /*!< GPIO pin used as error LED */
#define PLATFORM_LED_NOCARD_PORT    LD2_GPIO_Port /*!< GPIO port used as error LED*/
#define PLATFORM_LED_CARDFOUND_PIN	LD1_Pin        /*!< GPIO pin used as error LED */
#define PLATFORM_LED_CARDFOUND_PORT LD1_GPIO_Port  /*!< GPIO port used as error LED*/

#define PLATFORM_USER_BUTTON_PIN    B1_Pin                /*!< GPIO pin user button       */
#define PLATFORM_USER_BUTTON_PORT   B1_GPIO_Port          /*!< GPIO port user button      */

/*
******************************************************************************
* GLOBAL MACROS
******************************************************************************
*/
#define platformProtectST25RComm()                    do{ globalCommProtectCnt++; __DSB();NVIC_DisableIRQ(EXTI0_IRQn);__DSB();__ISB();}while(0) /*!< Protect unique access to ST25R communication channel - IRQ disable on single thread environment (MCU) ; Mutex lock on a multi thread environment      */
#define platformUnprotectST25RComm()                  do{ if (--globalCommProtectCnt==0) {NVIC_EnableIRQ(EXTI0_IRQn);} }while(0)                /*!< Unprotect unique access to ST25R communication channel - IRQ enable on a single thread environment (MCU) ; Mutex unlock on a multi thread environment */

#define platformProtectST25RIrqStatus()               platformProtectST25RComm()                    /*!< Protect unique access to IRQ status var - IRQ disable on single thread environment (MCU) ; Mutex lock on a multi thread environment */
#define platformUnprotectST25RIrqStatus()             platformUnprotectST25RComm()                  /*!< Unprotect the IRQ status var - IRQ enable on a single thread environment (MCU) ; Mutex unlock on a multi thread environment         */

#define platformLedOff( port, pin )                   platformGpioClear(port, pin)                  /*!< Turns the given LED Off                     */
#define platformLedOn( port, pin )                    platformGpioSet(port, pin)                    /*!< Turns the given LED On                      */
#define platformLedToggle( port, pin )                platformGpioToggle(port, pin)                 /*!< Toggles the given LED                       */

#define platformGpioSet( port, pin )                  HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET)    /*!< Turns the given GPIO High                   */
#define platformGpioClear( port, pin )                HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET)  /*!< Turns the given GPIO Low                    */
#define platformGpioToggle( port, pin )               HAL_GPIO_TogglePin(port, pin)                 /*!< Toggles the given GPIO                      */
#define platformGpioIsHigh( port, pin )               (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_SET) /*!< Checks if the given LED is High             */
#define platformGpioIsLow( port, pin )                (!platformGpioIsHigh(port, pin))              /*!< Checks if the given LED is Low              */

extern	bool timerIsExpired( uint32_t timer );
extern	uint32_t timerCalculateTimer( uint16_t time );

#define platformTimerCreate( t )                      timerCalculateTimer(t)                        /*!< Create a timer with the given time (ms)     */
#define platformTimerIsExpired( timer )               timerIsExpired(timer)                         /*!< Checks if the given timer is expired        */
#define platformDelay( t )                            HAL_Delay(t)                                  /*!< Performs a delay for the given time (ms)    */

#define platformGetSysTick()                          HAL_GetTick()                                 /*!< Get System Tick ( 1 tick = 1 ms)            */

#define platformAssert( exp )                         assert_param( exp )                           /*!< Asserts whether the given expression is true*/
#define platformErrorHandle()                         Error_Handler()                               /*!< Global error handle\trap                    */


extern	int32_t BSP_SPI1_SendRecv(const uint8_t * const pTxData, uint8_t * const pRxData, uint16_t Length);
#define platformSpiSelect()                           platformGpioClear(ST25R_SS_PORT, ST25R_SS_PIN)/*!< SPI SS\CS: Chip|Slave Select                */
#define platformSpiDeselect()                         platformGpioSet(ST25R_SS_PORT, ST25R_SS_PIN)  /*!< SPI SS\CS: Chip|Slave Deselect              */
#define platformSpiTxRx( txBuf, rxBuf, len )          BSP_SPI1_SendRecv(txBuf, rxBuf, len)                    /*!< SPI transceive                              */

#define platformLog(...)                              //logUsart(__VA_ARGS__)                         /*!< Log  method                                 */

/*
******************************************************************************
* GLOBAL VARIABLES
******************************************************************************
*/
extern uint8_t globalCommProtectCnt;                      /* Global Protection Counter provided per platform - instantiated in main.c    */

/*
******************************************************************************
* RFAL FEATURES CONFIGURATION
******************************************************************************
*/

#define RFAL_FEATURE_LISTEN_MODE               false      /*!< Enable/Disable RFAL support for Listen Mode                               */
#define RFAL_FEATURE_WAKEUP_MODE               true       /*!< Enable/Disable RFAL support for the Wake-Up mode                          */
#define RFAL_FEATURE_LOWPOWER_MODE             false      /*!< Enable/Disable RFAL support for the Low Power mode                        */
#define RFAL_FEATURE_NFCA                      false      /*!< Enable/Disable RFAL support for NFC-A (ISO14443A)                         */
#define RFAL_FEATURE_NFCB                      false      /*!< Enable/Disable RFAL support for NFC-B (ISO14443B)                         */
#define RFAL_FEATURE_NFCF                      false      /*!< Enable/Disable RFAL support for NFC-F (FeliCa)                            */
#define RFAL_FEATURE_NFCV                      true       /*!< Enable/Disable RFAL support for NFC-V (ISO15693)                          */
#define RFAL_FEATURE_T1T                       false      /*!< Enable/Disable RFAL support for T1T (Topaz)                               */
#define RFAL_FEATURE_T2T                       false      /*!< Enable/Disable RFAL support for T2T                                       */
#define RFAL_FEATURE_T4T                       false      /*!< Enable/Disable RFAL support for T4T                                       */
#define RFAL_FEATURE_ST25TB                    false      /*!< Enable/Disable RFAL support for ST25TB                                    */
#define RFAL_FEATURE_ST25xV                    true       /*!< Enable/Disable RFAL support for ST25TV/ST25DV                             */
#define RFAL_FEATURE_DYNAMIC_ANALOG_CONFIG     false      /*!< Enable/Disable Analog Configs to be dynamically updated (RAM)             */
#define RFAL_FEATURE_DPO                       false      /*!< Enable/Disable RFAL Dynamic Power Output support                          */
#define RFAL_FEATURE_ISO_DEP                   false      /*!< Enable/Disable RFAL support for ISO-DEP (ISO14443-4)                      */
#define RFAL_FEATURE_ISO_DEP_POLL              false      /*!< Enable/Disable RFAL support for Poller mode (PCD) ISO-DEP (ISO14443-4)    */
#define RFAL_FEATURE_ISO_DEP_LISTEN            false      /*!< Enable/Disable RFAL support for Listen mode (PICC) ISO-DEP (ISO14443-4)   */
#define RFAL_FEATURE_NFC_DEP                   false      /*!< Enable/Disable RFAL support for NFC-DEP (NFCIP1/P2P)                      */

#define RFAL_FEATURE_ISO_DEP_IBLOCK_MAX_LEN    256U       /*!< ISO-DEP I-Block max length. Please use values as defined by rfalIsoDepFSx */
#define RFAL_FEATURE_NFC_DEP_BLOCK_MAX_LEN     254U       /*!< NFC-DEP Block/Payload length. Allowed values: 64, 128, 192, 254           */
#define RFAL_FEATURE_NFC_RF_BUF_LEN            258U       /*!< RF buffer length used by RFAL NFC layer                                   */

#define RFAL_FEATURE_ISO_DEP_APDU_MAX_LEN      512U       /*!< ISO-DEP APDU max length. Please use multiples of I-Block max length       */
#define RFAL_FEATURE_NFC_DEP_PDU_MAX_LEN       512U       /*!< NFC-DEP PDU max length.                                                   */

/*
******************************************************************************
* RFAL CUSTOM SETTINGS
******************************************************************************
  Custom analog configs are used to cope with Automatic Antenna Tuning (AAT)
  that are optimized differently for each board.
*/
#define RFAL_ANALOG_CONFIG_CUSTOM                         /*!< Use Custom Analog Configs when defined                                    */

#ifndef platformProtectST25RIrqStatus
    #define platformProtectST25RIrqStatus()            /*!< Protect unique access to IRQ status var - IRQ disable on single thread environment (MCU) ; Mutex lock on a multi thread environment */
#endif /* platformProtectST25RIrqStatus */

#ifndef platformUnprotectST25RIrqStatus
    #define platformUnprotectST25RIrqStatus()          /*!< Unprotect the IRQ status var - IRQ enable on a single thread environment (MCU) ; Mutex unlock on a multi thread environment         */
#endif /* platformUnprotectST25RIrqStatus */

#ifndef platformProtectWorker
    #define platformProtectWorker()                    /* Protect RFAL Worker/Task/Process from concurrent execution on multi thread platforms   */
#endif /* platformProtectWorker */

#ifndef platformUnprotectWorker
    #define platformUnprotectWorker()                  /* Unprotect RFAL Worker/Task/Process from concurrent execution on multi thread platforms */
#endif /* platformUnprotectWorker */

#ifndef platformIrqST25RPinInitialize
    #define platformIrqST25RPinInitialize()            /*!< Initializes ST25R IRQ pin                     */
#endif /* platformIrqST25RPinInitialize */

#ifndef platformIrqST25RSetCallback
    #define platformIrqST25RSetCallback( cb )          /*!< Sets ST25R ISR callback                       */
#endif /* platformIrqST25RSetCallback */

#ifndef platformLedsInitialize
    #define platformLedsInitialize()                   /*!< Initializes the pins used as LEDs to outputs  */
#endif /* platformLedsInitialize */

#ifndef platformLedOff
    #define platformLedOff( port, pin )                /*!< Turns the given LED Off                       */
#endif /* platformLedOff */

#ifndef platformLedOn
    #define platformLedOn( port, pin )                 /*!< Turns the given LED On                        */
#endif /* platformLedOn */

#ifndef platformLedToggle
    #define platformLedToggle( port, pin )             /*!< Toggles the given LED                         */
#endif /* platformLedToggle */

#ifndef platformGetSysTick
    #define platformGetSysTick()                       /*!< Get System Tick ( 1 tick = 1 ms)              */
#endif /* platformGetSysTick */

#ifndef platformTimerDestroy
    #define platformTimerDestroy( timer )              /*!< Stops and released the given timer            */
#endif /* platformTimerDestroy */

#ifndef platformLog
    #define platformLog(...)                           /*!< Log method                                    */
#endif /* platformLog */

#ifndef platformAssert
    #define platformAssert( exp )                      /*!< Asserts whether the given expression is true */
#endif /* platformAssert */

#ifndef platformErrorHandle
    #define platformErrorHandle()                      /*!< Global error handler or trap                 */
#endif /* platformErrorHandle */

#ifdef __cplusplus
}
#endif

#endif /* RFAL_PLATFORM_H */

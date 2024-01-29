/******************************************************************************
  * @attention
  *
  * COPYRIGHT 2021 STMicroelectronics, all rights reserved
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied,
  * AND SPECIFICALLY DISCLAIMING THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
******************************************************************************/

/*
 *      PROJECT:   ST25R391x firmware
 *      $Revision: $
 *      LANGUAGE:  ANSI C
 */

/*! \file
 *
 *  \author
 *
 *  \brief Commands shared between the firmware and the PC application.
 *
 */

#ifndef DISPATCHER_COMMANDS_H
#define DISPATCHER_COMMANDS_H


#define ST25R3916_DIRECT_COMMAND               0x15U    /*!< Direct Command */
#define ST25R3916_RFAL_SET_OBSV_MODE           0x17U    /*!< Set Obsv mode */
#define ST25R3916_ANTENNA_GET_VECTOR           0x20U    /*!< Get vector antenna */
#define ST25R3916_RF_FIELD                     0x22U    /*!< Turn RF field On/Off */
#define ST25R3916_RFAL_GET_OBSV_MODE           0x23U    /*!< Get Obsv mode */
#define ST25R3916_HW_IDENTIFY                  0x24U    /*!< Hardware identify (led toggle) */
#define ST25R3916_GET_UID                      0x25U    /*!< Get UID */
#define RFAL_CMD_GET_ANALOG_CONFIG             0x26U    /*!< Get RFAL Analog Config */
#define RFAL_CMD_SET_ANALOG_CONFIG             0x27U    /*!< Set RFAL Analog Config */

#define RFAL_CMD_INITIALIZE                    0x40U    /*!< Initialize */
#define RFAL_CMD_CALIBRATE                     0x41U    /*!< Calibrate */
#define RFAL_CMD_DEINITIALIZE                  0x42U    /*!< Deinitialize */
#define RFAL_CMD_SET_MODE                      0x43U    /*!< Set Mode */
#define RFAL_CMD_GET_MODE                      0x44U    /*!< Get Mode */
#define RFAL_CMD_SET_BITRATE                   0x45U    /*!< Set bitrate */
#define RFAL_CMD_GET_BITRATE                   0x46U    /*!< Get bitrate */
#define RFAL_CMD_SET_FDTPOLL                   0x47U    /*!< Set FDT Poll */
#define RFAL_CMD_GET_FDTPOLL                   0x48U    /*!< Get FDT Poll */
#define RFAL_CMD_SET_FDTLISTEN                 0x49U    /*!< Set FDT Listen */
#define RFAL_CMD_GET_FDTLISTEN                 0x4AU    /*!< Get FDT Listen */
#define RFAL_CMD_SET_GT                        0x4BU    /*!< Set GT */
#define RFAL_CMD_GET_GT                        0x4CU    /*!< Get GT */
#define RFAL_CMD_FIELDON_STARTGT               0x4DU    /*!< Field On start GT */
#define RFAL_CMD_FIELDOFF                      0x4EU    /*!< Field Off */
#define RFAL_CMD_START_TXRX                    0x4FU    /*!< Start TXRX */

#define RFAL_CMD_GET_TXRX_STATUS               0x50U    /*!< Get TXRX Status */
#define RFAL_CMD_ISO14443_TXRX_SHORTFRAME      0x51U    /*!< ISO14443 TXRX Short Frame */
#define RFAL_CMD_ISO14443_TXRX_ANTICOLLISION   0x52U    /*!< ISO14443 TXRX Anticollision */
#define RFAL_CMD_FELICA_POLL                   0x53U    /*!< Felica Poll */
#define RFAL_CMD_ISO15693_TXRX_ANTICOLLISION   0x54U    /*!< ISO15693 TXRX Anticollision */
#define RFAL_CMD_ISO15693_TXRX_ANTICOLLISION_EOF 0x55U  /*!< ISO15693 TXRX Anticollision EOF */
#define RFAL_CMD_ISO15693_TXRX_EOF             0x56U    /*!< ISO15693 TXRX EOF */
#define RFAL_CMD_BLOCKING_TX                   0x57U    /*!< Blocking TX */
#define RFAL_CMD_BLOCKING_RX                   0x58U    /*!< Blocking RX */
#define RFAL_CMD_BLOCKING_TXRX                 0x59U    /*!< Blocking TXRX */
#define RFAL_CMD_DPO_READ                      0x5AU    /*!< DPO Read */
#define RFAL_CMD_DPO_WRITE                     0x5BU    /*!< DPO Write */
#define RFAL_CMD_DPO_MEASURE_FUNC              0x5CU    /*!< DPO Measure */
#define RFAL_CMD_DPO_ADJUST_AND_MEASURE        0x5DU    /*!< DPO Adjust and measure */
#define RFAL_CMD_DPO_SET_ENABLED               0x5EU    /*!< DPO Set enabled */
#define RFAL_CMD_DPO_IS_ENABLED                0x5FU    /*!< DPO Is enabled */

#define RFAL_CHIP_CMD_SET_RFO                  0x60U    /*!< Set RFO */
#define RFAL_CHIP_CMD_GET_RFO                  0x61U    /*!< Get RFO */
#define RFAL_CHIP_CMD_WAKEUP_START             0x62U    /*!< Wakeup Start */
#define RFAL_CHIP_CMD_WAKEUP_HASWOKE           0x63U    /*!< Wakeup Has woke */
#define RFAL_CHIP_CMD_WAKEUP_GETINFO           0x64U    /*!< Wakeup Get information */
#define RFAL_CHIP_CMD_WAKEUP_STOP              0x65U    /*!< Wakeup Stop */
#define RFAL_CMD_AC_SET                        0x66U    /*!< AC Set */
#define RFAL_CMD_DPO_GET_CURRENT_TABLE_ENTRY   0x67U    /*!< DPO Get current table index */
#define RFAL_CMD_DISPATCHER                    0x68U    /*!< RFAL command dispatcher */

#define RFAL_CMD_GET_TRANSCEIVE_RSSI           0x70U    /*!< Get Transceive RSSI */

#define ST25R3916_UID_LENGTH                     12U    /*!< UID length */

enum
{
	RfalCmd_None          = 0x000,
	RfalCmd_InitialiseA   = 0x001,
	RfalCmd_SelectA       = 0x002,
	RfalCmd_WakeupA       = 0x003,
	RfalCmd_DeInitialiseA = 0x004,

	RfalCmd_InitialiseB   = 0x011,
	RfalCmd_SelectB       = 0x012,
	RfalCmd_WakeupB       = 0x013,
	RfalCmd_DeInitialiseB = 0x014,
};

#endif /* DISPATCHER_COMMANDS_H */

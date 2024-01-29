/******************************************************************************
  * @attention
  *
  * COPYRIGHT 2018 STMicroelectronics, all rights reserved
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

#ifndef ST25FTM_COMMON_H
#define ST25FTM_COMMON_H

#include <stdint.h>
#include "st25ftm_protocol.h"
#include "st25ftm_config.h"

/*! Timeout before retransmiting a segment */
#define ST25FTM_WAIT_TIMEOUT (1000U)

/*! Helper to compute maximum number of bytes in a packet */
#define ST25FTM_MAX_DATA_IN_SINGLE_PACKET() ((gFtmState.tx.frameMaxLength) - sizeof(ST25FTM_Ctrl_Byte_t) )

/*! Helper to decide if a packet has a length byte */
#define ST25FTM_CTRL_HAS_PKT_LEN(msg)   ((msg).b.pktLen != 0U)

/*! Helper to decide if a packet has total length bytes */
#define ST25FTM_CTRL_HAS_TOTAL_LEN(msg) (((msg).b.position) == (uint8_t)(ST25FTM_FIRST_PACKET))

/*! Helper to decide if a packet is the only packet of the transfer */
#define ST25FTM_CTRL_IS_SINGLE_PACKET(ctrl) ((ST25FTM_Packet_Position_t)(ctrl) == (ST25FTM_SINGLE_PACKET))
/*! Helper to decide if a packet is the last packet of the transfer */
#define ST25FTM_CTRL_IS_LAST_PACKET(ctrl) ((ST25FTM_Packet_Position_t)(ctrl) == (ST25FTM_LAST_PACKET))
/*! Helper to decide if a transfer is completed */
#define ST25FTM_CTRL_IS_COMMAND_COMPLETE(msg) (ST25FTM_CTRL_IS_SINGLE_PACKET(((ST25FTM_Ctrl_Byte_t*)msg)->b.position) ||\
                                              ST25FTM_CTRL_IS_LAST_PACKET(((FTM_Ctrl_Byte_t*)msg)->b.position))


/*! Helper to decide if a packet has a CRC and is the only packet of the transfer */
#define ST25FTM_CTRL_IS_ACK_SINGLE(ackCtrl) ((ackCtrl) == (uint8_t)(ST25FTM_ACK_SINGLE_PKT))
/*! Helper to decide if a packet has a CRC and is part of a bigger transfer */
#define ST25FTM_CTRL_IS_SEGMENT_END(ackCtrl) ((ackCtrl) == (uint8_t)(ST25FTM_SEGMENT_END))
/*! Helper to decide if a packet has a CRC */
#define ST25FTM_CTRL_HAS_CRC(msg)       (ST25FTM_CTRL_IS_ACK_SINGLE((msg).b.ackCtrl) || \
                                      ST25FTM_CTRL_IS_SEGMENT_END((msg).b.ackCtrl))


/*! Helper to get packet length byre */
#define ST25FTM_GET_PKT_LEN(msg)        (((ST25FTM_Header_t*)msg)->with_len.length)
/*! Helper to get transfer length when packet length is present */
#define ST25FTM_GET_TOTAL_LEN_WITH_LEN(msg)     (((ST25FTM_Header_t*)msg)->with_len.totalLength)
/*! Helper to get transfer length when packet length is not present */
#define ST25FTM_GET_TOTAL_LEN_WITHOUT_LEN(msg)  (((ST25FTM_Header_t*)msg)->without_len.totalLength)

/*! Helper to invert word endianness */
#define ST25FTM_CHANGE_ENDIANESS(x) ( (x) = \
                                  (((x)>>24U)&0xFFU) | \
                                  (((x)>>8U)&0xFF00U)| \
                                  (((x)<<8U)&0xFF0000U)| \
                                  (((x)<<24U)&0xFF000000U))

/*! Defines packet positions in the transfer */
typedef enum {
  ST25FTM_SINGLE_PACKET = 0U,	/*!< This is the only packet for the transfer */
  ST25FTM_FIRST_PACKET = 1U,	/*!< This is the first packet of the transfer */
  ST25FTM_MIDDLE_PACKET = 2U,	/*!< This is a middle packet of the transfer */
  ST25FTM_LAST_PACKET = 3U		/*!< This is the last packet of the transfer */
} ST25FTM_Packet_Position_t;

/*! Defines the policy for packets acknowledgment */
typedef enum {
  ST25FTM_NO_ACK_PACKET = 0U,	/*!< This packet is not part of a segment: no CRC to be computed on this data */
  ST25FTM_SEGMENT_START = 1U,	/*!< This packet is the beginning of a segment: CRC to be initialized with this data */
  ST25FTM_SEGMENT_END = 2U,		/*!< This packet is the end of a segment: a CRC is present and a ACK is expected in response */
  ST25FTM_ACK_SINGLE_PKT = 3U	/*!< This is the only packet for the segment: a CRC is present and a ACK is expected in response */
} ST25FTM_Packet_Acknowledge_t;

/*! Defines ACK possible status */
typedef enum {
  ST25FTM_SEGMENT_OK = 0,		/*!< Segment data has been successfully validated */
  ST25FTM_CRC_ERROR = 1,		/*!< Segment data has not been successfully validated */
  ST25FTM_ACK_RFU = 2,			/*!< Reserved for Futur Use */
  ST25FTM_ABORT_TRANSFER = 3,	/*!< An unrecoverable error occured */
  ST25FTM_ACK_BUSY=-1,			/*!< The ACK has not been read/written, retry */
  ST25FTM_ACK_ERROR=-2			/*!< An error has occured */
} ST25FTM_Acknowledge_Status_t;

/*! Defines protocol bytes */
typedef enum {
  ST25FTM_CTRL_BYTE = 0x0,		/*!< This is a control byte: b7 = 0 */
  ST25FTM_STATUS_BYTE = 0x80	/*!< This is a status byte: b7 = 1 */
} ST25FTM_Packet_Identifier_t;

/*! Defines control byte content */
typedef union {
 struct {
  uint8_t inSegment:1;	/*!< b0: this packet is part of a segment */
  uint8_t segId:1;		/*!< b1: segment id, allow desync issue detection */
  uint8_t position:2;	/*!< b2-b3: position of the packet in the transfer */
  uint8_t ackCtrl:2;	/*!< b4-b5: position of the packet in the segment */
  uint8_t pktLen:1;		/*!< b6: length byte is present (the packet doesn't use the full length of the buffer) */
  uint8_t type:1;		/*!< b7: control byte = 0 */
  } b;					/*!< Bitwise access to the control byte */
  uint8_t byte;			/*!< Bytewise access to the control byte */
} ST25FTM_Ctrl_Byte_t;

/*! This struct is used to decode the received ST25FTM headers, it must be packed */
typedef  ST25FTM_PACKED(union)  {
	ST25FTM_PACKED(struct)  {
  uint8_t ctrl;						/*!< control byte */
  ST25FTM_Packet_Length_t length;	/*!< length byte */
  uint32_t totalLength;				/*!< total transfer length word */
  }  with_len;						/*!< when length byte is present */
  ST25FTM_PACKED(struct)  {
  uint8_t ctrl;						/*!< control byte */
  uint32_t totalLength;				/*!< totaltransfer length word */
  }  without_len;					/*!< when length byte is not present */
}  ST25FTM_Header_t;

/*! Packet information structure */
typedef struct {
  ST25FTM_Ctrl_Byte_t ctrl;	/*!< Control byte */
  uint32_t length;			/*!< Packet length */
  uint32_t totalLength;		/*!< Total transfer length */
  ST25FTM_Crc_t crc;		/*!< CRC value */
  uint8_t* data;			/*!< Payload pointer */
  uint8_t has_crc;			/*!< Packet has CRC */
} ST25FTM_Packet_t;

/*! Defines status byte content */
typedef union {
  struct {
    uint8_t status:4;	/*!< Segment status */
    uint8_t rfu:3;		/*!< Reserved for futur use */
    uint8_t type:1;		/*!< Status byte = 1 */
  } bit;				/*!< Bitwise access to the status byte */
  uint8_t byte;			/*!< Bytewise access to the status byte */
} ST25FTM_Status_Byte_t;

/*! Tx state machine variables */
typedef struct {
  ST25FTM_TxState_t   state;						/*!< Current Tx state */
  ST25FTM_TxState_t   lastState;					/*!< Last Tx state */
  uint8_t*        cmdPtr;							/*!< Pointer on the buffer to be sent */
  uint32_t        cmdLen;							/*!< Number of bytes in the transfer */
  uint32_t        remainingData;					/*!< Number of bytes still to be tranfsered */
  uint32_t        frameMaxLength;					/*!< Max tx frame length */
  uint32_t        nbError;							/*!< Number of errors in Tx */
  uint8_t         lastError;						/*!< Last error that occured in Tx */
  ST25FTM_Send_Ack_t  sendAck;						/*!< Ackowledgment policy for the transfer */
  uint8_t*        dataPtr;							/*!< Pointer on the data curently being transmitted */
  uint8_t*        segmentPtr;						/*!< Pointer on the segment currently being transmitted */
  uint8_t*        segmentStart;						/*!< Pointer on the beginning of the segment currently being transmitted, used for retransmission */
  uint32_t        segmentLength;					/*!< Length of a segment, in bytes  */
  uint32_t        segmentRemainingData;				/*!< Number of bytes still to be transmitted in the segment */
  uint32_t        segmentMaxLength;					/*!< Max segment length in bytes */
  uint8_t         retransmit;						/*!< Segment id, used to detect retransmission */
  uint32_t        pktIndex;							/*!< Packet index  */
  uint32_t        segmentIndex;						/*!< Segment index */
  uint8_t         packetBuf[ST25FTM_BUFFER_LENGTH];	/*!< Packet buffer */
  uint32_t        packetLength;						/*!< Packet length */
  uint32_t        segmentNumber;					/*!< Number of segment */
  ftm_data_cb     getdata_cb;						/*!< Callback when fetching data to be sent (optional) */
} ST25Ftm_InternalTxState_t;

/*! Rx state machine variables */
typedef struct {
  ST25FTM_RxState_t state;							/*!< Current Rx state */
  ST25FTM_RxState_t lastState;						/*!< Last RX state */
  uint8_t       isNewFrame;							/*!< A new transfer started */
  uint8_t*      cmdPtr;								/*!< Pointer to the reception buffer */
  uint32_t*     cmdLen;								/*!< Reception buffer length / used to return transfer length */
  uint32_t      maxCmdLen;							/*!< Max length of the reception */
  uint32_t      nbError;							/*!< Number of errors in Rx */
  uint8_t       lastError;							/*!< Last error in Rx */
  uint8_t       unrecoverableError;					/*!< An unrecoverable error occured */
  uint32_t      frameMaxLength;						/*!< Max Rx frame length */
  uint32_t      receivedLength;						/*!< Number of bytes received */
  uint32_t      validLength;						/*!< Number of bytes to be acknowledged */
  uint32_t      validReceivedLength;				/*!< Number of bytes acknowledged */
  uint32_t      totalValidReceivedLength;			/*!< Total number of bytes acknowledged */
  uint8_t*      segmentPtr;							/*!< Pointer on the segment */
  uint32_t      segmentLength;						/*!< Length of the segment */
  uint8_t*      dataPtr;							/*!< Pointer on the data currently being transmitted */
  uint8_t       lastAck;							/*!< Last packet had a CRC, a ACK is expected from peer device */
  uint8_t       rewriteOnFieldOff;					/*!< Used to rewrite the last message when RF field goes Off */
  uint8_t       ignoreRetransSegment;				/*!< Used to ignore a retransmitted segment */
  ST25FTM_Packet_Position_t pktPosition;			/*!< Indicated position of the packet in the transmission */
  uint32_t      segmentNumber;						/*!< Index of current segment */
  ftm_data_cb   recvdata_cb;						/*!< Optional callback used when data has been received */
  uint32_t      readBufferOffset;					/*!< Number of bytes currently received */
} ST25Ftm_InternalRxState_t;

/*! State machine return code */
typedef enum {
  ST25FTM_STATE_MACHINE_CONTINUE,					/*!< State machine immediately continue with next state */
  ST25FTM_STATE_MACHINE_RELEASE						/*!< State machine is waiting for an event to occur, exit */
} ST25FTM_StateMachineCtrl_t;

/*! ST25FTM global variables */
typedef struct {
  ST25FTM_State_t           state;					/*!< Current state */
  ST25FTM_State_t           lastState;				/*!< Previous state */
  ST25FTM_Field_State_t     rfField;				/*!< Is RF field present */
  uint32_t              totalDataLength;			/*!< Number of bytes actually transmitted (including metadata) */
  uint32_t              retryLength;				/*!< Number of bytes retransmitted */
  ST25Ftm_InternalTxState_t tx;						/*!< Tx state variables */
  ST25Ftm_InternalRxState_t rx;						/*!< Rx state variables */
  uint32_t              lastTick;					/*!< Timestamp of the previous call to the ST25FTM state machine */
} ST25FTM_InternalState_t;

/*! ST25FTM global variables */
extern ST25FTM_InternalState_t gFtmState;


/* API for ftm_tx/rx */
void ST25FTM_CRC_Initialize(void);
ST25FTM_Crc_t ST25FTM_GetCrc(uint8_t *data, uint32_t length, ST25FTM_crc_control_t control);
void logHexBuf(uint8_t* buf, uint32_t len);
ST25FTM_Acknowledge_Status_t ST25FTM_GetAcknowledgeStatus(void);
uint32_t ST25FTM_CompareTime(uint32_t a, uint32_t b);


/* From ftm_tx/rx */
void ST25FTM_State_Init(void);
void ST25FTM_TxStateInit(void);
void ST25FTM_RxStateInit(void);
void ST25FTM_Transmit(void);
void ST25FTM_Receive(void);
void ST25FTM_TxResetSegment(void);


#endif /* ST25FTM_COMMON_H */


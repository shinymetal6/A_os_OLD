/******************************************************************************
  * @attention
  *
  * COPYRIGHT 2019 STMicroelectronics, all rights reserved
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
#ifndef __FTM_CONFIG_H__
#define __FTM_CONFIG_H__
#include <stdint.h>
#include "../RFAL/rfal_st25xv.h"

typedef enum {
  ST25FTM_MSG_OK =0,
  ST25FTM_MSG_ERROR,
  ST25FTM_MSG_BUSY
} ST25FTM_MessageStatus_t;

typedef enum {
  ST25FTM_MESSAGE_EMPTY = 0,
  ST25FTM_MESSAGE_ME = 1,
  ST25FTM_MESSAGE_PEER = 2,
  ST25FTM_MESSAGE_OWNER_ERROR = 3
} ST25FTM_MessageOwner_t;

typedef enum {
  ST25FTM_CRC_START,
  ST25FTM_CRC_END,
  ST25FTM_CRC_ACCUMULATE,
  ST25FTM_CRC_ONESHOT
} ST25FTM_crc_control_t;

typedef uint32_t ST25FTM_Crc_t;

#if defined ( __GNUC__ ) && !defined (__CC_ARM)
/* GNU Compiler: packed attribute must be placed after the type keyword */
#define ST25FTM_PACKED(type) type __attribute__((packed,aligned(1)))
#else
/* ARM Compiler: packed attribute must be placed before the type keyword */
#define ST25FTM_PACKED(type) __packed type
#endif

/* Length of the buffer used to store a single message data */
#define ST25FTM_BUFFER_LENGTH (260)
/* Define format of the packet length field, when present */
typedef uint8_t ST25FTM_Packet_Length_t;

/* Length of the buffer used to store unvalidated data */
#define ST25FTM_SEGMENT_LEN (1024 + 16 + 12)

#define ST25FTM_TICK()  HAL_GetTick()

#define ST25FTM_CRYPTO_ENABLE 0

#define ST25FTM_ENABLE_LOG 0
#if (ST25FTM_ENABLE_LOG != 0)
#include "rfal_platform.h"
char* hex2Str(unsigned char * data, size_t dataLen);
int logUsart(const char* format, ...);
#define ST25FTM_LOG(...)  do { platformLog("%d: ",ST25FTM_TICK()); platformLog(__VA_ARGS__); } while (0)
#define ST25FTM_HEX2STR(buf,len) hex2Str(buf,len)
#else
#define ST25FTM_LOG(...)  
#define ST25FTM_HEX2STR(buf,len) 
#endif

void ST25FTM_SetDevice(rfalNfcvListenDevice *device);

/* Interface API */
/* Functions to implement for the platform */
ST25FTM_MessageOwner_t ST25FTM_GetMessageOwner(void);
ST25FTM_MessageStatus_t ST25FTM_ReadMessage(uint8_t *msg, uint32_t* msg_len);
ST25FTM_MessageStatus_t ST25FTM_WriteMessage(uint8_t* msg, uint32_t msg_len);
int ST25FTM_DeviceInit(void);
void ST25FTM_UpdateFieldStatus(void);
void ST25FTM_CRC_Initialize(void);
ST25FTM_Crc_t ST25FTM_GetCrc(uint8_t *data, uint32_t length, ST25FTM_crc_control_t crc_control);

#endif /* __FTM_CONFIG_H__ */

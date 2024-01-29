/******************************************************************************
  * @attention
  *
  * COPYRIGHT 2016 STMicroelectronics, all rights reserved
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

#ifndef STREAM_LOG_H
#define STREAM_LOG_H

#include <stdbool.h>
#include "st_errno.h"


#define LOG_BUFFER_SIZE            64
#define LOG_BUFFER_COUNT            8

typedef struct
{
    uint16_t offset;
    uint8_t  buffer[LOG_BUFFER_SIZE];
} log_t;

/*!
 *****************************************************************************
 * \brief Initialize the logging module
 *
 * This function initializes the logging module (clears buffers, sets \0,...)
 *
 * \return ERR_NONE on success, or a standard error code
 *****************************************************************************
 */
stError circularLogInit(bool enable);

/*!
 *****************************************************************************
 * \brief Add a buffer to the log buffer
 *
 * This functions adds a buffer to the end of the log buffer
 *
 * \param[in] data: buffer to log
 * \param[in] length: length to log
 * \param[in] bool: flush the buffer or keep adding to it
 *
 * \return ERR_NONE on success, or a standard error code
 *****************************************************************************
 */
stError circularLogBuffer(const uint8_t* data, uint16_t length, bool flush);

/*!
 *****************************************************************************
 * \brief Print a buffer to the log buffer
 *
 * This function convert hex2str and add the formatted string to the log buffer
 *
 * \param[in] buffer: buffer to log as a string
 * \param[in] length: length to log
 *
 * \return ERR_NONE on success, or a standard error code
 *****************************************************************************
 */
stError circularLogStringBuffer(const uint8_t* data, uint16_t length);

/*!
 *****************************************************************************
 * \brief Print string to the log buffer
 *
 * This function writes a formatted string to the log buffer, works like printf
 *
 * \param format[in]: format string for printf
 *
 * \return ERR_NONE on success, or a standard error code
 *****************************************************************************
 */
stError circularLogString(const char* format, ...);

/*!
 *****************************************************************************
 * \brief Get the log buffer
 *
 * This function gets the earliest log buffer
 *
 * \param[out] message: Pointer to the message to retrieve
 * \param[out] length:  Length of the message retrieved
 *
 * \return ERR_NONE on success, or a standard error code
 *****************************************************************************
 */
stError circularLogGet(uint8_t** string, uint32_t* length);

#endif /* STREAM_LOG_H */

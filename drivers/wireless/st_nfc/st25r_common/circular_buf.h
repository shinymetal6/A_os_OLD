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

#ifndef CIRCULAR_BUF_H
#define CIRCULAR_BUF_H

/*
 ******************************************************************************
 * INCLUDES
 ******************************************************************************
 */
#include <stdbool.h>
#include <stdint.h>

/*
 ******************************************************************************
 * DEFINES
 ******************************************************************************
 */

#define CIRCULAR_BUF_MONITOR

/*
 ******************************************************************************
 * GLOBAL DATA TYPES
 ******************************************************************************
 */

typedef struct circular_buf_t
{
    void* buffer;

    uint32_t head;
    uint32_t tail;
    uint32_t maxLen;
    uint32_t capacity; /* number of maxLen buffers */

    bool full;
#ifdef CIRCULAR_BUF_MONITOR
    uint32_t put_count;
    uint32_t full_count;
    uint32_t lost_count;
    uint32_t longest;
#endif
} circular_buf_t;

typedef struct circular_buf_t* cbuf_handle_t;

/*
 ******************************************************************************
 * GLOBAL FUNCTION PROTOTYPES
 ******************************************************************************
 */

/*!
 *****************************************************************************
 * \brief Reset the circular buffer to empty state
 *
 * This function initializes the circular buffer
 *
 * \param[in] h: Handle to the circular buffer
 *****************************************************************************
 */
bool circular_buf_reset(cbuf_handle_t h);

/*!
 *****************************************************************************
 * \brief Check whether the circular buffer is empty
 *
 * \param[in] h: Handle to the circular buffer
 *
 * \return true if empty, false otherwise
 *****************************************************************************
 */
bool circular_buf_is_empty(cbuf_handle_t h);

/*!
 *****************************************************************************
 * \brief Check whether the circular buffer is full
 *
 * \param[in] h: Handle to the circular buffer
 *
 * \return true if full, false otherwise
 *****************************************************************************
 */
bool circular_buf_is_full(cbuf_handle_t h);

/*!
 *****************************************************************************
 * \brief Return the circular buffer capacity
 *
 * The capacity is the total number of buffers handled as circular buffers.
 *
 * \param[in] h: Handle to the circular buffer
 *
 * \return the total number of buffers handled as circular buffers
 *****************************************************************************
 */
uint32_t circular_buf_capacity(cbuf_handle_t h);

/*!
 *****************************************************************************
 * \brief Return the current number of buffers stored as circular buffers
 *
 * \param[in] h: Handle to the circular buffer
 *
 * \return the actual number of buffers handled as circular buffers
 *****************************************************************************
 */
uint32_t circular_buf_size(cbuf_handle_t h);

/*!
 *****************************************************************************
 * \brief Return the number of bytes available in current log
 *
 * This is useful when concatenating logs.
 *
 * \param[in] h: Handle to the circular buffer
 *
 * \return the number of bytes allowed to add in the current log
 *****************************************************************************
 */
uint32_t circular_buf_put_length_available(cbuf_handle_t h);

#ifdef CIRCULAR_BUF_MONITOR
/*!
 *****************************************************************************
 * \brief Return monitoring information
 *
 * Return how many logs were added.
 *
 * \param[in] h: Handle to the circular buffer
 *
 * \return how many times logs were added
 *****************************************************************************
 */
uint32_t circular_buf_put_count(cbuf_handle_t h);

/*!
 *****************************************************************************
 * \brief Return monitoring information
 *
 * Return how many times the circular buffer was full.
 *
 * \param[in] h: Handle to the circular buffer
 *
 * \return how many times the circular buffer was full
 *****************************************************************************
 */
uint32_t circular_buf_full_count(cbuf_handle_t h);

/*!
 *****************************************************************************
 * \brief Return monitoring information
 *
 * Return how many buffers were lost due to the circular buffer being full.
 *
 * \param[in] h: Handle to the circular buffer
 *
 * \return the number of the circular buffer lost
 *****************************************************************************
 */
uint32_t circular_buf_lost_count(cbuf_handle_t h);

/*!
 *****************************************************************************
 * \brief Return monitoring information
 *
 * Return the length of longest buffer stored.
 *
 * \param[in] h: Handle to the circular buffer
 *
 * \return the length of the longest buffer stored
 *****************************************************************************
 */
uint32_t circular_buf_longest(cbuf_handle_t h);
#endif

/*!
 *****************************************************************************
 * \brief Initialize a circular buffer to store buffers
 *
 * The buffers are stored in the buffers given as parameter
 *
 * \param[in] h:      Circular buffer to initialize
 * \param[in] buffer: Buffer used to store the buffers
 * \param[in] length: Length of the buffers used to store buffers
 * \param[in] count:  Number of buffers
 *
 * \return handle to the circular buffer
 *****************************************************************************
 */
cbuf_handle_t circular_buf_init(circular_buf_t* h, uint8_t* data, uint32_t length, uint32_t capacity);

/*!
 *****************************************************************************
 * \brief Put data into the circular buffer
 *
 * Store a buffer into the circular buffer.
 * The buffer is not stored when the circular buffer is full.
 *
 * \param[in] h:      Handle to the circular buffer
 * \param[in] data:   Pointer to data to store
 * \param[in] length: Length of the data to store
 * \param[in] flush:  Flush the buffer or keep adding to it
 *
 * \return the number of bytes stored
 *****************************************************************************
 */
int32_t circular_buf_put(cbuf_handle_t h, uint8_t* data, uint32_t length, bool flush);

/*!
 *****************************************************************************
 * \brief Put data into the circular buffer
 *
 * Store a buffer into the circular buffer.
 * The buffer is stored even if the circular buffer is full,
 * the oldest buffer is lost.
 *
 * \param[in] h:      Handle to the circular buffer
 * \param[in] data:   Pointer to data to store
 * \param[in] length: Length of the data to store
 * \param[in] flush:  Flush the buffer or keep adding to it
 *
 * \return the number of bytes stored
 *****************************************************************************
 */
int32_t circular_buf_put_overwrite(cbuf_handle_t h, uint8_t* data, uint32_t length, bool flush);

/*!
 *****************************************************************************
 * \brief Get the earliest buffer stored into the circular buffer
 *
 * Get the oldest buffer that was stored in the circular buffer.
 *
 * \param[in]  h:      Handle to the circular buffer
 * \param[out] data:   Pointer to the data to retrieve
 * \param[out] length: Length of the data retrieved
 *
 * \return true if the buffer was retrieved, false if there is none
 *****************************************************************************
 */
bool circular_buf_get(cbuf_handle_t h, uint8_t** data, uint32_t* length);

#endif /* CIRCULAR_BUF_H */

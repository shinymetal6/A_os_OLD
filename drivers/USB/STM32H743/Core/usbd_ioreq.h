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
 * Project : A_os
*/
/*
 * usbd_ioreq.h
 *
 *  Created on: Jan 24, 2024
 *      Author: fil
 */

#ifndef DRIVERS_USB_STM32H743_CORE_USBD_IOREQ_H_
#define DRIVERS_USB_STM32H743_CORE_USBD_IOREQ_H_

#ifdef	STM32H743xx

#ifdef __cplusplus
extern "C" {
#endif

#include  "usbd_def.h"
#include  "usbd_core.h"

USBD_StatusTypeDef USBD_CtlSendData(USBD_HandleTypeDef *pdev, uint8_t *pbuf, uint32_t len);

USBD_StatusTypeDef USBD_CtlContinueSendData(USBD_HandleTypeDef *pdev, uint8_t *pbuf, uint32_t len);

USBD_StatusTypeDef USBD_CtlPrepareRx(USBD_HandleTypeDef *pdev, uint8_t *pbuf, uint32_t len);

USBD_StatusTypeDef USBD_CtlContinueRx(USBD_HandleTypeDef *pdev, uint8_t *pbuf, uint32_t len);

USBD_StatusTypeDef USBD_CtlSendStatus(USBD_HandleTypeDef *pdev);
USBD_StatusTypeDef USBD_CtlReceiveStatus(USBD_HandleTypeDef *pdev);

uint32_t USBD_GetRxCount(USBD_HandleTypeDef *pdev, uint8_t ep_addr);

#ifdef __cplusplus
}
#endif

#endif // #ifdef	STM32H743xx

#endif /* DRIVERS_USB_STM32H743_CORE_USBD_IOREQ_H_ */

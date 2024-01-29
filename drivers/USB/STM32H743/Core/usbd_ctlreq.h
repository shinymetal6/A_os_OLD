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
 * usbd_ctlreq.h
 *
 *  Created on: Jan 24, 2024
 *      Author: fil
 */

#ifndef DRIVERS_USB_STM32H743_CORE_USBD_CTLREQ_H_
#define DRIVERS_USB_STM32H743_CORE_USBD_CTLREQ_H_

#ifdef	STM32H743xx

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_def.h"

USBD_StatusTypeDef USBD_StdDevReq(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
USBD_StatusTypeDef USBD_StdItfReq(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
USBD_StatusTypeDef USBD_StdEPReq(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

void USBD_CtlError(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
void USBD_ParseSetupRequest(USBD_SetupReqTypedef *req, uint8_t *pdata);
void USBD_GetString(uint8_t *desc, uint8_t *unicode, uint16_t *len);

#ifdef __cplusplus
}
#endif

#endif // #ifdef	STM32H743xx

#endif /* DRIVERS_USB_STM32H743_CORE_USBD_CTLREQ_H_ */

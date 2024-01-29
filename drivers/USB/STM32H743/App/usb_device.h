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
 * usb_device.h
 *
 *  Created on: Jan 23, 2024
 *      Author: fil
 */

#ifndef DRIVERS_USB_STM32H743_APP_USB_DEVICE_H_
#define DRIVERS_USB_STM32H743_APP_USB_DEVICE_H_

#ifndef __USB_DEVICE__H__
#define __USB_DEVICE__H__

#ifdef	STM32H743xx

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"
#include "../Core/usbd_def.h"

/** USB Device initialization function. */
void MX_USB_DEVICE_Init(void);

#ifdef __cplusplus
}
#endif

#endif // #ifdef	STM32H743xx

#endif /* __USB_DEVICE__H__ */


#endif /* DRIVERS_USB_STM32H743_APP_USB_DEVICE_H_ */

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
 * usbd_MIDI_desc.h
 *
 *  Created on: Jan 24, 2024
 *      Author: fil
 */

#ifndef DRIVERS_USB_STM32H743_APP_USBD_MIDI_DESC_H_
#define DRIVERS_USB_STM32H743_APP_USBD_MIDI_DESC_H_

#ifdef	STM32H743xx

#ifdef __cplusplus
 extern "C" {
#endif

#include "../Core/usbd_def.h"

#define         DEVICE_ID1          (UID_BASE)
#define         DEVICE_ID2          (UID_BASE + 0x4)
#define         DEVICE_ID3          (UID_BASE + 0x8)

#define  USB_SIZ_STRING_SERIAL       0x1A

/** Descriptor for the Usb device. */
extern USBD_DescriptorsTypeDef FS_Desc;

#ifdef __cplusplus
}
#endif

#endif // #ifdef	STM32H743xx

#endif /* DRIVERS_USB_STM32H743_APP_USBD_MIDI_DESC_H_ */

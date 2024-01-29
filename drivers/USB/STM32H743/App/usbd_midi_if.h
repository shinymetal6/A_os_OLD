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
 * usbd_midi_if.h
 *
 *  Created on: Jan 24, 2024
 *      Author: fil
 */

#ifndef DRIVERS_USB_STM32H743_APP_USBD_MIDI_IF_H_
#define DRIVERS_USB_STM32H743_APP_USBD_MIDI_IF_H_

#ifdef	STM32H743xx

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include "../../../../kernel/system_default.h"

#ifdef	USB_MIDI

/* Includes ------------------------------------------------------------------*/
#include "../Class/MIDI/usbd_midi.h"
#define APP_RX_DATA_SIZE  2048
#define APP_TX_DATA_SIZE  2048

/** CDC Interface callback. */
extern USBD_MIDI_ItfTypeDef USBD_Interface_fops_FS;

uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);


#ifdef __cplusplus
}
#endif

#endif //ifdef	USB_CDC

#endif // #ifdef	STM32H743xx

#endif /* DRIVERS_USB_STM32H743_APP_USBD_MIDI_IF_H_ */

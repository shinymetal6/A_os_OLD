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
 * usb_device.c
 *
 *  Created on: Jan 23, 2024
 *      Author: fil
 */

#include "main.h"

#ifdef	STM32H743xx
#include "../../../../kernel/system_default.h"
#ifdef	USB_ENABLED

#include "usb_device.h"
#include "../Core/usbd_core.h"

#ifdef	USB_CDC
#include "usbd_CDC_desc.h"
#include "../Class/CDC/usbd_cdc.h"
#include "usbd_cdc_if.h"
#endif

#ifdef	USB_MIDI
#include "usbd_MIDI_desc.h"
#include "../Class/MIDI/usbd_midi.h"
#include "usbd_midi_if.h"
#endif

/* USB Device Core handle declaration. */
USBD_HandleTypeDef hUsbDeviceFS;

uint8_t MX_USB_Device_Init(void)
{
	  /* Init Device Library, add supported class and start the library. */
#ifdef	USB_ENABLED
	  if (USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS) != USBD_OK)
		  return 1;
#ifdef	USB_CDC
	  if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC) != USBD_OK)
		  return 1;
	  if (USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS) != USBD_OK)
		  return 1;
	  if (USBD_Start(&hUsbDeviceFS) != USBD_OK)
		  return 1;
#endif
#ifdef	USB_MIDI
	  if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_MIDI) != USBD_OK)
		  return 1;
	  if (USBD_MIDI_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS) != USBD_OK)
		  return 1;
	  if (USBD_Start(&hUsbDeviceFS) != USBD_OK)
		  return 1;
#endif
	  HAL_PWREx_EnableUSBVoltageDetector();
#endif
	  return 0;

}
#endif // #ifdef	USB_ENABLED

#endif // #ifdef	STM32H743xx

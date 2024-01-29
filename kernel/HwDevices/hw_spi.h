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
 * hw_spi.h
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */

#ifndef KERNEL_HWDEVICES_HW_SPI_H_
#define KERNEL_HWDEVICES_HW_SPI_H_

#if defined SPI1_ENABLED
extern	SPI_HandleTypeDef		hspi1;
#endif

#if defined NFC_ENABLED
#define	ST25R3915_SPI			hspi1
#endif

#define HW_SPI_ERROR_NONE                     0
#define HW_SPI_ERROR_BUS_FAILURE             -1
#define HW_SPI_ERROR_WRONG_PARAM             -2
#define HW_SPI_ERROR_FEATURE_NOT_SUPPORTED   -3
#define HW_SPI_ERROR_HW_NOT_OWNED			 -4

#define HW_SPI_TIMEOUT        2000U /* baud rate of SPI1 = 5 Mbps*/

#endif /* KERNEL_HWDEVICES_HW_SPI_H_ */

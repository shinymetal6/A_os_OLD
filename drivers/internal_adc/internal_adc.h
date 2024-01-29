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
 * Project : bb1xx_743_00 
*/
/*
 * internal_adc.h
 *
 *  Created on: Jan 3, 2024
 *      Author: fil
 */

#ifndef DRIVERS_INTERNAL_ADC_INTERNAL_ADC_H_
#define DRIVERS_INTERNAL_ADC_INTERNAL_ADC_H_

#ifdef ADC_ENABLED

typedef struct _ControlAdcDef
{
	uint16_t 	pot[6];
	uint16_t 	analog_in[4];
	uint8_t 	adc_flag;
}ControlAdcDef;

#define	INT_ADC_POT_DONE		0x80
#define	INT_ADC_ANALOG_IN_DONE	0x40

#define HW_ADC_ERROR_NONE			0
#define HW_ADC_ERROR_HW_NOT_OWNED	1


#endif // #ifdef ADC_DAC_ENABLED

#endif /* DRIVERS_INTERNAL_ADC_INTERNAL_ADC_H_ */

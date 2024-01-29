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
 * vca.c
 *
 *  Created on: Jan 8, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"	/* for BOARD_NAME variable only */
#include "../../kernel/kernel_opt.h"

#ifdef SYNTH_ENGINE_ENABLE

#include "../../kernel/audio.h"
#include "oscillators.h"
#include "oscillator_core.h"

extern	AudioFlagsTypeDef	AudioFlags;
uint8_t	vco_am=0;
void Vca( uint16_t *buffer_out,uint16_t *buffer_in)
{
uint32_t	i;
	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		if (( AudioFlags.control_flags & CONTROL_VCA) == CONTROL_VCA)
			*buffer_out++ = (uint16_t )((float )*buffer_in++ * ((float )vco_am / 4096.0F));
		else
			*buffer_out++ = *buffer_in++;
	}
}
#endif //#ifdef SYNTH_ENGINE_ENABLE

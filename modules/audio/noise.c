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
 * noise.c
 *
 *  Created on: Jan 8, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"	/* for BOARD_NAME variable only */
#include "../../kernel/audio.h"				/* for audio parameters */
#include "../../kernel/kernel_opt.h"

#ifdef SYNTH_ENGINE_ENABLE
#include <stdlib.h>

#ifdef	HWRANDOM_GEN
extern	RNG_HandleTypeDef hrng;
#endif

void Noise_Gen(int16_t *noise_buffer_gen)
{
uint32_t	i;
uint32_t random_number;
	for(i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i+=2)
	{
#ifdef	HWRANDOM_GEN
		HAL_RNG_GenerateRandomNumber(&hrng, &random_number);
		noise_buffer_gen[i] =  (random_number>>16) & 0xffff;
		noise_buffer_gen[i+1] =  random_number & 0xffff;
#else
		noise_buffer_gen[i] =  (rand()>>16) & 0xffff;
		noise_buffer_gen[i+1] =  rand() & 0xffff;
#endif
	}
}
#endif // #ifdef SYNTH_ENGINE_ENABLE

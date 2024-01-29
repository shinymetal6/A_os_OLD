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
 * audio.c
 *
 *  Created on: Jan 8, 2024
 *      Author: fil
 */

#include "main.h"
#include "A.h"
#include "system_default.h"
#include "A_exported_functions.h"
#include "kernel_opt.h"

#ifdef CODEC_ENABLED

#pragma GCC optimize("Ofast")

#include "audio.h"
DMA_NOCACHE_RAM	WaveLR_t	 WaveLR_Out[AUDIO_BUF_SIZE];
DMA_NOCACHE_RAM	WaveLR_t	WaveLR_In[AUDIO_BUF_SIZE];
DMA_NOCACHE_RAM	WaveLR_t	*audio_out, *audio_in;

OSCILLATORS_RAM	__attribute__ ((aligned (16))) AudioFlagsTypeDef	AudioFlags;
#define	OSCILLATORS	1

uint8_t StartAudioBuffers(int16_t *audio_in_buffer,int16_t *audio_out_buffer)
{
#ifdef CODEC_NAU88C22
	if ( allocate_hw(HW_I2S2,0) == HW_I2S2)
	{
		if ( HAL_I2SEx_TransmitReceive_DMA(&NAU88C22_I2S, (uint16_t*)audio_out_buffer, (uint16_t*)audio_in_buffer,AUDIO_BUF_SIZE*2) != HAL_OK)
		{
			deallocate_hw(HW_I2S2);
			return 1;
		}
		audio_out = (WaveLR_t *)audio_out_buffer;
		audio_in  = (WaveLR_t *)audio_in_buffer;
	}
	return 0;
#endif
	return 1;
}

ITCM_AREA_CODE void get_limits(uint16_t *start,uint16_t *end)
{
	if (( AudioFlags.audio_flags & AUDIO_HALFBUFOUT_FLAG ) == AUDIO_HALFBUFOUT_FLAG)
	{
		*start=0;
		*end = HALF_NUMBER_OF_AUDIO_SAMPLES;
	}
	else
	{
		*start=HALF_NUMBER_OF_AUDIO_SAMPLES;
		*end = NUMBER_OF_AUDIO_SAMPLES;
	}
}
extern uint16_t	oscout_buffer[HALF_NUMBER_OF_AUDIO_SAMPLES];

ITCM_AREA_CODE void IrqProcessSamples(void)
{
uint16_t	start,end,i;
	HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_SET);
#ifdef	OSCILLATORS
	RunOscillator32();
	get_limits(&start,&end);
	for(i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		/*
		WaveLR_Out[i+start].channel[AUDIO_LEFT_CH] = oscout_buffer[i];
		WaveLR_Out[i+start].channel[AUDIO_RIGHT_CH] = oscout_buffer[i];
		*/
		audio_out[i+start].channel[AUDIO_LEFT_CH]  = oscout_buffer[i];
		audio_out[i+start].channel[AUDIO_RIGHT_CH] = oscout_buffer[i];
	}
#else
	get_limits(&start,&end);
	for(i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		audio_out[i+start].channel[AUDIO_LEFT_CH]  = audio_in[i+start].channel[AUDIO_LEFT_CH];
		audio_out[i+start].channel[AUDIO_RIGHT_CH] = audio_in[i+start].channel[AUDIO_RIGHT_CH];
	}
#endif
	HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_RESET);
}


ITCM_AREA_CODE void HAL_I2SEx_TxRxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
	AudioFlags.audio_flags |= (AUDIO_HALFBUFOUT_FLAG | AUDIO_OUT_READY_FLAG);
	AudioFlags.audio_flags |= (AUDIO_HALFBUFIN_FLAG | AUDIO_IN_READY_FLAG);
	IrqProcessSamples();
}

ITCM_AREA_CODE void HAL_I2SEx_TxRxCpltCallback(I2S_HandleTypeDef *hi2s)
{
	AudioFlags.audio_flags &= ~AUDIO_HALFBUFOUT_FLAG;
	AudioFlags.audio_flags |= AUDIO_OUT_READY_FLAG;
	AudioFlags.audio_flags &= ~AUDIO_HALFBUFIN_FLAG;
	AudioFlags.audio_flags |= AUDIO_IN_READY_FLAG;
	IrqProcessSamples();
}

#endif

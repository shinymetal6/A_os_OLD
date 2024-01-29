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
 * oscillators.c
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

OSCILLATORS_RAM	__attribute__ ((aligned (16))) OscillatorsTypeDef	Oscillator[NUMOSCILLATORS];

ITCM_AREA_CODE	uint32_t FindOscillatorByMidiNote(uint8_t midi_note)
{
uint32_t	osc_number;
	for(osc_number=0;osc_number<NUMOSCILLATORS;osc_number++)
		if ( Oscillator[osc_number].midi_note == midi_note )
			return osc_number;
	return NUMOSCILLATORS+1;
}

ITCM_AREA_CODE	uint32_t FindFreeOscillator(void)
{
uint32_t		osc_number;
uint8_t			osc_ret = 0;
uint32_t		oldest_osc=0;

	for(osc_number=0;osc_number<NUMOSCILLATORS;osc_number+=VOICES)
	{
		if ( Oscillator[osc_number].state == OSC_OFF )
		{
			return osc_number;
		}
		else
		{
			if ( Oscillator[osc_number].oscillator_age > oldest_osc )
			{
				oldest_osc = Oscillator[osc_number].oscillator_age;
				osc_ret = osc_number;
			}
		}
	}
	oldest_osc=0;
	return osc_ret;
}

ITCM_AREA_CODE	void DisableOscillator(uint16_t channel, uint16_t midi_note , uint8_t velocity)
{
uint8_t	osc_number;
	for(osc_number=0;osc_number<NUMOSCILLATORS;osc_number++)
	{
		if (( Oscillator[osc_number].midi_note == midi_note ) && ( Oscillator[osc_number].state == OSC_ON ))
		{
			Oscillator[osc_number].state = OSC_GO_OFF;
		}
	}
}

ITCM_AREA_CODE	void DisableAllOscillator(void)
{
uint8_t	osc_number;
	for(osc_number=0;osc_number<NUMOSCILLATORS;osc_number++)
	{
		Oscillator[osc_number].state = OSC_GO_OFF;
	}
}

extern	float	midi_freq[128];
float delta_phase_k =  (float )WAVETABLE_SIZE / (float )SAMPLE_FREQUENCY;

ITCM_AREA_CODE	void EnableOscillator(uint16_t channel, uint16_t midi_note , uint8_t velocity)
{
float	delta_phase;
float	freq;
uint32_t	osc_number,i;

	osc_number = FindFreeOscillator();
	for(i=0 ; i< VOICES; i++)
	{
		freq = midi_freq[Oscillator[osc_number].midi_note] + Oscillator[osc_number].detune;
		delta_phase = (float )WAVETABLE_SIZE / ((float )SAMPLE_FREQUENCY / freq);
		Oscillator[osc_number+i].delta_phase = (uint16_t )(delta_phase * (float )INT_PRECISION);

		Oscillator[osc_number+i].current_phase = 0;
		Oscillator[osc_number+i].midi_note = midi_note;
		Oscillator[osc_number+i].state = OSC_ON;
		Oscillator[osc_number+i].oscillator_age = 0;
		Oscillator[osc_number+i].volume = ((float )AudioFlags.osc_volume[i] / 10.0F);
		/* TEST */
		Oscillator[osc_number+i].volume = ((float )1);
		/* TEST END */

		switch(AudioFlags.osc_waves[i])
		{
		case	TRIANGLE 	: Oscillator[osc_number+i].waveform = TRIANGLE; break;
		case	SQUARE 		: Oscillator[osc_number+i].waveform = SQUARE; break;
		case	NOISE 		: Oscillator[osc_number+i].waveform = NOISE; break;
		default 			: Oscillator[osc_number+i].waveform = SINE; break;
		}
	}
}

void InitOscillators(void)
{
float	freq,delta_phase;
uint16_t	osc_number;

	for(osc_number=0;osc_number<NUMOSCILLATORS;osc_number++)
	{
		Oscillator[osc_number].current_phase = 0.0f;
		Oscillator[osc_number].detune = 0.0f;
		Oscillator[osc_number].state = OSC_OFF;
		Oscillator[osc_number].waveform = SINE;
		Oscillator[osc_number].midi_note = INVALID_MIDI_NOTE;
		Oscillator[osc_number].duty = 50*DUTY_SCALE;
		Oscillator[osc_number].volume = 1.0F;
		freq = midi_freq[69] + Oscillator[osc_number].detune;
		delta_phase = (float )WAVETABLE_SIZE / ((float )SAMPLE_FREQUENCY / freq);
		Oscillator[osc_number].delta_phase = (uint16_t )(delta_phase * (float )INT_PRECISION);
		Oscillator[osc_number].delta_phase = 0;
	}
	InitOscillatorsTables();
}

#endif

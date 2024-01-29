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
 * oscillator_core.c
 *
 *  Created on: Jan 8, 2024
 *      Author: fil
 */
#include "main.h"
#include "../../kernel/system_default.h"	/* for BOARD_NAME variable only */
#include "../../kernel/audio.h"				/* for audio parameters */
#include "../../kernel/kernel_opt.h"

#ifdef SYNTH_ENGINE_ENABLE

#include "oscillators.h"
#include "oscillator_core.h"
#include "noise.h"

extern OscillatorsTypeDef	Oscillator[NUMOSCILLATORS];
extern	float	delta_k_phase;

__attribute__((section(".table"))) __attribute__ ((aligned (32))) const int16_t rom_osc_sine_tab[WAVETABLE_SIZE] =
{
		0,402,803,1205,1605,2005,2403,2800,3196,3589,3980,4369,4755,5139,5519,5896,
		6269,6639,7004,7365,7722,8075,8422,8764,9101,9433,9759,10079,10393,10700,11002,11296,
		11584,11865,12139,12405,12664,12915,13158,13394,13621,13841,14052,14254,14448,14633,14810,14977,
		15135,15285,15425,15556,15677,15789,15892,15984,16068,16141,16205,16259,16304,16338,16363,16378,
		16383,16378,16363,16338,16304,16259,16205,16141,16068,15984,15892,15789,15677,15556,15425,15285,
		15135,14977,14810,14633,14448,14254,14052,13841,13621,13394,13158,12915,12664,12405,12139,11865,
		11584,11296,11002,10700,10393,10079,9759,9433,9101,8764,8422,8075,7722,7365,7004,6639,
		6269,5896,5519,5139,4755,4369,3980,3589,3196,2800,2403,2005,1605,1205,803,402,
		0,-402,-803,-1205,-1605,-2005,-2403,-2800,-3196,-3589,-3980,-4369,-4755,-5139,-5519,-5896,
		-6269,-6639,-7004,-7365,-7722,-8075,-8422,-8764,-9101,-9433,-9759,-10079,-10393,-10700,-11002,-11296,
		-11584,-11865,-12139,-12405,-12664,-12915,-13158,-13394,-13621,-13841,-14052,-14254,-14448,-14633,-14810,-14977,
		-15135,-15285,-15425,-15556,-15677,-15789,-15892,-15984,-16068,-16141,-16205,-16259,-16304,-16338,-16363,-16378,
		-16383,-16378,-16363,-16338,-16304,-16259,-16205,-16141,-16068,-15984,-15892,-15789,-15677,-15556,-15425,-15285,
		-15135,-14977,-14810,-14633,-14448,-14254,-14052,-13841,-13621,-13394,-13158,-12915,-12664,-12405,-12139,-11865,
		-11584,-11296,-11002,-10700,-10393,-10079,-9759,-9433,-9101,-8764,-8422,-8075,-7722,-7365,-7004,-6639,
		-6269,-5896,-5519,-5139,-4755,-4369,-3980,-3589,-3196,-2800,-2403,-2005,-1605,-1205,-803,-402,
};

OSCILLATORS_RAM	__attribute__ ((aligned (16)))	int32_t		osc_buffer[HALF_NUMBER_OF_AUDIO_SAMPLES];
OSCILLATORS_RAM	__attribute__ ((aligned (16)))	int16_t		oscout_buffer[HALF_NUMBER_OF_AUDIO_SAMPLES];
OSCILLATORS_RAM	__attribute__ ((aligned (16)))	int32_t		osc_buffer_gen[HALF_NUMBER_OF_AUDIO_SAMPLES];
OSCILLATORS_RAM	__attribute__ ((aligned (16)))	int16_t 	osc_sine_tab[WAVETABLE_SIZE];
OSCILLATORS_RAM	__attribute__ ((aligned (16)))	int16_t		osc_square_tab[WAVETABLE_SIZE];
OSCILLATORS_RAM	__attribute__ ((aligned (16)))	int16_t		osc_tri_tab[WAVETABLE_SIZE];
OSCILLATORS_RAM	__attribute__ ((aligned (16)))	int16_t		osc_noise_tab[WAVETABLE_SIZE];

__attribute__((section(".table"))) __attribute__ ((aligned (32))) const float	rom_midi_freq[MIDI_NOTES] =
{
		8.176,
		8.662,
		9.177,
		9.723,
		10.301,
		10.913,
		11.562,
		12.250,
		12.978,
		13.750, /* # 1 */
		14.568,
		15.434,
		16.352,
		17.324,
		18.354,
		19.445,
		20.601,
		21.826,
		23.124,
		24.499,
		25.956,
		27.500, /* # 2 */
		29.135,
		30.867,
		32.703,
		34.648,
		36.708,
		38.890,
		41.203,
		43.653,
		46.249,
		48.999,
		51.913,
		55.000, /* # 3 */
		58.270,
		61.735,
		65.406,
		69.295,
		73.416,
		77.781,
		82.406,
		87.307,
		92.499,
		97.998,
		103.82,
		110.00, /* # 4 */
		116.54,
		123.47,
		130.81,
		138.59,
		146.83,
		155.56,
		164.81,
		174.61,
		184.99,
		195.99,
		207.65,
		220.00, /* # 5 */
		233.08,
		246.94,
		261.63,
		277.18,
		293.66,
		311.13,
		329.63,
		349.23,
		369.99,
		391.99,
		415.31,
		440.00, /* # 6 */
		466.16,
		489.88,
		523.25,
		554.37,
		587.33,
		622.25,
		659.26,
		698.46,
		739.99,
		783.99,
		830.61,
		880.00, /* # 7 */
		932.32,
		987.77,
		1046.5,
		1108.7,
		1174.7,
		1244.5,
		1318.5,
		1396.9,
		1480.0,
		1568.0,
		1661.2,
		1760.0, /* # 8 */
		1864.7,
		1975.5,
		2093.0,
		2217.5,
		2349.3,
		2489.0,
		2637.0,
		2793.8,
		2960.0,
		3136.0,
		3322.4,
		3520.0, /* # 9 */
		3729.3,
		3951.1,
		4186.0,
		4434.9,
		4698.6,
		4978.0,
		5274.0,
		5587.7,
		5919.9,
		6271.9,
		6644.9,
		7040.0, /* # 10 */
		7458.6,
		7902.1,
		8372.0,
		8869.8,
		9397.3,
		9956.1,
		10548.1,
		11175.3,
		11839.8,
		12543.9
};
float	midi_freq[MIDI_NOTES];

ITCM_AREA_CODE	void RunOscillator32(void)
{
uint16_t	i;
uint8_t		angle,osc_number;

	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		osc_buffer[i] = 0;
	for(osc_number=0;osc_number<NUMOSCILLATORS;osc_number++)
	{
		if ( Oscillator[osc_number].state != OSC_OFF )
		{
			Oscillator[osc_number].oscillator_age++;
			for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			{
				angle = (uint8_t )(Oscillator[osc_number].current_phase >> 8);

				osc_buffer_gen[i] = Oscillator[osc_number].wave[angle];
				Oscillator[osc_number].current_phase += Oscillator[osc_number].delta_phase;
				// zero crossing
				if ( Oscillator[osc_number].state == OSC_GO_OFF)
				{
					if ( (i > 1 ) && (i < (HALF_NUMBER_OF_AUDIO_SAMPLES -1 )))
					{
						if ( (( osc_buffer_gen[i-1] > 0 ) && (osc_buffer_gen[i] <0)) || (( osc_buffer_gen[i-1] < 0 ) && (osc_buffer_gen[i] > 0)) )
						{
							Oscillator[osc_number].state = OSC_OFF;
						}
					}
				}
				if ( Oscillator[osc_number].state == OSC_OFF)
					osc_buffer_gen[i] = 0;
				// zero crossing end

				osc_buffer[i] += (int32_t )( (float )osc_buffer_gen[i] * Oscillator[osc_number].volume);
				/*
				if ( Oscillator[osc_number].midi_note != INVALID_MIDI_NOTE)
				{
					if (( AudioFlags.oscillator_flags & OSC_TUNE_PENDING ) == OSC_TUNE_PENDING)
					{
						float	freq;
						float	delta_phase;
						freq = midi_freq[Oscillator[osc_number].midi_note] + Oscillator[osc_number].detune;
						delta_phase = (float )WAVETABLE_SIZE / ((float )SAMPLE_FREQUENCY / freq);
						Oscillator[osc_number].delta_phase = (uint16_t )(delta_phase * (float )INT_PRECISION);
					}
				}
				*/
			}
		}
	}
	//AudioFlags.oscillator_flags &= ~OSC_TUNE_PENDING;
	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		oscout_buffer[i] = osc_buffer[i] >> (VOICES / 4);
}

ITCM_AREA_CODE	void ChangeOscillatorWavePhase(uint16_t	osc_number ,uint8_t waveform )
{
	switch (waveform)
	{
	case	SINE:
		Oscillator[osc_number & 0x03].wave = osc_sine_tab;
		break;
	case	TRIANGLE:
		Oscillator[osc_number & 0x03].wave = osc_tri_tab;
		break;
	case	SQUARE:
		Oscillator[osc_number & 0x03].wave = osc_square_tab;
		break;
	case	NOISE:
		Oscillator[osc_number & 0x03].wave = osc_noise_tab;
		break;
	default :
		break;
	}
}

void InitOscillatorsTables(void)
{
uint16_t	i,tri_up = 1;
int16_t		tri_delta = MIN_SINEVAL;

	// 1 setup midi table conversion
	for(i=0;i<MIDI_NOTES;i++)
		midi_freq[i] = rom_midi_freq[i];
	// 2 setup tables in DTCMRAM
	for(i=0;i<NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		// 2.1 sine
		osc_sine_tab[i] = rom_osc_sine_tab[i];
		// 2.2 square
		osc_square_tab[i] = MAX_SINEVAL;
		if ( i < AUDIO_BUF_SIZE/2 )
			osc_square_tab[i] = MIN_SINEVAL;
		// 2.3 triangle
		osc_tri_tab[i] = tri_delta;
		if ( tri_up == 1 )
		{
			tri_delta += MAX_SINEVAL /  (WAVETABLE_SIZE/4);
			if ( tri_delta >= MAX_SINEVAL)
				tri_up = 0;
		}
		else
			tri_delta -= MAX_SINEVAL /  (WAVETABLE_SIZE/4);
	}
	// 2.4 create noise table
	Noise_Gen(osc_noise_tab);
	// 3 setup default table -> sine
	for(i=0;i<NUMOSCILLATORS;i++)
	{
		Oscillator[i].wave = osc_sine_tab;
	}
}

#endif

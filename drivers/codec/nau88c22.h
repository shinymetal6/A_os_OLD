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
 * nau88c22.h
 *
 *  Created on: Jan 4, 2024
 *      Author: fil
 */

#ifndef DRIVERS_CODEC_NAU88C22_H_
#define DRIVERS_CODEC_NAU88C22_H_

typedef struct
{
	uint8_t		reg_addr;
	uint16_t	reg_data;
}Nau88c22_t;

#define NAU88C22_RESET						0x00
#define NAU88C22_POWER_MANAGEMENT_1			0x01
#define NAU88C22_POWER_MANAGEMENT_2			0x02
#define NAU88C22_POWER_MANAGEMENT_3			0x03
#define NAU88C22_AUDIO_INTERFACE			0x04
#define NAU88C22_COMPANDING_CONTROL			0x05
#define NAU88C22_CLOCK_CONTROL1				0x06
#define NAU88C22_CLOCK_CONTROL2				0x07
#define NAU88C22_GPIO_CONTROL				0x08
#define NAU88C22_JACK_DETECT_CONTROL_1		0x09
#define NAU88C22_DAC_CONTROL				0x0A
#define NAU88C22_LEFT_DAC_DIGITAL_VOLUME	0x0B
#define NAU88C22_RIGHT_DAC_DIGITAL_VOLUME	0x0C
#define NAU88C22_JACK_DETECT_CONTROL_2		0x0D
#define NAU88C22_ADC_CONTROL				0x0E
#define NAU88C22_LEFT_ADC_DIGITAL_VOLUME	0x0F
#define NAU88C22_RIGHT_ADC_DIGITAL_VOLUME	0x10
#define NAU88C22_EQ1						0x12
#define NAU88C22_EQ2						0x13
#define NAU88C22_EQ3						0x14
#define NAU88C22_EQ4						0x15
#define NAU88C22_EQ5						0x16
#define NAU88C22_DAC_LIMITER_1				0x18
#define NAU88C22_DAC_LIMITER_2				0x19
#define NAU88C22_NOTCH_FILTER_1				0x1b
#define NAU88C22_NOTCH_FILTER_2				0x1c
#define NAU88C22_NOTCH_FILTER_3				0x1d
#define NAU88C22_NOTCH_FILTER_4				0x1e
#define NAU88C22_ALC_CONTROL_1				0x20
#define NAU88C22_ALC_CONTROL_2				0x21
#define NAU88C22_ALC_CONTROL_3				0x22
#define NAU88C22_NOISE_GATE					0x23
#define NAU88C22_PLL_N						0x24
#define NAU88C22_PLL_K1						0x25
#define NAU88C22_PLL_K2						0x26
#define NAU88C22_PLL_K3						0x27
#define NAU88C22_3D_CONTROL					0x29
#define NAU88C22_BEEP_CONTROL				0x2b
#define NAU88C22_INPUT_CONTROL				0x2c
#define NAU88C22_LEFT_INP_PGA_CONTROL		0x2d
#define NAU88C22_RIGHT_INP_PGA_CONTROL		0x2e
#define NAU88C22_LEFT_ADC_BOOST_CONTROL		0x2f
#define NAU88C22_RIGHT_ADC_BOOST_CONTROL	0x30
#define NAU88C22_OUTPUT_CONTROL				0x31
#define NAU88C22_LEFT_MIXER_CONTROL			0x32
#define NAU88C22_RIGHT_MIXER_CONTROL		0x33
#define NAU88C22_LOUT1_HP_CONTROL			0x34
#define NAU88C22_ROUT1_HP_CONTROL			0x35
#define NAU88C22_LOUT2_SPK_CONTROL			0x36
#define NAU88C22_ROUT2_SPK_CONTROL			0x37
#define NAU88C22_OUT2_MIXER_CONTROL			0x38
#define NAU88C22_OUT1_MIXER_CONTROL			0x39

#define NAU88C22_NUM_REGS					79

typedef struct
{
	uint16_t	adc_dac_path;
	uint8_t		band1_frequency;
	uint8_t		band1_gain;
	uint16_t	band2_narrow_wide;
	uint8_t		band2_frequency;
	uint8_t		band2_gain;
	uint16_t	band3_narrow_wide;
	uint8_t		band3_frequency;
	uint8_t		band3_gain;
	uint16_t	band4_narrow_wide;
	uint8_t		band4_frequency;
	uint8_t		band4_gain;
	uint8_t		band5_frequency;
	uint8_t		band5_gain;
}Nau88c22_equalizer_t;

extern	void Nau88c22_Init(void);
extern	void Nau88c22_SetHPVolume(uint8_t volume);
extern	uint8_t Nau88c22_SetBand1Equalizer(uint8_t adc_dac,    uint8_t band,uint8_t center_frequency,uint8_t gain);
extern	uint8_t Nau88c22_SetBand2Equalizer(uint8_t narrow_wide,uint8_t band,uint8_t center_frequency,uint8_t gain);
extern	uint8_t Nau88c22_SetBand3Equalizer(uint8_t narrow_wide,uint8_t band,uint8_t center_frequency,uint8_t gain);
extern	uint8_t Nau88c22_SetBand4Equalizer(uint8_t narrow_wide,uint8_t band,uint8_t center_frequency,uint8_t gain);
extern	uint8_t Nau88c22_SetBand5Equalizer(                    uint8_t band,uint8_t center_frequency,uint8_t gain);
extern	uint8_t Nau88c22_WriteEqualizer(void);

#endif /* DRIVERS_CODEC_NAU88C22_H_ */

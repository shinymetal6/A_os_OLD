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
 * nau88c22.c
 *
 *  Created on: Jan 3, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"
extern	void task_delay(uint32_t tick_count);

#ifdef CODEC_NAU88C22
#include "nau88c22.h"
Nau88c22_equalizer_t	Nau88c22_equalizer;

uint16_t	nau88c22_shadowregs[NAU88C22_NUM_REGS];
Nau88c22_t	Nau88c22[] =
{
		{
			NAU88C22_POWER_MANAGEMENT_1,/// pll off
			0x01ff
		},
		{
			NAU88C22_POWER_MANAGEMENT_2,
			//0x01b3	// Enable L/R Headphone, ADC Mix/Boost, ADC
			0x01bf		// Disable L/R Headphone, ADC Mix/Boost, ADC
		},
		{
			NAU88C22_POWER_MANAGEMENT_3,
			//0x007f	// Enable L/R main mixer, DAC
			0x01ff	// Enable L/R main mixer, DAC  - disable speaker and AUX
		},
		{
			NAU88C22_AUDIO_INTERFACE,
			0x0010	// 16-bit word length, I2S format, Stereo 000010000
			//0x0008	// 16-bit word length, I2S format, Stereo 000010000
		},
		{
			NAU88C22_COMPANDING_CONTROL,
			0x0000	// Companding control and loop back mode (all disable)
		},
		{
			NAU88C22_CLOCK_CONTROL1,
			//0x01ad	 // Divide by 6, 16K
			0x0100	 // Divide by 1 master clock source, fs and bclk are input
		},
		{
			NAU88C22_CLOCK_CONTROL2,
			//0x0006	// 16K for internal filter coefficients
			0x0000	// 48K
		},
		{
			NAU88C22_GPIO_CONTROL,
			0x0000	// gpio <- all in
			//0x0004	// gpio <- pll out
			//0x0005	// gpio <- pll lock
		},
		{
			NAU88C22_DAC_CONTROL,
			0x0008	// DAC soft mute is disabled, DAC oversampling rate is 128x
		},
		{
			NAU88C22_LEFT_DAC_DIGITAL_VOLUME,
			0x01ff	// DAC left digital volume control
		},
		{
			NAU88C22_RIGHT_DAC_DIGITAL_VOLUME,
			0x01ff	// DAC right digital volume control
		},
		{
			NAU88C22_ADC_CONTROL,
			0x0108	// ADC HP filter is disabled, ADC oversampling rate is 128x
		},
		{
			NAU88C22_LEFT_ADC_DIGITAL_VOLUME,
			0x01ff	// ADC left digital volume control
		},
		{
			NAU88C22_RIGHT_ADC_DIGITAL_VOLUME,
			0x01ff	// ADC right digital volume control
		},
		{
			NAU88C22_INPUT_CONTROL,
			//0x0000	// LLIN/RLIN is not connected to PGA
			0x0044	// only LLIN/RLIN connected to PGA -> 001000100
		},
		{
			NAU88C22_LEFT_ADC_BOOST_CONTROL,
			//0x0050	// LLIN connected, and its Gain value
			0x0055	// LLIN connected, 0 dB all 001010101
		},
		{
			NAU88C22_RIGHT_ADC_BOOST_CONTROL,
			//0x0050	 // RLIN connected, and its Gain value
			0x0055	// RLIN connected, 0 dB all 001010101
		},
		{
			NAU88C22_OUTPUT_CONTROL,
			0x0002	// Thermal shutdown enable
		},
		{
			NAU88C22_LEFT_MIXER_CONTROL,
			0x0001	// Left DAC connected to LMIX
		},
		{
			NAU88C22_RIGHT_MIXER_CONTROL,
			0x0001	// Right DAC connected to RMIX
		},
		{
			NAU88C22_OUT2_MIXER_CONTROL,
			0x0001	// Left DAC connected to LMIX
		},
		{
			NAU88C22_OUT1_MIXER_CONTROL,
			0x0001	// Right DAC connected to RMIX
		},
#ifdef EXT_FREQ_12MHZ
		/* 12 MHz ext clock */
		/*
			R = 98.304/12.000 = 8.192 --> 0x3126E9
			R36 0xnm8 ; integer portion of fraction, (nm represents other settings in R36)
			R37 0x00C ; highest order 6-bits of 24-bit fraction
			R38 0x093 ; middle 9-bits of 24-bit fraction
			R39 0x0E9 ; lowest order 9-bits of 24-bit fraction
		*/
		{
			NAU88C22_PLL_N,
			0x008	// 256fs IMCLK rate : 12.288Mhz internal / 12.0 Mhz external ref clk
		},
		{
			NAU88C22_PLL_K1,
			0x00c	//
		},
		{
			NAU88C22_PLL_K2,
			0x093	//
		},
		{
			NAU88C22_PLL_K3,
			0x0e9	//
		},
		{
			NAU88C22_RESET,
			0	// struct_terminator
		},
#endif
#ifdef EXT_FREQ_12_288MHZ
		/* 12.288 MHz ext clock */
		/*
			R = 98.304/12.288 = 8 --> 0
			R36 0xnm8 ; integer portion of fraction, (nm represents other settings in R36)
			R37 0x000 ; highest order 6-bits of 24-bit fraction
			R38 0x000 ; middle 9-bits of 24-bit fraction
			R39 0x000 ; lowest order 9-bits of 24-bit fraction
		*/
		{
			NAU88C22_PLL_N,
			0x008	// 256fs IMCLK rate : 12.288Mhz internal / 12.288Mhz external ref clk
		},
		{
			NAU88C22_PLL_K1,
			0x000	//
		},
		{
			NAU88C22_PLL_K2,
			0x000	//
		},
		{
			NAU88C22_PLL_K3,
			0x000	//
		},
#endif
		{
			NAU88C22_RESET,
			0	// struct_terminator
		},
};

uint8_t Nau88c22_CheckPresent(void)
{
	return HAL_I2C_IsDeviceReady(&NAU88C22_I2C, NAU88C22_ADDR, 5, NAU88C22_TIMEOUT);
}

void Nau88c22_WriteReg(uint8_t reg_address, uint16_t reg_data)
{
uint8_t i2c_data[2];
	nau88c22_shadowregs[reg_address] = reg_data;
	i2c_data[0] = (reg_address << 1) | ((reg_data & 0x100 )>> 8);
	i2c_data[1] = reg_data & 0xff;
	HAL_I2C_Mem_Write(&NAU88C22_I2C, NAU88C22_ADDR, i2c_data[0], 1, &i2c_data[1], 1, NAU88C22_TIMEOUT);
}

uint16_t Nau88c22_ReadReg(uint8_t reg_address)
{
	return nau88c22_shadowregs[reg_address];
}

void Nau88c22_Init(void)
{
uint8_t	i = 0;
	if ( Nau88c22_CheckPresent() == 0)
	{
		task_delay(20);
		Nau88c22_WriteReg(NAU88C22_RESET,  0);	// reset
		task_delay(20);
		while(Nau88c22[i].reg_addr != NAU88C22_RESET )
		{
			Nau88c22_WriteReg(Nau88c22[i].reg_addr,  Nau88c22[i].reg_data);
			i++;
		}
		task_delay(1);
		Nau88c22_WriteReg(NAU88C22_POWER_MANAGEMENT_1,  0x002f);
	}
}

void Nau88c22_SetHPVolume(uint8_t volume)
{
uint16_t		reg = 0x100 | (volume & 0x3f);
	Nau88c22_WriteReg(NAU88C22_LOUT1_HP_CONTROL,  reg);
	Nau88c22_WriteReg(NAU88C22_ROUT1_HP_CONTROL, reg);
}

uint8_t Nau88c22_SetBand1Equalizer(uint8_t adc_dac,uint8_t band,uint8_t center_frequency,uint8_t gain)
{
	if ( ( band > 4 ) || ( gain > 0x18 ) || ( center_frequency > 3 ))
		return 1;
	Nau88c22_equalizer.adc_dac_path = (adc_dac & 0x01) << 8;
	Nau88c22_equalizer.band1_frequency = center_frequency << 5;
	Nau88c22_equalizer.band1_gain = gain;
	return 0;
}

uint8_t Nau88c22_SetBand2Equalizer(uint8_t narrow_wide,uint8_t band,uint8_t center_frequency,uint8_t gain)
{
	if ( ( band > 4 ) || ( gain > 0x18 ) || ( center_frequency > 3 ))
		return 1;
	Nau88c22_equalizer.band2_narrow_wide = (narrow_wide & 0x01) << 8;
	Nau88c22_equalizer.band2_frequency = center_frequency << 5;
	Nau88c22_equalizer.band2_gain = gain;
	return 0;
}

uint8_t Nau88c22_SetBand3Equalizer(uint8_t narrow_wide,uint8_t band,uint8_t center_frequency,uint8_t gain)
{
	if ( ( band > 4 ) || ( gain > 0x18 ) || ( center_frequency > 3 ))
		return 1;
	Nau88c22_equalizer.band3_narrow_wide = (narrow_wide & 0x01) << 8;
	Nau88c22_equalizer.band3_frequency = center_frequency << 5;
	Nau88c22_equalizer.band3_gain = gain;
	return 0;
}

uint8_t Nau88c22_SetBand4Equalizer(uint8_t narrow_wide,uint8_t band,uint8_t center_frequency,uint8_t gain)
{
	if ( ( band > 4 ) || ( gain > 0x18 ) || ( center_frequency > 3 ))
		return 1;
	Nau88c22_equalizer.band4_narrow_wide = (narrow_wide & 0x01) << 8;
	Nau88c22_equalizer.band4_frequency = center_frequency << 5;
	Nau88c22_equalizer.band4_gain = gain;
	return 0;
}

uint8_t Nau88c22_SetBand5Equalizer(uint8_t band,uint8_t center_frequency,uint8_t gain)
{
	if ( ( band > 4 ) || ( gain > 0x18 ) || ( center_frequency > 3 ))
		return 1;
	Nau88c22_equalizer.band5_frequency = center_frequency << 5;
	Nau88c22_equalizer.band5_gain = gain;
	return 0;
}

uint8_t Nau88c22_WriteEqualizer(void)
{
	Nau88c22_WriteReg(NAU88C22_EQ1,  Nau88c22_equalizer.adc_dac_path |      Nau88c22_equalizer.band1_frequency | Nau88c22_equalizer.band1_gain);
	Nau88c22_WriteReg(NAU88C22_EQ2,  Nau88c22_equalizer.band2_narrow_wide | Nau88c22_equalizer.band2_frequency | Nau88c22_equalizer.band2_gain);
	Nau88c22_WriteReg(NAU88C22_EQ3,  Nau88c22_equalizer.band3_narrow_wide | Nau88c22_equalizer.band3_frequency | Nau88c22_equalizer.band3_gain);
	Nau88c22_WriteReg(NAU88C22_EQ4,  Nau88c22_equalizer.band4_narrow_wide | Nau88c22_equalizer.band4_frequency | Nau88c22_equalizer.band4_gain);
	Nau88c22_WriteReg(NAU88C22_EQ5,                                         Nau88c22_equalizer.band5_frequency | Nau88c22_equalizer.band5_gain);
	return 0;
}

#endif	//#ifdef CODEC_NAU88C22


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
 * lcd_7735.c
 *
 *  Created on: Jan 3, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"

#ifdef	LCD_096_ENABLED
extern	void task_delay(uint32_t tick_count);

#include "lcd_7735.h"
#include "st7735.h"

Video_t	Video;

#define VMARGIN 1
#define HMARGIN (12*7)-1

uint8_t	horizontal_line_space;
uint8_t	vertical_line_space;
uint8_t	current_highlight_line;
#define	NUMLINES	7

extern	uint16_t logo[];

void LcdSetBrightness(uint16_t brightness)
{
	if ( brightness <= FULL_BRIGHTNESS)
	{
		BACKLIGHT_TIMER.Instance->CCR1 = brightness;
		Video.current_brightness = brightness;
	}
}

void LcdInit(void)
{
    LcdSetBrightness(ZERO_BRIGHTNESS);
	HAL_TIM_PWM_Start(&BACKLIGHT_TIMER,TIM_CHANNEL_1);
    ST7735_Unselect();
	ST7735_Init();
	ST7735_ClearScreen();
}

void Draw_Logo(uint16_t *logo)
{
	ST7735_DrawImage(0,0,ST7735_WIDTH, ST7735_HEIGHT,logo);
}

void LcdClearScreen(void)
{
	//ST7735_FillScreen(ST7735_BLACK);
	ST7735_ClearScreen();
}
#endif

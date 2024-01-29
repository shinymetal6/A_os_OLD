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
 * system_functions.c
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */


#include "main.h"
#include "kernel_opt.h"

uint32_t A_bit_index_to_num(uint32_t bit_index )
{
uint8_t shft = 0;
	while(((bit_index >> shft) & 1) == 0)
		shft++;
	return shft;
}

void A_bzero(uint8_t *ptr,uint16_t count)
{
uint32_t	*ptr_32 = (uint32_t *)ptr;
	while ( count > 3 )
	{
		*ptr_32++ = 0;
		count -= 4;
	}

	while(count)
	{
		*ptr++ = 0;
		count--;
	}
}

void A_memcpy(uint8_t *dest,uint8_t *source,uint16_t size)
{
	uint32_t	*dest_32 = (uint32_t *)dest;
	uint32_t	*source_32 = (uint32_t *)source;
	while(size > 3)
	{
		*dest_32++=*source_32++;
		size-=4;
	}
	while(size)
	{
		*dest++=*source++;
		size--;
	}
}

void A_bmemcpy(uint8_t *dest,uint8_t *source,uint16_t size)
{
	while(size--)
		*dest++=*source++;
}

uint32_t time_start;
uint32_t usec_elapsed;
void A_get_timelapse_start(void)
{
	time_start = DWT->CYCCNT;
}

uint32_t A_get_timelapse_end(void)
{
    usec_elapsed = (DWT->CYCCNT - time_start)/480;
    return	usec_elapsed;
}

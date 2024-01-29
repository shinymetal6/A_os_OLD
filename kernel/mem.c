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
 * mem.c
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */


#include "main.h"
#include "A.h"
#include "system_default.h"
#include "scheduler.h"
#include "kernel_opt.h"

extern	MEMpool_t	MEMpool[POOL_NUM];
extern	Asys_t		Asys;

void A_mem_init(void)
{
uint32_t	i;
uint8_t		*mem_ptr;
MEMpool_t	*p = MEMpool;

	mem_ptr = (uint8_t *)POOL_END;
	*mem_ptr++ = 0xde;
	*mem_ptr++ = 0xad;
	*mem_ptr++ = 0xbe;
	*mem_ptr++ = 0xef;
	mem_ptr = (uint8_t *)POOL_START;
	Asys.first_data_address= (uint32_t )POOL_START;
	Asys.first_mem = (uint8_t *)&p[0];
	for(i=0;i<POOL_NUM;i++)
	{
		p[i].nxt_link = (uint8_t *)&p[i+1];
		p[i].pre_link = (i > 0) ? (uint8_t *)&p[i-1] : 0;
		p[i].mem_ptr = mem_ptr;
		p[i].chunk_count = 0;
		p[i].chunk_index = 0;
		p[i].process = p[i].flags = 0;
		mem_ptr += POOL_CHUNK_SIZE;
	}
	p[i-1].nxt_link = 0;
}

void reset_orphaned_chunks(uint8_t process)
{
MEMpool_t	*p = MEMpool;
uint32_t	i;
	__disable_irq();
	for(i=0;i<POOL_NUM;i++)
	{
		if ( p[i].process == process )
		{
			p[i].nxt_link = Asys.first_mem;
			p[i].pre_link = 0;
			p[i].chunk_count = 0;
			p[i].chunk_index = 0;
			p[i].process = p[i].flags = 0;
			Asys.first_mem = (uint8_t *)&p[i];
			Asys.num_buf_in_use--;
		}
	}
	__enable_irq();
}

void defrag_mem(void)
{
uint32_t	i;
uint8_t		*mem_ptr=(uint8_t *)POOL_START;
MEMpool_t	*p = MEMpool;

	__disable_irq();
#ifdef	DEBUG_MEM_DEFRAG_ON_PG6
	A_Debug_Set_One();
#endif
	Asys.first_mem = (uint8_t *)&p[0];
	for(i=0;i<POOL_NUM;i++)
	{
		p[i].nxt_link = (uint8_t *)&p[i+1];
		p[i].pre_link = (i > 0) ? (uint8_t *)&p[i-1] : 0;
		mem_ptr += POOL_CHUNK_SIZE;
	}
	p[i-1].nxt_link = 0;
	Asys.system_flags &= ~SYS_MEM_DEFRAG_REQUEST;
#ifdef	DEBUG_MEM_DEFRAG_ON_PG6
	A_Debug_Set_Zero();
#endif
	__enable_irq();
}

uint32_t	f_error = 0;
void find_error(void)
{
	f_error++;
}

uint16_t find_chunk_by_size(uint16_t size)
{
MEMpool_t	*p = (MEMpool_t *)Asys.first_mem;
uint16_t	num_buf,i;

	num_buf = size / POOL_CHUNK_SIZE;
	if ( size > (num_buf*POOL_CHUNK_SIZE))
		num_buf += 1;
	Asys.first_of_list = Asys.last_of_list = p;
	i = num_buf;
	for(i=0;i<num_buf;i++)
	{
		if(p->process != 0)
		{
			i = 0;
			p++;
			Asys.first_of_list = Asys.last_of_list = p;
		}
		else
			Asys.last_of_list++;
	}
	Asys.last_of_list--;

	if (( Asys.first_of_list < (MEMpool_t 	*)SRAM_START ) ||(Asys.last_of_list > (MEMpool_t 	*)SRAM_END) )
		find_error();

	return num_buf;
}


// warning : max 256 buffers of 256 byte
uint8_t *mem_get(uint32_t size )
{
MEMpool_t	*p,*working_ptr;
uint16_t	number_of_chunks,i;

	__disable_irq();
	// 1 . find a suitable chunk in memory
	number_of_chunks = find_chunk_by_size(size);
	// 2. if is the first in buffer pool update Asys.first_mem
	if ( Asys.first_of_list->pre_link == 0 )
	{
		Asys.first_mem = Asys.last_of_list->nxt_link;
		//2.1 clear pre_link of the first buffer
		p = (MEMpool_t *)Asys.last_of_list->nxt_link;
		p->pre_link = 0;
		//2.2 clear nxt_link of the last buffer
		Asys.last_of_list->nxt_link = 0;
	}
	else
	{
		//3. we are inside the free list and not pointed by Asys.first_mem:
		//		update the pre-link pointed buffer to the next_link of the last buffer
		//3.1 point to prevoius buffer in list
		p = (MEMpool_t *)Asys.first_of_list->pre_link;
		//3.2 update nxt_link to the nxt_link of the last buffer in the chunk
		p->nxt_link = (uint8_t *)Asys.last_of_list;
		//3.3 clear the pre_link of the first buffer in chunk
		Asys.first_of_list->pre_link = 0;
	}
	//4. now we have the chunk isolated from the list, pointed by first_of_list
	working_ptr = Asys.first_of_list;
	for(i=0;i<number_of_chunks;i++)
	{
		working_ptr->chunk_index = i;
		working_ptr->chunk_count = number_of_chunks;
		working_ptr->process = Asys.current_process;
		working_ptr->flags = MEM_IN_USE;
		working_ptr++;
		Asys.num_buf_in_use++;
	}
	Asys.system_flags |= SYS_MEM_DEFRAG_REQUEST;
	__enable_irq();
	return Asys.first_of_list->mem_ptr;
}

uint32_t	mem_addr_val;

uint32_t mem_release(uint8_t *data_ptr)
{
MEMpool_t	*p,*first_list_pool,*insert_pool;
uint8_t		chunk_count,i;

	__disable_irq();
	mem_addr_val = ((uint32_t ) data_ptr - Asys.first_data_address) / POOL_CHUNK_SIZE;
	p = (MEMpool_t *)&MEMpool[mem_addr_val];

	// 1. check if single buffer or linked list buffers
	if ( p->chunk_count == 1 )
	{
		//1.1 insert the buffer as first free buffer
		//1.1.1 nxt_link points to the pool pointed by Asys.first_mem
		p->nxt_link = Asys.first_mem;
		//1.1.2 nxt_link and flags is cleared
		p->pre_link = 0;
		p->process = p->flags = p->chunk_count = p->chunk_index = 0;
		Asys.first_mem = (uint8_t *)p;
		Asys.num_buf_in_use--;

	}
	else
	{
		chunk_count = p->chunk_count;
		first_list_pool = p;
		//2. go to last buffer clearing all the buffers flags & counters
		for(i=0;i<chunk_count;i++)
		{
			p->process = p->flags = p->chunk_count = p->chunk_index = 0;
			p++;
			Asys.num_buf_in_use--;
		}
		//2. p now points to the buffer that was pointed by Asys.first_mem, adjust pre_link
		//2.1 get the preceding buffer
		insert_pool = p-1;
		//2.2 make the preceding buffer points to saved first buffer in list
		p->pre_link = (uint8_t *)insert_pool;
		//3 adjust p
		p--;
		//4 update nxt_link with Asys.first_mem, so insert the buffer in the queue
		p->nxt_link = Asys.first_mem;
		first_list_pool->pre_link = 0;
		Asys.first_mem = (uint8_t *)first_list_pool;
	}
	__enable_irq();
	return 0;
}


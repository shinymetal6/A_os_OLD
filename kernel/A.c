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
 * A.c
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */

#include "main.h"
#include "A.h"
#include "system_default.h"
#include "scheduler.h"
#include "A_exported_functions.h"
#include "kernel_opt.h"

SYSTEM_RAM		Asys_t			Asys;
SYSTEM_RAM		MEMpool_t		MEMpool[POOL_NUM];
SYSTEM_RAM 		PCB_t 			process[MAX_PROCESS];
SYSTEM_RAM		HWMngr_t		HWMngr[PERIPHERAL_NUM];
SYSTEM_RAM		IrqMngr_t		IrqMngr[PERIPHERAL_NUM];
SYSTEM_RAM		HWMngr_queue_t	HwQueues[PERIPHERAL_NUM];
SYSTEM_RAM		Semaphores_t	Semaphores;

//
extern	USRprcs_t	UserProcesses[USR_PROCESS_NUMBER];

A_IpAddr_t	A_IpAddr =
{
		.IP_ADDR0 		= 192,
		.IP_ADDR1 		= 168,
		.IP_ADDR2		= 10,
		.IP_ADDR3 		= 220,
		.NETMASK_ADDR0	= 255,
		.NETMASK_ADDR1 	= 255,
		.NETMASK_ADDR2 	= 255,
		.NETMASK_ADDR3 	= 0,
		.GW_ADDR0 		= 192,
		.GW_ADDR1 		= 168,
		.GW_ADDR2 		= 10,
		.GW_ADDR3 		= 1
};
SYSTEM_RAM	A_IpAddr_t	A_DhcpIpAddr;

uint32_t	UsbDeviceId0 = 0xdeadbeef;
uint32_t	UsbDeviceId1 = 0xbeefdead;

__attribute__((naked)) void init_scheduler_stack(uint32_t sched_top_of_stack)
{
     __set_MSP(sched_top_of_stack);
     __asm volatile("BX LR");
}

void init_processes_stacks(void)
{
uint32_t *pPSP,i,j;
	process[0].current_state = PROCESS_READY_STATE;
	process[1].current_state = PROCESS_READY_STATE;
	process[2].current_state = PROCESS_READY_STATE;
	process[3].current_state = PROCESS_READY_STATE;
	process[4].current_state = PROCESS_READY_STATE;

	process[0].psp_value = IDLE_STACK_START;
	process[1].psp_value = FIRST_PRC_STACK_START;
	process[2].psp_value = process[1].psp_value - UserProcesses[0].stack_size;
	process[3].psp_value = process[2].psp_value - UserProcesses[1].stack_size;
	process[4].psp_value = process[3].psp_value - UserProcesses[2].stack_size;

	process[0].task_handler = supervisor;
	process[1].task_handler = supervisor_process1;
	process[2].task_handler = supervisor_process2;
	process[3].task_handler = supervisor_process3;
	process[4].task_handler = supervisor_process4;

	for(i = 0 ; i < MAX_TASKS ;i++)
	{
		pPSP = (uint32_t*) process[i].psp_value;
		pPSP--;
		*pPSP = DUMMY_XPSR;//0x00100000
		pPSP--; //PC
		*pPSP = (uint32_t) process[i].task_handler;
		pPSP--; //LR
		*pPSP = 0xFFFFFFFD;
		for(j = 0 ; j < 13 ; j++)
		{
			pPSP--;
			*pPSP = 0;
		}
		process[i].psp_value = (uint32_t)pPSP;
	}
}

void init_systick_timer(uint32_t tick_hz)
{
uint32_t ticks = (SYSTICK_TIM_CLK/tick_hz)-1;
    SysTick_Config(ticks);
    Asys.g_os_started = 1;
}

void A_init_mem(void)
{
	bzero((uint8_t *)&Asys,sizeof(Asys));
	bzero((uint8_t *)&Semaphores,sizeof(Semaphores));
	bzero((uint8_t *)HWMngr,sizeof(HWMngr));
	bzero((uint8_t *)HwQueues,sizeof(HwQueues));
	bzero((uint8_t *)process,sizeof(process));
	bzero((uint8_t *)POOL_START,(uint32_t )POOL_SIZE);
	bzero((uint8_t *)SRAM_START,(uint16_t )SRAM_SIZE);
}

void A_enable_processor_faults(void)
{
	SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
	SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk;
	SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk;
}

void A_IrqPriority_Init(void)
{
	HAL_NVIC_SetPriority(PendSV_IRQn,  PendSV_PRIORITY, 0);		/* Make PendSV_IRQn lower priority */
	HAL_NVIC_SetPriority(SysTick_IRQn, SysTick_PRIORITY, 0);	/* Make SysTick_IRQn higher priority than PendSV_IRQn */

#ifdef ENCODER_ENABLED
	HAL_NVIC_SetPriority(EXTI0_IRQn, Exti0_PRIORITY, 0);		/* Make EXTI0_IRQn higher priority than SysTick_IRQn and lower than PendSV_IRQn */
	HAL_NVIC_SetPriority(EXTI1_IRQn, Exti1_PRIORITY, 0);		/* Make EXTI1_IRQn higher priority than SysTick_IRQn and lower than PendSV_IRQn */
	HAL_NVIC_SetPriority(EXTI2_IRQn, Exti2_PRIORITY, 0);		/* Make EXTI2_IRQn higher priority than SysTick_IRQn and lower than PendSV_IRQn */
#endif //#ifdef ENCODER_ENABLED

#ifdef	CODEC_ENABLED
	HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, I2STX_PRIORITY, 0); /* Make DMA1_Stream2_IRQn middle priority */
	HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, I2SRX_PRIORITY, 0); /* Make DMA1_Stream3_IRQn middle priority */
#endif //#ifdef CODEC_ENABLED
}

void DWT_Delay_us(uint32_t au32_microseconds)
{
	uint32_t au32_initial_ticks = DWT->CYCCNT;
	uint32_t au32_ticks = (HAL_RCC_GetHCLKFreq() / 1000000);
	au32_microseconds *= au32_ticks;
	while ((DWT->CYCCNT - au32_initial_ticks) < au32_microseconds-au32_ticks);
}

uint32_t DWT_Delay_Init(void)
{
    /* Disable TRC */
    CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; // ~0x01000000;
    /* Enable TRC */
    CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk; // 0x01000000;

    /* Disable clock cycle counter */
    DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
    /* Enable  clock cycle counter */
    DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk; //0x00000001;

    /* Reset the clock cycle counter value */
    DWT->CYCCNT = 0;

    /* 3 NO OPERATION instructions */
    __ASM volatile ("NOP");
    __ASM volatile ("NOP");
    __ASM volatile ("NOP");

    /* Check if clock cycle counter has started */
    if(DWT->CYCCNT)
    {
       return 0; /*clock cycle counter started*/
    }
    else
    {
      return 1; /*clock cycle counter not started*/
    }
}

void A_MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x24040000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_64KB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

void A_start(void)
{
#if defined ETH_NRST_Pin
	HAL_GPIO_WritePin(ETH_NRST_GPIO_Port, ETH_NRST_Pin,GPIO_PIN_SET);
#endif
	A_PreOS_Init();
	__disable_irq();

	DWT_Delay_Init();
	A_Processor_Quirks();

	A_enable_processor_faults();
	A_IrqPriority_Init();
	init_scheduler_stack(SCHED_STACK_START);
	init_processes_stacks();
	init_systick_timer(TICK_HZ);
	A_mem_init();
#ifdef DATA_CACHE_ENABLE
	A_MPU_Config();
	SCB_EnableDCache();
#endif
	A_PreScheduler_Init();
	switch_sp_to_psp();

	__enable_irq();
	supervisor();
}


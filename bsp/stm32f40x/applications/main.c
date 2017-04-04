/*
 * File      : main.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2015, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2014-04-27     Bernard      make code cleanup.
 */

#include <rtthread.h>
#ifdef RT_USING_DFS
#include <dfs_elm.h>
#include <dfs_fs.h>
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <rtdevice.h>
#include <board.h>
static struct rt_semaphore rx_sem_uart1;
rt_device_t dev_uart1 = RT_NULL;
static struct rt_semaphore rx_sem_uart2;
rt_device_t dev_uart2 = RT_NULL;

static rt_err_t rx_ind_uart1(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(&rx_sem_uart1);
    return RT_EOK;
}
static void uart1_rx(void* parameter)
{
	rt_uint8_t buf[25] = {0};	
	int i=0,len;
	while (1)
	{			
		rt_sem_take(&rx_sem_uart1, RT_WAITING_FOREVER);
		len = rt_device_read(dev_uart1,0,buf,25);
		rt_device_write(dev_uart1,0,buf,len);
	}
}
static rt_err_t rx_ind_uart2(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(&rx_sem_uart2);
    return RT_EOK;
}
static void uart2_rx(void* parameter)
{		
	rt_uint8_t buf[25] = {0};	
	int i=0,len;
	while (1)
	{
		rt_sem_take(&rx_sem_uart2, RT_WAITING_FOREVER);
		len = rt_device_read(dev_uart2,0,buf,25);
		rt_device_write(dev_uart2,0,buf,len);
	}
}
static void led_thread(void* parameter)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin	   = GPIO_Pin_0;	
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;	
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;        
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
	GPIO_Init(GPIOB, &GPIO_InitStructure); 

	while (1)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_0); 
		rt_thread_delay(100);
		GPIO_ResetBits(GPIOB, GPIO_Pin_0); 
		rt_thread_delay(100);
	}
}

void sdram_init(void);
extern int nand_mtd_init(void);
extern rt_int32_t rt_hw_nand_init(void);
void *sram_malloc(unsigned long size);
void sram_free(void *ptr);

int main(void)
{
#if 1
	dev_uart1 = rt_device_find("uart1");

	if (dev_uart1 == RT_NULL) {
		rt_kprintf("can not find uart1 \n");
		return 0;
	}
	if (rt_device_open(dev_uart1, 
		RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX
		) == RT_EOK)
	{
		rt_sem_init(&rx_sem_uart1, "uart1_sem", 0, 0);
		rt_device_set_rx_indicate(dev_uart1, rx_ind_uart1);
		rt_thread_startup(rt_thread_create("uart1_rx",
			uart1_rx, RT_NULL,512, 20, 10));
	}

	dev_uart2 = rt_device_find("uart3");

	if (dev_uart2 == RT_NULL) {
		rt_kprintf("can not find uart2 \n");
		return 0;
	}
	if (rt_device_open(dev_uart2, 
		RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX
		) == RT_EOK)
	{
		rt_sem_init(&rx_sem_uart2, "uart2_sem", 0, 0);
		rt_device_set_rx_indicate(dev_uart2, rx_ind_uart2);
		rt_thread_startup(rt_thread_create("uart2_rx",
			uart2_rx, RT_NULL,512, 20, 10));
	}
#endif
	rt_thread_startup(rt_thread_create("led",
			led_thread, RT_NULL,256, 20, 10));
	sdram_init();
#ifdef RT_USING_DFS_UFFS
	nand_mtd_init();
	rt_kprintf("NAND File System initialzation %d \n",dfs_mount("nand0", "/", "uffs", 0, 0));
#else
	rt_hw_nand_init();
    rt_kprintf("NAND File System initialzation %d \n",dfs_mount("nand0", "/", "elm", 0, 0));
#endif
	rt_kprintf("last error %d\n",rt_get_errno());
	/*rt_uint8_t *tmp = (rt_uint8_t *)sram_malloc(128);
	rt_memset(tmp,0x34,127);
	tmp[127]='\0';
	rt_kprintf("test %s\n",tmp);
	sram_free(tmp);*/
    return 0;
}
struct rt_memheap system_heap;

void sdram_init(void)
{
	if (RT_EOK != rt_memheap_init(&system_heap,
						"system",
						(void *)0x10000000,
						(void *)0x10010000))
		return ;
	
	return ;
}
void *sram_malloc(unsigned long size)
{
    return rt_memheap_alloc(&system_heap, size);
}
RTM_EXPORT(sram_malloc);

void sram_free(void *ptr)
{
    rt_memheap_free(ptr);
}
RTM_EXPORT(sram_free);

void *sram_realloc(void *ptr, unsigned long size)
{
    return rt_memheap_realloc(&system_heap, ptr, size);
}
RTM_EXPORT(sram_realloc);

#ifdef RT_USING_FINSH
#ifdef FINSH_USING_MSH
#include <finsh.h>

#ifdef DFS_USING_WORKDIR
int cmd_exec(int argc, char **argv)
{
	if (argc == 2)
	{
		msh_exec(argv[1],strlen(argv[1]));
	}

	return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_exec, __cmd_exec, exec a app module);
#endif
#endif
#endif

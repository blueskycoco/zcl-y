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
/* dfs filesystem:ELM filesystem init */
#include <dfs_elm.h>
/* dfs Filesystem APIs */
#include <dfs_fs.h>
#include <spi_flash.h>
#include <spi_flash_sfud.h>
#include "drv_qspi.h"
#include "drv_sdio.h"
#include "drv_cpuusage.h"
#include "drv_eeprom.h"
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <msh.h>
#if 1
static struct rt_semaphore rx_sem_uart2;
static struct rt_semaphore tx_sem_uart2;
rt_device_t dev_uart2 = RT_NULL;

static rt_err_t rx_ind_uart2(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(&rx_sem_uart2);
    return RT_EOK;
}
static rt_err_t tx_ind_uart2(rt_device_t dev, void *size)
{
    rt_sem_release(&tx_sem_uart2);
    return RT_EOK;
}

static void usartd1_rx(void* parameter)
{
	int len = 0;
	rt_uint8_t buf[256] = {0};
	return;
	
}
static void uart2_rx_int_dma_tx(void* parameter)
{
	rt_kprintf("uart2_rx_int_dma_tx start\r\n");
	int len = 0;
	rt_uint8_t buf[256] = {0};
	while (1)
	{	
		if (rt_sem_take(&rx_sem_uart2, RT_WAITING_FOREVER) != RT_EOK) 
		{
			rt_kprintf("no message from uart2\n");
			continue;
		}
		len=rt_device_read(dev_uart2, 0, buf, 256);
		if (len > 0) {
		rt_device_write(dev_uart2,0,buf,len);
		rt_sem_take(&tx_sem_uart2, RT_WAITING_FOREVER);
		}
	}
}
#endif
void mnt_init(void)
{

    if (RT_EOK != rt_hw_sdio_init())
		return ;
    //rt_thread_delay(RT_TICK_PER_SECOND * 1);

    /* mount sd card fat partition 1 as root directory */
    if (dfs_mount("sd0", "/sd", "elm", 0, 0) == 0)
    {
        rt_kprintf("SD File System initialized!\n");
    }
    else
    {
        rt_kprintf("SD File System initialzation failed!\n");
    }
//    mkfs("elm","sd0");
	/*int fd;

	fd = open("/1.txt", O_RDWR | O_APPEND | O_CREAT, 0);
	if (fd >= 0)
	{
		write (fd, "1234", 4);
		close(fd);
	}
	else
	{
		rt_kprintf("open file:/1.txt failed!\n");
	}*/
}
//INIT_ENV_EXPORT(mnt_init);
void test_uart2_rx_int_dma_tx(void)
{
	dev_uart2 = rt_device_find("uart2");
	if (dev_uart2 == RT_NULL) {
		rt_kprintf("can not find uart2 \n");
		return ;
	}
	struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
	config.baud_rate=115200;
	config.parity=PARITY_NONE;
	rt_device_control(RT_DEVICE(dev_uart2), RT_DEVICE_CTRL_CONFIG, &config);
	rt_device_control(RT_DEVICE(dev_uart2), RT_DEVICE_CTRL_SET_INT, (void *)RT_DEVICE_FLAG_INT_RX);
	if (rt_device_open(dev_uart2, 
		RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_DMA_TX 
		) == RT_EOK)
	{		
		rt_sem_init(&rx_sem_uart2, "uart2_sem", 0, 0);
		rt_device_set_rx_indicate(dev_uart2, rx_ind_uart2);
		rt_sem_init(&tx_sem_uart2, "uart2_tsem", 0, 0);
		rt_device_set_tx_complete(dev_uart2, tx_ind_uart2);
		rt_thread_startup(rt_thread_create("uart2_rx",
			uart2_rx_int_dma_tx, RT_NULL,2048, 20, 10));
	}
}


int main(void)
{
#if 0
	dev_usart1 = rt_device_find("usart1");

	if (dev_usart1 == RT_NULL) {
		rt_kprintf("can not find usart1 \n");
		return 0;
	}
	struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;				
	//config.baud_rate=100000;
	//config.parity=PARITY_EVEN;
	config.bufsz = 0;
	rt_device_control(RT_DEVICE(dev_usart1), RT_DEVICE_CTRL_CONFIG, &config);
	if (rt_device_open(dev_usart1, 
		RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_DMA_RX | RT_DEVICE_FLAG_DMA_TX 
		) == RT_EOK)
	{
		rt_sem_init(&rx_sem, "usart1_sem", 0, 0);
		rt_device_set_rx_indicate(dev_usart1, rx_ind);
		rt_thread_startup(rt_thread_create("usart1_rx",
			usart1_rx, RT_NULL,2048, 20, 10));
	}
#else
	rt_thread_startup(rt_thread_create("usartd1_rx",
		usartd1_rx, RT_NULL,2048, 20, 10));
#endif
#ifdef RT_USING_DFS
	rt_hw_spi_init();	
    rt_sfud_flash_probe("flash", "spi10");	
    if (dfs_mount("flash", "/", "elm", 0, 0) == 0)
    {
    	DIR *dir = RT_NULL;
        rt_kprintf("root file system initialized!\n");
		if ((dir = opendir("/sd"))==RT_NULL)
			mkdir("/sd",0);
		else
			closedir(dir);
	}
	else
	{
		rt_kprintf("root file system failed %d!\n", rt_get_errno());
	}
#endif
	cpu_usage_init();
	#if 0
	eeprom_init();
	rt_uint8_t data[8]={0};
	for(int i=0;i<8;i++)
		data[i]=i*7;
	eeprom_write(0,data,8);
	rt_thread_delay(1);
	rt_memset(data,0,8);
	if(eeprom_read(0,data,8))
	{
		for(int i=0;i<8;i++)
			rt_kprintf("I2C %x\n", data[i]);
	}
	rt_uint16_t calc[7]={0};
	rt_uint32_t temp,press;
	ms5611_reset();
	if(ms5611_read(calc,&temp,&press))
	rt_kprintf("prees info is \ncalc %x %x %x %x %x %x %x\ntemp %x\npress %x\n", 
	calc[0],calc[1],calc[2],calc[3],calc[4],calc[5],calc[6],
	temp,press);
	#endif
	//mnt_init();
//	rt_data_queue_init(&data_queue, 8, 4, RT_NULL);		
//	rt_thread_startup(rt_thread_create("thr_moa",
//			moa_rx, RT_NULL,1024, 20, 10));
//	rt_thread_startup(rt_thread_create("thr_mob",
//			mob_tx, RT_NULL,1024, 20, 10));
	test_uart2_rx_int_dma_tx();
    return 0;
}

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
int cmd_cpu(int argc, char **argv)
{
	rt_uint8_t major,minor;
    cpu_usage_get(&major, &minor);
	rt_kprintf("CPU %d.%d%\n", major,minor);
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_cpu, __cmd_cpu, get cpuusage);

#endif

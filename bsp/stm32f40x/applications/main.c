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
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <rtdevice.h>
#if 1
static struct rt_semaphore rx_sem;
rt_device_t dev_usart1 = RT_NULL;

static rt_err_t rx_ind(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(&rx_sem);
    return RT_EOK;
}
static void usartd1_rx(void* parameter)
{
	int len = 0;
	rt_uint8_t buf[256] = {0};
	return;
	while (1)
	{	
		if (rt_sem_take(&rx_sem, RT_WAITING_FOREVER) != RT_EOK) continue;
		len=rt_device_read(dev_usart1, 0, buf, 256);
		buf[len]='\0';
		rt_kprintf("%s", buf);
	}
}
static void usart1_rx(void* parameter)
{		
	__attribute__((__aligned__(32))) rt_uint8_t buf[25] = {0};	
	int i=0;
	while (1)
	{	
		for(i=0;i<25;i++)
		{
			buf[i] = 0;
		}
		rt_device_read(dev_usart1,0,buf,25);	
		
		rt_sem_take(&rx_sem, RT_WAITING_FOREVER);

		for(i=0;i<25;i++)
		{
			rt_kprintf("%2x,",buf[i]);
		}
		rt_kprintf("\n");
		

	}

}
#endif
void mnt_init(void)
{
#if 0
    if (RT_EOK != rt_hw_sdio_init())
		return ;

    /* mount sd card fat partition 1 as root directory */
    if (dfs_mount("sd0", "/sd", "elm", 0, 0) == 0)
    {
        rt_kprintf("SD File System initialized!\n");
    }
    else
    {
        rt_kprintf("SD File System initialzation failed!\n");
    }
#endif
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
static struct rt_data_queue data_queue;
void moa_rx()
{
	const void *data_ptr;
    rt_size_t data_size;
	while(1)
	{
		rt_data_queue_pop(&data_queue, &data_ptr, &data_size, RT_WAITING_FOREVER);	
        rt_kprintf("%s\n", data_ptr);
		rt_free((void *)data_ptr);
	}
}
void mob_tx()
{
	rt_err_t result;
	rt_uint8_t *data = RT_NULL;
	int i = 0;

	while (1) {
	data = (rt_uint8_t *)rt_calloc(20, sizeof(rt_uint8_t));	
	//for (i = 0; i< 20; i++)
	//	data[i] = 0x30 + i;
	rt_memset(data, 0x30+i, 19);
	result = rt_data_queue_push(&data_queue, data, 20, RT_WAITING_FOREVER);
	if (result != RT_EOK)
	{
		rt_kprintf("mob push data failed\n");
	}
	i++;
	if (i > 10)
		i = 0;
	rt_thread_delay(RT_TICK_PER_SECOND);
	}
}

int main(void)
{
#if 1
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
/*#ifdef RT_USING_DFS
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
#endif*/

    return 0;
}


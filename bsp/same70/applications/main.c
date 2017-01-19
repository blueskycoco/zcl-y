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
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <msh.h>
static struct rt_semaphore rx_sem;
rt_device_t dev_usart1 = RT_NULL;

static rt_err_t rx_ind(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(&rx_sem);
    return RT_EOK;
}
static void usart1_rx(void* parameter)
{
	int len = 0;
	rt_uint8_t buf[256] = {0};
	while (1)
	{	
		if (rt_sem_take(&rx_sem, RT_WAITING_FOREVER) != RT_EOK) continue;
		len=rt_device_read(dev_usart1, 0, buf, 256);
		buf[len]='\0';
		rt_kprintf("%s", buf);
	}
}
int main(void)
{
	dev_usart1 = rt_device_find("usart1");

	if (dev_usart1 == RT_NULL) {
		rt_kprintf("can not find usart1 \n");
		return 0;
	}
	if (rt_device_open(dev_usart1, 
		RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX 
		) == RT_EOK)
	{
		rt_sem_init(&rx_sem, "usart1_sem", 0, 0);
		rt_device_set_rx_indicate(dev_usart1, rx_ind);
		rt_thread_startup(rt_thread_create("usart1_rx",
			usart1_rx, RT_NULL,2048, 20, 10));
	}

#ifdef RT_USING_DFS
	rt_hw_spi_init();	
    rt_sfud_flash_probe("flash", "spi10");	
    if (dfs_mount("flash", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("root file system initialized!\n");
	}
	else
	{
		rt_kprintf("root file system failed %d!\n", rt_get_errno());
	}
#endif
	
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
#endif

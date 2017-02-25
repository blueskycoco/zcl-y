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
#include <rtdevice.h>

void mob_tx()
{
	struct rt_data_queue data_queue;
	rt_err_t result;
	rt_uint8_t data[20] = {0x31};
	int i;

	rt_kprintf("mob 0\n");
#if 1
	for (i = 0; i< 20; i++)
		data[i] = 0x30 + i;

	rt_kprintf("mob 1\n");
	//get_data_queue(&data_queue);
	rt_data_queue_init(&data_queue, 8, 4, RT_NULL);
	rt_kprintf("mob 2\n");
	result = rt_data_queue_push(&data_queue, data, 20, RT_WAITING_FOREVER);
	rt_kprintf("mob 3\n");
	if (result == RT_EOK)
	{
		rt_kprintf("mob push data ok\n");
	}
#endif
}
int main(void)
{
	//rt_thread_startup(rt_thread_create("thr_mob",
	//		mob_tx, RT_NULL,1024, 20, 10));		
	//rt_kprintf("mob 0\n");
	mob_tx();
	return 0;
}






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
#if 0
#include "board.h"
struct _CalBaseProtocol 
{	float jgx;	float jgy;	float jgz;	float jax;	float jay;	float jaz;	float tempGx;	float tempGy;	float tempGz;	float speed_5004;	float hight_5611;};
struct _CalBaseProtocol a,b;
#endif
static struct rt_data_queue data_queue;
void moa_rx(void)
{
	const void *data_ptr;
    rt_size_t data_size;
	rt_kprintf("moa_rx\n");
	while(1)
	{
		rt_data_queue_pop(&data_queue, &data_ptr, &data_size, RT_WAITING_FOREVER);	
        rt_kprintf("%s\n", data_ptr);
		rt_free((void *)data_ptr);
	}
}
void mob_tx(void)
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
		rt_free(data);
		rt_kprintf("mob push data failed\n");
	}
	i++;
	if (i > 10)
		i = 0;
	rt_thread_delay(RT_TICK_PER_SECOND);
	}
}


int main(int argc, void *argv[])
{
	rt_data_queue_init(&data_queue, 8, 4, RT_NULL);		
	rt_thread_startup(rt_thread_create("thr_moa",
			moa_rx, RT_NULL,1024, 20, 10));
	rt_thread_startup(rt_thread_create("thr_mob",
			mob_tx, RT_NULL,2048, 20, 10));
	return 0;
}






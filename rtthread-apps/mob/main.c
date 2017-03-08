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
static struct rt_data_queue data_queueb;

void mob_tx()
{
	rt_err_t result;
	rt_uint8_t *data = RT_NULL;
	int i;

	data = (rt_uint8_t *)rt_calloc(20, sizeof(rt_uint8_t));	
	for (i = 0; i< 20; i++)
		data[i] = 0x30 + i;

	result = rt_data_queue_push(&data_queueb, data, 20, RT_WAITING_FOREVER);
	if (result == RT_EOK)
	{
		rt_kprintf("mob push data ok\n");
	}
}

int main(void)
{
	get_data_queue(&data_queueb);
	mob_tx();
	return 0;
}






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
#include "user.h"


enum _SYS_MODE WOKING_MODE=CALIBRATION_MODE;
unsigned int AD_DATA[8]={0};
float sensor_data[11]={0};
uint8_t temp_time=0;
struct _CalBaseProtocol IMU_Data;
struct _CalSendProtocol IMU_SData;
char *p_data;
char sendchardata[51]={0};
char Average_Time=0;
float imu_sum[11]={0};



int main(void)
{   /* put user application code here */

    spi_init(0,2,0);  
	io_init(RT_NULL);
	
	StartTimersInterrupt(1,0,1000,0,spi_getdata);
    return 0;
}






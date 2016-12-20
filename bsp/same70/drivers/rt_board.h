/*
 * File      : board.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-09-22     Bernard      add board.h to this bsp
 */
#ifndef __BOARD_H__
#define __BOARD_H__

//#include <stm32f7xx.h>
//#include "stm32f7xx_hal.h"

// <<< Use Configuration Wizard in Context Menu >>>
/* board configuration */
#define RT_USING_UART1
#define RT_USING_UART2
#define RT_USING_UART3

// <o> SDCard Driver <1=>SDIO sdcard <0=>SPI MMC card
//  <i>Default: 1
#define STM32_USE_SDIO          0

extern int _ebss;
#define HEAP_BEGIN    (&_ebss)

// <o> Internal SRAM memory size[Kbytes] <8-64>
//  <i>Default: 64
#define STM32_SRAM_SIZE   (384 * 1024)
#define HEAP_END          (0x20400000 + STM32_SRAM_SIZE)

void rt_hw_board_init(void);

#endif


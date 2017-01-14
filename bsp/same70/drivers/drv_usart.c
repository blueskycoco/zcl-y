/*
 * File      : drv_usart.c
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
 * 2015-08-01     xiaonong     the first version for same70f7xx
 */

#include "drv_usart.h"
#include "rt_board.h"

#include <rtdevice.h>
#include "board.h"
#ifdef RT_USING_USART0
/** USART0 pin RX */
#define PIN_USART0_RXD_DBG \
	{PIO_PB0C_RXD0, PIOB, ID_PIOB, PIO_PERIPH_C, PIO_DEFAULT}
/** USART0 pin TX */
#define PIN_USART0_TXD_DBG \
	{PIO_PB1C_TXD0, PIOB, ID_PIOB, PIO_PERIPH_C, PIO_DEFAULT}

#define USART0_PINS      {PIN_USART0_TXD_DBG, PIN_USART0_RXD_DBG}
#endif
#ifdef RT_USING_USART1
/** USART1 pin RX */
#define PIN_USART1_RXD \
	{PIO_PA21A_RXD1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/** USART1 pin TX */
#define PIN_USART1_TXD \
	{PIO_PB4D_TXD1, PIOB, ID_PIOB, PIO_PERIPH_D, PIO_DEFAULT}

#define USART1_PINS      {PIN_USART1_TXD, PIN_USART1_RXD}
#endif

struct same70_uart
{
    Usart *UartHandle;
    IRQn_Type irq;
};

static rt_err_t same70_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    struct same70_uart *uart;
	unsigned char data_width = 0;
	unsigned char stop_bits = 0;
	
    RT_ASSERT(serial != RT_NULL);
    RT_ASSERT(cfg != RT_NULL);

    uart = (struct same70_uart *)serial->parent.user_data;
	if (cfg->data_bits == DATA_BITS_8)
		data_width= US_MR_CHRL_8_BIT;

	if (cfg->stop_bits == STOP_BITS_1)
		stop_bits = US_MR_NBSTOP_1_BIT;
		
	MATRIX->MATRIX_WPMR  = MATRIX_WPMR_WPKEY_PASSWD;
	MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO4;
	uart->UartHandle->US_CR = US_CR_RSTRX | US_CR_RSTTX | US_CR_RSTSTA;
	uart->UartHandle->US_IDR = 0xFFFFFFFF;
	uart->UartHandle->US_BRGR = (BOARD_MCK / cfg->baud_rate) / 16;
	uart->UartHandle->US_MR =
			(US_MR_USART_MODE_NORMAL | US_MR_PAR_NO | US_MR_USCLKS_MCK
		   | data_width | stop_bits);
	uart->UartHandle->US_CR = US_CR_RXEN | US_CR_TXEN;

    return RT_EOK;
}

static rt_err_t same70_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    struct same70_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct same70_uart *)serial->parent.user_data;

    switch (cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
        /* disable rx irq */
        UART_DISABLE_IRQ(uart->irq);
        /* disable interrupt */
		USART_DisableIt(uart->UartHandle, US_IER_RXRDY);
        break;
    case RT_DEVICE_CTRL_SET_INT:
        /* enable rx irq */
        UART_ENABLE_IRQ(uart->irq);
        /* enable interrupt */
        USART_EnableIt(uart->UartHandle, US_IER_RXRDY);
        break;
    }

    return RT_EOK;
}

static int same70_putc(struct rt_serial_device *serial, char c)
{
    struct same70_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct same70_uart *)serial->parent.user_data;

	while ((uart->UartHandle->US_CSR & US_CSR_TXEMPTY) == 0);
	uart->UartHandle->US_THR = c;

    return 1;
}

static int same70_getc(struct rt_serial_device *serial)
{
    int ch;
    struct same70_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct same70_uart *)serial->parent.user_data;

    ch = -1;
    if (uart->UartHandle->US_CSR & US_CSR_RXRDY)
    {
        ch = uart->UartHandle->US_RHR & 0xff;
    }
	
    return ch;
}

static const struct rt_uart_ops same70_uart_ops =
{
    same70_configure,
    same70_control,
    same70_putc,
    same70_getc,
};

#if defined(RT_USING_USART0)
static struct same70_uart usart0;
struct rt_serial_device serial_usart0;

void USART0_Handler(void)
{
    struct same70_uart *uart;

    uart = &usart0;

    /* enter interrupt */
    rt_interrupt_enter();
	    
    if (uart->UartHandle->US_CSR & US_CSR_RXRDY)
    {
        rt_hw_serial_isr(&serial_usart0, RT_SERIAL_EVENT_RX_IND);
    }
    /* leave interrupt */
    rt_interrupt_leave();
}
#endif 
#if defined(RT_USING_USART1)
static struct same70_uart usart1;
struct rt_serial_device serial_usart1;

void USART1_Handler(void)
{
    struct same70_uart *uart;

    uart = &usart1;

    /* enter interrupt */
    rt_interrupt_enter();
	    
    if (uart->UartHandle->US_CSR & US_CSR_RXRDY)
    {
        rt_hw_serial_isr(&serial_usart1, RT_SERIAL_EVENT_RX_IND);
    }
    /* leave interrupt */
    rt_interrupt_leave();
}
#endif 

int same70_hw_usart_init(void)
{
    struct same70_uart *uart;
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

#ifdef RT_USING_USART0
    uart = &usart0;
    uart->UartHandle = USART0;
	uart->irq = USART0_IRQn;
    serial_usart0.ops    = &same70_uart_ops;
    serial_usart0.config = config;
	const Pin pPins0[] = USART0_PINS;
	PIO_Configure(pPins0, PIO_LISTSIZE(pPins0));
	PMC_EnablePeripheral(ID_USART0);

    /* register USART0 device */
    rt_hw_serial_register(&serial_usart0, "usart0",
                          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                          uart);
#endif 
#ifdef RT_USING_USART1
	uart = &usart1;
	uart->UartHandle = USART1;
	uart->irq = USART1_IRQn;
	serial_usart1.ops	 = &same70_uart_ops;
	serial_usart1.config = config;
	const Pin pPins1[] = USART1_PINS;
	PIO_Configure(pPins1, PIO_LISTSIZE(pPins1));
	PMC_EnablePeripheral(ID_USART1);

	/* register USART1 device */
	rt_hw_serial_register(&serial_usart1, "usart1",
						  RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
						  uart);
#endif 

    return 0;
}
INIT_BOARD_EXPORT(same70_hw_usart_init);

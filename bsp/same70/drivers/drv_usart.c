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
#ifdef RT_USING_UART0
/** UART0 pin RX */
#define PIN_UART0_RXD \
	{PIO_PA9A_URXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/** UART0 pin TX */
#define PIN_UART0_TXD \
	{PIO_PA10A_UTXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

#define UART0_PINS      {PIN_UART0_TXD, PIN_UART0_RXD}
#endif
#ifdef RT_USING_UART1
/** UART1 pin RX */
#define PIN_UART1_RXD \
	{PIO_PA5C_URXD1, PIOA, ID_PIOA, PIO_PERIPH_C, PIO_DEFAULT}
/** UART1 pin TX */
#define PIN_UART1_TXD \
	{PIO_PA6C_UTXD1, PIOA, ID_PIOA, PIO_PERIPH_C, PIO_DEFAULT}

#define UART1_PINS      {PIN_UART1_TXD, PIN_UART1_RXD}
#endif
#ifdef RT_USING_UART2
/** UART2 pin RX */
#define PIN_UART2_RXD \
	{PIO_PD25C_URXD2, PIOD, ID_PIOD, PIO_PERIPH_C, PIO_DEFAULT}
/** UART2 pin TX */
#define PIN_UART2_TXD \
	{PIO_PD26C_UTXD2, PIOD, ID_PIOD, PIO_PERIPH_C, PIO_DEFAULT}

#define UART2_PINS      {PIN_UART2_TXD, PIN_UART2_RXD}
#endif
#ifdef RT_USING_UART3
/** UART3 pin RX */
#define PIN_UART3_RXD \
	{PIO_PD28A_URXD3, PIOD, ID_PIOD, PIO_PERIPH_A, PIO_DEFAULT}
/** UART3 pin TX */
#define PIN_UART3_TXD \
	{PIO_PD30A_UTXD3, PIOD, ID_PIOD, PIO_PERIPH_A, PIO_DEFAULT}

#define UART3_PINS      {PIN_UART3_TXD, PIN_UART3_RXD}
#endif
#ifdef RT_USING_UART4
/** UART4 pin RX */
#define PIN_UART4_RXD \
	{PIO_PD18C_URXD4, PIOD, ID_PIOD, PIO_PERIPH_C, PIO_DEFAULT}
/** UART4 pin TX */
#define PIN_UART4_TXD \
	{PIO_PD19C_UTXD4, PIOD, ID_PIOD, PIO_PERIPH_C, PIO_DEFAULT}

#define UART4_PINS      {PIN_UART4_TXD, PIN_UART4_RXD}
#endif
extern sXdmad dmaDrv;
struct same70_uart
{
    Usart *UsartHandle;
	Uart *UartHandle;
    IRQn_Type irq;
	bool type_usart;
	UsartChannel UsartTx;
	UsartChannel UsartRx;
	UsartDma Usartd;
	UartChannel UartTx;
	UartChannel UartRx;
	UartDma Uartd;
	uint32_t mode;
	uint32_t baud;
	struct rt_serial_device *serial_device;
	int id;
};
rt_size_t same70_dma_transmit(struct rt_serial_device *serial, rt_uint8_t *buf, rt_size_t size, int direction);

static rt_err_t same70_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    struct same70_uart *uart;
	unsigned char data_width = 0;
	unsigned short stop_bits = 0;
	unsigned short parity_bits = US_MR_PAR_NO;
    RT_ASSERT(serial != RT_NULL);
    RT_ASSERT(cfg != RT_NULL);

    uart = (struct same70_uart *)serial->parent.user_data;

	
	if (uart->type_usart)
	{
		if (cfg->data_bits == DATA_BITS_8)
			data_width= US_MR_CHRL_8_BIT;
		else if (cfg->data_bits == DATA_BITS_7)
			data_width= US_MR_CHRL_7_BIT;
		else if (cfg->data_bits == DATA_BITS_6)
			data_width= US_MR_CHRL_6_BIT;
		else if (cfg->data_bits == DATA_BITS_5)
			data_width= US_MR_CHRL_5_BIT;
		
		if (cfg->stop_bits == STOP_BITS_1)
			stop_bits = US_MR_NBSTOP_1_BIT;
		else if (cfg->stop_bits == STOP_BITS_2)
			stop_bits = US_MR_NBSTOP_2_BIT;
		else if (cfg->stop_bits == STOP_BITS_3)
			stop_bits = US_MR_NBSTOP_1_5_BIT;
		
		if (cfg->parity == PARITY_ODD)
			parity_bits = US_MR_PAR_ODD;
		else if (cfg->parity == PARITY_EVEN)
			parity_bits = US_MR_PAR_EVEN;
		MATRIX->MATRIX_WPMR  = MATRIX_WPMR_WPKEY_PASSWD;
		MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO4;
		uart->UsartHandle->US_CR = US_CR_RSTRX | US_CR_RSTTX | US_CR_RSTSTA;
		uart->UsartHandle->US_IDR = 0xFFFFFFFF;
		uart->UsartHandle->US_BRGR = (BOARD_MCK / cfg->baud_rate) / 16;
		uart->UsartHandle->US_MR =
				(US_MR_USART_MODE_NORMAL | parity_bits | US_MR_USCLKS_MCK
			   | data_width | stop_bits);
		uart->UsartHandle->US_CR = US_CR_RXEN | US_CR_TXEN;
		uart->mode = (US_MR_USART_MODE_NORMAL | parity_bits | US_MR_USCLKS_MCK
			   | data_width | stop_bits);
	}
	else
	{
		if (cfg->parity == PARITY_ODD)
			parity_bits = UART_MR_PAR_ODD;
		else if (cfg->parity == PARITY_EVEN)
			parity_bits = UART_MR_PAR_EVEN;
		uart->UartHandle->UART_CR = UART_CR_RSTRX | UART_CR_RSTTX | UART_CR_RSTSTA;
		uart->UartHandle->UART_IDR = 0xFFFFFFFF;
		uart->UartHandle->UART_BRGR = (BOARD_MCK / cfg->baud_rate) / 16;
		uart->UartHandle->UART_MR =
				(UART_MR_CHMODE_NORMAL | parity_bits 
				| UART_MR_BRSRCCK_PERIPH_CLK);
		uart->UartHandle->UART_CR = UART_CR_RXEN | UART_CR_TXEN;
		uart->mode = (UART_MR_CHMODE_NORMAL | parity_bits 
				| UART_MR_BRSRCCK_PERIPH_CLK);
	}
	uart->baud = cfg->baud_rate;
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
		if (uart->type_usart)
			USART_DisableIt(uart->UsartHandle, US_IER_RXRDY);
		else
			UART_DisableIt(uart->UartHandle, UART_IER_RXRDY);
        break;
    case RT_DEVICE_CTRL_SET_INT:
        /* enable rx irq */
        UART_ENABLE_IRQ(uart->irq);
        /* enable interrupt */
		if (uart->type_usart)
	        USART_EnableIt(uart->UsartHandle, US_IER_RXRDY);
		else
			UART_EnableIt(uart->UartHandle, UART_IER_RXRDY);
        break;
    }

    return RT_EOK;
}

static int same70_putc(struct rt_serial_device *serial, char c)
{
    struct same70_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct same70_uart *)serial->parent.user_data;

	if (uart->type_usart)
	{
		while ((uart->UsartHandle->US_CSR & US_CSR_TXEMPTY) == 0);
		uart->UsartHandle->US_THR = c;
	}
	else
	{
		while ((uart->UartHandle->UART_SR & UART_SR_TXEMPTY) == 0);
		uart->UartHandle->UART_THR = c;
	}

    return 1;
}

static int same70_getc(struct rt_serial_device *serial)
{
    int ch;
    struct same70_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct same70_uart *)serial->parent.user_data;

    ch = -1;

	if (uart->type_usart)
	{
	    if (uart->UsartHandle->US_CSR & US_CSR_RXRDY)
	    {
	        ch = uart->UsartHandle->US_RHR & 0xff;
	    }
	}
	else
	{
	    if (uart->UartHandle->UART_SR & UART_SR_RXRDY)
	    {
	        ch = uart->UartHandle->UART_RHR & 0xff;
	    }
	}
    return ch;
}

static const struct rt_uart_ops same70_uart_ops =
{
    same70_configure,
    same70_control,
    same70_putc,
    same70_getc,
    same70_dma_transmit,
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
	    
    if (uart->UsartHandle->US_CSR & US_CSR_RXRDY)
    {
        rt_hw_serial_isr(&serial_usart0, RT_SERIAL_EVENT_RX_IND);
    }
	if (uart->UsartHandle->US_CSR & US_CSR_TIMEOUT) 
	{		
		USART_AcknowledgeRxTimeOut(uart->UsartHandle, 0);			
		UsartChannel *pUsartdCh = uart->Usartd.pRxChannel;		
		XDMAC_SoftwareFlushReq(uart->Usartd.pXdmad->pXdmacs, pUsartdCh->ChNum);		
		if (pUsartdCh->dmaProgress == 0) 
		{			
			USARTD_DisableRxChannels(&(uart->Usartd), &(uart->UsartRx));			
			SCB_InvalidateDCache_by_Addr((uint32_t *)pUsartdCh->pBuff, pUsartdCh->BuffSize);			
			rt_hw_serial_isr(&serial_usart0, RT_SERIAL_EVENT_RX_DMADONE);		
		}	
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
	    
    if (uart->UsartHandle->US_CSR & US_CSR_RXRDY)
    {
        rt_hw_serial_isr(&serial_usart1, RT_SERIAL_EVENT_RX_IND);
    }
	if (uart->UsartHandle->US_CSR & US_CSR_TIMEOUT) 
	{		
		USART_AcknowledgeRxTimeOut(uart->UsartHandle, 0);			
		UsartChannel *pUsartdCh = uart->Usartd.pRxChannel;		
		XDMAC_SoftwareFlushReq(uart->Usartd.pXdmad->pXdmacs, pUsartdCh->ChNum);		
		if (pUsartdCh->dmaProgress == 0) 
		{			
			USARTD_DisableRxChannels(&(uart->Usartd), &(uart->UsartRx));			
			SCB_InvalidateDCache_by_Addr((uint32_t *)pUsartdCh->pBuff, pUsartdCh->BuffSize);			
			rt_hw_serial_isr(&serial_usart1, RT_SERIAL_EVENT_RX_DMADONE);		
		}	
	}
    /* leave interrupt */
    rt_interrupt_leave();
}
#endif 
#if defined(RT_USING_UART0)
static struct same70_uart uart0;
struct rt_serial_device serial_uart0;

void UART0_Handler(void)
{
    struct same70_uart *uart;

    uart = &uart0;

    /* enter interrupt */
    rt_interrupt_enter();
	    
    if (uart->UartHandle->UART_SR & UART_SR_RXRDY)
    {
        rt_hw_serial_isr(&serial_uart0, RT_SERIAL_EVENT_RX_IND);
    }
    /* leave interrupt */
    rt_interrupt_leave();
}
#endif 
#if defined(RT_USING_UART1)
static struct same70_uart uart1;
struct rt_serial_device serial_uart1;

void UART1_Handler(void)
{
    struct same70_uart *uart;

    uart = &uart1;

    /* enter interrupt */
    rt_interrupt_enter();
	    
    if (uart->UartHandle->UART_SR & UART_SR_RXRDY)
    {
        rt_hw_serial_isr(&serial_uart1, RT_SERIAL_EVENT_RX_IND);
    }
    /* leave interrupt */
    rt_interrupt_leave();
}
#endif 
#if defined(RT_USING_UART2)
static struct same70_uart uart2;
struct rt_serial_device serial_uart2;

void UART2_Handler(void)
{
    struct same70_uart *uart;

    uart = &uart2;

    /* enter interrupt */
    rt_interrupt_enter();
	    
    if (uart->UartHandle->UART_SR & UART_SR_RXRDY)
    {
        rt_hw_serial_isr(&serial_uart2, RT_SERIAL_EVENT_RX_IND);
    }
    /* leave interrupt */
    rt_interrupt_leave();
}
#endif 
#if defined(RT_USING_UART3)
static struct same70_uart uart3;
struct rt_serial_device serial_uart3;

void UART3_Handler(void)
{
    struct same70_uart *uart;

    uart = &uart3;

    /* enter interrupt */
    rt_interrupt_enter();
	    
    if (uart->UartHandle->UART_SR & UART_SR_RXRDY)
    {
        rt_hw_serial_isr(&serial_uart3, RT_SERIAL_EVENT_RX_IND);
    }
    /* leave interrupt */
    rt_interrupt_leave();
}
#endif 
#if defined(RT_USING_UART4)
static struct same70_uart uart4;
struct rt_serial_device serial_uart4;

void UART4_Handler(void)
{
    struct same70_uart *uart;

    uart = &uart4;

    /* enter interrupt */
    rt_interrupt_enter();
	    
    if (uart->UartHandle->UART_SR & UART_SR_RXRDY)
    {
        rt_hw_serial_isr(&serial_uart4, RT_SERIAL_EVENT_RX_IND);
    }
    /* leave interrupt */
    rt_interrupt_leave();
}
#endif 
static void same70_USARTD_Tx_Cb(uint32_t channel, struct same70_uart *pArg)
{
       UsartChannel *pUsartdCh = pArg->Usartd.pTxChannel;

       if (channel != pUsartdCh->ChNum)
               return;

       USARTD_DisableTxChannels(&(pArg->Usartd), &(pArg->UsartTx));
       rt_hw_serial_isr(pArg->serial_device, RT_SERIAL_EVENT_TX_DMADONE);
}
static void same70_USARTD_Rx_Cb(uint32_t channel, struct same70_uart *pArg)
{

       UsartChannel *pUsartdCh = pArg->Usartd.pRxChannel;

       if (channel != pUsartdCh->ChNum)
               return;

       /* Release the DMA channels */
       USARTD_DisableRxChannels(&(pArg->Usartd), &(pArg->UsartRx));
       SCB_InvalidateDCache_by_Addr((uint32_t *)pUsartdCh->pBuff, pUsartdCh->BuffSize);
       rt_hw_serial_isr(pArg->serial_device, (pUsartdCh->BuffSize << 8)|RT_SERIAL_EVENT_RX_DMADONE);
}
static void same70_UARTD_Tx_Cb(uint32_t channel, struct same70_uart *pArg)
{
       UartChannel *pUartdCh = pArg->Uartd.pTxChannel;

       if (channel != pUartdCh->ChNum)
               return;

       UARTD_DisableTxChannels(&(pArg->Uartd), &(pArg->UartTx));
       rt_hw_serial_isr(pArg->serial_device, RT_SERIAL_EVENT_TX_DMADONE);
}
static void same70_UARTD_Rx_Cb(uint32_t channel, struct same70_uart *pArg)
{

       UartChannel *pUartdCh = pArg->Uartd.pRxChannel;

       if (channel != pUartdCh->ChNum)
               return;

       /* Release the DMA channels */
       UARTD_DisableRxChannels(&(pArg->Uartd), &(pArg->UartRx));
       SCB_InvalidateDCache_by_Addr((uint32_t *)pUartdCh->pBuff, pUartdCh->BuffSize);
       rt_hw_serial_isr(pArg->serial_device, (pUartdCh->BuffSize << 8)|RT_SERIAL_EVENT_RX_DMADONE);
}
rt_size_t same70_dma_transmit(struct rt_serial_device *serial, rt_uint8_t *buf, rt_size_t size, int direction)
{
	struct same70_uart *uart;

	RT_ASSERT(serial != RT_NULL);
	uart = (struct same70_uart *)serial->parent.user_data;
	if (uart->type_usart) 
	{
		if (direction == RT_SERIAL_DMA_TX)
		{
		       rt_memset(&(uart->UsartTx), 0, sizeof(UsartChannel));
		       uart->UsartTx.BuffSize = size;
		       uart->UsartTx.pBuff = buf;
		       uart->UsartTx.dmaProgress = 1;
		       uart->UsartTx.dmaProgrammingMode = XDMAD_SINGLE;
		       uart->Usartd.pXdmad = &dmaDrv;
		       uart->Usartd.pTxChannel = &(uart->UsartTx);
		       uart->Usartd.pTxChannel->callback = (UsartdCallback)same70_USARTD_Tx_Cb;
		       uart->Usartd.pTxChannel->pArgument= uart;
		       USARTD_Configure(&(uart->Usartd), uart->id, uart->mode, uart->baud, BOARD_MCK);
		       USARTD_EnableTxChannels(&(uart->Usartd), &(uart->UsartTx));
		       USARTD_SendData(&(uart->Usartd));
		}
		else
		{
				rt_memset(&(uart->UsartRx), 0, sizeof(UsartChannel));
				uart->UsartRx.BuffSize = size;
				uart->UsartRx.pBuff = buf;
				uart->UsartRx.dmaProgress = 1;
				uart->UsartRx.dmaProgrammingMode = XDMAD_SINGLE;
				uart->Usartd.pXdmad = &dmaDrv;
				uart->Usartd.pRxChannel = &(uart->UsartRx);
				uart->Usartd.pRxChannel->callback = (UsartdCallback)same70_USARTD_Rx_Cb;
				uart->Usartd.pRxChannel->pArgument= uart;
				USARTD_Configure(&(uart->Usartd), uart->id, uart->mode, uart->baud, BOARD_MCK);
				USART_EnableRecvTimeOut(uart->UsartHandle, 115200);
				USART_EnableIt(uart->UsartHandle, US_IER_TIMEOUT);
				UART_ENABLE_IRQ(uart->irq);
				USARTD_EnableRxChannels(&(uart->Usartd), &(uart->UsartRx));
				USARTD_RcvData(&(uart->Usartd));
		}
	}
	else
	{
		if (direction == RT_SERIAL_DMA_TX)
		{
		       rt_memset(&(uart->UartTx), 0, sizeof(UartChannel));
		       uart->UartTx.BuffSize = size;
		       uart->UartTx.pBuff = buf;
		       uart->UartTx.dmaProgrammingMode = XDMAD_SINGLE;
		       uart->Uartd.pXdmad = &dmaDrv;
		       uart->Uartd.pTxChannel = &(uart->UartTx);
		       uart->Uartd.pTxChannel->callback = (UartdCallback)same70_UARTD_Tx_Cb;
		       uart->Uartd.pTxChannel->pArgument= uart;
		       UARTD_Configure(&(uart->Uartd), uart->id, uart->mode, uart->baud, BOARD_MCK);
		       UARTD_EnableTxChannels(&(uart->Uartd), &(uart->UartTx));
		       UARTD_SendData(&(uart->Uartd));
		}
		else
		{
				rt_memset(&(uart->UartRx), 0, sizeof(UartChannel));
				uart->UartRx.BuffSize = size;
				uart->UartRx.pBuff = buf;
				uart->UartRx.dmaProgrammingMode = XDMAD_SINGLE;
				uart->Uartd.pXdmad = &dmaDrv;
				uart->Uartd.pRxChannel = &(uart->UartRx);
				uart->Uartd.pRxChannel->callback = (UartdCallback)same70_UARTD_Rx_Cb;
				uart->Uartd.pRxChannel->pArgument= uart;
				UARTD_Configure(&(uart->Uartd), uart->id, uart->mode, uart->baud, BOARD_MCK);
				UARTD_EnableRxChannels(&(uart->Uartd), &(uart->UartRx));
				UARTD_RcvData(&(uart->Uartd));
		}
	}
	return size;
}

int same70_hw_usart_init(void)
{
    struct same70_uart *uart;
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

#ifdef RT_USING_USART0
    uart = &usart0;
    uart->UsartHandle = USART0;
	uart->irq = USART0_IRQn;
    serial_usart0.ops    = &same70_uart_ops;
    serial_usart0.config = config;
	const Pin pPins0[] = USART0_PINS;
	PIO_Configure(pPins0, PIO_LISTSIZE(pPins0));
	PMC_EnablePeripheral(ID_USART0);
	uart->type_usart = true;
	uart->serial_device = &serial_usart0;
	uart->id = ID_USART0;
    /* register USART0 device */
    rt_hw_serial_register(&serial_usart0, "usart0",
                          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_DMA_TX | RT_DEVICE_FLAG_DMA_RX,
                          uart);
#endif 
#ifdef RT_USING_USART1
	uart = &usart1;
	uart->UsartHandle = USART1;
	uart->irq = USART1_IRQn;
	serial_usart1.ops	 = &same70_uart_ops;
	serial_usart1.config = config;
	const Pin pPins1[] = USART1_PINS;
	PIO_Configure(pPins1, PIO_LISTSIZE(pPins1));
	PMC_EnablePeripheral(ID_USART1);
	uart->type_usart = true;
	uart->serial_device = &serial_usart1;
	uart->id = ID_USART1;
	/* register USART1 device */
	rt_hw_serial_register(&serial_usart1, "usart1",
						  RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_DMA_TX | RT_DEVICE_FLAG_DMA_RX,
						  uart);
#endif 
#ifdef RT_USING_UART0
		uart = &uart0;
		uart->UartHandle = UART0;
		uart->irq = UART0_IRQn;
		serial_uart0.ops	 = &same70_uart_ops;
		const Pin pPins2[] = UART0_PINS;
		PIO_Configure(pPins2, PIO_LISTSIZE(pPins2));
		PMC_EnablePeripheral(ID_UART0);
		uart->type_usart = false;
		uart->serial_device = &serial_uart0;
		uart->id = ID_UART0;
		/* register UART0 device */
		rt_hw_serial_register(&serial_uart0, "uart0",
							  RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_DMA_TX | RT_DEVICE_FLAG_DMA_RX,
							  uart);
#endif 
#ifdef RT_USING_UART1
			uart = &uart1;
			uart->UartHandle = UART1;
			uart->irq = UART1_IRQn;
			serial_uart1.ops	 = &same70_uart_ops;
			const Pin pPins3[] = UART1_PINS;
			PIO_Configure(pPins3, PIO_LISTSIZE(pPins3));
			PMC_EnablePeripheral(ID_UART1);
			uart->type_usart = false;
			uart->serial_device = &serial_uart1;
			uart->id = ID_UART1;
			/* register UART1 device */
			rt_hw_serial_register(&serial_uart1, "uart1",
								  RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_DMA_TX | RT_DEVICE_FLAG_DMA_RX,
								  uart);
#endif 
#ifdef RT_USING_UART2
			uart = &uart2;
			uart->UartHandle = UART2;
			uart->irq = UART2_IRQn;
			serial_uart2.ops	 = &same70_uart_ops;
			serial_uart2.config = config;
			const Pin pPins4[] = UART2_PINS;
			PIO_Configure(pPins4, PIO_LISTSIZE(pPins4));
			PMC_EnablePeripheral(ID_UART2);
			uart->type_usart = false;		
			uart->serial_device = &serial_uart2;			
			uart->id = ID_UART2;
			/* register UART2 device */
			rt_hw_serial_register(&serial_uart2, "uart2",
								  RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_DMA_TX | RT_DEVICE_FLAG_DMA_RX,
								  uart);
#endif 
#ifdef RT_USING_UART3
			uart = &uart3;
			uart->UartHandle = UART3;
			uart->irq = UART3_IRQn;
			serial_uart3.ops	 = &same70_uart_ops;
			serial_uart3.config = config;
			const Pin pPins5[] = UART3_PINS;
			PIO_Configure(pPins5, PIO_LISTSIZE(pPins5));
			PMC_EnablePeripheral(ID_UART3);
			uart->type_usart = false;
			uart->serial_device = &serial_uart3;			
			uart->id = ID_UART3;
			/* register UART3 device */
			rt_hw_serial_register(&serial_uart3, "uart3",
								  RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_DMA_TX | RT_DEVICE_FLAG_DMA_RX,
								  uart);
#endif 
#ifdef RT_USING_UART4
			uart = &uart4;
			uart->UartHandle = UART4;
			uart->irq = UART4_IRQn;
			serial_uart4.ops	 = &same70_uart_ops;
			serial_uart4.config = config;
			const Pin pPins6[] = UART4_PINS;
			PIO_Configure(pPins6, PIO_LISTSIZE(pPins6));
			PMC_EnablePeripheral(ID_UART4);
			uart->type_usart = false;
			uart->serial_device = &serial_uart4;
			uart->id = ID_UART4;
			/* register UART4 device */
			rt_hw_serial_register(&serial_uart4, "uart4",
								  RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_DMA_TX | RT_DEVICE_FLAG_DMA_RX,
								  uart);
#endif 

    return 0;
}
INIT_BOARD_EXPORT(same70_hw_usart_init);

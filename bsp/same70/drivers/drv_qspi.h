#ifndef __DRV_QSPI_H
#define __DRV_QSPI_H

#include <rtdevice.h>

#include "rt_board.h"

//#define SPI_USE_DMA

struct same70_qspi_bus
{
    struct rt_spi_bus parent;    
};

struct same70_qspi_cs
{
    //GPIO_TypeDef * GPIOx;
    uint16_t GPIO_Pin;
};

/* public function list */
//rt_err_t stm32_spi_register(SPI_TypeDef * SPI,
//                            struct same70_qspi * stm32_spi,
//                            const char * spi_bus_name);
void rt_hw_spi_init(void);
#endif 

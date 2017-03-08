#ifndef __DRV_SPI_H
#define __DRV_SPI_H

#include "rt_board.h"
#define SPI_DEVICE_SAME70Q21_AD 0
#define AD_SPI_MISO {PIO_PD20B_SPI0_MISO, PIOD, ID_PIOD, PIO_PERIPH_B, PIO_DEFAULT}
#define AD_SPI_MOSI {PIO_PD21B_SPI0_MOSI, PIOD, ID_PIOD, PIO_PERIPH_B, PIO_DEFAULT}
#define AD_SPI_SPCK {PIO_PD22B_SPI0_SPCK, PIOD, ID_PIOD, PIO_PERIPH_B, PIO_DEFAULT}
#define AD_SPI_NPCS2 {PIO_PD12C_SPI0_NPCS2, PIOD, ID_PIOD, PIO_PERIPH_C, PIO_DEFAULT}
static const Pin ad_spi_pins[] = {	AD_SPI_MISO,	AD_SPI_MOSI,	AD_SPI_SPCK,	AD_SPI_NPCS2};

bool spi_init(int SPI_ID, uint8_t cs, uint8_t type);
bool spi_write8bits(int SPI_ID, uint8_t data, uint8_t cs);
bool spi_read8bits(int SPI_ID, uint8_t *data, uint8_t cs);
bool spi_write16bits(int SPI_ID, uint16_t data, uint8_t cs);
bool spi_read16bits(int SPI_ID, uint16_t *data, uint8_t cs);
#endif

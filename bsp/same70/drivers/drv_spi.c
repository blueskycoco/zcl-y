#include <rtthread.h>
#include "board.h"

#include "drv_spi.h"

bool spi_init(int SPI_ID, uint8_t cs, uint8_t type)
{
	if (SPI_ID != 0)
		return false;

	if (cs != 2)
		return false;

	if (type != SPI_DEVICE_SAME70Q21_AD)
		return false;

	if (type == SPI_DEVICE_SAME70Q21_AD)
		PIO_Configure(ad_spi_pins, PIO_LISTSIZE(ad_spi_pins));

	SPI_Configure(SPI0, ID_SPI0, (SPI_MR_MSTR | SPI_MR_MODFDIS					
		| SPI_PCS( 2 )));	
	SPI_ConfigureNPCS( SPI0,			
		2,			
		SPI_DLYBCT( 1000, BOARD_MCK ) |			
		SPI_DLYBS(1000, BOARD_MCK) |			
		SPI_SCBR( 13000000, BOARD_MCK) );
	SPI_Enable(SPI0);
	return true;
}
bool spi_write8bits(int SPI_ID, uint8_t data, uint8_t cs)	
{
	if (SPI_ID != 0)
		return false;

	if (cs != 2)
		return false;
	
	SPI_Write(SPI0, cs , (uint16_t)data);
	return true;
}
bool spi_read8bits(int SPI_ID, uint8_t *data, uint8_t cs)
{
	if (SPI_ID != 0)
		return false;

	if (cs != 2)
		return false;
	
	*data = (uint8_t) SPI_Read(SPI0);
	return true;
}

bool spi_write16bits(int SPI_ID, uint16_t data, uint8_t cs)
{
	if (SPI_ID != 0)
		return false;

	if (cs != 2)
		return false;
	
	SPI_Write(SPI0, cs , data);
	return true;
}

bool spi_read16bits(int SPI_ID, uint16_t *data, uint8_t cs)
{
	if (SPI_ID != 0)
		return false;

	if (cs != 2)
		return false;
	
	*data = (uint16_t) SPI_Read(SPI0);
	return true;
}
#ifdef RT_USING_FINSH
#include <finsh.h>
static void spi_init_t(int SPI_ID, uint8_t cs, uint8_t type)
{
	int result = spi_init(SPI_ID,cs,type);
	if (result == true)
		rt_kprintf("spi_init ok\n");
	else		
		rt_kprintf("spi_init failed\n");
}
static void spi_w8(int SPI_ID, uint8_t data, uint8_t cs)	
{
	int result = spi_write8bits(SPI_ID,data,cs);
	if (result == true)
		rt_kprintf("spi_write8bits ok\n");
	else		
		rt_kprintf("spi_write8bits failed\n");
}
static void spi_w16(int SPI_ID, uint16_t data, uint8_t cs)	
{
	int result = spi_write16bits(SPI_ID,data,cs);
	if (result == true)
		rt_kprintf("spi_write16bits ok\n");
	else		
		rt_kprintf("spi_write16bits failed\n");
}
static void spi_r8(int SPI_ID, uint8_t cs)	
{
	uint8_t data;
	int result = spi_read8bits(SPI_ID,&data,cs);
	if (result == true)
		rt_kprintf("spi_read8bits ok 0x%x\n",data);
	else		
		rt_kprintf("spi_read8bits failed\n");
}
static void spi_r16(int SPI_ID, uint8_t cs)	
{
	uint16_t data;
	int result = spi_read16bits(SPI_ID,&data,cs);
	if (result == true)
		rt_kprintf("spi_read16bits ok 0x%x\n",data);
	else		
		rt_kprintf("spi_read16bits failed\n");
}
FINSH_FUNCTION_EXPORT(spi_init_t, test spi_init);
FINSH_FUNCTION_EXPORT(spi_w8, test spi_write8bits);
FINSH_FUNCTION_EXPORT(spi_r8, test spi_read8bits);
FINSH_FUNCTION_EXPORT(spi_w16, test spi_write16bits);
FINSH_FUNCTION_EXPORT(spi_r16, test spi_read16bits);
#endif

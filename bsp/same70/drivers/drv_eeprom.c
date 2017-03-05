#include <rtthread.h>
#include "board.h"

#include "drv_eeprom.h"
#define AT24MAC_ADDRESS 0x50
#define PAGE_SIZE       8
#define EEPROM_PAGES    256
#define BOARD_PINS_TWI_EEPROM PINS_TWI0
#define BOARD_ID_TWI_EEPROM   ID_TWIHS0
#define BOARD_BASE_TWI_EEPROM TWIHS0
#define TWCK            400000

#define MS561101BA_ADDR   (0xee>>1)
#define MS561101BA_D1_OSR_4096 0x48
#define MS561101BA_D2_OSR_4096 0x58 
#define MS561101BA_PROM_REG_COUNT 6
#define MS561101BA_PROM_BASE_ADDR 0xA0
#define MS561101BA_PROM_REG_SIZE 2
#define MS561101BA_RESET 0x1E

static const Pin pins[] = BOARD_PINS_TWI_EEPROM;
static Twid twid;
static struct rt_mutex i2c_lock;

bool eeprom_init()
{
	if (rt_mutex_init(&i2c_lock, "i2c_lock", RT_IPC_FLAG_FIFO) != RT_EOK)
    {
        rt_kprintf("init i2c lock mutex failed\n");
        return false;
    }
	PIO_Configure(pins, PIO_LISTSIZE(pins));	
	PMC_EnablePeripheral(BOARD_ID_TWI_EEPROM);
	TWI_ConfigureMaster(BOARD_BASE_TWI_EEPROM, TWCK, BOARD_MCK);	
	TWID_Initialize(&twid, BOARD_BASE_TWI_EEPROM);
	return true;
}
RTM_EXPORT(eeprom_init);

bool eeprom_write(int addr, uint8_t *data, uint8_t len)	
{	
	bool result = false;
	rt_mutex_take(&i2c_lock, RT_WAITING_FOREVER);
	if (data != NULL && addr < EEPROM_PAGES && len <= PAGE_SIZE)
		result = !TWID_Write(&twid, AT24MAC_ADDRESS, addr*PAGE_SIZE, 1, data, len, 0);
	rt_mutex_release(&i2c_lock);
	return result;
}
RTM_EXPORT(eeprom_write);
bool eeprom_read(int addr, uint8_t *data, uint8_t len)	
{	
	bool result = false;
	rt_mutex_take(&i2c_lock, RT_WAITING_FOREVER);
	if (data != NULL && addr < EEPROM_PAGES && len <= PAGE_SIZE)
		result = !TWID_Read(&twid, AT24MAC_ADDRESS, addr*PAGE_SIZE, 1, data, len, 0);
	rt_mutex_release(&i2c_lock);
	return result;
}
RTM_EXPORT(eeprom_read);
 
bool ms5611_read(uint16_t *calc, uint32_t *temp, uint32_t *press)
{
	bool result = false;
	rt_uint8_t cmd;
	rt_uint8_t buf[2] = {0};
	rt_mutex_take(&i2c_lock, RT_WAITING_FOREVER);
	/*get calc*/
	cmd = MS561101BA_RESET;
	result = !TWID_Write(&twid, MS561101BA_ADDR, 0, 0, &cmd, 1, 0);
	if (!result)
	{
		rt_kprintf("reset ms5611 failed\n");
		rt_mutex_release(&i2c_lock);
		return result;
	}
	rt_thread_delay(10);
	for (int i=0; i <= MS561101BA_PROM_REG_COUNT; i++) 
	{
		result = !TWID_Read(&twid, MS561101BA_ADDR, MS561101BA_PROM_BASE_ADDR + (i * MS561101BA_PROM_REG_SIZE), 
				1, buf, 2, 0);
		if (result)
			calc[i]=(buf[0]<<8)|buf[1];
		else
		{
			rt_kprintf("get calc failed\n");
			rt_mutex_release(&i2c_lock);
			return result;
		}
	}
	/*get temp*/
	cmd = MS561101BA_D2_OSR_4096;
	result = !TWID_Write(&twid, MS561101BA_ADDR, 0, 0, &cmd, 1, 0);
	if (result)
	{
		rt_thread_delay(10);
		result = !TWID_Read(&twid, MS561101BA_ADDR, 0, 1, (uint8_t *)temp, 3, 0);
	}
	/*get press*/
	cmd = MS561101BA_D1_OSR_4096;
	if (result)
	{
		rt_thread_delay(10);
		result = !TWID_Write(&twid, MS561101BA_ADDR, 0, 0, &cmd, 1, 0);
	}
	if (result)
	{
		rt_thread_delay(10);
		result = !TWID_Read(&twid, MS561101BA_ADDR, 0, 1, (uint8_t *)press, 3, 0);
	}

	if (!result)
	{
		rt_kprintf("TWID failed\n");
	}
	rt_mutex_release(&i2c_lock);
	return result;
}
RTM_EXPORT(ms5611_read);

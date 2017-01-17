#include <rtthread.h>
#include "board.h"

#include "drv_dsram.h"

const Pin pinsMutex[] = {BOARD_MUTEX_PINS};
volatile uint8_t *g_pui8EBISram = (uint8_t *)EBI_CS2_ADDR;

bool dsram_init(void)
{
	const Pin pinsDsram[] = {BOARD_DSRAM_PINS};
	PIO_Configure(pinsDsram, PIO_LISTSIZE(pinsDsram));	
	PIO_Configure(pinsMutex, PIO_LISTSIZE(pinsMutex));	
	PMC_EnablePeripheral(ID_SMC);

	/* Configure SMC, NCS2 is assigned to Dual sram */	
	SMC->SMC_CS_NUMBER[SMC_EBI_DSRAM_CS].SMC_SETUP = SMC_SETUP_NWE_SETUP(2)			
					| SMC_SETUP_NCS_WR_SETUP(0)			
					| SMC_SETUP_NRD_SETUP(0)			
					| SMC_SETUP_NCS_RD_SETUP(0);
	SMC->SMC_CS_NUMBER[SMC_EBI_DSRAM_CS].SMC_PULSE = SMC_PULSE_NWE_PULSE(6)			
					| SMC_PULSE_NCS_WR_PULSE(0xA)			
					| SMC_PULSE_NRD_PULSE(0xA)			
					| SMC_PULSE_NCS_RD_PULSE(0xA);
	SMC->SMC_CS_NUMBER[SMC_EBI_DSRAM_CS].SMC_CYCLE = SMC_CYCLE_NWE_CYCLE(0xA)			
					| SMC_CYCLE_NRD_CYCLE(0xA);
	SMC->SMC_CS_NUMBER[SMC_EBI_DSRAM_CS].SMC_MODE  = 
						SMC_MODE_READ_MODE			|
						SMC_MODE_WRITE_MODE		    |	
						SMC_MODE_DBW_16_BIT		    |							
						SMC_MODE_BAT_BYTE_SELECT	|
						SMC_MODE_TDF_MODE			|
						SMC_MODE_TDF_CYCLES(0xF);
	return true;
}

bool dsram_write(uint8_t addr, uint8_t *data, int len)
{
	if (addr+len > DSRAM_LENGTH)
		return false;
	//while (PIO_Get(&pinsMutex[1]));
	PIO_Set(&pinsMutex[0]);
	rt_memcpy ((void *)(g_pui8EBISram + addr), (const void *)data, len);
	PIO_Clear(&pinsMutex[0]);
	return true;
}
bool dsram_read(uint8_t addr, uint8_t *data, int len)
{	
	if (addr+len > DSRAM_LENGTH)
		return false;
	//while (PIO_Get(&pinsMutex[1]));
	PIO_Set(&pinsMutex[0]);
	rt_memcpy ((void *)data, (const void *)(g_pui8EBISram + addr), len);
	PIO_Clear(&pinsMutex[0]);
	return true;
}
#ifdef RT_USING_FINSH
#include <finsh.h>
static uint8_t data[DSRAM_LENGTH]={0};
static void dsram_init_t(void)
{
	bool result = dsram_init();
	if (result == true)
		rt_kprintf("dsram_init ok\n");
	else		
		rt_kprintf("dsram_init failed\n");
}
static void dsram_read_t(int addr, int len)	
{
	bool result = dsram_read(addr,data, len);
	if (result == true)
	{
		int i;
		rt_kprintf("dsram_read %d :\n", len);
		for (i = 0; i < len; i++)
		{
			rt_kprintf("%02x ", data[i]);
			if ((i+1)%32 == 0)
				rt_kprintf("\n");
		}
		rt_kprintf("\n");
	}
	else		
		rt_kprintf("dsram_read failed\n");
}
static void dsram_write_t(int addr, int begin, int len)	
{
	int i;
	rt_kprintf("dsram_write %d :\n", len);
	for (i = 0; i < len; i++)
	{
		data[i] = begin+i;
		rt_kprintf("%02x ", data[i]);
		if ((i+1)%32 == 0)
			rt_kprintf("\n");
	}
	rt_kprintf("\n");
	bool result = dsram_write(addr,data, len);
	if (result == true)
		rt_kprintf("dsram_write ok\n");
	else		
		rt_kprintf("dsram_write failed\n");
}

FINSH_FUNCTION_EXPORT(dsram_init_t, test dsram_init);
FINSH_FUNCTION_EXPORT(dsram_write_t, test dsram_write);
FINSH_FUNCTION_EXPORT(dsram_read_t, test dsram_read);
#endif


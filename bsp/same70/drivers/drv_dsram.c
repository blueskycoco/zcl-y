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
static volatile uint16_t gs_dualram_buff[384];

uint8_t dual_test(void)
{
	uint16_t i;
	//uint16_t *pul = (uint16_t *)BOARD_DUALRAM_ADDR;
	uint16_t ul_num = 0;

	for (i=0;i<384;i++)
	{
		if (i & 1)
		{
			g_pui8EBISram[i]= 0x55AA;
			} else {
			g_pui8EBISram[i]= 0xAA55;
		}
	}
	
	for (ul_num = 0; ul_num < 384;ul_num++)
	{
		gs_dualram_buff[ul_num] = g_pui8EBISram[ul_num];
	}
	
	for (i = 0; i < 384; i++)
	{
		if (i & 1) {
			if (gs_dualram_buff[i] != 0x55AA)
			{
				rt_kprintf("DUALRAM FAILED 1!");
				return 0;
			}
			} else {
			if (gs_dualram_buff[i] != 0xAA55)
			{
				rt_kprintf("DUALRAM FAILED 2!");
				return 0;
			}
		}
	}
	
	for (i = 0; i < 384; i++)
	{
	g_pui8EBISram[i] = 0;		
	}
	
    rt_kprintf("DUALRAM SUCCESS!");
	return 1;
}
uint32_t dual_test2(void)
{
    uint32_t i;
    uint32_t ret = 1;
	uint32_t baseAddr = EBI_CS2_ADDR;
	uint32_t size=8192;
    uint32_t *ptr32 = (uint32_t *) baseAddr;
    uint16_t *ptr16 = (uint16_t *) baseAddr;
    uint8_t *ptr8 = (uint8_t *) baseAddr;
    /* Test for 55AA55AA/AA55AA55 pattern */
    rt_kprintf(" Test for 55AA55AA/AA55AA55 pattern ... \n\r");

    for (i = 0; i < size; i ++) {
        if (i & 1)
            ptr32[i] = 0x55AA55AA;
        else
            ptr32[i] = 0xAA55AA55;

        memory_barrier()
    }

    for (i = 0; i <  size; i++) {
        if (i & 1) {
            if (ptr32[i] != 0x55AA55AA) {
                rt_kprintf("-E- Expected:%x, read %x @ %x \n\r" ,
                        0xAA55AA55, (unsigned)ptr32[i], (unsigned)(baseAddr + i));
                ret = 0;

            }
        } else {
            if (ptr32[i] != 0xAA55AA55) {
                rt_kprintf("-E- Expected:%x, read %x @ %x \n\r" ,
                        0xAA55AA55 , (unsigned)ptr32[i], (unsigned)(baseAddr + i));
                ret = 0;
            }
        }
    }

    if (!ret) return ret;

    rt_kprintf(" Test for BYTE accessing... \n\r");

    /* Test for BYTE accessing */
    for (i = 0; i < size; i ++)
        ptr8[i] = (uint8_t)(i & 0xFF);

    for (i = 0; i <  size; i++) {
        if (ptr8[i] != (uint8_t)(i & 0xFF))  {
            rt_kprintf("-E- Expected:%x, read %x @ %x \n\r" ,
                    (unsigned)(i & 0xFF), ptr8[i], (unsigned)(baseAddr + i));
            ret = 0;
        }
    }
	
		if (!ret) return ret;
	
		rt_kprintf(" Test for WORD accessing... \n\r");
	
		/* Test for
		 * WORD
		 * accessing
		 * */
		for (i = 0; i < size / 2; i ++)
			ptr16[i] = (uint16_t)(i & 0xFFFF);
	
		for (i = 0; i <  size / 2; i++) {
			if (ptr16[i] != (uint16_t)(i & 0xFFFF))  {
				rt_kprintf("-E- Expected:%x, read %x @ %x \n\r" ,
						(unsigned)(i & 0xFFFF), ptr16[i], (unsigned)(baseAddr + i));
				ret = 0;
			}
		}
	
		if (!ret) return ret;
	
		rt_kprintf(" Test for DWORD accessing... \n\r");
	
		/* Test
		 * for
		 * DWORD
		 * accessing
		 * */
		for (i = 0; i < size / 4; i ++) {
			ptr32[i] = (uint32_t)(i & 0xFFFFFFFF);
			memory_barrier()
		}
	
		for (i = 0; i <  size / 4; i++) {
			if (ptr32[i] != (uint32_t)(i & 0xFFFFFFFF))  {
				rt_kprintf("-E- Expected:%x, read %x @ %x \n\r" ,
						(unsigned)(i & 0xFFFFFFFF), (unsigned)ptr32[i], (unsigned)(baseAddr + i));
				ret = 0;
			}
		}
	rt_kprintf(" Test for dual sram SUCCESS... \n\r");
		return ret;
	}
FINSH_FUNCTION_EXPORT(dsram_init_t, test dsram_init);
FINSH_FUNCTION_EXPORT(dsram_write_t, test dsram_write);
FINSH_FUNCTION_EXPORT(dsram_read_t, test dsram_read);
FINSH_FUNCTION_EXPORT(dual_test, test dsram);
FINSH_FUNCTION_EXPORT(dual_test2, test dsram2);
#endif


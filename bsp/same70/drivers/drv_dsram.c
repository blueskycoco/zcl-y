#include <rtthread.h>
#include "board.h"

#include "drv_dsram.h"

bool dsram_init(void)
{
	const Pin pinsDsram[] = {BOARD_DSRAM_PINS};
	PIO_Configure(pinsDsram, PIO_LISTSIZE(pinsDsram));	
	PMC_EnablePeripheral(ID_SMC);

	/* Configure SMC, NCS2 is assigned to Dual sram */	
	SMC->SMC_CS_NUMBER[SMC_EBI_DSRAM_CS].SMC_SETUP = 
						SMC_SETUP_NWE_SETUP(2)      |
						SMC_SETUP_NCS_WR_SETUP(0)   |			
						SMC_SETUP_NRD_SETUP(0)      |
						SMC_SETUP_NCS_RD_SETUP(0);	
	SMC->SMC_CS_NUMBER[SMC_EBI_DSRAM_CS].SMC_PULSE = 
						SMC_PULSE_NWE_PULSE(6)	    |		
						SMC_PULSE_NCS_WR_PULSE(0xA) |			
						SMC_PULSE_NRD_PULSE(0xA)	|		
						SMC_PULSE_NCS_RD_PULSE(0xA);	
	SMC->SMC_CS_NUMBER[SMC_EBI_DSRAM_CS].SMC_CYCLE = 
						SMC_CYCLE_NWE_CYCLE(0xA)	|		
						SMC_CYCLE_NRD_CYCLE(0xA);	
	SMC->SMC_CS_NUMBER[SMC_EBI_DSRAM_CS].SMC_MODE  = 
						SMC_MODE_READ_MODE			|
						SMC_MODE_WRITE_MODE		    |	
						SMC_MODE_DBW_16_BIT		    |	
						SMC_MODE_EXNW_MODE_DISABLED |			
						SMC_MODE_TDF_CYCLES(0xF);

	return true;
}

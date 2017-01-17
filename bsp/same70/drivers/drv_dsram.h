#ifndef __DRV_DSRAM_H
#define __DRV_DSRAM_H

#include "rt_board.h"
/** SMC pin definition for Dual sram */
/** Dual port sram data pin */
#define PIN_EBI_DSRAM_D0_7        {0xFF, PIOC, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP}
#define PIN_EBI_DSRAM_D8_13       {0x3F, PIOE, ID_PIOE, PIO_PERIPH_A, PIO_PULLUP}
#define PIN_EBI_DSRAM_D14_15      {PIO_PA15A_D14|PIO_PA16A_D15, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_PULLUP}

/** Dual port sram addr pin */
#define PIN_EBI_DSRAM_A1_12   	  {0x7FF80000, PIOC, ID_PIOC, PIO_PERIPH_A, PIO_DEFAULT}

/** Dual port sram WE pin */
#define PIN_EBI_DSRAM_NWE         {PIO_PC8A_NWE,  PIOC, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP}
/** Dual port sram RD pin */
#define PIN_EBI_DSRAM_NRD         {PIO_PC11A_NRD, PIOC, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP}
/*  Dual port sram CS pin (NCS2) */
#define PIN_EBI_DSRAM_CS          {PIO_PA22C_NCS2, PIOA, ID_PIOA, PIO_PERIPH_C, PIO_PULLUP}
/** Dual port sram WR pin */
#define PIN_EBI_DSRAM_NWR         {PIO_PD15C_NWR1,  PIOD, ID_PIOD, PIO_PERIPH_C, PIO_PULLUP}
/** Dual port sram BS pin */
#define PIN_EBI_DSRAM_NBS         {PIO_PC18A_NBS0,  PIOC, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP}

#define BOARD_DSRAM_PINS PIN_SDRAM_D0_7, PIN_EBI_DSRAM_D8_13 , PIN_EBI_DSRAM_D14_15,\
	PIN_EBI_DSRAM_A1_12, PIN_EBI_DSRAM_NWE, PIN_EBI_DSRAM_NRD, \
	PIN_EBI_DSRAM_CS, PIN_EBI_DSRAM_NWR, PIN_EBI_DSRAM_NBS
#define SMC_EBI_DSRAM_CS          2

bool dsram_init(void);
bool dsram_write(uint8_t addr, uint8_t *data, int len);
bool dsram_read(uint8_t addr, uint8_t *data, int len);
#endif


#include <rtthread.h>
#include "board.h"

#include "drv_timer.h"
#define PIN_TC0_CH0   {PIO_PA0B_TIOA0, PIOA, ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT}
#define PIN_TC0_CH1   {PIO_PA1B_TIOB0, PIOA, ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT}
#define PIN_TC2_CH1   {PIO_PC9B_TIOB7, PIOC, ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT}
#define PIN_TC2_CH2   {PIO_PC12B_TIOB8, PIOC, ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT}
static const Pin pPwmpins[] = {PIN_TC0_CH0, PIN_TC0_CH1, PIN_TC2_CH1, PIN_TC2_CH2};


ErrorID GeneratePWMByTimers (int TimerID,int Timer_CHID,int LineID,int Freq,int PulseWidth)
{
	Tc *tc_base;
	uint32_t ra, rc;
	
	if (TimerID != 0 && TimerID !=1 
		&& TimerID !=2 && TimerID != 3)
		return TimesIDError;

	if (Timer_CHID != 0 && Timer_CHID != 1
		&& Timer_CHID != 2)
		return TimesCHIDError;

	if (LineID != 1 && LineID !=2)
		return LineIDError;
	
	if (Freq < 1 || Freq > 1000)
		return WrongFrequency;

	if (PulseWidth < 10 || PulseWidth > 5000)
		return WrongPulseWidth;

	if (Freq == 1000 && PulseWidth >= 1000)
		return WrongPulseWidth;

	if (TimerID == 3)
		tc_base = TC3;
	else
		tc_base = (Tc *)(0x4000C000 + TimerID*4000);
	
	PIO_Configure( pPwmpins, PIO_LISTSIZE(pPwmpins));

	if (TimerID == 0)		
		PMC_EnablePeripheral(ID_TC0);
	else if (TimerID == 2)
	{
		if (Timer_CHID == 1)
			PMC_EnablePeripheral(ID_TC7);
		else if (Timer_CHID == 2)
			PMC_EnablePeripheral(ID_TC8);
	}
	tc_base->TC_CHANNEL[Timer_CHID].TC_CCR = TC_CCR_CLKDIS;	
	tc_base->TC_CHANNEL[Timer_CHID].TC_IDR = 0xFFFFFFFF;	
	tc_base->TC_CHANNEL[Timer_CHID].TC_SR;	

	tc_base->TC_CHANNEL[Timer_CHID].TC_CMR =		
		waveformConfigurations[configuration].clockSelection/* Waveform Clock Selection */
		| TC_CMR_WAVE									 /* Waveform mode is enabled */
		| TC_CMR_ACPA_SET								 /* RA Compare Effect: set */
		| TC_CMR_ACPC_CLEAR 							 /* RC Compare Effect: clear */
		| TC_CMR_CPCTRG;								 /* UP mode with automatic trigger on RC Compare */
	rc = (BOARD_MCK / divisors[waveformConfigurations[configuration].clockSelection]) \
		/ waveformConfigurations[configuration].frequency;
	tc_base->TC_CHANNEL[Timer_CHID].TC_RC = rc;
	ra = (100 - waveformConfigurations[configuration].dutyCycle) * rc / 100;
	tc_base->TC_CHANNEL[Timer_CHID].TC_RA = ra;
	tc_base->TC_CHANNEL[Timer_CHID].TC_CCR =  TC_CCR_CLKEN | TC_CCR_SWTRG;	
	rt_kprintf ("Start waveform: Frequency = %d Hz,Duty Cycle = %2d%%\n\r",			
		waveformConfigurations[configuration].frequency,			
		waveformConfigurations[configuration].dutyCycle);
    return Function_OK;
}
INIT_APP_EXPORT(GeneratePWMByTimers);


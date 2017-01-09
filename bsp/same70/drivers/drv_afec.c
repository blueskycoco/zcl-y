bool AFEC_init(int AFEC_ID, int CH_ID) 
{	
	Afec *afec_base;
	int id;

	if (AFEC_ID != 0 && AFEC_ID != 1)
		return false;

	if (AFEC_ID == 0 && CH_ID != 7 && CH_ID !=8)
		return false;

	if (AFEC_ID ==1 && CH_ID != 6)
		return false;

	if (AFEC_ID ==0)
	{
		afec_base = AFEC0;
		id = ID_AFEC0;		
	}
	else
	{
		afec_base = AFEC1;
		id = ID_AFEC1;
	}
	AFEC_Initialize( afec_base, id );	
	AFEC_SetModeReg(afec_base, 0			
		| AFEC_EMR_RES_NO_AVERAGE			
		| (1 << AFEC_MR_TRANSFER_Pos)			
		| (2 << AFEC_MR_TRACKTIM_Pos)			
		| AFEC_MR_ONE			
		| AFEC_MR_SETTLING_AST3			
		| AFEC_MR_STARTUP_SUT64);	
	AFEC_SetClock(afec_base, 6000000, BOARD_MCK);	
	AFEC_SetExtModeReg(afec_base, 0			
		| AFEC_EMR_RES_NO_AVERAGE			
		| AFEC_EMR_TAG			
		| AFEC_EMR_STM );	
	AFEC_EnableChannel(afec_base, CH_ID);	
	afec_base->AFEC_ACR = AFEC_ACR_IBCTL(2)		
		| (1 << 4)		
		| AFEC_ACR_PGA0_ON		
		| AFEC_ACR_PGA1_ON;	
	AFEC_SetChannelGain(afec_base, AFEC_CGR_GAIN11(0));
	AFEC_SetAnalogOffset(afec_base, CH_ID, 0x200);
}


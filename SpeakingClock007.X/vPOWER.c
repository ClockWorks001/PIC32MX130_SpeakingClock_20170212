/*******************************************************************************
 *	POWER save
 *	author  :hiroshi murakami
 *	created :20161225
 *	modified:-
 ******************************************************************************/
#define _vPOWER_C

#include "mcc_generated_files/mcc.h"
#include "vPOWER.h"

//******************************************************************************
//	Function Name :POWER save
//	Outline		:POWER save
//	Input		:
//	Output		:
//	Return		: wait count until next sleep
//******************************************************************************
int iPowerSave(void)
{
//	CloseTimer4();				//stop SD card reading

	TMR1_Stop();
//	TMR3_Stop();
	
	vADC1_DISEN();
	vSPI2_DISEN();
//	vI2C1_DISEN();
//	vUART2_DISEN();

//    CFGCONbits.IOLOCK = 0;
//    RPB11Rbits.RPB11R = 0x0000;   //clear RB11->UART2:U2RX;
//    CFGCONbits.IOLOCK = 1;
//	  TRISBbits.TRISB11 = 1;
	CNENBbits.CNIEB11 = 1; // Pin : RB11 /  change notice setting for UART pin

	SDCARD_DISEN();
	AUDIO_DISEN();
	vWAVE_close();

	vDisableWDT();

	// Sleep
	//OSCCONbits.SLPEN = 1;	//===>!!! OSCCONbits setup is valid only in the first configurations!!!
	_wait();

	vEnableWDT();
//	AUDIO_EN();
//	SDCARD_EN();
	
	vADC1_EN();
	vSPI2_EN();
//	vI2C1_EN();
//	vUART2_EN();
	TMR1_Start();
	
//    CFGCONbits.IOLOCK = 0;
//    U2RXRbits.U2RXR = 0x0003;   //RB11->UART2:U2RX;
//    CFGCONbits.IOLOCK = 1;
	CNENBbits.CNIEB11 = 0; // Pin : RB11 /  change notice setting for UART pin, dis enable
	return iWait_Count;
}

//******************************************************************************
//	Function Name :PORT Change Notice clear 
//	Outline		:PortCN need equivalent PORT and register.
//              :hens, read PORT temporarily.
//	Input		:
//	Output		:
//******************************************************************************
void vPORTchangeNoticeClear(void)
{
	int temp;
//	if(CNSTATBbits.CNSTATB11 == 1){ 
	if(U2MODEbits.WAKE == 0){		// when RX has signal.. 
		U2MODEbits.WAKE = 1;
		iWait_Count = 2000000;		
	}else{
		iWait_Count = 100;
	}

	temp = PORTA;
	temp = PORTB;
	
}

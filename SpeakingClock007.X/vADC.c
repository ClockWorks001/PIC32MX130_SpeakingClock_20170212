/*******************************************************************************
 *	  ADC control
 *	  author  :hiroshi murakami
 *	  created :20170405
 *	  modified:-
 ******************************************************************************/
#define _vADC_C

#include "mcc_generated_files/mcc.h"
#include "vADC.h"
#include "vTMR1.h"
#include "vSWITCH.h"
#include "vPOWER.h"

//******************************************************************************
// Get a ADC value 
//******************************************************************************
void vADC_open (
	ADC1_CHANNEL channel	
)
{
	ADC1_ChannelSelect(channel);
	ADCtask11.channel  = channel;
	ADCtask11.adcValue = -1;
	ADCtask11.status = eADC_start;
}

//******************************************************************************
// Get a ADC value 
//******************************************************************************
void vADC_task (
)
{
	if(IS_uiTMR005_FINISH){
		uiTMR005 = 1; //msec period
	} else {
		return;
	}

	switch (ADCtask11.status) {
	case	eADC_idle:
		break;
	case	eADC_start:
		ADC1_Start();
		ADCtask11.status++;
		break;
	case	eADC_stop:
		ADC1_Stop();
		ADCtask11.status++;
		break;
	case	eADC_getResult:
		ADCtask11.adcValue = ADC1_ConversionResultGet();
		ADCtask11.status = eADC_idle;
		break;
	default:
		ADCtask11.status = eADC_idle;
		break;
	}
}

//******************************************************************************
// for AN5(VR), RB3 analog EN & pullup DISENABLE
//******************************************************************************
void vADC_AN5_EN_RB3_DISE (
)
{
	ANSELBbits.ANSB3 = 1;
	CNPUBbits.CNPUB3 = 0;
//	CNENBbits.CNIEB3 = 0; // Pin : RB3 change notice dis enable
}
//******************************************************************************
// for SW1, RB3 degital EN & pullup EN
//******************************************************************************
void vADC_RB3_EN_AN5_DISE (
)
{
	ANSELBbits.ANSB3 = 0;
	CNPUBbits.CNPUB3 = 1;
//	CNENBbits.CNIEB3 = 1; // Pin : RB3 change notice enable
}


//******************************************************************************
// initialize SENSE dark & power properties 
//******************************************************************************
void vSENSE_init (
)
{
	SENSEtask12.status = eSENSE_idle;
	SENSEtask12.brightThreshold = 70;
	SENSEtask12.brightValue = 1000;
	SENSEtask12.powerThreshold = -1;
	SENSEtask12.powerValue = 1000;
	cFlagNightSenseON = vFLAG_ON;
}


//******************************************************************************
// SENSE dark & power task
//******************************************************************************
void vSENSE_task (
)
{
	switch (SENSEtask12.status) {
	case	eSENSE_idle:
		break;
		
	case	eSENSE_Start:
		vADC1_EN();
		SENSEtask12.status++;
		break;
		
	case	eSENSE_darkStart:
		vADC_open(ADC1_CHANNEL_AN0);
		SENSEtask12.status++;
		break;
	case	eSENSE_darkWait:
		if(ADCtask11.status == eADC_idle) SENSEtask12.status++;
		break;
	case	eSENSE_darkGetValue:
		SENSEtask12.brightValue = ADCtask11.adcValue;
		SENSEtask12.status++;
		break;
		
	case	eSENSE_powerStart:
		vADC_AN5_EN_RB3_DISE();
		vADC_open(ADC1_CHANNEL_AN5);
		SENSEtask12.status++;
		break;
	case	eSENSE_powerWait:
		if(ADCtask11.status == eADC_idle) SENSEtask12.status++;
		break;
	case	eSENSE_powerGetValue:
		SENSEtask12.powerValue = ADCtask11.adcValue;
		vADC_RB3_EN_AN5_DISE();
		SENSEtask12.status++;
		break;
		
	case	eSENSE_end:
	default:
		vADC1_DISEN();
		SENSEtask12.status = eSENSE_idle;
		break;
	}
}

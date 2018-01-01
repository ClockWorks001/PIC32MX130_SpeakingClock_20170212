/*******************************************************************************
 *	  RTCC処理
 *	  author  :hiroshi murakami
 *	  created :20161225
 *	  modified:-
 ******************************************************************************/
#define _vRTCC_C

#include <string.h>
#include "xprintf.h"
#include "vRTCC.h"
#include "vWAVE.h"
#include "vADC.h"

/*****************************
 * VARIABLES
 *****************************/


//******************************************************************************
/*----------------------------------------------------------*/
/* User Provided RTC Function for FatFs module				*/
/*----------------------------------------------------------*/
/* This is a real time clock service to be called from		*/
/* FatFs module. Any valid time must be returned even if	*/
/* the system does not support an RTC.						*/
/* This function is not required in read-only cfg.			*/
//******************************************************************************
DWORD get_fattime (void)
{
//	struct tm currentTime;
	DWORD tmr;

	RTCC_TimeGet(&currentTime);
	tmr =	(((DWORD)currentTime.tm_year + 2000 - 1980) << 25)
		 | ((DWORD)currentTime.tm_mon << 21)
		 | ((DWORD)currentTime.tm_mday << 16)
		 | (WORD)(currentTime.tm_hour << 11)
		 | (WORD)(currentTime.tm_min << 5)
		 | (WORD)(currentTime.tm_sec >> 1);

	return tmr;
}

//******************************************************************************
//	initialize RTCC property
//
//	Description	: -
//	Input		: -
//	Output		:
//******************************************************************************
void vRTCC_init(void)
{
	vRTCC_alarmTurnOFF();
	vRTCC_chimeTurnOFF();
	alarmTask11.iAlarmRepeat = 10;
	alarmTask11.status =eAlarmT11_idle;
	
	//check the currentTime
	RTCC_TimeGet(&currentTime);
	if(currentTime.tm_hour > 23 || currentTime.tm_min > 59 || currentTime.tm_mon > 12 || currentTime.tm_mday > 31 || currentTime.tm_year > 99)
	{
		// set RTCC time 2017-03-22 22-30-00
		RTCDATE = 0x17050103;
		RTCTIME = 0x12000000;
		RTCCONbits.CAL = 0x3FF;

	} else {
		RTCC_TimeReset(1);
	}
}

//******************************************************************************
//	Interrupt RTCC (call from MCC RTCC interrupt call back routine )
//
//	Description	: -
//	Input		: -
//	Output		:
//******************************************************************************
void Interrupt_RTCC(void)
{
   	cFlag1Minute++;
}

//******************************************************************************
//	when 1 minute go through, this function checks time then speaking timel.
//
//	Description	: It needs Interrupt RTCC which repeate evry 1 minute.
//	Input		: -
//	Output		:
//******************************************************************************
void vRTCC_TimeCheck(void)
{
   	if(!(cFlag1Minute)){
		return;
	}
   	cFlag1Minute = 0 ;

	RTCC_TimeGet(&currentTime);
	xsprintf(bCurrentHHMM, "%02d%02d", currentTime.tm_hour, currentTime.tm_min);
	
	////////////////////////////////////////////////////////////////////////////
	//	alarm time checking
	////////////////////////////////////////////////////////////////////////////
	if(	(RTCC_alarmON) &&
	    (strcmp(bCurrentHHMM, bAlarmHHMM) == 0)){
		alarmTask11.status = eAlarmT11_open;
		return;
	}
	
	////////////////////////////////////////////////////////////////////////////
	//	speaking current time checking
	////////////////////////////////////////////////////////////////////////////
	if(RTCC_chimeOFF) return;
	if((cFlagNightSenseON == vFLAG_ON) && 
		(SENSEtask12.brightValue < SENSEtask12.brightThreshold)
	){
		return;
	}
	
	if(
		(strcmp(bNextHHMM, "0000") < 0) || 
		(strcmp(bNextHHMM, "2359") > 0) ||
		(	(strcmp(bPreviousHHMM, bNextHHMM) < 0) && 
			!((strcmp(bPreviousHHMM, bCurrentHHMM) <= 0) && (strcmp(bCurrentHHMM, bNextHHMM) <= 0))
		)   ||
		(	(strcmp(bPreviousHHMM, bNextHHMM) > 0) && 
			((strcmp(bNextHHMM, bCurrentHHMM) < 0) && (strcmp(bCurrentHHMM, bPreviousHHMM) < 0))
		)
	){
		strcpy(bPreviousHHMM, bCurrentHHMM);
		strcpy(bNextHHMM, bCurrentHHMM);
		pbFFF_get_nextTime(bNextHHMM, bChimeFile);
	}
	
	if(strcmp(bCurrentHHMM, bNextHHMM) == 0){
		vWAVE_words12_open(bCurrentHHMM, bChimeFile);
		if(waveMusic13.status != eWaveM13_idle ) waveMusic13.status = eWaveM13_playNumberOfFiles;
		strcpy(bPreviousHHMM, bCurrentHHMM);
		pbFFF_get_nextTime(bNextHHMM, bChimeFile);
		xprintf("nextTime is %s\n", bNextHHMM);
	}
	return;
}

//******************************************************************************
//	alarm playing
//
//	Description	: -
//	Input		: -
//	Output		:
//******************************************************************************
void vRTCC_alarm11_control(void)
{
	switch(alarmTask11.status){
	case eAlarmT11_idle:
		return;
		break;
	
	case eAlarmT11_open:
		if(waveMusic13.status != eWaveM13_idle )waveMusic13.status = eWaveM13_playNumberOfFiles;
		alarmTask11.iAlarmRepeatCountdown = alarmTask11.iAlarmRepeat;
		alarmTask11.status++;
		break;

	case eAlarmT11_repeat:
		eWAVE_task11_play_number(1, bAlarmPath);
		alarmTask11.status++;
		break;
		
	case eAlarmT11_busy:
		if(waveTask11.status == eWaveT11_idle) {
			alarmTask11.status++;
		}
		break;

	case eAlarmT11_end:
		alarmTask11.iAlarmRepeatCountdown--;
		if(alarmTask11.iAlarmRepeatCountdown == 0){
			alarmTask11.status = eAlarmT11_idle;
		} else {
			alarmTask11.status = eAlarmT11_repeat;
		}
		break;

	default:
		alarmTask11.status == eAlarmT11_idle;
		break;
	}

	return;
}

//******************************************************************************
//	alarm flag turn ON / OFF
//
//	Description	: -
//	Input		: -
//	Output		:
//******************************************************************************
void vRTCC_alarmTurnON(void)
{
   	alarm.cFlagON = vFLAG_ON;
   	alarm.cLCDoutPutChar = 'a';
}

void vRTCC_alarmTurnOFF(void)
{
   	alarm.cFlagON = vFLAG_OFF;
   	alarm.cLCDoutPutChar = ' ';
}

//******************************************************************************
//	alarm flag turn ON / OFF
//
//	Description	: -
//	Input		: -
//	Output		:
//******************************************************************************
void vRTCC_chimeTurnON(void)
{
   	chime.cFlagON = vFLAG_ON;
   	chime.cLCDoutPutChar = 'c';
}

void vRTCC_chimeTurnOFF(void)
{
   	chime.cFlagON = vFLAG_OFF;
   	chime.cLCDoutPutChar = ' ';
}

//******************************************************************************
//	RTCC time setting
//
//	Description	: -
//	Input		: -
//	Output		:
//******************************************************************************
void vRTCC_timeSet(
	struct tm setupTime
)
{
	short	cal;
	int		cTime, sTime;
	RTCC_TimeGet(&currentTime);
	cTime = ((currentTime.tm_hour * 0x100) + currentTime.tm_min) * 0x100 + currentTime.tm_sec;
	sTime = ((setupTime.tm_hour * 0x100) + setupTime.tm_min) * 0x100 + setupTime.tm_sec;
	cal = RTCCONbits.CAL << 6;
	if(cTime < sTime){
		cal = cal + (1 << 6);
	} else if(cTime > sTime){
		cal = cal - (1 << 6);
	}
	RTCCONbits.CAL = cal >> 6;
	currentTime.tm_hour = setupTime.tm_hour;
	currentTime.tm_min  = setupTime.tm_min;
	currentTime.tm_sec  = setupTime.tm_sec;
	RTCC_TimeSet(&currentTime);			
}

void vRTCC_dateSet(
	struct tm setupTime
)
{
		RTCC_TimeGet(&currentTime);
		currentTime.tm_year = setupTime.tm_year;
		currentTime.tm_mon  = setupTime.tm_mon;
		currentTime.tm_mday = setupTime.tm_mday;
		RTCC_TimeSet(&currentTime);			//日付時間をRTCCモジュールに設定する
}


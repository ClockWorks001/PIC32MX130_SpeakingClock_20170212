/*******************************************************************************
 *	  LCD output menu
 *	  author  :hiroshi murakami
 *	  created :20170309
 *	  modified:-
 ******************************************************************************/
#define _vLCD_MENU_C

//#include "mcc_generated_files/mcc.h"
#include "vTMR1.h"
#include "vRTCC.h"
#include "vWAVE.h"
#include "vSWITCH.h"
#include "vLCD_MENU.h"
#include "vADC.h"
#include "vPOWER.h"

//******************************************************************************
//	display menu open. 
//	Description	:
//	Precondition:
//	Input		: 
//	Parameters	:
//	Returns		:
//******************************************************************************
void vLCD_menu12_open(
)
{
	vLCD_lines_init();
	LcdMenu12.status = eLcdM12_displayTimeDateOut;
	uiTMR007 = 2000; //msec period
}

//******************************************************************************
//	display menu close. 
//	Description	:
//	Precondition:
//	Input		: 
//	Parameters	:
//	Returns		:
//******************************************************************************
void vLCD_menu12_close(
)
{
	LcdMenu12.status = eLcdM12_idle;
}


/*****************************
* LcdMenu12.speakON
*****************************/
char cSpeakON(void)
{
	if(LcdMenu12.speakON == vFLAG_ON){
		LcdMenu12.speakON = vFLAG_OFF;
		return 1;
	}
	return 0;
}

//******************************************************************************
//	LCD line2 output & speaking 
//	Description	:
//	Precondition: this Function called from "eLcdM12_menuSelect" ONLY!!
//	Input		: 
//	Parameters	:
//	Returns		:
//******************************************************************************
void vLCD_LL2_print(
	char* str
)
{
	xsprintf(LL2.bLineBuffer, str);
	if(cSpeakON())	vWAVE_words12_open(str, bMenuFile);
}

//******************************************************************************
//	LCD line1 output & speaking 
//	Description	:
//	Precondition: this Function called after "Flags setted up" ONLY!!
//	Input		: 
//	Parameters	:
//	Returns		:
//******************************************************************************
void vLCD_LL1_print_and_End(
	char* str
)
{
		xsprintf(LL1.bLineBuffer, str);
		iLCD_scroll_line(&LL1);
		if(cSpeakON())	vWAVE_words12_open(str, bMenuFile);
		uiTMR007 = 2000; //msec period
		LcdMenu12.status = eLcdM12_menuSelectEndWait;
		iScrollCount = 0;
}

//******************************************************************************
//	display menu control. LCD menu control. 
//	Description	:
//	Precondition: open vLCD_task11_open, vLCD_menu12_open.
//	Input		: 
//	Parameters	:
//	Returns		:
//******************************************************************************
void vLCD_menu12_control(void)
{
	static int i;
	int a,b;
	
	if(IS_uiTMR007_FINISH){
		uiTMR007 = 250; //msec period
	} else {
		return;
	}
	
	// status change check
	if( (LcdMenu12.jump != LcdMenu12.pre_jump)		||
		(LcdMenu12.status != LcdMenu12.pre_status)	||
		(LCD_contrast != pre_LCD_contrast)			||
		(alarmTask11.iAlarmRepeat != alarmTask11.pre_iAlarmRepeat)			||
		((
		  setupTime.tm_hour + setupTime.tm_min + setupTime.tm_sec + setupTime.tm_year + setupTime.tm_mon + setupTime.tm_mday
		  ) != ( 
		  pre_setupTime.tm_hour + pre_setupTime.tm_min + pre_setupTime.tm_sec  + pre_setupTime.tm_year + pre_setupTime.tm_mon + pre_setupTime.tm_mday
		))
	){
		LcdMenu12.pre_jump = LcdMenu12.jump;
		LcdMenu12.pre_status = LcdMenu12.status;
		pre_LCD_contrast = LCD_contrast;
		alarmTask11.pre_iAlarmRepeat = alarmTask11.iAlarmRepeat;
		pre_setupTime = setupTime;
		LcdMenu12.speakON = vFLAG_ON;
	}
	
	switch (LcdMenu12.status) {
	case 	eLcdM12_idle:
		LcdMenu12.status++;
		uiTMR007 = 0; //msec period
		break;
			
	case	eLcdM12_displayTimeDateOut:
		vLCD_blinkOFF();
		RTCC_TimeGet(&currentTime);
		xsprintf(LL1.bLineBuffer, "Time %02d:%02d:%02d", currentTime.tm_hour, currentTime.tm_min, currentTime.tm_sec);
		iLCD_scroll_line(&LL1);
		if(SENSEtask12.powerValue < vPOWER_4V_INTEGER){
			a = vPOWER_get_volt(SENSEtask12.powerValue);
			b = vPOWER_get_pvolt(SENSEtask12.powerValue);
			xsprintf(LL2.bLineBuffer, "LOW POWER %d.%dv", a, b);
		}else{
			xsprintf(LL2.bLineBuffer, "Date %02d/%02d/%02d %c%c", currentTime.tm_year, currentTime.tm_mon, currentTime.tm_mday, chime.cLCDoutPutChar, alarm.cLCDoutPutChar);
		}
		iLCD_scroll_line(&LL2);
		LcdMenu12.status = eLcdM12_idle;
		break;
		
	case	eLcdM12_menuSelectOpen:
		i = 1;
		cSW2_ON();		// switch clear
		cSW3_ON();		// switch clear
		vLCD_lines_init();
		vLCD_blinkOFF();
		xsprintf(LL1.bLineBuffer, "Menu");
		uiTMR007 = 50; //msec period
		LcdMenu12.status++;
		//break;
	case	eLcdM12_menuSelect:
		if(cSW2_ON()) i++;
		if(cSW3_ON()) i--;

		switch (i){
		case	0:
			i = 12;
			break;
		case	1:
			LcdMenu12.jump = eLcdM12_playStart;
			vLCD_LL2_print("Play_Music?");
			break;
		case	2:
			LcdMenu12.jump = eLcdM12_playMusicFromFirst;
			vLCD_LL2_print("Play_Music_1st?");
			break;
		case	3:
			LcdMenu12.jump = eLcdM12_alarmON;
			vLCD_LL2_print("Alarm_ON?");
			break;
		case	4:
			LcdMenu12.jump = eLcdM12_alarmOFF;
			vLCD_LL2_print("Alarm_OFF?");
			break;
		case	5:
			LcdMenu12.jump = eLcdM12_chimeON;
			vLCD_LL2_print("Chime_ON?");
			break;
		case	6:
			LcdMenu12.jump = eLcdM12_chimeOFF;
			vLCD_LL2_print("Chime_OFF?");
			break;
		case	7:
			LcdMenu12.jump = eLcdM12_nightSenseON;
			vLCD_LL2_print("Night_Sense_ON?");
			break;
		case	8:
			LcdMenu12.jump = eLcdM12_nightSenseOFF;
			vLCD_LL2_print("Night_Sense_OFF?");
			break;
		case	9:
			LcdMenu12.jump = eLcdM12_LCDcontrast;
			vLCD_LL2_print("LCD_Contrast?");
			break;
		case	10:
			LcdMenu12.jump = eLcdM12_dateSetup;
			vLCD_LL2_print("Date_Setup?");
			break;
		case	11:
			LcdMenu12.jump = eLcdM12_timeSetup;
			vLCD_LL2_print("Time_Setup?");
			break;
		case	12:
			LcdMenu12.jump = eLcdM12_alarmSetup;
			vLCD_LL2_print("Alarm_Setup?");
			break;
//		case	13:
//			LcdMenu12.jump = eLcdM12_ADCtest;
//			xsprintf(LL2.bLineBuffer, "ADC test?");
//			break;
			
		default:
			i = 1;
			break;
		}
		iLCD_scroll_line(&LL1);
		iLCD_scroll_line(&LL2);
		break;
		
	case	eLcdM12_menuSelectJump:
		LcdMenu12.status = LcdMenu12.jump;
		break;

	case	eLcdM12_playMusicFromFirst:
			waveMusic13.fnumber = 1; 
			LcdMenu12.status++;					//to next status
			break;
		case	eLcdM12_playStart:
			vLCD_lines_init();
			xsprintf(LL1.bLineBuffer, "Play Music");	
			vWAVE_music13_open(waveMusic13.fnumber, "/music");
			xsprintf(LL2.bLineBuffer, "%d/%d", waveMusic13.fnumber, waveMusic13.maxFnumber);
			LcdMenu12.status++;					//to next status
			break;
			
		case	eLcdM12_playing:
			if(cSW2_ON()){
				waveMusic13.fnumber++;
				LcdMenu12.status = eLcdM12_playStart;
				uiTMR007 = 0; //msec period
				break;
			}
			if(cSW3_ON()){
				waveMusic13.fnumber--;
				if(waveMusic13.fnumber == 0) waveMusic13.fnumber = waveMusic13.maxFnumber;
				LcdMenu12.status = eLcdM12_playStart;
				uiTMR007 = 0; //msec period
				break;
			}
			if(waveTask11.status == eWaveT11_error){
				xsprintf(LL2.bLineBuffer, "SD CARD ERROR");
				iLCD_scroll_line(&LL2);
				LcdMenu12.status = eLcdM12_playStop;
				uiTMR007 = 5000; //msec period
				break;
			}
			if(waveMusic13.status == eWaveM13_idle){
				LcdMenu12.status = eLcdM12_playStop;
				break;
			}
//			RTCC_TimeGet(&currentTime);
//			xsprintf(LL1.bLineBuffer, "Time %02d:%02d:%02d", currentTime.tm_hour, currentTime.tm_min, currentTime.tm_sec);
			xsprintf(LL2.bLineBuffer, "%d/%d %s ", waveMusic13.fnumber, waveMusic13.maxFnumber, waveTask11.fileNames);
			iLCD_scroll_line(&LL2);
			iLCD_scroll_line(&LL1);
			uiTMR007 = 800; //msec period
			break;
			
		case	eLcdM12_playNext:
				waveMusic13.fnumber++;
				LcdMenu12.status = eLcdM12_playStart;
				uiTMR007 = 0; //msec period
			break;
			
		case	eLcdM12_playStop:
			vWAVE_stop();
			vLCD_LL1_print_and_End("Play_Stop!!");
			LcdMenu12.status = eLcdM12_end;
			break;

	case	eLcdM12_alarmON:
		vRTCC_alarmTurnON();
		vLCD_LL1_print_and_End("Alarm_ON");
		break;
	case	eLcdM12_alarmOFF:
		vRTCC_alarmTurnOFF();
		vLCD_LL1_print_and_End("Alarm_OFF");
		break;
	case	eLcdM12_chimeON:
		vRTCC_chimeTurnON();
		vLCD_LL1_print_and_End("Chime_ON");
		break;
	case	eLcdM12_chimeOFF:
		vRTCC_chimeTurnOFF();
		vLCD_LL1_print_and_End("Chime_OFF");
		break;
	case	eLcdM12_nightSenseON:
		cFlagNightSenseON = vFLAG_ON;
		vLCD_LL1_print_and_End("Night_Sense_ON");
		iScrollCount = 24;
		break;
	case	eLcdM12_nightSenseOFF:
		cFlagNightSenseON = vFLAG_OFF;
		vLCD_LL1_print_and_End("Night_Sense_OFF");
		iScrollCount = 24;
		break;

	case	eLcdM12_LCDcontrast:
		vLCD_lines_init();
		xsprintf(LL1.bLineBuffer, "Seting LCD");
		iLCD_scroll_line(&LL1);
		LcdMenu12.status++;
		if(cSpeakON())	vWAVE_words12_open("LCDcontrast", bMenuFile);
		break;
		
		case	eLcdM12_LCDcontrastSetup:
			LCD_contrast = iAddNumber(LCD_contrast, cSW2_ON(), 10, 60);
			LCD_contrast = iSubNumber(LCD_contrast, cSW3_ON(), 10, 60);
		    vLCD_cmd(0x39);              // 8bit 2line Extend mode
			vLCD_cmd(0x70 + (LCD_contrast & 0x0F));
			vLCD_cmd(0x5C + ((LCD_contrast >> 4) & 0x03));
		    vLCD_cmd(0x38);              // 8bit 2line Normal mode
			xsprintf(LL2.bLineBuffer, "LCD Contrast %02d",  LCD_contrast);
			iLCD_scroll_line(&LL2);
	        vLCD_printAt(LCD_LINE2,14);
			vLCD_blinkON();
			if(cSpeakON()){
				xsprintf(bMenuWords, "%02d eLcdM12_LCDcontrastSetup",  LCD_contrast);
				vWAVE_words12_open(bMenuWords, bMenuFile);
			}
			break;
		case	eLcdM12_LCDcontrastEnd:
			xsprintf(LL1.bLineBuffer, "Contrast Setted UP!!");
			iLCD_scroll_line(&LL1);
			vLCD_blinkOFF();
			uiTMR007 = 2000; //msec period
			LcdMenu12.status = eLcdM12_end;
			if(cSpeakON()){
				xsprintf(bMenuWords, "%02d eLcdM12_LCDcontrastEnd",  LCD_contrast);
				vWAVE_words12_open(bMenuWords, bMenuFile);
			}
			break;

	case	eLcdM12_timeSetup:
		vLCD_lines_init();
		RTCC_TimeGet(&setupTime);
		xsprintf(LL1.bLineBuffer, "Seting Time");
		iLCD_scroll_line(&LL1);
		LcdMenu12.status++;
		if(cSpeakON())	vWAVE_words12_open("timeSetup", bMenuFile);
		break;
		
		case	eLcdM12_timeHHsetup:
			setupTime.tm_hour = iAddNumber(setupTime.tm_hour, cSW2_ON(), 0, 23);
			setupTime.tm_hour = iSubNumber(setupTime.tm_hour, cSW3_ON(), 0, 23);
			xsprintf(LL2.bLineBuffer, "Time %02d:%02d:%02d", setupTime.tm_hour, setupTime.tm_min, setupTime.tm_sec);
			iLCD_scroll_line(&LL2);
	        vLCD_printAt(LCD_LINE2,6);
			vLCD_blinkON();
			if(cSpeakON()){
				xsprintf(bMenuWords, "%02dhour",  setupTime.tm_hour);
				vWAVE_words12_open(bMenuWords, bMenuFile);
			}
			break;
		case	eLcdM12_timeMMsetup:
			setupTime.tm_min = iAddNumber(setupTime.tm_min, cSW2_ON(), 0, 59);
			setupTime.tm_min = iSubNumber(setupTime.tm_min, cSW3_ON(), 0, 59);
			xsprintf(LL2.bLineBuffer, "Time %02d:%02d:%02d", setupTime.tm_hour, setupTime.tm_min, setupTime.tm_sec);
			iLCD_scroll_line(&LL2);
	        vLCD_printAt(LCD_LINE2,9);
			vLCD_blinkON();
			if(cSpeakON()){
				xsprintf(bMenuWords, "%02dmin",  setupTime.tm_min);
				vWAVE_words12_open(bMenuWords, bMenuFile);
			}
			break;
		case	eLcdM12_timeSSsetup:
			setupTime.tm_sec = iAddNumber(setupTime.tm_sec, cSW2_ON(), 0, 59);
			setupTime.tm_sec = iSubNumber(setupTime.tm_sec, cSW3_ON(), 0, 59);
			xsprintf(LL2.bLineBuffer, "Time %02d:%02d:%02d", setupTime.tm_hour, setupTime.tm_min, setupTime.tm_sec);
			iLCD_scroll_line(&LL2);
	        vLCD_printAt(LCD_LINE2,12);
			vLCD_blinkON();
			if(cSpeakON()){
				xsprintf(bMenuWords, "%02dsec",  setupTime.tm_sec);
				vWAVE_words12_open(bMenuWords, bMenuFile);
			}
			break;
		case	eLcdM12_timeSetupEnd:
			vRTCC_timeSet(setupTime);
			xsprintf(LL1.bLineBuffer, "Time Setted UP!!");
			iLCD_scroll_line(&LL1);
			vLCD_blinkOFF();
			uiTMR007 = 2000; //msec period
			LcdMenu12.status = eLcdM12_end;
			if(cSpeakON()){
				xsprintf(bMenuWords, "%02dhour %02dmin %02dsec timeSetupEnd", setupTime.tm_hour, setupTime.tm_min, setupTime.tm_sec);
				vWAVE_words12_open(bMenuWords, bMenuFile);
			}
			break;

	case	eLcdM12_dateSetup:
		vLCD_lines_init();
		RTCC_TimeGet(&setupTime);
		xsprintf(LL1.bLineBuffer, "Seting Date");
		iLCD_scroll_line(&LL1);
		LcdMenu12.status++;
		if(cSpeakON())	vWAVE_words12_open("dateSetup", bMenuFile);
		break;
		case	eLcdM12_dateYYsetup:
			setupTime.tm_year = iAddNumber(setupTime.tm_year, cSW2_ON(), 0, 99);
			setupTime.tm_year = iSubNumber(setupTime.tm_year, cSW3_ON(), 0, 99);
			xsprintf(LL2.bLineBuffer, "Date %02d/%02d/%02d", setupTime.tm_year, setupTime.tm_mon, setupTime.tm_mday);
			iLCD_scroll_line(&LL2);
	        vLCD_printAt(LCD_LINE2,6);
			vLCD_blinkON();
			if(cSpeakON()){
				xsprintf(bMenuWords, "%02dyear",  setupTime.tm_year);
				vWAVE_words12_open(bMenuWords, bMenuFile);
			}
			break;
		case	eLcdM12_dateMMsetup:
			setupTime.tm_mon = iAddNumber(setupTime.tm_mon, cSW2_ON(), 1, 12);
			setupTime.tm_mon = iSubNumber(setupTime.tm_mon, cSW3_ON(), 1, 12);
			xsprintf(LL2.bLineBuffer, "Date %02d/%02d/%02d", setupTime.tm_year, setupTime.tm_mon, setupTime.tm_mday);
			iLCD_scroll_line(&LL2);
	        vLCD_printAt(LCD_LINE2,9);
			vLCD_blinkON();
			if(cSpeakON()){
				xsprintf(bMenuWords, "%02dmon",  setupTime.tm_mon);
				vWAVE_words12_open(bMenuWords, bMenuFile);
			}
			break;
		case	eLcdM12_dateDDsetup:
			if(setupTime.tm_mon == 2 && setupTime.tm_year % 4 > 0){
				setupTime.tm_mday = iAddNumber(setupTime.tm_mday, cSW2_ON(), 1, 28); 
				setupTime.tm_mday = iSubNumber(setupTime.tm_mday, cSW3_ON(), 1, 28);
			} else {
				setupTime.tm_mday = iAddNumber(setupTime.tm_mday, cSW2_ON(), 1, check_date[setupTime.tm_mon]);
				setupTime.tm_mday = iSubNumber(setupTime.tm_mday, cSW3_ON(), 1, check_date[setupTime.tm_mon]);
			}
			xsprintf(LL2.bLineBuffer, "Date %02d/%02d/%02d", setupTime.tm_year, setupTime.tm_mon, setupTime.tm_mday);
			iLCD_scroll_line(&LL2);
	        vLCD_printAt(LCD_LINE2,12);
			vLCD_blinkON();
			if(cSpeakON()){
				xsprintf(bMenuWords, "%02dmday",  setupTime.tm_mday);
				vWAVE_words12_open(bMenuWords, bMenuFile);
			}
			break;
		case	eLcdM12_dateSetupEnd:
			vRTCC_dateSet(setupTime);
			xsprintf(LL1.bLineBuffer, "Date Setted UP!!");
			iLCD_scroll_line(&LL1);
			vLCD_blinkOFF();
			uiTMR007 = 2000; //msec period
			LcdMenu12.status = eLcdM12_end;
			if(cSpeakON()){
				xsprintf(bMenuWords,  "%02dyear %02dmon %02dmday dateSetupEnd", setupTime.tm_year, setupTime.tm_mon, setupTime.tm_mday);
				vWAVE_words12_open(bMenuWords, bMenuFile);
			}
			break;

	case	eLcdM12_alarmSetup:
		vLCD_lines_init();
		RTCC_TimeGet(&alarmTime);
		xsprintf(LL1.bLineBuffer, "Seting Alarm");
		iLCD_scroll_line(&LL1);
		LcdMenu12.status++;
		break;
		
		case	eLcdM12_alarmHHsetup:
			setupTime.tm_hour = iAddNumber(setupTime.tm_hour, cSW2_ON(), 0, 23);
			setupTime.tm_hour = iSubNumber(setupTime.tm_hour, cSW3_ON(), 0, 23);
			xsprintf(LL2.bLineBuffer, "Alarm %02d:%02d", setupTime.tm_hour, setupTime.tm_min);
			iLCD_scroll_line(&LL2);
	        vLCD_printAt(LCD_LINE2,7);
			vLCD_blinkON();
			if(cSpeakON()){
				xsprintf(bMenuWords, "%02dhour",  setupTime.tm_hour);
				vWAVE_words12_open(bMenuWords, bMenuFile);
			}
			break;
		case	eLcdM12_alarmMMsetup:
			setupTime.tm_min = iAddNumber(setupTime.tm_min, cSW2_ON(), 0, 59);
			setupTime.tm_min = iSubNumber(setupTime.tm_min, cSW3_ON(), 0, 59);
			xsprintf(LL2.bLineBuffer, "Alarm %02d:%02d", setupTime.tm_hour, setupTime.tm_min);
			iLCD_scroll_line(&LL2);
	        vLCD_printAt(LCD_LINE2,10);
			vLCD_blinkON();
			if(cSpeakON()){
				xsprintf(bMenuWords, "%02dmin",  setupTime.tm_min);
				vWAVE_words12_open(bMenuWords, bMenuFile);
			}
			break;
		case	eLcdM12_alarmRepeat:
			alarmTask11.iAlarmRepeat = iAddNumber(alarmTask11.iAlarmRepeat, cSW2_ON(), 1, 99);	
			alarmTask11.iAlarmRepeat = iSubNumber(alarmTask11.iAlarmRepeat, cSW3_ON(), 1, 99);
			xsprintf(LL2.bLineBuffer, "Repeat:%3d", alarmTask11.iAlarmRepeat);
			iLCD_scroll_line(&LL2);
//	        vLCD_printAt(LCD_LINE2,14);
			vLCD_blinkOFF();
			if(cSpeakON()){
				xsprintf(bMenuWords, "%02d kai",  alarmTask11.iAlarmRepeat);
				vWAVE_words12_open(bMenuWords, bMenuFile);
			}
			break;
		case	eLcdM12_alarmSetupEnd:
			alarmTime.tm_hour = setupTime.tm_hour;
			alarmTime.tm_min  = setupTime.tm_min;
			alarmTime.tm_sec  = 0;
			xsprintf(bAlarmHHMM, "%02d%02d", alarmTime.tm_hour, alarmTime.tm_min);
			xsprintf(LL1.bLineBuffer, "Alarm Setted UP!!");
			iLCD_scroll_line(&LL1);
			xsprintf(LL2.bLineBuffer, "Alarm %02d:%02d r%d", alarmTime.tm_hour, alarmTime.tm_min,  alarmTask11.iAlarmRepeat);
			iLCD_scroll_line(&LL2);
			vLCD_blinkOFF();
			vRTCC_alarmTurnON();
			uiTMR007 = 2000; //msec period
			LcdMenu12.status = eLcdM12_end;
			if(cSpeakON()){
				xsprintf(bMenuWords,  "%02dhour %02dmin %02d kai alarmSetupEnd", alarmTime.tm_hour, alarmTime.tm_min,  alarmTask11.iAlarmRepeat);
				vWAVE_words12_open(bMenuWords, bMenuFile);
			}
			break;

			
#if _V_DEBUG
	case	eLcdM12_ADCtest:
		xsprintf(LL1.bLineBuffer, "ADC TEST PW/CDS");
		iLCD_scroll_line(&LL1);
//		uiTMR007 = 30; //msec period
		LcdMenu12.status++;
		case	eLcdM12_ADCopen:
			vSENSE_start();
//			uiTMR007 = 30; //msec period
			LcdMenu12.status++;
			break;
		case	eLcdM12_ADCdisplay:
			xsprintf(LL2.bLineBuffer, "A5%4d / A0%4d", SENSEtask12.powerValue, SENSEtask12.brightValue);
			iLCD_scroll_line(&LL2);
//			uiTMR007 = 30; //msec period
			
			if(cSW3_ON()){
				LcdMenu12.status = eLcdM12_ADCend;
			}else{
				LcdMenu12.status = eLcdM12_ADCopen;
			}
			break;
		case	eLcdM12_ADCend:
			vADC_RB3_EN_AN5_DISE();
			LcdMenu12.status = eLcdM12_end;
			break;
#endif

	case	eLcdM12_menuSelectEndWait:
		// scroll and wait
		if(iScrollCount-- <= 0){
			LcdMenu12.status = eLcdM12_end;
			uiTMR007 = 1500; //msec period
			iScrollCount = 0;
			break;
		}
		iLCD_scroll_line(&LL1);
		iLCD_scroll_line(&LL2);
		uiTMR007 = 700; //msec period
		break;

			
	case	eLcdM12_end:
		vLCD_blinkOFF();
		LcdMenu12.status = eLcdM12_displayTimeDateOut;
		break;
	
	default:
		LcdMenu12.status = eLcdM12_end;
		break;
	}

}


//******************************************************************************
//   Add Number and return
//
//   Description   : 時刻日付の設定用。
//                 : 時間・番号を受け取り、指定の範囲で加算して返信する。
//                 : 時間・番号を加算した結果が、指定範囲を超えていた場合は最小値を返す。
//   Precondition  :
//   Input         : iNowNumber:加算する元
//                 : iAddNumber:加算値
//                 : iMinNumber:指定の範囲、最小値
//                 : iMaxNumber:指定の範囲、最大値
//   Output        : 算出した値
//******************************************************************************
int iAddNumber(int iNowNumber, int iAddNumber, int iMinNumber, int iMaxNumber)
{
	iNowNumber = iNowNumber + iAddNumber;
	if(iNowNumber > iMaxNumber){
		return iMinNumber;
	}else{
		return iNowNumber;
	}

}

//******************************************************************************
//   Subtract Number and return
//
//   Description   : 時刻日付の設定用。
//                 : 時間・番号を受け取り、指定の範囲で減算して返信する。
//                 : 時間・番号を減算した結果が、指定範囲を超えていた場合は最大値を返す。
//   Precondition  :
//   Input         : iNowNumber:減算する元
//                 : iAddNumber:減算値
//                 : iMinNumber:指定の範囲、最小値
//                 : iMaxNumber:指定の範囲、最大値
//   Output        : 算出した値
//******************************************************************************
int iSubNumber(int iNowNumber, int iSubNumber, int iMinNumber, int iMaxNumber)
{
	iNowNumber = iNowNumber - iSubNumber;
	if(iNowNumber < iMinNumber){
		return iMaxNumber;
	}else{
		return iNowNumber;
	}
}


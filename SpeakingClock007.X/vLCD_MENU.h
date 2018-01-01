/*******************************************************************************
 *	I2C LCD charactor munu task
 *	I2C1 use
 *	author  :hiroshi murakami
 *	created :20170305
 *	modified:-
 ******************************************************************************/
#ifndef _vLCD_MENU_H
#define _vLCD_MENU_H

#include "mcc_generated_files/mcc.h"
#include "integer.h"
#include "v.h"
#include "vLCD_I2C.h"

/*****************************
 * VARIABLES
 *****************************/
// LCD Menu12
typedef enum {
	// ■LCD　menu
	eLcdM12_idle,			// idle表示
			
	eLcdM12_displayTimeDateOut,

	eLcdM12_menuSelectOpen,	//メニュー選択表示
	eLcdM12_menuSelect,		//メニュー選択表示
	eLcdM12_menuSelectJump,		//メニュー選択表示

	eLcdM12_playMusicFromFirst,		//playing music 表示
		eLcdM12_playStart,			//start表示
		eLcdM12_playing,			//playin表示
		eLcdM12_playNext,
		eLcdM12_playStop,

	eLcdM12_alarmON,		//アラームON
	eLcdM12_alarmOFF,		//アラームOFF

	eLcdM12_chimeON,		//チャイムON
	eLcdM12_chimeOFF,		//チャイムOFF
	eLcdM12_nightSenseON,	//センサーON
	eLcdM12_nightSenseOFF,	//センサーOFF

	eLcdM12_LCDcontrast,		//LCDコントラスト設定表示
		eLcdM12_LCDcontrastSetup,	//コントラスト設定
		eLcdM12_LCDcontrastEnd,	//設定完了
			
	eLcdM12_timeSetup,		//時間設定表示
		eLcdM12_timeHHsetup,		//時設定
		eLcdM12_timeMMsetup,		//分設定
		eLcdM12_timeSSsetup,		//秒設定
		eLcdM12_timeSetupEnd,		//設定完了

	eLcdM12_dateSetup,		//日付設定表示
		eLcdM12_dateYYsetup,		//年設定
		eLcdM12_dateMMsetup,		//月設定
		eLcdM12_dateDDsetup,		//日設定
		eLcdM12_dateSetupEnd,		//設定完了
			
	eLcdM12_alarmSetup,		//アラーム設定表示
		eLcdM12_alarmHHsetup,		//時設定
		eLcdM12_alarmMMsetup,		//分設定
		eLcdM12_alarmRepeat,		//秒設定
		eLcdM12_alarmSetupEnd,		//設定完了
			
	eLcdM12_ADCtest,	
		eLcdM12_ADCopen,
		eLcdM12_ADCdisplay,
		eLcdM12_ADCend,

	eLcdM12_menuSelectEndWait,		//wait for menu select.
	eLcdM12_end

} LCD_MENU12_STATUS;

typedef struct {
	LCD_MENU12_STATUS status;
	LCD_MENU12_STATUS jump;
	LCD_MENU12_STATUS pre_status;
	LCD_MENU12_STATUS pre_jump;
	int speakON;		// for voice menu
} LCD_MENU12_PROPERTY;

#if defined _vLCD_MENU_C
	LCD_MENU12_PROPERTY LcdMenu12;
	BYTE check_date[13] =
		  {0,31,29,31,30,31,30,31,31,30,31,30,31};
		//{0,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10,11,12}
	BYTE bMenuFile[] = "LCDmenu.txt";
	BYTE bMenuWords[128];
	int	iScrollCount;

#else
	extern LCD_MENU12_PROPERTY LcdMenu12;
#endif

	
/*****************************
 * PROTOTYPES
 *****************************/
void vLCD_data(unsigned char data);
void vLCD_menu12_open(void);
void vLCD_menu12_close(void);
void vLCD_menu12_control(void);	// LCD menu control
int iAddNumber(int iNowNumber, int iAddNumber, int iMinNumber, int iMaxNumber);
int iSubNumber(int iNowNumber, int iSubNumber, int iMinNumber, int iMaxNumber);
char cSpeakON(void);
void vLCD_LL2_print(char* str);	//called from "eLcdM12_menuSelect" ONLY!!
void vLCD_LL1_print_and_End(char* str);	//called after "Flags setted up" ONLY!!


#endif

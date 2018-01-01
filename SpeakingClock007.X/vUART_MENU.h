/*******************************************************************************
 *	UART Moniter Menu
 *	author  :hiroshi murakami
 *	created :20161225
 *	modified:-
 ******************************************************************************/
#ifndef _vUART_MENU_H
#define _vUART_MENU_H

#include "ff.h"		//FatFs定義体取得の為
#include "v.h"
#include "vUART_CMND.h"
#include "vWAVE.h"
#include "vLCD_MENU.h"

/*****************************
 * VARIABLES
 *****************************/
enum eModeC2
{
	eModeC2_menu_msg1,					//Start message
	eModeC2_menu_init,
	eModeC2_vGetsCMND,
	eModeC2_vExecCmd,					//コマンド分岐
	eModeC2_Command_error,
	eModeC2_end
};


#if defined _vUART_MENU_C
	enum eModeC2 eModeStatusC2;
	unsigned char *ucPtr1, *ucPtr2;			// Pointer for Byte data
#else
	extern enum eModeC2 eModeStatusC2;
#endif


/*****************************
 * PROTOTYPES
 *****************************/
void vUART_menu_init(void);
void vUART_menu11_control(void);		//serial menu control.

void vCommand_TimeSet(void);
void vCommand_DateSet(void);
void vCommand_AlarmSet(void);
void vCommand_RTCCFlagSet(void);
void vCommand_CheckAndTest(void);

void vComandFormatCheck_7Digit(void);//Comand format check 7digit
void vShowNowTimeMessage(void);		//時間の表示

void vCommand_Help(void);
void vCommand_FatFs(void);			//FatFs command control
void vCommand_WAVE(void);				//Wave command control
void vCommand_FFF(void);				//FFF command control
void vCommand_Status(void);			//Status output control

void vXputs_return(void);
void vXputs_commandError(char* str);
void vXputs_FatFsCommandError(void);
void vXputs_FormatError(void);
void put_rc(FRESULT rc);				//ステータスOUTPUT

void put_sts_eWaveT11(WAVE_TASK11_STATUS rc);
void put_sts_eWaveW12(WAVE_TASK12_STATUS rc);
void put_sts_eWaveM13(WAVE_TASK13_STATUS rc);

void put_sts_eLcdT11(LCD_TASK11_STATUS rc, I2C1_MESSAGE_STATUS pstatus);


#endif

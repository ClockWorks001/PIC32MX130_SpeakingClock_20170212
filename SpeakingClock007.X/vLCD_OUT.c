/*******************************************************************************
 *	I2C LCD charactor display output control
 *	I2C1 use
 *	author  :hiroshi murakami
 *	created :20170305
 *	modified:-
 ******************************************************************************/
#define _vLCD_OUT_C

#include <string.h>
#include "vLCD_I2C.h"
#include "vLCD_OUT.h"
#include "vRTCC.h"

//******************************************************************************
// 1行目の先頭から出力
//******************************************************************************
void vLcdOutLine(
	int iLine,
	BYTE *bLine_buff
)
{
	int l;
	vLCD_printAt(iLine,0);	// move to "iLine" head.

//	l = strlen(bLine_buff);
//	vLCD_str(bLine_buff);		//lcd2行目へ出力
  
	l = 0;
	while(*bLine_buff && l < LCD_LINE_SIZE){
		vLCD_data(*bLine_buff);
		bLine_buff++;
		l++;
	}
	while(l < LCD_LINE_SIZE){	//表示の後ろの部分にspace詰め
        vLCD_data(' ');			//space出力
		l++;
    }

}

//******************************************************************************
// bufferの指定位置からから出力
//******************************************************************************
void vLcdShiftLine(
	int iLine, 
	BYTE* pbLine_buff, 
	int iShiftBytes
)
{
	int i, length;
	BYTE *ptr;
	
	length = strlen(pbLine_buff);

	if(length >= LCD_LINE_SIZE) iShiftBytes = 0;
	if(iShiftBytes >= LCD_LINE_SIZE) iShiftBytes = 0;
	if(iShiftBytes >= length) iShiftBytes = 0;
	
	vLCD_printAt(iLine,0);	// move to "iLine" head.

	ptr = pbLine_buff + iShiftBytes;
	for(i = 0; i < LCD_LINE_SIZE ; i++){
		if(i >= length){
			vLCD_data(' ');
		}else if(*ptr){
			vLCD_data(*ptr);
			ptr++;
		} else {
			vLCD_data(' ');
			ptr = pbLine_buff;
		}
	}
}

////*****************************
//// write the Menu on LCD
////*****************************
//void vLCD_L1buff_TimeSet(void)
//{
//	xsprintf(bLCD_Lin1_buff, "Time %02u:%02u:%02u ",ucHour,ucMin,ucSec);
//	if(eModeLcdL1Status == eModeL1_pose){			//一定時間隔起動が停止なら
//		vLcdOutLine(LCD_LINE1,bLCD_Lin1_buff);				//LCD1行目へ出力
//	}
//}

////*****************************
//// write the Menu on LCD
////*****************************
//void vLCD_L2buff_DateSet(void)
//{
//	xsprintf(bLCD_Lin2_buff, "Date %02u-%2u-%2u ",ucYear,ucMonth,ucDay);
//	if(eModeLcdL2Status == eModeL2_pose){			//一定時間隔起動が停止なら
//		vLcdOutLine(LCD_LINE2,bLCD_Lin2_buff);				//LCD2行目へ出力
//	}
//}

//******************************************************************************
// bufferのnullクリア
//******************************************************************************
void vLcdBuffClear(
	BYTE *bLine_buff
)
{
	int i;
	for(i=0; i < LCD_BUFFER_SIZE; *(bLine_buff + i++) = '\0');
}

//******************************************************************************
//   Lcd Display mode control 01
//
//   Description   : 表示のコントロール用。
//                 : 0.5秒間隔で表示。
//   Precondition  :
//   Input         : bLCD_Lin1_buff :LCDの1行目
//                 : bLCD_Lin2_buff :LCDの2行目
//   Output        : LCD
//******************************************************************************
void vModeLcdControl01(void)
{
	static int iL1Cposition;
	static int iL2Cposition;

	switch (eModeLcdL1Status) {
	case  	eModeL1_TimeOut:
//		vGetsRTCC_Time();					// RTCCモジュールから時間を取得する。
		vLCD_L1buff_TimeSet();
		vLcdOutLine(LCD_LINE1,bLCD_Lin1_buff);
		break;
	case  	eModeL1_pose:					//表示処理を switch_menuの設定に任せる状態
		break;
	default:
		eModeLcdL1Status = eModeL1_TimeOut;
		break;
	}

	switch (eModeLcdL2Status) {
	case  	eModeL2_DateOut :
//		vGetsRTCC_Date();					// RTCCモジュールから日付を取得する。
		vLCD_L2buff_DateSet();
		vLcdOutLine(LCD_LINE2,bLCD_Lin2_buff);
		break;
	case  	eModeL2_pose:					//表示処理を switch_menuの設定に任せる状態
		break;
	case	eModeL2_shift_L_pre:
		//表示を左に動かす準備
		iL2Cposition = 0;
		eModeLcdL2Status++;
		break;
	case	eModeL2_shift_L_repeat:
		//表示を左に動かす動作中
		if(bLCD_Lin2_buff[iL2Cposition]=='\0'){
			iL2Cposition = 0;
		}
		vLcdShiftLine(LCD_LINE2, bLCD_Lin2_buff, iL2Cposition);
		iL2Cposition++;
		break;
	default:
		eModeLcdL2Status = eModeL2_DateOut;
		break;
	}

}

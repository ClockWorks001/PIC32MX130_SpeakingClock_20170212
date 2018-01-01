/*******************************************************************************
 *	I2C LCD charactor display output control
 *	I2C1 use
 *	author  :hiroshi murakami
 *	created :20170305
 *	modified:-
 ******************************************************************************/
#ifndef _vLCD_OUT_H
#define _vLCD_OUT_H

#include "integer.h"

//pushbuttons define
#define LCD_BUFFER_SIZE 30				//LCD display buffer size
#define LCD_LINE1 0						//LCD の1行目
#define LCD_LINE2 1						//LCD の2行目

enum eModeL1 {
	eModeL1_TimeOut,					//一定周期で16ケタフルに表示する。
	eModeL1_pose,						//一定周期は停止、表示は設定メニュー側で行う
	eModeL1_end
};
enum eModeL2 {
	eModeL2_DateOut,					//一定周期で16ケタフルに表示する。
	eModeL2_pose,						//一定周期は停止、表示は設定メニュー側で行う
	eModeL2_shift_L_pre,
	eModeL2_shift_L_repeat,				//表示を左に流す。
	eModeL2_end
};

/*****************************
** 定数変数定義
*****************************/
#if defined _vLCD_OUT_C

	BYTE bLCD_Lin1_buff[LCD_BUFFER_SIZE + 1];			// LCD buffer
	BYTE bLCD_Lin2_buff[LCD_BUFFER_SIZE + 1];			// LCD buffer
	enum eModeL1 eModeLcdL1Status;
	enum eModeL2 eModeLcdL2Status;

#else
	extern BYTE bLCD_Lin1_buff[];
	extern BYTE bLCD_Lin2_buff[];
	extern enum eModeL1 eModeLcdL1Status;
	extern enum eModeL2 eModeLcdL2Status;
#endif

/*****************************
*  プロトタイプ定義ファイル
*****************************/
void vLcdOutLine(int iline, BYTE *bLine_buff);	//2行目の先頭から出力
void vLCD_L1buff_TimeSet(void);
void vLCD_L2buff_DateSet(void);
void vLcdShiftLine(int iline, BYTE *bLine_buff, int iPosition); //LCDのシフト表示用
void vModeLcdControl01(void);			//lcd表示タスク


#endif

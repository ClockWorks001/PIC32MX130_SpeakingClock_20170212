/*******************************************************************************
 *	I2C LCD charactor display task
 *	I2C1 use
 *	author  :hiroshi murakami
 *	created :20170305
 *	modified:-
 ******************************************************************************/
#ifndef _vLCD_I2C_H
#define _vLCD_I2C_H

#include "mcc_generated_files/mcc.h"
#include "integer.h"
#include "vFIFO.h"

#define LCD_LINE_SIZE 16				//LCD の1行の表示文字数
#define LCD_LINE1 0						//LCD の1行目
#define LCD_LINE2 1						//LCD の2行目

#define LCD_LINE_BUFFER_SIZE 64
#define LCDDATA 0x40
#define LCDCMD	0x00
#define LCDWAIT	0xFF
#define LCDPOWERON	0xFE
#define LCDPOWEROFF	0xFD

#define ICON_ANT 		0	// アンテナ
#define ICON_TEL 		1	// 電話
#define ICON_RADIO 		2	// 無線
#define ICON_JACK 		3	// ジャック
#define ICON_UP 		4	// △
#define ICON_DOWN 		5	// ▽
#define ICON_UPDOWN		6	// △▽
#define ICON_KEY 		7	// 鍵
#define ICON_PIN 		8	// ピン
#define ICON_BAT_EMPTY 	9	// 電池無し
#define ICON_BAT_LOW 	10	// 容量少
#define ICON_BAT_MID 	11	// 容量中
#define ICON_BAT_HIGH 	12	// 容量多
#define ICON_ROUND	 	13	// 丸
#define ICON_ON	 		1
#define ICON_OFF	 	0

/*****************************
 * VARIABLES
 *****************************/
// LCD Task11 for i2c command
typedef enum {
	eLcdT11_idle,					// 1.idl
	eLcdT11_LcdCommandPut,			// 2.Lcd command put out.
	eLcdT11_wait,					// 3.waiting.
	eLcdT11_nextCommand,			// 4.next command.
	eLcdT11_end,
	eLcdT11_fail,
	eLcdT11_close
} LCD_TASK11_STATUS;

// for LCD Task11 i2c command task
typedef struct {
	//for i2c1.c 
	BYTE data[2];
	I2C1_MESSAGE_STATUS pstatus;
	//LCD command FIFO
	FIFO_STRUCT fifo;
	BYTE buff[LCD_LINE_SIZE * 6 * 2 * 3];	// Task11 command & data buffer (6=command 2byte,data 2byte,wait 2byte) * 2Line *3margin
	LCD_TASK11_STATUS status;
} LCD_TASK11_PROPERTY;

typedef struct {
	int iLine;
	BYTE bLineBuffer[LCD_LINE_BUFFER_SIZE];
	int iShiftBytes;
} LCD_LINE_PROPERTY;

#if defined _vLCD_I2C_C
	//  LCD i2c setting
	//I2C_7_BIT_ADDRESS   SlaveAddress;
	#define LCD_ADDRESS		0b00111110

	//#define CONTRAST	0x18	// for 5.0V
	#define CONTRAST  40		// for 3.3V (max=0x3F=63)
	BYTE LCD_contrast = CONTRAST;
	BYTE pre_LCD_contrast = CONTRAST;

	//　ICONS 
	const unsigned char ICON[14][2]={
		{0x00, 0x10},		// アンテナ
		{0x02, 0x10},		// 電話
		{0x04, 0x10},		// 無線
		{0x06, 0x10},		// ジャック
		{0x07, 0x10},		// △
		{0x07, 0x08},		// ▽
		{0x07, 0x18},		// △▽
		{0x09, 0x10},		// 鍵
		{0x0B, 0x10},		// ピン
		{0x0D, 0x02},		// 電池無し
		{0x0D, 0x12},		// 容量少
		{0x0D, 0x1A},		// 容量中
		{0x0D, 0x1E},		// 容量多
		{0x0F, 0x10}		// 丸
	};
	
	LCD_TASK11_PROPERTY LcdTask11;
	LCD_LINE_PROPERTY LL1;
	LCD_LINE_PROPERTY LL2;
#else
	extern BYTE LCD_contrast;
	extern BYTE pre_LCD_contrast;
	extern LCD_TASK11_PROPERTY LcdTask11;
	extern LCD_LINE_PROPERTY LL1;
	extern LCD_LINE_PROPERTY LL2;
#endif

	
/*****************************
 * PROTOTYPES
 *****************************/
void vLCD_powerON();
void vLCD_powerOFF();
void vLCD_wait(BYTE wait);
void vLCD_data(unsigned char data);
void vLCD_cmd(unsigned char cmd);
void vLCD_init(void);
void vLCD_clear(void);
void vLCD_printAt(unsigned char ucLine, unsigned char ucColumn);
void vLCD_str(const unsigned char* ptr);
void vLCD_icon(unsigned char num, unsigned char onoff);
void vLCD_blinkON(void);
void vLCD_blinkOFF(void);

void vLCD_lines_init(void);
int iLCD_scroll_line(LCD_LINE_PROPERTY* ll);

void vLCD_task11_open(void);
void vLCD_task11_close(void);
void vLCD_task11_control(void);	//LCD command & data byte stream output.

#define LCD_POWER_EN()		LATBbits.LATB12 = 1
#define	LCD_POWER_DISEN()	LATBbits.LATB12 = 0
#define	vLCD_clear()	 vLCD_cmd(0x01)
#define	vLCD_blinkON()	 vLCD_cmd(0x0F)
#define	vLCD_blinkOFF()	 vLCD_cmd(0x0C)



#endif

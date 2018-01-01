/*******************************************************************************
 *	I2C LCD charactor display task
 *	I2C1 use
 *	author  :hiroshi murakami
 *	created :20170305
 *	modified:-
 ******************************************************************************/
#define _vLCD_I2C_C

#include <string.h>
#include "xprintf.h"
#include "vLCD_I2C.h"
#include "vTMR1.h"

//******************************************************************************
// "Power ON" command into I2C FIFO buffer.
//******************************************************************************
void vLCD_powerON(
)
{
	eFIFO_putc(&LcdTask11.fifo, LCDPOWERON);	// Power ON command for task11
	eFIFO_putc(&LcdTask11.fifo, 0);
}
//******************************************************************************
// "Power OFF" command into I2C FIFO buffer.
//******************************************************************************
void vLCD_powerOFF(
)
{
	eFIFO_putc(&LcdTask11.fifo, LCDPOWEROFF);	// Power OFF command for task11
	eFIFO_putc(&LcdTask11.fifo, 0);
}

//******************************************************************************
// "wait" msec into I2C FIFO buffer.
//******************************************************************************
void vLCD_wait(
	BYTE wait
)
{
	eFIFO_putc(&LcdTask11.fifo, LCDWAIT);	// LCDWAIT is waiting command for task11
	eFIFO_putc(&LcdTask11.fifo, wait);
}

//******************************************************************************
// output 1 character to I2C FIFO buffer.
//******************************************************************************
void vLCD_data(
	BYTE data
)
{
	eFIFO_putc(&LcdTask11.fifo, LCDDATA);
	eFIFO_putc(&LcdTask11.fifo, data);
	vLCD_wait(1);
}

//******************************************************************************
// output 1 command data to I2C FIFO buffer.
//******************************************************************************
void vLCD_cmd(
	unsigned char cmd
)
{
	eFIFO_putc(&LcdTask11.fifo, LCDCMD);
	eFIFO_putc(&LcdTask11.fifo, cmd);
	// Clear or Home
    if((cmd == 0x01)||(cmd == 0x02)){
		vLCD_wait(2);				// wait 2msec
    } else {
		vLCD_wait(1);				// wait 30usec though 1msec
    }
}

//******************************************************************************
// initialize LCD command.
//******************************************************************************
void vLCD_init(void)
{
	vFIFO_init(&LcdTask11.fifo, sizeof(LcdTask11.buff), LcdTask11.buff);
	LcdTask11.data[0] = 0x00;
	LcdTask11.data[1] = 0x00;
    vLCD_powerOFF();
	vLCD_wait(100);
	vLCD_wait(100);
	vLCD_powerON();
	
    vLCD_wait(100);
    vLCD_wait(100);
    vLCD_cmd(0x38);              // 8bit 2line Normal mode
    vLCD_cmd(0x39);              // 8bit 2line Extend mode
    vLCD_cmd(0x14);              // BIAS 1/5 CLK 380Khz
    /* コントラスト設定 */
    vLCD_cmd(0x70 + (LCD_contrast & 0x0F));
//    vLCD_cmd(0x5C + (LCD_contrast >> 4));
	vLCD_cmd(0x5C + ((LCD_contrast >> 4) & 0x03));
//  vLCD_cmd(0x6A);              // Follower for 5.0V
	vLCD_cmd(0x6B);              // Follower for 3.3V
    vLCD_wait(100);
    vLCD_cmd(0x38);              // Set Normal mode
    vLCD_cmd(0x0C);              // Display On
    vLCD_cmd(0x01);              // Clear Display
    vLCD_wait(100);
}

//******************************************************************************
// move cursor position on LCD display.
// ucLine 	: 0 -> Line 1
// 			: 1 -> Line 2
// ucColumn	: 0 -> column 1
//******************************************************************************
void vLCD_printAt(
	unsigned char ucLine, 
	unsigned char ucColumn
)
{
	switch (ucLine) {
	case  	LCD_LINE1 :			// Line 1
	    vLCD_cmd(0b10000000 | (0x00 + ucColumn)); // add Column position
		break;
	default:					// Line 2
	    vLCD_cmd(0b10000000 | (0x40 + ucColumn)); // add Column position
	}
}

//******************************************************************************
// output string to LCD.
//******************************************************************************
void vLCD_str(
	const unsigned char *ptr
)
{
	int i;
	i = 0;
    while(*ptr && i < LCD_LINE_SIZE){        		
        vLCD_data(*ptr++);				// LCD data output byte by byte.
		i++;
    }
}

//******************************************************************************
// icons dislpay 
//******************************************************************************
void vLCD_icon(
	unsigned char num, 
	unsigned char onoff
)
{
    vLCD_cmd(0x39);              // Extend mode
    vLCD_cmd(0x40 | ICON[num][0]);   // set icon address
    if(onoff){
        vLCD_data(ICON[num][1]); // icon on data
    }else{
        vLCD_data(0x00);         // icon off
    }
    vLCD_cmd(0x38);              // Normal Mode
}

//******************************************************************************
//	LCD command task open. LCD task open. 
//
//	Description	:
//	 1.initialize FIFO buffer.
//	 2.status initialize.
//	Precondition:
//	Input		: 
//	Parameters	: 
//	Returns		:
//******************************************************************************
void vLCD_task11_open(void)
{

	vLCD_init();
	
	LcdTask11.status = eLcdT11_idle;
	LcdTask11.pstatus = I2C1_MESSAGE_COMPLETE;

}

//******************************************************************************
//	LCD command task close. I2C disenable. Power disenable.  
//
//	Description	:
//	 1.initialize FIFO buffer.
//	 2.status initialize.
//	Precondition:
//	Input		: 
//	Parameters	: 
//	Returns		:
//******************************************************************************
void vLCD_task11_close(void)
{
	vFIFO_init(&LcdTask11.fifo, sizeof(LcdTask11.buff), LcdTask11.buff);
	I2C1CONbits.ON = 0;
	LCD_POWER_DISEN();
	LcdTask11.status = eLcdT11_close;
	LcdTask11.pstatus = I2C1_MESSAGE_COMPLETE;

}

//******************************************************************************
//	LCD command & data byte stream output then wait command done. LCD task control. 
//
//	Description	:
//	 1.LCD BUFFER data format = 2 byte (command, data )
//	 2.command (0x00:command, 0x40:data, 0xFF:wait *0xFF is special command for this object )
//	 3.wait until command done.
//	 4.next command.
//	Precondition: open task11 
//	Input		: Fifo buffer
//	Parameters	: 
//	Returns		:
//******************************************************************************
void vLCD_task11_control(void)
{

	switch (LcdTask11.status) {
	case	eLcdT11_idle:					
		if(iFIFO_remains(&LcdTask11.fifo)) {
			LcdTask11.status = eLcdT11_LcdCommandPut;
		}
		break;
		
	case	eLcdT11_LcdCommandPut:			// 2.get command & data then put into I2C.
		if(LcdTask11.pstatus == I2C1_MESSAGE_COMPLETE){

			LcdTask11.data[0] = bFIFO_getc(&LcdTask11.fifo);
			LcdTask11.data[1] = bFIFO_getc(&LcdTask11.fifo);
			if(LcdTask11.data[0] == LCDCMD || LcdTask11.data[0] == LCDDATA){
				I2C1_MasterWrite(
					LcdTask11.data,
					2,
					LCD_ADDRESS,
					&LcdTask11.pstatus
				);
			}else if(LcdTask11.data[0] == LCDPOWERON) {
				LCD_POWER_EN();
				I2C1CONbits.ON = 1;
				I2C1_Initialize();
			}else if(LcdTask11.data[0] == LCDPOWEROFF) {
				LcdTask11.pstatus = I2C1_MESSAGE_COMPLETE;
				I2C1CONbits.ON = 0;
				LCD_POWER_DISEN();
			}else if(LcdTask11.data[0] == LCDWAIT) {
				uiTMR006 = (unsigned int)LcdTask11.data[1];	//wait msec set
				LcdTask11.status =eLcdT11_wait;
				break;
			}else{
				LcdTask11.status = eLcdT11_fail;
				break;
			}
		}else if(LcdTask11.pstatus == I2C1_MESSAGE_FAIL){
			LcdTask11.status = eLcdT11_fail;
			break;
		}
		LcdTask11.status = eLcdT11_nextCommand;	
		break;

	case	eLcdT11_wait:						//wait until idle.
		if(IS_uiTMR006_FINISH){
			LcdTask11.status = eLcdT11_idle;	
		}
		break;

	case	eLcdT11_nextCommand:
		LcdTask11.status = eLcdT11_idle;
		break;
		
	case	eLcdT11_end:
		LcdTask11.status = eLcdT11_idle;
		break;

	case	eLcdT11_fail:
		vLCD_task11_open();
		LcdTask11.status = eLcdT11_end;
		break;

	case	eLcdT11_close:
		if(iFIFO_remains(&LcdTask11.fifo)) {
			vFIFO_init(&LcdTask11.fifo, sizeof(LcdTask11.buff), LcdTask11.buff);
		}
		break;

	default:
		LcdTask11.status = eLcdT11_end;
		break;
	}

}

//******************************************************************************
// initialize LCD_LINE_PROPERTY
//******************************************************************************
void vLCD_lines_init(void)
{
	int i;
	LL1.iLine = LCD_LINE1;
	LL1.iShiftBytes = 0;
	for( i = 0 ; i < LCD_LINE_BUFFER_SIZE ; i++) LL1.bLineBuffer[i] = '\0';
	LL2.iLine = LCD_LINE2;
	LL2.iShiftBytes = 0;
	for( i = 0 ; i < LCD_LINE_BUFFER_SIZE ; i++) LL2.bLineBuffer[i] = '\0';
}

//******************************************************************************
//	output a line data to LCD. this Function is for scrolling lines.
//
//	Description	:
//	 1.output a line data to LCD.
//	 2.output position is "iLine" line and "0" column.
//	 3."iShiftBytes" shift a line data.
//	Precondition:
//	Input		: 
//	Parameters	:
//	Returns		: shift bytes. if it's over return "0".
//******************************************************************************
int iLCD_scroll_line(
	LCD_LINE_PROPERTY* llp
)
{
	int i, length;
	BYTE *ptr;
	
	length = strlen(llp->bLineBuffer);

	if(llp->iShiftBytes > 0) {
		if(length <= LCD_LINE_SIZE) llp->iShiftBytes = 0;
		if(llp->iShiftBytes >= length) llp->iShiftBytes = 0;
	}
	vLCD_printAt(llp->iLine,0);	// move to "iLine" head.

	ptr = llp->bLineBuffer + llp->iShiftBytes;		// shift line buffer
	for(i = 0; i < LCD_LINE_SIZE ; i++){
		if(i >= length){
			vLCD_data(' ');
		}else if(*ptr){
			vLCD_data(*ptr);
			ptr++;
		} else {
			ptr = llp->bLineBuffer;  //return to first byte of line
			vLCD_data(' ');
		}
	}
	if(length > LCD_LINE_SIZE) llp->iShiftBytes++;
	return llp->iShiftBytes;
}

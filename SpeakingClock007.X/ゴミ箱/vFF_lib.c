/*******************************************************************************
 *	File 処理
 *	新規:20140114
 *	修正:-
 ******************************************************************************/
#define FF_LIB

//#include <plib.h>            /* Include to use PIC32 peripheral libraries      */
//#include <sys/attribs.h>     /* For __ISR definition                          */
//#include "system.h"

#include "integer.h"
#include "xprintf.h"
#include "ffconf.h"		//FatFs定義体取得の為
#include "ff.h"			//FatFs定義体取得の為
#include "vERROR_CODE.h"
#include "vFF_lib.h"

/*****************************
 * 2byteの順序をひっくり返す。
 *****************************/
WORD wSwab_w(unsigned char *A){
	union SWAB_W_DATA{
		unsigned short usData;
		unsigned char ucData[2];
	} ;
	union SWAB_W_DATA B;
	B.ucData[0] = *(A);
	B.ucData[1] = *(++A);
	return B.usData;
}

/*****************************
 * 4byteの順序をひっくり返す。
 *****************************/
DWORD dwSwab_dw(unsigned char *A){
	union SWAB_DW_DATA{
		unsigned long ulData;
		unsigned char ucData[4];
	} ;
	union SWAB_DW_DATA B;
	B.ucData[0] = *(A);
	B.ucData[1] = *(++A);
	B.ucData[2] = *(++A);
	B.ucData[3] = *(++A);
	return B.ulData;
//	return	(DWORD)(((DWORD)*(A+3)<<24)|((DWORD)*(A+2)<<16)|((WORD)*(A+1)<<8)|*A);
}

//******************************************************************************
//* First Read
//*
//* Description   : read First file Sector
//* Output        : buffer
//* Input         : reading SD card
//******************************************************************************
int iFF_FirstRead(FATFILE* ff, const TCHAR* bFileName){
	FRESULT res;
	ff->dwReadIndex = 0;
	ff->iSDReading = SD_READ_OFF;
	res = f_mount(&FatFs, "", 1);
	if(res) return res;
	res = f_open(&ff->fObject, bFileName, FA_READ);
	if(res) return res;
	return iFF_ReadNextSector(ff);
}

//******************************************************************************
//* Read next Sector
//*
//* Description   : read next file Sector
//* Output        : buffer
//* Input         : reading SD card
//******************************************************************************
int iFF_ReadNextSector(FATFILE* ff){
	FRESULT res;
	UINT uiS1;
	DWORD dwBuffSector;		// File_Object.fptrのあるsector番号 0,1,2,3,4,,,
	SHORT shBuffLine;		// File_Object.fptrのあるBuffer番号 0,1
	// 読み込み中かチェック
	if (ff->iSDReading) return WAITING;
	// fileObjectが有効化かチェック
	if (ff->fObject.fs == 0 ) return EOF;
	// fileが最後まで読まれたかチェック
	if (ff->fObject.fptr >= ff->fObject.fsize ) return EOF;

	// 次のバッファにデータを読み込んで良いかチェック
	// ReadIndexが追いついたら、次の512byteを読み込む
	if((ff->dwReadIndex + (SD_BUFFER_SIZE * (SD_BUFFER_LINE - 1)) >= ff->fObject.fptr) ){

		ff->iSDReading = SD_READ_ON;	//読み込み開始
		dwBuffSector = ff->fObject.fptr / SD_BUFFER_SIZE;	//SDを読込むfptrが何sector読み込んだか計算
		shBuffLine = dwBuffSector %  SD_BUFFER_LINE;		//SDを読込む次のbuffer0,1を計算
		res = f_read(&(ff->fObject), &(ff->bBuff[shBuffLine][0]), SD_BUFFER_SIZE, &uiS1);

		ff->iSDReading = SD_READ_OFF;	//読み込み終了
		if(res) return res;
	}
	return NO_ERROR;
}


//******************************************************************************
//* get 1 byte from bBuff
//*
//* Description   : get 1 byte from bBuff
//* Output        : 1byte character
//* Input         : Pointer to the file object
//******************************************************************************
int iFF_Getc(FATFILE* ff){
	short shReadIndex;
	short shRIndexLine;		// ReadIndexのあるLine番号 0,1

	//while(ff->dwReadIndex >= ff->fObject.fptr);

	if(ff->dwReadIndex >= ff->fObject.fsize) return EOF;

	shRIndexLine = (ff->dwReadIndex / SD_BUFFER_SIZE) %  SD_BUFFER_LINE;	//ReadIndexが何sector読み出したか計算し、lineの番号を求める。
	shReadIndex = ff->dwReadIndex % SD_BUFFER_SIZE ;	//ReadIndexからbuffの読み出し位置を計算

	ff->dwReadIndex++;
	return ff->bBuff[shRIndexLine][shReadIndex];
}

//******************************************************************************
//* get 1 line1 and set a buff
//*
//* Description   :  get 1 line1 and set a buff
//******************************************************************************
int iFF_Get_Line(
BYTE* buff,			//	Output	: Pointer to the line characters buffer
int len,			//	Input	: Output line buffer size
FATFILE* ff			//	Input	: Pointer to the file object
){
	int n = 0;
	int iChar;

	while (n < len-1 ) {	// Read characters until buffer gets filled
		iChar = iFF_Getc(ff);
		if(iChar == EOF) {
			*buff = NULL;
			if(n==0) return EOF;
			return NO_ERROR;
		}else if(iChar=='\n'){
			*buff = NULL;
			return NO_ERROR;
		}else if(iChar=='\r'){
		}else{
			*buff = (BYTE)iChar;
			buff++;
			n++;
		}
	}
	return ERR_OVERFLOW;
}

//******************************************************************************
//* get chars and set a buff
//*
//* Description   :get chars and set a buff
//******************************************************************************
int iFF_Get_Chars(
BYTE* buff,			//	Output	: Pointer to the line characters buffer
int len,			//	Input	: Output line buffer size
FATFILE* ff			//	Input	: Pointer to the file object
){
	int n;
	int iChar;

	if (len == 0) return NO_ERROR;
	n = 0;
	while (n < len ) {	// Read characters until buffer gets filled
		iChar = iFF_Getc(ff);
		if(iChar == EOF) {
			*buff = NULL;
		}else{
			*buff = (BYTE)iChar;
		}
		buff++;
		n++;
	}
	return NO_ERROR;
}

//******************************************************************************
//* Skip bytes
//*
//* Description   :get chars and set a buff
//******************************************************************************
int iFF_Get_Skip(
DWORD len,			//	Input	: Output line buffer size
FATFILE* ff			//	Input	: Pointer to the file object
){
	DWORD n = 0;
	int iChar;

	if (len == 0) return NO_ERROR;
	while (n < len ) {	// Read characters until buffer gets filled
		iChar = iFF_Getc(ff);
		if(iChar == EOF) break;
		n++;
	}
	return NO_ERROR;
}

//******************************************************************************
//* get a DWORD from SD card
//*
//* Description   :get a DWORD data from SD card
//******************************************************************************
DWORD dwFF_Get_DWord(
FATFILE* ff			//	Input	: Pointer to the file object
){
	union {
	BYTE b4[4];
	DWORD dw;
	} u;

	iFF_Get_Chars(u.b4, sizeof(u.b4),ff);
	return u.dw;
//	iFF_Get_Chars(b4, sizeof(b4),ff);
//	return dwSwab_dw(b4);
}

//******************************************************************************
//* get a WORD from a file object
//*
//* Description   :get a WORD data from a file object
//******************************************************************************
WORD wFF_Get_Word(
FATFILE* ff			//	Input	: Pointer to the file object
){
	union {
	BYTE b2[2];
	WORD w;
	} u;

	iFF_Get_Chars(u.b2, sizeof(u.b2),ff);
	return u.w;
//	iFF_Get_Chars(b2, sizeof(b2),ff);
//	return wSwab_w(b2);
}

//******************************************************************************
//* Search a file and get a hit line
//*
//* Description   : get 1 line from file object
//******************************************************************************
int iFF_Search_File(
	BYTE* buff,		//	Output	: Pointer to the output line buffer
	int len,		//	Input   : Output line buffer size
 	BYTE* word,		//	Input   : Pointer to search word buffer
	int wlen,		//	Input   : Search word length
	FATFILE* ff		//	Input	: Pointer to the FATFILE object
){
	int res;
	while (1) {	// Read characters until buffer gets filled
		res = iFF_Get_Line(buff, len, ff);
		if(res == EOF) return NO_RESULT;
		if(strncmp(buff, word, wlen) == 0) return NO_ERROR;
		if(res) return res;
	}
	return ERR_OVERFLOW;
}


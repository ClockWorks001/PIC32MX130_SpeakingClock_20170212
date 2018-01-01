/*******************************************************************************
 *	UART_Menu
 ******************************************************************************/
#define UART_MENU_LIB

//#include <plib.h>            /* Include to use PIC32 peripheral libraries      */
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
//#include <stdio.h>
#include "xprintf.h"

#include "integer.h"
//#include "ff.h"		//FatFs定義体取得の為
//#include "vFF_lib.h"
//#include "vERROR_CODE.h"

//#include "vRTCC_lib.h"
#include "vUART_CMND_lib.h"
#include "vUART_MENU_lib.h"


char cMsgLine01;
const BYTE bMsg01[] = {
	"\r\n+Input your command!"			//入力表示コマンドモード
	"\r\n+(DEL)->clear the command"		//コマンドクリア
	"\r\n+(ESC)->exit or clear"			//
	"\r\n+Dyymmdd->Date setting"		//
	"\r\n+Thhmmss->Time setting"		//
	"\r\n+G->Time Output"				//時間の表示
	"\r\n+fi [<mount>] - Force initialized the logical drive"	//
	"\r\n+fs [<path>] - Show logical drive status"				//
	"\r\n+fl [<path>] - Directory listing"						//
	"\r\n+fo <mode> <file> - Open a file"						//
	"\r\n+fc - Close a file"									//
	"\r\n+fe <ofs> - Seek file pointer"							//
	"\r\n+fr <len> - read file"									//
	"\r\n+fd <len> - read and dump file from current fp"		//
	"\r\n+fg <path> - Change current directory"					//
	"\r\n+fq - Show current dir path"							//
	"\r\n+fp s Sho file pointer"								//
	"\r\n+v <filename> - fFF test : read out file every 1 char"								//
	"\r\n+w <filename> - fFF test : read out file every 1 line"								//
	"\0"			//
};


//#pragma code
//******************************************************************************
//* initialization
//******************************************************************************
void vUART_MENU_Init(void) {
	eModeStatusC2 = eModeC2_menu_init ;
	eUART_CMND_Init();
	//xputs("\r\n>");					//改行&入力指示プロンプト
	vPut_CRLF();
}

//******************************************************************************
//* Menu mode control
//******************************************************************************
void vUART_MENU_Control01(void) {

	switch (eModeStatusC2) {
	case  	eModeC2_menu_msg1 :
		xputs(bMsg01);							//初期メッセージ出力
		eModeStatusC2++;						//モードを次のステータスへ遷移
		break;

	case  	eModeC2_menu_init :
		vPut_CRLF();						//改行&入力指示プロンプト
		eModeStatusC2++;						//モードを次のステータスへ遷移
		break;

	case  	eModeC2_eUART_CMND_Getc :
		switch(eUART_CMND_Getc()) {					//コマンド文字列取得->cCmdBuf格納
		case eUART_CMND_Exec:
			eModeStatusC2 = eModeC2_vExecCmd;
			break;
		case eUART_CMND_Esc:
			eModeStatusC2 = eModeC2_menu_msg1;
			break;
		case eUART_CMND_Del:
			eModeStatusC2 = eModeC2_menu_init;
			break;
		default:								//コマンド入力まち
			break;
		}
		break;

	case  	eModeC2_vExecCmd:					//Command selection
		switch(cCmdBuf[0]) {
		case 'T':								//時計設定コマンド
		case 't':								//時計設定コマンド
		case 'D':								//日付設定コマンド
		case 'd':								//日付設定コマンド
			vUART_Command_DateTimeSet();
			eModeStatusC2 = eModeC2_menu_init;
			break;
		case 'g':
		case 'G':
			vUART_MessageOut_NowDateTime();
			eModeStatusC2 = eModeC2_menu_init;
			break;
		case '?':								//メニュー
			eModeStatusC2 = eModeC2_menu_msg1;
			break;
		case '\r':								//コマンド入力まち
			eModeStatusC2 = eModeC2_menu_init;
			break;

			//pFatFS moniter////////////////////////////////////////////////
		case 'f':
			vUART_Command_FatFs();
			eModeStatusC2 = eModeC2_menu_init;
			break;
			//pFatFS moniter////////////////////////////////////////////////

			//vFF test//////////////////////////////////////////////////////
		case 'v':
		case 'V':
			// file read test for 1 character
			// read a specific file. and output it to UART character by character.
			// Command format -> v filename.txt
			vUART_Command_vFF_test1(&cCmdBuf[2]);	// file read test for 1 character
			eModeStatusC2 = eModeC2_menu_init;	// wave close check
			break;
		case 'w':
		case 'W':
			// file read test for 1 line
			// read a specific file. and output it to UART line by line.
			// Command format -> w filename.txt
			vUART_Command_vFF_test2(&cCmdBuf[2]);	// file read test for 1 line
			eModeStatusC2 = eModeC2_menu_init;	// wave close check
			break;
		case 'x':
		case 'X':
			// read out wave file header
			// open a specific wave file. and readoutput a header property to UART.
			// Command format -> s 0000 filename.txt
			iUART_Command_vFF_test4(&cCmdBuf[2]);	// file read test for search a file
			eModeStatusC2 = eModeC2_menu_init;
			break;
		case 's':
		case 'S':
			// file read test for search a file
			// read a specific file. and search it by first 4 characters. and output a search result to UART.
			// Command format -> s 0000 filename.txt
			vUART_Command_vFF_test3(&cCmdBuf[2],&cCmdBuf[7]);	// file read test for search a file
			eModeStatusC2 = eModeC2_menu_init;
			break;
			//vFF test//////////////////////////////////////////////////////

		default:								// コマンドエラー
			eModeStatusC2 = eModeC2_Command_error;
			break;
		}
		break;

	case  	eModeC2_Command_error:
		// コマンド該当なし
		xputs("\r\n? Unrecognized command format");	// 入力エラー
		eModeStatusC2 = eModeC2_menu_init;
		break;

	default:
		eModeStatusC2 = eModeC2_menu_init;
	}

}


//******************************************************************************
//* Timer Setting
//* Timer Setting
//******************************************************************************
void vUART_Command_DateTimeSet(void) {
	DATETIME dt;
	unsigned int uiData;

	//コマンドフォーマットチェック *hhmmss
	vUART_Command_FormatCheck_6Digit();

	switch(cCmdBuf[0]) {
	case 'D':							//日付設定コマンド
	case 'd':							//日付設定コマンド
		//ascii->numeric
		uiData = (unsigned int)atoi(&cCmdBuf[1]);
		dt.ucYear = uiData / 10000;
		dt.ucMonth = (uiData % 10000) / 100;
		dt.ucDay  = (uiData % 100);
		iRTCC_Set_Date(&dt);				//時間をRTCCモジュールに設定する
		xputs("\n\rDate Setting Finish!");
		vUART_MessageOut_NowDateTime();					//設定時間の再表示
		break;

	case 'T':							//時計設定コマンド
	case 't':							//時計設定コマンド
		//ascii->numeric
		uiData = (unsigned int)atoi(&cCmdBuf[1]);
		dt.ucHour = uiData / 10000;
		dt.ucMin = (uiData % 10000) / 100;
		dt.ucSec  = (uiData % 100);
		iRTCC_Set_Time(&dt);				//時間をRTCCモジュールに設定する
		xputs("\n\rTime Setting Finish!");
		vUART_MessageOut_NowDateTime();					//設定時間の再表示
		break;

	default:
		xputs("\r\n? Format Error.");	//入力エラー
		break;
	}

}

//******************************************************************************
//* Show Now Time Message
//******************************************************************************
void vUART_MessageOut_NowDateTime(void) {
	//vRTCC_Get_DateTime();			// RTCCモジュールから日付時間を取得する。
	DATETIME dt;
	iRTCC_Get_DateTime(&dt);			// RTCCモジュールから日付時間を取得する。
	xprintf( "\n\rTime is %02d:%02d:%02d", dt.ucHour, dt.ucMin, dt.ucSec);
	xprintf( "\n\rDate is %02d/%02d/%02d", dt.ucYear, dt.ucMonth, dt.ucDay);
}

//******************************************************************************
//* Comand format check 6digit
//******************************************************************************
void vUART_Command_FormatCheck_6Digit(void) {
	char i;
	for( i=1 ; i<7 ; i++) {
		if(!isdigit(cCmdBuf[i])) {
			cCmdBuf[0] = '?';
		}
	}
}


//******************************************************************************
//* scan files
//******************************************************************************
static FRESULT scan_files (
	char* path		/* Pointer to the path name working buffer */
)
{
	//DIR dir;
	FRESULT res;
	int i;
	char *fn;
	DWORD AccSize;			/* Work register for fs command */
	WORD AccFiles, AccDirs;


	if ((res = f_opendir(&dir, path)) == FR_OK) {
		i = strlen(path);
		while (((res = f_readdir(&dir, &Finfo)) == FR_OK) && Finfo.fname[0]) {
			if (_FS_RPATH && Finfo.fname[0] == '.') continue;
#if _USE_LFN
			fn = *Finfo.lfname ? Finfo.lfname : Finfo.fname;
#else
			fn = Finfo.fname;
#endif
			if (Finfo.fattrib & AM_DIR) {
				AccDirs++;
				path[i] = '/'; strcpy(&path[i+1], fn);
				res = scan_files(path);
				path[i] = 0;
				if (res != FR_OK) break;
			} else {
#if 0
				xprintf("%s/%s\n", path, fn);
#endif
				AccFiles++;
				AccSize += Finfo.fsize;
			}
		}
	}

	return res;
}

//******************************************************************************
//* FatFs command test menu
//******************************************************************************
void vUART_Command_FatFs(void) {

FRESULT res;
long p1, p2;
UINT s1, s2, cnt;
DWORD ofs = 0; //, sect = 0;
char Line[256];			/* Console input buffer */

#if _USE_LFN
    Finfo.lfname = Lfname;
    Finfo.lfsize = sizeof Lfname;
#endif
	
	xputs("\r\n");
	ucPtr1 = &cCmdBuf[2];
	switch(cCmdBuf[1]) {
			case 'i' :	/* fi [<mount>] - Force initialized the logical drive */
					          /*^^^^^ 0:Do not mount (delayed mount), 1:Mount immediately */
				if (!xatoi(&ucPtr1, &p2)) p2 = 0;
				vPut_Result(f_mount(&FatFs, "", (BYTE)p2));
				break;

			case 'l' :	/* fl [<path>] - Directory listing */
				while (*ucPtr1 == ' ') ucPtr1++;
				res = f_opendir(&dir, ucPtr1);
				if (res) { vPut_Result(res); break; }
				p1 = s1 = s2 = 0;
				for(;;) {
					res = f_readdir(&dir, &Finfo);
					if ((res != FR_OK) || !Finfo.fname[0]) break;
					if (Finfo.fattrib & AM_DIR) {
						s2++;
					} else {
						s1++; p1 += Finfo.fsize;
					}
					xprintf("%c%c%c%c%c %u/%02u/%02u %02u:%02u %9lu  %-12s  %s\n",
							(Finfo.fattrib & AM_DIR) ? 'D' : '-',
							(Finfo.fattrib & AM_RDO) ? 'R' : '-',
							(Finfo.fattrib & AM_HID) ? 'H' : '-',
							(Finfo.fattrib & AM_SYS) ? 'S' : '-',
							(Finfo.fattrib & AM_ARC) ? 'A' : '-',
							(Finfo.fdate >> 9) + 1980, (Finfo.fdate >> 5) & 15, Finfo.fdate & 31,
							(Finfo.ftime >> 11), (Finfo.ftime >> 5) & 63, Finfo.fsize, Finfo.fname,
#if _USE_LFN
							Lfname);
#else
							"");
#endif
				}
				xprintf("%4u File(s),%10lu bytes total\n%4u Dir(s)", s1, p1, s2);

				break;

			case 'o' :	/* fo <mode> <file> - Open a file */
				             //^^^^ FA_OPEN_EXISTING	0x00 <==not select
				             //^^^^ FA_READ				0x01 <==select
				             //^^^^ FA_WRITE			0x02
				             //^^^^ FA_CREATE_NEW		0x04
				             //^^^^ FA_CREATE_ALWAYS	0x08
				             //^^^^ FA_OPEN_ALWAYS		0x10
				             //^^^^ FA__WRITTEN			0x20
				             //^^^^ FA__DIRTY			0x40
				if (!xatoi(&ucPtr1, &p1)){ xputs("option error"); break; }
				while (*ucPtr1 == ' ') ucPtr1++;
//				vPut_Result(f_open(&FF2.fObject, ucPtr1, (BYTE)p1));
				vPut_Result(f_open(&FF2.fObject, ucPtr1, 1));
				break;

			case 'c' :	/* fc - Close a file */
				vPut_Result(f_close(&FF2.fObject));
				break;

			case 'e' :	/* fe <ofs> - Seek file pointer */
				if (!xatoi(&ucPtr1, &p1)){ xputs("option error"); break; }
				res = f_lseek(&FF2.fObject, p1);
				vPut_Result(res);
				if (res == FR_OK)
					xprintf("fucPtr1 = %lu(0x%lX)\n", f_tell(&FF2.fObject), f_tell(&FF2.fObject));
				break;

			case 'r' :	/* fr <len> - read file */
				if (!xatoi(&ucPtr1, &p1)){ xputs("option error"); break; }
				p2 = 0;
				while (p1) {
					if ((DWORD)p1 >= SD_BUFFER_SIZE) {
						cnt = SD_BUFFER_SIZE; p1 -= SD_BUFFER_SIZE;
					} else {
						cnt = p1; p1 = 0;
					}
					res = f_read(&FF2.fObject, &FF2.bBuff[0][0], cnt, &s2);
					if (res != FR_OK) { vPut_Result(res); break; }
					p2 += s2;
					if (cnt != s2) break;
				}
				xprintf("%lu bytes read\n", p2);
				break;

			case 'd' :	/* fd <len> - read and dump file from current fp */
				if (!xatoi(&ucPtr1, &p1)){ xputs("option error1"); break; }
				ofs = f_tell(&FF2.fObject);
				while (p1) {
					if ((UINT)p1 >= 16) { cnt = 16; p1 -= 16; }
					else 				{ cnt = p1; p1 = 0; }
					res = f_read(&FF2.fObject, &FF2.bBuff[0][0], cnt, &cnt);
					if (res != FR_OK) { vPut_Result(res); break; }
					if (!cnt) break;
					xput_dump(&FF2.bBuff[0][0], ofs, cnt, DW_CHAR);
					ofs += 16;
				}
				break;


#if _FS_RPATH >= 1
			case 'g' :	/* fg <path> - Change current directory */
				while (*ucPtr1 == ' ') ucPtr1++;
				vPut_Result(f_chdir(ucPtr1));
				break;
#if _FS_RPATH >= 2
			case 'q' :	/* fq - Show current dir path */
				res = f_getcwd(Line, sizeof Line);
				if (res)
					vPut_Result(res);
				else
					xprintf("%s\n", Line);
				break;
#endif
#endif
			case 'p' :	/* fp - Show file pointer */
				xprintf("File1.flag = %lu\nFile1.err = %lu\n"
						"File1.fptr = %lu\nFile1.fsize = %lu\n"
						"File2.flag = %lu\nFile2.err = %lu\n"
						"File2.fptr = %lu\nFile2.fsize = %lu\n",
						FF1.fObject.flag , FF1.fObject.err,
						FF1.fObject.fptr , FF1.fObject.fsize,
						FF2.fObject.flag , FF2.fObject.err,
						FF2.fObject.fptr , FF2.fObject.fsize
				);
				break;

			default:
				xputs("? FatFs Command Error.");	// 入力エラー
				break;

	}

}

//******************************************************************************
//* 改行&入力指示プロンプト
//******************************************************************************
void vPut_CRLF(void) {
	xputs("\r\n>");					//改行&入力指示プロンプト
}


//******************************************************************************
//* pFatFs Moniter
//******************************************************************************
void vPut_Result (int rc) {		//ステータスOUTPUT
	FRESULT i;
	const char *str1 = {
		"OK\0"
		"DISK_ERR\0"
		"INT_ERR\0"
		"NOT_READY\0"
		"NO_FILE\0"
		"NO_PATH\0"
		"INVALID_NAME\0"
		"DENIED\0"
		"EXIST\0"
		"INVALID_OBJECT\0"
		"WRITE_PROTECTED\0"
		"INVALID_DRIVE\0"
		"NOT_ENABLED\0"
		"NO_FILE_SYSTEM\0"
		"MKFS_ABORTED\0"
		"TIMEOUT\0"
		"LOCKED\0"
		"NOT_ENOUGH_CORE\0"
		"TOO_MANY_OPEN_FILES\0"
	};
	const char *str2 = {
		"OK\0"
		"END_OF_FILE\0"
		"OVERFLOW\0"
		"WAITING\0"
		"NO_RESULT\0"
	};
	if(rc >= 0){
		for (i = 0; i != rc && *str1; i++) {
			while (*str1++) ;
		}
		xprintf("\r\nrc=%u FR_%s\r\n>", (UINT)rc, str1);
	}else{
		for (i = 0; i != rc && *str2; i--) {
			while (*str2++) ;
		}
		xprintf("\r\nrc=%d %s\r\n>", rc, str2);
	}

}


//******************************************************************************
//* vFF test for 1 character
//* file read test for 1 character
//* read a specific file. and output it to UART character by character.
//******************************************************************************
void vUART_Command_vFF_test1(BYTE* bFileName) {

	int iCher;

	vPut_CRLF();
	vPut_Result(iFF_FirstRead(&FF1, bFileName));

	while(1){
		iCher = iFF_Getc(&FF1);
		if (iCher == EOF) break;
		xputc((BYTE)iCher);
	}

	vPut_Result(f_close(&FF1.fObject));
	xputs("\r\nvFF_test1_end");
	vPut_CRLF();

}

//******************************************************************************
//* fFF test for 1 line
//* file read test for 1 line
//* read a specific file. and output it to UART line by line.
//******************************************************************************
void vUART_Command_vFF_test2(BYTE* bFileName) {

	BYTE bLine[256];

	vPut_CRLF();
	vPut_Result(iFF_FirstRead(&FF1, bFileName));

	while (iFF_Get_Line(bLine, sizeof(bLine), &FF1)!=EOF) {
		xputs(bLine);
		vPut_CRLF();
	}
	vPut_Result(f_close(&FF1.fObject));
	xputs("vFF_test2_end");
	vPut_CRLF();
}

//******************************************************************************
//* fFF test for search a file
//* file read test for search a file
//* read a specific file. and search it by first 4 characters. and output a search result to UART.
//******************************************************************************
void vUART_Command_vFF_test3(BYTE* bSearchWord, BYTE* bFileName) {

	BYTE bLine[256];

	vPut_CRLF();
	vPut_Result(iFF_FirstRead(&FF1, bFileName));

	vPut_Result(iFF_Search_File( bLine, sizeof(bLine), bSearchWord, 4, &FF1));
	vPut_Result(f_close(&FF1.fObject));
	xputs(bLine);
	vPut_CRLF();
	xputs("vFF_test3_end");
	vPut_CRLF();
}

	//int	iChannel, iSamplingRate, iBlockSize, iBitSampling;	//WAV Format Parameters1
	WAVEHEADER wh;

	BYTE bLine[100];
	DWORD dwChunk;
	DWORD dwSize;

//******************************************************************************
//* read out wave file header
//* open a specific wave file. and readoutput a header property to UART.
//******************************************************************************
int iUART_Command_vFF_test4(BYTE* bFileName) {


	vPut_CRLF();
	vPut_Result(iFF_FirstRead(&FF1, bFileName));

	vPut_Result(iFF_Get_Chars(bLine, 8, &FF1));
	dwChunk = dwFF_Get_DWord(&FF1);

	if (dwChunk != FCC('W','A','V','E')) return NO_WAVE_FILE;

	for (;;) {
		dwChunk = dwFF_Get_DWord(&FF1);		// Get Chunk ID
		dwSize = dwFF_Get_DWord(&FF1);		// Get Chunk size

		switch (dwChunk) {					// Switch by chunk ID
		case FCC('f','m','t',' ') :					// 'fmt ' chunk
			if (dwSize > 100 || dwSize < 16) return NO_WAVE_FILE;	// Check chunk size
			wh.iFormatID = (int)wFF_Get_Word(&FF1);
			if (wh.iFormatID != 1) return NO_WAVE_FILE;		// Check coding type (LPCM)
			wh.iChannel = (int)wFF_Get_Word(&FF1);
	xprintf("\r\nCH=%u", wh.iChannel);
			if (wh.iChannel != 1 && wh.iChannel != 2) return NO_WAVE_FILE; // Check channels (1 or 2)
			wh.iSamplingRate = (int)dwFF_Get_DWord(&FF1);
	xprintf("\r\nFQ=%luHz", wh.iSamplingRate);
			if (wh.iSamplingRate < 8000 || wh.iSamplingRate > 48000) return NO_WAVE_FILE; // Check sampling freqency (8k-48k)
			wh.iBlockSize = (int)dwFF_Get_DWord(&FF1);	// skip a byte/sec info
			wh.iBlockSize = (int)wFF_Get_Word(&FF1);
	xprintf("\r\nBLOCK=%uBytes", wh.iBlockSize);
			if (wh.iBlockSize < 0 || wh.iBlockSize > 4) return NO_WAVE_FILE; // Check Byte/sampling
			wh.iBitSampling = (int)wFF_Get_Word(&FF1);
	xprintf("\r\nBIT=%uBits", wh.iBitSampling);
			if (wh.iBitSampling != 8 && wh.iBitSampling != 16) return NO_WAVE_FILE; // Check Bit/sampleing

			dwSize = dwSize -16;
			if(dwSize) iFF_Get_Skip(dwSize, &FF1); //skip unnecessary data
			break;

		case FCC('d','a','t','a') :		/* 'data' chunk */
			wh.dwDataSize = dwSize;
	xprintf("\r\nDataSize=%uBytes", wh.dwDataSize);
			goto RESULT_WAVE_HEADER;
			break;

		case FCC('D','I','S','P') :		/* 'DISP' chunk */
		case FCC('L','I','S','T') :		/* 'LIST' chunk */
		case FCC('f','a','c','t') :		/* 'fact' chunk */
			iFF_Get_Skip(dwSize, &FF1); ;			/* Skip this chunk */
			break;

		default :						/* Unknown chunk */
			goto RESULT_WAVE_HEADER;
			return 0;
		}
	}

RESULT_WAVE_HEADER:

	//int	iChannel, iSamplingRate, iBlockSize, iBitSampling;	//WAV Format Parameters1
	xputs("\r\n>");

	return 0;
}


/*******************************************************************************
 *	  FatFs interface with FIFO
 *    This is a higher-level layer of FatFs file system. 
 *	  author  :hiroshi murakami
 *	  created :20170121
 *	  modified:-
 ******************************************************************************/
#ifndef _vFFF_H
#define _vFFF_H

#include "integer.h"
#include "ff.h"		 //for getting FatFs typedef
#include "vFIFO.h"

#define SD_BUFFER_BLOCK 4			//2
#define SD_BUFFER_BLOCK_SIZE _MAX_SS		//512
#define SD_FILES		2			//number of file object
#define SD_FILE_0		0			//file object name
//#define SD_FILE_1		1			//file object name
#define WAVE_FILE_1 	1			//file object name
#define vFFF_NULL		0x00		//\0
#define vFFF_SP 		0x20		//
#define vFFF_LF 		0x0a		//\n
#define vFFF_CR 		0x0d		//\r
#define vFFF_TAB		0x09		//\t
#define vFFF_TILDE  	0x7e
#define vFFF_UNMATCH	0

/*****************************
 * VARIABLES
 *****************************/
typedef union {
	BYTE	bBuff[SD_BUFFER_BLOCK * SD_BUFFER_BLOCK_SIZE];		// FIFO buffer
	BYTE	bBlockBuff[SD_BUFFER_BLOCK][SD_BUFFER_BLOCK_SIZE];	// FIFO buffer Byte line
} FFF_BUFF;

#if defined _vFFF_C
FATFS FatFs;							// File system object */
FIL	File[SD_FILES];						// File objects */
DIR	Dir;								//Directory object
FILINFO Finfo;							//File information
FFF_BUFF fffBuff[SD_FILES];
FIFO_STRUCT ffFifo[SD_FILES];
#else
extern FATFS FatFs;						// File system object
extern FIL	File[SD_FILES];				// File objects
extern DIR	Dir;						//Directory object
extern FILINFO Finfo;					//File information
extern FFF_BUFF fffBuff[SD_FILES];
extern FIFO_STRUCT ffFifo[SD_FILES];
#endif

/*****************************
 * PROTOTYPES
 *****************************/
void vFFF_init(void);
void vFFF_FIFO_init(int fileObjNo);
FRESULT rFFF_open(BYTE* fileName, int fileObjNo);
FRESULT rFFF_close(int fileObjNo);
FRESULT rFFF_BlockRead(int fileObjNo);
BYTE bFFF_getc(int fileObjNo);
BYTE* pbFFF_gets(BYTE* out_buffer, int length, int fileObjNo );
int iFFF_eof(int fileObjNo);						// return 1:eof , 0 not end
int iFFF_space(int fileObjNo);						// return buffer space bytes
BYTE bFFF_text_getc(int fileObjNo);
BYTE* pbFFF_text_getline(BYTE* out_buffer, int length,int fileObjNo);	// return out_buffer address
int iFFF_match_words(BYTE* wordA, BYTE* wordB);		//matching check for first words of each lines.
int iFFF_get_No_of_files(BYTE* pbPath, BYTE* pbSearchExt);
BYTE* pbFFF_get_fileName(	BYTE* pbPath, BYTE* pbSearchExt, int fileNumber);
BYTE* pbFFF_get_nextTime(BYTE* currentTime, BYTE*  chimeFileName);

#define rFFF_mount(Fifop)		f_mount((Fifop),"",0)
#define iFFF_remains(fileObjNo)	!(iFFF_eof(fileObjNo))

#endif

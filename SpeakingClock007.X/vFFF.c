/*******************************************************************************
 *	  FatFs interface with FIFO
 *    This is a higher-level layer of FatFs file system. 
 *	  author  :hiroshi murakami
 *	  created :20170121
 *	  modified:-
 ******************************************************************************/
#define _vFFF_C

#include "mcc_generated_files/mcc.h"
#include <string.h>
#include "vFFF.h"

//******************************************************************************
// Initialize FFF
//******************************************************************************
void vFFF_init(void)
{
	int i;

	// Clear FIFO buffers & File object
	for(i = 0 ; i < SD_FILES ; i++) {
		vFFF_FIFO_init(i);
		f_close(&File[i]);
	}
}

//******************************************************************************
// Initialize FFF FIFO
//******************************************************************************
void vFFF_FIFO_init(
	int fileObjNo
)
{
	// Clear FIFOs
	vFIFO_init (&ffFifo[fileObjNo], sizeof(fffBuff[fileObjNo].bBuff), fffBuff[fileObjNo].bBuff);
}

//******************************************************************************
// FFF open
//******************************************************************************
FRESULT rFFF_open (
	BYTE* fileName,
	int fileObjNo
)
{
	FRESULT res;
	vFFF_FIFO_init(fileObjNo);
	res = f_open(&File[fileObjNo], fileName,  FA_READ);
	return res;
}

//******************************************************************************
// FFF close
//******************************************************************************
FRESULT rFFF_close (
	int fileObjNo
)
{
	return f_close(&File[fileObjNo]);
}

//******************************************************************************
// FFF block read
//******************************************************************************
FRESULT rFFF_BlockRead(
	int fileObjNo
)
{
	FRESULT res;
	UINT read_out_byte,block;
	FSIZE_t buffSector, fsPtr;

	res = FR_OK;
	if(File[fileObjNo].obj.fs) {				//File[fileObjNo].obj.fsポンタ=nullはファイルオブジェクトなし
		if(f_eof(&File[fileObjNo])) {
			f_close(&File[fileObjNo]);
		} else {
			while(SD_BUFFER_BLOCK_SIZE <= iFIFO_space(&ffFifo[fileObjNo])) {
				if(f_eof(&File[fileObjNo])) break;
				if( eFIFO_busy <= ffFifo[fileObjNo].status) break;
				
				ffFifo[fileObjNo].status = eFIFO_busy;
				fsPtr =  f_tell(&File[fileObjNo]);
				buffSector = fsPtr / SD_BUFFER_BLOCK_SIZE;
				block = buffSector % SD_BUFFER_BLOCK;
				res = f_read(&File[fileObjNo], &fffBuff[fileObjNo].bBlockBuff[block][0], SD_BUFFER_BLOCK_SIZE, &read_out_byte);
				if (res != FR_OK) {
					put_rc(res);
					File[fileObjNo].obj.fs = 0;
					put_rc(rFFF_mount(&FatFs));	  // file system mount
					vFFF_init ();
					return res;
				}
				ffFifo[fileObjNo].ct = ffFifo[fileObjNo].ct + read_out_byte;
				ffFifo[fileObjNo].status = eFIFO_bufferStored;
			}

		}
	} else {
		return FR_NO_FILE;
	}
	return res;
}

//******************************************************************************
// return end of file = 1
//******************************************************************************
int iFFF_eof(
	int fileObjNo
)
{
	if(ffFifo[fileObjNo].ct == 0) {
		if(f_eof(&File[fileObjNo])) {
			return 1;
		}
	}
	return 0;
}

//******************************************************************************
// FFF getc
//******************************************************************************
BYTE bFFF_getc(
	int fileObjNo
)
{
	if(0 == iFIFO_remains(&ffFifo[fileObjNo])) return vFFF_NULL;
	return bFIFO_getc(&ffFifo[fileObjNo]);
}

//******************************************************************************
// return buffer space bytes
//******************************************************************************
int iFFF_space(
	int fileObjNo
)
{
	return (ffFifo[fileObjNo].buffer_size - ffFifo[fileObjNo].ct);
}

//******************************************************************************
// pbFFF gets
// put in out_buffer from vFFF
// return buffer address
//******************************************************************************
BYTE* pbFFF_gets(
	BYTE* out_buffer,
	int length,
	int fileObjNo
)
{
	int i;
	BYTE* buff;

	if(iFFF_eof(fileObjNo)) return NULL;
	buff = out_buffer;
	for(i = 0; i < length ; i++) {
		*buff = bFFF_getc(fileObjNo);
		buff++;
	}
	return out_buffer;
}


//******************************************************************************
// FFF text getc (it return only text character code
//	  vFFF_SP(space)---vFFF_TILDE(~), vFFF_CR(CR) (*vFFF_LF(LF) is not return.)
//******************************************************************************
BYTE bFFF_text_getc(
	int fileObjNo
)
{
	BYTE a;

	while(!(iFFF_eof(fileObjNo))) {
//		a = bFIFO_getc(&ffFifo[fileObjNo]);
		a = bFFF_getc(fileObjNo);
		if(vFFF_SP <= a && a <= vFFF_TILDE) return a;
		if(vFFF_LF == a) return a;							// vFFF_LF = \n = LF
		if(vFFF_CR == a) return vFFF_LF;					// vFFF_CR = CR => LF
		if(vFFF_TAB == a) return vFFF_SP;					// vFFF_TAB = TAB => SPACE
		if(vFFF_NULL == a) return a;						// null
	}
	return vFFF_NULL;
}

//******************************************************************************
// pbFFF get a text line
//
// skip Space character codes when there are Space characters at head of line.
// with in 0x00 instead of LF at end of line.
// put in out_buffer from vFFF
// returnout_ buffer address
// Precondition : open a file use fileObjNo
// Return		: out_buffer address. 
//******************************************************************************
BYTE* pbFFF_text_getline(
	BYTE* out_buffer,
	int length,					  // it is need one character byte for \0 at end of line.
	int fileObjNo
)
{
	int i;
	BYTE* buff;

	if(iFFF_eof(fileObjNo)) return NULL;

	buff = out_buffer;
	*buff = bFFF_text_getc(fileObjNo);
	while(*buff == vFFF_SP || *buff == vFFF_LF) {			// head of space or LF are skip
		*buff = bFFF_text_getc(fileObjNo);
	}
	buff++;
	length--;
	for(i = 1; i < length ; i++) {
		*buff = bFFF_text_getc(fileObjNo);
		if(*buff == vFFF_LF || *buff == vFFF_NULL) break;	 // stop when LF or NULL exist.
		buff++;
	}
	*buff = '\0';
	return out_buffer;
}

//******************************************************************************
// compare first words of each lines untill exist space.
//
// return	:unmach = 0, match = bytes of word
//******************************************************************************
int iFFF_match_words(
	BYTE* wordA,
	BYTE* wordB
)
{
	int i;
	i = 0;
	while(*wordA == *wordB) { // head of space or LF are skip
		if(i >= 128) return vFFF_UNMATCH;
		if(*wordA == vFFF_NULL	|| *wordB == vFFF_NULL) break;
		if(*wordA == vFFF_SP	|| *wordB == vFFF_SP) break;
		if(*wordA == vFFF_LF	|| *wordB == vFFF_LF) break;
		wordA++;
		wordB++;
		i++;
	}
	if(!(*wordA == vFFF_NULL) && !(*wordA == vFFF_SP) && !(*wordA == vFFF_LF)) return vFFF_UNMATCH;
	if(!(*wordB == vFFF_NULL) && !(*wordB == vFFF_SP) && !(*wordB == vFFF_LF)) return vFFF_UNMATCH;
	return i;	// when match, return bytes of word.
}

//******************************************************************************
// get number of files with specific extention.
//
// return	:number of files which are mached condition.
//******************************************************************************
int iFFF_get_No_of_files(
	BYTE* pbPath,
	BYTE* pbSearchExt
)
{
	FRESULT res;
	int i = 0;
	BYTE* ptr;
	BYTE ext[5];
	BYTE fnm[128];

	res = f_opendir(&Dir, pbPath);			// Dir : Directory object
	strcpy(ext, pbSearchExt);
	for(ptr = ext; *ptr; ptr++ ) *ptr = toupper(*ptr);			 //change search word to Upper

	for(;;) {
		res = f_readdir(&Dir, &Finfo);
		if ((res != FR_OK) || !Finfo.fname[0]) break;
		if (Finfo.fattrib & AM_ARC) {
			strcpy(fnm, Finfo.fname);
			for(ptr = fnm; *ptr; ptr++ ) *ptr = toupper(*ptr);	//same as befor line, to Upper
			if(strstr(fnm, ext)) {
				i++;
				//xprintf("File name is %d %s\n", i, Finfo.fname);
			}
		}
	}
	return i;	// when match return bytes of word.
}


//******************************************************************************
// input file number, return a file name
//
// return	:file name (buffer address) or null
//******************************************************************************
BYTE* pbFFF_get_fileName(
	BYTE* pbPath,
	BYTE* pbSearchExt,
	int fileNumber
)
{
	FRESULT res;
	int i = 0;
	BYTE* ptr;
	BYTE ext[4];
	BYTE fnm[128];

	res = f_opendir(&Dir, pbPath);
	strcpy(ext, pbSearchExt);
	for(ptr = ext; *ptr; ptr++ ) *ptr = toupper(*ptr);			 //change search word to Upper

	for(;;) {
		res = f_readdir(&Dir, &Finfo);
		if ((res != FR_OK) || !Finfo.fname[0]) break;
		if (Finfo.fattrib & AM_ARC) {
			strcpy(fnm, Finfo.fname);
			for(ptr = fnm; *ptr; ptr++ ) *ptr = toupper(*ptr);	//same as befor line, to Upper
			if(strstr(fnm, ext)) {
				i++;
				if(fileNumber == i) {
					//xprintf("File name is %d %s\n", i, Finfo.fname);
					return Finfo.fname;
				}
			}
		}
	}
	return 0x00;	// when unmatch the case, return null.
}


//******************************************************************************
// This function is specific for RTCC chime speaking.
//
// Precondition : -
// Return		: out_buffer address. 
//******************************************************************************
BYTE* pbFFF_get_nextTime(
	BYTE* currentTime,
	BYTE* chimeFileName
)
{
	BYTE textbuff[256];
	BYTE minTime[5];
	BYTE nextTime[5];
	
	strcpy(minTime, "9999");
	strcpy(nextTime, "9999");
	
	rFFF_open(chimeFileName, SD_FILE_0);
	while(iFFF_remains(SD_FILE_0)) {
		rFFF_BlockRead(SD_FILE_0);
		pbFFF_text_getline(&textbuff[0], sizeof(textbuff), SD_FILE_0);
		textbuff[4] = '\0';
		if(textbuff[0] != '\0'){
			if(strcmp(textbuff, minTime) < 0){	
				strcpy(minTime, textbuff);		//get mimum Time
			}
			if(strcmp(textbuff, currentTime) > 0){	
				if(strcmp(textbuff, nextTime) < 0){	//get nextTime
					strcpy(nextTime, textbuff);
				}
			}
		}
	}
	
	if(strcmp(nextTime, "9999" ) == 0){		//if didn't find "nextTime", then set minimumTime.
		strcpy(currentTime, minTime);
	}else{
		strcpy(currentTime, nextTime);
	}
	return currentTime;
}
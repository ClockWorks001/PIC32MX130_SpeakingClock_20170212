/*******************************************************************************
 *	  WAVE file process
 *	  author  :hiroshi murakami
 *	  created :20170129
 *	  modified:-
 *  Timer2 : for PWM source clock
 *  Timer3 : for sampling rate of WAVE play
 ******************************************************************************/
#define _vWAVE_C

#include "mcc_generated_files/mcc.h"
#include <stdlib.h>
#include <string.h>
#include "vTMR1.h"
#include "vUART_MENU.h"
#include "vWAVE.h"
#include "vPOWER.h"

//******************************************************************************
//	change 2bytes stream to word (unsigned short)
//******************************************************************************
WORD wSwab_w(
	unsigned char *A
)
{
	union SWAB_W_DATA {
		unsigned short usData;
		unsigned char ucData[2];
	} ;
	static union SWAB_W_DATA B;
	B.ucData[0] = *(A);
	B.ucData[1] = *(++A);
	return B.usData;
}

//******************************************************************************
//	change 4bytes stream to double word (unsigned long)
//******************************************************************************
DWORD dwSwab_dw(
	unsigned char *A
)
{
	union SWAB_DW_DATA {
		unsigned long ulData;
		unsigned char ucData[4];
	} ;
	union SWAB_DW_DATA B;
	B.ucData[0] = *(A);
	B.ucData[1] = *(++A);
	B.ucData[2] = *(++A);
	B.ucData[3] = *(++A);
	return B.ulData;
}

//******************************************************************************
//	initialization
//	TMR3 is making Wave sampling rate
//******************************************************************************
void vTMR3_init(
	unsigned int T3_Hz
)
{
	TMR3_Period16BitSet ( _XTAL_FREQ / T3_Hz );
	TMR3_Start();
}

//******************************************************************************
//	Interrupt TMR3 (call from MCC TMR3 interrupt call back routine )
//
//	Description	: PWM output by timing of sampling rate
//	Input		: sampling rate
//	Output		:
//******************************************************************************
void Interrupt_TMR3(void)
{
	unsigned short pwm_value;

//	IEC0bits.T3IE = 0;						//for wave Interrupt_TMR3 => bFIFO_getc

	if (iFFF_eof(WAVE_FILE_1) || wave.ulDataByte == 0) {
		OC1_PWMPulseWidthSet(PWM_CENTER_VALUE);	// PWM_CENTER_VALUE 128
		TMR3_Stop();
		wave.ePlayStatus = eWave_end;
		return;
	}

	pwm_value = usGetPwmValue();
	if (wave.iChannel == 2) {
		pwm_value = (pwm_value +  usGetPwmValue()) / 2;
	}

	OC1_PWMPulseWidthSet(pwm_value);		// PWM_CENTER_VALUE 128
	
//	IEC0bits.T3IE = 1;						//for wave Interrupt_TMR3 => bFIFO_getc
	
}

//******************************************************************************
//	Get PWM Value from vFFF file
//
//	Description	: 16bits is signed data ==> unsigned 8bits
//				: 8bits is unsigned data ==> unsigned 8bits
//	Input		: vFFF (file Bytes stream)
//	Output		: PwmValue(8bit 0-255)
//******************************************************************************
unsigned short usGetPwmValue(void)
{
	union SWAB_W_DATA {
		short sData;
		BYTE  bData[2];
	} ;
	union SWAB_W_DATA a;

	if (wave.iBits == 16) {
		//set PCM data to PWM (16bit 2ch)
		wave.ulDataByte = wave.ulDataByte -2;
		a.bData[0] = bFFF_getc(WAVE_FILE_1);
		a.bData[1] = bFFF_getc(WAVE_FILE_1);
		return (unsigned short)(a.sData / 0x100 + PWM_CENTER_VALUE);	// PWM_CENTER_VALUE 128
	} else {
		wave.ulDataByte--;
		return (unsigned short)bFFF_getc(WAVE_FILE_1) ;
	}
}

//******************************************************************************
//	WAVE object initialization
//******************************************************************************
void vWAVE_init(void)
{
	xputs("vWAVE_init\n");

	OC1_PWMPulseWidthSet(PWM_CENTER_VALUE);	// PWM_CENTER_VALUE 128
	TMR2_Start();
	OC1_Start();
	
	xputs("rFFF_close\n");
	put_rc(rFFF_close(WAVE_FILE_1));

	xputs("vFFF_FIFO_init\n");
	vFFF_FIFO_init(WAVE_FILE_1);

	wave.ePlayStatus = eWave_idle;
}

//******************************************************************************
//	close PWM
//******************************************************************************
void vWAVE_stop(void)
{
	xputs("vWAVE_stop!\n");
	waveTask11.status = eWaveT11_idle;
	waveWords12.status = eWaveW12_idle;
	waveMusic13.status = eWaveM13_idle;

//	OC1_PWMPulseWidthSet(PWM_CENTER_VALUE);	// PWM_CENTER_VALUE 128
	TMR3_Stop();
	vWAVE_init();
	wave.ePlayStatus = eWave_idle;
}

//******************************************************************************
//	close PWM
//******************************************************************************
void vWAVE_close(void)
{
	xputs("vWAVE_close\n");

	vWAVE_stop();
	
	// close OC1,Timer2,3
	OC1_Stop();
	TMR2_Stop();
	TMR3_Stop();
	wave.ePlayStatus = eWave_close;
}

//******************************************************************************
//	open Wave file & get a Property
//
//	Description	:
//	 1.open the Wave file.
//	 2.check the file header.
//	 3.get the Wave property.
//	Precondition:
//	 The SD card must have been initialized & mount by "f_mount" before this function.
//	Input		: SD card
//	Parameters	: file name.
//	Returns		: a error cod of WAVE logic RESULT.
//******************************************************************************
WAVE_RESULT eWAVE_file_open(
	BYTE* waveFileName
)
{
	FRESULT res;
	BYTE buff[10];
	DWORD i;

	//----------------------------------------------------------------------
	wave.ePlayStatus = eWave_close;
	TMR3_Stop();
	vWAVE_init();

	xprintf("rFFF_open = %s \n", waveFileName);
	res = rFFF_open(waveFileName, WAVE_FILE_1);
	if (res != FR_OK) {
		put_rc(res);
		return eWAVE_FILE_OPEN_ERROR;
	}

	xprintf( "rFFF_BlockRead\n");
	res = rFFF_BlockRead(WAVE_FILE_1);						// first read a file
	if (res != FR_OK) {
		put_rc(res);
		return eWAVE_FILE_READ_ERROR;
	}

	//----------------------------------------------------------------------
	if(strncmp(pbFFF_gets(&buff[0], 4, WAVE_FILE_1),"RIFF",4) != 0 ) {
		xputs("NO RIFF\n");
		return eWAVE_NOT_WAVE_FILE;
	}
	pbFFF_gets(&buff[0], 4, WAVE_FILE_1);	//skip 4 bytes, RIFF data size
	if(strncmp(pbFFF_gets(&buff[0], 4, WAVE_FILE_1),"WAVE",4) != 0 ) {
		xputs("NO WAVE\n");
		return eWAVE_NOT_WAVE_FILE;
	}
	pbFFF_gets(&buff[0], 4, WAVE_FILE_1);	//skip 4 bytes, "fmt "

	wave.iFmtByte	= dwSwab_dw(pbFFF_gets(&buff[0], 4, WAVE_FILE_1));		//fmt byte のデータサイズ
	wave.iFmtID	 = wSwab_w(pbFFF_gets(&buff[0], 2, WAVE_FILE_1));			//fmt ID
	wave.iChannel	= wSwab_w(pbFFF_gets(&buff[0], 2, WAVE_FILE_1));		//mono=1 stereo=2
	wave.iSamplingRate = dwSwab_dw(pbFFF_gets(&buff[0], 4, WAVE_FILE_1));	//44.1khz=44AC 22khz=2256 16khz=803E
	wave.iDataSpeed = dwSwab_dw(pbFFF_gets(&buff[0], 4, WAVE_FILE_1));		//データ速度
	wave.iBlockSize = wSwab_w(pbFFF_gets(&buff[0], 2, WAVE_FILE_1));		//ブロックサイズ (Byte/sample×チャンネル数)
	wave.iBits	  = wSwab_w(pbFFF_gets(&buff[0], 2, WAVE_FILE_1));			//bit数 16 or 8

	for (i = 36; i < File[WAVE_FILE_1].obj.objsize; i++) {			// data 部の頭出し
		buff[0] = buff[1];
		buff[1] = buff[2];
		buff[2] = buff[3];
		buff[3] = bFFF_getc(WAVE_FILE_1);
		if(strncmp(&buff[0],"data",4) == 0 ) {
			wave.ulDataByte = dwSwab_dw(pbFFF_gets(&buff[0], 4, WAVE_FILE_1));
			break;
		}
	}

	xprintf("BIT=%uBits\n", wave.iBits);
	xprintf("CH=%u\n", wave.iChannel);
	xprintf("BLOCK=%uBytes\n", wave.iBlockSize);
	xprintf("FQ=%luHz\n", wave.iSamplingRate);
	xprintf("DATABYTE=%uBytes\n>", wave.ulDataByte);
	return eWAVE_OK;
}


//******************************************************************************
//	start playing Wave data
//  It's a Interrupt_TMR3 task
//******************************************************************************
void vWAVE_play_start(void)
{

	OC1_PWMPulseWidthSet(PWM_CENTER_VALUE);	// PWM_CENTER_VALUE 128
	TMR2_Start();							// PWM source timer
	OC1_Start();							// PWM start

	TMR3_Period16BitSet( _XTAL_FREQ / wave.iSamplingRate );	// sampling rate set
	TMR3_Start();											// feed PWM periperal with WAVE data
	AUDIO_EN();

	wave.ePlayStatus = eWave_busy;
}

//******************************************************************************
//	playing some files. WAVE task setup and start playing
//
//	Description	:
//	 1.get file names(with path) by 1 line.
//	Precondition:
//	Input		: file names
//	Parameters	: file names
//	Returns		:
//******************************************************************************
void vWAVE_task11_open(
	char* fileNames
)
{
	char *ptr;
	
	while (*fileNames == ' ') fileNames++;	// skip space 
	ptr = fileNames;
	while (*ptr != '\0' && *ptr !='\n') ptr++;
	ptr = '\0';
	strcpy(waveTask11.fileNames, fileNames);

	waveTask11.pbFnm = &waveTask11.fileNames[0];
	waveTask11.status = eWaveT11_fileOpenAndPlayStart;

}

//******************************************************************************
//	playing some files. WAVE task control. 
//
//	Description	:
//	 1.get file names(with path) by 1 line.
//	 2.open wave file and start play.
//	 3.wait until idle.
//	 4.next wave file search.
//	Precondition: open task11 / rFFF_BlockRead(WAVE_FILE_1);
//	Input		: file names
//	Parameters	: file names
//	Returns		:
//******************************************************************************
void vWAVE_task11_control(void)
{
	char fnmbuff[BUFFER_LENGTH];
	char* fnm;

	switch (waveTask11.status) {
	case	eWaveT11_idle:					// 1.waiting to get file names(with path).
		break;

	case	eWaveT11_fileOpenAndPlayStart:	// 2.open wave file and start playing.
		while (*waveTask11.pbFnm == ' ') waveTask11.pbFnm++;	//skip space

		fnm = &fnmbuff[0];
		while (*waveTask11.pbFnm != '\0'  && *waveTask11.pbFnm !=' ' && *waveTask11.pbFnm !='\n'){
			*fnm = *waveTask11.pbFnm;
			waveTask11.pbFnm++;
			fnm++;
		}
		*fnm++ = '\0';
		xprintf("eWaveT11_fileOpenAndPlayStart = %s\n", fnmbuff);
		if(eWAVE_file_open(&fnmbuff[0]) != eWAVE_OK) {
//			waveTask11.status =  eWaveT11_nextFile;
			waveTask11.status =  eWaveT11_error;
			break;
		}
		vWAVE_play_start();
		waveTask11.status++;			//to next status
		break;

	case	eWaveT11_busy:				//	 3.wait until idle.
		if(wave.ePlayStatus !=  eWave_busy) {
			waveTask11.status++;		//to next status
		}
		break;

	case	eWaveT11_nextFile :
		while (*waveTask11.pbFnm != ' '  && *waveTask11.pbFnm !='\0') *waveTask11.pbFnm++;	//skip plyed file name
		while (*waveTask11.pbFnm == ' ') waveTask11.pbFnm++;	//skip space then next file name

		if(*waveTask11.pbFnm == '\0') {
			waveTask11.status = eWaveT11_end;
			break;
		}
		waveTask11.status = eWaveT11_fileOpenAndPlayStart;
		break;
		
	case	eWaveT11_end :
		waveTask11.status = eWaveT11_idle;
		break;
		
	case	eWaveT11_error :
		break;

	default:
		waveTask11.status = eWaveT11_end;
		break;
	}

}

//******************************************************************************
//	play a WAVE file by specified Number 
//
//	Description	:
//	 1.add pathName to text buffer
//	 2.add '/'
//	 3.get a file name from file number
//	 4.add the file name to text buffer
//	 5.set WAVE task parameter (task11 start)
//	Precondition:
//	Input		: file number, file number  
//	Parameters	:
//	Returns		:
//******************************************************************************
WAVE_RESULT eWAVE_task11_play_number(
	int		number,
	TCHAR* 	pathName
)
{
	TCHAR *ptr, *ptr2;
	TCHAR fnmbuff[BUFFER_LENGTH];
	
	while(*pathName == ' ') pathName++;					// skip space

	ptr2 = &fnmbuff[0];									// set in fnmbuff with path
	ptr = pathName;										// add file name
	for(1 ; *ptr != '\0' && *ptr != ' ' ; ptr++ ) {
		*ptr2 = *ptr;
		ptr2++;
	}

	*ptr2++ = '/';

	ptr = pbFFF_get_fileName(pathName, "wav", number);	// ptr = fileName 
	if(!ptr) return eWAVE_NOT_WAVE_FILE;
	xprintf("File name is %d %s\n", number, ptr);

	for(1 ; *ptr != '\0' && *ptr != ' '  && *ptr != '\n' ; ptr++ ){		// add filename to text buff
		*ptr2 = *ptr;
		ptr2++;
	}
	*ptr2++ = '\0';

	vWAVE_task11_open(&fnmbuff[0]);
	return eWAVE_OK;

}
	
//******************************************************************************
//	search word in a specified file and get a line then play wave file
//	 sample ==>	// wb <Time> <file> - search a file and get a Time line then Speaking
//
//	Description	:
//	 1.open file
//	 2.get a text line
//	 3.check the word which exist in the getted line
//	 4.when mach the word then play the wave file of in the getted line.
//	Precondition:
//	Input		: search word, file name 
//	Parameters	:
//	Returns		:
//******************************************************************************
WAVE_RESULT eWAVE_task11_speak_word(
	BYTE* 	searchWord,
	TCHAR*	fileName
)
{
	BYTE textbuff[BUFFER_LENGTH];
	int iRtnMatchCharBytes;
	FRESULT res;
	
	while(*searchWord == ' ') searchWord++;			// skip space
	while(*fileName == ' ') fileName++;				// skip space and get file name
//	rFFF_open(fileName, SD_FILE_0);
	res = rFFF_open(fileName, SD_FILE_0);
	if(res != FR_OK) {
		xprintf("eWAVE_task11_speak_word. file open error.\n");
		put_rc(res);
		return eWAVE_FILE_OPEN_ERROR;
	}
	
	while(iFFF_remains(SD_FILE_0)) {
		rFFF_BlockRead(SD_FILE_0);
		pbFFF_text_getline(&textbuff[0], sizeof(textbuff), SD_FILE_0);
		iRtnMatchCharBytes = iFFF_match_words(searchWord, &textbuff[0]);
		if(iRtnMatchCharBytes) {
			xprintf("Get!! file names = %s\n", textbuff);
			break;
		}
	}
	if(!(iRtnMatchCharBytes)) {						// if not match then..
		xputs("Not Found!!\n");
		return eWAVE_NOT_WAVE_FILE;
	}
	vWAVE_task11_open(&textbuff[iRtnMatchCharBytes]);	// skip the mach word and play wave file
	return eWAVE_OK;
}

//******************************************************************************
//	get some words. get words list file name
//
//	Description	:
//	 1.get search words.
//   2.get words list file name.
//	Precondition:
//	Input		: words, file name of search list.
//	Parameters	: words, file name of search list.
//	Returns		:
//******************************************************************************
void vWAVE_words12_open(
	BYTE* searchWords,
	TCHAR* wordsFileName
)
{
	xputs("vWAVE_words12_open ");
	xputs(searchWords);
	xputs("\n");
	
	TCHAR *ptr, *ptr2;
	
	while (*searchWords == ' ') searchWords++;	//skip space 
	strcpy(waveWords12.searchWords, searchWords);
	while (*wordsFileName == ' ') wordsFileName++;	//skip space 
	//strcpy(waveWords12.wordsFileName, wordsFileName);
	ptr = wordsFileName;
	ptr2 = waveWords12.wordsFileName;
	while (*ptr != ' ' && *ptr != '\0'){
		*ptr2 = *ptr;
		ptr++;
		ptr2++;
	}
	*ptr2 = '\0';

	waveWords12.pbWrd = &waveWords12.searchWords[0];
	waveWords12.pbFnm = &waveWords12.wordsFileName[0];
	waveWords12.status = eWaveW12_fileOpenAndSearchWord;

	xputs(waveWords12.pbFnm);
	xputs(" : ");
	xputs(waveWords12.pbWrd);
	xputs("\n");
}

//******************************************************************************
//	playing some words with words list file. WAVE task12 control
//
//	Description	:
//	 1.get words 
//	 2.open specified file and search a word.
//	 3.get a line which contain a word.
//	 4.play wave file.
//	 5.next wave file search.
//	Precondition: open task12
//	Input		: words, file name of search list.
//	Parameters	: words, file name of search list.
//	Returns		:
//******************************************************************************
void vWAVE_words12_control(void)
{
	switch (waveWords12.status) {
	case	eWaveW12_idle:					// 1.waiting to get words
		break;

	case	eWaveW12_fileOpenAndSearchWord:	// 2.open specified file then search a word
		while (*waveWords12.pbWrd == ' ') waveWords12.pbWrd++;	//skip space

		eWAVE_task11_speak_word(waveWords12.pbWrd, waveWords12.pbFnm);
		waveWords12.status++;			//to next status
		break;

	case	eWaveW12_busy:				//	 3.wait until idle.
		if(waveTask11.status == eWaveT11_idle) {
			waveWords12.status++;		//to next status
		}
		if(waveTask11.status == eWaveT11_error) {
			waveWords12.status = eWaveW12_end;		
		}
		break;

	case	eWaveW12_nextWord :
		while (*waveWords12.pbWrd != ' '  && *waveWords12.pbWrd !='\0') *waveWords12.pbWrd++;	//check next word
		while (*waveWords12.pbWrd == ' ') waveWords12.pbWrd++;	//skip space
		if(*waveWords12.pbWrd == '\0') {
			waveWords12.status = eWaveW12_end;
			break;
		}
		waveWords12.status = eWaveW12_fileOpenAndSearchWord;
		break;

	case	eWaveW12_end :
		waveWords12.status = eWaveW12_idle;
		break;

	default:
		waveWords12.status = eWaveW12_end;
		break;
	}

}

//******************************************************************************
//	playing music wave files until end of folder. WAVE task13 open
//
//	Description	:
//	 1.get a music path 
//	 2.get a start file number.
//	 3.foword status of task13 to playing. 
//	Precondition:
//	Input		: start music Number & music folder path 
//	Parameters	: play number, music foleder.
//	Returns		:
//******************************************************************************
void vWAVE_music13_open(
	int fileNumber,
	TCHAR* musicPath
)
{
	TCHAR *ptr, *ptr2;
	
	while (*musicPath == ' ') musicPath++;	//skip space 
	ptr = musicPath;
	ptr2 = waveMusic13.musicPath;
	while (*ptr != ' ' && *ptr != '\0'){	// copy musicPath to waveMusic13
		*ptr2 = *ptr;
		ptr++;
		ptr2++;
	}
	*ptr2 = '\0';

	waveMusic13.maxFnumber = iFFF_get_No_of_files(waveMusic13.musicPath, "wav");
	waveMusic13.fnumber = fileNumber;
	if(waveMusic13.fnumber > waveMusic13.maxFnumber){
		waveMusic13.fnumber = waveMusic13.maxFnumber;
	}else if(waveMusic13.fnumber < 1){
		waveMusic13.fnumber = 1;
	}
	waveMusic13.pbPth = &waveMusic13.musicPath[0];
	
	if(waveMusic13.fnumber < 1) {
		waveTask11.status = eWaveT11_error;
		waveMusic13.status = eWaveM13_end;
	}else{
		waveMusic13.status = eWaveM13_playNumberOfFiles;
	}
	
	xprintf("%s : %d\n", waveMusic13.pbPth, waveMusic13.fnumber);
}


//******************************************************************************
//	playing music wave files until end of folder. WAVE task13 control
//
//	Description	:
//	 1.get a music path 
//	 2.check the number of wave files in the music folder.
//	 3.check the start number. if it is little than number of wave files. 
//	 4.play number of wave file.
//	 5.up the play number tne next wave file.
//	Precondition: open task13
//	Input		: music folder path
//	Parameters	: play number. max number of wave files in the music foleder.
//	Returns		:
//******************************************************************************
void vWAVE_music13_control(void)
{
	int j;

	switch (waveMusic13.status) {
	case	eWaveM13_idle:				// 1.waiting to get words
		break;

	case	eWaveM13_playNumberOfFiles:	// 2.check the number of wave files in the music folder.
		j = iFFF_get_No_of_files(waveMusic13.musicPath, "wav");
		waveMusic13.maxFnumber = j;
		if(waveMusic13.fnumber > j || waveMusic13.fnumber < 1)  {
			waveMusic13.fnumber = 1;
			waveMusic13.status = eWaveM13_end;
			waveTask11.status = eWaveT11_end;
			break;
		}
		eWAVE_task11_play_number(waveMusic13.fnumber, waveMusic13.musicPath);
		waveMusic13.status++;			//to next status
		break;

	case	eWaveM13_busy:				//	 3.wait until idle.
		if(waveTask11.status == eWaveT11_idle) {
			waveMusic13.status++;		//to next status
		}
		if(waveTask11.status == eWaveT11_error) {
			waveMusic13.status = eWaveM13_end;		
		}
		break;

	case	eWaveM13_nextNumber :
		waveMusic13.fnumber++;
		waveMusic13.status = eWaveM13_playNumberOfFiles;
		break;

	case	eWaveM13_end :
		waveMusic13.status = eWaveM13_idle;
		break;

	default:
		waveMusic13.status = eWaveM13_end;
		break;
	}

}



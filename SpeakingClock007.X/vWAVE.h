/*******************************************************************************
 *	WAVE file process
 *	author  :hiroshi murakami
 *	created :20170129
 *	modified:-
 *  Timer2 : for PWM source clock
 *  Timer3 : for sampling rate of WAVE play
 ******************************************************************************/
#ifndef _vWAVE_H
#define _vWAVE_H

#include "integer.h"
#include "vFFF.h"

/*****************************
 * VARIABLES
 *****************************/

#define	PWM_CENTER_VALUE	128
#define	BUFFER_LENGTH	256

//---------------------------------------------------------
// WAVE parameter
//---------------------------------------------------------

// wave ojbect result statuses
typedef enum {
	eWAVE_ERROR99		= -99,
	eWAVE_FILE_OPEN_ERROR,
	eWAVE_FILE_READ_ERROR,
	eWAVE_NOT_WAVE_FILE,
	eWAVE_OK = 0
} WAVE_RESULT;

// wave playing Task
typedef enum {
	eWave_idle = 0,
	eWave_busy,
	eWave_end,
	eWave_close
} WAVE_PLAY_STATUS;

typedef struct {
	int iFmtByte, iFmtID, iChannel, iSamplingRate,
	    iDataSpeed,  iBlockSize, iBits;		//WAV Format Parameters1
	unsigned long ulDataByte;					//WAV Format Parameters2
	WAVE_PLAY_STATUS 	ePlayStatus;		// 1:Busy, 0:idle
} WAVE_STRUCT;


// Task11 playing some files.
typedef enum {
	eWaveT11_idle,					//     1.get file names(with path).
	eWaveT11_fileOpenAndPlayStart,	//     2.open wave file and start playing.
	eWaveT11_busy,					//     3.wait until idle.
	eWaveT11_nextFile,				//     4.next wave file search.
	eWaveT11_end,
	eWaveT11_error
} WAVE_TASK11_STATUS;

typedef struct {
	TCHAR fileNames[BUFFER_LENGTH];
	TCHAR* pbFnm;
	WAVE_TASK11_STATUS status;
} WAVE_TASK11_PROPERTY;

// Task12 playing some words.
typedef enum {
	eWaveW12_idle,					//     1.get file names(with path).
	eWaveW12_fileOpenAndSearchWord,	//     2.file open then search word.
	eWaveW12_busy,					//     3.wait until idle.
	eWaveW12_nextWord,				//     4.next word search.
	eWaveW12_end
} WAVE_TASK12_STATUS;

typedef struct {
	BYTE searchWords[BUFFER_LENGTH];
	BYTE* pbWrd;
	TCHAR wordsFileName[BUFFER_LENGTH];
	TCHAR* pbFnm;
	WAVE_TASK12_STATUS status;
} WAVE_TASK12_PROPERTY;


// Task13 playing music wave files until end of folder.
typedef enum {
	eWaveM13_idle,					//     1.waiting to get a music path
	eWaveM13_playNumberOfFiles,		//     2.check the number of wave files in the music folder.
	eWaveM13_busy,					//     3.play number of wave file.
	eWaveM13_nextNumber,			//     4.next number check.
	eWaveM13_end
} WAVE_TASK13_STATUS;

typedef struct {
	int fnumber;					// file number
	int maxFnumber;					// max file number in the musicPath
	TCHAR musicPath[20];
	TCHAR* pbPth;
	WAVE_TASK13_STATUS status;
} WAVE_TASK13_PROPERTY;


#if defined  _vWAVE_C
	//---------------------------------------------------------
	// FatFs Work Area
	//---------------------------------------------------------
	BYTE res;
	WORD s1, s2, s3, ofs, cnt, w;

	//---------------------------------------------------------
	// WAVE Work Area
	//---------------------------------------------------------
	WAVE_STRUCT wave;
	WAVE_TASK11_PROPERTY waveTask11;
	WAVE_TASK12_PROPERTY waveWords12;
	WAVE_TASK13_PROPERTY waveMusic13;

#else
	//---------------------------------------------------------
	// pFatFs Work Area
	//---------------------------------------------------------
	extern BYTE res;
	extern WORD s1, s2, s3, ofs, cnt, w;

	//---------------------------------------------------------
	// pFatFs Work Area
	//---------------------------------------------------------
	extern WAVE_STRUCT wave;
	extern WAVE_TASK11_PROPERTY waveTask11;
	extern WAVE_TASK12_PROPERTY waveWords12;
	extern WAVE_TASK13_PROPERTY waveMusic13;

#endif

/*****************************
 * PROTOTYPES
 *****************************/
WORD wSwab_w(unsigned char *A);	//change 2bytes stream to word (unsigned short)
DWORD dwSwab_dw(unsigned char *A);	//change 4bytes stream to double word (unsigned long)
void vWAVE_init(void) ;		//TIMER & PWM initialize
void vWAVE_close(void) ;	//TIMER & PWM close
void vWAVE_stop(void) ;	//TIMER & PWM close
void vTMR3_init(unsigned int T3_PR);
void Interrupt_TMR3(void);
unsigned short usGetPwmValue(void);

WAVE_RESULT eWAVE_file_open(BYTE *WaveFileName);
void vWAVE_play_start(void);

WAVE_RESULT eWAVE_task11_play_number(int number, TCHAR* pathName);
WAVE_RESULT eWAVE_task11_speak_word(BYTE* searchWord,	TCHAR* fileName);

void vWAVE_task11_open(TCHAR* fileNames);
void vWAVE_task11_control(void);	//playing some files.

void vWAVE_words12_open(BYTE* searchWords, TCHAR* wordsFileName);
void vWAVE_words12_control(void);	//playing some words with words list file.

void vWAVE_music13_open(int fileNumber, TCHAR* musicPath);
void vWAVE_music13_control(void);	//playing music wave files until end of folder.


#endif

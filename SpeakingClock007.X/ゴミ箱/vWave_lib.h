/*******************************************************************************
 *	WAVE 処理
 *	新規:20121122
 *	修正:-
 ******************************************************************************/

/*****************************
 * VARIABLES
 *****************************/
#include "pff.h"		//file system を構築する為、ここに記述

#define PW_AMP_EN()		LATBbits.LATB5 = 0	/* CS = L */
#define	PW_AMP_DISEN()	LATBbits.LATB5 = 1	/* CS = H */

#define OP_AMP_EN()		LATBbits.LATB10 = 1	/* CS = H */
#define	OP_AMP_DISEN()	LATBbits.LATB10 = 0	/* CS = L */

//---------------------------------------------------------
// pFatFs Work Area
//---------------------------------------------------------
//unsigned char Line[128];	// Console input buffer. SDカードの読み書きのバッファとしても使用
#define SD_BUFFER_SIZE 512
union WAVE{
BYTE	bLine[2][SD_BUFFER_SIZE];	// FIFO buffer Byte line//
SHORT	sLine[2][SD_BUFFER_SIZE/2];	// FIFO buffer Word line//
};
struct SDBUFF{
	union WAVE uWV;
	int		iFmtByte, iChannel, iLength, iBlockSize, iSamplingRate;	//WAV Format Parameters1
	unsigned long ulDataByte;						//WAV Format Parameters2

	int		ri;			// Read index, for reading out by Byte
	int		di;			// Data index, for reading out by Byte/Word block
	int		li;			// line index, for reading from SDBUFF
	int		buffli;		// buffer line index, for SD card read and write to bLine[buffi]
	int		iWaveBusy;	// 1:Busy, 0:idle

};
//---------------------------------------------------------

//---------------------------------------------------------
// File indexs
//---------------------------------------------------------
typedef struct {
	BYTE bIndexMin[5];
	BYTE bIndexMax[5];
	BYTE bIndexNxt[5];
} FILE_INDEXS;
//---------------------------------------------------------

enum eWaveC3 {
	eWaveC3_idle,	//idle
	eWaveC3_FileSystemOpen,	//file open
	eWaveC3_NameCheck,	//start
	eWaveC3_start,	//start
	eWaveC3_busy,	//busy
	eWaveC3_next,
	eWaveC3_end
};

#define WAVE_LIST_BUFFER_SIZE 128
#if defined WAVE_LIB

	enum eWaveC3 eWaveStatusC3;

	//---------------------------------------------------------
	// pFatFs Work Area
	//---------------------------------------------------------
	// struct SDBUFF SdFF;	//SD card FiFo // --> vWave_lib.h
//	unsigned char *ucPtr1, *ucPtr2;			// Pointer for Byte data
//	long p1, p2;
	BYTE res;
	WORD s1, s2, s3, ofs, cnt, w;
	FATFS fs;								//File system object
	DIR dir;								//Directory object
	FILINFO fno;							//File information
	//---------------------------------------------------------

	//---------------------------------------------------------
	// pFatFs Work Area
	//---------------------------------------------------------
	struct SDBUFF SdFF;	//SD card FiFo
	//---------------------------------------------------------
	BYTE bWaveFileNames[WAVE_LIST_BUFFER_SIZE + 1];
	BYTE *bWaveFileName_ptr;

#else
	extern enum eWaveC3 eWaveStatusC3;

	//---------------------------------------------------------
	// pFatFs Work Area
	//---------------------------------------------------------
	//extern struct SDBUFF SdFF;	//SD card FiFo // --> vWave_lib.h
//	extern unsigned char *ucPtr1, *ucPtr2;			// Pointer for Byte data
	extern BYTE res;
	extern WORD s1, s2, s3, ofs, cnt, w;
	extern FATFS fs;							//File system object
	extern DIR dir;								//Directory object
	extern FILINFO fno;							//File information
	//---------------------------------------------------------

	//---------------------------------------------------------
	// pFatFs Work Area
	//---------------------------------------------------------
	extern struct SDBUFF SdFF;	//SD card FiFo

#endif

/*****************************
 * PROTOTYPES
 *****************************/
unsigned short usSwab_w(unsigned char *A);	//2byteの順序をひっくり返す。
unsigned long ulSwab_dw(unsigned char *A);	//4byteの順序をひっくり返す。
void vWAVE_init(void) ;		//TIMER & PWM initialize
void vWAVE_close(void) ;	//TIMER & PWM close
void vTIMER3_init(unsigned int T3_PR);
//void vTIMER3_isr(void);
unsigned short usCalPwmValue(void);
void vTIMER4_init(void);
//void vTIMER4_isr(void);
void vReadNextSector(void);
BYTE bGetWaveProperty(BYTE *WaveFileName) ;
BYTE bSearchListAndGetFileName(BYTE *pListIndex, BYTE *pListName, BYTE **pReturnFileNames, FILE_INDEXS *pFileIndexs) ;	//ListのWaveFile名の取得
BYTE bSearchDirAndPlayWave(int iFileNo, BYTE *pListName, int *iFileMaxNo, BYTE **pReturnFileNames) ;		//DirectoryのWaveFile再生
void vWavePlayControl01(void) ;				//Wave Play status control
void vWavePlayStart(BYTE *ptr1) ;		//WAVE FILE LISTを渡してSTART
void vSpeakTimeMessage(void);			//時間の発声



/*******************************************************************************
 *	File 構造体
 *	新規:20131215
 *	修正:-
 ******************************************************************************/

/*****************************
 * VARIABLES
 *****************************/

#define SD_READ_ON 1			//
#define SD_READ_OFF 0			//
#define SD_BUFFER_LINE 2			//2
#define SD_BUFFER_SIZE _MAX_SS		//512
typedef struct {
	FIL fObject;					// File objects
	BYTE bBuff[SD_BUFFER_LINE][SD_BUFFER_SIZE];	// Working double buffer
	DWORD dwReadIndex;
	int iSDReading; //SD card read status ON:Now reading
}FATFILE;

typedef struct {
	int	iFormatID ,iChannel, iSamplingRate, iBlockSize, iBitSampling;	//WAV Format Parameters1
	DWORD dwDataSize;
}WAVEHEADER;

#define FCC(c1,c2,c3,c4)	(((DWORD)c4<<24)+((DWORD)c3<<16)+((WORD)c2<<8)+(BYTE)c1)	/* FourCC */
//#define FCC(c1,c2,c3,c4)	(((DWORD)c1<<24)+((DWORD)c2<<16)+((WORD)c3<<8)+(BYTE)c4)	/* FourCC */

#if defined FF_LIB
	//---------------------------------------------------------
	// pFatFs Work Area
	//---------------------------------------------------------
	FILINFO Finfo;
	FATFILE FF1;						// File object & readBuffer & readIndex
	FATFILE FF2;						// File object & readBuffer & readIndex

	FATFS FatFs;						// File system object
	FATFS *fs;							// Pointer to file system object
	DIR dir;							// Directory object
	const BYTE ft[] = {0,12,16,32};
	//FRESULT res;
	//UINT uiS1; // s2, s3, ofs, cnt, w;
	#if _USE_LFN
	TCHAR Lfname[_MAX_LFN + 1];
	#endif
#else
	extern FILINFO Finfo;
	extern FATFILE FF1;						// File object & readBuffer & readIndex
	extern FATFILE FF2;						// File object & readBuffer & readIndex

	extern FATFS FatFs;						// File system object
	extern FATFS *fs;						// Pointer to file system object
	extern DIR dir;							// Directory object
	extern const BYTE ft[];
	//extern FRESULT res;
	//extern UINT uiS1; // s2, s3, ofs, cnt, w;
	#if _USE_LFN
	extern TCHAR Lfname[_MAX_LFN + 1];
	#endif

#endif


/*****************************
 * PROTOTYPES
 *****************************/
unsigned short wSwab_w(unsigned char *A);	//2byteの順序をひっくり返す。
unsigned long dwSwab_dw(unsigned char *A);	//4byteの順序をひっくり返す。
int iFF_FirstRead(FATFILE* ff, const TCHAR* bFileName);
int iFF_ReadNextSector(FATFILE* ff);
int iFF_Getc(FATFILE* ff);
int iFF_Get_Line(BYTE* buff, int len, FATFILE* ff);
int iFF_Search_File( BYTE* buff, int len, BYTE* word, int wlen, FATFILE* ff);
int iFF_Get_Chars(BYTE* buff, int len, FATFILE* ff);
int iFF_Get_Skip(DWORD len, FATFILE* ff);
WORD wFF_Get_Word(FATFILE* ff);
DWORD dwFF_Get_DWord(FATFILE* ff);

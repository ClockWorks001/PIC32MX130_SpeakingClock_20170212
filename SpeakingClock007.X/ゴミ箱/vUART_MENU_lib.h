/*******************************************************************************
 *	USART_Menu処理
 *	新規:20120922
 *	修正:-
 ******************************************************************************/

/*****************************
 * VARIABLES
 *****************************/
enum eModeC2 {
	eModeC2_menu_msg1,					//Start message
	eModeC2_menu_init,
	eModeC2_eUART_CMND_Getc,
	eModeC2_vExecCmd,					//コマンド分岐
	eModeC2_Command_error,
	eModeC2_end
};

enum eTest01 {
	eTest01_wait,
	eTest01_file_open,
	eTest01_file_read,
	eTest01_file_close,
	eTest01_end
};

#if defined UART_MENU_LIB

	enum eModeC2 eModeStatusC2;
	unsigned char *ucPtr1; //, *ucPtr2;			// Pointer for Byte data
	//long p1, p2;

//	enum eTest01 eTest01Status;

#else
	extern enum eModeC2 eModeStatusC2;
#endif


/*****************************
 * PROTOTYPES
 *****************************/
void vUART_MENU_Init(void);
void vUART_MENU_Control01(void);

void vUART_Command_DateTimeSet(void);

void vUART_Command_FormatCheck_6Digit(void);	//Comand format check 6digit
void vUART_MessageOut_NowDateTime(void);		//時間の表示

void vUART_Command_FatFs(void);
void vUART_Command_vFF_test1(BYTE* bFileName);
void vUART_Command_vFF_test2(BYTE* bFileName);
void vUART_Command_vFF_test3(BYTE* bSearchWord, BYTE* bFileName);
int iUART_Command_vFF_test4(BYTE* bFileName);

void vPut_CRLF(void);
void vPut_Result (int rc);				//ステータスOUTPUT



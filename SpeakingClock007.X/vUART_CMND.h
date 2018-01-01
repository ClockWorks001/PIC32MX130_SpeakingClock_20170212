/*******************************************************************************
 *	serial monitor input
 *	UART2 use
 *	author  :hiroshi murakami
 *	created :20161225
 *	modified:-
 ******************************************************************************/
#ifndef _vUART_CMND_H
#define _vUART_CMND_H

#include "vFIFO.h"

/*****************************
 * VARIABLES
 *****************************/
#define CMND_BUFFER_SIZE 128					//コマンドバッファサイズ　UART I/O buffer size
enum eUART_CMND {
	eUART_CMND_Wait = 0,
	eUART_CMND_Exec,
	eUART_CMND_Esc,
	eUART_CMND_Del,
	eUART_CMND_End,
	eUART_CMND_Err
}	;


#if defined _vUART_CMND_C
	unsigned char cCmdBuf[CMND_BUFFER_SIZE + 1];	//コマンドバッファ、最後の一文字はnullの格納用
	unsigned char ucCPtr;							//コマンドバッファの挿入位置

	//xprintf FIFO
	FIFO_STRUCT TxFifo;
	BYTE TxBuff[256];

#else
	extern unsigned char cCmdBuf[];
	extern FIFO_STRUCT TxFifo;
	extern BYTE TxBuff[256];

#endif


/*****************************
 * PROTOTYPES
 *****************************/

void vUART_CMND_init(void);				//コマンドタスク用初期化処理
enum eUART_CMND eUART_CMND_Getc(void);	//コマンド読み込み、マンドバッファへ格納処理

void vPutc_to_TxFifo (BYTE d);			//for xprintf
void vUART_TxPutc(void);					//FIFO byte data stream output to UART TX


#endif

/*******************************************************************************
 *	UART_入力処理処理
 *	UART2を使用 *c32ではprintfはデフォルトでUSART2へ出力
 *	新規:20130130
 *	修正:-
 ******************************************************************************/
#define USART_CMND_LIB

#include <plib.h>            /* Include to use PIC32 peripheral libraries      */
#include <sys/attribs.h>     /* For __ISR definition                          */
#include <stdlib.h>
//#include <stdio.h>
#include "xprintf.h"

#include "vUSART_CMND_lib.h"
#include "integer.h"
#include "system.h"



/*****************************
 * initialization
 *****************************/
void vCMND_init(void) {
	for (ucCPtr = 0; ucCPtr < sizeof(cCmdBuf); cCmdBuf[ucCPtr++] = 0) ;	// Buffer clear
	ucCPtr = 0;							// バッファクリアの指示。バッファポインタを戻す

	// Assign UART2 Pin
	//  Rx:U2RX<==RPB8 (PORTB8)
    //U2RXR  = 0b0100;
	//  Rx:U2RX<==RPB11 (PORTB11)
    //U2RXR  = 0b0011;
	//  Rx:U2RX<==RPB11 (PORTB1)
    //U2RXR  = 0b00010;
	//  Rx:U2RX<==RPA1 (PORTA1)
    U2RXR  = 0b00000;
	//  Tx:RPB9R(PORTB9)==>U2TX
    //RPB9R = 0b0010;
	//  Tx:RPB10R(PORTB10)==>U2TX
    //RPB10R = 0b0010;
	//  Tx:RPB10R(PORTB0)==>U2TX
    //RPB0R = 0b0010;
	//  Tx:RPA3R(PORTA3)==>U2TX
    RPA3R = 0b0010;

    UARTConfigure(UART2, UART_ENABLE_PINS_TX_RX_ONLY);
//    UARTSetFifoMode(UART2, UART_INTERRUPT_ON_TX_DONE | UART_INTERRUPT_ON_RX_NOT_EMPTY);
//	  UARTSetFifoMode(UART2, UART_INTERRUPT_ON_TX_NOT_FULL | UART_INTERRUPT_ON_RX_NOT_EMPTY);
//    UARTSetFifoMode(UART2, UART_INTERRUPT_ON_TX_BUFFER_EMPTY | UART_INTERRUPT_ON_RX_NOT_EMPTY);
    UARTSetFifoMode(UART2, UART_INTERRUPT_ON_TX_BUFFER_EMPTY);
    UARTSetLineControl(UART2, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
    UARTSetDataRate(UART2, PBCLK, 57600);
    UARTEnable(UART2, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));

// Configure UART2 TX Interrupt
//	INTEnable(INT_SOURCE_UART_TX(UART2), INT_ENABLED);  //=?	//UxA/B have "sticky" interrupts, as you  have seen. !!!
	INTSetVectorPriority(INT_VECTOR_UART(UART2), INT_PRIORITY_LEVEL_2);
	INTSetVectorSubPriority(INT_VECTOR_UART(UART2), INT_SUB_PRIORITY_LEVEL_0);

	INTClearFlag(INT_SOURCE_UART_TX(UART2));
	INTEnable(INT_SOURCE_UART_TX(UART2), INT_DISABLED);

	TxFifo.ct = TxFifo.ri = TxFifo.wi = 0;
	//xdev_in(uart_getc);	// Join UART and console //
	xdev_out(uart_putc);

}

/*****************************
 * コマンド文字列取得,コマンドモード移行
 * 1文字づつ受信したデータをコマンドバッファに格納する。
 * エコーバック処理を行う
 *****************************/
enum eUSART_CMND vGetsCMND() {
	//	static char ch;
	unsigned char ucRxData;
	//		BYTE numBytesRead;

	if(UARTReceivedDataIsAvailable(UART2)) {		//受信状況をチェック。したら、
		// PIR1bits.RC1IF = 0;
		ucRxData = UARTGetDataByte(UART2);			//受信データを取り込む

		switch(ucRxData) {						//取り込んだ1文字をチェック
		case 0x00:								//null
			break;
		case '\b':	// BS
			if(ucCPtr > 0) {
				ucCPtr--;						//バッファから1文字削除
				//コマンド・バッファから1文字削除されたとき消されたように表示する
				xputs("\b \b");		//1文字もどってクリア
			}
			return eUSART_CMND_Wait;
			break;

		case '\r':	// Enter(コマンド確定)
			if(ucCPtr < CMND_BUFFER_SIZE) {
				//まだコマンドバッファに登録できるとき
				cCmdBuf[ucCPtr] = 0x00;			//NULLコード
			} else {
				//コマンドバッファが満杯のとき
				cCmdBuf[CMND_BUFFER_SIZE] = 0x00;		//NULLコード
			}
			ucCPtr = 0 ;						//コマンドバッファをクリア
			return eUSART_CMND_Exec;
			break;

		case '\x7F':	// (DEL)入力のキャンセル
			ucCPtr = 0 ;						//コマンドバッファをクリア
			xputs("\n\rDelete the Command");
			return eUSART_CMND_Del;
			break;

		case '\x1B':	// (ESC)メニュー階層のエスケープ
			ucCPtr = 0 ;						//コマンドバッファをクリア
			xputs("\n\rEscape the Command");
			return eUSART_CMND_Esc;
			break;

		default:	// コマンド文字列の取得
			if(ucCPtr < CMND_BUFFER_SIZE) {
				//まだコマンドバッファに登録できるとき
				cCmdBuf[ucCPtr] = ucRxData;			//コマンド文字を登録
				UARTSendDataByte(UART2, ucRxData);	//1文字エコーバック
				ucCPtr++;							//次の登録位置
			} else {
				xputs("\n\rCommand Over Flow");
				return eUSART_CMND_Err;
			}
		}

	}
	return eUSART_CMND_Wait;
}

//******************************************************************************************
//   Following section is xputs / xprintf
//******************************************************************************************
//******************************************************************************************
//   Function Name :
//   Title         : UART2 Tx interrupt ISR
//   Input         :
//   Output        :
//******************************************************************************************
void __ISR(_UART_2_VECTOR, ipl2) Intrupt_UART2(void)
{
	int i;

//	if ( INTGetFlag(INT_SOURCE_UART_TX(UART2)) ){
		INTClearFlag(INT_SOURCE_UART_TX(UART2));
		while(!U2STAbits.UTXBF){	// TX buffer has space //
			if (TxFifo.ct) {		// If any data is available, pop a byte and send it. //
				i = TxFifo.ri;
				U2TXREG = TxFifo.buff[i++];		// Send a byte //
				TxFifo.ri = i % TX_BUFFER_SIZE;	// Next read ptr //
				TxFifo.ct --;
			} else {				// No data in the Tx FIFO //
				INTEnable(INT_SOURCE_UART_TX(UART2), INT_DISABLED);
				break;
			}
		}
//	}
}

/* Put a byte into Tx FIFO */
void uart_putc (BYTE d)
{
	int i;

	while (TxFifo.ct >= TX_BUFFER_SIZE) ;	/* Wait while Tx FIFO is full */

	INTEnable(INT_SOURCE_UART_TX(UART2), INT_DISABLED);
	i = TxFifo.wi;		/* Put a data into the Tx FIFO */
	TxFifo.buff[i++] = d;
	TxFifo.wi = i % TX_BUFFER_SIZE;
	TxFifo.ct++;
	INTEnable(INT_SOURCE_UART_TX(UART2), INT_ENABLED);
}


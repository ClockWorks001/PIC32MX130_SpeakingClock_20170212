/*******************************************************************************
 *	USART_入力処理処理
 *	USART1を使用 *c18ではprintfはデフォルトでUSART1へ出力
 *	新規:20121006
 *	修正:-
 ******************************************************************************/

/*****************************
 * VARIABLES
 *****************************/
#define CMND_BUFFER_SIZE 128					//コマンドバッファサイズ　UART I/O buffer size
enum eUSART_CMND {
	eUSART_CMND_Wait,
	eUSART_CMND_Exec,
	eUSART_CMND_Esc,
	eUSART_CMND_Del,
	eUSART_CMND_End,
	eUSART_CMND_Err
}	;


#if defined USART_CMND_LIB
unsigned char cCmdBuf[CMND_BUFFER_SIZE + 1];			//コマンドバッファ、最後の一文字はnullの格納用
unsigned char ucCPtr;					//コマンドバッファの挿入位置

//xprintf FIFO
#define TX_BUFFER_SIZE 256
//static volatile int TxRun;		/* Tx running flag */
static volatile struct {
	int		ri, wi, ct;			/* Read index, Write index, Data counter */
	BYTE	buff[TX_BUFFER_SIZE];	/* FIFO buffer */
} TxFifo;

#else
extern unsigned char cCmdBuf[];
#endif


/*****************************
 * PROTOTYPES
 *****************************/

void vCMND_init(void);					//コマンドタスク用初期化処理
enum eUSART_CMND vGetsCMND(void);		//コマンド読み込み、マンドバッファへ格納処理

void uart_putc (BYTE d);				//for xprintf



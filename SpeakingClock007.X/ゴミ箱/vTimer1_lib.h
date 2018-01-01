/*******************************************************************************
 *	処理タイミング用 TIMER1 設定
 *	新規:20130128
 *	修正:-
 ******************************************************************************/

/*****************************
 * VARIABLES
 *****************************/
#if defined TIMER1_LIB
//short sMSecc = 0;						//msec時間計測用

//char cFlag10mSec = 0;                   //10msec時間計測用
//char cFlagSec = 0;						//1sec時間計測用
//char cFlagSec05 = 0;					//0.5sec時間計測用

#else
//extern char cFlag1mSec;
//extern char cFlag10mSec;
//extern char cFlagSec;
//extern char cFlagSec05;

#endif

/*****************************
 * PROTOTYPES
 *****************************/
void vTIMER1_init(void);
void vTIMER1_close(void);
int iIntervalOn(int *iCheckTime, int iWaitTime); //Whenever a interval time passes, it returns 1.
int iWaiting(int *iCheckTime, int iWaitTime);  //During waiting, it returns 1.


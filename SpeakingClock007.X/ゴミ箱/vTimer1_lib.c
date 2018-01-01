/*******************************************************************************
 *	処理タイミング用 TIMER1 設定
 *  100Hz 10msec interval
 *	新規:20130128
 *	修正:-
 ******************************************************************************/
#define TIMER1_LIB

//#include <xc.h>            /* Include to use PIC32 peripheral libraries      */
#include "mcc_generated_files/mcc.h"
#include "vTIMER1_lib.h"


//#define OUT_T1	LATBbits.LATB4
#define INTERVAL_END_TIME 32700		//-32768～32767 msec
#define WAITING_END 32767			//-32768～32767 msec
int iIntervalTimerMsec = 0; 		//10msecWait用※フリーラン


//******************************************************************************
//   Function Name :
//   Title         : Intrupt TIMER1
//                 :タイマ1割り込み処理  10msc毎
//   Input         :
//   Output        :
//******************************************************************************
void Intrupt_TIMER1(void)
{

/*
	sMSecc = sMSecc + 10;
	cFlag10mSec ++;					//10msec TaskFlag set. Switch, Task 用
	if(!(sMSecc % 500) ){			//500msec = 0.5sec 経過をチェック
		cFlagSec05++;				//0.5sec  TaskFlag set. Lcd 用
	}
	if(sMSecc >= 1000 ){			//1000msec = 1sec 経過をチェック
		sMSecc = 0;
		cFlagSec++;					//1sec TaskFlag set. LED moniter 用
	}
*/

	//インターバルタイマー
	iIntervalTimerMsec = iIntervalTimerMsec + 10;
	if(iIntervalTimerMsec >= INTERVAL_END_TIME){
		iIntervalTimerMsec = 1;
	}
}


//******************************************************************************
//   Function Name : iIntervalOn()
//   Title         : Check the iIntervalTimerMsec repeatedly.
//				   : When the first call, it returns 0.
//				   : When the iWaitTime passes, it returns 1.
//   Input         : iCheckTime (input output)
//                 : iWaitTime (measure is 1msec)
//   Output        : return 1:Waiting time passed  0:don't pass
//                 : iCheckTime :When Waiting time passed, it is set by new time
//******************************************************************************
int iIntervalOn(
    int *iCheckTime, 
    int iWaitTime
){
		if(*iCheckTime == 0){						//When the first call return 0
			*iCheckTime = iIntervalTimerMsec;
			return 0;								//The first call.
		}else if(iIntervalTimerMsec >= (*iCheckTime + iWaitTime)){		//When the Waiting time passes..
			*iCheckTime = iIntervalTimerMsec;
			return 1;								//When the iWaitTime passes, it returns 1.
		}else if(iIntervalTimerMsec < *iCheckTime){
			*iCheckTime = *iCheckTime - INTERVAL_END_TIME;
		}
		return 0;
}

//******************************************************************************
//   Function Name : iWaiting()
//   Title         : Check the iIntervalTimerMsec.
//				   : When the first call, it returns 1.
//                 : During waiting, it returns 1.
//				   : When the iWaitTime passes, it returns 0, and Waiting is end.
//   Input         : iCheckTime (input output)
//                 : iWaitTime (measure is 1msec)
//   Output        : return 1:Now Waiting   0:Not waiting.
//                 : iCheckTime :When Waiting time passed, it is set by new time
//******************************************************************************
int iWaiting(
    int *iCheckTime, 
    int iWaitTime
){
		if(*iCheckTime == WAITING_END){				//When waiting end, return 0
			return 0;								//When the iWaitTime passes, it returns 1.
		}else if(*iCheckTime == 0){					//When the first call return 1
			*iCheckTime = iIntervalTimerMsec;
			return 1;								//The first call.
		}else if(iIntervalTimerMsec >= (*iCheckTime + iWaitTime)){	//When the Waiting time passes..
			*iCheckTime = WAITING_END;
			return 0;								//When the iWaitTime passes, it returns 1.
		}else if(iIntervalTimerMsec < *iCheckTime){
			*iCheckTime = *iCheckTime - INTERVAL_END_TIME;
		}
		return 1;
}


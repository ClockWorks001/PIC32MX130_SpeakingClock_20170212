/*******************************************************************************
 *		処理タイミング用 TIMER1 設定
 *		1000Hz 1msec interval
 *		author  :hiroshi murakami
 *		created :20161225
 *		modified:-
 ******************************************************************************/
#define _vTMR1_C

#include "mcc_generated_files/mcc.h"
#include "vTMR1.h"

//******************************************************************************
//	Function Name :Intrupt TIMER1
//	Outline		:タイマ1割り込み処理  1000Hz 1msec interval
//	Input		:
//	Output		:
//******************************************************************************
void Intrupt_TMR1(void)
{
	if (uiTMR001 > 0) --uiTMR001; // 1000Hz decrement timer with zero stopped
	if (uiTMR002 > 0) --uiTMR002; // 1000Hz decrement timer with zero stopped
	if (uiTMR003 > 0) --uiTMR003; // 1000Hz decrement timer with zero stopped
	if (uiTMR004 > 0) --uiTMR004; // 1000Hz decrement timer with zero stopped
	if (uiTMR005 > 0) --uiTMR005; // 1000Hz decrement timer with zero stopped
	if (uiTMR006 > 0) --uiTMR006; // 1000Hz decrement timer with zero stopped
	if (uiTMR007 > 0) --uiTMR007; // 1000Hz decrement timer with zero stopped
}

void vTMR1_init(void)
{
    uiTMR001 = 0;
    uiTMR002 = 0;
    uiTMR003 = 0;
    uiTMR004 = 0;
    uiTMR005 = 0;
    uiTMR006 = 0;
    uiTMR007 = 0;
}



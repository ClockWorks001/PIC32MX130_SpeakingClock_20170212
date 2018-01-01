/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using MPLAB(c) Code Configurator

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  MPLAB(c) Code Configurator - 4.0
        Device            :  PIC32MX130F064B
        Driver Version    :  2.00
    The generated drivers are tested against the following:
        Compiler          :  XC32 1.40
        MPLAB             :  MPLAB X 3.40
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#include "mcc_generated_files/mcc.h"
#include "integer.h"
#include "xprintf.h"
#include "v.h"
#include "vTMR1.h"
#include "vRTCC.h"
#include "vUART_CMND.h"
#include "vUART_MENU.h"
#include "vFIFO.h"
//#include "vFFF.h"
#include "vWAVE.h"
#include "vLCD_I2C.h"
#include "vLCD_MENU.h"
#include "vADC.h"
#include "vPOWER.h"

#define LED1_Toggle()	IO_RB6_Toggle() 
#define LED1_ON()		LATBbits.LATB6=1
#define LED1_OFF()		LATBbits.LATB6=0
#define isLED1_ON()		PORTBbits.RB6
#define isLED1_OFF()	!(PORTBbits.RB6)


/*
                         Main application
 */
int main(void)
{
//	BYTE startmessage[] = "kon001.wav";
   	int wait_count;
   	BYTE buff[5];
	FRESULT res;
	
	for(wait_count=0;wait_count<300000;wait_count++);


    // initialize the device
    SYSTEM_Initialize();
	
	//for SW3, RB3 degital EN & pullup EN
	vADC_RB3_EN_AN5_DISE();
	
	vTMR1_init();
    vUART_CMND_init();
    vUART_menu_init();
//	vRTCC_init();

    xputs("rFFF_mount\n");
    vFFF_init();
    put_rc(rFFF_mount(&FatFs));     // file system mount
//    put_rc(rFFF_mount(&FatFs));     // file system mount
	
//    vWAVE_init();    //★★★★★★★★★★★
    vWAVE_close();    //★★★★★★★★★★★
	waveMusic13.fnumber = 1;
    AUDIO_EN();
	
//	LCD_POWER_EN();
	vLCD_task11_open();	
	vLCD_menu12_open();
	
    vLCD_cmd(0x80);              // display at Line 1 
	vLCD_str("Hello!");
    vLCD_cmd(0xC0);              // display at Line 2 
	vLCD_str("Are you fine?");
	
	vSENSE_init();		// for darkness & power voltage sense
	vSENSE_start();

//	eWAVE_file_open(startmessage);
//	vWAVE_play_start();

	vPORTchangeNoticeClear();
    vClearEventWDT();
	vEnableWDT();
	
	wait_count = 10000; //wait until sleep
    while (1)
    {
        // Add your application code

		//vClearWDT();
		
		// LED monitor
        if(IS_uiTMR001_FINISH){
			if(isLED1_ON()){
		        uiTMR001 = 1995; //msec period
				LED1_OFF();
			} else {
		        uiTMR001 = 5; //msec period
				LED1_ON();
			}
        }

		// RTCC task
		vRTCC_TimeCheck();
		
		// Switch check
        if(IS_uiTMR004_FINISH){
            uiTMR004 = 10; //msec period
			vSW1_Check();
			vSW2_Check();
			vSW3_Check();
			vSW4_Check();
			
			//cancel check SW4 is Cancellation
			if(cSW4_ON()){
				vWAVE_stop();
			    put_rc(rFFF_mount(&FatFs));     // file system re-mount
				alarmTask11.iAlarmRepeatCountdown = 0;
				alarmTask11.status = eAlarmT11_idle;
				
				vSWs_clear();
				LcdMenu12.status = eLcdM12_displayTimeDateOut;
				uiTMR007 = 0;	// for LCD display update immediately.
				wait_count = 500; //wait until sleep
			}else{
				vClearWDT();
			}
			
			//speaking time
			if(LcdMenu12.status < eLcdM12_menuSelectOpen){
				if(cSW3_ON() || cSW2_ON() ){
					RTCC_TimeGet(&currentTime);
					xsprintf(buff, "%02d%02d", currentTime.tm_hour, currentTime.tm_min);
					vWAVE_words12_open(buff, "TimeTone.txt");
					if(waveMusic13.status != eWaveM13_idle )waveMusic13.status = eWaveM13_playNumberOfFiles;
					LcdMenu12.status = eLcdM12_displayTimeDateOut;
					uiTMR007 = 0;	// for LCD display update immediately.
				}
			}
        }
		
		// Power Check 
		if(SENSEtask12.powerValue < vPOWER_38V_INTEGER){
			vLCD_task11_close();
			if (wave.ePlayStatus != eWave_close){
				vWAVE_close();
			}
		} else if(LcdTask11.status == eLcdT11_close){
			vLCD_task11_open();
		}

		// UART task
        vUART_TxPutc();			//FIFO byte data stream output to UART TX
        vUART_menu11_control();	//serial menu control.

		// SD card read
        rFFF_BlockRead(SD_FILE_0);		//for UART menu
        res = rFFF_BlockRead(WAVE_FILE_1);
        if(   (res != FR_OK)
		   && (res != FR_NO_FILE)) {
			waveTask11.status = eWaveT11_error;
		}
		
		// WAVE playing task
		vWAVE_task11_control();	//playing some files.
		
		if(alarmTask11.status != eAlarmT11_idle){
			vRTCC_alarm11_control();	//alarm repeat
		}else if(waveWords12.status != eWaveW12_idle){
			vWAVE_words12_control();	//playing some words with words list file.
		}else if(waveMusic13.status != eWaveM13_idle){
			vWAVE_music13_control();	//playing music wave files until end of folder.
		}

		// LCD menu
		vLCD_task11_control();	//LCD command & data byte stream output.
		vLCD_menu12_control();	//LCD menu control.

		// ADC task
		vADC_task();
		vSENSE_task();
		
		if( (waveTask11.status  == eWaveT11_idle || waveTask11.status == eWaveT11_error) &&
			(waveWords12.status == eWaveW12_idle) &&
			(waveMusic13.status == eWaveM13_idle) &&
			(alarmTask11.status == eAlarmT11_idle) &&
			(LcdTask11.status   == eLcdT11_idle || LcdTask11.status == eLcdT11_close)  &&
			(LcdMenu12.status   == eLcdM12_idle) &&
			(SENSEtask12.status == eSENSE_idle) &&
			(iFIFO_remains(&TxFifo) == 0) &&
			(iFIFO_remains(&LcdTask11.fifo) == 0) &&
			(isLED1_OFF())  &&
			(wait_count == 0)
		){
//			vWAVE_close;
//			xprintf("LcdMenu12.status=%u\n", LcdMenu12.status);
			// Night-Dark Sense
			if( (cFlagNightSenseON == vFLAG_ON) && 
				(SENSEtask12.brightValue < SENSEtask12.brightThreshold)
			){
				vLCD_task11_close();
			} else if(LcdTask11.status == eLcdT11_close){
				vLCD_task11_open();
			}

			wait_count = iPowerSave();
			vSENSE_start();
			uiTMR007 = 0;	// for LCD display update immediately.
			LcdMenu12.status = eLcdM12_displayTimeDateOut;
			uiTMR004 = 0;	// for Switch checking immediately.

		}

		// this wait time is for after menu setting display
		if(LcdMenu12.status   >  eLcdM12_displayTimeDateOut){
			wait_count = 500; //wait until sleep
		}else if(wait_count > 0){
			wait_count--;
		}

    }

    return -1;
}

/**
 End of File
*/

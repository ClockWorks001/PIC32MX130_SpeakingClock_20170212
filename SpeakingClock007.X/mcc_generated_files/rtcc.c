
/**
  RTCC Generated Driver API Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    rtcc.c

  @Summary:
    This is the generated header file for the RTCC driver using MPLAB(c) Code Configurator

  @Description:
    This header file provides APIs for driver for RTCC.
    Generation Information :
        Product Revision  :  MPLAB(c) Code Configurator - 4.15.5
        Device            :  PIC32MX130F064B
        Driver Version    :  0.5
    The generated drivers are tested against the following:
        Compiler          :  XC32 1.40
        MPLAB 	          :  MPLAB X 3.40
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


/**
 Section: Included Files
*/

#include <xc.h>
#include "rtcc.h"
#include "../vRTCC.h"

/**
* Section: Function Prototype
*/
static bool rtccTimeInitialized;
static bool RTCCTimeInitialized(void);
static uint8_t ConvertHexToBCD(uint8_t hexvalue);
static uint8_t ConvertBCDToHex(uint8_t bcdvalue);

/**
* Section: Driver Interface Function Definitions
*/

void RTCC_Initialize(void)
{
   SYSTEM_RegUnlock(); 

   RTCCONbits.RTCWREN = 1;

   RTCCONbits.ON = 0;
   
   //check the currentTime
   vRTCC_init();
   if(!RTCCTimeInitialized())
   {
       // set RTCC time 2017-03-22 22-30-00
       RTCDATE = 0x17032203;
       RTCTIME = 0x12000000;    
   }
   
   // set Alarm time 2016-12-28 00-00-00
   RTCALRMbits.ALRMEN = 0;
   ALRMDATE = 122803;   
   ALRMTIME = 0;

   // ALRMSYNC disabled; AMASK Every minute; ARPT 0; PIV disabled; CHIME enabled; ALRMEN enabled; 
   RTCALRM = 0xC300;
   // RTSECSEL Alarm pulse on RTCC pin; RTCCLKON disabled; SIDL disabled; RTCWREN disabled; RTCOE disabled; ON enabled; CAL 0; 
   RTCCON = 0x8000;
   // Clear Write Enable       
   RTCCONbits.RTCWREN = 0;
   SYSTEM_RegLock();

   //Enable RTCC interrupt
   IEC0bits.RTCCIE = 1;
}

/**
 This function implements RTCC_TimeReset.This function is used to
 used by application to reset the RTCC value and reinitialize RTCC value.
*/

void RTCC_TimeReset(bool reset)
{
    rtccTimeInitialized = reset;
}

static bool RTCCTimeInitialized(void)
{
    return(rtccTimeInitialized);
}

/**
 This function implements RTCC_TimeGet. It access the 
 registers of  RTCC and writes to them the values provided 
 in the function argument currentTime
*/

bool RTCC_TimeGet(struct tm *currentTime)
{
    uint32_t register_value;
    if(RTCCONbits.RTCSYNC){
        return false;
    }

    register_value = RTCDATE;
    currentTime->tm_year = ConvertBCDToHex((register_value & 0xFF000000) >> 24);
    currentTime->tm_mon = ConvertBCDToHex((register_value & 0xFF0000) >> 16);
    currentTime->tm_mday = ConvertBCDToHex((register_value & 0xFF00) >> 8);
    currentTime->tm_wday = ConvertBCDToHex(register_value & 0xF);
    register_value = RTCTIME;
    currentTime->tm_hour = ConvertBCDToHex((register_value & 0xFF000000) >> 24);
    currentTime->tm_min = ConvertBCDToHex((register_value & 0xFF0000) >> 16);
    currentTime->tm_sec = ConvertBCDToHex((register_value & 0xFF00) >> 8);
   
    return true;
}

/**
 * This function sets the RTCC value and takes the input time in decimal format
*/

void RTCC_TimeSet(struct tm *initialTime)
{
   
   SYSTEM_RegUnlock();

   RTCCONbits.RTCWREN = 1;
   RTCCONbits.ON = 0;

   IFS0bits.RTCCIF = false;
   IEC0bits.RTCCIE = 0;

   // set RTCC initial time
   RTCDATE =  ((ConvertHexToBCD(initialTime->tm_year) << 24) |
               (ConvertHexToBCD(initialTime->tm_mon) << 16) | 
               (ConvertHexToBCD(initialTime->tm_mday) << 8) |
               (ConvertHexToBCD(initialTime->tm_wday)));
   RTCTIME =  ((ConvertHexToBCD(initialTime->tm_hour) << 24) |
               (ConvertHexToBCD(initialTime->tm_min) << 16) | 
               (ConvertHexToBCD(initialTime->tm_sec) << 8));

   // Enable RTCC, clear RTCWREN         
   RTCCONbits.ON = 1;  
   RTCCONbits.RTCWREN = 0;
   SYSTEM_RegLock();

   IEC0bits.RTCCIE = 1;

}

/**
 This function reads the RTCC time and returns the date and time in BCD format
  */

bool RTCC_BCDTimeGet(bcdTime_t *currentTime)
{
    uint32_t register_value;
    if(RTCCONbits.RTCSYNC){
        return false;
    }

    register_value = RTCDATE;
    currentTime->tm_year = (register_value & 0xFF000000) >> 24;
    currentTime->tm_mon = (register_value & 0xFF0000) >> 16;
    currentTime->tm_mday = (register_value & 0xFF00) >> 8;
    currentTime->tm_wday = (register_value & 0xF);

    register_value = RTCTIME;
    currentTime->tm_hour = (register_value & 0xFF000000) >> 24;
    currentTime->tm_min = (register_value & 0xFF0000) >> 16;
    currentTime->tm_sec = (register_value & 0xFF00) >> 8;

    return true;
}

/**
 This function takes the input date and time in BCD format and sets the RTCC
 */

void RTCC_BCDTimeSet(bcdTime_t *initialTime)
{
   
   SYSTEM_RegUnlock();

   RTCCONbits.RTCWREN = 1;

   RTCCONbits.ON = 0;

   IFS0bits.RTCCIF = false;
   IEC0bits.RTCCIE = 0;

   // set RTCC initial time
   RTCDATE = ((initialTime->tm_year) << 24 | (initialTime->tm_mon << 16) | 
               (initialTime->tm_mday << 8) | (initialTime->tm_wday));
   RTCTIME = ((initialTime->tm_hour << 24) | (initialTime->tm_min << 16) | 
               (initialTime->tm_sec << 8)); 

   // Enable RTCC, clear RTCWREN
   RTCCONbits.ON = 1;
   RTCCONbits.RTCWREN = 0;
   SYSTEM_RegLock();

   IEC0bits.RTCCIE = 1;

}

static uint8_t ConvertHexToBCD(uint8_t hexvalue)
{
    uint8_t bcdvalue;
    bcdvalue = (hexvalue / 10) << 4;
    bcdvalue = bcdvalue | (hexvalue % 10);
    return (bcdvalue);
}

static uint8_t ConvertBCDToHex(uint8_t bcdvalue)
{
    uint8_t hexvalue;
    hexvalue = (((bcdvalue & 0xF0) >> 4)* 10) + (bcdvalue & 0x0F);
    return hexvalue;
}

/* Function:
  void __ISR(_RTCC_VECTOR, IPL1AUTO) _IntHandlerDrvRtcc( void )

  Summary:
    Interrupt Service Routine for the RTCC Peripheral

  Description:
    This is the interrupt service routine for the RTCC peripheral. Add in code if 
    required in the ISR. 
*/

void __ISR(_RTCC_VECTOR, IPL1AUTO) _IntHandlerDrvRtcc( void )
{
    /* TODO : Add interrupt handling code */
	//	xputs( "\n\rIntrupt_RTCC\n\r>");
	Interrupt_RTCC();

    IFS0bits.RTCCIF = false;
}


/**
 End of File
*/

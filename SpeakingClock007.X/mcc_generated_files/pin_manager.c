/**
  System Interrupts Generated Driver File 

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.c

  @Summary:
    This is the generated manager file for the MPLAB(c) Code Configurator device.  This manager
    configures the pins direction, initial state, analog setting.
    The peripheral pin select, PPS, configuration is also handled by this manager.

  @Description:
    This source file provides implementations for MPLAB(c) Code Configurator interrupts.
    Generation Information : 
        Product Revision  :  MPLAB(c) Code Configurator - 4.15.5
        Device            :  PIC32MX130F064B
        Version           :  1.02
    The generated drivers are tested against the following:
        Compiler          :  XC32 1.40
        MPLAB             :  MPLAB X 3.40

    Copyright (c) 2013 - 2015 released Microchip Technology Inc.  All rights reserved.

    Microchip licenses to you the right to use, modify, copy and distribute
    Software only when embedded on a Microchip microcontroller or digital signal
    controller that is integrated into your product or third party product
    (pursuant to the sublicense terms in the accompanying license agreement).

    You should refer to the license agreement accompanying this Software for
    additional information regarding your rights and obligations.

    SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
    EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
    MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
    IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
    CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
    OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
    INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
    CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
    SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
    (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

*/


/**
    Section: Includes
*/
#include <xc.h>
#include <sys/attribs.h>
#include "pin_manager.h"
#include "mcc.h"
#include "../vPOWER.h"

/**
    void PIN_MANAGER_Initialize(void)
*/
void PIN_MANAGER_Initialize(void)
{
    /****************************************************************************
     * Setting the Output Latch SFR(s)
     ***************************************************************************/
    LATA = 0x0000;
    LATB = 0x0000;

    /****************************************************************************
     * Setting the GPIO Direction SFR(s)
     ***************************************************************************/
    TRISA = 0x000D;
    TRISB = 0x2B1F;

    /****************************************************************************
     * Setting the Weak Pull Up and Weak Pull Down SFR(s)
     ***************************************************************************/
    CNPDA = 0x0000;
    CNPDB = 0x0800;
    CNPUA = 0x000C;
    CNPUB = 0x2004;

    /****************************************************************************
     * Setting the Open Drain SFR(s)
     ***************************************************************************/
    ODCA = 0x0000;
    ODCB = 0x0020;

    /****************************************************************************
     * Setting the Analog/Digital Configuration SFR(s)
     ***************************************************************************/
    ANSELA = 0x0001;
    ANSELB = 0x0008;

    /****************************************************************************
     * Set the PPS
     ***************************************************************************/
    SYSTEM_RegUnlock(); // unlock PPS
    CFGCONbits.IOLOCK = 0;

    RPB7Rbits.RPB7R = 0x0005;   //RB7->OC1:OC1;
    RPA4Rbits.RPA4R = 0x0007;   //RA4->INTERNAL OSCILLATOR:REFCLKO;
    REFCLKIRbits.REFCLKIR = 0x0002;   //RB4->INTERNAL OSCILLATOR:REFCLKI;
    U2RXRbits.U2RXR = 0x0003;   //RB11->UART2:U2RX;
    SDI2Rbits.SDI2R = 0x0003;   //RB13->SPI2:SDI2;
    RPA1Rbits.RPA1R = 0x0004;   //RA1->SPI2:SDO2;
    RPB10Rbits.RPB10R = 0x0002;   //RB10->UART2:U2TX;

    CFGCONbits.IOLOCK = 1; // lock   PPS
    SYSTEM_RegLock(); 

    /****************************************************************************
     * Interrupt On Change for group CNCONA&B - config
     ***************************************************************************/
	CNCONAbits.ON = 1; 
	CNCONBbits.ON = 1; 

    /****************************************************************************
     * Interrupt On Change for group CNENA - any
     ***************************************************************************/
	CNENAbits.CNIEA2 = 1; // Pin : RA2
	CNENAbits.CNIEA3 = 1; // Pin : RA3

    /****************************************************************************
     * Interrupt On Change for group CNENB - any
     ***************************************************************************/
	CNENBbits.CNIEB11 = 1; // Pin : RB11
	CNENBbits.CNIEB2 = 1; // Pin : RB2

    IEC1bits.CNAIE = 1; // Enable CNAI interrupt 
    IEC1bits.CNBIE = 1; // Enable CNBI interrupt 
    
}

/* Interrupt service routine for the change notification interrupt. */
void __ISR (_CHANGE_NOTICE_VECTOR, IPL1AUTO) _CHANGE_NOTICE ( void )
{
    if(IFS1bits.CNAIF == 1)
    {
        // Clear the flag
        IFS1bits.CNAIF = 0;
    }
    if(IFS1bits.CNBIF == 1)
    {
        // Clear the flag
        IFS1bits.CNBIF = 0;
    }
	
	// It need to confirm PORT and register
	vPORTchangeNoticeClear();
}

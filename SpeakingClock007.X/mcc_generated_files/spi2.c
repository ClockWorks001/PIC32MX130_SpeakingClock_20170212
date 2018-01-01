
/**
  SPI2 Generated Driver API Source File

  Company:
    Microchip Technology Inc.

  File Name:
    spi2.c

  @Summary
    This is the generated source file for the SPI2 driver using MPLAB(c) Code Configurator

  @Description
    This source file provides APIs for driver for SPI2.
    Generation Information :
        Product Revision  :  MPLAB(c) Code Configurator - 4.15.5
        Device            :  PIC32MX130F064B
        Driver Version    :  0.5
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

/**
  Section: Included Files
*/

#include <xc.h>
#include "spi2.h"

/**
 Section: File specific functions
*/

void SPI2_Exchange( uint8_t *pTransmitData, uint8_t *pReceiveData );
uint16_t SPI2_ExchangeBuffer(uint8_t *pTransmitData, uint16_t byteCount, uint8_t *pReceiveData);

/**
 Section: Driver Interface Function Definitions
*/


void SPI2_Initialize (void)
{
    // FRMERR disabled; 
    SPI2STAT = 0x0;
    // SPI2BRG 3; 
    SPI2BRG = 0x3;
    // AUDMONO disabled; AUDEN disabled; SPITUREN disabled; FRMERREN disabled; IGNROV disabled; SPISGNEXT disabled; SPIROVEN disabled; AUDMOD disabled; IGNTUR disabled; 
    SPI2CON2 = 0x0;
    // MCLKSEL PBCLK; DISSDO disabled; SRXISEL Last Word is Read; CKP Idle:Low, Active:High; FRMEN disabled; FRMSYPW One-Clock; SSEN disabled; FRMCNT 1; MSSEN disabled; MSTEN Master; MODE16 disabled; FRMPOL disabled; SMP End; SIDL disabled; FRMSYNC disabled; CKE Active to Idle; MODE32 disabled; SPIFE Frame Sync pulse precedes; STXISEL Complete; DISSDI disabled; ON enabled; ENHBUF enabled; 
    SPI2CON = 0x18320;

}


void SPI2_Exchange( uint8_t *pTransmitData, uint8_t *pReceiveData )
{

    while( SPI2STATbits.SPITBF == true )
    {

    }

    SPI2BUF = *((uint8_t*)pTransmitData);

    while ( SPI2STATbits.SPIRBE == true)
    {
    
    }

    *((uint8_t*)pReceiveData) = SPI2BUF;

}

uint16_t SPI2_ExchangeBuffer(uint8_t *pTransmitData, uint16_t byteCount, uint8_t *pReceiveData)
{

    uint16_t dataSentCount = 0;
    uint16_t count = 0;
    uint32_t dummyDataReceived = 0;
    uint32_t dummyDataTransmit = SPI2_DUMMY_DATA;

    uint8_t  *pSend, *pReceived;


    // set the pointers for transmit and receive operations
    if (pTransmitData == NULL)
    {
        pSend = (uint8_t*)&dummyDataTransmit;
    }
    else
    {
        pSend = (uint8_t*)pTransmitData;
    }
        
    if (pReceiveData == NULL)
    {
       pReceived = (uint8_t*)&dummyDataReceived;
    }
    else
    {
       pReceived = (uint8_t*)pReceiveData;
    }


    while( SPI2STATbits.SPITBF == true )
    {

    }

    while (dataSentCount < byteCount)
    {
        if ((count < ((SPI2_FIFO_FILL_LIMIT)*4)))
        {
            SPI2BUF = *pSend;
            pSend += 1;
            dataSentCount++;
            count++;
        }

        if (SPI2STATbits.SPIRBE == false)
        {
            *pReceived = SPI2BUF;
            pReceived += 1;
            count--;
        }

    }
    while (count)
    {
        if (SPI2STATbits.SPIRBE == false)
        {
            *pReceived = SPI2BUF;
            pReceived += 1;
            count--;
        }
    }

    return dataSentCount;
}



uint8_t SPI2_Exchange8bit( uint8_t data )
{
    uint8_t receiveData;
    
    SPI2_Exchange(&data, &receiveData);

    return (receiveData);
}


uint16_t SPI2_Exchange8bitBuffer(uint8_t *dataTransmitted, uint16_t byteCount, uint8_t *dataReceived)
{
    return (SPI2_ExchangeBuffer(dataTransmitted, byteCount, dataReceived));
}

SPI2_STATUS SPI2_StatusGet()
{
    return(SPI2STAT);
}

/**
 End of File
*/

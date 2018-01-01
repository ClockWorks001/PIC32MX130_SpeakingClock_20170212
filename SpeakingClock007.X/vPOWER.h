/*******************************************************************************
 *	POWER save
 *	author  :hiroshi murakami
 *	created :20161225
 *	modified:-
 ******************************************************************************/
#ifndef _vPOWER_H
#define _vPOWER_H

#include "mcc_generated_files/mcc.h"
#include "integer.h"

/*****************************
 * VARIABLES
 *****************************/

#if defined _vPOWER_C
 int iWait_Count;
#else

#endif

/*****************************
 * PROTOTYPES
 *****************************/
int iPowerSave(void);		// return wait count until next sleep 
void vPORTchangeNoticeClear(void);

#define vClearEventWDT()	RCONbits.WDTO =0
#define vClearWDT()			WDTCONbits.WDTCLR = 1 
#define vEnableWDT()		WDTCONbits.ON = 1 
#define vDisableWDT()		WDTCONbits.ON = 0  

#define AUDIO_EN()			LATBbits.LATB5 = 0	/* CS = L */
#define	AUDIO_DISEN()		LATBbits.LATB5 = 1	/* CS = H */

#define SDCARD_EN()			LATBbits.LATB14 = 0	/* MMC CS = L  select */
#define SDCARD_DISEN()		LATBbits.LATB14 = 1	/* MMC CS = H  disselect*/

#define vADC1_EN()			AD1CON1bits.ON = 1
#define vADC1_DISEN()		AD1CON1bits.ON = 0

#define vI2C1_EN()			I2C1CONbits.ON = 1
#define vI2C1_DISEN()		I2C1CONbits.ON = 0

#define vSPI2_EN()			SPI2CONbits.ON = 1
#define vSPI2_DISEN()		SPI2CONbits.ON = 0

#define vUART2_EN()			U2MODEbits.ON  = 1
#define vUART2_DISEN()		U2MODEbits.ON  = 0

#define vPOWER_5V_INTEGER	775		// 775 <- 1023*(2.5/3.3) <- 2.5v <- 5v
#define vPOWER_4V_INTEGER	619		// 619 <- 1023*(2.0/3.3) <- 2.0v <- 4v
#define vPOWER_38V_INTEGER	589		// 589 <- 1023*(1.9*/3.3) <- 1.9v <- 3.8v
#define vPOWER_get_integer(volt)	((volt / 5) * vPOWER_5V_INTEGER)
#define vPOWER_get_volt(integer)	((integer * 5) / vPOWER_5V_INTEGER)
#define vPOWER_get_pvolt(integer)	(((integer * 5) % vPOWER_5V_INTEGER) * 10  / vPOWER_5V_INTEGER)

#endif


/*******************************************************************************
 *	WAVE file process
 *	author  :hiroshi murakami
 *	created :20170129
 *	modified:-
 *  Timer2 : for PWM source clock
 *  Timer3 : for sampling rate of WAVE play
 ******************************************************************************/
#define _vWAVE_C

#include "mcc_generated_files/mcc.h"
#include <stdlib.h>
#include "vUART_MENU.h"
#include "vERROR_CODE.h"

#include "vWAVE.h"

//******************************************************************************
//   change 2bytes stream to word (unsigned short)
//******************************************************************************
WORD wSwab_w(
    unsigned char *A
)
{
	union SWAB_W_DATA{
		unsigned short usData;
		unsigned char ucData[2];
	} ;
	union SWAB_W_DATA B;
	B.ucData[0] = *(A);
	B.ucData[1] = *(++A);
	return B.usData;
}

//******************************************************************************
//   change 4bytes stream to double word (unsigned long)
//******************************************************************************
DWORD dwSwab_dw(
    unsigned char *A
)
{
	union SWAB_DW_DATA{
		unsigned long ulData;
		unsigned char ucData[4];
	} ;
	union SWAB_DW_DATA B;
	B.ucData[0] = *(A);
	B.ucData[1] = *(++A);
	B.ucData[2] = *(++A);
	B.ucData[3] = *(++A);
	return B.ulData;
}

//******************************************************************************
//   initialization
//   TMR3 is making Wave sampling rate
//******************************************************************************
void vTMR3_init(
    unsigned int T3_Hz
)
{
    TMR3_Counter16BitSet ( _XTAL_FREQ/T3_Hz );
    TMR3_Start();
}

//******************************************************************************
//   Interrupt TMR3 (call from MCC TMR3 interrupt call back routine )
//
//   Description   : PWM output by timing of sampling rate
//   Input         : sampling rate
//   Output        :
//******************************************************************************
void Interrupt_TMR3(void)
{
	int n;
	unsigned short pwm_value;

    IEC0bits.T3IE = false;
    
	if (iFFF_eof(WAVE_FILE_1)) {
		wave.eWaveStatus = eWave_end;
		OC1_PWMPulseWidthSet(PWM_CENTER_VALUE);	// PWM_CENTER_VALUE 128
        xputs("PLAY END!!\n>");
        TMR3_Stop();
		return;
	}

	pwm_value = usGetPwmValue();
	if (wave.iChannel==2){
		pwm_value = (pwm_value +  usGetPwmValue()) / 2;
	}

	OC1_PWMPulseWidthSet(pwm_value);	// PWM_CENTER_VALUE 128

    IEC0bits.T3IE = true;

}

//******************************************************************************
//   Get PWM Value from vFFF file
//
//   Description   : 16bits is signed data ==> unsigned 8bits
//                 : 8bits is unsigned data ==> unsigned 8bits
//   Input         : vFFF (file Bytes stream)
//   Output        : PwmValue(8bit 0-255)
//******************************************************************************
unsigned short usGetPwmValue(void) 
{
	union SWAB_W_DATA{
		short sData;
		BYTE  bData[2];
	} ;
   	union SWAB_W_DATA a;

	if (wave.iBits == 16) {
		//set PCM data to PWM (16bit 2ch)
        a.bData[0] = bFFF_getc(WAVE_FILE_1);
        a.bData[1] = bFFF_getc(WAVE_FILE_1);
		return (unsigned short)(a.sData / 0x100 + PWM_CENTER_VALUE); // PWM_CENTER_VALUE 128
	}else{
		return (unsigned short)bFFF_getc(WAVE_FILE_1) ;
	}
}

//******************************************************************************
//   WAVE object initialization
//******************************************************************************
void vWAVE_init(void) 
{
	AUDIO_EN();

    TMR2_Start();
    OC1_Start();
    
	xputs("rFFF_close\n");
    put_rc(rFFF_close(WAVE_FILE_1));
    
	xputs("vFFF_FIFO_init\n");
    vFFF_FIFO_init(WAVE_FILE_1);

	wave.eWaveStatus = eWave_idle;
}

//******************************************************************************
//   close PWM
//******************************************************************************
void vWAVE_close(void) 
{
	AUDIO_DISEN();
    
	// close OC1,Timer2,3
    OC1_Stop();
    TMR2_Stop();
    TMR3_Stop();

	xputs("rFFF_close\n");
    put_rc(rFFF_close(WAVE_FILE_1));
    
	xputs("vFFF_FIFO_init\n");
    vFFF_FIFO_init(WAVE_FILE_1);
    
	wave.eWaveStatus = eWave_close;
	// (4)Disconect PWM output Pin
	//  RPB7R(PORTB7)<==OC1
//  RPB7R = 0b0000;
//	LATBbits.LATB7 = 0;

}

//******************************************************************************
//   open Wave file & get a Property
//
//   Description   :
//     1.open the Wave file.
//     2.check the file header.
//     3.get the Wave property.
//   Precondition  :
//     The SD card must have been initialized & mount by "f_mount" before this function.
//   Input         : SD card
//   Parameters    :
//     BYTE *WaveFileName : wave file name.
//   Returns       :
//     The return status from file system.
//******************************************************************************
FRESULT rWAVE_file_open(
	BYTE* WaveFileName
) 
{
   	FRESULT res;
	BYTE buff[10];
	DWORD i;

	//----------------------------------------------------------------------
    vWAVE_init();
    
	xputs("rFFF_open ");
    xputs(WaveFileName);
	xputs("\n");
	res = rFFF_open(WaveFileName, WAVE_FILE_1);
    if (res != FR_OK) { put_rc(res); return res; }
    
    for(i = 0 ; i < SD_BUFFER_BLOCK ; i++){
        xprintf( "rFFF_BlockRead %02d\n", i);
        res = rFFF_BlockRead(WAVE_FILE_1);			// first read a file
        if (res != FR_OK) { put_rc(res); return res; }
    }

	//----------------------------------------------------------------------
    if(strncmp(pbFFF_gets(&buff[0], 4, WAVE_FILE_1),"RIFF",4) != 0 ){xputs("NO RIFF\n");return ERROR99;}
    pbFFF_gets(&buff[0], 4, WAVE_FILE_1);    //skip 4 bytes, RIFF data size
    if(strncmp(pbFFF_gets(&buff[0], 4, WAVE_FILE_1),"WAVE",4) != 0 ){xputs("NO WAVE\n");return ERROR99;}
    pbFFF_gets(&buff[0], 4, WAVE_FILE_1);    //skip 4 bytes, "fmt "

	wave.iFmtByte   = dwSwab_dw(pbFFF_gets(&buff[0], 4, WAVE_FILE_1));     //fmt byte のデータサイズ
	wave.iFmtID     = wSwab_w(pbFFF_gets(&buff[0], 2, WAVE_FILE_1));       //fmt ID
	wave.iChannel   = wSwab_w(pbFFF_gets(&buff[0], 2, WAVE_FILE_1));       //mono=1 stereo=2
	wave.iSamplingRate = dwSwab_dw(pbFFF_gets(&buff[0], 4, WAVE_FILE_1));   //44.1khz=44AC 22khz=2256 16khz=803E
	wave.iDataSpeed = dwSwab_dw(pbFFF_gets(&buff[0], 4, WAVE_FILE_1));      //データ速度
	wave.iBlockSize = wSwab_w(pbFFF_gets(&buff[0], 2, WAVE_FILE_1));        //ブロックサイズ (Byte/sample×チャンネル数)
	wave.iBits      = wSwab_w(pbFFF_gets(&buff[0], 2, WAVE_FILE_1));		//bit数 16 or 8

	for (i = 36; i < File[WAVE_FILE_1].obj.objsize; i++){		// data 部の頭出し
		buff[0] = buff[1];
		buff[1] = buff[2];
		buff[2] = buff[3];
		buff[3] = bFFF_getc(WAVE_FILE_1);
		if(strncmp(&buff[0],"data",4) == 0 ){
			wave.ulDataByte = dwSwab_dw(pbFFF_gets(&buff[0], 4, WAVE_FILE_1));
			break;
		}
	}

	xprintf("BIT=%uBits\n", wave.iBits);
	xprintf("CH=%u\n", wave.iChannel);
	xprintf("BLOCK=%uBytes\n", wave.iBlockSize);
	xprintf("FQ=%luHz\n", wave.iSamplingRate);
	xprintf("DATABYTE=%uBytes\n", wave.ulDataByte);
	return FR_OK;

}


//******************************************************************************
//   start play Wave data
//******************************************************************************
void vWAVE_play_start(void) 
{
    
	//vWAVE_init();
    TMR2_Start();			// PWM source timer
    OC1_Start();			// PWM start

	wave.eWaveStatus = eWave_busy;

	//vTMR3_init(wave.iSamplingRate);	// start wave playing
    TMR3_Period16BitSet( _XTAL_FREQ / wave.iSamplingRate ); // sampling rate set
    TMR3_Start();						// feed PWM periperal with WAVE data

}


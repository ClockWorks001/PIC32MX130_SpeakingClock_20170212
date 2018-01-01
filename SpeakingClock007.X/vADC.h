/*******************************************************************************
 *      ADC control
 *      author  :hiroshi murakami
 *      created :20170405
 *      modified:-
 ******************************************************************************/
#ifndef _vADC_H
#define _vADC_H

#include "integer.h"
#include "v.h"

/*****************************
 * VARIABLES
 *****************************/
typedef enum {
	eADC_idle,	
		eADC_start,
		eADC_stop,
		eADC_getResult
} ADC_STATUS;

typedef volatile struct {
	ADC1_CHANNEL channel;
	uint32_t	adcValue;
	ADC_STATUS	status; 
} ADC_TASK_PROPERTY; 

typedef enum {
	eSENSE_idle,
		eSENSE_Start,
		eSENSE_darkStart,
		eSENSE_darkWait,
		eSENSE_darkGetValue,
		eSENSE_powerStart,
		eSENSE_powerWait,
		eSENSE_powerGetValue,
		eSENSE_end
} SENSE_STATUS;

typedef volatile struct {
	int32_t		brightValue;
	int32_t		brightThreshold;
	int32_t		powerValue;
	int32_t		powerThreshold;
	SENSE_STATUS	status; 
} SENSE_TASK_PROPERTY; 

#if defined _vADC_C
	ADC_TASK_PROPERTY ADCtask11;
	SENSE_TASK_PROPERTY SENSEtask12;
	char cFlagNightSenseON;	//0:off 1:on
#else
	extern ADC_TASK_PROPERTY ADCtask11;
	extern SENSE_TASK_PROPERTY SENSEtask12;
	extern char cFlagNightSenseON;	//0:off 1:on
#endif

/*****************************
 * PROTOTYPES
 *****************************/
void vADC_open(ADC1_CHANNEL channel);
void vADC_task(void);
void vADC_AN5_EN_RB3_DISE (void);
void vADC_RB3_EN_AN5_DISE (void);

void vSENSE_init (void);
void vSENSE_task (void);
#define vSENSE_start()	SENSEtask12.status = eSENSE_Start


#endif





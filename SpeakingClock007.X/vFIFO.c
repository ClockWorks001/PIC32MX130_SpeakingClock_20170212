/*******************************************************************************
 *	  FIFO buffer for serial data
 *	  author  :hiroshi murakami
 *	  created :20160827
 *	  modified:-
 ******************************************************************************/
#define _vFIFO_C

#include "mcc_generated_files/mcc.h"
#include "string.h"
#include "vFIFO.h"

//******************************************************************************
// Get a byte from FIFO *return a data from buffer
//******************************************************************************
BYTE bFIFO_getc (
	FIFO_STRUCT* Fifo
)
{
	BYTE d;
	int i;

	Fifo->status = eFIFO_bufferStored;
	if (!Fifo->ct) {						//when FIFO buffer empty */
		Fifo->status = eFIFO_bufferEmpty;
		return 0x00;
	}

	///////////////////////////////////////////////////////////////////
	IEC0bits.T3IE = 0;						//for wave Interrupt_TMR3
	///////////////////////////////////////////////////////////////////
	
	i = Fifo->ri;							// Get a byte from FIFO
	d = Fifo->buffer[i++];
	Fifo->ct--;
	Fifo->ri = i % Fifo->buffer_size;
	
	///////////////////////////////////////////////////////////////////
	IEC0bits.T3IE = 1;						//for wave Interrupt_TMR3
	///////////////////////////////////////////////////////////////////

	return d;								//return a data
}

//******************************************************************************
// Put a byte into FIFO *return a status
//******************************************************************************
FIFO_STATUS eFIFO_putc (
	FIFO_STRUCT* Fifo,
	BYTE d
)
{
	int i;

	if (iFIFO_full(Fifo)) {					//  Return when FIFO is full
		Fifo->status = eFIFO_bufferFull;
		return  eFIFO_bufferFull;
	}

	Fifo->status = 0;
	i = Fifo->wi;							// Put a data into the FIFO
	Fifo->buffer[i++] = d;
	Fifo->ct++;
	Fifo->wi = i % Fifo->buffer_size;
	return  eFIFO_bufferStored;

}

//******************************************************************************
// Initialize FIFO instance
//******************************************************************************
void vFIFO_init (
	FIFO_STRUCT* Fifo,
	int buffer_size,
	BYTE* buffer
)
{
	/* Clear FIFOs */
	Fifo->ri = 0;
	Fifo->wi = 0;
	Fifo->ct = 0;
	Fifo->status = eFIFO_bufferEmpty;
	Fifo->buffer_size = buffer_size;
	Fifo->buffer = buffer;
	memset( (Fifo->buffer) , 0x00 , Fifo->buffer_size );
}



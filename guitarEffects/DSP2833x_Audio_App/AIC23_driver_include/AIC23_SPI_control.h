// TI File $Revision:  $
// Checkin $Date:  $
//###########################################################################
//
// FILE:	AIC23.h
//
// TITLE:	TLV320AIC23B Driver Register and Bit Definitions
//
//###########################################################################
// $TI Release:   $
// $Release Date:   $
//###########################################################################

#ifndef _AIC_23_H
#define _AIC_23_H

#include "DSP2833x_Device.h"

// AIC23_SPI_control.c External Function Prototypes
extern void init_mcbsp_spi();
void mcbsp_xmit(int a);
void aic23_init(int mic, int i2s_mode);

#endif

//===========================================================================
// End of file.
//===========================================================================

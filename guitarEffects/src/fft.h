/*
 * fft.h
 *
 *  Created on: Sep 22, 2014
 *      Author: brks9622
 */

#ifndef FFT_H_
#define FFT_H_

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "math.h"
#include "fpu_rfft.h"

#define RFFT_STAGES			9
#define RFFT_SIZE			(1 << RFFT_STAGES)

#define ADC_BUF_LEN			RFFT_SIZE	// ADC buffer length
#define ADC_SAMPLE_PERIOD   3124       	// 3124 = (3125-1) = 48 KHz sampling w/ 150 MHz SYSCLKOUT

#define F_PER_SAMPLE        1000.0L/(float)RFFT_SIZE  //Internal sampling rate is 48kHz

void initFFT();
int findFrequency();
int storeFFT(int);			//Return true if FFT ready to compute

#endif /* FFT_H_ */

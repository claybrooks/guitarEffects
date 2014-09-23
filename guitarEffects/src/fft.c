#include "DSP28x_Project.h"
#include "math.h"
#include "fpu_rfft.h"
#include "fft.h"

float RFFToutBuff[RFFT_SIZE];		//Calculated FFT result
float RFFTF32Coef[RFFT_SIZE];    	//Coefficient table buffer
float RFFTmagBuff[RFFT_SIZE/2+1];  	//Magnitude of frequency spectrum

//--- Global Variables
uint16_t AdcBuf[ADC_BUF_LEN];		// ADC buffer allocation

volatile uint16_t FFTStartFlag = 0;	// One frame data ready flag

uint16_t DEBUG_TOGGLE = 1;			// Used in realtime mode investigation

RFFT_ADC_F32_STRUCT rfft_adc;
RFFT_F32_STRUCT rfft;


void initFFT(){
	/*
	 * Link the Link the RFFT_ADC_F32_STRUCT to RFFT_F32_STRUCT.
	 * Tail pointer of RFFT_ADC_F32_STRUCT is passed to the OutBuf pointer
	 * of RFFT_F32_STRUCT.
	 */
	rfft_adc.Tail = &rfft.OutBuf;
	rfft.FFTSize  = RFFT_SIZE;			//Real FFT size
	rfft.FFTStages = RFFT_STAGES;		//Real FFT stages
	rfft_adc.InBuf = &AdcBuf[0];		//Input buffer
	rfft.OutBuf = &RFFToutBuff[0];		//Output buffer
	rfft.CosSinBuf = &RFFTF32Coef[0];	//Twiddle factor
	rfft.MagBuf = &RFFTmagBuff[0];		//Magnitude output buffer

	RFFT_f32_sincostable(&rfft);  		//Calculate twiddle factor
	int i = 0;

	//Clean up output buffer
	for (i=0; i < RFFT_SIZE; i++) RFFToutBuff[i] = 0;

	//Clean up magnitude buffer
	for (i=0; i < RFFT_SIZE/2; i++) RFFTmagBuff[i] = 0;
}

int findFrequency(){
	RFFT_adc_f32u(&rfft_adc);   // This version of FFT doesn't need buffer alignment
	RFFT_f32_mag(&rfft);		// Calculate spectrum amplitude
	int i = 0, j = 1, freq = RFFTmagBuff[1];
	for(i=2;i<RFFT_SIZE/2+1;i++)
	{
		//Looking for the maximum valude of spectrum magnitude
		if(RFFTmagBuff[i] > freq)
		{
			j = i;
			freq = RFFTmagBuff[i];
		}
	}
	freq = F_PER_SAMPLE * (float)j;	//Convert normalized digital frequency to analog frequency
	FFTStartFlag = 0;			   	//Start collecting the next frame of data
	return freq;
}

int storeFFT(int sample){
	static uint16_t *AdcBufPtr = AdcBuf;		// Pointer to ADC data buffer

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;		// Must acknowledge the PIE group

	//--- Read the ADC result
	*AdcBufPtr++ = sample;		// Read the result

	//--- Brute-force the circular buffer
	if( AdcBufPtr == (AdcBuf + ADC_BUF_LEN) ){
		AdcBufPtr = AdcBuf;						// Rewind the pointer to the beginning
		FFTStartFlag = 1;						// One frame data ready
	}
	return FFTStartFlag;
}

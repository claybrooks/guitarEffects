#include "adc.h"
#include "DSP28x_Project.h"

void initADC(){
	EALLOW;
	SysCtrlRegs.HISPCP.all = ADC_MODCLK;	// HSPCLK = SYSCLKOUT/ADC_MODCLK
	EDIS;

	InitAdc();  // For this example, init the ADC

	// Specific ADC setup for this example:
	AdcRegs.ADCTRL1.bit.ACQ_PS = ADC_SHCLK;
	AdcRegs.ADCTRL3.bit.ADCCLKPS = ADC_CKPS;
	AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;		// 1  Cascaded mode
	AdcRegs.ADCMAXCONV.all = 0x0001; 		//Number of conversions, hex + 1;

	//0x0-0x7 = A0-A7, 0x8-0xF = B0-B7
	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0; 	// ADCINA0 -> ADCRESULT0
	AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x8; 	// ADCINB0 -> ADCRESULT1

	AdcRegs.ADCTRL1.bit.CONT_RUN = 1;       // Setup continuous run

	AdcRegs.ADCTRL2.all = 0x2000;			// Start SEQ1
}

int getAdc(){
	return AdcRegs.ADCRESULT0>>4;
}

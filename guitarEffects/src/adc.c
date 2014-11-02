#include "../include/adc.h"
#include "DSP28x_Project.h"

void initAdc(void)
{
	EALLOW;
	SysCtrlRegs.HISPCP.all = ADC_MODCLK;	// HSPCLK = SYSCLKOUT/ADC_MODCLK
	EDIS;

	InitAdc();



	// Specific ADC setup for this example:
	AdcRegs.ADCTRL1.bit.ACQ_PS = ADC_SHCLK;
	AdcRegs.ADCTRL3.bit.ADCCLKPS = ADC_CKPS;
	AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;		// 1  Cascaded mode
	AdcRegs.ADCMAXCONV.all = 0x0002; 		//Number of conversions, hex + 1;

	//0x0-0x7 = A0-A7, 0x8-0xF = B0-B7
	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;   // ADCINA0 -> ADCRESULT0
	AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x1;   // ADCINA1 -> ADCRESULT1
	AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x2;   // ADCINA7 -> ADCRESULT2
	//AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 0x7;   // ADCINA3 -> ADCRESULT3
	//AdcRegs.ADCCHSELSEQ2.bit.CONV04 = 0x4;   // ADCINA4 -> ADCRESULT4
	//AdcRegs.ADCCHSELSEQ2.bit.CONV05 = 0x5;   // ADCINA5 -> ADCRESULT5
	//AdcRegs.ADCCHSELSEQ2.bit.CONV06 = 0x6;   // ADCINA6 -> ADCRESULT6



	AdcRegs.ADCTRL1.bit.CONT_RUN = 1;       // Setup continuous run
	AdcRegs.ADCTRL2.all |= 0x2000;			// Start SEQ1
}

int getAdc(){
	return AdcRegs.ADCRESULT0>>4;
}

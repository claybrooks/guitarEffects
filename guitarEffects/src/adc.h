// ADC start parameters
#if (CPU_FRQ_150MHZ)     // Default - 150 MHz SYSCLKOUT
  #define ADC_MODCLK 0x3 // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 150/(2*3)   = 25.0 MHz
#endif
#if (CPU_FRQ_100MHZ)
  #define ADC_MODCLK 0x2 // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 100/(2*2)   = 25.0 MHz
#endif
#define ADC_CKPS   0x1   // ADC module clock = HSPCLK/2*ADC_CKPS   = 25.0MHz/(1*2) = 12.5MHz
#define ADC_SHCLK  0xf   // S/H width in ADC module periods                        = 16 ADC clocks

void initADC(){
	// Specific clock setting for this example:
	EALLOW;
		SysCtrlRegs.HISPCP.all = ADC_MODCLK;	// HSPCLK = SYSCLKOUT/ADC_MODCLK
	EDIS;

	InitAdc();  // For this example, init the ADC

	// Specific ADC setup for this example:
	   AdcRegs.ADCTRL1.bit.ACQ_PS = ADC_SHCLK;
	   AdcRegs.ADCTRL3.bit.ADCCLKPS = ADC_CKPS;
	   AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;        // 1  Cascaded mode
	   AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;
	   AdcRegs.ADCTRL1.bit.CONT_RUN = 1;       // Setup continuous run
	  // Start SEQ1
	      AdcRegs.ADCTRL2.all = 0x2000;
}

int getAdc(){
	return AdcRegs.ADCRESULT0 >> 4;
}

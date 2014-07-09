interrupt void adc_isr(void);

#if (CPU_FRQ_150MHZ)     // Default - 150 MHz SYSCLKOUT
  #define ADC_MODCLK 0xf // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 150/(2*3)   = 25.0 MHz
#endif
#if (CPU_FRQ_100MHZ)
  #define ADC_MODCLK 0x2 // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 100/(2*2)   = 25.0 MHz
#endif
#define ADC_CKPS   0xf   // ADC module clock = HSPCLK/1      = 25.5MHz/(1)   = 25.0 MHz
#define ADC_SHCLK  0xf   // S/H width in ADC module periods                  = 2 ADC cycle

void initADC(){
	InitAdc();
	EALLOW;


	// Enable ADCINT in PIE
	PieCtrlRegs.PIEIER1.bit.INTx6 = 1;
	//PieVectTable.ADCINT = &adc_isr;
	IER |= M_INT1; // Enable CPU Interrupt 1

	SysCtrlRegs.HISPCP.all = ADC_MODCLK;	// HSPCLK = SYSCLKOUT/ADC_MODCLK
	AdcRegs.ADCTRL1.bit.ACQ_PS = ADC_SHCLK;
	AdcRegs.ADCTRL3.bit.ADCCLKPS = ADC_CKPS;
	AdcRegs.ADCTRL1.bit.CPS = 1;
	//1
	AdcRegs.ADCTRL1.bit.RESET = 0x1;
	int i;
	for(i = 0; i < 10; i++){}
	//2
	AdcRegs.ADCTRL1.bit.RESET = 0x0;
	AdcRegs.ADCTRL1.bit.CONT_RUN = 0x1;
	AdcRegs.ADCTRL1.bit.CPS = 1;
	AdcRegs.ADCREFSEL.bit.REF_SEL = 0x0003;
	//3
	AdcRegs.ADCTRL3.bit.ADCBGRFDN = 0x3;
	AdcRegs.ADCTRL3.bit.ADCPWDN = 0x1;
	AdcRegs.ADCTRL3.bit.ADCCLKPS = 0;
	//4
	AdcRegs.ADCMAXCONV.all = 0x0;
	//5
	AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 0x0;
	AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x0;
	AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x0;
	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;
	//6
	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 0x1;
	AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 0x1;
	//AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;
	EINT;          // Enable Global interrupt INTM
	ERTM;          // Enable Global realtime interrupt DBGM
}

interrupt void adc_isr(void);


void initADC(){
	InitAdc();
	EALLOW;
	//0
	SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 0x1;
	//1
	AdcRegs.ADCTRL1.bit.RESET = 0x1;
	int i;
	for( i = 0; i < 10; i++){}
	//2
	AdcRegs.ADCTRL1.bit.RESET = 0x0;
	AdcRegs.ADCTRL1.bit.CONT_RUN = 0x1;
	//3
	AdcRegs.ADCTRL3.bit.ADCBGRFDN = 0x3;
	AdcRegs.ADCTRL3.bit.ADCPWDN = 0x1;
	AdcRegs.ADCREFSEL.bit.REF_SEL = 0x1;
	//4
	AdcRegs.ADCMAXCONV.all = 0x0;
	//5
	AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 0x3;
	AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x3;
	AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x3;
	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x3;
	//6
	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 0x1;
	AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 0x1;
}

interrupt void cpu_timer0_isr(void);
//interrupt void xint1_isr(void);

void initINTS(){
	InitPieCtrl();
	// Disable CPU interrupts and clear all CPU interrupt flags:
	IER = 0x0000;
	IFR = 0x0000;
	InitPieVectTable();

	GpioCtrlRegs.GPADIR.bit.GPIO11 = 0;
}

void initXINT1(){
	EALLOW;
	//Mode toggle interrupt on GPIO 11
	PieVectTable.XINT1 = &xint1_isr;

	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;		// Enable the PIE block
	PieCtrlRegs.PIEIER1.bit.INTx4 = 1;		// Enable PIE Group 1 INT4
	PieCtrlRegs.PIEIER1.all

	GpioCtrlRegs.GPAMUX2.all &= 0x000F;		//Set GPIO20-31 to 0

	GpioCtrlRegs.GPADIR.all &= 0x000FFFFF;	//GPIO20-31 to input (0)

	GpioCtrlRegs.GPAQSEL2.all &= 0x000F;	//Synch to SYSCLKOUT GPIO20-31

	GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 0x1F;	//GPIO 31 as interrupt

	XIntruptRegs.XINT1CR.bit.POLARITY = 1;      // Rising edge interrupt
	XIntruptRegs.XINT1CR.bit.ENABLE = 1;        // Enable Xint1
}

void updateTIMER0(float time){
	ConfigCpuTimer(&CpuTimer0, 150, time);
	CpuTimer0Regs.TCR.all = 0x4001;
	IER |= M_INT1;
	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
	PieVectTable.TINT0 = &cpu_timer0_isr;
	EINT;   // Enable Global interrupt INTM
	ERTM;   // Enable Global realtime interrupt DBGM
}

void initTIMER0(){
	InitCpuTimers();
	EALLOW;
	ConfigCpuTimer(&CpuTimer0, 150,20.833f);
	CpuTimer0Regs.TCR.all = 0x4001;
	IER |= M_INT1;
	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
	PieVectTable.TINT0 = &cpu_timer0_isr;
	EINT;   // Enable Global interrupt INTM
	ERTM;   // Enable Global realtime interrupt DBGM
}

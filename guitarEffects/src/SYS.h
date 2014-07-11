interrupt void cpu_timer0_isr(void);
interrupt void xint1_isr(void);
void initSYS(){
	EALLOW;
	SysCtrlRegs.WDCR = 0x0068;  //Watchdog
	SysCtrlRegs.LOSPCP.all = 0x1;  //Low speed clock
}

void initPLL(){
	EALLOW;
	while(SysCtrlRegs.PLLSTS.bit.MCLKSTS == 1){}
	if(SysCtrlRegs.PLLSTS.bit.DIVSEL == 2 || SysCtrlRegs.PLLSTS.bit.DIVSEL == 3){
		SysCtrlRegs.PLLSTS.bit.DIVSEL = 0;
	}
	SysCtrlRegs.PLLSTS.bit.MCLKOFF = 1;
	SysCtrlRegs.PLLCR.all = 0xA;
	while(SysCtrlRegs.PLLSTS.bit.PLLLOCKS != 1){}
	SysCtrlRegs.PLLSTS.bit.MCLKOFF = 0;
	SysCtrlRegs.PLLSTS.bit.DIVSEL = 3;
}

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

	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;          // Enable the PIE block
	PieCtrlRegs.PIEIER1.bit.INTx4 = 1;          // Enable PIE Gropu 1 INT4

	GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 0;         // GPIO
	GpioCtrlRegs.GPADIR.bit.GPIO11 = 0;          // input
	GpioCtrlRegs.GPAQSEL1.bit.GPIO11 = 0;        // Xint1 Synch to SYSCLKOUT only


	GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 11;

	if(GpioDataRegs.GPADAT.bit.GPIO11 == 0) XIntruptRegs.XINT1CR.bit.POLARITY = 1;      // Rising edge interrupt
	else XIntruptRegs.XINT1CR.bit.POLARITY = 0;
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
	if(GpioDataRegs.GPADAT.bit.GPIO11 == 0) ConfigCpuTimer(&CpuTimer0, 150, 20.833f);
	else ConfigCpuTimer(&CpuTimer0, 150, 1000.0f);
	CpuTimer0Regs.TCR.all = 0x4001;
	IER |= M_INT1;
	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
	PieVectTable.TINT0 = &cpu_timer0_isr;
	EINT;   // Enable Global interrupt INTM
	ERTM;   // Enable Global realtime interrupt DBGM
}

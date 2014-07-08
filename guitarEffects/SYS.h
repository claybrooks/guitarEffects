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
}


/*void initTIMER0(){
	InitCpuTimers();
	EALLOW;
	ConfigCpuTimer(&CpuTimer0, 150, 22.75);
	CpuTimer0Regs.TCR.all = 0x4001;
	IER |= M_INT1;
	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
	PieVectTable.TINT0 = &cpu_timer0_isr;
	EINT;   // Enable Global interrupt INTM
	ERTM;   // Enable Global realtime interrupt DBGM
}*/

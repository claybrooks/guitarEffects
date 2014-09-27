/*
 * sys.c
 *
 *  Created on: Sep 19, 2014
 *      Author: Clay
 */
#include "sys.h"
#include "DSP28x_Project.h"

void initINTS(){
	/*
	 * Move Pie Vect Table To RAM
	 */
		PieCtrlRegs.PIECTRL.bit.ENPIE = 0; // Disable the PIE
		asm(" EALLOW"); // Enable EALLOW protected register access
		memcpy((void *)0x000D00, &PieVectTableInit, 256);
		asm(" EDIS"); // Disable EALLOW protected register access
	/*
	 *
	 */




	InitPieCtrl();
	// Disable CPU interrupts and clear all CPU interrupt flags:
	IER = 0x0000;
	IFR = 0x0000;
	InitPieVectTable();
	EALLOW;
	//Timer0
		PieVectTable.TINT0 = &cpu_timer0_isr;
		ConfigCpuTimer(&CpuTimer0, 150, 22.67);
		CpuTimer0Regs.TCR.all = 0x4001;
		PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

	//XINT1
		EALLOW;
		//Mode toggle interrupt on GPIO 25
		PieVectTable.XINT1 = &xint1_isr;
		PieCtrlRegs.PIECTRL.bit.ENPIE = 1;          // Enable the PIE block
		PieCtrlRegs.PIEIER1.bit.INTx4 = 1;          // Enable PIE Gropu 1 INT4

		GpioCtrlRegs.GPAMUX1.all &= 0x0FFF;
		GpioCtrlRegs.GPADIR.all  &= 0xFFFF0FFF;

		GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 25;	//GPIO 25 as interrupt
		GpioCtrlRegs.GPAQSEL2.bit.GPIO25 = 0;		//Sync to SYSCLKOUT
		GpioCtrlRegs.GPACTRL.bit.QUALPRD0 = 0xFF;   // Each sampling window is 510*SYSCLKOUT

		XIntruptRegs.XINT1CR.bit.POLARITY = 1;      // Rising edge interrupt
		XIntruptRegs.XINT1CR.bit.ENABLE = 1;        // Enable Xint1

		EINT;   // Enable Global interrupt INTM
		ERTM;   // Enable Global realtime interrupt DBGM
		IER |= M_INT1;
}

void updateTimer0(float time){
	ConfigCpuTimer(&CpuTimer0, 150, time);
	CpuTimer0Regs.TCR.bit.TSS = 0;      // 1 = Stop timer, 0 = Start/Restart Timer
}

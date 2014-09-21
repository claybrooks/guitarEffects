/*
 * sys.c
 *
 *  Created on: Sep 19, 2014
 *      Author: Clay
 */
#include "sys.h"
#include "DSP28x_Project.h"

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
	//Mode toggle interrupt on GPIO 25
	PieVectTable.XINT1 = &xint1_isr;
	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;          // Enable the PIE block
	PieCtrlRegs.PIEIER1.bit.INTx4 = 1;          // Enable PIE Gropu 1 INT4
	IER |= M_INT1;                              // Enable CPU int1
	EINT;                                       // Enable Global Interrupts

	GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 0;
	GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 0;

	GpioCtrlRegs.GPADIR.bit.GPIO8 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO9 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO10 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO11 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO12 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO13 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO14 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO15 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO25 = 0;

	GpioCtrlRegs.GPAQSEL2.bit.GPIO25 = 0;		//Sync to SYSCLKOUT GPIO20-31
	GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 25;	//GPIO 25 as interrupt
	GpioCtrlRegs.GPACTRL.bit.QUALPRD0 = 0xFF;   // Each sampling window is 510*SYSCLKOUT

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





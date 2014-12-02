/*
 * interrupts.c
 *
 *  Created on: Sep 19, 2014
 *      Author: Clay, Jonathan
 */
#include "interrupts.h"
#include "DSP28x_Project.h"

void initINTS(){
	DINT;
	InitPieCtrl();
	IER = 0x0000;
	IFR = 0x0000;
	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;        //Enable the PIE block
	// Disable CPU interrupts and clear all CPU interrupt flags:
	InitCpuTimers();
	EALLOW;
	//Timer0
		ConfigCpuTimer(&CpuTimer0,150,24);
		PieVectTable.TINT0 = &cpu_timer0_isr;
		CpuTimer0Regs.TCR.all = 0x4001;
		PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
	//Timer1
		ConfigCpuTimer(&CpuTimer1, 150, 3000000);
		PieVectTable.XINT13 = &timeout;
		PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

	//External Interrupts
		//Set up isr functions
		PieVectTable.XINT1 = &preset_up;
		PieVectTable.XINT2 = &preset_down;
		PieVectTable.XINT3 = &load_preset;
		PieVectTable.XINT4 = &save_preset;
		PieVectTable.XINT5 = &effects;
		PieVectTable.XINT6 = &rotary;

		//preset up
		PieCtrlRegs.PIEIER1.bit.INTx4 = 1;          //Enable PIE Group 1 INT4
		GpioCtrlRegs.GPADIR.bit.GPIO5 = 0;			//GPIO5 as input
		GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 5;	//GPIO5 as interrupt
		GpioCtrlRegs.GPAQSEL1.bit.GPIO5 = 2;		//XINT1 Qual using 6 samples
		GpioCtrlRegs.GPACTRL.bit.QUALPRD1 = 0xFF;   //Each sampling window is 510*SYSCLKOUT
		XIntruptRegs.XINT1CR.bit.POLARITY = 1;      //Rising edge interrupt
		XIntruptRegs.XINT1CR.bit.ENABLE = 1;        //Enable Xint1

		//preset down
		PieCtrlRegs.PIEIER1.bit.INTx5 = 1;          //Enable PIE Group 1 INT5
		GpioCtrlRegs.GPADIR.bit.GPIO6 = 0;			//GPIO6 as input
		GpioIntRegs.GPIOXINT2SEL.bit.GPIOSEL = 6;	//GPIO6 as interrupt
		GpioCtrlRegs.GPAQSEL1.bit.GPIO6 = 2;		//XINT2 Qual using 6 samples
		GpioCtrlRegs.GPACTRL.bit.QUALPRD1 = 0xFF;   //Each sampling window is 510*SYSCLKOUT
		XIntruptRegs.XINT2CR.bit.POLARITY = 1;      //Rising edge interrupt
		XIntruptRegs.XINT2CR.bit.ENABLE = 1;        //Enable Xint2

		//save preset
		PieCtrlRegs.PIEIER12.bit.INTx1 = 1;         //Enable PIE Group 12 INT1
		GpioCtrlRegs.GPBDIR.bit.GPIO48 = 0;			//GPIO48 as input
		GpioIntRegs.GPIOXINT3SEL.bit.GPIOSEL = 16;	//GPIO48 as interrupt
		GpioCtrlRegs.GPBQSEL2.bit.GPIO48 = 2;		//XINT3 Qual using 6 samples
		GpioCtrlRegs.GPBCTRL.bit.QUALPRD2 = 0xFF;   //Each sampling window is 510*SYSCLKOUT
		XIntruptRegs.XINT3CR.bit.POLARITY = 1;      //Rising edge interrupt
		XIntruptRegs.XINT3CR.bit.ENABLE = 1;        //Enable Xint3

		//load Preset
		PieCtrlRegs.PIEIER12.bit.INTx2 = 1;         //Enable PIE Group 12 INT2
		GpioCtrlRegs.GPBDIR.bit.GPIO49 = 0;			//GPIO49 as input
		GpioIntRegs.GPIOXINT4SEL.bit.GPIOSEL = 17;	//GPIO49 as interrupt
		GpioCtrlRegs.GPBQSEL2.bit.GPIO49 = 2;		//XINT4 Qual using 6 samples
		GpioCtrlRegs.GPBCTRL.bit.QUALPRD2 = 0xFF;   //Each sampling window is 510*SYSCLKOUT
		XIntruptRegs.XINT4CR.bit.POLARITY = 1;      //Rising edge interrupt
		XIntruptRegs.XINT4CR.bit.ENABLE = 1;        //Enable Xint4

		//Effects
		PieCtrlRegs.PIEIER12.bit.INTx3 = 1;         //Enable PIE Group 12 INT3
		GpioCtrlRegs.GPBDIR.bit.GPIO36 = 0;			//GPIO36 as input
		GpioIntRegs.GPIOXINT5SEL.bit.GPIOSEL = 4;	//GPIO36 as interrupt
		GpioCtrlRegs.GPBQSEL1.bit.GPIO36 = 2;		//XINT5 Qual using 6 samples
		GpioCtrlRegs.GPBCTRL.bit.QUALPRD0 = 0xFF;   //Each sampling window is 510*SYSCLKOUT
		XIntruptRegs.XINT5CR.bit.POLARITY = 1;      //Rising edge interrupt
		XIntruptRegs.XINT5CR.bit.ENABLE = 1;        //Enable Xint5

		//Rotary
		PieCtrlRegs.PIEIER12.bit.INTx4 = 1;          //Enable PIE Group 12 INT4
		GpioCtrlRegs.GPBDIR.bit.GPIO34 = 0;			//GPIO34 as input
		GpioIntRegs.GPIOXINT6SEL.bit.GPIOSEL = 2;	//GPIO34 as interrupt
		GpioCtrlRegs.GPBQSEL1.bit.GPIO34 = 2;		//XINT6 Qual using 6 samples
		GpioCtrlRegs.GPBCTRL.bit.QUALPRD0 = 0xFF;   //Each sampling window is 510*SYSCLKOUT
		XIntruptRegs.XINT6CR.bit.POLARITY = 1;      //Rising edge interrupt
		XIntruptRegs.XINT6CR.bit.ENABLE = 1;        //Enable Xint6

	//I2C
		PieVectTable.I2CINT1A = &i2c_int1a_isr;
		PieCtrlRegs.PIEIER8.bit.INTx1 = 1;

	IER |= M_INT1;
	IER |= M_INT7;
	IER |= M_INT8;
	IER |= M_INT12;
	IER |= M_INT13;

	EINT;   // Enable Global interrupt INTM
	ERTM;   // Enable Global realtime interrupt DBGM

}



void updateTimer0(float time){
	ConfigCpuTimer(&CpuTimer0, 150, time);
	CpuTimer0Regs.TCR.bit.TSS = 0;      // 1 = Stop timer, 0 = Start/Restart Timer
}
void updateTimer1(float time){
	ConfigCpuTimer(&CpuTimer1, 150, time);
	CpuTimer1Regs.TCR.bit.TSS = 0;      // 1 = Stop timer, 0 = Start/Restart Timer
	CpuTimer1Regs.TCR.all = 0x4001;
}

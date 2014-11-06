/*
 * sys.c
 *
 *  Created on: Sep 19, 2014
 *      Author: Clay
 */
#include "sys.h"
#include "DSP28x_Project.h"

void initINTS(){
	DINT;
	InitPieCtrl();
	IER = 0x0000;
	IFR = 0x0000;
	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;        //Enable the PIE block
	// Disable CPU interrupts and clear all CPU interrupt flags:
	InitCpuTimers();
	//InitPieVectTable();
	EALLOW;
	//Timer0
			ConfigCpuTimer(&CpuTimer0,75,23);
			PieVectTable.TINT0 = &cpu_timer0_isr;
			CpuTimer0Regs.TCR.all = 0x4001;
			PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
	//Timer1
		ConfigCpuTimer(&CpuTimer1, 75, 3000000);
		PieVectTable.XINT13 = &cpu_timer1_isr;
		//CpuTimer1Regs.TCR.all = 0x4001;
		PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

	//External Interrupts
			//Set up isr functions
			PieVectTable.XINT1 = &preset_up;
			PieVectTable.XINT2 = &preset_down;
			PieVectTable.XINT3 = &load_preset;
			PieVectTable.XINT4 = &save_preset;
			PieVectTable.XINT5 = &effects;

			//preset up
			PieCtrlRegs.PIEIER1.bit.INTx4 = 1;          //Enable PIE Gropu 1 INT4
			GpioCtrlRegs.GPADIR.bit.GPIO5 = 0;			//As input
			GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 5;	//GPIO 5 as interrupt
			GpioCtrlRegs.GPAQSEL1.bit.GPIO5 = 2;		//XINT1 Qual using 6 samples
			GpioCtrlRegs.GPACTRL.bit.QUALPRD1 = 0xFF;   //Each sampling window is 510*SYSCLKOUT
			XIntruptRegs.XINT1CR.bit.POLARITY = 1;      //Rising edge interrupt
			XIntruptRegs.XINT1CR.bit.ENABLE = 1;        //Enable Xint1

			//preset down
			PieCtrlRegs.PIEIER1.bit.INTx5 = 1;          //Enable PIE Gropu 1 INT4
			GpioCtrlRegs.GPADIR.bit.GPIO6 = 0;			//As input
			GpioIntRegs.GPIOXINT2SEL.bit.GPIOSEL = 6;	//GPIO 5 as interrupt
			GpioCtrlRegs.GPAQSEL1.bit.GPIO6 = 2;		//XINT1 Qual using 6 samples
			GpioCtrlRegs.GPACTRL.bit.QUALPRD1 = 0xFF;   //Each sampling window is 510*SYSCLKOUT
			XIntruptRegs.XINT2CR.bit.POLARITY = 1;      //Rising edge interrupt
			XIntruptRegs.XINT2CR.bit.ENABLE = 1;        //Enable Xint1

			//save preset
			PieCtrlRegs.PIEIER12.bit.INTx1 = 1;          //Enable PIE Gropu 1 INT4
			GpioCtrlRegs.GPBDIR.bit.GPIO48 = 0;			//As input
			GpioIntRegs.GPIOXINT3SEL.bit.GPIOSEL = 16;	//GPIO 5 as interrupt
			GpioCtrlRegs.GPBQSEL2.bit.GPIO48 = 2;		//XINT1 Qual using 6 samples
			GpioCtrlRegs.GPBCTRL.bit.QUALPRD2 = 0xFF;   //Each sampling window is 510*SYSCLKOUT
			XIntruptRegs.XINT3CR.bit.POLARITY = 1;      //Rising edge interrupt
			XIntruptRegs.XINT3CR.bit.ENABLE = 1;        //Enable Xint1

			//load Preset
			PieCtrlRegs.PIEIER12.bit.INTx2 = 1;          //Enable PIE Gropu 1 INT4
			GpioCtrlRegs.GPBDIR.bit.GPIO49 = 0;			//As input
			GpioIntRegs.GPIOXINT4SEL.bit.GPIOSEL = 17;	//GPIO 5 as interrupt
			GpioCtrlRegs.GPBQSEL2.bit.GPIO49 = 2;		//XINT1 Qual using 6 samples
			GpioCtrlRegs.GPBCTRL.bit.QUALPRD2 = 0xFF;   //Each sampling window is 510*SYSCLKOUT
			XIntruptRegs.XINT4CR.bit.POLARITY = 1;      //Rising edge interrupt
			XIntruptRegs.XINT4CR.bit.ENABLE = 1;        //Enable Xint1

			//Effects
			PieCtrlRegs.PIEIER12.bit.INTx3 = 1;          //Enable PIE Gropu 1 INT4
			GpioCtrlRegs.GPBDIR.bit.GPIO36 = 0;			//As input
			GpioIntRegs.GPIOXINT5SEL.bit.GPIOSEL = 4;	//GPIO 5 as interrupt
			GpioCtrlRegs.GPBQSEL1.bit.GPIO36 = 2;		//XINT1 Qual using 6 samples
			GpioCtrlRegs.GPBCTRL.bit.QUALPRD0 = 0xFF;   //Each sampling window is 510*SYSCLKOUT
			XIntruptRegs.XINT5CR.bit.POLARITY = 1;      //Rising edge interrupt
			XIntruptRegs.XINT5CR.bit.ENABLE = 1;        //Enable Xint1

	//ADC
		//PieVectTable.ADCINT = &adc_isr;
		//PieCtrlRegs.PIEIER1.bit.INTx6 = 1;

		AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;  // Enable SEQ1 interrupt (every EOS)
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

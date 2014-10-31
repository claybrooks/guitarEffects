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
	InitCpuTimers();
	//InitPieVectTable();
	EALLOW;
	//Timer0
			ConfigCpuTimer(&CpuTimer0, 150, 22.4);
			PieVectTable.TINT0 = &cpu_timer0_isr;
			CpuTimer0Regs.TCR.all = 0x4001;
			PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
	//Timer1
		ConfigCpuTimer(&CpuTimer1, 150, 1500000);
		PieVectTable.XINT13 = &cpu_timer1_isr;
		//CpuTimer1Regs.TCR.all = 0x4001;
		PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
	//XINT1
		//Mode toggle interrupt on GPIO 25
		PieVectTable.XINT1 = &xint1_isr;
		PieCtrlRegs.PIECTRL.bit.ENPIE = 1;          // Enable the PIE block
		PieCtrlRegs.PIEIER1.bit.INTx4 = 1;          // Enable PIE Gropu 1 INT4

		GpioCtrlRegs.GPADIR.bit.GPIO25 = 1;

		GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 25;	//GPIO 25 as interrupt
		GpioCtrlRegs.GPAQSEL2.bit.GPIO25 = 0;		//Sync to SYSCLKOUT
		GpioCtrlRegs.GPACTRL.bit.QUALPRD0 = 0xFF;   // Each sampling window is 510*SYSCLKOUT

		XIntruptRegs.XINT1CR.bit.POLARITY = 1;      // Rising edge interrupt
		XIntruptRegs.XINT1CR.bit.ENABLE = 1;        // Enable Xint1
	//ADC
		PieVectTable.ADCINT = &adc_isr;
		PieCtrlRegs.PIEIER1.bit.INTx6 = 1;
		AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;  // Enable SEQ1 interrupt (every EOS)
	//I2C
	//	PieVectTable.I2CINT1A = &i2c_int1a_isr;
		//sPieCtrlRegs.PIEIER8.bit.INTx1 = 1;

	IER |= M_INT1;
	IER |= M_INT7;
	IER |= M_INT8;
	IER |= M_INT13;

	EINT;   // Enable Global interrupt INTM
	ERTM;   // Enable Global realtime interrupt DBGM

}



void updateTimer0(float time){
	ConfigCpuTimer(&CpuTimer0, 150, time);
	CpuTimer0Regs.TCR.bit.TSS = 0;      // 1 = Stop timer, 0 = Start/Restart Timer
}

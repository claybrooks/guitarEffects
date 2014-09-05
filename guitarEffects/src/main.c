#include "DSP28x_Project.h"    				// Device Headerfile and Examples Include File
#include "LCD.h"
#include "SPI.h"
#include "SYS.h"
#include "EFFECT.h"
#include "ADC.h"

//#include "../src/FFT.h"

int updateLCD = -1;

int main(){
		InitSysCtrl();
//Initialize Interrupts
		initINTS();
//Initialize Effects
		GpioCtrlRegs.GPADIR.bit.GPIO8 = 0;
		GpioCtrlRegs.GPADIR.bit.GPIO9 = 0;
		GpioCtrlRegs.GPADIR.bit.GPIO10 = 0;
		initWAH(500,4);
//Initialize FFT
		//initFFT();
//Initialize LCD
		initLCD();
//Initialize SPI
		initSPI();
//Initialize ADC
		initADC();
//Initialize Timer 0
		initTIMER0();
//Initialize External Interrupt 1
		//initXINT1()

	EALLOW;
	while(1){	// endless loop - wait for an interrupt
		if(updateLCD != (GpioDataRegs.GPADAT.bit.GPIO10 + GpioDataRegs.GPADAT.bit.GPIO9 + GpioDataRegs.GPADAT.bit.GPIO8)){
			updateLCD = GpioDataRegs.GPADAT.bit.GPIO10 + GpioDataRegs.GPADAT.bit.GPIO9 + GpioDataRegs.GPADAT.bit.GPIO8;
			printEFFECT();
		}
	}
}

void cpu_timer0_isr(void){

	printSPI(AdcRegs.ADCRESULT0 >> 4);
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

/*void xint1_isr(void){
	if(GpioDataRegs.GPADAT.bit.GPIO11){  //Rising Edge interrupt, switching to tuner
		updateTIMER0(1000.0f);
		XIntruptRegs.XINT1CR.bit.POLARITY = 0;      // Falling edge interrupt
		updateLCD = -1;
	}
	else{	//Falling Edge interrupt, switching to effects
		updateTIMER0(20.833f);
		XIntruptRegs.XINT1CR.bit.POLARITY = 1;      // Rising edge interrupt
	}
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}*/

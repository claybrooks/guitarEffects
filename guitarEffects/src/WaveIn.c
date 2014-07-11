#include "DSP28x_Project.h"    				// Device Headerfile and Examples Include File
#include "../src/LCD.h"
#include "../src/SPI.h"
#include "../src/SYS.h"
#include "../src/EFFECT.h"
#include "../src/ADC.h"
#include "../src/FFT.h"

int updateLCD = -1;

int main(){
	InitSysCtrl();
	initINTS();
	initEFFECTS();
	initFFT();
	initLCD();
	initSPI();
	initADC();
	initTIMER0();
	initXINT1();
	EALLOW;
	while(1){	// endless loop - wait for an interrupt
		if(FFTStartFlag){
				printFREQ(findFREQ());	//If FFT ready, will only be set if mode is set correctly taken care of in timer0
		}
		else if(!GpioDataRegs.GPADAT.bit.GPIO11){	//EFFECT
			if(updateLCD != (GpioDataRegs.GPADAT.bit.GPIO10 + GpioDataRegs.GPADAT.bit.GPIO9 + GpioDataRegs.GPADAT.bit.GPIO8)){
				updateLCD = GpioDataRegs.GPADAT.bit.GPIO10 + GpioDataRegs.GPADAT.bit.GPIO9 + GpioDataRegs.GPADAT.bit.GPIO8;
				printEFFECT();
			}
		}
	}
}

void cpu_timer0_isr(void){
	if(!GpioDataRegs.GPADAT.bit.GPIO11){
		setARRAY(AdcRegs.ADCRESULT0);
		printSPI();
	}
	if(GpioDataRegs.GPADAT.bit.GPIO11){
		storeFFT();
	}
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

void xint1_isr(void){
	if(GpioDataRegs.GPADAT.bit.GPIO11){  //Rising Edge interrupt, switching to tuner
		updateTIMER0(1000.0f);
		initFFT();
		XIntruptRegs.XINT1CR.bit.POLARITY = 0;      // Falling edge interrupt
		updateLCD = -1;
	}
	else{	//Falling Edge interrupt, switching to effects
		updateTIMER0(20.833f);
		XIntruptRegs.XINT1CR.bit.POLARITY = 1;      // Rising edge interrupt
	}
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

#include "DSP28x_Project.h"    				// Device Headerfile and Examples Include File
#include "lcd.h"
#include "spi.h"
#include "sys.h"
#include "effect.h"
#include "adc.h"

//#include "../src/FFT.h"
void INITIALIZE(){
		InitSysCtrl();
	//Initialize Interrupts
		initINTS();
	//Initialize Effects
		//GpioCtrlRegs.GPADIR.bit.GPIO8 = 0;
		//GpioCtrlRegs.GPADIR.bit.GPIO9 = 0;
		//GpioCtrlRegs.GPADIR.bit.GPIO10 = 0;
		//initWAH(500,4);
	//Initialize FFT
		//initFFT();
	//Initialize LCD
		//initLCD();
	//Initialize SPI
		initSPI();
	//Initialize ADC
		initADC();
	//Initialize Timer 0
		initTIMER0();
	//Initialize External Interrupt 1
		//initXINT1()
		asm(" CLRC INTM, DBGM");

	EALLOW;
}

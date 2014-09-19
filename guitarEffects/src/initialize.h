#include "lcd.h"
#include "spi.h"
#include "sys.h"
#include "adc.h"
#include "effect.h"

//#include "../src/FFT.h"
void INITIALIZE(){
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
		//initXINT1();
		asm(" CLRC INTM, DBGM");
}

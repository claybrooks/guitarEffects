#include "initialize.h"
#include "lcd.h"
#include "spi.h"
#include "sys.h"
#include "adc.h"
#include "effect.h"

void initialize(){
	//Initialize Interrupts
	initINTS();

	//Initialize Effects
	initEffects();

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
	initXINT1();
	asm(" CLRC INTM, DBGM");
}

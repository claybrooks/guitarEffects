#include "initialize.h"
#include "lcd.h"
#include "spi.h"
#include "sys.h"
#include "adc.h"
#include "fft.h"
#include "effect.h"

void initialize(){
	//Initialize Effects
	initEffects();

	//Initialize FFT
	initFFT();

	//Initialize LCD
	initLCD();

	//Initialize SPI
	initSPI();

	//Initialize ADC
	initADC();

	//Initialize Interrupts
	initINTS();

	asm(" CLRC INTM, DBGM");
}

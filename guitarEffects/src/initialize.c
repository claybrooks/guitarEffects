#include "../include/initialize.h"
#include "../include/lcd.h"
#include "../include/spi.h"
#include "../include/sys.h"
#include "../include/adc.h"
//#include "../include/fft.h"
//#include "../include/effect.h"

void initialize(){
	//Initialize Effects
	//initEffects();

	//Initialize FFT
	//initFFT();

	//Initialize LCD
	//initLCD();

	//Initialize SPI
	//initSPI();

	//Initialize ADC
	//initADC();


	//Initialize Interrupts
	//initINTS();

	asm(" CLRC INTM, DBGM");
}

#include "initialize.h"
#include "spi.h"
#include "handler.h"
#include "lcd.h"
#include "adc.h"
#include "spi.h"
#include "effect.h"
#include "fft.h"
#include "DSP28x_Project.h"

int screen, input = 0, sample, tuner;

int main(){
	InitSysCtrl();
	initialize();
	screen = -1;
	while(1){
		if(screen > -1){
			updateLCD(screen);
			screen = -1;
		}
	}
}

/*interrupt void spi_isr(void){
	if(!tuner){
		sample = getAdc();
		writeSPI(process(sample));
		PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
	}
	else{
		storeFFT(getAdc());
	}
}*/

interrupt void cpu_timer0_isr(void){
	if(!tuner){
		sample = getAdc();
		writeSPI(process(sample));
		PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
	}
	else{
		storeFFT(getAdc());
	}
}

interrupt void xint1_isr(void){
	input = (GpioDataRegs.GPADAT.all & 0x0000FF00) >> 8;
	if(input!= 0){
		screen = handle(input);
		if(screen == TUNER)tuner = 1;
		else tuner = 0;
	}
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

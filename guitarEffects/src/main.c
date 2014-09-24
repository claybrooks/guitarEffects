#include "initialize.h"
#include "spi.h"
#include "handler.h"
#include "lcd.h"
#include "adc.h"
#include "spi.h"
#include "effect.h"
#include "fft.h"
#include "DSP28x_Project.h"

int screen = 0, screenUpdate = 0, input = 0, sample = 0, tuner = 0;

int main(){
	InitSysCtrl();
	initialize();
	while(1){
		if(screenUpdate){
			updateLCD(screen);
			screenUpdate = 0;
		}
	}
}

/*interrupt void spi_isr(void){
	if(tuner){
		if(storeFFT(getAdc())) findFrequency();
	}
	else{
		sample = getAdc();
		writeSPI(process(sample));
		PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
	}
}*/

interrupt void cpu_timer0_isr(void){
	if(tuner){
		if(storeFFT(getAdc())){
			screen = findFrequency();	//Here, screen represents frequency
			screenUpdate = 1;			//Tell main to updaet screen
		}
	}
	else{
		process(sample);
		//sample = getAdc();
		//writeSPI(process(sample));
		PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
	}
}

interrupt void xint1_isr(void){
	input = (GpioDataRegs.GPADAT.all & 0x0000FF00) >> 8;
	if(input!= 0){
		int returnVal = handle(input);
		if(returnVal == TUNER){
			tuner ^= 1;			//Screen update needs to wait for FFT calculation
		}
		else{
			screen = returnVal;
			screenUpdate = 1;	//Update screen Immediately
		}
	}
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

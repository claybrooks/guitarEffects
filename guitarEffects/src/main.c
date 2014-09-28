#include "initialize.h"
#include "spi.h"
#include "lcd.h"
#include "adc.h"
#include "spi.h"
#include "effect.h"
#include "fft.h"
#include "sys.h"
#include "DSP28x_Project.h"

int indexLookup(int);

int screen = 0, screenUpdate = 0, input = 0, sample = 0, tuner = 0;

int main(){
	InitSysCtrl();
	initialize();
	GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO14 = 1;
	while(1){
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
	GpioDataRegs.GPADAT.bit.GPIO14 ^= GpioDataRegs.GPADAT.bit.GPIO14;
	if(tuner){
		if(storeFFT(getAdc())){
			printFreq(findFrequency());	//Here, screen represents frequency
		}
	}
	else{
		//process(sample);
		sample = getAdc();
		writeSPI(process(sample));
		PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
	}

}

interrupt void xint1_isr(void){
	DELAY_US(50000);
	input = (GpioDataRegs.GPADAT.all & 0x0000F00) >> 8;
	if(input!= 0){
		//Handle Input
			//Clear pipeline from last use
			if(input == 0x0008){
				clearPipeline();
				updateLCD(CLEAR);
			}

			//Switch to tuning function
			else if(input == 0x0004){
				tuner ^= 1;			//Screen update needs to wait for FFT calculation
				updateLCD(TUNER);
				if(tuner){
					updateTimer0(1000);
				}
				else{
					updateTimer0(22.675f);
				}
			}
			//Look to either queue effect or toggle state
			else{
				/*Simple lookup vs mathematical computation
				 * Gets the effect to be manipulated
				*/
				int effect = indexLookup(input);
				//toggleOn_Off return 1 if it can be toggled, else 0 meaning its not in queue;
				if(!toggleOn_Off(effect)) queueEffect(effect);		//queue the effect
				updateLCD(effect);
			}
	}
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

int indexLookup(int input){
	if(input == 1) return 0;
	else if(input == 2) return 1;
	else if(input == 4) return 2;
	else if(input == 8) return 3;
	else if(input == 16) return 4;
	else if(input == 32) return 5;
	else if(input == 64) return 6;
	else if(input == 128) return 7;
	else return 8;
}

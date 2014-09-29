#include "../include/initialize.h"
#include "../include/spi.h"
#include "../include/lcd.h"
#include "../include/adc.h"
#include "../include/spi.h"
#include "../include/effect.h"
#include "../include/fft.h"
#include "../include/sys.h"
#include "DSP28x_Project.h"

int indexLookup(int);
int tuner = 0;

int main(){
	InitSysCtrl();
	initialize();
	while(1){
		//Wait for interrupts
	}
}

/*interrupt void spi_isr(void){
	//Tuner      //Will return true if frequency is ready to find
	if(tuner) if(storeFFT(getAdc())) printFreq(findFrequency());

	//Play through amp
	else writeSPI(process(getAdc()));

	//Acknowledge Interrupt
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}*/

interrupt void cpu_timer0_isr(void){
	//Tuner      //Will return true if frequency is ready to find
	if(tuner){
		if(storeFFT(getAdc())) printFreq(findFrequency());
	}

	//Play through amp
	else writeSPI(process(getAdc()));

	//Acknowledge Interrupt
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

}

interrupt void xint1_isr(void){
	//Get user input, shift for clearer comparisons
	int input = (GpioDataRegs.GPADAT.all & 0x000FF00) >> 8;

	//Takes care of weird calls to xint1_isr with no button push
	if(input!= 0){
		//Clear pipeline of all effects/ clear screen
		if(input == 0x0080){
			clearPipeline();
			updateLCD(CLEAR);
		}

		//Switch to tuning function
		else if(input == 0x0040){
			tuner ^= 1;			//signal for timer0 to not sample out to SPI
			updateLCD(TUNER);	//Update LCD to tuner screen
			if(tuner) updateTimer0(1000);	//Slower sample rate for FFT analysis = Higher bin resolution
			else updateTimer0(22.675f);		//FFT was toggled off, switch back to sample out to SPI
		}

		//Look to either queue effect or toggle state
		else{
			//Simple lookup vs mathematical computation gets the effect to be manipulated
			int effect = indexLookup(input);
			//toggleOn_Off returns 1 if it can be toggled, else 0 meaning its not in queue;
			if(!toggleOn_Off(effect)) queueEffect(effect);		//queue the effect
			updateLCD(effect);
		}
	}
	//Acknowledge Interrupt
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

//Eliminates need to call log(input)/log(2).  Simple lookup will be faster
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

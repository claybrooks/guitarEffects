/*
 * WaveIn.c
 *
 *  Created on: Jul 6, 2014
 *      Author: Clay
*/
#include "DSP28x_Project.h"    				// Device Headerfile and Examples Include File
#include "math.h"
#include "LCD.h"
#include "ADC.h"
#include "SPI.h"
#include "SYS.h"

int aboveThresh = 0;
int belowThresh = 1;
int crossings = 0;

int main(){
	InitSysCtrl();
	initINTS();
	initLCD();
	initSPI();
	initTIMER0();
	initADC();


	while(1){

	};
}

void cpu_timer0_isr(){
	printFreq(crossings);
	crossings = 0;
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

void adc_isr(void){
	if((AdcRegs.ADCRESULT0 >> 4 > 0x7ff) && belowThresh){
		belowThresh = 0;
		aboveThresh = 1;
		crossings++;

	}
	else if((AdcRegs.ADCRESULT0 >> 4 < 0x7ff) && aboveThresh){
		belowThresh = 1;
		aboveThresh = 0;
		crossings++;
	}
	printSPI( (0x3FF0 & (AdcRegs.ADCRESULT0 >> 2)) << 2);
	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;         // Reset SEQ1
	AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;       // Clear INT SEQ1 bit
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // Acknowledge interrupt to PIE
}

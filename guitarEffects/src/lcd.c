/*
 * lcd.c
 *
 *  Created on: Sep 19, 2014
 *      Author: Clay
 */
#include "lcd.h"
#include "math.h"
#include "DSP28x_Project.h"

#define DELAYLCD 11000

int mainDisplay[10];
int mainDisplay_show[10];
int numInMainDisplay;

void updateLCD(int update){
	//Clear Screen, called by a reset on the queue
	if(update == CLEAR){

	}
	else if(update >= DELAY && update <= PITCHSHIFT){
			//Initialize Variables
		int i = 0, inMainDisplay;
		//Loop through mainDisplay to see if the effect is already set to print to LCD
		for(;i<numInMainDisplay;i++){
			//If it is in main display, break
			if(mainDisplay[i] == update){
				inMainDisplay = 1;
				break;
			}
		}
		//If not inMainDisplay, put in mainDisplay array in the correct location, turn on to print to screen
		//where update == delay through pitch shift values defined in lcd.h
		if(!inMainDisplay){
			mainDisplay[numInMainDisplay] = update;
			mainDisplay_show[numInMainDisplay] = 1;
			numInMainDisplay++;
		}
		else{
			//If its already in main display, its already in the queue, so toggle the effect on/off, toggle
			//the display on/off
			mainDisplay_show[i] ^= 1;
		}
	}

	else{
		printFreq(update);
	}
}

void wait(int temp){
	while(temp != 0) temp--;
}

void initLCD(){
	int i = 0;
	for(;i<10;i++){
		mainDisplay[i] = 0;
		mainDisplay_show[i] = 0;
	}
	EALLOW;
	GpioCtrlRegs.GPADIR.bit.GPIO0 = 0x1;
	GpioCtrlRegs.GPADIR.bit.GPIO1 = 0x1;
	GpioCtrlRegs.GPADIR.bit.GPIO2 = 0x1;
	GpioCtrlRegs.GPADIR.bit.GPIO3 = 0x1;
	GpioCtrlRegs.GPADIR.bit.GPIO4 = 0x1;
	GpioCtrlRegs.GPADIR.bit.GPIO5 = 0x1;
	GpioCtrlRegs.GPADIR.bit.GPIO6 = 0x1;
	GpioCtrlRegs.GPADIR.bit.GPIO7 = 0x1;
	GpioCtrlRegs.GPBDIR.bit.GPIO48 = 0x1;
	GpioCtrlRegs.GPBDIR.bit.GPIO49 = 0x1;
	//E = 48
	//RS = 49
	//8 Bit Mode
	//Write 3F for 1st function set
	controlLCD(0x3F);
	//Write 3F for 2nd function set
	controlLCD(0x3F);
	//Write 0F, D = 1 for display on, C = 1 for cursor on, B = 1 for blinking on
	controlLCD(0x0F);
	//Write 01 to clear display
	controlLCD(0x01);
	//Entry mode set for inc and no shift
	controlLCD(0x06);
}

void controlLCD(int data){
	GpioDataRegs.GPBDAT.bit.GPIO49 = 0;
	GpioDataRegs.GPADAT.all = data;
	wait(DELAYLCD);
	GpioDataRegs.GPBDAT.bit.GPIO48 = 1;
	wait(DELAYLCD);
	GpioDataRegs.GPBDAT.bit.GPIO48 = 0;
	wait(DELAYLCD);

}

void printLCD(int data){
	GpioDataRegs.GPBDAT.bit.GPIO49 = 1;
	GpioDataRegs.GPADAT.all = data;
	wait(DELAYLCD);
	GpioDataRegs.GPBDAT.bit.GPIO48 = 1;
	wait(DELAYLCD);
	GpioDataRegs.GPBDAT.bit.GPIO48 = 0;
	wait(DELAYLCD);
}

void printFreq(int data){
	controlLCD(0x01);

	unsigned int* array = (unsigned int*)0xA000;
	int counter = 0;
	while(data >= 1){
		counter += 1;
		*array = fmod(data, 10);
		data  = data / 10;
		array += 1;
	}
	array -= 1;
	for(; counter > 0; counter--, array--){
		printLCD(*array+0x30);
	}
}

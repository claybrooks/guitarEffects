/*
 * lcd.c
 *
 *  Created on: Sep 19, 2014
 *      Author: Clay
 */
#include "../include/lcd.h"
#include "math.h"
#include "DSP28x_Project.h"

#define DELAYLCD 15000

//Similar to queue system.  mainDisplay contains what could possibly be printed to screen.
//mainDisplay_show holds whether or not it is printed to screen (on_off[]);
int mainDisplay[10];
int mainDisplay_show[10];
int numInMainDisplay, tunerScreen;

//Eventually parses code and tells the printLCD() what to print
void updateLCD(int update){
	//Clear Screen, called by a reset on the queue. Block input if on tuner screen
	if(update == CLEAR && !tunerScreen){
		numInMainDisplay = 0;
		controlLCD(0x01);
	}
	//Block input if on tuner scree
	else if((update >= TREMOLO && update <= FLANGE) && !tunerScreen){
			//Initialize Variables
		int i = 0, inMainDisplay = 0;
		//Loop through mainDisplay to see if the effect is already set to print to LCD
		for(;i<numInMainDisplay;i++){
			//If it is in main display, break
			if(mainDisplay[i] == update){
				inMainDisplay = 1;
				break;
			}
		}
		//If not inMainDisplay, put in mainDisplay array in the correct location, turn on to print to screen
		//where update == tremolo through pitch shift values defined in lcd.h
		if(!inMainDisplay){
			mainDisplay[numInMainDisplay] = update;
			mainDisplay_show[numInMainDisplay] = 1;
			numInMainDisplay++;

			//Send toLCD(effect to be printed)
			addToLCD(update);
			printLCD(0x20);
		}
		else{
			//If its already in main display, its already in the queue, so toggle the effect on/off, toggle
			//the display on/off
			mainDisplay_show[i] ^= 1;

			toggleLCD(update, i, mainDisplay_show[i]);
		}
	}
	else if(update == TUNER){
		if(tunerScreen){
			tunerScreen = 0;
			controlLCD(0x01);
			int i;
			for(i = 0; i < numInMainDisplay; i++){
				if(mainDisplay_show[i])addToLCD(mainDisplay[i]);
				else{
					printLCD(0x20);
					printLCD(0x20);
				}
				printLCD(0x20);
			}
		}
		else{
			tunerScreen = 1;
			controlLCD(0x01);
			printLCD(0x54);	//T
			printLCD(0x55);	//U
			printLCD(0x4E);	//N
			printLCD(0x45);	//E
			printLCD(0x52);	//R
			printLCD(0x3A);	//:
		}
	}
	else if(tunerScreen == 1){
		printFreq(update);
	}
}


void addToLCD(int effect){
	if(effect == TREMOLO){
		printLCD(0x54);
		printLCD(0x52);
	}
	else if(effect == DISTORTION){
		printLCD(0x44);
		printLCD(0x49);
	}
	else if(effect == CRUNCH){
		printLCD(0x43);
		printLCD(0x52);
	}
	else if(effect == DELAY){
		printLCD(0x44);
		printLCD(0x45);
	}
	else if(effect == WAH){
		printLCD(0x57);
		printLCD(0x41);
	}
	else if(effect == PHASER){
		printLCD(0x50);
		printLCD(0x48);
	}
	/*else if(effect == FLANGE){
		printLCD(0x46);
		printLCD(0x4B);
	}
	else if(effect == REVERB){
		printLCD(0x52);
		printLCD(0x45);
	}
	else if(effect == CHORUS){
		printLCD(0x43);
		printLCD(0x48);
	}
	else if(effect == PITCHSHIFT){
		printLCD(0x50);
		printLCD(0x53);
	}*/
}

void toggleLCD(int effect, int index, int on){
	controlLCD(0x02); //Return cursor to home;

	//Shift to beggining of section that needs to be changed
	int i;
	for(i = 0;i<index*3;i++){
		controlLCD(0x14);
	}

	//Went from off to on, reprint effect in proper spot.  Fill in proper vales
	if(on){
		addToLCD(effect);
	}

	//Went from on to off, remove effect from spot
	else{
		printLCD(0x20);
		printLCD(0x20);
	}

	controlLCD(0x02); //Return cursor to home;
	for(i = 0;i<numInMainDisplay*3;i++){
			controlLCD(0x14);
	}
}


void printFreq(int data){
	controlLCD(0xB8); //Second line;
	int i;
	for(i = 0; i < 3; i++){
		printLCD(0x20);	//clear previous frequency. theres gotta be a better way
	}
	controlLCD(0xB8); //Second line;

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

	controlLCD(0x02); //Return cursor to home;
}




void wait(int temp){
	while(temp != 0) temp--;
}

void initLCD(){
	int i = 0;
	tunerScreen = 0;
	for(;i<10;i++){
		mainDisplay[i] = 0;
		mainDisplay_show[i] = 0;
	}
	numInMainDisplay = 0;
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
	controlLCD(0x38);
	//Write 3F for 2nd function set
	controlLCD(0x38);
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

/*
 * lcd.c
 *
 *  Created on: Sep 19, 2014
 *      Author: Clay
 */
#include "lcd.h"
#include "lcdCodes.h"
#include "effect.h"
#include "math.h"
#include "DSP28x_Project.h"
#include "F28335_example.h"
#define DELAYLCD 10000

//Similar to queue system.  mainDisplay contains what could possibly be printed to screen.
//mainDisplay_show holds whether or not it is printed to screen (on_off[]);
int numInMainDisplay, tunerScreen, currentPreset, presetScreen, reprint;


#pragma CODE_SECTION(controlLCD, "secureRamFuncs")
#pragma CODE_SECTION(printLCD, "secureRamFuncs")
#pragma CODE_SECTION(updateLCD, "secureRamFuncs")
#pragma CODE_SECTION(printTremolo, "secureRamFuncs")
#pragma CODE_SECTION(printReverb, "secureRamFuncs")
#pragma CODE_SECTION(updateLevel, "secureRamFuncs")
#pragma CODE_SECTION(goToMain, "secureRamFuncs")
#pragma CODE_SECTION(addToLCD, "secureRamFuncs")
#pragma CODE_SECTION(shiftCursor, "secureRamFuncs")
#pragma CODE_SECTION(toggleLCD, "secureRamFuncs")
#pragma CODE_SECTION(printFreq, "secureRamFuncs")

void printTremolo(){
	controlLCD(CL);
	controlLCD(SECOND);
	printLCD(T);
	printLCD(R+lc);
	printLCD(E+lc);
	printLCD(M+lc);
	printLCD(O+lc);
	printLCD(L+lc);
	printLCD(O+lc);
	controlLCD(HOME); //Return cursor to home;
}
void printReverb(){
	controlLCD(CL);
	controlLCD(SECOND);
	printLCD(R);
	printLCD(E+lc);
	printLCD(V+lc);
	printLCD(E+lc);
	printLCD(R+lc);
	printLCD(B+lc);
	controlLCD(HOME); //Return cursor to home;
}

void printDelay(){
	controlLCD(CL);
	controlLCD(SECOND);
	printLCD(D);
	printLCD(E+lc);
	printLCD(L+lc);
	printLCD(A+lc);
	printLCD(Y+lc);
	controlLCD(HOME); //Return cursor to home;
}
void printFlange(){
	controlLCD(CL);
	controlLCD(SECOND);
	printLCD(F);
	printLCD(L+lc);
	printLCD(A+lc);
	printLCD(N+lc);
	printLCD(G+lc);
	printLCD(E+lc);
	controlLCD(HOME); //Return cursor to home;
}
void printChorus(){
	controlLCD(CL);
	controlLCD(SECOND);
	printLCD(C);
	printLCD(H+lc);
	printLCD(O+lc);
	printLCD(R+lc);
	printLCD(U+lc);
	printLCD(S+lc);
	controlLCD(HOME); //Return cursor to home;
}
void updateLevel(int newLevel, int oldLevel){
	int i = 0;
	//decreasing
	if(oldLevel >= newLevel){
		//Shift cursor to level, print spaces to oldLevel
		if(reprint){
			controlLCD(HOME);
			for(i = 0; i < oldLevel; i++)printLCD(BAR);
			reprint = 0;
		}
		int cursorShift = 0x80 | (newLevel);
		controlLCD(cursorShift);
		for(i = newLevel; i <= oldLevel; i++){
			printLCD(SPACE);
		}
	}
	//increasing
	else{
		if(reprint){
			oldLevel = 0;
			reprint = 0;
		}
		//Shift cursor to oldlevel, print bars to level
		int cursorShift = 0x80 | (oldLevel);
		controlLCD(cursorShift);
		for(i = oldLevel; i < newLevel; i++){
			printLCD(BAR);
		}
	}
}

//Eventually parses code and tells the printLCD() what to print
void updateLCD(int* update, int* mainDisplay, int* on_off, int* currentPreset, int* numQueued){
	//Clear Screen, called by a reset on the queue. Block input if on tuner screen or preset screen
	if(*update == CLEAR && !tunerScreen && !presetScreen){
		*numQueued = 0;
		controlLCD(CL);
	}
	//Block input if tunerScreen
	else if(*update == PRESETTIMEOUT || *update == MAIN){
		presetScreen = 0;
		goToMain(mainDisplay, on_off, numQueued);

	}
	else if(*update == TUNER){
		if(tunerScreen){
			tunerScreen = 0;
			goToMain(mainDisplay, on_off, numQueued);
		}
		else{
			tunerScreen = 1;
			controlLCD(CL);
			printLCD(T);
			printLCD(U);
			printLCD(N);
			printLCD(E);
			printLCD(R);
			printLCD(COLON);
		}
	}
	else if(tunerScreen == 1){
		printFreq(*update);
	}
	else if(*update == PRESETUP){
		if(presetScreen){
			controlLCD(HOME); //Return cursor to home;
			//Shift Cursor over to correct position
			shiftCursor(8);
			printLCD(*currentPreset+0x30);
		}
		else{
			//Clear Screen
			controlLCD(CL);
			controlLCD(HOME); //Return cursor to home;
			presetScreen = 1;
			printLCD(P);	//P
			printLCD(R);	//R
			printLCD(E);	//E
			printLCD(S);	//S
			printLCD(E);	//E
			printLCD(T);	//T
			printLCD(COLON);	//:
			printLCD(SPACE); //_
			printLCD(*currentPreset + 0x30);
		}
	}
	else if(*update == PRESETDOWN){
		if(presetScreen){
			controlLCD(HOME); //Return cursor to home;
			//Shift Cursor over to correct position
			shiftCursor(8);
			printLCD(*currentPreset+0x30);
		}
		else{
			//Clear Screen
			controlLCD(CL);
			controlLCD(HOME); //Return cursor to home;
			presetScreen = 1;
			printLCD(P);	//P
			printLCD(R);	//R
			printLCD(E);	//E
			printLCD(S);	//S
			printLCD(E);	//E
			printLCD(T);	//T
			printLCD(COLON);	//:
			printLCD(SPACE); //_
			printLCD(*currentPreset + 0x30);
		}
	}
	else if(*update == SAVEPRESET && presetScreen){
		//Save presets.  Method of calling needs to be moved to main
		presetScreen = 0;
		goToMain(mainDisplay, on_off, numQueued);
	}
	else if(*update == LOADPRESET && presetScreen){
		//Load presets.  Method of calling needs to be moved to main
		presetScreen = 0;
		goToMain(mainDisplay, on_off, numQueued);
	}
	else if(*update == CHANGETREMOLO){
		printTremolo();
		reprint = 1;
	}
	else if(*update == CHANGEREVERB){
		printReverb();
		reprint = 1;
	}
	else if(*update == CHANGECHORUS){
		printChorus();
	}
	else if(*update == CHANGEDELAY){
		printDelay();
	}
	else if(*update == CHANGEFLANGE){
		printFlange();
	}
}

void loadPresetScreen(int* location, int* mainDisplay, int* numQueued){
	//0-10 is location
	int i;
	/*Must loop over entire location array.  If the index value is not -1,
	you know the effect is queued.  Here, the index 'i' corresponds to the queued
	effect, defined in the #define section of lcd.h.  The value of location[i] in
	are location values, '1,2,3' of the effect in the queue.  This corresponds to the
	location in the mainDisplay array.  The on_off array(array offset by 10) is then used to determine
	whether or not the queued effect is currently turned on.*/

	for(i = 0; i < *numQueued; i++){
			mainDisplay[i] = location[i];
	}
}

void goToMain(int* mainDisplay, int* on_off, int* numQueued){
	//Reprint main screen.  Usually called after a load/save preset or presettimeout or tuner screen toggle
	controlLCD(CL);
	controlLCD(HOME);
	int i;
	for(i = 0; i < *numQueued; i++){
		if(i == 3) controlLCD(0xB8); //Second line;
		if(on_off[i]){
			printLCD(i+1 + 0x30);
			printLCD(0x3A);
			addToLCD(mainDisplay[i]);
		}
		else{
			printLCD(SPACE);
			printLCD(SPACE);
			printLCD(SPACE);
			printLCD(SPACE);
		}
		printLCD(SPACE);
	}
}

void shiftCursor(int shift){
	int i;
	for(i = 0;i < shift;i++){
		controlLCD(SHIFTR);	//Shift to the right
	}
}

void addToLCD(int effect){
	if(effect == TREMOLO){
		printLCD(T);
		printLCD(R);
	}
	else if(effect == REVERB){
			printLCD(R);
			printLCD(E);
		}
	else if(effect == DISTORTION){
		printLCD(D);
		printLCD(I);
	}
	else if(effect == CRUNCH){
		printLCD(C);
		printLCD(R);
	}
	else if(effect == DELAY){
		printLCD(D);
		printLCD(E);
	}
	else if(effect == WAH){
		printLCD(W);
		printLCD(A);
	}

	else if(effect == SUSTAIN){
			printLCD(S);
			printLCD(U);
	}
	else if(effect == FLANGE){
	 	printLCD(F);
		printLCD(L);
	}
	else if(effect == PHASER){
		printLCD(P);
		printLCD(H);
	}
	else if(effect == CHORUS){
		printLCD(C);
		printLCD(H);
	}
	else if(effect == PITCHSHIFT){
		printLCD(P);
		printLCD(I);
	}
}

void toggleLCD(int effect, int index, int on, int numQueued){
	//controlLCD(0x02); //Return cursor to home;
	//Shift to beggining of section that needs to be changed

	int i;
	int cursorShift = 0x80 | (index*5);
	controlLCD(cursorShift);


	//Went from off to on, reprint effect in proper spot.  Fill in proper vales
	if(on){
		printLCD(index+1 + 0x30);
		printLCD(0x3A);
		addToLCD(effect);
	}

	//Went from on to off, remove effect from spot
	else{
		for(i = 0; i < 4; i++) printLCD(0x20);
	}
/*
	controlLCD(0x02); //Return cursor to home;
	for(i = 0;i<numQueued*5;i++){
			controlLCD(0x14);
	}
	*/
	cursorShift = 0x80 | (numQueued*5);
	controlLCD(cursorShift);

}


void printFreq(int data){

	controlLCD(SECOND); //Second line;
	int i;
	for(i = 0; i < 3; i++){
		printLCD(SPACE);	//clear previous frequency. theres gotta be a better way
	}
	controlLCD(SECOND); //Second line;

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

	controlLCD(HOME); //Return cursor to home;

}




void wait(int temp){
	while(temp != 0) temp--;
}

void initLCD(){
	tunerScreen = 0;
	currentPreset = 1;
	presetScreen = 0;
	EALLOW;
	GpioCtrlRegs.GPADIR.bit.GPIO8 = 0x1;
	GpioCtrlRegs.GPADIR.bit.GPIO9 = 0x1;
	GpioCtrlRegs.GPADIR.bit.GPIO10 = 0x1;
	GpioCtrlRegs.GPADIR.bit.GPIO11 = 0x1;
	GpioCtrlRegs.GPADIR.bit.GPIO12 = 0x1;
	GpioCtrlRegs.GPADIR.bit.GPIO13 = 0x1;
	GpioCtrlRegs.GPADIR.bit.GPIO14 = 0x1;
	GpioCtrlRegs.GPADIR.bit.GPIO15 = 0x1;
	GpioCtrlRegs.GPADIR.bit.GPIO25 = 0x1;
	GpioCtrlRegs.GPADIR.bit.GPIO21 = 0x1;
	//E = 48
	//RS = 49
	//8 Bit Mode
	//Write 38 for 1st function set
	controlLCD(0x38);
	//Write 38 for 2nd function set
	controlLCD(0x38);
	//Write 0C, D = 1 for display on, C = 0 for cursor off, B = 0 for blinking off
	controlLCD(0x0C);
	//Write 01 to clear display
	controlLCD(CL);
	//Entry mode set for inc and no shift
	controlLCD(0x06);
}

void controlLCD(int data){
	GpioDataRegs.GPADAT.bit.GPIO21 = 0;
	GpioDataRegs.GPADAT.all = data << 8;
	GpioDataRegs.GPADAT.bit.GPIO25 = 1;
	DelayUs(15);
	GpioDataRegs.GPADAT.bit.GPIO25 = 0;
	DelayUs(37);
	if(data == CL || data == HOME){
		DelayUs(1483);
	}
}

void printLCD(int data){
	GpioDataRegs.GPADAT.bit.GPIO21 = 1;
	GpioDataRegs.GPADAT.all = data << 8;
	GpioDataRegs.GPADAT.bit.GPIO25 = 1;
	DelayUs(15);
	GpioDataRegs.GPADAT.bit.GPIO25 = 0;
	DelayUs(48);
}

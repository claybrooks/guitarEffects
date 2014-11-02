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

#define DELAYLCD 10000

//Similar to queue system.  mainDisplay contains what could possibly be printed to screen.
//mainDisplay_show holds whether or not it is printed to screen (on_off[]);
int mainDisplay[10];
int mainDisplay_show[10];
int numInMainDisplay, tunerScreen, currentPreset, presetScreen;

//#pragma CODE_SECTION(controlLCD, "secureRamFuncs")
//#pragma CODE_SECTION(printLCD, "secureRamFuncs")

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
void updateLevel(int level, int oldLevel){
	//controlLCD(0x01);
	controlLCD(HOME);//return home
	int i;
	for(i = 0; i < level; i++){
		printLCD(BAR);
	}

	for(i = level; i < 16; i++){
		printLCD(SPACE);
	}

}

void toggleEffectDisplay(int effect){
	//Block input if tunerScreen

	//Initialize Variables
	int i = 0;//, inMainDisplay = 0;
	//Loop through mainDisplay to see if the effect is already set to print to LCD
	for(;i<numInMainDisplay;i++){
		//If it is in main display, break
		if(mainDisplay[i] == effect){
			//inMainDisplay = 1;
			break;
		}
	}
	//If not inMainDisplay, put in mainDisplay array in the correct location, turn on to print to screen
	//where update == tremolo through pitch shift values defined in lcd.h
	mainDisplay_show[i] ^= 1;
	toggleLCD(effect, i, mainDisplay_show[i]);


}

int toggleDisplayOn_Off(int effect){
	int i = 0, inMainDisplay = 0;
	//Loop through mainDisplay to see if the effect is already set to print to LCD
	for(;i<numInMainDisplay;i++){
		//If it is in main display, break
		if(mainDisplay[i] == effect){
			//inMainDisplay = 1;
			break;
		}
	}
	if(inMainDisplay){
		mainDisplay_show[i] ^= 1;
		return 1;
	}
	else return 0;
}

void queueDisplay(int effect){
	//If not inMainDisplay, put in mainDisplay array in the correct location, turn on to print to screen
	//where update == tremolo through pitch shift values defined in lcd.h
		mainDisplay[numInMainDisplay] = effect;
		mainDisplay_show[numInMainDisplay] = 0;
		numInMainDisplay++;
}


//Eventually parses code and tells the printLCD() what to print
void updateLCD(int update){
	//Clear Screen, called by a reset on the queue. Block input if on tuner screen or preset screen
	if(update == CLEAR && !tunerScreen && !presetScreen){
		numInMainDisplay = 0;
		controlLCD(CL);
	}
	//Block input if tunerScreen
		else if((update >= TREMOLO && update <= REVERB) && !tunerScreen && !presetScreen){
			//Initialize Variables
			int i = 0, inMainDisplay = 0;
			//Loop through mainDisplay to see if the effect is already set to print to LCD
			for(;i<numInMainDisplay;i++){
				//If it is in main display, break
				if(mainDisplay[i] == update){
					break;
				}
			}
			//If not inMainDisplay, put in mainDisplay array in the correct location, turn on to print to screen
			//where update == tremolo through pitch shift values defined in lcd.h


			//If its already in main display, its already in the queue, so toggle the effect on/off, toggle
			//the display on/off
			mainDisplay_show[i] ^= 1;
			toggleLCD(update, i, mainDisplay_show[i]);

		}
	else if(update == PRESETTIMEOUT){
		presetScreen = 0;
		goToMain();

	}
	else if(update == TUNER){
		if(tunerScreen){
			tunerScreen = 0;
			goToMain();
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
		printFreq(update);
	}
	else if(update == PRESETUP){
		if(presetScreen){
			controlLCD(HOME); //Return cursor to home;
			//Shift Cursor over to correct position
			shiftCursor(8);
			currentPreset++;
			if(currentPreset == 10)currentPreset = 1;
			printLCD(currentPreset+0x30);
		}
		else{
			//Clear Screen
			controlLCD(0x01);
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
			printLCD(currentPreset + 0x30);
		}
	}
	else if(update == PRESETDOWN){
		if(presetScreen){
			controlLCD(HOME); //Return cursor to home;
			//Shift Cursor over to correct position
			shiftCursor(8);
			currentPreset--;
			if(currentPreset == 0)currentPreset = 9;
			printLCD(currentPreset+0x30);
		}
		else{
			//Clear Screen
			controlLCD(CLEAR);
			presetScreen = 1;
			printLCD(P);	//P
			printLCD(R);	//R
			printLCD(E);	//E
			printLCD(S);	//S
			printLCD(E);	//E
			printLCD(T);	//T
			printLCD(COLON);	//:
			printLCD(SPACE); //_
			printLCD(currentPreset + 0x30);
		}
	}
	else if(update == SAVEPRESET && presetScreen){
		/*//Save presets.  Method of calling needs to be moved to main
		savePreset(currentPreset);
		int start = (currentPreset-1)*42;
		int i;
		for(i = 21; i < 31; i++){
			presets[start + i] = mainDisplay[i-21];
			presets[start + i + 10] = mainDisplay_show[i-21];
		}
		presets[start + i + 10] = numInMainDisplay;*/
		presetScreen = 0;
		goToMain();
	}
	else if(update == LOADPRESET && presetScreen){
		//Load presets.  Method of calling needs to be moved to main
		/*loadPreset(currentPreset);
		int start = (currentPreset-1)*42;
		int i;
		for(i = 21; i < 31; i++){
			mainDisplay[i-21] = presets[start + i];
			mainDisplay_show[i-21] = presets[start + i + 10];
		}
		numInMainDisplay = presets[start + i + 10];*/
		presetScreen = 0;
		goToMain();
	}
	else if(update == CHANGETREMOLO){
		printTremolo();
	}
	else if(update == CHANGEREVERB){
		printReverb();
	}
	else if(update == CHANGECHORUS){
		printChorus();
	}
	else if(update == CHANGEDELAY){
		printDelay();
	}
	else if(update == CHANGEFLANGE){
		printFlange();
	}
}

void loadPresetScreen(int* array){
	//0-10 is location
	int i;
	/*Must loop over entire location array.  If the index value is not -1,
	you know the effect is queued.  Here, the index 'i' corresponds to the queued
	effect, defined in the #define section of lcd.h.  The value of array[i] in
	are location values, '1,2,3' of the effect in the queue.  This corresponds to the
	location in the mainDisplay array.  The on_off array(array offset by 10) is then used to determine
	whether or not the queued effect is currently turned on.*/

	for(i = 0; i < 10; i++){
		if(array[i] != -1){
			mainDisplay[array[i]] = i;
			mainDisplay_show[array[i]] = array[i+10];
			numInMainDisplay++;
		}
	}
}

void goToMain(){
	//Reprint main screen.  Usually called after a load/save preset or presettimeout or tuner screen toggle
	controlLCD(0x01);
	int i;
	for(i = 0; i < numInMainDisplay; i++){
		if(i == 3) controlLCD(0xB8); //Second line;
		if(mainDisplay_show[i]){
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
	else if(effect == REVERB){
		printLCD(R);
		printLCD(E);
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

void toggleLCD(int effect, int index, int on){
	controlLCD(0x02); //Return cursor to home;
	//Shift to beggining of section that needs to be changed
	int i;
	for(i = 0;i < index*5;i++){
		controlLCD(0x14);
		if(i == 14)controlLCD(0xB8);//Move to second line
	}

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

	controlLCD(0x02); //Return cursor to home;
	for(i = 0;i<numInMainDisplay*5;i++){
			controlLCD(0x14);
	}
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
	int i = 0;
	tunerScreen = 0;
	currentPreset = 1;
	presetScreen = 0;
	for(;i<10;i++){
		mainDisplay[i] = -1;
		mainDisplay_show[i] = 0;
	}
	numInMainDisplay = 0;
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
	controlLCD(0x01);
	//Entry mode set for inc and no shift
	controlLCD(0x06);
}

void controlLCD(int data){
	GpioDataRegs.GPADAT.bit.GPIO21 = 0;
	GpioDataRegs.GPADAT.all = data << 8;
	GpioDataRegs.GPADAT.bit.GPIO25 = 1;
	wait(DELAYLCD);
	GpioDataRegs.GPADAT.bit.GPIO25 = 0;
	wait(DELAYLCD);

}

void printLCD(int data){
	GpioDataRegs.GPADAT.bit.GPIO21 = 1;
	GpioDataRegs.GPADAT.all = data << 8;
	GpioDataRegs.GPADAT.bit.GPIO25 = 1;
	wait(DELAYLCD);
	GpioDataRegs.GPADAT.bit.GPIO25 = 0;
	wait(DELAYLCD);
}

/*
 * lcd.h
 *
 *  Created on: Sep 22, 2014
 *      Author: brks9622
 */

#ifndef LCD_H_
#define LCD_H_

#define TREMOLO			0
#define REVERB	 		1
#define FLANGER			3
#define WAH		 		2
#define DISTORTION 		5
#define CRUNCH			4

#define MAIN 			11
#define CLEAR 			14
#define TUNER 			15
#define PRESETUP		16
#define PRESETDOWN		17
#define	LOADPRESET		18
#define SAVEPRESET		19
#define PRESETTIMEOUT	20
#define CHANGETREMOLO	21
#define CHANGEREVERB	22
#define CHANGEFLANGER	23
#define CHANGEWAH		24



void	findNote(int);
void 	initLCD(void);
void 	printLCD(int);
void 	addToLCD(int);
void 	goToMain(int* mainDisplay, int* on_off, int* numQueued, int distortion);
void 	toggleLCD(int,int,int,int);
void	toggleDistortion(int);
void 	controlLCD(int);
void	updateLCD(int* update, int* mainDisplay, int* on_off, int* presetNumber, int* numQueued, int distortion);
void 	printFreq(int);
void 	presetUp(int);
void 	presetDown(int);
void 	loadPresetScreen(int*, int*, int*);

void 	printTremolo();
void 	printReverb();
void 	printFlange();
void 	printWah();
void 	updateLevel(int,int);

#endif /* LCD_H_ */

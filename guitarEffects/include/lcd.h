/*
 * lcd.h
 *
 *  Created on: Sep 22, 2014
 *      Author: brks9622
 */

#ifndef LCD_H_
#define LCD_H_

#define TREMOLO			0
#define REVERB	 		5
#define FLANGER			1
#define DELAY	 		3
#define WAH 			4
#define DISTORTION		6
#define CRUNCH 			2
#define PHASER 			7
#define CHORUS 			8
#define PITCHSHIFT 		9
#define SUSTAIN			10
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
#define CHANGEBASS		24
#define CHANGEVOLUME	25
#define	CHANGETREBLE	26
#define CHANGEMID		27
#define CHANGEDELAY		29
#define	CHANGECHORUS	30

void	findNote(int);
void 	initLCD(void);
void 	printLCD(int);
void 	addToLCD(int);
void 	goToMain(int* mainDisplay, int* on_off, int* numQueued);
void  	shiftCursor(int);
void 	toggleLCD(int,int,int,int);
void 	controlLCD(int);
void	updateLCD(int* update, int* mainDisplay, int* on_off, int* presetNumber, int* numQueued);
void 	printFreq(int);
void 	presetUp(int);
void 	presetDown(int);
void 	loadPresetScreen(int*, int*, int*);

void 	printTremolo();
void 	printReverb();
void 	printDelay();
void 	printChorus();
void 	printFlange();
void 	updateLevel(int,int);

#endif /* LCD_H_ */

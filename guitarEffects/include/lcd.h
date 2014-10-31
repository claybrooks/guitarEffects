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
#define CRUNCH 		2
#define DELAY	 		3
#define WAH 			4
#define DISTORTION		5
#define FLANGE 			6
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
#define	CHANGECHORUS	23
#define CHANGEDELAY		24
#define	CHANGEFLANGE	25
#define CHANGEVOLUME	26
#define CHANGEBASS		27
#define CHANGEMID		28
#define	CHANGETREBLE	29



void 	initLCD(void);
void 	printLCD(int);
void 	addToLCD(int);
void 	goToMain();
void  	shiftCursor(int);
void 	toggleLCD(int,int,int);
void 	controlLCD(int);
void	updateLCD(int);
void 	printFreq(int);
void 	presetUp(int);
void 	presetDown(int);

void 	printTremolo();
void 	printReverb();
void 	printDelay();
void 	printChorus();
void 	printFlange();
void 	updateLevel(int,int);

#endif /* LCD_H_ */

/*
 * lcd.h
 *
 *  Created on: Sep 22, 2014
 *      Author: brks9622
 */

#ifndef LCD_H_
#define LCD_H_

#define TREMOLO		0
#define DISTORTION 	1
#define CRUNCH 		2
#define DELAY	 	3
#define WAH 		4
#define PHASER 		5
#define FLANGE 		6
#define REVERB 		7
#define CHORUS 		8
#define PITCHSHIFT 	9
#define MAIN 		10
#define CLEAR 		14
#define TUNER 		15

void 	initLCD(void);
void 	printLCD(int);
void 	addToLCD(int);
void 	toggleLCD(int,int,int);
void 	controlLCD(int);
void	updateLCD(int);
void 	printFreq(int);

#endif /* LCD_H_ */

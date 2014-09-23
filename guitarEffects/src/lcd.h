/*
 * lcd.h
 *
 *  Created on: Sep 22, 2014
 *      Author: brks9622
 */

#ifndef LCD_H_
#define LCD_H_

#define DELAY 		1
#define DISTORTION 	2
#define CRUNCH 		3
#define TREMOLO 	4
#define WAH 		5
#define PHASER 		6
#define FLANGE 		7
#define REVERB 		8
#define CHORUS 		9
#define PITCHSHIFT 	10
#define CLEAR 		14
#define TUNER 		15

void 	initLCD(void);
void 	printLCD(int);
void 	controlLCD(int);
void	updateLCD(int);
void 	printFreq(int);

#endif /* LCD_H_ */

/*
 * lcd.h
 *
 *  Created on: Sep 22, 2014
 *      Author: brks9622
 */

#ifndef LCD_H_
#define LCD_H_

#define CLEAR 	0
#define MAIN	1
#define TUNER 	2

void 	initLCD(void);
void 	printLCD(int);
void 	controlLCD(int);
int 	updateLCD(int);

#endif /* LCD_H_ */

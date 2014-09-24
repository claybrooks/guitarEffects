/*
 * handler.c
 *
 *  Created on: Sep 22, 2014
 *      Author: brks9622
 */
#include "handler.h"
#include "effect.h"
#include "lcd.h"
#include "fft.h"

int indexLookup(int input){
	if(input == 1) return 0;
	else if(input == 2) return 1;
	else if(input == 4) return 2;
	else if(input == 8) return 3;
	else if(input == 16) return 4;
	else if(input == 32) return 5;
	else if(input == 64) return 6;
	else if(input == 128) return 7;
	else return 8;
}

int handle(int input){
	//Clear pipeline from last use
	if(input == 0x0080){
		clearPipeline();
		return CLEAR;
	}
	//Switch to tuning function
	else if(input == 0x0040){
		return TUNER;
	}
	//Look to either queue effect or toggle state
	else{
		/*Simple lookup vs mathematical computation
		 * Gets the effect to be manipulated
		*/
		int effect = indexLookup(input);
		//toggleOn_Off return 1 if it can be toggled, else 0 meaning its not in queue;
		if(!toggleOn_Off(effect)) queueEffect(effect);		//queue the effect
		return effect;
	}
}

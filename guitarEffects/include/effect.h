/*
 * effect.h
 *
 *  Created on: Sep 22, 2014
 *      Author: brks9622
 */
#ifndef EFFECT_H_
#define EFFECT_H_
#include "DSP28x_Project.h"
#define PI 3.1415926

struct params{
	int tremoloCounter, tremoloRate, tremoloCount, tremoloLimit;
		Uint32 reverbDelay[800], reverbCount, reverbStart; //15 ms delay length at 44.1 kHz
};

Uint32 processDelay(Uint32,struct params*);
Uint32 processDistortion(Uint32, struct params*);
Uint32 processCrunch(Uint32, struct params*);
Uint32 processTremolo(Uint32, struct params*);
Uint32 processWah(Uint32, struct params*);
Uint32 processPhaser(Uint32, struct params*);
Uint32 processFlange(Uint32, struct params*);
Uint32 processReverb(Uint32, struct params*);
Uint32 processChorus(Uint32, struct params*);
Uint32 processDelay(Uint32, struct params*);
Uint32 processPitchShift(Uint32, struct params*);

void	queueEffect(int);	//Sticks effect into queue
void 	clearPipeline();	//Clears the queue
Uint32 	process(Uint32);		//Process the effects in the queue, FIFO
void 	initEffects();		//Initialize registers
int 	indexLookup(int);	//Maps GPIO input to an effect
int 	toggleOn_Off(int);	//Toggles effect on/off once its in the queue

#endif /* EFFECT_H_ */

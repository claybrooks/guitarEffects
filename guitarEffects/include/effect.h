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

//Type definition for process*Effect* method prototypes


struct params{
	int tremoloCounter, tremoloRate, tremoloCount, tremoloLimit;
		int reverbDelay[800], reverbCount, reverbStart; //15 ms delay length at 44.1 kHz
};

typedef int FUNC(int, struct params*);
int processDelay(int,struct params*);
int processDistortion(int, struct params*);
int processCrunch(int, struct params*);
int processTremolo(int, struct params*);
int processWah(int, struct params*);
int processPhaser(int, struct params*);
int processFlange(int, struct params*);
int processReverb(int, struct params*);
int processChorus(int, struct params*);
int processDelay(int, struct params*);
int processPitchShift(int, struct params*);

void savePreset(int presetNum);
int* loadPreset(int presetNum);
void	queueEffect(int);	//Sticks effect into queue
void 	clearPipeline();	//Clears the queue
int 	process(int, int,int*, FUNC**, struct params*);		//Process the effects in the queue, FIFO
void 	initEffects(struct params*);		//Initialize registers
int 	indexLookup(int);	//Maps GPIO input to an effect
int 	toggleOn_Off(int);	//Toggles effect on/off once its in the queue

#endif /* EFFECT_H_ */

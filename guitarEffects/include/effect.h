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
		int flangerDelay[600], flangerSweepCount, flangerCounter, flangerLimit, flangerStart, flangerCount;
		double flangerSweep; //15 ms delay length at 44.1
};

typedef int FUNC(int, struct params*, int*);
int processDelay(int, struct params*, int*);
int processDistortion(int, struct params*, int*);
int processCrunch(int, struct params*, int*);
int processTremolo(int, struct params*, int*);
int processWah(int, struct params*, int*);
int processPhaser(int, struct params*, int*);
int processFlanger(int, struct params*, int*);
int processReverb(int, struct params*, int*);
int processChorus(int, struct params*, int*);
int processDelay(int, struct params*, int*);
int processPitchShift(int, struct params*, int*);

void savePreset(int presetNum, int* location, int* on_off);
void loadPreset(int presetNum, FUNC**pipeline,FUNC**list, int* location, int* on_off, int* numQueued);
void	queueEffect(int);	//Sticks effect into queue
void 	clearPipeline();	//Clears the queue
int 	process(int, int,int*, FUNC**, struct params*, int*);		//Process the effects in the queue, FIFO
void 	initEffects(struct params*);		//Initialize registers
int 	indexLookup(int);	//Maps GPIO input to an effect
int 	toggleOn_Off(int);	//Toggles effect on/off once its in the queue

#endif /* EFFECT_H_ */

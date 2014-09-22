/*
 * effect.h
 *
 *  Created on: Sep 22, 2014
 *      Author: brks9622
 */
#ifndef EFFECT_H_
#define EFFECT_H_

#define PI 3.1415926
struct params{
	int tremoloCount;
};


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

void	queueEffect(int);	//Sticks effect into queue
void 	clearPipeline();	//Clears the queue
int 	process(int);		//Process the effects in the queue, FIFO
void 	initEffects();		//Initialize registers
int 	indexLookup(int);	//Maps GPIO input to an effect
int 	toggleOn_Off(int);	//Toggles effect on/off once its in the queue

#endif /* EFFECT_H_ */

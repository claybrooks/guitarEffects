/*
 * effect.h
 *
 *  Created on: Sep 22, 2014
 *      Author: brks9622
 */
#ifndef EFFECT_H_
#define EFFECT_H_

#define PI 3.1415926

#define DELAY 		1
#define DISTORTION 	2
#define CRUNCH 		3
#define TREMOLO 	4
#define WAH 		5
#define PHASER 		6
#define FLANGE 		7
#define REVERB 		8
#define CHORUS 		9
#define PITCHSHIFT 	10;

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

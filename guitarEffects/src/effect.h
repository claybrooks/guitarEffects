//#include "wah.h"
#ifndef __EFFECT__
#define __EFFECT__
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

void queueEffect(int (*f)(int, struct params*));
void clearPipeline();
int process(int val);
void handle(int input);
void initEffects();
int indexLookup(int);


#endif /* EFFECT */

#include "../include/effect.h"
#include "math.h"
#include "DSP28x_Project.h"

//Type definition for process*Effect* method prototypes
typedef Uint32 FUNC(Uint32, struct params*);

//Create FUNC variables
FUNC processTremolo,processDistortion,processCrunch,processDelay,processWah,processPhaser,processFlange,processReverb,processChorus,processPitchShift;

//Static list of available effects, GPIO must match this
FUNC *list[10] = {processTremolo,processReverb,processDistortion,processCrunch,processDelay,processWah,processPhaser,processFlange,processChorus,processPitchShift};

/*The indices of this array map  directly to the *list array.  This location array holds the location of the effect in the pipeline array.
 * Index 0 of the location array maps to index 0 of the list array.  But the data at index 0 of the location arary points to
 * where that effect is located in the pipeline array. Increase in mem usage for speed gain, don't have to iterate through *list to find
 * the right effect.*/

int location[10];

//Array of FUNC's, this is the queue set by the user.
FUNC *pipeline[10];

//State of each queued effect, this allows user to stomp effects on/off without losing location in queue
//Indices of this array map direclty to indices of pipeline
int on_off[10];

//Number of queued effects
int numQueued;

//Struct for all the parameters available to effects, passed into the process functions
static struct params params;

void initEffects(){
	//Clear the queue
	clearPipeline();
	//Tremolo
	params.tremoloCounter = 0;
			params.tremoloCount = 0;
			params.tremoloRate = 0;
			params.tremoloLimit = 0;
			params.reverbCount = 0;
			params.reverbStart = 0;
}

int toggleOn_Off(int effect){
	//If location[] == 0, then the effect is not in the queue so return 0 to signal
	//that this effect needs to be queued up.  Else just toggle the state.
	if(location[effect] != -1){
		on_off[location[effect]] ^= 1;
		return 1;
	}
	else return 0;
}

void queueEffect(int effect){

	location[effect] = numQueued;			//Set location of the effect in the location array, corresponds to the index in the queue, -1 because we inc numQueued first
	pipeline[numQueued] = list[effect];	//queue function pointer into the pipeline, -1 because we inc numQueued first
	on_off[numQueued] = 1;					//Turn effect on, makes sense for the user
	numQueued++;
}


void clearPipeline(){
	//Clear location/on_off arrays
	int i;
	for(i=0; i < 10; i++){
		location[i] = -1;
		on_off[i] = 0;
	}
	numQueued = 0;
}

Uint32 process(Uint32 sample){
	int index;
	//Loop through the entire queue, if its on -> process, else skip
	for(index = 0; index < numQueued; index++){
		if(on_off[index]) sample = pipeline[index](sample, &params);  //pipeline[index] maps to a function, (sample, &params) is the prototype
	}
	return sample;
}

/*
 * Processing functions
 */
Uint32 processDelay(Uint32 sample, struct params* p){
	return sample;
}
Uint32 processDistortion(Uint32 sample, struct params* p){
	if(sample > 0xF20) return 0xF20;
	//else if (sample < 0xC000) return 0xC000;
	else return sample;
}
Uint32 processCrunch(Uint32 sample, struct params* p){
	return sample;
}
Uint32 processTremolo(Uint32 sample, struct params* p){
	//Sets rate at which the effect runs
		double max = 0x0666;
		double pedal = AdcRegs.ADCRESULT1>>4;
		p->tremoloLimit = (double)4000*(pedal/max)+ 3000;

		//Count up or down, if it hits upper limit then count up else count down
		if(p->tremoloCounter >= p->tremoloLimit) p->tremoloCount = -1;
		else if(p->tremoloCounter == 0) p->tremoloCount = 1;
		p->tremoloCounter+=p->tremoloCount;

		//Calculate new tremolo sample
		double temp = (double)p->tremoloCounter*.7/(double)p->tremoloLimit;//*(double)sample;

		return (int)(temp*(double)sample);
}
Uint32 processWah(Uint32 sample, struct params* p){
	return sample;
}
Uint32 processPhaser(Uint32 sample, struct params* p){
	return sample;
}
Uint32 processFlange(Uint32 sample, struct params* p){
	return sample;
}
Uint32 processReverb(Uint32 sample, struct params* p){
	//Reinitialize the reverb array on every start

	if(p->reverbCount == 800){
			p->reverbStart = 1;
			p->reverbCount = 0;
		}
		double decay = AdcRegs.ADCRESULT3 >> 4;
		decay = ((double)decay / (double)0xFFF)*.3 + .15;
		//Once reinitialized, start to process reverb
		if(p->reverbStart){

			Uint32 temp = p->reverbDelay[p->reverbCount];
			sample += p->reverbDelay[p->reverbCount];
			p->reverbDelay[p->reverbCount] = (double)sample*decay + (double)temp*(decay-.08);
		}
		else p->reverbDelay[p->reverbCount] = (double)sample*decay;

		p->reverbCount++;
		return sample;
}
Uint32 processChorus(Uint32 sample, struct params* p){
	return sample;
}
Uint32 processPitchShift(Uint32 sample, struct params* p){
	return sample;
}

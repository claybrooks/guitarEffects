#include "effect.h"
#include "math.h"
#include "DSP28x_Project.h"

//Type definition, like a #define but more of a variable
typedef int FUNC(int, struct params*);

//Create FUNC variables
FUNC processDelay,processDistortion,processCrunch,processTremolo,processWah,processPhaser,processFlange,processReverb,processChorus,processPitchShift;

//Static list of available effects, GPIO must match this
FUNC *list[10] = {processDelay,processDistortion,processCrunch,processTremolo,processWah,processPhaser,processFlange,processReverb,processChorus,processPitchShift};

/*The indices of this array map  directly to the list array.  This location array holds the location of the effect in the pipeline array.
 * Index 0 of the location array maps to index 0 of the list array.  But the data at index 0 of the location arary points to
 * where that effect is located in the pipeline array
 */
int location[10];

//Array of FUNC's, user set queue.
FUNC *pipeline[10];

//State of each queued effect, this allows user to stomp effects on/off without losing location in queue
//Indices of this array map direclty to indices of pipeline
int on_off[10];

//Number of queued effects
int numQueued;

//Struct for all the parameters available to effects, passed into the process functions
static struct params params;

void initEffects(){
	clearPipeline();		//Clear the queue
}

int toggleOn_Off(int effect){
	if(location[effect] != -1){
		on_off[location[effect]] ^= 1;
		return 1;
	}
	else return 0;
}

//Eliminates need to call log(input)/log(2).  Simple lookup will be faster
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

void queueEffect(int effect){
	location[effect] = numQueued;		//Set location of the effect in the location array, basically where is effect stored in queue?
	pipeline[numQueued] = list[effect];	//Store function pointer, store function pointer into the pipeline
	on_off[numQueued] = 1;				//Turn effect on, makes sense for the user
	numQueued++;						//Increase numQueued
}

void clearPipeline(){
	int i;
	for(i=0; i < 10; i++){				//Clear location/on_off arrays
		location[i] = -1;
		on_off[i] = 0;
	}
	numQueued = 0;
}

int process(int sample){
	int index;
	//Loop through the entire queue, if its on -> process, else skip
	for(index = 0; index < numQueued; index++){
		if(on_off[index]) sample = pipeline[index](sample, &params);  //pipeline[index] maps to a function, (sample, &params) is the prototype
	}
	return sample;
}

int processDelay(int sample, struct params* p){
	return sample;
}
int processDistortion(int sample, struct params* p){
	return sample;
}
int processCrunch(int sample, struct params* p){
	return sample;
}
int processTremolo(int sample, struct params* p){
	float adcmax = 0x8CA0;
	float pedalLevel = AdcRegs.ADCRESULT1;
	float result = pedalLevel/adcmax;
	return sample * (result);

}
int processWah(int sample, struct params* p){
	return sample;
}
int processPhaser(int sample, struct params* p){
	return sample;
}
int processFlange(int sample, struct params* p){
	return sample;
}
int processReverb(int sample, struct params* p){
	return sample;
}
int processChorus(int sample, struct params* p){
	return sample;
}
int processPitchShift(int sample, struct params* p){
	return sample;
}

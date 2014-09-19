#include "wah.h"
#define PI 3.1415926

void queueEffect(int);
void handleInput(int);

long tremoloCount = 0;
long frequency = 2205;
int control = 0;

int pipeline[10] = {0,0,0,0,0,0,0,0,0,0};
int on_off[10] = {0,0,0,0,0,0,0,0,0,0};
int numEffects = 0;


#define DISTORTION	0
#define CRUNCH 		1
#define TREMOLO		2
#define WAH			3
#define	PHASER		4
#define FLANGE		5
#define REVERB		6
#define CHORUS		7
#define DELAY		8
#define PITCH_SHIFT	9

void handleInput(int input){
	int effect = (int)log(input)/log(2);	//Effect that wants to be set or turned off
	int i;
	for(i = 0; i < numEffects; i++){
		//If already queued toggle state
		if(pipeline[i] == effect){
			if(on_off[i] == 1) on_off[index] = 0;	//Turn off effect
			else on_off[i] = 1;						//Turn on effect
			break;
		}
		//Queue effect
		if(i == numEffects-1) queueEffect(effect);
	}
}

void queueEffect(int effect){
	pipeline[numEffects] = effect;
	on_off[numEffects] = 1;
	numEffects++;
}

void clearPipeline(){
	int i;
	for(i = 0; i < 10; i++){
		pipeline[i] = 0;
		on_off[i] = 0;
	}
	numEffects = 0;
}

int processEffect(int val){
	int effect, on;

	for(index = 0; index < 10; index++){
		//EFFECT PIPLINE
		//COMP/EFX::Compressor, Auto Wah, Booster, Tremolo, Phaser
		//DRIVE::Clean, Crunch, Fuzz, Dirty
		//MODULATION::Chorus, Ensemble, Flanger, pitch shift
		//DELAY:: Delay, Tape echo, Analog Delay,
		//REVERB:: Hall, Room, Spring

		effect = pipeline[index];
		on = on_off[index];

		if(effect == 0) break;
		else if(effect == DISTORTION && on){

		}
		else if(effect == CRUNCH && on){

		}
		else if(effect == TREMOLO && on){
			if(control == 0){
				val = ((val - 0x4E0)*(1+.5*sin(2*PI*tremoloCount/frequency)))+0x4E0;
				tremoloCount++;
				if(tremoloCount == frequency) control = 1;
			}
			else{
				val = ((val - 0x4E0)*(1+.5*sin(2*PI*tremoloCount/frequency)))+0x4E0;
				tremoloCount--;
				if(tremoloCount == 0) control = 0;
			}
		}
		else if(effect == WAH && on){
			sweepWAH();
			val = val + (int)getWAH(val) - 0x4F0;

		}

		else if(effect == PHASER && on){


		}
		else if(effect == FLANGE && on){

		}
		else if(effect == REVERB && on){

		}
		else if(effect == CHORUS && on){

		}
		else if(effect == DELAY && on){

		}
		else if(effect == PITCH_SHIFT && on){

		}
	}
	return val;
}

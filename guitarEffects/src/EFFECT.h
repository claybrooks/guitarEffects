#include "wah.h"
#define PI 3.1415926
long tremoloCount = 0;
long frequency = 2205;
int control = 0;

int getEFFECT(int val){
	//EFFECT PIPLINE
	//COMP/EFX::Compressor, Auto Wah, Booster, Tremolo, Phaser
	//DRIVE::Clean, Crunch, Fuzz, Dirty
	//MODULATION::Chorus, Ensemble, Flanger, pitch shift
	//DELAY:: Delay, Tape echo, Analog Delay,
	//REVERB:: Hall, Room, Spring

//COMP/EFX
	if(GpioDataRegs.GPADAT.bit.GPIO8){
		sweepWAH();
		val = val + (int)getWAH(val) - 0x4F0;

	}
	else if(GpioDataRegs.GPADAT.bit.GPIO9){
		if(control == 0){
			val *= (1+.5*sin(2*PI*tremoloCount/frequency));
			tremoloCount++;
			if(tremoloCount == frequency) control = 1;
		}
		else{
			val *= (1+.5*sin(2*PI*tremoloCount/frequency));
			tremoloCount--;
			if(tremoloCount == 0) control = 0;
		}
	}

	return val;

}

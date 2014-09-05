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
	/*if(GpioDataRegs.GPADAT.bit.GPIO8){
		sweepWAH();
		val = val + (int)getWAH(val) - 0x4F0;

	}
	else if(GpioDataRegs.GPADAT.bit.GPIO9){
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
	if(GpioDataRegs.GPADAT.bit.GPIO10){
		val = val - 0x05C7;
		if(val <= 0x0200){
			val = 2*val;
		}
		else if(val <= 0x0400 ){
			val = (3-(2-3*val)*(2-3*val))/3;
		}
		else{
			val = 0x0600;
		}
		val += 0x05C7;

	}*/

	return val;

}

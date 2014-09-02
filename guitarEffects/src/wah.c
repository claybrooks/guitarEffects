//#include "butterworth_bp.h"
#include "bp_iir.h"
#include "wah.h"

#define freq_step 50
#define maxf 1200
#define minf 400
#define sampling 48000

static int centerFrequency;	//Variable center frequency
static int counter;			//Counter for variable centerFrequency
static int limit;			//Max for counter
static int incDecToggle;
static int maxFrequency;	//Max frequency to sweep to
static int minFrequency;	//Min frequency to sweep from

void initWAH(int effect_rate, int Q){
        //wah process variables
        centerFrequency = 0;
        counter = effect_rate;
        incDecToggle = 0;
		index = 0;

        //Rate at which the center frequency sweeps
        limit = effect_rate;

        //Index ranges for frequency sweep, shifted to 0
        minFrequency = 0;
        maxFrequency = (maxf - minf)/freq_step;

        //Initialize registers to steady state
		int i;
		for(i = 0; i < NCoef+1; i++){
			y[i] = 0;
			x[i] = 0;
		}
}

double getWAH(int val) {
      return filterBP(val,index);
}

void sweepWAH(void) {
		//Sweep the wah after designated time has passed
        if (!--counter) {
        	//If the current center frequency is the min frequency, increment up
			if (!incDecToggle) {
					index++;
					//If you hit max freq, toggle incDecToggle to start decrementing the step
					if (centerFrequency > maxFrequency) incDecToggle = 1;
			}
			else if (incDecToggle) {
					index--;
					if (centerFrequency == minFrequency) incDecToggle = 0;
			}
			counter = limit;
		}
}

#include "butterworth_bp.h"
#include "wah.h"

static int centerFrequency;		//Variable center frequency
static int counter;			//Counter for variable centerFrequency
static int limit;	//Max for counter
static int incDecToggle;
static int maxFrequency;		//Max frequency to sweep to
static int minFrequency;		//Min frequency to sweep from
static int frequencyStep;			//Steps for the frequency sweep
static struct bp_filter bp;	//Filter structure

void initWAH(int effect_rate, long sampling, int maxf, int minf, int Q, int freq_step){
        //wah process variables
        centerFrequency = 0;
        counter = effect_rate;
        incDecToggle = 0;

        //Rate at which the center frequency sweeps
        limit = effect_rate;

        //Index ranges for frequency sweep, shifted to 0
        minFrequency = 0;
        maxFrequency = (maxf - minf)/freq_step;

        //Initialize registers to steady state
		bp.y[2] = 0;
		bp.y[1] = 0;
		bp.y[0] = 0;
		bp.x[2] = 0;
		bp.x[1] = 0;
		bp.x[0] = 0;

        initBP(sampling, Q, freq_step, minf);
        frequencyStep = freq_step;
}

double getWAH(int val) {
      return filterBP(val,&bp)*1000 + 0x4f0;


}

void sweepWAH(void) {
		//Sweep the wah after designated time has passed
        if (!--counter) {
        	//If the current center frequency is the min frequency, increment up
			if (!incDecToggle) {
					loadBP(&bp,(centerFrequency+=frequencyStep));
					//If you hit max freq, toggle incDecToggle to start decrementing the step
					if (centerFrequency > maxFrequency) incDecToggle = 1;
			}
			else if (incDecToggle) {
					loadBP(&bp,(centerFrequency-=frequencyStep));
					if (centerFrequency == minFrequency) incDecToggle = 0;
			}
			counter = limit;
		}
}

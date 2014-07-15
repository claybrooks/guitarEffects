/*
 * butterworth_bp.c
 *
 *  Created on: Jul 14, 2014
 *      Author: Clay
 */
#include "butterworth_bp.h"
#include "math.h"
#define PI 3.1415926
#define num_val 120

static struct bp_vals bp_val_array[num_val];

/*This initialization function will create the
band pass filter coefficients array, you have to specify:
fs = Sampling Frequency
Q      = Q factor, Higher Q gives narrower band
fstep  = Frequency step to increase center frequencies
in the array
fmin   = Fc will range from fmin to (fmin + fstep * 120)
*/
void initBP(double fs,double Q, int fstep, int fmin) {
	int i = 0;
	double B = 1.0/Q;
	//damp = gb/sqrt(1 - pow(gb,2))
	for (i=0;i<num_val;i++) {
			//wo = 2*PI*(fstep*i + fmin)/fsfilt;
			//bp_coeff_arr[i].e = 1/(1 + damp*tan(wo/(Q*2)));
			//bp_coeff_arr[i].p = cos(wo);
			//bp_coeff_arr[i].d[0] = (1-bp_coeff_arr[i].e);
			//bp_coeff_arr[i].d[1] = 2*bp_coeff_arr[i].e*bp_coeff_arr[i].p;
			//bp_coeff_arr[i].d[2] = (2*bp_coeff_arr[i].e-1);
			double c = cos((fmin+(fstep*i))/(2*fs))/sin((fmin+(fstep*i))/(2*fs));
			bp_val_array[i].n0 = B*c;
			bp_val_array[i].n2 = -bp_val_array[i].n0;
			bp_val_array[i].d[0] = B*c+c*c+1;
			bp_val_array[i].d[1] = -2/(c*c-1);
			bp_val_array[i].d[2] = -B*c+c*c+1;
	}
}

void loadBP(struct bp_filter* bp,int index) {
	bp->n0 = bp_val_array[index].n0;
	bp->n2 = bp_val_array[index].n2;
	bp->d[0] = bp_val_array[index].d[0];
	bp->d[1] = bp_val_array[index].d[1];
	bp->d[2] = bp_val_array[index].d[2];
}

double filterBP(int val, struct bp_filter* bp){
		//Filter calculation
       double return_val =  (bp->n0/bp->d[0]*val + bp->n2/bp->d[0]*bp->x[0] - bp->d[1]/bp->d[0]*bp->y[1] - bp->d[2]/bp->d[0]*bp->y[0]);

        //Shift ADC data down;
        bp->x[0] =  bp->x[1];
        bp->x[1] =  bp->x[2];
        bp->x[2] = val;

        //Shift calculated data down
        bp->y[0] =  bp->y[1];
        bp->y[1] =  bp->y[2];
        bp->y[2] = return_val;


        return return_val;
}

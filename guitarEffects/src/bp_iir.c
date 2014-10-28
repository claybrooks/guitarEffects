#include "bp_iir.h"
#include <math.h>
#define BP_MAX_COEFS 120
#define PI 3.1415926

/*This is an array of the filter parameters object
defined in the br_iir.h file*/
static struct bp_coeffs bp_coeff_arr[BP_MAX_COEFS];

/*This initialization function will create the
band pass filter coefficients array, you have to specify:
fsfilt = Sampling Frequency
gb     = Gain at cut frequencies
Q      = Q factor, Higher Q gives narrower band
fstep  = Frequency step to increase center frequencies
in the array
fmin   = Minimum frequency for the range of center   frequencies
*/
void bp_iir_init(double fsfilt,double gb,double Q,short fstep, short fmin, struct bp_filter * H) {
      int i;
      double damp;
      double wo;

       damp = gb/sqrt(1 - pow(gb,2));

        for (i=0;i<BP_MAX_COEFS;i++) {
            wo = 2*PI*(fstep*i + fmin)/fsfilt;
                bp_coeff_arr[i].e = 1/(1 + damp*tan(wo/(Q*2)));
                bp_coeff_arr[i].p = cos(wo);
                bp_coeff_arr[i].d[0] = (1-bp_coeff_arr[i].e);
                bp_coeff_arr[i].d[1] = 2*bp_coeff_arr[i].e*bp_coeff_arr[i].p;
                bp_coeff_arr[i].d[2] = (2*bp_coeff_arr[i].e-1);
        }
        H->x[0] = 0;
               H->x[1] =  0;
               H->x[2] = 0;

               H->y[0] =  0;
               H->y[1] =  0;

               H->y[2] = 0;
}

/*This function loads a given set of band pass filter coefficients acording to a center frequency index
into a band pass filter object
H = filter object
ind = index of the array mapped to a center frequency
*/
void bp_iir_setup(struct bp_filter * H,int ind) {
        H->e = bp_coeff_arr[ind].e;
        H->p = bp_coeff_arr[ind].p;
        H->d[0] = bp_coeff_arr[ind].d[0];
        H->d[1] = bp_coeff_arr[ind].d[1];
        H->d[2] = bp_coeff_arr[ind].d[2];
}

/*This function loads a given set of band pass filter coefficients acording to a center frequency index
into a band pass filter object
H = filter object
ind = index of the array mapped to a center frequency
*/
double bp_iir_filter(double yin,struct bp_filter * H) {
        double yout;

        H->x[0] =  H->x[1];
        H->x[1] =  H->x[2];
        H->x[2] = yin;

        H->y[0] =  H->y[1];
        H->y[1] =  H->y[2];

        H->y[2] = H->d[0]* H->x[2] - H->d[0]* H->x[0] + (H->d[1]* H->y[1]) - H->d[2]* H->y[0];

        yout =  H->y[2];

        return yout;
}

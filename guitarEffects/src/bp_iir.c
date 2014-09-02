#include "bp_iir.h"
#define BP_MAX_STEPS 17
#define PI 3.1415926

/*This function loads a given set of band pass filter coefficients acording to a center frequency index
into a band pass filter object
H = filter object
ind = index of the array mapped to a center frequency
*/
int filterBP(int yin, int bpindex) {
	int n;
	int coefIndex = bpindex*5;
	
	for(n=NCoef; n>0; n--) {
		x[n] = x[n-1];
		y[n] = y[n-1];
	}
	
	x[0] = yin;
	y[0] = acoef[0+coefIndex] * x[0];
	for(n=1; n<=NCoef; n++)	y[0] += acoef[n+coefIndex] * x[n] - bcoef[n+coefIndex] * y[n];
	y[0] /= bcoef[0+coefIndex];
	return y[0] / gains[bpindex];
	/*	
	double yout;

	H->x[0] =  H->x[1];
	H->x[1] =  H->x[2];
	H->x[2] = yin;

	H->y[0] =  H->y[1];
	H->y[1] =  H->y[2];

	H->y[2] = H->d[0]* H->x[2] - H->d[0]* H->x[0] + (H->d[1]* H->y[1]) - H->d[2]* H->y[0];

	yout =  H->y[2];

	return yout;*/
}

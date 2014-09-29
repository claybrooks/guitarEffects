/*
 * butterworth_bp.h
 *
 *  Created on: Jul 14, 2014
 *      Author: Clay
 */

#ifndef BUTTERWORTH_BP_H_
#define BUTTERWORTH_BP_H_

struct bp_vals{
        double n0;
        double n2;
        double d[3];	//Will change with differeing frequencies
};

struct bp_filter{
		double n0;
		double n2;
		double d[3];
        double x[3];
        double y[3];
};
void initBP(double fs,double Q, int fstep, int fmin);
void loadBP(struct bp_filter* bp,int index);
double filterBP(int val, struct bp_filter* bp);
#endif /* BUTTERWORTH_BP_H_ */

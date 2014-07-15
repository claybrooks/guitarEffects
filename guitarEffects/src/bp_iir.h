#ifndef __BP_IIR_H__
#define __BP_IIR_H__

struct bp_coeffs{
        double e;
        double p;
        double d[3];
};

struct bp_filter{
        double e;
        double p;
        double d[3];
        double x[3];
        double y[3];
};

extern void bp_iir_init(double fsfilt,double gb,double Q,int fstep, int fmin);
extern void bp_iir_setup(struct bp_filter * H,int index);
extern double bp_iir_filter(double yin,struct bp_filter * H);

#endif

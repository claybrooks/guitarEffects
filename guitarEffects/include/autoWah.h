/*
 * autoWah.h
 *
 *  Created on: Oct 27, 2014
 *      Author: Clay
 */

#ifndef AUTOWAH_H_
#define AUTOWAH_H_
#include "DSP28x_Project.h"
extern void AutoWah_init(short effect_rate,short sampling,short maxf,short minf,short Q,double gainfactor,short freq_step);
extern Uint16 AutoWah_process(double xin);
extern void AutoWah_sweep(double xin);

#endif /* AUTOWAH_H_ */

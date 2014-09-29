/*
 * wah.h
 *
 *  Created on: Sep 22, 2014
 *      Author: brks9622
 */

#ifndef WAH_H_
#define WAH_H_

extern void initWAH(int effect_rate, int Q);
extern double getWAH(int xin);
extern void sweepWAH(void);
static int index;

#endif /* WAH_H_ */

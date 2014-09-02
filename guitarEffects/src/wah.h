#ifndef __AUTOWAH_H__
#define __AUTOWAH_H__

extern void initWAH(int effect_rate, int Q);
extern double getWAH(int xin);
extern void sweepWAH(void);
static int index;

#endif


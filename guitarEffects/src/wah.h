#ifndef __AUTOWAH_H__
#define __AUTOWAH_H__

extern void initWAH(int effect_rate,long sampling,int maxf,int minf,int Q,int freq_step);
extern double getWAH(int xin);
extern void sweepWAH(void);

#endif


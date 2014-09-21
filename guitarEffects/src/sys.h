#ifndef __SYS__
#define __SYS__

interrupt void cpu_timer0_isr(void);
interrupt void xint1_isr(void);

void initINTS();
void initXINT1();
void updateTIMER0(float time);
void initTIMER0();

#endif

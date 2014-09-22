/*
 * sys.h
 *
 *  Created on: Sep 22, 2014
 *      Author: brks9622
 */

#ifndef SYS_H_
#define SYS_H_

interrupt void cpu_timer0_isr(void);
interrupt void xint1_isr(void);

void initINTS();
void initXINT1();
void updateTIMER0(float time);
void initTIMER0();

#endif /* SYS_H_ */

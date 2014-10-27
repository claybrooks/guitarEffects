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
interrupt void adc_isr(void);
interrupt void cpu_timer1_isr(void);
interrupt void epwm5_isr(void);
//interrupt void i2c_int1a_isr(void);
interrupt void local_D_INTCH1_ISR(void); // Channel 1 Rx ISR
interrupt void local_D_INTCH2_ISR(void); // Channel 2 Tx ISR

void initINTS();
void initXINT1();
void updateTimer0(float time);
void initTimer0();

#endif /* SYS_H_ */

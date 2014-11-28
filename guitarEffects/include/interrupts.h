/*
 * interrupts.h
 *
 *  Created on: Sep 22, 2014
 *      Author: Clay, Jonathan
 */

#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

interrupt void cpu_timer0_isr(void);
interrupt void timeout(void);

interrupt void preset_up(void);
interrupt void preset_down(void);
interrupt void load_preset(void);
interrupt void save_preset(void);
interrupt void effects(void);


interrupt void i2c_int1a_isr(void);
interrupt void rotary(void);

void initINTS();
void initXINT1();
void updateTimer0(float time);
void initTimer0();

#endif /* INTERRUPTS_H_ */

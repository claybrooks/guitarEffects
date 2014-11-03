/*
 * sys.h
 *
 *  Created on: Sep 22, 2014
 *      Author: brks9622
 */

#ifndef SYS_H_
#define SYS_H_

interrupt void cpu_timer0_isr(void);
interrupt void preset_up(void);
interrupt void preset_down(void);
interrupt void load_preset(void);
interrupt void save_preset(void);
interrupt void effects(void);
/*interrupt void xint6_isr(void);
interrupt void xint7_isr(void);
interrupt void xint8_isr(void);*/

interrupt void adc_isr(void);
interrupt void cpu_timer1_isr(void);
interrupt void i2c_int1a_isr(void);

void initINTS();
void initXINT1();
void updateTimer0(float time);
void initTimer0();

#endif /* SYS_H_ */

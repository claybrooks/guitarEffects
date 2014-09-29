/*
 * adc.h
 *
 *  Created on: Sep 22, 2014
 *      Author: brks9622
 */

#ifndef ADC_H_
#define ADC_H_

#define ADC_MODCLK 0x3	// HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 150/(2*3)   = 25.0 MHz
#define ADC_CKPS   0x1	// ADC module clock = HSPCLK/2*ADC_CKPS   = 25.0MHz/(1*2) = 12.5MHz
#define ADC_SHCLK  0xA  // S/H width in ADC module periods                        = 16 ADC clocks

void initADC();
int getAdc();


#endif /* ADC_H_ */

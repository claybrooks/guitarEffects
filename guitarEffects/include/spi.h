/*
 * spi.h
 *
 *  Created on: Sep 22, 2014
 *      Author: brks9622
 */

#ifndef SPI_H_
#define SPI_H_
#include "DSP28x_Project.h"

extern void init_mcbsp_spi();
void mcbsp_xmit(Uint32 command);
void write_dac(int sample);
int read_adc();
void init_adc_spi();


#endif /* SPI_H_ */

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
void mcbsp_xmit(Uint32 a);
void write_dac(int);
int readSPI();
void initSPI(void);

#endif /* SPI_H_ */

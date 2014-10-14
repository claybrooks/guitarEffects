/*
 * spi.c
 *
 *  Created on: Sep 19, 2014
 *      Author: Clay
 */
#include "../include/spi.h"
#include "DSP28x_Project.h"

void writeSPI(int val){
	GpioDataRegs.GPADAT.bit.GPIO19 = 0;
	SpiaRegs.SPITXBUF = 0x4000 | val;
	DELAY_US(2);
	GpioDataRegs.GPADAT.bit.GPIO19 = 1;
}
int readSPI(){
	GpioDataRegs.GPADAT.bit.GPIO19 = 1;  //LRCIN, 1 = Left channel, 0 = right channel
	return 0;
}

void initSPI(){
	InitSpiaGpio();
	EALLOW;
	SpiaRegs.SPICCR.bit.SPISWRESET=0; 		// Reset SPI

	SpiaRegs.SPICCR.bit.SPICHAR =  0xF;		//16-bit character

	SpiaRegs.SPICTL.bit.MASTER_SLAVE = 0;   // Slave mode enabled
	SpiaRegs.SPICTL.bit.TALK = 1; 			// Transmit capability enabled
	SpiaRegs.SPISTS.all=0x0000;
	SpiaRegs.SPIBRR = 30;           	// Baud rate ~  16.6666MHz
	SpiaRegs.SPICCR.bit.CLKPOLARITY = 1;  	// falling edge with delay
	SpiaRegs.SPICTL.bit.CLK_PHASE = 1;
	SpiaRegs.SPICTL.bit.SPIINTENA = 1;

	SpiaRegs.SPICCR.bit.SPISWRESET=1;  		// Enable SPI
}





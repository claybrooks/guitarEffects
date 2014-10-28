/*
 * spi.c
 *
 *  Created on: Sep 19, 2014
 *      Author: Clay
 */
#include "../include/spi.h"
#include "DSP28x_Project.h"

void write_dac(int sample){
	long temp = sample;
	Uint32 command = 0x18000000 | (temp<<8);
	mcbsp_xmit(command);
}
int read_adc(){
	//Toggle CONVST high
	GpioDataRegs.GPADAT.bit.GPIO19 = 1;	//CONVST, ~CS
	DELAY_US(2);	//Delay min of 500ns, max of 8800ns.

	while(SpiaRegs.SPIFFRX.bit.RXFFST !=1) { }
	int sample = SpiaRegs.SPIRXBUF;
	return sample;
}

void init_adc_spi(){
	InitSpiaGpio();
	EALLOW;
	SpiaRegs.SPICCR.bit.SPISWRESET=0; 		// Reset SPI

	SpiaRegs.SPICCR.bit.SPICHAR =  0xF;		//16-bit character

	SpiaRegs.SPICTL.bit.MASTER_SLAVE = 0;   // Slave mode enabled
	SpiaRegs.SPICTL.bit.TALK = 1; 			// Transmit capability enabled
	SpiaRegs.SPISTS.all=0x0000;
	SpiaRegs.SPIBRR = 30;           	// Baud rate ~  16.6666MHz
	SpiaRegs.SPICCR.bit.CLKPOLARITY = 0;  	// Rising edge with delay
	SpiaRegs.SPICTL.bit.CLK_PHASE = 1;

	SpiaRegs.SPICCR.bit.SPISWRESET=1;  		// Enable SPI
}

void init_mcbsp_spi()
{   EALLOW;
     // McBSP-B register settings
    McbspbRegs.SPCR2.all=0x0000;		 // Reset FS generator, sample rate generator & transmitter
	McbspbRegs.SPCR1.all=0x0000;		 // Reset Receiver, Right justify word, Digital loopback dis.
	McbspbRegs.SPCR1.bit.RJUST = 2;
    McbspbRegs.PCR.all=0x0F08;           //(CLKXM=CLKRM=FSXM=FSRM= 1, FSXP = 1)
    McbspbRegs.SPCR2.bit.FREE = 1;
    McbspbRegs.SPCR2.bit.SOFT = 1;
    McbspbRegs.SPCR1.bit.CLKSTP = 2;     // Together with CLKXP/CLKRP determines clocking scheme
	McbspbRegs.PCR.bit.CLKXP = 0;		 // CPOL = 1, CPHA = 0 falling edge no delay
	McbspbRegs.PCR.bit.CLKRP = 0;
    McbspbRegs.RCR2.bit.RDATDLY=0;      // FSX setup time 1 in master mode. 0 for slave mode (Receive)
    McbspbRegs.XCR2.bit.XDATDLY=0;      // FSX setup time 1 in master mode. 0 for slave mode (Transmit)

	McbspbRegs.RCR1.bit.RWDLEN1=2;       // 16-bit word
    McbspbRegs.XCR1.bit.XWDLEN1=4;       // 24-bit word

    McbspbRegs.SRGR2.all=0x2000; 	 	 // CLKSM=1, FPER = 1 CLKG periods
    McbspbRegs.SRGR1.all= 0x0063;	     // Frame Width = 1 CLKG period, CLKGDV=99 baud rate = 3.75 MHz/(99+1)= 375 kHz
    delay_loop();                        // Wait at least 2 SRG clock cycles
	McbspbRegs.SPCR2.bit.GRST=1;         // Enable the sample rate generator
    clkg_delay_loop();                   // Wait at least 2 CLKG cycles
	McbspbRegs.SPCR2.bit.XRST=1;         // Release TX from Reset
	McbspbRegs.SPCR1.bit.RRST=1;         // Release RX from Reset
    McbspbRegs.SPCR2.bit.FRST=1;         // Frame Sync Generator reset
    EDIS;
}

void mcbsp_xmit(Uint32 a)
{   while (McbspbRegs.SPCR2.bit.XRDY == 0){}
	Uint16 lowerHalf = a>>8;
	Uint16 upperHalf = (a >> 24);
	McbspbRegs.DXR1.all = lowerHalf;
    McbspbRegs.DXR2.all =  upperHalf;

}



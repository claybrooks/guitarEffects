// TI File $Revision:  $
// Checkin $Date:  $
//###########################################################################
//
// FILE:	AIC23_SPI_control.c
//
// TITLE:	McBSP-B SPI AIC23 Control Interface
//
// DESCRIPTION:
//
// This file includes functions which initialize the McBSP-B in SPI mode and
// transmits initialization commands via the McBSP-B to the
// AIC23 using AIC23 driver functions found in AIC23.c
//
//###########################################################################
// $TI Release:   $
// $Release Date:   $
//###########################################################################


/* McBSP-B SPI AIC23 Control Interface */

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "AIC23.h"
#include "AIC23_SPI_control.h"

/* Useful Definitions */
// Sample Rates:
#define SRUSB44_1    0x0022 // 44.1 kHz USB, BOSR set
#define SR48         0x0000 // 48 kHz USB/normal, BOSR clear
#define SR44_1       0x0020 // 44.1 kHz normal - 11.2896 MHz MCLK, BOSR clear


// Volume Control:
#define LIV         0x0017 // Left line input volume control
                           // 11111=12 dB down to 00000= -34.5 dB
						   // at 1.5 dB per step
						   // Default = 10111 = 0dB
#define RIV         0x0017 // Right line input volume control
                           // 11111=12 dB down to 00000= -34.5 dB
						   // at 1.5 dB per step
						   // Default = 10111 = 0dB
#define LHV         0x0069 // Left Channel Headphone Volume Control
                           // 1111111 = 6 dB, 79 steps between +6 and -73 dB (mute)
						   // 0110000 = -73 dB (mute)
						   // Currently set at 1101001 = -16 dB
#define RHV         0x0069 // Right Channel Headphone Volume Control
                           // 1111111 = 6 dB, 79 steps between +6 and -73 dB (mute)
						   // 0110000 = -73 dB (mute)
						   // Currently set at 1101001 = -16 dB



void init_mcbsp_spi()
{   EALLOW;
     // McBSP-B register settings
    McbspbRegs.SPCR2.all=0x0000;		 // Reset FS generator, sample rate generator & transmitter
	McbspbRegs.SPCR1.all=0x0000;		 // Reset Receiver, Right justify word, Digital loopback dis.
    McbspbRegs.PCR.all=0x0F08;           //(CLKXM=CLKRM=FSXM=FSRM= 1, FSXP = 1)
    McbspbRegs.SPCR2.bit.FREE = 1;
    McbspbRegs.SPCR2.bit.SOFT = 1;
    McbspbRegs.SPCR1.bit.CLKSTP = 2;     // Together with CLKXP/CLKRP determines clocking scheme
	McbspbRegs.PCR.bit.CLKXP = 1;		 // CPOL = 1, CPHA = 0 falling edge no delay
	McbspbRegs.PCR.bit.CLKRP = 0;
    McbspbRegs.RCR2.bit.RDATDLY=01;      // FSX setup time 1 in master mode. 0 for slave mode (Receive)
    McbspbRegs.XCR2.bit.XDATDLY=01;      // FSX setup time 1 in master mode. 0 for slave mode (Transmit)

	McbspbRegs.RCR1.bit.RWDLEN1=2;       // 16-bit word
    McbspbRegs.XCR1.bit.XWDLEN1=2;       // 16-bit word

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

void mcbsp_xmit(int a)
{   while (McbspbRegs.SPCR2.bit.XRDY == 0){}
    McbspbRegs.DXR1.all=a;
}

void aic23_init(int mic, int i2s_mode)
{
    Uint16 command;
    command = reset();
	mcbsp_xmit (command);
    command = softpowerdown();       // Power down everything except device and clocks
	mcbsp_xmit (command);
    command = linput_volctl(LIV);    // Unmute left line input and maintain default volume
	mcbsp_xmit (command);
	command = rinput_volctl(RIV);    // Unmute right line input and maintain default volume
	mcbsp_xmit (command);
	command = lhp_volctl(LHV);       // Left headphone volume control
	mcbsp_xmit (command);
	command = rhp_volctl(RHV);       // Right headphone volume control
	mcbsp_xmit (command);
    if (mic == 1) {
        command = aaudpath();
    } else {
   	    command = nomicaaudpath();      // Turn on DAC, mute mic
    }
	mcbsp_xmit (command);
	command = digaudiopath();       // Disable DAC mute
	mcbsp_xmit (command);

	if (i2s_mode == 1) {
	    command = I2Sdigaudinterface(); // AIC23 master mode, I2S mode,32-bit data, LRP=1 to match with XDATADLY=1
 	    mcbsp_xmit (command);
	    command = CLKsampleratecontrol (SR48); // 12.288 MHz external MCLK -  48 kHz sample rate (12.288 MHz/64/4 = 48 kHz)
	    mcbsp_xmit (command);
	} else {
 	    command = DSPdigaudinterface(); // AIC23 master mode, DSP mode,32-bit data, LRP=0 to match with XDATADLY=0
	    mcbsp_xmit (command);
	    command = USBsampleratecontrol (SR48); // 12 MHz USB clock -  48 kHz sample rate in USB mode
	    mcbsp_xmit (command);
    }
	command = digact();             // Activate digital interface
	mcbsp_xmit (command);
    if (mic == 1) {
        command = fullpowerup();
	} else {
   	    command = nomicpowerup();      // Turn everything on except Mic.
    }
    mcbsp_xmit (command);
}


//===========================================================================
// End of file.
//===========================================================================

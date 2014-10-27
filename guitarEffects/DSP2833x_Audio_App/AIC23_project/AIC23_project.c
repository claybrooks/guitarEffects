// TI File $Revision:  $
// Checkin $Date:  $
//###########################################################################
//
// FILE:	AIC23_project.c
//
// TITLE:	DSP2833x McBSP to AIC23 Audio Codec Interface via DMA
//
// ASSUMPTIONS:
//
//    This program requires the DSP2833x header files.
//    As supplied, this project is configured for "boot to SARAM"
//    operation.  The 2833x Boot Mode table is shown below.
//
//       $Boot_Table:
//
//         GPIO87   GPIO86     GPIO85   GPIO84
//          XA15     XA14       XA13     XA12
//           PU       PU         PU       PU
//        ==========================================
//            1        1          1        1    Jump to Flash
//            1        1          1        0    SCI-A boot
//            1        1          0        1    SPI-A boot
//            1        1          0        0    I2C-A boot
//            1        0          1        1    eCAN-A boot
//            1        0          1        0    McBSP-A boot
//            1        0          0        1    Jump to XINTF x16
//            1        0          0        0    Jump to XINTF x32
//            0        1          1        1    Jump to OTP
//            0        1          1        0    Parallel GPIO I/O boot
//            0        1          0        1    Parallel XINTF boot
//            0        1          0        0    Jump to SARAM	    <- "boot to SARAM"
//            0        0          1        1    Branch to check boot mode
//            0        0          1        0    Boot to flash, bypass ADC cal
//            0        0          0        1    Boot to SARAM, bypass ADC cal
//            0        0          0        0    Boot to SCI-A, bypass ADC cal
//                                              Boot_Table_End$
//
// DESCRIPTION:
//
// This program will transmit and receive audio data between the DSP2833x McBSP and
// the TLV320AIC23B Stereo Audio Codec configured for DSP or I2S mode.  The following connections
// are requred -
//
// EXTERNAL CONNECTIONS:
//
// Audio Connections
//
//  AIC23               McBSP-A
//----------------------------------------------
//  DIN         ---      MDXA
//  LRCIN       ---      MFSXA
//  BCLK        ---      MCLKXA/MCLKRA (short)
//  DOUT        ---      MDRA
//  LRCOUT      ---      MFSRA
//  Feed Line In from CD/DVD/MP3 headphone jack or audio out
//  Feed HPOUT or Line Out to headphones or speakers respectively
//
// Control Signals
//
//  AIC23               McBSP-B
//----------------------------------
//  SDIN        ---     MDXB
//  SCLK        ---     MCLKXB
//  CSn         ---     MFSXB
//
//
// -----------------------------------------------------------------------------------------
//
//
// * Prior to building and loading code, first select microphone and digital audio interface
// (DSP mode or I2S mode) options using #define MIC and I2S_SEL directives.
//
// Data is transferred as follows:
//
// 1. RRDY signal triggers DMA interrupt as soon as McBSP-A receiver is enabled.
//    - This first interrupt is serviced and ignored because no data has been
//      received yet.
// 2. 32-bits of L-channel data is received in DRR2->DRR1, which is moved by
//    DMA to buffer (ping or pong - starts with ping_buffer[1]).
//    (Remember 32-bit reads read larger address first, then
//    smaller address second - i.e. if buffer is at 0xD000, a 32-bit read of
//    ping_buffer[1] would read MSB in 0xD001, then LSB in 0xD000).
// 3. Then 32-bits of R-channel data is received in DRR2->DRR1, which is moved
//    by DMA to ping(or pong)_buffer[513] (Again, MSB in ping/pong_buffer[513],
//    LSB in ping/pong_buffer[512])
// 4. DMA wraps around - receives L-channel data again, and places it in next 32-bit
//    buffer address. Likewise R-channel is received and data is written to next
//    buffer address (same as Step 3)
// 5. After 512 sets of 32-bit L and R-channel data have been received (1024 sets of
//    16-bit data), DMA channel 1 interrupts indicating buffer is filled.
// 6. Process data in buffer. Then kick off DMA channel 2 here to transmit data
//    from the same buffer from which data was received.
// 7. Change DMA destination address to pong_buff_offset if ping_buff_offset was
//    previous destination address and ping_buff_offset if pong_buff_offset was
//    previous destination address. As soon as interrupt is acknowledged, DMA
//    channel 1 will start receiving from the other buffer while DMA channel 2 is
//    transmitting the first buffer.
// 8. On DMA Channel 2, the transmit is opposite of receive (i.e. source = ping/
//    pong buffer offset destination = McBSP DXR2/DXR1 registers).
//    L/R-channel data moves from buffer to DXR2/DXR1 registers to be tx'ed to
//    AIC23. Then when complete, switch to other ping/pong buffer.
// 9. Steps 2-8 are repeated continuously via interrupts.
//
// Watch Variables:
//        ping_buffer (buffer of 1024 Uint32 values - audio data)
//        pong_buffer (buffer of 1024 Uint32 values - audio data)
//
//
//###########################################################################
//
// Original Author: C. Peng
//
// $TI Release:  $
// $Release Date:  $
//###########################################################################

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File

// AIC23_DSP_SPI_control.c External Function Prototypes
extern void init_mcbsp_spi();
extern void mcbsp_xmit (int a);
extern void aic23_init(int mic, int i2s_mode);

// Prototype statements for functions found within this file.
void init_dma(void);
void init_mcbspa(void);
interrupt void local_D_INTCH1_ISR(void); // Channel 1 Rx ISR
interrupt void local_D_INTCH2_ISR(void); // Channel 2 Tx ISR

//===============================================================
// SELECT AUDIO INPUT AND DIGITAL AUDIO INTERFACE OPTIONS HERE:
//===============================================================

#define MIC 0      // 0 = line input, 1 = microphone input
#define I2S_SEL 1 // 0 = normal DSP McBSP dig. interface, 1 = I2S interface

//===============================================================


// 2 buffers, PING & PONG are used. When PING is being filled up, PONG data is processed and vice versa.
// L & R channel data is stored contigously in PING. Likewise in PONG.
// In both buffers, First 512 32-bit locations are L-channel data. Next 512 32-bit locations are R-channel data.

#pragma DATA_SECTION (ping_buffer, "DMARAML5"); // Place ping and pong in DMA RAM L5
#pragma DATA_SECTION (pong_buffer, "DMARAML5");

Uint32 ping_buffer[1024];  	// Note that Uint32 is used, not Uint16
Uint32 pong_buffer[1024];
Uint32 * L_channel = &ping_buffer[0];	// This pointer points to the beginning of the L-C data in either of the buffers
Uint32 * R_channel = &ping_buffer[512];	// This pointer points to the beginning of the R-C data in either of the buffers
Uint32 ping_buff_offset = (Uint32) &ping_buffer[0];
Uint32 pong_buff_offset = (Uint32) &pong_buffer[0];


Uint16 first_interrupt = 1;   // 1 indicates first interrupt
Uint32 k = 0;

void main(void)
{


	EALLOW;

// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
    InitSysCtrl();
// Step 2. Initalize GPIO:
// For this example, enable the GPIO PINS for McBSP operation.

	InitMcbspaGpio();
	InitMcbspbGpio();
	//EPWM clocking
					InitEPwm1Gpio();
					EALLOW;
					SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
					EPwm1Regs.TBPRD = 6;           // Set timer period 801 TBCLKs
					EPwm1Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
					EPwm1Regs.TBCTR = 0x0000;                      // Clear counter
					// Set Compare values
					EPwm1Regs.CMPA.half.CMPA = 3;     // Set compare A value
					// Setup counter mode
					EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
					EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
					EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;
					// Setup shadowing
					EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
					EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;  // Load on Zero
					// Set actions
					EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;             // Set PWM1A on event A, up count
					EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR;           // Clear PWM1A on event A, down count

					EALLOW;
					   SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;






/* Fill the buffer with dummy data */

	for(k=0; k<1024; k++) { ping_buffer[k] = 0x0; }
	for(k=0; k<1024; k++) { pong_buffer[k] = 0x0; }


// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts
   DINT;

// Initialize PIE control registers to their default state.
// The default state is all PIE interrupts disabled and flags
// are cleared.
// This function is found in the DSP2833x_PieCtrl.c file.
   InitPieCtrl();
   EALLOW;
   DINT;			// Disable interrupts again (for now)

// Disable CPU interrupts and clear all CPU interrupt flags:
   IER = 0x0000;
   IFR = 0x0000;

// Step 4. Initialize the Peripherals

	ping_buff_offset++;    // Start at location 1 (32-bit r/w from loc. 1, then 0)
	pong_buff_offset++;    // Start at location 1 (32-bit r/w from loc. 1, then 0)
    init_mcbsp_spi();       // Initialize McBSP-B as SPI Control
    aic23_init(MIC, I2S_SEL);  // Set up AIC23 w/ McBSP-B

	init_dma();				// Initialize the DMA before McBSP, so that DMA is ready to transfer the McBSP data
   	init_mcbspa();      	// Initalize McBSP-A

    delay_loop();

    EALLOW;
    DmaRegs.CH1.CONTROL.bit.RUN = 1; // Start rx on Channel 1

/* Reassign ISRS*/


	PieVectTable.DINTCH1 = &local_D_INTCH1_ISR;
    PieVectTable.DINTCH2 = &local_D_INTCH2_ISR;

/* Configure PIE interrupts*/

	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;  // Enable vector fetching from PIE block
	PieCtrlRegs.PIEACK.all = 0xFFFF;    // Enables PIE to drive a pulse into the CPU

// The interrupt can be asserted in the following interrupt lines


	PieCtrlRegs.PIEIER7.bit.INTx1 = 1;	// Enable INTx.1 of INT7 (DMA CH1)
    PieCtrlRegs.PIEIER7.bit.INTx2 = 1;  // Enable INTx.2 of INT7 (DMA CH2)

/* Configure system interrupts*/

	IER |= 0x0040;					    // Enable  INT7
    EINT;      					        // Global enable of interrupts
    EDIS;

/* Wait for data */

  	while(1) {
  	}						// Code loops here all the time
}

//===========================================================================
// End of main()
//===========================================================================

// INT7.1 -
interrupt void local_D_INTCH1_ISR(void)		// DMA Ch1 - McBSP-A Rx
{
    EALLOW;

    if(first_interrupt==1) // No processing needs to be done (B/c interrupt occurs
	{                      // at beginning of DMA transfers to ping buffer - no data received yet)
	    first_interrupt=0; // Turn flag off and exit interrupt
	} else
	{
	// Do whatever processing is needed on the buffered data here
  	// Once that is done, switch to the other buffer
	    DmaRegs.CH2.CONTROL.bit.RUN = 1; // Start tx on CH2 after CH1 has finished ping buffer
	}

    // When DMA first starts working on ping buffer, set the shadow registers
    //   to start at pong buffer next time and vice versa
    if(DmaRegs.CH1.DST_ADDR_SHADOW == ping_buff_offset)
	{
		DmaRegs.CH1.DST_ADDR_SHADOW = pong_buff_offset;
  		DmaRegs.CH1.DST_BEG_ADDR_SHADOW = pong_buff_offset;

	}
	else
	{
		DmaRegs.CH1.DST_ADDR_SHADOW = ping_buff_offset;
  		DmaRegs.CH1.DST_BEG_ADDR_SHADOW = ping_buff_offset;
	}
// To receive more interrupts from this PIE group, acknowledge this interrupt
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;
    EDIS;
}

// INT7.2
interrupt void local_D_INTCH2_ISR(void)		// DMA Ch2 - McBSP-A Tx
{
    EALLOW;
	// When DMA first starts working on ping buffer, set the shadow registers
    //   to start at pong buffer next time and vice versa
   	if(DmaRegs.CH2.SRC_ADDR_SHADOW == ping_buff_offset)
	{
	    DmaRegs.CH2.SRC_ADDR_SHADOW = pong_buff_offset;
	    DmaRegs.CH2.SRC_BEG_ADDR_SHADOW = pong_buff_offset;
	}
	else
	{
	    DmaRegs.CH2.SRC_ADDR_SHADOW = ping_buff_offset;
	    DmaRegs.CH2.SRC_BEG_ADDR_SHADOW = ping_buff_offset;
	}

   	PieCtrlRegs.PIEACK.all = PIEACK_GROUP7; // To receive more interrupts from this PIE group, acknowledge this interrupt
    EDIS;

}
void init_dma()
{
  EALLOW;
  DmaRegs.DMACTRL.bit.HARDRESET = 1;
  asm("     NOP");

  DmaRegs.PRIORITYCTRL1.bit.CH1PRIORITY = 0;

  /* DMA Channel 1 - McBSP-A Receive */
  DmaRegs.CH1.BURST_SIZE.all = 1;	// 2 16-bit words/burst (1 32-bit word) - memory address bumped up by 1 internally
  DmaRegs.CH1.SRC_BURST_STEP = 1;	// DRR2 must be read first & then DRR1. Increment by 1. Hence a value of +1. (This is a 2's C #)
  DmaRegs.CH1.DST_BURST_STEP = -1;	// Copy DRR2 data to address N+1 and DRR1 data to N. Hence -1 (32-bit read= read addr N+1 as MSB, then N as LSB)
  DmaRegs.CH1.TRANSFER_SIZE = 1023;	// DMA Interrupt every 1024 (n+1) bursts (1024 32-bit words).

  DmaRegs.CH1.SRC_TRANSFER_STEP = -1; // Decrement source address by 1 (from DRR1 back to DRR2) after processing a burst of data
  DmaRegs.CH1.DST_TRANSFER_STEP = 1025; // After copying 1 32-bit word of L-C data (1 burst), move down to R-C data in a given buffer

  DmaRegs.CH1.SRC_ADDR_SHADOW = (Uint32) &McbspaRegs.DRR2.all;  // First read from DRR2
  DmaRegs.CH1.SRC_BEG_ADDR_SHADOW = (Uint32) &McbspaRegs.DRR2.all;
  DmaRegs.CH1.DST_ADDR_SHADOW = ping_buff_offset;               // First write to ping_buffer[1]
  DmaRegs.CH1.DST_BEG_ADDR_SHADOW = ping_buff_offset;

  DmaRegs.CH1.DST_WRAP_SIZE = 1;	  // After LEFT(1) and then RIGHT(2), go back to LEFT buffer
  DmaRegs.CH1.SRC_WRAP_SIZE = 0xFFFF; // Arbitary large value. We'll never hit this.....
  DmaRegs.CH1.DST_WRAP_STEP = 2;      // From starting address, move down 2 16-bit addresses to write nxt 32-bit word

  DmaRegs.CH1.CONTROL.bit.PERINTCLR = 1;
  DmaRegs.CH1.CONTROL.bit.SYNCCLR = 1;
  DmaRegs.CH1.CONTROL.bit.ERRCLR = 1;

  DmaRegs.CH1.MODE.bit.CHINTE = 1;          // Enable DMA channel interrupts
  DmaRegs.CH1.MODE.bit.CHINTMODE = 0;       // Interrupt at beginning of transfer
  DmaRegs.CH1.MODE.bit.PERINTSEL = 15;		// McBSP MREVTA
  DmaRegs.CH1.MODE.bit.CONTINUOUS = 1;      // Enable continuous mode (continuously receives)// Enable interrupts from peripheral (to trigger DMA)
  DmaRegs.CH1.MODE.bit.PERINTE = 1;         // Enable interrupts from peripheral (to trigger DMA)



  /* DMA Channel 2 - McBSP-A Transmit */
  DmaRegs.CH2.BURST_SIZE.all = 1;	// 2 16-bit words/burst (1 32-bit word) - value bumped up by 1 internally
  DmaRegs.CH2.SRC_BURST_STEP = -1;	// Copy data at address N+1 to DXR2 first then data at N to DXR1. Hence -1
  DmaRegs.CH2.DST_BURST_STEP = 1;	// DXR2 must be written to first & then DXR1. Increment by 1. Hence a value of +1. (This is a 2's C #)
  DmaRegs.CH2.TRANSFER_SIZE = 1023;	// DMA Interrupt every 1024 (n+1) 16-bit words. McBSP still handles 16-bit data only in registers

  DmaRegs.CH2.SRC_TRANSFER_STEP = 1025; // After copying 1 32-bit word L-C data, move down to R-C data in a given buffer
  DmaRegs.CH2.DST_TRANSFER_STEP = -1;   // Decrement dest. address by 1 (DXR1 back to DXR2) after processing a burst of data

  DmaRegs.CH2.SRC_ADDR_SHADOW = ping_buff_offset;               // First read from ping_buffer[1]
  DmaRegs.CH2.SRC_BEG_ADDR_SHADOW = ping_buff_offset;
  DmaRegs.CH2.DST_ADDR_SHADOW = (Uint32) &McbspaRegs.DXR2.all;  // First write to DXR2
  DmaRegs.CH2.DST_BEG_ADDR_SHADOW = (Uint32) &McbspaRegs.DXR2.all;

  DmaRegs.CH2.SRC_WRAP_SIZE = 1;	     // After LEFT(1) and then RIGHT(2), go back to LEFT buffer
  DmaRegs.CH2.DST_WRAP_SIZE = 0xFFFF;  	 // Arbitary large value. We'll never hit this.....
  DmaRegs.CH2.SRC_WRAP_STEP = 2;         // From starting address, move down 2 16-bit addresses to read next 32-bit word

  DmaRegs.CH2.CONTROL.bit.PERINTCLR = 1;
  DmaRegs.CH2.CONTROL.bit.SYNCCLR = 1;
  DmaRegs.CH2.CONTROL.bit.ERRCLR = 1;


  DmaRegs.CH2.MODE.bit.CHINTE = 1;          // Enable DMA channel interrupts
  DmaRegs.CH2.MODE.bit.CHINTMODE = 0;       // Interrupt at beginning of transfer
  DmaRegs.CH2.MODE.bit.PERINTSEL = 14;		// McBSP MXEVTA
  DmaRegs.CH2.MODE.bit.CONTINUOUS = 1;      // Enable continuous mode (continuously transmits)
  DmaRegs.CH2.MODE.bit.PERINTE = 1;         // Enable interrupts from peripheral (to trigger DMA)

}



void init_mcbspa()
{
    EALLOW;
    McbspaRegs.SPCR2.all=0x0000;		// Reset FS generator, sample rate generator & transmitter
	McbspaRegs.SPCR1.all=0x0000;		// Reset Receiver, Right justify word

    McbspaRegs.SPCR1.bit.RJUST = 2;		// left-justify word in DRR and zero-fill LSBs

 	McbspaRegs.MFFINT.all=0x0;			// Disable all interrupts

    McbspaRegs.SPCR1.bit.RINTM = 0;		// McBSP interrupt flag to DMA - RRDY
	McbspaRegs.SPCR2.bit.XINTM = 0;     // McBSP interrupt flag to DMA - XRDY

    McbspaRegs.RCR2.all=0x0;			// Clear Receive Control Registers	
    McbspaRegs.RCR1.all=0x0;

    McbspaRegs.XCR2.all=0x0;			// Clear Transmit Control Registers
    McbspaRegs.XCR1.all=0x0;

    McbspaRegs.RCR2.bit.RWDLEN2 = 5;	// 32-BIT OPERATION
    McbspaRegs.RCR1.bit.RWDLEN1 = 5;
    McbspaRegs.XCR2.bit.XWDLEN2 = 5;
    McbspaRegs.XCR1.bit.XWDLEN1 = 5;

    McbspaRegs.RCR2.bit.RPHASE = 1;		// Dual-phase frame
	McbspaRegs.RCR2.bit.RFRLEN2 = 0;	// Recv frame length = 1 word in phase2
	McbspaRegs.RCR1.bit.RFRLEN1 = 0;	// Recv frame length = 1 word in phase1

	McbspaRegs.XCR2.bit.XPHASE = 1;		// Dual-phase frame
	McbspaRegs.XCR2.bit.XFRLEN2 = 0;	// Xmit frame length = 1 word in phase2
	McbspaRegs.XCR1.bit.XFRLEN1 = 0;	// Xmit frame length = 1 word in phase1

	McbspaRegs.RCR2.bit.RDATDLY = 1;	// n = n-bit data delay, in DSP mode, X/RDATDLY=0
	McbspaRegs.XCR2.bit.XDATDLY = 1;    // DSP mode: If LRP (AIC23) = 0, X/RDATDLY=0, if LRP=1, X/RDATDLY=1
	                                    // I2S mode: R/XDATDLY = 1 always

    McbspaRegs.SRGR1.all=0x0001;		// Frame Width = 1 CLKG period, CLKGDV must be 1 as slave
                                        // SRG clocked by LSPCLK - SRG clock MUST be at least 2x external data shift clk

    McbspaRegs.PCR.all=0x0000;			// Frame sync generated externally, CLKX/CLKR driven
    McbspaRegs.PCR.bit.FSXM = 0;		// FSX is always an i/p signal
	McbspaRegs.PCR.bit.FSRM = 0;		// FSR is always an i/p signal
	McbspaRegs.PCR.bit.SCLKME = 0;

#if I2S_SEL                             // In I2S mode:
    McbspaRegs.PCR.bit.FSRP = 1;		// 1-FSRP is active low (L-channel first)
	McbspaRegs.PCR.bit.FSXP = 1 ;       // 1-FSXP is active low (L-channel first)
#else                                   // In normal DSP McBSP mode:
    McbspaRegs.PCR.bit.FSRP = 0;		// 0-FSRP is active high (data rx'd from rising edge)
	McbspaRegs.PCR.bit.FSXP = 0 ;       // 0-FSXP is active high (data tx'd from rising edge)
#endif

    McbspaRegs.PCR.bit.CLKRP  = 1;		// 1-Rcvd data sampled on rising edge of CLKR
	McbspaRegs.PCR.bit.CLKXP  = 0;      // 0- Tx data sampled on falling edge of CLKX
	McbspaRegs.SRGR2.bit.CLKSM = 1;		// LSPCLK is clock source for SRG

	McbspaRegs.PCR.bit.CLKXM = 0;		// 0-MCLKXA is an i/p driven by an external clock
    McbspaRegs.PCR.bit.CLKRM = 0;		// MCLKRA is an i/p signal

    McbspaRegs.SPCR2.all |=0x00C0;     	// Frame sync & sample rate generators pulled out of reset
    delay_loop();
	McbspaRegs.SPCR2.bit.XRST=1;       	// Enable Transmitter
    McbspaRegs.SPCR1.bit.RRST=1;		// Enable Receiver

    EDIS;
}



//===========================================================================
// End of file.
//===========================================================================


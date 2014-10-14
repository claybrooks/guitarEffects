#include "../include/initialize.h"
#include "../include/spi.h"
#include "../include/lcd.h"
#include "../include/adc.h"
#include "../include/spi.h"
#include "../include/effect.h"
#include "../include/PWM.h"
//#include "../include/fft.h"
#include "../include/sys.h"
#include "DSP28x_Project.h"
#include "AIC23.h"
void initPWM();
interrupt void epwm6_isr(void);
interrupt void Mcbspb_rx_isr(void);
interrupt void Mcbspb_tx_isr(void);
int indexLookup(int);
int LRCIN = 0;
int tuner = 0,current = 0, resetTimer = 0, sysStart = 1, reverbLevel = 0, tremoloLevel = 0, reverbChange, tremoloChange;
int adcVals[2];
int main(){
		InitSysCtrl();
		EALLOW;
	//initialize();
			InitEPwm4Gpio();
			InitEPwm5Gpio();
			InitEPwm6Gpio();

			EALLOW;
			SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
			EDIS;
			EALLOW;
				// Setup BitCLK
				EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
				EPwm4Regs.TBPRD =56 ;       // Set timer period
				EPwm4Regs.TBCTR = 0x0000;                  // Clear counter
				EPwm4Regs.TBCTL.bit.HSPCLKDIV = 0x0;   // Clock ratio to SYSCLKOUT
				EPwm4Regs.TBCTL.bit.CLKDIV = 0x0;

				// Setup SYSCLK
				EPwm5Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
				EPwm5Regs.TBPRD =7 ;       // Set timer period
				EPwm5Regs.TBCTR = 0x0000;                  // Clear counter
				EPwm5Regs.TBCTL.bit.HSPCLKDIV = 0x0;   // Clock ratio to SYSCLKOUT
				EPwm5Regs.TBCTL.bit.CLKDIV = 0x0;

				// Setup LRCIn
				EPwm6Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
				EPwm6Regs.TBPRD = 1792 ;       // Set timer period
				EPwm6Regs.TBCTR = 0x0000;                  // Clear counter
				EPwm6Regs.TBCTL.bit.HSPCLKDIV = 0x0;   // Clock ratio to SYSCLKOUT
				EPwm6Regs.TBCTL.bit.CLKDIV = 0x0;

				// Setup shadow register load on ZERO
				EPwm4Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
				EPwm5Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
				EPwm6Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;

				// Set Compare values
				EPwm4Regs.CMPA.half.CMPA = 28; // Set compare A value
				EPwm5Regs.CMPA.half.CMPA = 3; // Set compare A value
				EPwm6Regs.CMPA.half.CMPA = 896; // Set compare A value

				// Set Actions
				EPwm4Regs.AQCTLA.bit.CAU = AQ_SET;
				EPwm4Regs.AQCTLA.bit.CAD = AQ_CLEAR;
				EPwm5Regs.AQCTLA.bit.CAU = AQ_SET;
				EPwm5Regs.AQCTLA.bit.CAD = AQ_CLEAR;
				EPwm6Regs.AQCTLA.bit.CAU = AQ_SET;
				EPwm6Regs.AQCTLA.bit.CAD = AQ_CLEAR;

				EALLOW;
				SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
				EDIS;

			//Reset Codec
			GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 0;
			GpioCtrlRegs.GPADIR.bit.GPIO14 = 1;
			GpioDataRegs.GPADAT.bit.GPIO14 = 0;
			DELAY_US(1);
			GpioDataRegs.GPADAT.bit.GPIO14 = 1;

			InitMcbspbGpio();

				//Recieve Registers
				McbspbRegs.RCR1.bit.RWDLEN1 = 0x2;	//16 bit word
				McbspbRegs.RCR2.bit.RPHASE = 0x1;	//Dual-phase frame
				McbspbRegs.RCR1.bit.RFRLEN1 = 0x0;	//1 word in phase1;
				McbspbRegs.RCR2.bit.RFRLEN2 = 0x0;	//1 word in phase2
				McbspbRegs.RCR2.bit.RCOMPAND = 0x0;	//No companding
				McbspbRegs.RCR2.bit.RDATDLY = 0x0;	//0 clock data-delay

				//Transmit Registers
				McbspbRegs.XCR1.bit.XWDLEN1 = 0x2;	//16 bit word
				McbspbRegs.XCR2.bit.XPHASE = 0x1;	//Dual-phase frame
				McbspbRegs.XCR1.bit.XFRLEN1 = 0x0;	//1 word in phase1;
				McbspbRegs.XCR2.bit.XFRLEN2 = 0x0;	//1 word in phase2
				McbspbRegs.XCR2.bit.XCOMPAND = 0x0;	//No companding
				McbspbRegs.XCR2.bit.XDATDLY = 0x0;	//0 clock data-delay

				//Clock Generation
				McbspbRegs.PCR.bit.FSXM = 0;		//Transmit frame-sync generated externally
				McbspbRegs.PCR.bit.FSRM = 0;		//Recieve frame-sync generated externally
				McbspbRegs.PCR.bit.CLKXM = 0;		//Transmit clock is generated externally
				McbspbRegs.PCR.bit.CLKRM = 0;		//Receive clock is generated externally
				McbspbRegs.PCR.bit.SCLKME = 1;		//MCLKX is used
				McbspbRegs.SRGR2.bit.CLKSM = 1;		//MCLKX is used
				McbspbRegs.PCR.bit.FSXP = 0;		//Active high transmit frame-sync pulse
				McbspbRegs.PCR.bit.FSRP = 0;		//Active high recive frame-sync pulse
				McbspbRegs.PCR.bit.CLKXP = 0;		//Data is clocked out on the rising edge of SCK
				McbspbRegs.PCR.bit.CLKRP = 0;		//Data is sampled in on the falling edge of SCK
				linput_volctl(4);
			//************ Enable TX/RX unit
			McbspbRegs.SPCR2.bit.XRST=1;
			McbspbRegs.SPCR1.bit.RRST=1;

			InitPieCtrl();
			IER = 0x0000;
			IFR = 0x0000;
			InitPieVectTable();
			EALLOW;
		    PieVectTable.MRINTB = &Mcbspb_rx_isr;
		    PieVectTable.MRINTB = &Mcbspb_tx_isr;
			McbspbRegs.MFFINT.bit.RINT = 1; 	 // Enable Receive Interrupts
			McbspbRegs.MFFINT.bit.XINT = 1; 	 // Enable Receive Interrupts
			PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   // Enable the PIE block
			PieCtrlRegs.PIEIER6.bit.INTx3=1;     // Enable PIE Group 6, INT 3
			PieCtrlRegs.PIEIER6.bit.INTx4=1;     // Enable PIE Group 6, INT 4

			IER |= M_INT6;                            // Enable CPU INT6

			EINT;   // Enable Global interrupt INTM
			ERTM;   // Enable Global realtime interrupt DBGM

	while(1){
		//Wait for interrupts
	}
}

interrupt void Mcbspb_rx_isr(void){
	 PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;
}
interrupt void Mcbspb_tx_isr(void){
	 PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;
}


interrupt void spiRx_isr(void){

	int sample = readSPI();

	//Acknowledge Interrupt
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

interrupt void cpu_timer0_isr(void){
		PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

interrupt void xint1_isr(void){
	/*//Get user input, shift for clearer comparisons
	int input = (GpioDataRegs.GPADAT.all & 0x000FF00) >> 8;

	//Takes care of weird calls to xint1_isr with no button push
	if(input!= 0){
		//Clear pipeline of all effects/ clear screen
		if(input == 0x0080){
			clearPipeline();
			updateLCD(CLEAR);
		}

		//Switch to tuning function
		else if(input == 0x0040){
			tuner ^= 1;			//signal for timer0 to not sample out to SPI
			updateLCD(TUNER);	//Update LCD to tuner screen
			if(tuner) updateTimer0(1000);	//Slower sample rate for FFT analysis = Higher bin resolution
			else updateTimer0(22.675f);		//FFT was toggled off, switch back to sample out to SPI
		}

		//Look to either queue effect or toggle state
		else{
			//Simple lookup vs mathematical computation gets the effect to be manipulated
			int effect = indexLookup(input);
			//toggleOn_Off returns 1 if it can be toggled, else 0 meaning its not in queue;
			if(!toggleOn_Off(effect)) queueEffect(effect);		//queue the effect
			updateLCD(effect);
		}
	}
	//Acknowledge Interrupt
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;*/

}
//Timeout counter for Preset selection
interrupt void cpu_timer1_isr(void){
	controlLCD(0x01);
	DELAY_US(1520);
	//Stop Timer
	CpuTimer1Regs.TCR.bit.TSS = 1;

	reverbChange = 0;
	tremoloChange = 0;
	current = 0;
	//Acknowledge Interrupt
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

interrupt void adc_isr(void){
	if(abs(adcVals[0] - (AdcRegs.ADCRESULT2 >> 4)) > 0x00C0){
		adcVals[0] = AdcRegs.ADCRESULT2>>4;

		if(!tremoloChange|| current != 1){
			current = 1;
			controlLCD(0x01);
			DELAY_US(1520);
			controlLCD(0xB8);
			printLCD(0x54);//T
			printLCD(0x72);//r
			printLCD(0x65);//e
			printLCD(0x6D);//m
			printLCD(0x6F);//o
			printLCD(0x6C);//l
			printLCD(0x6F);//o
			tremoloChange = 1;
			controlLCD(0x02); //Return cursor to home;
			DELAY_US(1520);
		}

		int temp = 16*((double)adcVals[0]/(double)0x0FFF);

		//controlLCD(0x01);
		controlLCD(0x02);//return home
		int i;
		for(i = 0; i < temp; i++){
			printLCD(0x7C);
		}
		if(temp < tremoloLevel){
			for(i = temp; i < tremoloLevel; i++){
				printLCD(0x20);
			}
		}
		tremoloLevel = temp;
		resetTimer = 1;

	}
	else if(abs(adcVals[1] - (AdcRegs.ADCRESULT3 >> 4)) > 0x00C0){
		adcVals[1] = AdcRegs.ADCRESULT3>>4;

		if(!reverbChange || current != 2){
			current = 2;
			controlLCD(0x01);
			DELAY_US(1520);
			controlLCD(0xB8);
			printLCD(0x52);
			printLCD(0x65);
			printLCD(0x76);
			printLCD(0x65);
			printLCD(0x72);
			printLCD(0x62);
			reverbChange = 1;
			controlLCD(0x02); //Return cursor to home;
			DELAY_US(1520);
		}
		int temp = 16*((double)adcVals[1]/(double)0x0FFF);
		controlLCD(0x02);//return home
		int i;
		for(i = 0; i < temp; i++){
			printLCD(0x7C);
		}
		if(temp < reverbLevel){
			for(i = temp; i < reverbLevel; i++){
				printLCD(0x20);
			}
		}
		reverbLevel = temp;
		resetTimer = 1;
	}


		if(resetTimer){
			CpuTimer1Regs.TCR.all = 0x4001;
			CpuTimer1Regs.TCR.bit.TRB = 1;
			resetTimer = 0;
		}

		//Clear Flags
	AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;       // Clear INT SEQ1 bit
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // Acknowledge interrupt to PIE

}


//Eliminates need to call log(input)/log(2).  Simple lookup will be faster
int indexLookup(int input){
	if(input == 1) return 0;
	else if(input == 2) return 1;
	else if(input == 4) return 2;
	else if(input == 8) return 3;
	else if(input == 16) return 4;
	else if(input == 32) return 5;
	else if(input == 64) return 6;
	else if(input == 128) return 7;
	else return 8;
}

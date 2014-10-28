#include "spi.h"
#include "lcd.h"
#include "adc.h"
#include "spi.h"
#include "effect.h"
#include "fft.h"
#include "sys.h"
#include "PWM.h"
#include "eeprom.h"
#include "DSP2833x_Mcbsp.h"

int indexLookup(int);

int tuner = 0, preset = 0, currentChangeScreen = 0, resetTimer = 0, sysStart = 1;
int volumeLevel = 0, bassLevel = 0, midLevel = 0, trebleLevel = 0, reverbLevel = 0, tremoloLevel = 0, delayLevel = 0, chorusLevel = 0,flangeLevel;
int bassChange = 0, volumeChange = 0, trebleChange = 0, midChange = 0, flangeChange = 0,delayChange = 0, chorusChange = 0, reverbChange = 0, tremoloChange = 0;
int updateLcd = 0, updateCode = 0, updateChange, newLevel = 0, oldLevel = 0;
int updateVolume = 0, newVolumeLevel = 0;
int adcVals[10];

int main(){
	InitSysCtrl();
	EALLOW;
	adcVals[0] = 0;
	adcVals[1] = 0;
	adcVals[2] = 0;
	adcVals[3] = 0;
	adcVals[4] = 0;
	adcVals[5] = 0;
	adcVals[6] = 0;
	adcVals[7] = 0;
	adcVals[8] = 0;
	adcVals[9] = 0;


	// For this example, enable the GPIO PINS for McBSP operation.
		InitMcbspbGpio();

	    EALLOW;


		//Initialize I2C
			InitI2CGpio();
			I2CA_Init();
		//Initialize ADC/DAC
			GpioCtrlRegs.GPADIR.bit.GPIO19 = 1;	//CONVST
			init_adc_spi();
			init_mcbsp_spi();
			mcbsp_xmit(0x38000100);
		//Initialize ADC
			initAdc();
		//Initialize Effects
			initEffects();
		//Initialize FFT
			//initFFT();
		//Initialize LCD
			//initLCD();
		//Initialize Interrupts
			initINTS();
	    EINT;      					        // Global enable of interrupts
	    EDIS;
	while(1){

		//Wait for interrupts
		if(updateLcd){
			//updateLCD(updateCode);
			updateLcd = 0;
		}
		if(updateChange){
		//	updateLevel(newLevel, oldLevel);
			updateChange = 0;
		}
	}
}

interrupt void cpu_timer0_isr(void){
	int sample = read_adc();	//Get sample from ADC
	sample = process(sample);	//Process sample
	write_dac(sample);			//write sample to DAC
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	//Clear flag to accept more interrupts
}

//Timeout counter for Preset selection
interrupt void cpu_timer1_isr(void){

	CpuTimer1Regs.TCR.bit.TSS = 1;

	//Return to homescreen
	updateLcd = 1;
	updateCode = PRESETTIMEOUT;

	//Reset flag bits that may have caused this
	preset = 0;
	bassChange = 0, volumeChange = 0, trebleChange = 0, midChange = 0, flangeChange = 0,delayChange = 0, chorusChange = 0, reverbChange = 0, tremoloChange = 0;
	currentChangeScreen = 0;

	//Acknowledge Interrupt
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

interrupt void adc_isr(void){
	/*Format for every if statement is the same, save for volume change because
	because a signal needs to be sent to update volume registers on codec
	Format:
		-Check to see if the voltage level changed a significant enough amount
		-Save the new amount into its respective index in the array
		-If this is the first attempt to change the respective level OR the screen is not on the respective
		 change screen, set the necessary signals to tell main while loop to udpate LCD to proper screen
		 currentChangeScreen = the current change screen you are on, updateLcd/updateCode are signals for main while loop,
		 respectiveChange blocks the reprinting of the respective LCD screen if the knob is continuously turned.
		-temp is the number of '|' characters to print to the screen depending on the current voltage level.
		-newLevel is the newly calculated temp level
		-oldLevel was the previous one.  This is needed to make sure the function that prints the '|' characters
		 properly prints the screen on every iteration
		-resetTimer resets the timer1 to start over counting down from 3 seconds.  This will cause a timeout signal
		 to be sent to the LCD after 3 seconds of no change on any ADC channel.
	*/
	//TREMOLO
	if(abs(adcVals[0] - (AdcRegs.ADCRESULT0 >> 4)) > 0x00F0){
		adcVals[0] = AdcRegs.ADCRESULT0 >> 4;
		if(!tremoloChange|| currentChangeScreen != 1){
			currentChangeScreen = 1;
			updateLcd = 1;
			updateCode = CHANGETREMOLO;
			tremoloChange = 1;
		}
		int temp = 16*((double)adcVals[0]/(double)0x0FFF);
		newLevel = temp;
		oldLevel = tremoloLevel;
		updateChange = 1;
		tremoloLevel = temp;
		resetTimer = 1;
	}
	//REVERB
	else if(abs(adcVals[1]- (AdcRegs.ADCRESULT1 >> 4)) > 0x00F0){
		adcVals[1] = AdcRegs.ADCRESULT1 >> 4;
		if(!reverbChange || currentChangeScreen != 2){
			currentChangeScreen = 2;
			updateLcd = 1;
			updateCode = CHANGEREVERB;
			reverbChange = 1;
		}
		int temp = 16*((double)adcVals[1]/(double)0x0FFF);
		newLevel = temp;
		oldLevel = reverbLevel;
		updateChange = 1;
		reverbLevel = temp;
		resetTimer = 1;
	}
	//VOLUME
	else if(abs(adcVals[2] - (AdcRegs.ADCRESULT2 >> 4)) > 0x00F0){
		adcVals[2] = AdcRegs.ADCRESULT2 >> 4;
		if(!volumeChange || currentChangeScreen != 3){
			currentChangeScreen = 3;
			updateLcd = 1;
			updateCode = CHANGEVOLUME;
			volumeChange = 1;
		}
		int temp = 16*((double)adcVals[2]/(double)0x0FFF);
		newLevel = temp;
		oldLevel = volumeLevel;
		updateChange = 1;
		newVolumeLevel = (((double)adcVals[2]/(double)0xFFF))*(0x1F);
		resetTimer = 1;
		updateVolume = 1;
	}
	//BASS
	else if(abs(adcVals[3] - (AdcRegs.ADCRESULT3 >> 4)) > 0x00F0){
		adcVals[3] = AdcRegs.ADCRESULT3 >> 4;
		if(!bassChange || currentChangeScreen != 4){
			currentChangeScreen = 4;
			updateLcd = 1;
			updateCode = CHANGEBASS;
			bassChange = 1;
		}
		int temp = 16*((double)adcVals[3]/(double)0x0FFF);
		newLevel = temp;
		oldLevel = bassLevel;
		updateChange = 1;
		bassLevel = temp;
		resetTimer = 1;
	}
	//MID
	else if(abs(adcVals[4] - (AdcRegs.ADCRESULT4 >> 4)) > 0x00F0){
			adcVals[4] = AdcRegs.ADCRESULT4 >> 4;
			if(!midChange || currentChangeScreen != 5){
				currentChangeScreen = 5;
				updateLcd = 1;
				updateCode = CHANGEMID;
				midChange = 1;
			}
			int temp = 16*((double)adcVals[4]/(double)0x0FFF);
			newLevel = temp;
			oldLevel = midLevel;
			updateChange = 1;
			midLevel = temp;
			resetTimer = 1;
	}
	//TREBLE
	else if(abs(adcVals[5] - (AdcRegs.ADCRESULT5 >> 4)) > 0x00F0){
			adcVals[5] = AdcRegs.ADCRESULT5 >> 4;
			if(!trebleChange || currentChangeScreen != 6){
				currentChangeScreen = 6;
				updateLcd = 1;
				updateCode = CHANGETREBLE;
				trebleChange = 1;
			}
			int temp = 16*((double)adcVals[5]/(double)0x0FFF);
			newLevel = temp;
			oldLevel = trebleLevel;
			updateChange = 1;
			trebleLevel = temp;
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

interrupt void xint1_isr(void){
	//Get user input, shift for clearer comparisons
	int input = (GpioDataRegs.GPADAT.all & 0x000001E) >> 1;

	//Scroll up through presets
	if(GpioDataRegs.GPADAT.bit.GPIO5){
		//If initial entry into preset, set up timer and flag
		if(!preset){
			CpuTimer1Regs.TCR.all = 0x4001;
			preset = 1;
		}
		CpuTimer1Regs.TCR.bit.TRB = 1;
		updateLcd = 1;
		updateCode = PRESETUP;
	}
	//Scroll down through presets
	else if(GpioDataRegs.GPADAT.bit.GPIO21){
		//If initial entry into preset, set up timer and flag
		if(!preset){
			CpuTimer1Regs.TCR.all = 0x4001;
			preset = 1;
		}
		CpuTimer1Regs.TCR.bit.TRB = 1;
		updateLcd = 1;
		updateCode = PRESETDOWN;
	}
	//Load preset
	else if(GpioDataRegs.GPADAT.bit.GPIO20){
		//Stop timer and reset flag
		CpuTimer1Regs.TCR.bit.TSS = 1;
		preset = 0;
		updateLcd = 1;
		updateCode = LOADPRESET;
	}
	//Save Preset
	else if(GpioDataRegs.GPADAT.bit.GPIO16){
		//Stop timer and reset flag
		CpuTimer1Regs.TCR.bit.TSS = 1;
		preset = 0;
		updateLcd = 1;
		updateCode = SAVEPRESET;
	}
		//Clear pipeline of all effects/ clear screen
	else if(input == 0x0008){
		updateLcd = 1;
		clearPipeline();
		updateCode = CLEAR;
	}

	//Switch to tuning function
	else if(input == 0x0040){
		tuner ^= 1;			//signal for timer0 to not sample out to SPI
		updateLcd = 1;
		updateCode = TUNER;
		if(tuner) updateTimer0(1000);	//Slower sample rate for FFT analysis = Higher bin resolution
		else updateTimer0(22.675f);		//FFT was toggled off, switch back to sample out to SPI
	}

	//Look to either queue effect or toggle state
	else{
		//Simple lookup vs mathematical computation gets the effect to be manipulated
		int effect = indexLookup(input);
		//toggleOn_Off returns 1 if it can be toggled, else 0 meaning its not in queue;
		if(!toggleOn_Off(effect)) queueEffect(effect);		//queue the effect
		updateLcd = 1;
		updateCode = effect;
	}

	//Acknowledge Interrupt
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
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

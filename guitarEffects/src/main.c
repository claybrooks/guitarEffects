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
#include "F28335_example.h"

void getPot();
int hungry = 0;

/*********************************************************************************************************************************************************************
 * Effect variables/functions
 */
int toggleOn_Off(int effect);
void queueEffect(int effect);
void clearPipeline();
int indexLookup(int);
//typedef int FUNC(int, struct params*);
//Struct for all the parameters available to effects, passed into the process functions
static struct params params;

//Create FUNC variables
FUNC processTremolo,processReverb,processCrunch,processDelay,processWah,processPhaser,processFlange,processReverb,processChorus,processPitchShift;

//Static list of available effects, GPIO must match this
FUNC *list[10] = {processTremolo,processReverb,processDistortion,processCrunch,processDelay,processReverb,processPhaser,processFlange,processChorus,processPitchShift};

/*The indices of this array map  directly to the *list array.  This location array holds the location of the effect in the pipeline array.
 * Index 0 of the location array maps to index 0 of the list array.  But the data at index 0 of the location arary points to
 * where that effect is located in the pipeline array. Increase in mem usage for speed gain, don't have to iterate through *list to find
 * the right effect.*/
int location[10];
//Array of FUNC's, this is the queue set by the user.
FUNC *pipeline[10];
//lowpassType* lowpass;

/*********************************************************************************************************************************************************************/

/*********************************************************************************************************************************************************************
 * LCD controller variables/functions
 */
int 	toggleDisplayOn_Off(int);	//Toggles effect on/off once its in the queue
void	queueDisplay(int);	//Sticks effect into queue
int	toggleEffectOnDisplay(int);	//Sticks effect into queue
//LCD Display Queue
int mainDisplay[10];
int toggle = 0, effectToToggle = 0, indexToToggle = 0;
int tuner = 0, preset = 0, currentChangeScreen = 0, resetTimer = 0, sysStart = 1;
int updateLcd = 0, updateCode = 0, updateChange, newLevel = 0, oldLevel = 0;
/********************************************************************************************************************************************************************/

/*********************************************************************************************************************************************************************
 * Shared Variables between LCD controller and effects
 */
//State of each queued effect, this allows user to stomp effects on/off without losing location in queue
//Indices of this array map direclty to indices of pipeline.  LCD will use this to know what to display
int on_off[10];
//Number of queued effects
int numQueued;
/********************************************************************************************************************************************************************/

/********************************************************************************************************************************************************************
 * ADC ISR variables
 */
int volumeLevel = 0, bassLevel = 0, midLevel = 0, trebleLevel = 0, reverbLevel = 0, tremoloLevel = 0, delayLevel = 0, chorusLevel = 0,flangeLevel;
int bassChange = 0, volumeChange = 0, trebleChange = 0, midChange = 0, flangeChange = 0,delayChange = 0, chorusChange = 0, reverbChange = 0, tremoloChange = 0;
int updateVolume = 0, newVolumeLevel = 0;
int adcVals[10];
/********************************************************************************************************************************************************************/

/**********************************************************************************************************************************************************************
 * Preset Variables
 */
int load = 0, save = 0, presetNumber = 1;
/********************************************************************************************************************************************************************/

#define DEBOUNCE 50000

#pragma CODE_SECTION(cpu_timer0_isr, "secureRamFuncs")
#pragma CODE_SECTION(cpu_timer1_isr, "secureRamFuncs")
#pragma CODE_SECTION(getPot, "secureRamFuncs")
#pragma CODE_SECTION(preset_up, "secureRamFuncs")
#pragma CODE_SECTION(preset_down, "secureRamFuncs")
#pragma CODE_SECTION(load_preset, "secureRamFuncs")
#pragma CODE_SECTION(save_preset, "secureRamFuncs")
#pragma CODE_SECTION(effects, "secureRamFuncs")

A int main(){
	InitSysCtrl();

	memcpy(&secureRamFuncs_runstart, &secureRamFuncs_loadstart, (Uint32)&secureRamFuncs_loadsize);
	InitFlash();

	EALLOW;

	// For this example, enable the GPIO PINS for McBSP operation.
		InitMcbspbGpio();

	    EALLOW;

	    //GPIO for muxing

		//Initialize I2C
			InitI2CGpio();
			I2CA_Init();
		//Initialize ADC/DAC
			init_mcbsp_spi();
			mcbsp_xmit(0x38000100);
			GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;
			GpioCtrlRegs.GPADIR.bit.GPIO19 = 1;	//CONVST
			init_adc_spi();
		//Initialize ADC
			initAdc();
		//Initialize Effects
			initEffects(&params);
		//Initialize FFT
			//initFFT();
		//Initialize LCD
			initLCD();
		//Initialize Interrupts
			initINTS();
			int i = 0;
			for(;i < 10; i++){
				adcVals[i] = 0;
				mainDisplay[i] = -1;
			}
			//save to initialize eeprom, i have no idea why i have to do this
			savePreset(15, location, on_off);
			loadPreset(15, pipeline, list, location, on_off, &numQueued);
			EALLOW;
	while(1){
		//if(hungry >= 1000){
		//	getPot();
		//	hungry = 0;
		//}
		//Wait for signals
		//Toggle LCD screen
		if(toggle){
			toggle = 0;
			toggleLCD(effectToToggle,indexToToggle,on_off[indexToToggle], numQueued);
		}
		//Print lcd screen with signals generated from ISR
		if(updateLcd){
			updateLCD(&updateCode, mainDisplay, on_off, &presetNumber, &numQueued);
			updateLcd = 0;
		}
		//Print level of change from pot input
		if(updateChange){
			updateLevel(newLevel, oldLevel);
			updateChange = 0;
		}
		//Load preset in effect and lcd controller
		if(load){
			loadPreset(presetNumber, pipeline, list, location, on_off, &numQueued);
			loadPresetScreen(location,mainDisplay, &numQueued);
			updateCode = MAIN;
			updateLcd = 1;
			load = 0;
		}
		//Save preset in effect controller
		if(save){
			savePreset(presetNumber, location, on_off);
			save = 0;
		}
	}
}

interrupt void cpu_timer0_isr(void){
	EALLOW;
	//hungry++;
	//if(GpioDataRegs.GPADAT.bit.GPIO6 == 1)GpioDataRegs.GPADAT.bit.GPIO6 = 0;
	//else GpioDataRegs.GPADAT.bit.GPIO6 = 1;
	//GpioDataRegs.GPADAT.bit.GPIO20 = 1;
	//GpioDataRegs.GPATOGGLE.bit.GPIO22 = 1;
	int sample = read_adc();	//Get sample from ADC
	//int sample = AdcRegs.ADCRESULT2;// >> 4;
	sample = process(sample,numQueued, on_off,&pipeline[0],&params);	//Process sample
	write_dac(sample);			//write sample to DAC
	//GpioDataRegs.GPADAT.bit.GPIO20 = 0;
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	//Clear flag to accept more interrupts
}

//Timeout counter for Preset selection
interrupt void cpu_timer1_isr(void){

	GpioDataRegs.GPADAT.bit.GPIO1 = 1;
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

//interrupt void adc_isr(void){
void getPot(){
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
	//AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;       // Clear INT SEQ1 bit
	AdcRegs.ADCTRL2.all |= 0x2000;			// Start SEQ1
	//TREMOLO

	if(!preset){
		if(abs(adcVals[0] - (AdcRegs.ADCRESULT0 >> 4)) > 0x00FF){
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
		else if(abs(adcVals[1]- (AdcRegs.ADCRESULT1 >> 4)) > 0x00FF){
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
		/*//VOLUME
		else if(abs(adcVals[2] - (AdcRegs.ADCRESULT2 >> 4)) > 0x00FF){
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
		else if(abs(adcVals[3] - (AdcRegs.ADCRESULT3 >> 4)) > 0x00FF){
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
		else if(abs(adcVals[4] - (AdcRegs.ADCRESULT4 >> 4)) > 0x00FF){
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
		else if(abs(adcVals[5] - (AdcRegs.ADCRESULT5 >> 4)) > 0x00FF){
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

*/
		if(resetTimer){
			CpuTimer1Regs.TCR.all = 0x4001;
			CpuTimer1Regs.TCR.bit.TRB = 1;
			resetTimer = 0;
		}
	}
}
//Preset Up
interrupt void preset_up(void){
	//If initial entry into preset, set up timer and flag
	if(GpioDataRegs.GPADAT.bit.GPIO5){
		if(!preset){
			CpuTimer1Regs.TCR.all = 0x4001;
			preset = 1;
		}
		else{
			presetNumber++;
			if(presetNumber == 10) presetNumber= 1;
		}
		CpuTimer1Regs.TCR.bit.TRB = 1;
		updateLcd = 1;
		updateCode = PRESETUP;
	}

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
//Preset Down
interrupt void preset_down(void){
	//If initial entry into preset, set up timer and flag
	if(GpioDataRegs.GPADAT.bit.GPIO6){
		if(!preset){
			CpuTimer1Regs.TCR.all = 0x4001;
			preset = 1;
		}
		else{
			presetNumber--;
			if(presetNumber == 0) presetNumber= 9;
		}
		CpuTimer1Regs.TCR.bit.TRB = 1;
		updateLcd = 1;
		updateCode = PRESETDOWN;
	}

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
//Load Preset
interrupt void load_preset(void){
	//Stop timer and reset flag
	if(GpioDataRegs.GPBDAT.bit.GPIO48){
		CpuTimer1Regs.TCR.bit.TSS = 1;
		preset = 0;
		load = 1;
		updateLcd = 1;
		updateCode = LOADPRESET;
	}
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
}
//Save Preset
interrupt void save_preset(void){
	//Stop timer and reset flag
	if(GpioDataRegs.GPBDAT.bit.GPIO49){
		CpuTimer1Regs.TCR.bit.TSS = 1;
		preset = 0;
		updateLcd = 1;
		updateCode = SAVEPRESET;
		save = 1;
	}

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
}
interrupt void effects(void){
	DelayUs(1);
	int input = (GpioDataRegs.GPADAT.all & 0x000000E) >> 1;
	if(input){
		/*if(input == 0x0004){
			updateLcd = 1;
			clearPipeline();
			updateCode = CLEAR;
		}*/
		//Switch to tuning function
		/*else if(input == 0x0004){
			/*tuner ^= 1;			//signal for timer0 to not sample out to SPI
			updateLcd = 1;
			updateCode = TUNER;
			if(tuner) updateTimer0(1000);	//Slower sample rate for FFT analysis = Higher bin resolution
			else updateTimer0(22.675f);		//FFT was toggled off, switch back to sample out to SPI
		}*/

		//Look to either queue effect or toggle state
		//else{
			toggle = 1;
			//Simple lookup vs mathematical computation gets the effect to be manipulated
			int effect = indexLookup(input);
			//toggleOn_Off returns 1 if it can be toggled, else 0 meaning its not in queue;
			if(!toggleOn_Off(effect)){
				queueEffect(effect);					//queue the effect
				mainDisplay[numQueued-1] = effect;		//queue the display
			}
			//Initialize Variables
			int i = 0;//, inMainDisplay = 0;
			//Loop through mainDisplay to see if the effect is already set to print to LCD
			for(;i<10;i++){
				//If it is in main display, break
				if(mainDisplay[i] == effect){
					indexToToggle = i;
					break;
				}
			}
			effectToToggle = effect;
		//}
	}
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
}
int toggleOn_Off(int effect){
	//If location[] == 0, then the effect is not in the queue so return 0 to signal
	//that this effect needs to be queued up.  Else just toggle the state.
	if(location[effect] != -1){
		on_off[location[effect]] ^= 1;
		return 1;
	}
	else return 0;
}
void queueEffect(int effect){
	location[effect] = numQueued;			//Set location of the effect in the location array, corresponds to the index in the queue, -1 because we inc numQueued first
	pipeline[numQueued] = list[effect];	//queue function pointer into the pipeline, -1 because we inc numQueued first
	on_off[numQueued] = 1;					//Turn effect on, makes sense for the user
	numQueued++;
}

void clearPipeline(){
	//Clear location/on_off arrays
	int i;
	for(i=0; i < 10; i++){
		location[i] = -1;
		on_off[i] = 0;
		mainDisplay[i] = -1;
	}
	numQueued = 0;
}
int toggleEffectOnDisplay(int effect){
	//Block input if tunerScreen

	//Initialize Variables
	int i = 0;//, inMainDisplay = 0;
	//Loop through mainDisplay to see if the effect is already set to print to LCD
	for(;i<10;i++){
		//If it is in main display, break
		if(mainDisplay[i] == effect)break;
	}
	return i;
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

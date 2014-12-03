#include "lcd.h"
#include "spi.h"
#include "effect.h"
#include "../include/fft.h"
#include "interrupts.h"
#include "eeprom.h"
#include "DSP2833x_Mcbsp.h"
#include "F28335_example.h"


/*********************************************************************************************************************************************************************
 * Effect variables/functions
 */
#define numEffects 4
int toggleOn_Off(int effect);
void queueEffect(int effect);
void clearPipeline();
int indexLookup(int);
//typedef int FUNC(int, struct params*);
//Struct for all the parameters available to effects, passed into the process functions
static struct params params;

//Create FUNC variables
FUNC processTremolo,processReverb,processFlanger,processWah, processPhaser;

//Static list of available effects, GPIO must match this
FUNC *list[numEffects] = {processTremolo,processReverb,processWah, processPhaser};

/*The indices of this array map  directly to the *list array.  This location array holds the location of the effect in the pipeline array.
 * Index 0 of the location array maps to index 0 of the list array.  But the data at index 0 of the location arary points to
 * where that effect is located in the pipeline array. Increase in mem usage for speed gain, don't have to iterate through *list to find
 * the right effect.*/
int location[numEffects];
//Array of FUNC's, this is the queue set by the user.
FUNC *pipeline[numEffects];
//lowpassType* lowpass;
int distortion = -1;
/*********************************************************************************************************************************************************************/

/*********************************************************************************************************************************************************************
 * LCD controller variables/functions
 */
int 	toggleDisplayOn_Off(int);	//Toggles effect on/off once its in the queue
void	queueDisplay(int);	//Sticks effect into queue
int		toggleEffectOnDisplay(int);	//Sticks effect into queue
//LCD Display Queue
int mainDisplay[numEffects];
int toggle = 0, effectToToggle = 0, indexToToggle = 0, toggleDist = 0;
int tuner = 0, preset = 0, currentChangeScreen = -1, resetTimer = 0, sysStart = 1, freq = 0;
int updateLcd = 0, updateCode = 0, updateChange, newLevel = 0, oldLevel = 0, updateFrequency = 0;
/********************************************************************************************************************************************************************/

/*********************************************************************************************************************************************************************
 * Shared Variables between LCD controller and effects
 */
//State of each queued effect, this allows user to stomp effects on/off without losing location in queue
//Indices of this array map direclty to indices of pipeline.  LCD will use this to know what to display
int on_off[numEffects];
//Number of queued effects
int numQueued;
/********************************************************************************************************************************************************************/

/********************************************************************************************************************************************************************
 * Input variables/functions
 */
void getInputs();
int updateInputs = 0, inputNumber = 0;
int previous, inputs[numEffects], previousInputs[numEffects];
/********************************************************************************************************************************************************************/

/**********************************************************************************************************************************************************************
 * Preset Variables
 */
int load = 0, save = 0, presetNumber = 1;
/********************************************************************************************************************************************************************/

#pragma CODE_SECTION(cpu_timer0_isr, "secureRamFuncs")
#pragma CODE_SECTION(timeout, "secureRamFuncs")
#pragma CODE_SECTION(preset_up, "secureRamFuncs")
#pragma CODE_SECTION(preset_down, "secureRamFuncs")
#pragma CODE_SECTION(load_preset, "secureRamFuncs")
#pragma CODE_SECTION(save_preset, "secureRamFuncs")
#pragma CODE_SECTION(effects, "secureRamFuncs")

int sample = 0;
int iCount = 0;
int blocked = 0;
int startTimeout = 0;

int main(){

	InitSysCtrl();
	//Copy from flash to ram
	memcpy(&secureRamFuncs_runstart, &secureRamFuncs_loadstart, (Uint32)&secureRamFuncs_loadsize);
	InitFlash();
	EALLOW;
		GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 0;
		GpioCtrlRegs.GPADIR.bit.GPIO20 = 1;

		GpioCtrlRegs.GPCDIR.bit.GPIO85 = 1;
		GpioCtrlRegs.GPCDIR.bit.GPIO86 = 1;
		GpioCtrlRegs.GPCDIR.bit.GPIO87 = 1;

		//Initiliaze McBSP
			InitMcbspbGpio();
		//Initialize I2C
			InitI2CGpio();
			I2CA_Init();
		//Initialize ADC/DAC
			init_mcbsp_spi();
			//Initialization for Expecting reference to be fed in
			//mcbsp_xmit(0x02000100);

			//Initialization for Using internal reference
			mcbsp_xmit(0x38000100);

			GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;
			GpioCtrlRegs.GPADIR.bit.GPIO19 = 1;	//CONVST
			init_adc_spi();
		//Initialize FFT
			initFFT();
		//Initialize LCD
			initLCD();
		//Initialize Interrupts
			initINTS();
			int i = 0;
			for(;i < numEffects; i++){
				mainDisplay[i] = -1;
				inputs[i] = 8 ;
				previousInputs[i] = 8;
			}

			//Run through save/load sequence to start I2C properly
			initEffects(&params);
			savePreset(20, location, on_off, inputs, distortion);
			loadPreset(20, pipeline, list, location, on_off, &numQueued, inputs,&distortion);

			//Initialize Effects
			distortion = 0;
			toggleDistortion(distortion);
	while(1){
		//Wait for signals
		//Retrieve inputs from rotary switch
		if(toggleDist){
			toggleDistortion(distortion);
			toggleDist = 0;
		}
		if(updateInputs){
			getInputs();
			updateInputs = 0;
		}
		//Toggle effect on the LCD
		if(toggle){
			toggle = 0;
			toggleLCD(effectToToggle,indexToToggle,on_off[indexToToggle], numQueued);
		}
		//Update LCD
		if(updateLcd || updateChange){
			if(updateLcd){
				updateLCD(&updateCode, mainDisplay, on_off, &presetNumber, &numQueued, distortion);
				updateLcd = 0;
				if(updateCode == CLEAR){
					toggleDistortion(distortion);
				}
			}
			//Must be called after the update LCD function because updateLCD could potentially call a clear screen.
			//This would clear out the level and it won't display.
			if(updateChange){
				updateLevel(inputs[inputNumber], previousInputs[inputNumber]);
				updateChange = 0;
			}
		}
		//Load Preset
		if(load){
			loadPreset(presetNumber, pipeline, list, location, on_off, &numQueued, inputs, &distortion);
			loadPresetScreen(location,mainDisplay, &numQueued);
			updateCode = MAIN;
			updateLcd = 1;
			load = 0;
		}
		//Save Preset
		if(save){
			savePreset(presetNumber, location, on_off, inputs,distortion);
			save = 0;
		}
		//Update Input Frequency
		if(updateFrequency){
			printFreq(freq);
			updateFrequency = 0;
		}
		//Start a 3 second display timeout for all user inputs.
		if(startTimeout){
			ConfigCpuTimer(&CpuTimer1, 150, 3000000);
			//CpuTimer1Regs.TCR.bit.TRB = 1;
			CpuTimer1Regs.TCR.all = 0x4001;
			startTimeout = 0;
		}
	}
}

interrupt void rotary(){
	//If the LCD is currently displaying a change screen.  This stops inputs after
	//A presetTimeout to incrememnt the inputNumber before the change screen is
	//reprinted
	if(!preset && !blocked){
		if(currentChangeScreen != -1){
			inputNumber++;
			if(inputNumber == numEffects) inputNumber = 0;
		}
		//previousInputs[inputNumber] = 0;	//Forces LCD to reprint entire line of bars
		updateLcd = 1;	//Signal LCD to change based on updatCode
		updateCode = inputNumber+21;
		updateChange = 1;	//update an input
		currentChangeScreen = inputNumber;
		//Reset Timer
		blocked = 1;
		ConfigCpuTimer(&CpuTimer1, 150, 240000);
		CpuTimer1Regs.TCR.all = 0x4001;
		//startTimeout = 1;
	}
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;	//Clear flag to accept more interrupts
}

void getInputs(){
	//Get input from rotary switch
	if(currentChangeScreen!=-1 && !preset){
		int change = 0;
		int B = GpioDataRegs.GPADAT.bit.GPIO23;
		int A = GpioDataRegs.GPADAT.bit.GPIO22;
		if((!B) && (previous)){
			previousInputs[inputNumber] = inputs[inputNumber];
			change = 1;
			if(A){
				if(inputs[inputNumber] <16) inputs[inputNumber]++;
			}
			else{
				if(inputs[inputNumber] >0) inputs[inputNumber]--;
			}
		}
		previous= B;

		if(!preset && change){
			updateChange = 1;
			startTimeout = 1;
		}
	}
}


interrupt void cpu_timer0_isr(void){
	sample = read_adc();	//Get sample from ADC
	if(distortion == 1){
		GpioDataRegs.GPCDAT.bit.GPIO85 = 0;
		GpioDataRegs.GPCDAT.bit.GPIO86 = 1;
		GpioDataRegs.GPCDAT.bit.GPIO87 = 0;
		sample -= 5300;
	}
	else if(distortion == 2){
		GpioDataRegs.GPCDAT.bit.GPIO85 = 0;
		GpioDataRegs.GPCDAT.bit.GPIO86 = 0;
		GpioDataRegs.GPCDAT.bit.GPIO87 = 1;
		sample -= 5300;
	}
	else{
		GpioDataRegs.GPCDAT.bit.GPIO85 = 1;
		GpioDataRegs.GPCDAT.bit.GPIO86 = 0;
		GpioDataRegs.GPCDAT.bit.GPIO87 = 0;
		sample -= 16270;
	}

	if(tuner){// && sampleCount == 23){
		if(storeFFT(sample - 5000)){
			freq = findFrequency();
			updateFrequency = 1;
		}
	}
	//else sampleCount++;
	else{
		sample = process(sample,numQueued, on_off,&pipeline[0],&params, inputs);	//Process sample
		if(distortion == 1 || distortion == 2){
			sample += 5300;
		}
		else sample += 16270;
		write_dac(sample);			//write sample to DAC
		updateInputs = 1;
	}
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	//Clear flag to accept more interrupts
}

//Timeout counter for Preset selection
interrupt void timeout(void){
	CpuTimer1Regs.TCR.bit.TSS = 1;

	//If debounce is over, go in to unblock user input.  If it is coming from a
	//preset up/down signal, signal to start a new timer1 sequence of 3 seconds.
	//If it's coming from the effect function, unblock and clear flag.
	if(blocked){
		blocked = 0;
		if(preset || (currentChangeScreen != -1 && !tuner)) startTimeout = 1;
	}
	else{
		//Return to homescreen
		updateLcd = 1;
		updateCode = PRESETTIMEOUT;

		//Reset flag bits that may have caused this
		preset = 0;
		currentChangeScreen = -1;
	}
	//Acknowledge Interrupt
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

interrupt void preset_up(void){
	//If initial entry into preset, set up timer and flag
	if(GpioDataRegs.GPADAT.bit.GPIO5 && currentChangeScreen == -1 && !blocked){
		blocked = 1;
		ConfigCpuTimer(&CpuTimer1, 150, 240000);
		CpuTimer1Regs.TCR.all = 0x4001;
		if(!preset){
			preset = 1;
		}
		else{
			presetNumber++;
			if(presetNumber == 10) presetNumber= 1;
		}
		//CpuTimer1Regs.TCR.bit.TRB = 1;
		updateLcd = 1;
		updateCode = PRESETUP;
	}

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

interrupt void preset_down(void){
	//If initial entry into preset, set up timer and flag
	if(GpioDataRegs.GPADAT.bit.GPIO6 && currentChangeScreen == -1 && !blocked){
		blocked = 1;
		ConfigCpuTimer(&CpuTimer1, 150, 240000);
		CpuTimer1Regs.TCR.all = 0x4001;
		if(!preset){
			preset = 1;
		}
		else{
			presetNumber--;
			if(presetNumber == 0) presetNumber= 9;
		}
		updateLcd = 1;
		updateCode = PRESETDOWN;
	}

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

interrupt void load_preset(void){
	//Stop timer and reset flag
	if(GpioDataRegs.GPBDAT.bit.GPIO48 && preset && !blocked){
		//CpuTimer1Regs.TCR.bit.TSS = 1;
		blocked = 1;
		updateTimer1(30000);
		preset = 0;
		load = 1;
		updateLcd = 1;
		updateCode = LOADPRESET;
	}
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
}

interrupt void save_preset(void){
	//Stop timer and reset flag
	if(GpioDataRegs.GPBDAT.bit.GPIO49 && preset && !blocked){
		//CpuTimer1Regs.TCR.bit.TSS = 1;
		blocked = 1;
		updateTimer1(120000);
		preset = 0;
		updateLcd = 1;
		updateCode = SAVEPRESET;
		save = 1;
	}
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
}

interrupt void effects(void){
	int input = (GpioDataRegs.GPBDAT.all & 0x03FC0000) >> 18;
	if(input && !preset && currentChangeScreen == -1 && !blocked){
		blocked = 1;
		updateTimer1(240000);
		iCount++;
		if(input == 0x80){
			updateLcd = 1;
			clearPipeline();
			updateCode = CLEAR;
		}
		else if(input == 0x40){
			tuner ^= 1;			//signal for timer0 to not sample out to SPI
			updateLcd = 1;
			updateCode = TUNER;
			if(tuner) updateTimer0(1000);	//Slower sample rate for FFT analysis = Higher bin resolution
			else updateTimer0(24);		//FFT was toggled off, switch back to sample out to SPI
		}
		else{
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

		}
	}
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
}
int toggleOn_Off(int effect){
	//If location[] == 0, then the effect is not in the queue so return 0 to signal
	//that this effect needs to be queued up.  Else just toggle the state.
	if(effect == CRUNCH){
		toggleDist = 1;
		if(distortion == 0){
			distortion = 1;
		}
		else if(distortion == 1){
			distortion = 0;
		}
		else{
			distortion = 1;
		}
		return 1;
	}
	else if(effect == DISTORTION){
		toggleDist = 1;
		if(distortion == 0){
			distortion = 2;
		}
		else if(distortion == 1){
			distortion = 2;
		}
		else{
			distortion = 0;
		}
		return 1;
	}
	else{
		if(location[effect] != -1){
			on_off[location[effect]] ^= 1;
			return 1;
		}
		else return 0;
	}
}
void queueEffect(int effect){
	if(effect != CRUNCH){
		location[effect] = numQueued;			//Set location of the effect in the location array, corresponds to the index in the queue, -1 because we inc numQueued first
		pipeline[numQueued] = list[effect];	//queue function pointer into the pipeline, -1 because we inc numQueued first
		on_off[numQueued] = 1;					//Turn effect on, makes sense for the user
		numQueued++;
	}
}

void clearPipeline(){
	//Clear location/on_off arrays
	int i;
	for(i=0; i < numEffects; i++){
		location[i] = -1;
		on_off[i] = 0;
		mainDisplay[i] = -1;
		inputs[i] = 8;
	}
	distortion = 0;
	numQueued = 0;
}
int toggleEffectOnDisplay(int effect){
	//Block input if tunerScreen

	//Initialize Variables
	int i = 0;//, inMainDisplay = 0;
	//Loop through mainDisplay to see if the effect is already set to print to LCD
	for(;i<numEffects;i++){
		//If it is in main display, break
		if(mainDisplay[i] == effect)break;
	}
	return i;
}
//Eliminates need to call log(input)/log(2).  Simple lookup will be faster

int indexLookup(int input){
	if(input == 1) return 0;		//Tremolo
	else if(input == 2) return 1;	//Reverb
	else if(input == 4) return 2;	//Wah
	else if(input == 8) return 3;	//Flanger
	else if(input == 16) return 4;	//DISTORTION
	else return 5;					//CRUNCH
}

#include "../include/effect.h"
#include "F28335_example.h"
#include "math.h"
#include "DSP28x_Project.h"
#include "spi.h"
#include "autoWah.h"
#include "lowpass.h"
#include "eeprom.h"
#include "effect.h"


#pragma CODE_SECTION(loadPreset, "secureRamFuncs")
#pragma CODE_SECTION(savePreset, "secureRamFuncs")
#pragma CODE_SECTION(eepromWrite, "secureRamFuncs")
#pragma CODE_SECTION(eepromRead, "secureRamFuncs")
#pragma CODE_SECTION(i2c_int1a_isr, "secureRamFuncs")
#pragma CODE_SECTION(savePreset, "secureRamFuncs")
#pragma CODE_SECTION(process, "secureRamFuncs")
#pragma CODE_SECTION(processTremolo, "secureRamFuncs")
#pragma CODE_SECTION(processReverb, "secureRamFuncs")
#pragma CODE_SECTION(processCrunch, "secureRamFuncs")
#pragma CODE_SECTION(processDistortion, "secureRamFuncs")


//For eeprom reading/writing
int readDone, writeDone = 0;
unsigned int adcVal[5];
#define EEPROMREADDELAY 5000
#define EEPROMWRITEDELAY 5000
struct I2CMSG *CurrentMsgPtr;	// Used in interrupts
struct I2CMSG messageOut;
struct I2CMSG messageIn;
#define I2C_SLAVE_ADDR        0x50
#define I2C_NUMBYTES          5
Uint16 Error;
void eepromWrite();
void eepromRead();

int returnArray[26];

void initEffects(struct params* params){
	//Lowpass Filter
	//lowpass = lowpass_create();
	messageOut.MsgStatus = I2C_MSGSTAT_INACTIVE;
	//Autowah
	//AutoWah_init(2000,  /*Effect rate 2000*/22000, /*Sampling Frequency*/1500,  /*Maximum frequency*/200,   /*Minimum frequency*/ 4,     /*Q*/0.707, /*Gain factor*/5     /*Frequency increment*/);
	//Clear the queue
	clearPipeline();
	//Tremolo
	params->tremoloCounter = 0;
	params->tremoloCount = 0;
	params->tremoloRate = 0;
	params->tremoloLimit = 0;
	params->reverbCount = 0;
	params->reverbStart = 0;
	params->flangerLimit = 20000;
	params->flangerCounter = 0;
	params->flangerStart = 0;
	params->flangerCount = 0;
	params->flangerSweep = 0;
	params->wahCount = 0;
	params->wahCounter = 500;
	params->wahStart = 0;
	int i;
	for(i = 0; i < 600; i++) params->flangerDelay[i] = 0;
}

int process(int sample, int numQueued, int* on_off, FUNC**pipeline, struct params* params, int* counts){
	int index;
	//Loop through the entire queue, if its on -> process, else skip
	for(index = 0; index < numQueued; index++){
		if(on_off[index]) sample = pipeline[index](sample, params, counts);  //pipeline[index] maps to a function, (sample, &params) is the prototype
	}
	return sample;
}

/*
 * Processing functions
 */
int processDelay(int sample, struct params* p, int* counts){
	return sample;
}
int processDistortion(int sample, struct params* p, int* counts){
	//Spit out sample to analog Distortion circuit on DAC B.  Will need a GPIO to select where the signal goes from the analog switch
	long temp = sample;
	Uint32 command = 0x19000000 | (temp<<8);
	mcbsp_xmit(command);
	DelayUs(.5);
	sample = read_adc();
	return sample;
}
int processCrunch(int sample, struct params* p, int* counts){
	//Spit out sample to analog Distortion circuit on DAC B.  Will need a GPIO to select where the signal goes from the analog switch
	long temp = sample;
	Uint32 command = 0x19000000 | (temp<<8);
	mcbsp_xmit(command);
	DelayUs(.5);
	sample = read_adc();
	return sample;
}
int processTremolo(int sample, struct params* p, int* counts){
	sample-=8900;
	//Sets rate at which the effect runs
		int pedal = counts[0];
		//int temp1 = p->tremoloLimit;
		p->tremoloLimit = (double)3000*((float)pedal/(float)16)+ 1000;
		//temp1 = pedal>>12;
		//temp1 <<= 12;
		//temp1 = pedal + 4000;
		//temp1 += 4000;
		//p->tremoloLimit = pedal + 4000;

		//Count up or down, if it hits upper limit then count up else count down
		if(p->tremoloCounter >= p->tremoloLimit) p->tremoloCount = -1;
		else if(p->tremoloCounter == 0) p->tremoloCount = 1;
		p->tremoloCounter+=p->tremoloCount;

		//Calculate new tremolo sample
		double temp = (double)p->tremoloCounter*.7/(double)p->tremoloLimit;//*(double)sample;
		//double temp = temp1>>1;
		//temp /= p->tremoloLimit;
		sample = temp*(double)sample;
		sample += 8900;
		return sample;
}
int processWah(int sample, struct params* p, int* counts){
	sample-=8900;
	// sample = (int)AutoWah_process(sample);

	double damp = .05;
	int minf = 500;
	int maxf = 3000;
	double Fw = 3000;
	double Fs = 22000;
	double delta = Fw/Fs;


	if(p->wahCounter >= maxf) p->wahCount = -delta;
	else if(p->wahCounter <= minf) p->wahCount = delta;
	p->wahCounter+=p->wahCount;

	double F1 = 2*sin(((double)PI*(double)p->wahCounter)/Fs);
	double Q1 = 2*damp;
	if(!p->wahStart){
		p->wahStart = 1;
		p->yh[0] = sample;
		p->yb[0] = F1*p->yh[0];
		p->yl[0] = F1*p->yb[0];
		p->yb[1] = sample;
	}
	else{
		p->yh[1] = (double)sample - p->yl[0] - Q1*p->yb[0];
		p->yb[1] = F1*p->yh[1] + p->yb[0];
		p->yl[1] = F1*p->yb[1] + p->yl[0];
	}
	sample = (int)p->yb[1] + 10;
	p->yb[0] = p->yb[1];
	p->yl[0] = p->yl[1];
	sample+=8900;
	return sample;
}
int processPhaser(int sample, struct params* p, int* counts){
	return sample;
}
int processFlanger(int sample, struct params* p, int* counts){
	//Process sweep
	//Max delay = 15ms
	//Delay sweep at 1 Hz
	//int Fs = 22727;
	//int rate = 1;
	sample-=8900;
	double decay = .6;
		//Once reinitialized, start to process reverb
	if(p->flangerCount == 600){
		p->flangerStart = 1;
		p->flangerCount = 0;
	}

	//p->flangerCounter++;
	//if(p->flangerCounter == 22000)p->flangerCounter = 0;
	//int delay = p->flangerSweep * maxSampleDelay;
	//int delayIndex = delay;//abs(p->flangerCount - delay);
	if(p->flangerStart){

		double rate = counts[2];
		rate /= 16;
		rate = rate*(double)22000 + (double)22000;

		double toSine = 2.0*(double)PI*(double)p->flangerSweepCount*(double)1/(double)rate;

		p->flangerSweep = sin(toSine);
		if(p->flangerSweep < 0) p->flangerSweep *= (double)-1.0;
		p->flangerSweepCount++;
		if(p->flangerSweepCount == rate) p->flangerSweepCount = 0;
		int sweepDelay = p->flangerSweep * (double)100;
		int delayIndex = p->flangerCount;
		delayIndex = delayIndex - sweepDelay;
		if(delayIndex < 0) delayIndex += 100;
		int temp = p->flangerDelay[delayIndex];
		p->flangerDelay[p->flangerCount] = (double)sample*decay + (double)p->flangerDelay[p->flangerCount]* decay;
		p->flangerCount++;

		sample +=  temp;
	}
	else p->flangerDelay[p->flangerCount] = (double)sample*decay;

	p->flangerCount++;
	sample += 8900;
	return sample;
}
int processReverb(int sample, struct params* p, int* counts){
	//Reinitialize the reverb array on every start
	sample-=8900;
	if(p->reverbCount == 800){
			p->reverbStart = 1;
			p->reverbCount = 0;
		}
		double decay = counts[1];
		decay = ((double)decay / (double)16)*.45 + .15;
		//Once reinitialized, start to process reverb
		if(p->reverbStart){
			int temp = p->reverbDelay[p->reverbCount];
			sample += p->reverbDelay[p->reverbCount];
			p->reverbDelay[p->reverbCount] = (double)sample*decay + (double)temp*(decay-.08);
		}
		else p->reverbDelay[p->reverbCount] = (double)sample*decay;

		p->reverbCount++;
		sample += 8900;
		return sample;
}
int processChorus(int sample, struct params* p, int* counts){
	return sample;
}
int processPitchShift(int sample, struct params* p, int* counts){
	return sample;
}








void savePreset(int presetNum, int* location, int* on_off){
	//Calculate addresses based on presetNum
	int i = 0;
	int locationMessage1 = (presetNum-1)*32;
	int locationMessage2 = locationMessage1 + 5;
	int on_offMessage1 = locationMessage2 + 5;
	int on_offMessage2 = on_offMessage1 + 5;
	int adcMessage1 = on_offMessage2 + 5;
	int adcMessage2 = adcMessage1 + 6;
	int tLocation[10];
	//Inc location array by 1 because I don't want to save -1 in eeprom
	for(i = 0; i < 10; i++){
		int temp = location[i];
		tLocation[i] = ++temp;
	}

	//write first half of location array
	messageOut.MemoryLowAddr = locationMessage1 & 0x00FF;
	messageOut.MemoryHighAddr = (locationMessage1 & 0xFF00)>>8;
	messageOut.MsgStatus = I2C_MSGSTAT_SEND_WITHSTOP;
	messageOut.SlaveAddress = 0x50;
	messageOut.NumOfBytes = 5;
	for(i = 0; i < messageOut.NumOfBytes; i++) messageOut.MsgBuffer[i] = tLocation[i];
	eepromWrite();
	DelayUs(EEPROMWRITEDELAY);
	while(messageOut.MsgStatus != I2C_MSGSTAT_INACTIVE){};

	//write second half of location array
	messageOut.MemoryLowAddr = locationMessage2 & 0x00FF;
	messageOut.MemoryHighAddr = (locationMessage2 & 0xFF00)>>8;
	messageOut.MsgStatus = I2C_MSGSTAT_SEND_WITHSTOP;
	messageOut.SlaveAddress = 0x50;
	messageOut.NumOfBytes = 5;
	for(i = 0; i < messageOut.NumOfBytes; i++) messageOut.MsgBuffer[i] = tLocation[i+5];
	eepromWrite();
	DelayUs(EEPROMWRITEDELAY);
	while(messageOut.MsgStatus != I2C_MSGSTAT_INACTIVE){};


	//write first half of on_off array
	messageOut.MemoryLowAddr = on_offMessage1 & 0x00FF;
	messageOut.MemoryHighAddr = (on_offMessage1 & 0xFF00)>>8;
	messageOut.MsgStatus = I2C_MSGSTAT_SEND_WITHSTOP;
	messageOut.SlaveAddress = 0x50;
	messageOut.NumOfBytes = 5;
	for(i = 0; i < messageOut.NumOfBytes; i++) messageOut.MsgBuffer[i] = on_off[i];
	eepromWrite();
	DelayUs(EEPROMWRITEDELAY);
	while(messageOut.MsgStatus != I2C_MSGSTAT_INACTIVE){};

	//write second half of on_off array
	messageOut.MemoryLowAddr = on_offMessage2 & 0x00FF;
	messageOut.MemoryHighAddr = (on_offMessage2 & 0xFF00)>>8;
	messageOut.MsgStatus = I2C_MSGSTAT_SEND_WITHSTOP;
	messageOut.SlaveAddress = 0x50;
	messageOut.NumOfBytes = 5;
	for(i = 0; i < messageOut.NumOfBytes; i++) messageOut.MsgBuffer[i] = on_off[i+5];
	eepromWrite();
	DelayUs(EEPROMWRITEDELAY);
	while(messageOut.MsgStatus != I2C_MSGSTAT_INACTIVE){};

	//write out first half of adc values
	messageOut.MemoryLowAddr = adcMessage1 & 0x00FF;
	messageOut.MemoryHighAddr = (adcMessage1 & 0xFF00)>>8;
	messageOut.MsgStatus = I2C_MSGSTAT_SEND_WITHSTOP;
	messageOut.SlaveAddress = 0x50;
	messageOut.NumOfBytes = 6;
	for(i = 0; i < messageOut.NumOfBytes; i++) messageOut.MsgBuffer[i] = adcVal[i];
	eepromWrite();
	DelayUs(EEPROMWRITEDELAY);
	while(messageOut.MsgStatus != I2C_MSGSTAT_INACTIVE){};

	//write out second half of adc values
	messageOut.MemoryLowAddr = adcMessage2 & 0x00FF;
	messageOut.MemoryHighAddr = (adcMessage2 & 0xFF00)>>8;
	messageOut.MsgStatus = I2C_MSGSTAT_SEND_WITHSTOP;
	messageOut.SlaveAddress = 0x50;
	messageOut.NumOfBytes = 6;
	for(i = 0; i < messageOut.NumOfBytes; i++) messageOut.MsgBuffer[i] = adcVal[i] >> 8;
	eepromWrite();
	DelayUs(EEPROMWRITEDELAY);
	while(messageOut.MsgStatus != I2C_MSGSTAT_INACTIVE){};
}

void loadPreset(int presetNum, FUNC**pipeline, FUNC**list, int* location, int* on_off, int* numQueued){
	int i = 0;
	//Calculate addresses based on presetNum
	int locationMessage1 = (presetNum-1)*32;
	int locationMessage2 = locationMessage1 + 5;
	int on_offMessage1 = locationMessage2 + 5;
	int on_offMessage2 = on_offMessage1 + 5;
	int adcMessage1 = on_offMessage2 + 5;
	int adcMessage2 = adcMessage1 + 6;

	//Read in first half of location array
	messageIn.MemoryLowAddr = locationMessage1 & 0x00FF;
	messageIn.MemoryHighAddr = (locationMessage1 & 0xFF00)>>8;
	messageIn.MsgStatus = I2C_MSGSTAT_SEND_NOSTOP;
	messageIn.SlaveAddress = 0x50;
	messageIn.NumOfBytes = 5;
	eepromRead();
	DelayUs(EEPROMREADDELAY);
	for(i = 0; i < 5; i++) {
		location[i] = messageIn.MsgBuffer[i];
		returnArray[i] = location[i];
	}
	while(messageIn.MsgStatus != I2C_MSGSTAT_INACTIVE);

	//Read in second half of location array
	messageIn.MemoryLowAddr = locationMessage2 & 0x00FF;
	messageIn.MemoryHighAddr = (locationMessage2 & 0xFF00)>>8;
	messageIn.MsgStatus = I2C_MSGSTAT_SEND_NOSTOP;
	messageIn.SlaveAddress = 0x50;
	messageIn.NumOfBytes = 5;
	eepromRead();
	DelayUs(EEPROMREADDELAY);
	for(i = 0; i < 5; i++) {
		location[i+5] = messageIn.MsgBuffer[i];
	}
	while(messageIn.MsgStatus != I2C_MSGSTAT_INACTIVE);

	//Read in second half of on_off array
	messageIn.MemoryLowAddr = on_offMessage1 & 0x00FF;
	messageIn.MemoryHighAddr = (on_offMessage1 & 0xFF00)>>8;
	messageIn.MsgStatus = I2C_MSGSTAT_SEND_NOSTOP;
	messageIn.SlaveAddress = 0x50;
	messageIn.NumOfBytes = 5;
	eepromRead();
	DelayUs(EEPROMREADDELAY);
	for(i = 0; i < 5; i++){
		on_off[i] = messageIn.MsgBuffer[i];
	}
	while(messageIn.MsgStatus != I2C_MSGSTAT_INACTIVE);

	//Read in second half of on_off array
	messageIn.MemoryLowAddr = on_offMessage2 & 0x00FF;
	messageIn.MemoryHighAddr = (on_offMessage2 & 0xFF00)>>8;
	messageIn.MsgStatus = I2C_MSGSTAT_SEND_NOSTOP;
	messageIn.SlaveAddress = 0x50;
	messageIn.NumOfBytes = 5;
	eepromRead();
	DelayUs(EEPROMREADDELAY);
	for(i = 0; i < 5; i++){
		on_off[i+5] = messageIn.MsgBuffer[i];
	}
	while(messageIn.MsgStatus != I2C_MSGSTAT_INACTIVE);

	//Read in first half of adc values
	messageIn.MemoryLowAddr = adcMessage1 & 0x00FF;
	messageIn.MemoryHighAddr = (adcMessage1 & 0xFF00)>>8;
	messageIn.MsgStatus = I2C_MSGSTAT_SEND_NOSTOP;
	messageIn.SlaveAddress = 0x50;
	messageIn.NumOfBytes = 5;
	eepromRead();
	DelayUs(EEPROMREADDELAY);
	for(i = 0; i < 5; i++){
		adcVal[i] = messageIn.MsgBuffer[i];
	}
	while(messageIn.MsgStatus != I2C_MSGSTAT_INACTIVE);

	//Read in second half of adc values
	messageIn.MemoryLowAddr = adcMessage2 & 0x00FF;
	messageIn.MemoryHighAddr = (adcMessage2 & 0xFF00)>>8;
	messageIn.MsgStatus = I2C_MSGSTAT_SEND_NOSTOP;
	messageIn.SlaveAddress = 0x50;
	messageIn.NumOfBytes = 5;
	eepromRead();
	DelayUs(EEPROMREADDELAY);
	for(i = 0; i < 5; i++){
		unsigned int temp = messageIn.MsgBuffer[i] << 8;
		adcVal[i] = temp | adcVal[i];
	}
	while(messageIn.MsgStatus != I2C_MSGSTAT_INACTIVE);

	//Dec location array by 1 because I don't want to save -1 in eeprom
	for(i = 0; i < 10; i++){
		int temp = location[i];
		temp = temp - 1;
		location[i] = temp;
	}

	*numQueued = 0;
	for(i = 0; i < 10; i++){
		int temp = location[i];
		if(temp != -1){
			location[i] = temp;
			pipeline[temp] = list[i];
			*numQueued = *numQueued + 1;
		}
	}
}

interrupt void i2c_int1a_isr(void){
   Uint16 IntSource, i;
   // Read interrupt source
   IntSource = I2caRegs.I2CISRC.all;
   // Interrupt source = stop condition detected
   if(IntSource == I2C_SCD_ISRC){
      // If completed message was writing data, reset msg to inactive state
      if (CurrentMsgPtr->MsgStatus == I2C_MSGSTAT_WRITE_BUSY){
         CurrentMsgPtr->MsgStatus = I2C_MSGSTAT_INACTIVE;
      }
      else{
         // If a message receives a NACK during the address setup portion of the
         // EEPROM read, the code further below included in the register access ready
         // interrupt source code will generate a stop condition. After the stop
         // condition is received (here), set the message status to try again.
         // User may want to limit the number of retries before generating an error.
         if(CurrentMsgPtr->MsgStatus == I2C_MSGSTAT_SEND_NOSTOP_BUSY){
            CurrentMsgPtr->MsgStatus = I2C_MSGSTAT_SEND_NOSTOP;
         }
         // If completed message was reading EEPROM data, reset msg to inactive state
         // and read data from FIFO.
         else if (CurrentMsgPtr->MsgStatus == I2C_MSGSTAT_READ_BUSY){
            CurrentMsgPtr->MsgStatus = I2C_MSGSTAT_INACTIVE;
            for(i=0; i < I2C_NUMBYTES; i++){
              CurrentMsgPtr->MsgBuffer[i] = I2caRegs.I2CDRR;
              readDone = 1;
            }
         }
      }
   }  // end of stop condition detected

   // Interrupt source = Register Access Ready
   // This interrupt is used to determine when the EEPROM address setup portion of the
   // read data communication is complete. Since no stop bit is commanded, this flag
   // tells us when the message has been sent instead of the SCD flag. If a NACK is
   // received, clear the NACK bit and command a stop. Otherwise, move on to the read
   // data portion of the communication.
   else if(IntSource == I2C_ARDY_ISRC){
      if(I2caRegs.I2CSTR.bit.NACK == 1){
         I2caRegs.I2CMDR.bit.STP = 1;
         I2caRegs.I2CSTR.all = I2C_CLR_NACK_BIT;
      }
      else if(CurrentMsgPtr->MsgStatus == I2C_MSGSTAT_SEND_NOSTOP_BUSY){
         CurrentMsgPtr->MsgStatus = I2C_MSGSTAT_RESTART;
      }
   }  // end of register access ready

   else{
      // Generate some error due to invalid interrupt source
      asm("   ESTOP0");
   }

   // Enable future I2C (PIE Group 8) interrupts
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;
}

void eepromWrite(){
	writeDone = 0;
	while(!writeDone){
		 if(messageOut.MsgStatus == I2C_MSGSTAT_SEND_WITHSTOP){
			Error = I2CA_WriteData(&messageOut);
			if (Error == I2C_SUCCESS){
				writeDone = 1;
				CurrentMsgPtr = &messageOut;
				messageOut.MsgStatus = I2C_MSGSTAT_WRITE_BUSY;
			}
		}  // end of write section
	}
}

void eepromRead(){
	readDone = 0;
	while(!readDone){
	//Check outgoing message status. Bypass read section if status is not inactive.
		  if (messageOut.MsgStatus == I2C_MSGSTAT_INACTIVE){
			 // Check incoming message status.
			 if(messageIn.MsgStatus == I2C_MSGSTAT_SEND_NOSTOP){
				// EEPROM address setup portion
				while(I2CA_ReadData(&messageIn) != I2C_SUCCESS){

				}
				// Update current message pointer and message status
				CurrentMsgPtr = &messageIn;
				messageIn.MsgStatus = I2C_MSGSTAT_SEND_NOSTOP_BUSY;
			 }

			 // Once message has progressed past setting up the internal address
			 // of the EEPROM, send a restart to read the data bytes from the
			 // EEPROM. Complete the communique with a stop bit. MsgStatus is
			 // updated in the interrupt service routine.
			 else if(messageIn.MsgStatus == I2C_MSGSTAT_RESTART){
				// Read data portion
				while(I2CA_ReadData(&messageIn) != I2C_SUCCESS){
				   // Maybe setup an attempt counter to break an infinite while
				   // loop.
				}
				// Update current message pointer and message status
				CurrentMsgPtr = &messageIn;
				messageIn.MsgStatus = I2C_MSGSTAT_READ_BUSY;
			 }
		  }
	}
}

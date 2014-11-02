#include "../include/effect.h"
#include "math.h"
#include "DSP28x_Project.h"
#include "spi.h"
#include "autoWah.h"
#include "lowpass.h"
#include "eeprom.h"


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


//Type definition for process*Effect* method prototypes
typedef int FUNC(int, struct params*);

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
lowpassType* lowpass;


//State of each queued effect, this allows user to stomp effects on/off without losing location in queue
//Indices of this array map direclty to indices of pipeline
int on_off[10];

//Number of queued effects
int numQueued;

//Struct for all the parameters available to effects, passed into the process functions
static struct params params;

void initEffects(){
	//Lowpass Filter
	lowpass = lowpass_create();

	//Autowah
	AutoWah_init(2000,  /*Effect rate 2000*/16000, /*Sampling Frequency*/1000,  /*Maximum frequency*/500,   /*Minimum frequency*/ 4,     /*Q*/0.707, /*Gain factor*/10     /*Frequency increment*/);
	//Clear the queue
	clearPipeline();
	//Tremolo
	params.tremoloCounter = 0;
			params.tremoloCount = 0;
			params.tremoloRate = 0;
			params.tremoloLimit = 0;
			params.reverbCount = 0;
			params.reverbStart = 0;

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
	}
	numQueued = 0;
}

int process(int sample){
	int index;
	//Loop through the entire queue, if its on -> process, else skip
	for(index = 0; index < numQueued; index++){
		if(on_off[index]) sample = pipeline[index](sample, &params);  //pipeline[index] maps to a function, (sample, &params) is the prototype
	}
	return sample;
}

/*
 * Processing functions
 */
int processDelay(int sample, struct params* p){
	return sample;
}
int processDistortion(int sample, struct params* p){
	//Spit out sample to analog Distortion circuit on DAC B.  Will need a GPIO to select where the signal goes from the analog switch
	long temp = sample;
	Uint32 command = 0x19000000 | (temp<<8);
	mcbsp_xmit(command);
	DELAY_US(3);
	return (sample);
}
int processCrunch(int sample, struct params* p){
	//Spit out sample to analog Distortion circuit on DAC B.  Will need a GPIO to select where the signal goes from the analog switch
	long temp = sample;
	Uint32 command = 0x19000000 | (temp<<8);
	mcbsp_xmit(command);
	DELAY_US(3);
	return (sample);
}
int processTremolo(int sample, struct params* p){
	//Sets rate at which the effect runs
		double max = 0x0FFF;
		double pedal = AdcRegs.ADCRESULT0>>4;
		p->tremoloLimit = (double)1000*(pedal/max)+ 1000;

		//Count up or down, if it hits upper limit then count up else count down
		if(p->tremoloCounter >= p->tremoloLimit) p->tremoloCount = -1;
		else if(p->tremoloCounter == 0) p->tremoloCount = 1;
		p->tremoloCounter+=p->tremoloCount;

		//Calculate new tremolo sample
		double temp = (double)p->tremoloCounter*.7/(double)p->tremoloLimit;//*(double)sample;

		return (int)(temp*(double)sample);
}
int processWah(int sample, struct params* p){
	return AutoWah_process(sample);
}
int processPhaser(int sample, struct params* p){
	return sample;
}
int processFlange(int sample, struct params* p){
	return sample;
}
int processReverb(int sample, struct params* p){
	//Reinitialize the reverb array on every start

	if(p->reverbCount == 800){
			p->reverbStart = 1;
			p->reverbCount = 0;
		}
		double decay = AdcRegs.ADCRESULT1 >> 4;
		decay = ((double)decay / (double)0xFFF)*.5 + .15;
		//Once reinitialized, start to process reverb
		if(p->reverbStart){
			int temp = p->reverbDelay[p->reverbCount];
			sample += p->reverbDelay[p->reverbCount];
			p->reverbDelay[p->reverbCount] = (double)sample*decay + (double)temp*(decay-.08);
		}
		else p->reverbDelay[p->reverbCount] = (double)sample*decay;

		p->reverbCount++;
		return sample;
}
int processChorus(int sample, struct params* p){
	return sample;
}
int processPitchShift(int sample, struct params* p){
	return sample;
}








void savePreset(int presetNum){
	/*
	//Calculate addresses based on presetNum
	int i = 0;
	int locationMessage1 = (presetNum-1)*32;
	int locationMessage2 = locationMessage1 + 5;
	int on_offMessage1 = locationMessage2 + 5;
	int on_offMessage2 = on_offMessage1 + 5;
	int adcMessage1 = on_offMessage2 + 5;
	int adcMessage2 = adcMessage1 + 6;

	//Inc location array by 1 because I don't want to save -1 in eeprom
	for(i = 0; i < 10; i++){
		int temp = location[i];
		location[i] = ++temp;
	}

	//write first half of location array
	messageOut.MemoryLowAddr = locationMessage1 & 0x00FF;
	messageOut.MemoryHighAddr = (locationMessage1 & 0xFF00)>>8;
	messageOut.MsgStatus = I2C_MSGSTAT_SEND_WITHSTOP;
	messageOut.SlaveAddress = 0x50;
	messageOut.NumOfBytes = 5;
	for(i = 0; i < messageOut.NumOfBytes; i++) messageOut.MsgBuffer[i] = location[i];
	eepromWrite();
	DELAY_US(EEPROMWRITEDELAY);
	while(messageOut.MsgStatus != I2C_MSGSTAT_INACTIVE){};

	//write second half of location array
	messageOut.MemoryLowAddr = locationMessage2 & 0x00FF;
	messageOut.MemoryHighAddr = (locationMessage2 & 0xFF00)>>8;
	messageOut.MsgStatus = I2C_MSGSTAT_SEND_WITHSTOP;
	messageOut.SlaveAddress = 0x50;
	messageOut.NumOfBytes = 5;
	for(i = 0; i < messageOut.NumOfBytes; i++) messageOut.MsgBuffer[i] = location[i+5];
	eepromWrite();
	DELAY_US(EEPROMWRITEDELAY);
	while(messageOut.MsgStatus != I2C_MSGSTAT_INACTIVE){};


	//write first half of on_off array
	messageOut.MemoryLowAddr = on_offMessage1 & 0x00FF;
	messageOut.MemoryHighAddr = (on_offMessage1 & 0xFF00)>>8;
	messageOut.MsgStatus = I2C_MSGSTAT_SEND_WITHSTOP;
	messageOut.SlaveAddress = 0x50;
	messageOut.NumOfBytes = 5;
	for(i = 0; i < messageOut.NumOfBytes; i++) messageOut.MsgBuffer[i] = on_off[i];
	eepromWrite();
	DELAY_US(EEPROMWRITEDELAY);
	while(messageOut.MsgStatus != I2C_MSGSTAT_INACTIVE){};

	//write second half of on_off array
	messageOut.MemoryLowAddr = on_offMessage2 & 0x00FF;
	messageOut.MemoryHighAddr = (on_offMessage2 & 0xFF00)>>8;
	messageOut.MsgStatus = I2C_MSGSTAT_SEND_WITHSTOP;
	messageOut.SlaveAddress = 0x50;
	messageOut.NumOfBytes = 5;
	for(i = 0; i < messageOut.NumOfBytes; i++) messageOut.MsgBuffer[i] = on_off[i+5];
	eepromWrite();
	DELAY_US(EEPROMWRITEDELAY);
	while(messageOut.MsgStatus != I2C_MSGSTAT_INACTIVE){};

	//write out first half of adc values
	messageOut.MemoryLowAddr = adcMessage1 & 0x00FF;
	messageOut.MemoryHighAddr = (adcMessage1 & 0xFF00)>>8;
	messageOut.MsgStatus = I2C_MSGSTAT_SEND_WITHSTOP;
	messageOut.SlaveAddress = 0x50;
	messageOut.NumOfBytes = 6;
	for(i = 0; i < messageOut.NumOfBytes; i++) messageOut.MsgBuffer[i] = adcVal[i];
	eepromWrite();
	DELAY_US(EEPROMWRITEDELAY);
	while(messageOut.MsgStatus != I2C_MSGSTAT_INACTIVE){};

	//write out second half of adc values
	messageOut.MemoryLowAddr = adcMessage2 & 0x00FF;
	messageOut.MemoryHighAddr = (adcMessage2 & 0xFF00)>>8;
	messageOut.MsgStatus = I2C_MSGSTAT_SEND_WITHSTOP;
	messageOut.SlaveAddress = 0x50;
	messageOut.NumOfBytes = 6;
	for(i = 0; i < messageOut.NumOfBytes; i++) messageOut.MsgBuffer[i] = adcVal[i] >> 8;
	eepromWrite();
	DELAY_US(EEPROMWRITEDELAY);
	while(messageOut.MsgStatus != I2C_MSGSTAT_INACTIVE){};*/
}

int* loadPreset(int presetNum){
	/*
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
	DELAY_US(EEPROMREADDELAY);
	for(i = 0; i < 5; i++) {
		location[i] = messageIn.MsgBuffer[i];
		returnArray[i] = location[i];
	}
	//while(messageIn.MsgStatus != I2C_MSGSTAT_INACTIVE);

	//Read in second half of location array
	messageIn.MemoryLowAddr = locationMessage2 & 0x00FF;
	messageIn.MemoryHighAddr = (locationMessage2 & 0xFF00)>>8;
	messageIn.MsgStatus = I2C_MSGSTAT_SEND_NOSTOP;
	messageIn.SlaveAddress = 0x50;
	messageIn.NumOfBytes = 5;
	eepromRead();
	DELAY_US(EEPROMREADDELAY);
	for(i = 0; i < 5; i++) {
		location[i+5] = messageIn.MsgBuffer[i];
		returnArray[i+5] = location[i+5];
	}
	//while(messageIn.MsgStatus != I2C_MSGSTAT_INACTIVE);

	//Read in second half of on_off array
	messageIn.MemoryLowAddr = on_offMessage1 & 0x00FF;
	messageIn.MemoryHighAddr = (on_offMessage1 & 0xFF00)>>8;
	messageIn.MsgStatus = I2C_MSGSTAT_SEND_NOSTOP;
	messageIn.SlaveAddress = 0x50;
	messageIn.NumOfBytes = 5;
	eepromRead();
	DELAY_US(EEPROMREADDELAY);
	for(i = 0; i < 5; i++){
		on_off[i] = messageIn.MsgBuffer[i];
		returnArray[i+10] = on_off[i];
	}
	//while(messageIn.MsgStatus != I2C_MSGSTAT_INACTIVE);

	//Read in second half of on_off array
	messageIn.MemoryLowAddr = on_offMessage2 & 0x00FF;
	messageIn.MemoryHighAddr = (on_offMessage2 & 0xFF00)>>8;
	messageIn.MsgStatus = I2C_MSGSTAT_SEND_NOSTOP;
	messageIn.SlaveAddress = 0x50;
	messageIn.NumOfBytes = 5;
	eepromRead();
	DELAY_US(EEPROMREADDELAY);
	for(i = 0; i < 5; i++){
		on_off[i+5] = messageIn.MsgBuffer[i];
		returnArray[i+15] = on_off[i+5];
	}
	//while(messageIn.MsgStatus != I2C_MSGSTAT_INACTIVE);

	//Read in first half of adc values
	messageIn.MemoryLowAddr = adcMessage1 & 0x00FF;
	messageIn.MemoryHighAddr = (adcMessage1 & 0xFF00)>>8;
	messageIn.MsgStatus = I2C_MSGSTAT_SEND_NOSTOP;
	messageIn.SlaveAddress = 0x50;
	messageIn.NumOfBytes = 5;
	eepromRead();
	DELAY_US(EEPROMREADDELAY);
	for(i = 0; i < 5; i++){
		adcVal[i] = messageIn.MsgBuffer[i];
	}
	//while(messageIn.MsgStatus != I2C_MSGSTAT_INACTIVE);

	//Read in second half of adc values
	messageIn.MemoryLowAddr = adcMessage2 & 0x00FF;
	messageIn.MemoryHighAddr = (adcMessage2 & 0xFF00)>>8;
	messageIn.MsgStatus = I2C_MSGSTAT_SEND_NOSTOP;
	messageIn.SlaveAddress = 0x50;
	messageIn.NumOfBytes = 5;
	eepromRead();
	DELAY_US(EEPROMREADDELAY);
	for(i = 0; i < 5; i++){
		unsigned int temp = messageIn.MsgBuffer[i] << 8;
		adcVal[i] = temp | adcVal[i];
		returnArray[i+20] = adcVal[i];
	}
	//while(messageIn.MsgStatus != I2C_MSGSTAT_INACTIVE);

	//Dec location array by 1 because I don't want to save -1 in eeprom
	for(i = 0; i < 10; i++){
		location[i] = location[i]--;
	}

	numQueued = 0;
	for(i = 0; i < 10; i++){
		int temp = location[i];
		if(temp != 0){
			location[i] = temp;
			pipeline[temp] = list[i];
			numQueued++;
		}
	}
	returnArray[25] = numQueued;
	return returnArray;*/
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

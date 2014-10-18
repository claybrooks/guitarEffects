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
#include "../include/eeprom.h"
#include "AIC23.h"
void savePreset(int);
void loadPreset(int);
void eepromWrite();
void eepromRead();
interrupt void i2c_int1a_isr(void);

int readDone, writeDone = 0;

//Struct for all the parameters available to effects, passed into the process functions
static struct params params;

//Type definition for process*Effect* method prototypes
	typedef int FUNC(int, struct params*);

	//Create FUNC variables
	FUNC processTremolo,processDistortion,processCrunch,processDelay,processWah,processPhaser,processFlange,processReverb,processChorus,processPitchShift;

	//Static list of available effects, GPIO must match this
	FUNC *list[10] = {processTremolo,processDistortion,processCrunch,processDelay,processWah,processPhaser,processFlange,processReverb,processChorus,processPitchShift};

	/*The indices of this array map  directly to the *list array.  This location array holds the location of the effect in the pipeline array.
	 * Index 0 of the location array maps to index 0 of the list array.  But the data at index 0 of the location array points to
	 * where that effect is located in the pipeline array. Increase in mem usage for speed gain, don't have to iterate through *list to find
	 * the right effect.
	 */
	int location[10];

	//Array of FUNC's, this is the queue set by the user.
	FUNC *pipeline[10];

	//State of each queued effect, this allows user to stomp effects on/off without losing location in queue
	//Indices of this array map direclty to indices of pipeline
	int on_off[10];

	//Number of queued effects
	int numQueued;

	unsigned int adcVal[5];

#define EEPROMREADDELAY 5000
#define EEPROMWRITEDELAY 5000

struct I2CMSG *CurrentMsgPtr;				// Used in interrupts
struct I2CMSG messageOut;
struct I2CMSG messageIn;
#define I2C_SLAVE_ADDR        0x50
#define I2C_NUMBYTES          5
Uint16 Error;
void main(void)
{

   Uint16 i;

   CurrentMsgPtr = &messageOut;

// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the DSP2833x_SysCtrl.c file.
   InitSysCtrl();


// Step 2. Initalize GPIO:
// This example function is found in the DSP2833x_Gpio.c file and
// illustrates how to set the GPIO to it's default state.
// InitGpio();
// Setup only the GP I/O only for I2C functionality
   InitI2CGpio();

// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts
   DINT;

// Initialize PIE control registers to their default state.
// The default state is all PIE interrupts disabled and flags
// are cleared.
// This function is found in the DSP2833x_PieCtrl.c file.
   InitPieCtrl();

// Disable CPU interrupts and clear all CPU interrupt flags:
   IER = 0x0000;
   IFR = 0x0000;

// Initialize the PIE vector table with pointers to the shell Interrupt
// Service Routines (ISR).
// This will populate the entire table, even if the interrupt
// is not used in this example.  This is useful for debug purposes.
// The shell ISR routines are found in DSP2833x_DefaultIsr.c.
// This function is found in DSP2833x_PieVect.c.
   InitPieVectTable();

// Interrupts that are used in this example are re-mapped to
// ISR functions found within this file.
   EALLOW;	// This is needed to write to EALLOW protected registers
   PieVectTable.I2CINT1A = &i2c_int1a_isr;
   EDIS;   // This is needed to disable write to EALLOW protected registers

// Step 4. Initialize all the Device Peripherals:
// This function is found in DSP2833x_InitPeripherals.c
// InitPeripherals(); // Not required for this example
   I2CA_Init();

// Step 5. User specific code

   // Clear Counters
   PassCount = 0;
   FailCount = 0;

   // Clear incoming message buffer
   for (i = 0; i < I2C_MAX_BUFFER_SIZE; i++)
   {
       messageIn.MsgBuffer[i] = 0x0000;
   }

// Enable interrupts required for this example

// Enable I2C interrupt 1 in the PIE: Group 8 interrupt 1
   PieCtrlRegs.PIEIER8.bit.INTx1 = 1;

// Enable CPU INT8 which is connected to PIE group 8
   IER |= M_INT8;
   EINT;

	messageOut.MsgStatus = I2C_MSGSTAT_INACTIVE;
	on_off[0] = 1;
	on_off[1] = 1;
	on_off[2] = 2;
	on_off[3] = 1;
	on_off[4] = 1;
	on_off[5] = 1;
	on_off[6] = 2;
	on_off[7] = 1;
	on_off[8] = 2;
	on_off[9] = 1;

	location[0] = 1;
	location[1] = 2;
	location[2] = 3;
	location[3] = 4;
	location[4] = 5;
	location[5] = 1;
	location[6] = 1;
	location[7] = 1;
	location[8] = 1;
	location[9] = 1;

	adcVal[0] = 0x0000;
	adcVal[1] = 0x3333;
	adcVal[2] = 0x6666;
	adcVal[3] = 0x9999;
	adcVal[4] = 0xCCCC;

	savePreset(1);

	on_off[0] = 0;
	on_off[1] = 0;
	on_off[2] = 0;
	on_off[3] = 0;
	on_off[4] = 0;
	on_off[5] = 0;
	on_off[6] = 0;
	on_off[7] = 0;
	on_off[8] = 0;
	on_off[9] = 0;

	location[0] = 0;
	location[1] = 0;
	location[2] = 0;
	location[3] = 0;
	location[4] = 0;
	location[5] = 0;
	location[6] = 0;
	location[7] = 0;
	location[8] = 0;
	location[9] = 0;

	adcVal[0] = 0x0000;
	adcVal[1] = 0x0000;
	adcVal[2] = 0x0000;
	adcVal[3] = 0x0000;
	adcVal[4] = 0x0000;

	loadPreset(1);
	while(1){
		//Wait for interrupts
	}
}

void clearBuffer(struct I2CMSG* msg){
	int i = 0;
	for(i = 0; i < I2C_MAX_BUFFER_SIZE; i++){
		msg->MsgBuffer[i] = 0;
	}
}

void savePreset(int presetNum){
	//Calculate addresses based on presetNum
	int i = 0;
	int locationMessage1 = (presetNum-1)*32;
	int locationMessage2 = locationMessage1 + 5;
	int on_offMessage1 = locationMessage2 + 5;
	int on_offMessage2 = on_offMessage1 + 5;
	int adcMessage1 = on_offMessage2 + 5;
	int adcMessage2 = adcMessage1 + 6;

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
	while(messageOut.MsgStatus != I2C_MSGSTAT_INACTIVE){};

}

void loadPreset(int presetNum){
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
	for(i = 0; i < 5; i++)  location[i] = messageIn.MsgBuffer[i];
	while(messageIn.MsgStatus != I2C_MSGSTAT_INACTIVE);

	//Read in second half of location array
	messageIn.MemoryLowAddr = locationMessage2 & 0x00FF;
	messageIn.MemoryHighAddr = (locationMessage2 & 0xFF00)>>8;
	messageIn.MsgStatus = I2C_MSGSTAT_SEND_NOSTOP;
	messageIn.SlaveAddress = 0x50;
	messageIn.NumOfBytes = 5;
	eepromRead();
	DELAY_US(EEPROMREADDELAY);
	for(i = 0; i < 5; i++)  location[i+5] = messageIn.MsgBuffer[i];
	while(messageIn.MsgStatus != I2C_MSGSTAT_INACTIVE);

	//Read in second half of on_off array
	messageIn.MemoryLowAddr = on_offMessage1 & 0x00FF;
	messageIn.MemoryHighAddr = (on_offMessage1 & 0xFF00)>>8;
	messageIn.MsgStatus = I2C_MSGSTAT_SEND_NOSTOP;
	messageIn.SlaveAddress = 0x50;
	messageIn.NumOfBytes = 5;
	eepromRead();
	DELAY_US(EEPROMREADDELAY);
	for(i = 0; i < 5; i++) on_off[i] = messageIn.MsgBuffer[i];
	while(messageIn.MsgStatus != I2C_MSGSTAT_INACTIVE);

	//Read in second half of on_off array
	messageIn.MemoryLowAddr = on_offMessage2 & 0x00FF;
	messageIn.MemoryHighAddr = (on_offMessage2 & 0xFF00)>>8;
	messageIn.MsgStatus = I2C_MSGSTAT_SEND_NOSTOP;
	messageIn.SlaveAddress = 0x50;
	messageIn.NumOfBytes = 5;
	eepromRead();
	DELAY_US(EEPROMREADDELAY);
	for(i = 0; i < 5; i++) on_off[i+5] = messageIn.MsgBuffer[i];
	while(messageIn.MsgStatus != I2C_MSGSTAT_INACTIVE);

	//Read in first half of adc values
	messageIn.MemoryLowAddr = adcMessage1 & 0x00FF;
	messageIn.MemoryHighAddr = (adcMessage1 & 0xFF00)>>8;
	messageIn.MsgStatus = I2C_MSGSTAT_SEND_NOSTOP;
	messageIn.SlaveAddress = 0x50;
	messageIn.NumOfBytes = 5;
	eepromRead();
	DELAY_US(EEPROMREADDELAY);
	for(i = 0; i < 5; i++) adcVal[i] = messageIn.MsgBuffer[i];
	while(messageIn.MsgStatus != I2C_MSGSTAT_INACTIVE);

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
	}
	while(messageIn.MsgStatus != I2C_MSGSTAT_INACTIVE);


numQueued = 0;
int i;
	for(i = 0; i < 10; i++){
		if(i < 5){
			int temp = location[i];
			if(temp != 0){
				location[i] = temp;
				pipeline[temp] = list[i];
				numQueued++;
			}
		}
	}
}



interrupt void i2c_int1a_isr(void)     // I2C-A
{
   Uint16 IntSource, i;

   // Read interrupt source
   IntSource = I2caRegs.I2CISRC.all;

   // Interrupt source = stop condition detected
   if(IntSource == I2C_SCD_ISRC)
   {
      // If completed message was writing data, reset msg to inactive state
      if (CurrentMsgPtr->MsgStatus == I2C_MSGSTAT_WRITE_BUSY)
      {
         CurrentMsgPtr->MsgStatus = I2C_MSGSTAT_INACTIVE;
      }
      else
      {
         // If a message receives a NACK during the address setup portion of the
         // EEPROM read, the code further below included in the register access ready
         // interrupt source code will generate a stop condition. After the stop
         // condition is received (here), set the message status to try again.
         // User may want to limit the number of retries before generating an error.
         if(CurrentMsgPtr->MsgStatus == I2C_MSGSTAT_SEND_NOSTOP_BUSY)
         {
            CurrentMsgPtr->MsgStatus = I2C_MSGSTAT_SEND_NOSTOP;
         }
         // If completed message was reading EEPROM data, reset msg to inactive state
         // and read data from FIFO.
         else if (CurrentMsgPtr->MsgStatus == I2C_MSGSTAT_READ_BUSY)
         {
            CurrentMsgPtr->MsgStatus = I2C_MSGSTAT_INACTIVE;
            for(i=0; i < CurrentMsgPtr->NumOfBytes; i++)
            {
              CurrentMsgPtr->MsgBuffer[i] = I2caRegs.I2CDRR;
            }
            readDone = 1;
         }
      }
   }  // end of stop condition detected

   // Interrupt source = Register Access Ready
   // This interrupt is used to determine when the EEPROM address setup portion of the
   // read data communication is complete. Since no stop bit is commanded, this flag
   // tells us when the message has been sent instead of the SCD flag. If a NACK is
   // received, clear the NACK bit and command a stop. Otherwise, move on to the read
   // data portion of the communication.
   else if(IntSource == I2C_ARDY_ISRC)
   {
      if(I2caRegs.I2CSTR.bit.NACK == 1)
      {
         I2caRegs.I2CMDR.bit.STP = 1;
         I2caRegs.I2CSTR.all = I2C_CLR_NACK_BIT;
      }
      else if(CurrentMsgPtr->MsgStatus == I2C_MSGSTAT_SEND_NOSTOP_BUSY)
      {
         CurrentMsgPtr->MsgStatus = I2C_MSGSTAT_RESTART;
      }
   }  // end of register access ready

   else
   {
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

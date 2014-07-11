#define MAXINDEX 0x1000

void setARRAY(int);
int getARRAY(int);
int getEFFECT(void);
unsigned int* array = (unsigned int*)0xB000;
int index = 0;
int delayIndex = 0;
int delaySamples = 4000;
int delayCount = 0;
int delayLength = 4000;

//Assume voltage range will be from 1.3v-.7v.  This will change obviously
//Array values are right justified, DAC values must be left justified
int FUZZ(){
	//Add pot to change fuzzines
	//FUZZ has nothing to do with anything except voltage level.  No index needed, just current ADC reading

	if(getARRAY(index-1) > 0x0900) return 0x0900;
	else return getARRAY(index-1);
}

int DELAY(){
	//Dec delayLength to play out the delayed signal.  Will become a pot value maybe.
	int val;
	if(delayLength == 0){
		delayLength = 2000;
		delayCount = 0;
	}
	//If the correct delayCount has passed, start adding in delay, dec delayLength to play out correct delayed sample length;
	if(delayCount == delaySamples){
		delayLength--;
		val =  (getARRAY(delayIndex) >> 2) + getARRAY(index-1);
		delayIndex += 1;
		if(delayIndex == MAXINDEX) delayIndex = 0;
	}
	else{
		delayCount += 1;
		val =  getARRAY(index-1);
	}
	return val;
}

int FLANGE(){

	return 0;
}

int PHASER(){

	return 0;
}

int CHORUS(){

	return 0;
}

int REVERB(){

	return 0;
}

int getEFFECT(){
	if(!GpioDataRegs.GPADAT.bit.GPIO10 && !GpioDataRegs.GPADAT.bit.GPIO9 && GpioDataRegs.GPADAT.bit.GPIO8) return FUZZ();
	else if(!GpioDataRegs.GPADAT.bit.GPIO10 && GpioDataRegs.GPADAT.bit.GPIO9 && !GpioDataRegs.GPADAT.bit.GPIO8) return DELAY();
	else if(!GpioDataRegs.GPADAT.bit.GPIO10 && GpioDataRegs.GPADAT.bit.GPIO9 && GpioDataRegs.GPADAT.bit.GPIO8) return FLANGE();
	else if(GpioDataRegs.GPADAT.bit.GPIO10 && !GpioDataRegs.GPADAT.bit.GPIO9 && !GpioDataRegs.GPADAT.bit.GPIO8) return PHASER();
	else if(GpioDataRegs.GPADAT.bit.GPIO10 && !GpioDataRegs.GPADAT.bit.GPIO9 && GpioDataRegs.GPADAT.bit.GPIO8) return REVERB();
	else return getARRAY(index-1);
}

void setARRAY(int data){
	if(index == MAXINDEX){
		index = 0;
		array = (unsigned int*)0xB000;
	}
	if((index & 0x003) == 0) *array = (*array & 0x000F) | data;
	else if((index & 0x003) ==  1){
		*array = (*array & 0xFFF0) | ((data & 0xF000) >> 12);
		array += 1;
		*array = (*array & 0x00FF) | (data & 0x0FF0) << 4;
	}
	else if((index & 0x003) ==  2){
		*array = (*array & 0xFF00) | ((data & 0xFF00) >> 8);
		array += 1;
		*array = (*array & 0x0FFF) | ((data & 0x00F0) << 8);
	}
	else{
		*array = (*array & 0xF000) | (data >> 4);
		array += 1;
	}
	index++;
}
int getARRAY(int index){
	//Position = index % 4
	unsigned int* get = (unsigned int*)(0xB000 + ((index*3) >> 2));

	if((index & 0x003) == 0) return *get >> 4;
	else if((index & 0x003) ==  1) return ((*get & 0x000F) << 8) | ((*(get+1) & 0xFF00)>> 8);
	else if((index & 0x003) ==  2) return ((*get & 0x00FF) << 4) | ((*(get+1) & 0xF000) >> 12);
	else return *get & 0x0FFF;
}

void initEFFECTS(){
	//Switch select for effect;
	GpioCtrlRegs.GPADIR.bit.GPIO8 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO9 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO10 = 0;
}

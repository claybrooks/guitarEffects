void setARRAY(int);
int getARRAY(int);
unsigned int* array = (unsigned int*)0xB000;
int point = 0;

//Assume voltage range will be from 1.3v-.7v.  This will change obviously

int FUZZ(){
	//Add pot to change fuzzines
	//FUZZ has nothing to do with anything except voltage level.  No index needed, just current ADC reading

	if(getARRAY(point-1) > 0x03FF) return 0x3FF0;
	else return getARRAY(point-1) << 4;
}

int DELAY(){

	return 0;
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
	if((GpioDataRegs.GPADAT.bit.GPIO8 + GpioDataRegs.GPADAT.bit.GPIO9 + GpioDataRegs.GPADAT.bit.GPIO10) == 1) return FUZZ();
	else if((GpioDataRegs.GPADAT.bit.GPIO8 + GpioDataRegs.GPADAT.bit.GPIO9 + GpioDataRegs.GPADAT.bit.GPIO10) == 2) return DELAY();
	else if((GpioDataRegs.GPADAT.bit.GPIO8 + GpioDataRegs.GPADAT.bit.GPIO9 + GpioDataRegs.GPADAT.bit.GPIO10) == 3) return FLANGE();
	else if((GpioDataRegs.GPADAT.bit.GPIO8 + GpioDataRegs.GPADAT.bit.GPIO9 + GpioDataRegs.GPADAT.bit.GPIO10) == 4) return PHASER();
	else if((GpioDataRegs.GPADAT.bit.GPIO8 + GpioDataRegs.GPADAT.bit.GPIO9 + GpioDataRegs.GPADAT.bit.GPIO10) == 5) return REVERB();
	else return getARRAY(point-1) << 4;
}

void setARRAY(int data){
	if(point == 0x1000){
		point = 0;
		array = (unsigned int*)0xB000;
	}
	if((point & 0x003) == 0) *array = (*array & 0x000F) | data;
	else if((point & 0x003) ==  1){
		*array = (*array & 0xFFF0) | ((data & 0xF000) >> 12);
		array += 1;
		*array = (*array & 0x00FF) | (data & 0x0FF0) << 4;
	}
	else if((point & 0x003) ==  2){
		*array = (*array & 0xFF00) | ((data & 0xFF00) >> 8);
		array += 1;
		*array = (*array & 0x0FFF) | ((data & 0x00F0) << 8);
	}
	else{
		*array = (*array & 0xF000) | (data >> 4);
		array += 1;
	}
	point++;
}
int getARRAY(int val){
	//Position = index % 4
	unsigned int* get = (unsigned int*)(0xB000 + ((val*3) >> 2));

	if((val & 0x003) == 0) return *get >> 4;
	else if((val & 0x003) ==  1) return ((*get & 0x000F) << 8) | ((*(get+1) & 0xFF00)>> 8);
	else if((val & 0x003) ==  2) return ((*get & 0x00FF) << 4) | ((*(get+1) & 0xF000) >> 12);
	else return *get & 0x0FFF;
}

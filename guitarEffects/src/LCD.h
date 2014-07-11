#include "math.h"
void initLCD();
void printLCD(int data);
void controlLCD(int data);

int delay = 11000;

void wait(int temp){
	while(temp != 0) temp--;
}

void initLCD(){
	EALLOW;
	GpioCtrlRegs.GPADIR.bit.GPIO0 = 0x1;
	GpioCtrlRegs.GPADIR.bit.GPIO1 = 0x1;
	GpioCtrlRegs.GPADIR.bit.GPIO2 = 0x1;
	GpioCtrlRegs.GPADIR.bit.GPIO3 = 0x1;
	GpioCtrlRegs.GPADIR.bit.GPIO4 = 0x1;
	GpioCtrlRegs.GPADIR.bit.GPIO5 = 0x1;
	GpioCtrlRegs.GPADIR.bit.GPIO6 = 0x1;
	GpioCtrlRegs.GPADIR.bit.GPIO7 = 0x1;
	GpioCtrlRegs.GPBDIR.bit.GPIO48 = 0x1;
	GpioCtrlRegs.GPBDIR.bit.GPIO49 = 0x1;
	//E = 48
	//RS = 49
	//8 Bit Mode
	//Write 3F for 1st function set
	controlLCD(0x3F);
	//Write 3F for 2nd function set
	controlLCD(0x3F);
	//Write 0F, D = 1 for display on, C = 1 for cursor on, B = 1 for blinking on
	controlLCD(0x0F);
	//Write 01 to clear display
	controlLCD(0x01);
	//Entry mode set for inc and no shift
	controlLCD(0x06);
}

void controlLCD(int data){
	GpioDataRegs.GPBDAT.bit.GPIO49 = 0;
	GpioDataRegs.GPADAT.all = data;
	wait(delay);
	GpioDataRegs.GPBDAT.bit.GPIO48 = 1;
	wait(delay);
	GpioDataRegs.GPBDAT.bit.GPIO48 = 0;
	wait(delay);

}

void printLCD(int data){
	GpioDataRegs.GPBDAT.bit.GPIO49 = 1;
	GpioDataRegs.GPADAT.all = data;
	wait(delay);
	GpioDataRegs.GPBDAT.bit.GPIO48 = 1;
	wait(delay);
	GpioDataRegs.GPBDAT.bit.GPIO48 = 0;
	wait(delay);
}

void printEFFECT(){
	if(GpioDataRegs.GPADAT.bit.GPIO8 && !GpioDataRegs.GPADAT.bit.GPIO9 && !GpioDataRegs.GPADAT.bit.GPIO10){
		controlLCD(0x01);
		printLCD(0x46);	//F
		printLCD(0x55);	//U
		printLCD(0x5A);	//Z
		printLCD(0x5A);	//Z
	}
	else if(!GpioDataRegs.GPADAT.bit.GPIO8 && GpioDataRegs.GPADAT.bit.GPIO9 && !GpioDataRegs.GPADAT.bit.GPIO10){
		controlLCD(0x01);
		printLCD(0x44);	//D
		printLCD(0x45);	//E
		printLCD(0x4C);	//L
		printLCD(0x41);	//A
		printLCD(0x59);	//Y
	}
	else if(GpioDataRegs.GPADAT.bit.GPIO8 && GpioDataRegs.GPADAT.bit.GPIO9 && !GpioDataRegs.GPADAT.bit.GPIO10){
		controlLCD(0x01);
		printLCD(0x46);	//F
		printLCD(0x4C);	//L
		printLCD(0x41);	//A
		printLCD(0x4E);	//N
		printLCD(0x47);	//G
		printLCD(0x45);	//E
	}
	else if(!GpioDataRegs.GPADAT.bit.GPIO8 && !GpioDataRegs.GPADAT.bit.GPIO9 && GpioDataRegs.GPADAT.bit.GPIO10){
		controlLCD(0x01);
		printLCD(0x50);	//P
		printLCD(0x48);	//H
		printLCD(0x41);	//A
		printLCD(0x53);	//S
		printLCD(0x45);	//E
		printLCD(0x52);	//R
	}
	else if(GpioDataRegs.GPADAT.bit.GPIO8 && !GpioDataRegs.GPADAT.bit.GPIO9 && GpioDataRegs.GPADAT.bit.GPIO10){
		controlLCD(0x01);
		printLCD(0x52);	//R
		printLCD(0x45);	//E
		printLCD(0x56);	//V
		printLCD(0x45);	//E
		printLCD(0x52);	//R
		printLCD(0x42);	//B
	}
	else{
		controlLCD(0x01);
		printLCD(0x43);	//C
		printLCD(0x4C);	//L
		printLCD(0x45);	//E
		printLCD(0x41);	//A
		printLCD(0x4E);	//N
	}
}

void printFREQ(int data){
	controlLCD(0x01);

	unsigned int* array = (unsigned int*)0xA000;
	int counter = 0;
	while(data >= 1){
		counter += 1;
		*array = fmod(data, 10);
		data  = data / 10;
		array += 1;
	}
	array -= 1;
	for(; counter > 0; counter--, array--){
		printLCD(*array+0x30);
	}
}

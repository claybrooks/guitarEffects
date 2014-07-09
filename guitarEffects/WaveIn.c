#include "DSP28x_Project.h"    				// Device Headerfile and Examples Include File
#include "LCD.h"
#include "SPI.h"
#include "SYS.h"
#include "EFFECT.h"
#include "ADC.h"

int temp;


int main(){
	InitSysCtrl();
	initINTS();
	initLCD();
	initSPI();
	initADC();
	initTIMER0();
	EALLOW;

	//Switch select for effect;
	GpioCtrlRegs.GPADIR.bit.GPIO8 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO9 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO10 = 0;

	while(1)							// endless loop - wait for an interrupt
	{


	}
}

void cpu_timer0_isr(void){
	setARRAY(AdcRegs.ADCRESULT0);
	printSPI();
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

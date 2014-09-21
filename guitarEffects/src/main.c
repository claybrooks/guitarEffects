#include "initialize.h"
#include "DSP28x_Project.h"

int screen = -1, input = 0, sample;

int main(){
	InitSysCtrl();
	initialize();
	while(1){
		if(screen > -1){
			updateLCD(screen);
			screen = -1;
		}
	}
}

interrupt void cpu_timer0_isr(void){
	sample = getAdc();
	//process(0x0AAA);
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

interrupt void xint1_isr(void){
	input = (GpioDataRegs.GPADAT.all & 0x0000FF00) >> 8;
	if(input!= 0)handle(input);
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

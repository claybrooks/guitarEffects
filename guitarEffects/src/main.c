#include "initialize.h"
#include "DSP28x_Project.h"

int updateCode = -1;

int main(){
	InitSysCtrl();
	INITIALIZE();
	while(1){
		if(updateCode != -1){
			updateCode = updateLCD(updateCode);
		}
	}
}

void cpu_timer0_isr(void){
	printSPI(processEffect(getAdc()));
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

void xint1_isr(void){
	int input = ((GpioDataRegs.GPADAT.all) & 0xFFF00000) >> 20;
	handleInput(input);
	updateCode = MAIN;
}

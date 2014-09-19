#include "initialize.h"
int main(){

	INITIALIZE();

	while(1){

	}
}

void cpu_timer0_isr(void){
	printSPI(getAdc());
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

void xint1_isr(void){
	int input = ((GpioDataRegs.GPADAT.all) & 0xFFF00000) >> 20;
	if(input == 0x0800) clearPipeline();
	else handleInput(input&0xCFFF)
}

#include "DSP28x_Project.h"    				// Device Headerfile and Examples Include File
#include "LCD.h"
#include "SPI.h"
#include "SYS.h"
#include "EFFECT.h"
#include "ADC.h"

int main(){
	InitSysCtrl();
	initINTS();
	initLCD();
	initSPI();
	initADC();
	//initTIMER0();
	EALLOW;

	//Switch select for effect;
	GpioCtrlRegs.GPADIR.bit.GPIO8 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO9 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO10 = 0;

	while(1)							// endless loop - wait for an interrupt
	{


	}
}

/*void cpu_timer0_isr(){

}*/

void adc_isr(void){
	setARRAY(AdcRegs.ADCRESULT0);
	printSPI();

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;				// Must acknowledge the PIE group
	//--- Manage the ADC registers
	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;					// Reset SEQ1 to CONV00 state
	AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;					// Clear ADC SEQ1 interrupt flag
}

void printSPI();
void initSPI();

void printSPI(){

	GpioDataRegs.GPADAT.bit.GPIO19 = 0;
	SpiaRegs.SPITXBUF = getEFFECT();
	DELAY_US(2.2);
	GpioDataRegs.GPADAT.bit.GPIO19 = 1;
}

void initSPI(){
	InitSpiaGpio();
	EALLOW;
	SpiaRegs.SPICCR.bit.SPISWRESET=0; // Reset SPI

	SpiaRegs.SPICCR.all=0x000B;       //12-bit character
	SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;   // Master mode enabled
	SpiaRegs.SPICTL.bit.TALK = 1; // Transmit capability enabled
	SpiaRegs.SPISTS.all=0x0000;
	SpiaRegs.SPIBRR = 0x0002;           // Baud rate ~ 10 MHz
	SpiaRegs.SPICCR.bit.CLKPOLARITY = 0;  // Rise edge w/o delay
	SpiaRegs.SPICTL.bit.CLK_PHASE = 1;

	SpiaRegs.SPICCR.bit.SPISWRESET=1;  // Enable SPI
}

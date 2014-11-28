/**********************************************************************
* File: F28335_example.h
* Device: TMS320F28335
* Author: David M. Alter, Texas Instruments Inc.
* Description: Include file for example project.  Include this file in
*   all C-source files.
* Notes:
*   1) The following constants may be defined in the CCS project build
*      options:
*        EXAMPLE_NONBIOS
*        EXAMPLE_BIOS5
*        EXAMPLE_FLASH
*        EXAMPLE_RAM
* History:
*   03/15/08 - Added prototype for InitXintf() (D. Alter)
*   12/13/07 - original (D. Alter)
**********************************************************************/

#ifndef F28335_EXAMPLE_H
#define F28335_EXAMPLE_H


//---------------------------------------------------------------------------
// Constant Definitions
//

//---------------------------------------------------------------------------
// Include Standard C Language Header Files
//
#include <string.h>


//---------------------------------------------------------------------------
// Include any other Header Files
//
#include "DSP2833x_Device.h"				// Peripheral address definitions

#ifdef EXAMPLE_NONBIOS
	#include "DSP2833x_DefaultIsr.h"		// ISR definitions
#endif

#ifdef EXAMPLE_BIOS5
	#ifdef EXAMPLE_FLASH
	    #include "F28335_example_BIOS5_flashcfg.h"
	#endif

	#ifdef EXAMPLE_RAM
	    #include "F28335_example_BIOS5_ramcfg.h"
	#endif
#endif


//---------------------------------------------------------------------------
// Function Prototypes
//
extern void DelayUs(Uint16);
extern void InitAdc(void);
extern void InitECap(void);
extern void InitEPwm(void);

#ifdef EXAMPLE_FLASH
	extern void InitFlash(void);
#endif

extern void InitGpio(void);
extern void InitPieCtrl(void);
extern void InitSysCtrl(void);
extern void InitWatchdog(void);
extern void InitXintf(void);
extern void SetDBGIER(Uint16);

#ifdef EXAMPLE_BIOS5
	extern void UserInit(void);
#endif


//---------------------------------------------------------------------------
// Global symbols defined in the linker command file
//
	extern Uint16 secureRamFuncs_loadstart;
	extern Uint16 secureRamFuncs_loadsize;
	extern Uint16 secureRamFuncs_runstart;


//---------------------------------------------------------------------------
// Global Variable References
//

#ifdef EXAMPLE_NONBIOS
	extern const struct PIE_VECT_TABLE PieVectTableInit;	// Pie vector table (non-BIOS only)
#endif


//---------------------------------------------------------------------------
// Macros
//
#define ADC_cal_func_ptr (void (*)(void))0x380080


//---------------------------------------------------------------------------
#endif  // end of F28335_EXAMPLE_H definition


//--- end of file -----------------------------------------------------

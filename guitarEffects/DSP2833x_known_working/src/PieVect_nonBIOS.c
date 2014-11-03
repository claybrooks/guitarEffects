/**********************************************************************
* File: PieVect_nonBIOS.c
* Devices: TMS320F2833x
* Author: David M. Alter, Texas Instruments Inc.
* History:
*   12/18/07 - original (D. Alter)
**********************************************************************/
#include "F28335_example.h"				// Main include file


/**********************************************************************
* The PIE vector initialization table for the F2833x.
**********************************************************************/
const struct PIE_VECT_TABLE PieVectTableInit = {

//--- Base vectors
    rsvd_ISR,                       // 0x000D00  reserved
    rsvd_ISR,                       // 0x000D02  reserved
    rsvd_ISR,                       // 0x000D04  reserved
    rsvd_ISR,                       // 0x000D06  reserved
    rsvd_ISR,                       // 0x000D08  reserved
    rsvd_ISR,                       // 0x000D0A  reserved
    rsvd_ISR,                       // 0x000D0C  reserved
    rsvd_ISR,                       // 0x000D0E  reserved
    rsvd_ISR,                       // 0x000D10  reserved
    rsvd_ISR,                       // 0x000D12  reserved
    rsvd_ISR,                       // 0x000D14  reserved
    rsvd_ISR,                       // 0x000D16  reserved
    rsvd_ISR,                       // 0x000D18  reserved
    INT13_ISR,                      // 0x000D1A  INT13 - XINT13 (or CPU Timer1, reserved for TI)
    INT14_ISR,                      // 0x000D1C  INT14 - CPU Timer2, reserved for TI
    DATALOG_ISR,                    // 0x000D1E  DATALOG - CPU data logging interrupt
    RTOSINT_ISR,                    // 0x000D20  RTOSINT - CPU RTOS interrupt
    EMUINT_ISR,                     // 0x000D22  EMUINT - CPU emulation interrupt
    NMI_ISR,                        // 0x000D24  NMI - XNMI interrupt
    ILLEGAL_ISR,                    // 0x000D26  ILLEGAL - illegal operation trap
    USER1_ISR,                      // 0x000D28  USER1 - software interrupt #1
    USER2_ISR,                      // 0x000D2A  USER2 - software interrupt #2
    USER3_ISR,                      // 0x000D2C  USER3 - software interrupt #3
    USER4_ISR,                      // 0x000D2E  USER4 - software interrupt #4
    USER5_ISR,                      // 0x000D30  USER5 - software interrupt #5
    USER6_ISR,                      // 0x000D32  USER6 - software interrupt #6
    USER7_ISR,                      // 0x000D34  USER7 - software interrupt #7
    USER8_ISR,                      // 0x000D36  USER8 - software interrupt #8
    USER9_ISR,                      // 0x000D38  USER9 - software interrupt #9
    USER10_ISR,                     // 0x000D3A  USER10 - software interrupt #10
    USER11_ISR,                     // 0x000D3C  USER11 - software interrupt #11
    USER12_ISR,                     // 0x000D3E  USER12 - software interrupt #12

//--- Core interrupt #1 re-map
    SEQ1INT_ISR,                    // 0x000D40  SEQ1INT - ADC Sequencer 1
    SEQ2INT_ISR,                    // 0x000D42  SEQ2INT - ADC Sequencer 2
    rsvd_ISR,                       // 0x000D44  reserved
    XINT1_ISR,                      // 0x000D46  XINT1
    XINT2_ISR,                      // 0x000D48  XINT2
    ADCINT_ISR,                     // 0x000D4A  ADCINT
    TINT0_ISR,                      // 0x000D4C  TINT0 - CPU TIMER 0
    WAKEINT_ISR,                    // 0x000D4E  WAKEINT - LPM/WD

//--- Core interrupt #2 re-map
    EPWM1_TZINT_ISR,                // 0x000D50  EPWM1_TZINT
    EPWM2_TZINT_ISR,                // 0x000D52  EPWM2_TZINT
    EPWM3_TZINT_ISR,                // 0x000D54  EPWM3_TZINT
    EPWM4_TZINT_ISR,                // 0x000D56  EPWM4_TZINT
    EPWM5_TZINT_ISR,                // 0x000D58  EPWM5_TZINT
    EPWM6_TZINT_ISR,                // 0x000D5A  EPWM6_TZINT
    rsvd_ISR,                       // 0x000D5C  reserved
    rsvd_ISR,                       // 0x000D5E  reserved

//--- Core interrupt #3 re-map
    EPWM1_INT_ISR,                  // 0x000D60  EPWM1_INT
    EPWM2_INT_ISR,                  // 0x000D62  EPWM2_INT
    EPWM3_INT_ISR,                  // 0x000D64  EPWM3_INT
    EPWM4_INT_ISR,                  // 0x000D66  EPWM4_INT
    EPWM5_INT_ISR,                  // 0x000D68  EPWM5_INT
    EPWM6_INT_ISR,                  // 0x000D6A  EPWM6_INT
    rsvd_ISR,                       // 0x000D6C  reserved
    rsvd_ISR,                       // 0x000D6E  reserved

//--- Core interrupt #4 re-map
    ECAP1_INT_ISR,                  // 0x000D70  ECAP1_INT
    ECAP2_INT_ISR,                  // 0x000D72  ECAP2_INT
    ECAP3_INT_ISR,                  // 0x000D74  ECAP3_INT
    ECAP4_INT_ISR,                  // 0x000D76  ECAP4_INT
    ECAP5_INT_ISR,                  // 0x000D78  ECAP5_INT
    ECAP6_INT_ISR,                  // 0x000D7A  ECAP6_INT
    rsvd_ISR,                       // 0x000D7C  reserved
    rsvd_ISR,                       // 0x000D7E  reserved

//--- Core interrupt #5 re-map
    EQEP1_INT_ISR,                  // 0x000D80  EQEP1_INT
    EQEP2_INT_ISR,                  // 0x000D82  EQEP2_INT
    rsvd_ISR,                       // 0x000D84  reserved
    rsvd_ISR,                       // 0x000D86  reserved
    rsvd_ISR,                       // 0x000D88  reserved
    rsvd_ISR,                       // 0x000D8A  reserved
    rsvd_ISR,                       // 0x000D8C  reserved
    rsvd_ISR,                       // 0x000D8E  reserved

//--- Core interrupt #6 re-map
    SPIRXINTA_ISR,                  // 0x000D90  SPIRXINTA
    SPITXINTA_ISR,                  // 0x000D92  SPIRXINTA
    MRINTB_ISR,                     // 0x000D94  MRINTB
    MXINTB_ISR,                     // 0x000D96  MXINTB
    MRINTA_ISR,                     // 0x000D98  MRINTA
    MXINTA_ISR,                     // 0x000D9A  MXINTA
    rsvd_ISR,                       // 0x000D9C  reserved
    rsvd_ISR,                       // 0x000D9E  reserved

//--- Core interrupt #7 re-map
    DINTCH1_ISR,                    // 0x000DA0  DINTCH1
    DINTCH2_ISR,                    // 0x000DA2  DINTCH2
    DINTCH3_ISR,                    // 0x000DA4  DINTCH3
    DINTCH4_ISR,                    // 0x000DA6  DINTCH4
    DINTCH5_ISR,                    // 0x000DA8  DINTCH5
    DINTCH6_ISR,                    // 0x000DAA  DINTCH6
    rsvd_ISR,                       // 0x000DAC  reserved
    rsvd_ISR,                       // 0x000DAE  reserved

//--- Core interrupt #8 re-map
    I2CINT1A_ISR,                   // 0x000DB0  I2CINT1A
    I2CINT2A_ISR,                   // 0x000DB2  I2CINT2A
    rsvd_ISR,                       // 0x000DB4  reserved
    rsvd_ISR,                       // 0x000DB6  reserved
    SCIRXINTC_ISR,                  // 0x000DB8  SCIRXINTC
    SCITXINTC_ISR,                  // 0x000DBA  SCITXINTC
    rsvd_ISR,                       // 0x000DBC  reserved
    rsvd_ISR,                       // 0x000DBE  reserved

//--- Core interrupt #9 re-map
    SCIRXINTA_ISR,                  // 0x000DC0  SCIRXINTA
    SCITXINTA_ISR,                  // 0x000DC2  SCITXINTA
    SCIRXINTB_ISR,                  // 0x000DC4  SCIRXINTB
    SCITXINTB_ISR,                  // 0x000DC6  SCITXINTB
    ECAN0INTA_ISR,                  // 0x000DC8  ECAN0_INTA
    ECAN1INTA_ISR,                  // 0x000DCA  ECAN1_INTA
    ECAN0INTB_ISR,                  // 0x000DCC  ECAN0_INTB
    ECAN1INTB_ISR,                  // 0x000DCE  ECAN1_INTB

//--- Core interrupt #10 re-map
    rsvd_ISR,                       // 0x000DD0  reserved
    rsvd_ISR,                       // 0x000DD2  reserved
    rsvd_ISR,                       // 0x000DD4  reserved
    rsvd_ISR,                       // 0x000DD6  reserved
    rsvd_ISR,                       // 0x000DD8  reserved
    rsvd_ISR,                       // 0x000DDA  reserved
    rsvd_ISR,                       // 0x000DDC  reserved
    rsvd_ISR,                       // 0x000DDE  reserved

//--- Core interrupt #11 re-map
    rsvd_ISR,                       // 0x000DE0  reserved
    rsvd_ISR,                       // 0x000DE2  reserved
    rsvd_ISR,                       // 0x000DE4  reserved
    rsvd_ISR,                       // 0x000DE6  reserved
    rsvd_ISR,                       // 0x000DE8  reserved
    rsvd_ISR,                       // 0x000DEA  reserved
    rsvd_ISR,                       // 0x000DEC  reserved
    rsvd_ISR,                       // 0x000DEE  reserved


//--- Core interrupt #12 re-map
    XINT3_ISR,                      // 0x000DF0  XINT3
    XINT4_ISR,                      // 0x000DF0  XINT4
    XINT5_ISR,                      // 0x000DF0  XINT5
    XINT6_ISR,                      // 0x000DF0  XINT6
    XINT7_ISR,                      // 0x000DF0  XINT7
    rsvd_ISR,                       // 0x000DFA  reserved
    LVF_ISR,                        // 0x000DFC  LVF - FPU
    LUF_ISR,                        // 0x000DFE  LUF - FPU

}; // end PieVectTableInit{}


//--- end of file -----------------------------------------------------

/**********************************************************************
* File: F28335_nonBIOS_flash.cmd -- Linker command file for DSP/BIOS
* code with DSP in jump-to-flash boot mode.
*
* History:
*   01/20/11 - Deleted unneeded Boot ROM memory definitions. (D. Alter)
*   09/18/07 - original (D. Alter)
**********************************************************************/

MEMORY
{
PAGE 0:    /* Program Memory */
   BEGIN_M0           : origin = 0x000000, length = 0x000002     /* Part of M0SARAM.  Used for "Boot to M0" bootloader mode. */
   L0123SARAM         : origin = 0x008000, length = 0x004000     /* L0 - L3 SARAM combined, CSM secure */
   FLASH_ABCDEFGH (R) : origin = 0x300000, length = 0x03FF80     /* On-chip FLASH */
   CSM_RSVD       (R) : origin = 0x33FF80, length = 0x000076     /* Part of FLASH Sector A.  Reserved when CSM is in use. */
   BEGIN_FLASH    (R) : origin = 0x33FFF6, length = 0x000002     /* Part of FLASH Sector A.  Used for "Jump to flash" bootloader mode. */
   PASSWORDS      (R) : origin = 0x33FFF8, length = 0x000008     /* Part of FLASH Sector A.  CSM password locations. */
   ADC_CAL    		  : origin = 0x380080, length = 0x000009
   OTP            (R) : origin = 0x380400, length = 0x000400     /* OTP */
   IQTABLES       (R) : origin = 0x3FE000, length = 0x000B50     /* Part of Boot ROM */
   IQTABLES2      (R) : origin = 0x3FEB50, length = 0x00008C     /* Part of Boot ROM */
   FPUTABLES      (R) : origin = 0x3FEBDC, length = 0x0006A0     /* Part of Boot ROM */
   RESET          (R) : origin = 0x3FFFC0, length = 0x000002     /* part of Boot ROM */

 PAGE 1 :   /* Data Memory */
   M0SARAM         : origin = 0x000002, length = 0x0003FE        /* M0 SARAM */
   M1SARAM         : origin = 0x000400, length = 0x000400        /* M1 SARAM */
   L4SARAM         : origin = 0x00C000, length = 0x001000        /* L4 SARAM, DMA accessible, 1 WS prog access */
   L5SARAM         : origin = 0x00D000, length = 0x001000        /* L5 SARAM, DMA accessible, 1 WS prog access */
   L6SARAM         : origin = 0x00E000, length = 0x001000        /* L6 SARAM, DMA accessible, 1 WS prog access */
   L7SARAM         : origin = 0x00F000, length = 0x001000        /* L7 SARAM, DMA accessible, 1 WS prog access */
}

 
SECTIONS
{
/*** Compiler Required Sections ***/
  /* Program memory (PAGE 0) sections */
   .text             : > FLASH_ABCDEFGH,        PAGE = 0
   .cinit            : > FLASH_ABCDEFGH,        PAGE = 0
   .const            : > FLASH_ABCDEFGH,        PAGE = 0
   .econst           : > FLASH_ABCDEFGH,        PAGE = 0      
   .pinit            : > FLASH_ABCDEFGH,        PAGE = 0
   .reset            : > RESET,                 PAGE = 0, TYPE = DSECT  /* We are not using the .reset section */
   .switch           : > FLASH_ABCDEFGH,        PAGE = 0

  /* Data Memory (PAGE 1) sections */
   .bss              : > L4SARAM,               PAGE = 1
   .ebss             : >> L4SARAM | L5SARAM | L6SARAM | L7SARAM    PAGE = 1
   .cio              : > L4SARAM,               PAGE = 1
   .stack            : > M1SARAM,               PAGE = 1
   .sysmem           : > L4SARAM,               PAGE = 1
   .esysmem          : > L4SARAM,               PAGE = 1
    DMARAML5         : > L5SARAM,     			PAGE = 1

/*** User Defined Sections ***/
   codestart         : > BEGIN_FLASH,           PAGE = 0                /* Used by file CodeStartBranch.asm */
   csm_rsvd          : > CSM_RSVD,              PAGE = 0                /* Used by file Passwords.asm */
   internalMemFuncs  : > FLASH_ABCDEFGH,        PAGE = 0                /* Used by file Xintf.c.  Link to internal memory */
   passwords         : > PASSWORDS,             PAGE = 0                /* Used by file Passwords.asm */
   secureRamFuncs    :   LOAD = FLASH_ABCDEFGH, PAGE = 0                /* Used by file Flash.c */
                         RUN = L0123SARAM,      PAGE = 0                /* Load to flash, run from CSM secure RAM */
                         LOAD_START(_secureRamFuncs_loadstart),
                         LOAD_SIZE(_secureRamFuncs_loadsize),
                         RUN_START(_secureRamFuncs_runstart)
/* Allocate ADC_cal function (pre-programmed by factory into TI reserved memory) */
	.adc_cal         : load = ADC_CAL,   		PAGE = 0, TYPE = NOLOAD

}

/******************* end of file ************************/

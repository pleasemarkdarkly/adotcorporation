/* THIS SOFTWARE IS PROVIDED "AS IS" WITH NO WARRANTIES EXPRESSED OR IMPLIED,
   INCLUDING WITHOUT LIMITATION WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
   PARTICULAR PURPOSE OR NON-INFRINGEMENT. LICENSEE BEARS ALL RISK OF 
   NON-PERFORMANCE, LOSS OF DATA, AND OTHER PROBLEMS AND LICENSOR WILL NOT BE
   LIABLE UNDER ANY CONTRACT, NEGLIGENCE, STRICT LIABILITY OR OTHER THEORY
   FOR ANY DAMAGES INCLUDING, WITHOUT LIMITATION, DIRECT, INCIDENTAL OR 
   CONSEQUENTIAL DAMAGES OR COST OF PROCUREMENT OF SUBSTITUTE GOODS, SERVICES
   OR TECHNOLOGY */

#ifndef _EDK_SW_H_
#define _EDK_SW_H_

/* Compiler switches */

/* DRIVE_MASTER
   Determines which device on the IDE controller that the driver 
   uses.  If true then uses the master.  If false then uses the 
   slave.
*/
#define DRIVE_MASTER 1

/* MACROS 
   Determines whether the register IO will use macros or 
   routines for accessing the IO.  Macros are quicker but use up
   more code space.  Routines are slower but optimize code space
*/
#define MACROS 1

/* USE_IRQ
   Determines whether to use the interrupt from the drive to signal 
   that the drive is not busy.  If false then the code will poll the
   status register until not busy.
*/
//#define USE_IRQ 0 // moved to edk_hw.h

/* MULTI_PROCESS
   Determines whether the procedure returns to host when waiting
   for the drive to be not busy.  Then the host can service a few
   things and then call ProcessInterrupt.  ProcessInterrupt will 
   check if the drive is ready to continue.  The is especially 
   useful in DMA mode because the host can let the DMA controller 
   handle the entire transfer and the host will be free to do other
   things.   
   
   It does not make too much sense to use this if DMA and interupts
   are not used (DMA=1, USE_IRQ=1).
*/
#define MULTI_PROCESS 0

/* BIT_RESET
   If set to 0 then this does a device reset by writing command 08h.
   If set to 1 then the Device Control Register is used and bit 4
   is toggled to 1 and then 0 called a bit reset.
   
   Note: A bit reset is usually more reliable however it resets both
   the master and the slave on the IDE controller.  If the iomega
   drive is the only device on the controller this does not matter.
   If however there is more than one device one the controller than 
   a device reset might be preferred.
*/
#define BIT_RESET 1

/* Host Buffer Size */
/* This determines the maximum number of bytes that the host can take 
   for each interrupt or block of data.
*/                     



/* Tools */
/* If the developer does want to use the edk_tool.c code then you must 
make one of the following equates equal to one.  If the developer does NOT 
want to use the tools then either make both equates zero or do not include
the file edk_tool.c in your project 
*/
#define USE_C_TOOLS 0
#define USE_ASM_TOOLS 0



#define HOST_BUFFER_HI 0x02
#define HOST_BUFFER_LO 0x00
                    
/* Common typedefs */
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef BYTE *PBUFFER;
                    
#endif /* _EDK_SW_H_ */

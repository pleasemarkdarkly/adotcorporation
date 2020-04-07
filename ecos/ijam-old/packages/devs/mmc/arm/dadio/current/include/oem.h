/*****************************************************************************
*Filename: OEM.H - Defines & structures for ms-dos utilities
*                     
* SanDisk Host Developer's Toolkit
*
* Copyright (c) 1997-1999 SanDisk Corporation
* All rights reserved.
* This code may not be redistributed in source or linkable object form
* without the consent of its author.
*
* Description: 
*       OEM function implementation.
*
****************************************************************************/

#ifndef __OEM__


#ifdef __cplusplus
extern "C" {
#endif



#include "sdtypes.h"
#include "sdconfig.h"

/* Function Prototypes implemented for PLATFORM specific */
/* These routines are system specific and implemented by
** the developers.
**
** The macros allow the run-time driver to HOOK the system
** specific code to the driver.
**/

/************************* DEPENDENT CODE *********************************/
/**************************************************************************/
/* These routines are system specific.  Mostly OEM dependent codes */

#if (!USE_MEMMODE)
UTINY inpbyte(UINT16 addr);
SDVOID outpbyte(UINT16 addr, UTINY dat);
  
#if (WORD_ACCESS_ONLY)
UINT16 inpword(UINT16 addr);
SDVOID outpword(UINT16 addr, UINT16 dat);
#endif
#endif

/*****************************************************************************
** These routines:
**      platform_critical_handler,
**      platform_report_error
** are in criterr.c and report.c and needed to be ported to
** the specific platform.
*****************************************************************************/

#if 0
/**************************** Critical Error ********************************/
/* Critical error handler routine is in criterr.c */
/* Interaction between users and the HDTK. */
INT16 platform_critical_handler(INT16 drv, INT16 errno, ULONG sector);
#define CRITICAL_HANDLER(drv, errno, sector)  (platform_critical_handler(drv, errno, sector))

/***************************** Error Report *********************************/
/* platform_report_error routine is in report.c */
/* Report the error to user */
SDVOID platform_report_error(INT16 err);
#define REPORT_ERROR(err)       (platform_report_error(err))
#endif


/********************************* Timer ************************************/
/*****************************************************************************
** These routines:
**      platform_delayms
**      platform_get_ticks,
**      platform_ticks_per_second,
**      oem_getsysdate
** are in timer.c and needed to be ported for to the specific platform.
*****************************************************************************/
SDVOID dev_platform_delayms(COUNT tdelay);    /* Time to delay in milisecond */
ULONG dev_platform_get_ticks(SDVOID);         /* Get the current tick */
ULONG dev_platform_ticks_p_second(SDVOID);    /* Ticks per second */

/* These macros are to allow HOOKING system specific
** code to the run-time driver.  The macros are used
** by the FILE SYSTEM and the INTERFACE drivers.
*/
#define OS_WAIT(tdelay)         (dev_platform_delayms(tdelay))
#define OS_GET_TICKS()          (dev_platform_get_ticks())
#define OS_TICKS_P_SECOND()     (dev_platform_ticks_p_second())


#if (USE_FILE_SYSTEM)
#if (RTFS_WRITE)
/* oem_getsysdate() in timer.c */
SDVOID oem_getsysdate(UINT16 *tdate, UINT16 *ttime);
#endif
#endif  /* USE_FILE_SYSTEM */



/*****************************************************************************
** These routines:
**      platform_controller_init,
**      platform_controller_close,
**      platform_clear_signal,
**      platform_set_signal,
**      platform_wait_for_action
** are in interupt.c and needed to be ported to the specific platform.
** These routines are for the interrupt implementation.
**
** The OS_XX_XX macros are for the FILE SYSTEM and INTERFACE drivers.
*****************************************************************************/

#if (USE_INTERRUPTS)

/*********************** IDE & PCMCIA interrupts ***************************/
SDVOID platform_controller_init(INT16 controller_no);
SDVOID platform_controller_close(INT16 controller_no);
SDVOID platform_clear_signal(INT16 controller_no);
SDVOID platform_set_signal(INT16 controller_no);
SDBOOL platform_wait_for_action(INT16 controller_no, COUNT time_out);

#define OS_CONTROLLER_INIT(controller_no)  (platform_controller_init(controller_no))
#define OS_CONTROLLER_CLOSE(controller_no) (platform_controller_close(controller_no))
#define OS_CLEAR_SIGNAL(controller_no)     (platform_clear_signal(controller_no))
#define OS_SET_SIGNAL(controller_no)       (platform_set_signal(controller_no))
#define OS_WAIT_FOR_ACTION(controller_no, time_out)  (platform_wait_for_action(controller_no, time_out))

#else   

#define OS_CONTROLLER_INIT(controller_no)
#define OS_CONTROLLER_CLOSE(controller_no)
#define OS_CLEAR_SIGNAL(controller_no)
#define OS_SET_SIGNAL(controller_no)
#define OS_WAIT_FOR_ACTION(controller_no, time_out) YES

#endif  /* USE_INTERRUPTS */

/*****************************************************************************
********************** Transfer speed related topics *************************
*****************************************************************************/
#if (USE_SPI || USE_SPI_EMULATION)

/* Intialize SPI hardware */
SDVOID spi_hw_init(INT16 controllerno);

/* Enable Chip Select */
SDVOID spi_cs_enable(INT16 driveno);

/* Disable Chip Select */
SDVOID spi_cs_disable(INT16 driveno);

#if (!USE_SET_FEATURES)
#else
UINT16 calculateDataCRC16 ( UCHAR *pdata, UINT16 data_length );
UINT16 checkResponseCRC ( UCHAR *response, UINT16 offset, UINT16 response_length );
UINT16 appendCmdCRC ( UCHAR *cmd_bytes, UINT16 cmd_length );

#endif /* (!USE_SET_FEATURES) */

UINT16 calculateCmdCRC(UINT32 Args, UINT16 cmdIndex);
SDVOID startMMC80Clocks(INT16 ctrlno);
SDVOID mmcSendCommand(UINT32 cmd_arg, UINT16 cmd_index, UINT16 crcCMD);
MMC_CC getMMCResponseInfo(UINT16 *respBuff, UINT16 respType);
MMC_CC MMCPrepareAndSetup(UINT32 Arg, UINT16 Cmd, UINT16 noBlocks, UINT16 Resp);
MMC_CC checkCardBusy(INT16 ctrlno);
MMC_CC MMCTransmit(UCHAR *buffer, UINT16 dataLength);
MMC_CC MMCReceive(UCHAR *buffer, UINT16 dataLength);

#endif  /* (USE_SPI || USE_SPI_EMULATION) */


#if (USE_MMC || USE_MMC_EMULATION)

UINT16 calculateDataCRC16 ( UCHAR *pdata, UINT16 data_length );
UINT16 checkResponseCRC ( UCHAR *response, UINT16 offset, UINT16 response_length );
UINT16 appendCmdCRC ( UCHAR *cmd_bytes, UINT16 cmd_length );
UINT16 calculateCmdCRC(UINT32 Args, UINT16 cmdIndex);

SDVOID mmc_hw_init(INT16 driveno);
SDVOID startMMC80Clocks(INT16 ctrlno);
MMC_CC MMCPrepareAndSetup(UINT32 Arg, UINT16 Cmd, UINT16 noBlocks, UINT16 Resp);
MMC_CC getMMCResponseInfo(UCHAR *respBuff, UINT16 respLength, UINT16 respType);
extern MMC_CC (*MMCTransmit)(UCHAR *buffer, UINT16 dataLength);
extern MMC_CC (*MMCReceive)(UCHAR *buffer, UINT16 dataLength);
UINT16 getMMCStatus(SDVOID);
MMC_CC checkCardBusy( INT16 controller_no );
SDVOID resetMMCController(UINT16 ctrlno);
SDVOID startMMCClock(SDVOID);
SDBOOL stopMMCClock(SDVOID);
SDBOOL setMMCClockRate(UINT16 ClockRate);
SDVOID MMCSelectController( INT16 ctrlno );
  
  
#endif

/* Static checking for device present */
SDBOOL is_device_changed (INT16 driveno);
INT16  get_extended_error(INT16 driveno);


/************************* END of DEPENDENT CODE ****************************/


#ifdef __cplusplus
}
#endif


#define __OEM__

#endif




/******************************************************************************
* Filename: INTRFACE.H - Interface layer for IDE, PCMCIA, SPI, MMC
*                     
* SanDisk Host Developer's Toolkit
*
* Copyright (c) 1997-1998 SanDisk Corporation
* All rights reserved.
* This code may not be redistributed in source or linkable object form
* without the consent of its author.
*
* Description: 
*       Provide the initial state, device status and closing state
*       for IDE, PCMCIA, SPI, or MMC controller.
*
******************************************************************************/

#ifndef __INTRFACE_H__

#include "sdtypes.h"
#include "sdconfig.h"

#ifdef __cplusplus
extern "C" {
#endif


#if (USE_FILE_SYSTEM)

/******************************** SPI *************************************/
#if (USE_SPI || USE_SPI_EMULATION)


/*************************** SPI INTERFACE ********************************/

/* spi.c */
/* Initialize SPI controller */
#define CONTROLLER_INIT()       (spi_init())

#define INTERFACE_DEVICE_CLOSE(driveno)        (spi_drive_close(driveno))

#define INTERFACE_DEVICE_OPEN(driveno)  (spi_drive_open(driveno))

#define DEV_READ_SERIAL(drive_no, idDrvPtr)    (\
                        spi_read_serial(drive_no, idDrvPtr))

#define DEV_READ(driveno, sector, buffer, count) (\
                        spi_read(driveno, sector, buffer, count))

#define DEV_WRITE(driveno, sector, buffer, count) (\
                        spi_write(driveno, sector, buffer, count))

#define DEV_SECTORS_ERASE(driveno, sector, count) (\
                        spi_erase(driveno, sector, count))

#endif          /* (USE_SPI || USE_SPI_EMULATION) */



/******************************** MMC *************************************/
#if (USE_MMC || USE_MMC_EMULATION)

/* mmc.c */
#if 1
#include "iome_mmc_fat.h"

#define CONTROLLER_INIT(x)       (iome_fat_mmc_init(x))

#define INTERFACE_DEVICE_CLOSE(driveno)        (iome_mmc_drive_close(driveno))

#define INTERFACE_DEVICE_OPEN(driveno)  (iome_mmc_drive_open(driveno))

#define DEV_READ_SERIAL(drive_no, idDrvPtr)    (\
                        iome_mmc_read_serial(drive_no, idDrvPtr))

#define DEV_READ(driveno, sector, buffer, count) (\
                        iome_mmc_read(driveno, sector, buffer, count))

#define DEV_WRITE(driveno, sector, buffer, count) (\
                        iome_mmc_write(driveno, sector, buffer, count))

#define DEV_SECTORS_ERASE(driveno, sector, count) (\
                        iome_mmc_erase(driveno, sector, count))

#else
/* Initialize MMC controller */
#define CONTROLLER_INIT(x)       (mmc_init(x))

#define INTERFACE_DEVICE_CLOSE(driveno)        (mmc_drive_close(driveno))

#define INTERFACE_DEVICE_OPEN(driveno)  (mmc_drive_open(driveno))

#define DEV_READ_SERIAL(drive_no, idDrvPtr)    (\
                        mmc_read_serial(drive_no, idDrvPtr))

#define DEV_READ(driveno, sector, buffer, count) (\
                        mmc_read(driveno, sector, buffer, count))

#define DEV_WRITE(driveno, sector, buffer, count) (\
                        mmc_write(driveno, sector, buffer, count))

#define DEV_SECTORS_ERASE(driveno, sector, count) (\
                        mmc_erase(driveno, sector, count))

#endif
#endif          /* (USE_MMC || USE_MMC_EMULATION) */


/* Drive Formating and mapping information */ 
SDVOID drive_format_information(INT16 driveno, UINT16 *n_heads, UINT16 *sec_ptrack, UINT16 *n_cyls);

/* Interface error routine */
SDIMPORT INT16 get_interface_error(INT16 driveno);


#endif  /* (USE_FILE_SYSTEM) */


#ifdef __cplusplus
}
#endif


#define __INTRFACE_H__

#endif


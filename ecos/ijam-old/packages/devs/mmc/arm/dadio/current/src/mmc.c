/*****************************************************************************
* FileName: MMC.C - MMC interface driver
*
* SanDisk Host Developer's Toolkit
*
* Copyright (c) 1997 - 1999 SanDisk Corporation
* All rights reserved.
* This code may not be redistributed in source or linkable object form
* without the consent of its author.
*
* Description:
*
*       MMC Interface driver routines.
*
******************************************************************************/

#include <dadio/io/sdmmc.h>

/****************************************************************************/
/*******************************  MMC INTERFACE  ****************************/
/****************************************************************************/

/*****************************************************************************
* Name: mmc_init
*
* Processing:
*       Initialize data structure for MMC controller and
*       start up devices on the selected bus
*
* Input:
*       None
*
* Output:
*       Internal data structures are configured
*
* Returns:
*       YES if successful
*       NO if failure
*       
*****************************************************************************/
SDBOOL mmc_init( INT16 controllerno ) /*__fn__*/
{
	INT16 i;

        /* Clear internal drive structure */
	clear_controller_structure( controllerno );
#if 0
        /* Initialize the MMC hardware in known state. */
        for (i = 0; i < N_INTERFACES; i++)
		mmc_hw_init(i);
#endif
	mmc_hw_init( controllerno );
        /* Put all devices in READY STATE. */
        /* This is part of the IDENTIFICATION STATE process. */
        return (mmc_init_setup( controllerno ));
}



/*****************************************************************************
* Name: mmc_drive_open
*
* Processing:
*       Set up internal data structure and configure the system.
*
* Entries:
*       INT16   driveno         Driver number
*
* Returns:
*       YES if successful
*       NO if failure
*       
*****************************************************************************/
SDBOOL mmc_drive_open ( INT16 driveno ) /*__fn__*/
{
	return(mmedia_drive_open(driveno));
}



/*****************************************************************************
* Name: mmc_drive_close
*
* Processing:
*       Release internal data structure and restore memory configuration
*
* Entries:
*       INT16   driveno         Driver number
*
* Returns:
*       YES if successful
*       NO if failure
*       
*****************************************************************************/
SDBOOL mmc_drive_close ( INT16 driveno ) /*__fn__*/
{
	return(mmedia_drive_close(driveno));
}


/*****************************************************************************
* Name: mmc_read_serial
*
* Processing:
*       Get the drive geometry information.
*
* Entries:
*       INT16           driveno         Driver number
*       PDRV_GEOMETRY_DESC idDrvPtr     Pointer to the drive geometry struture
*
* Returns:
*       YES if successful
*       NO if failure
*       
*****************************************************************************/
SDBOOL mmc_read_serial(INT16 driveno, PDRV_GEOMETRY_DESC idDrvPtr) /* __fn__*/
{
	return(mmedia_read_serial(driveno, idDrvPtr));
}



/*****************************************************************************
* Name: mmc_read
*
* Processing:
*       Get data from the drive at the requested LBA.
*
* Entries:
*       INT16   driveno         Driver number
*
* Returns:
*       YES if successful
*       NO if failure
*       
*****************************************************************************/
SDBOOL mmc_read(INT16 driveno, ULONG startingSector, UCHAR *buff, UCOUNT numSectors) /*__fn__*/
{
	if (mmedia_io(driveno, startingSector, buff, numSectors, READING))
		return (NO);

	return(YES);
}


/*****************************************************************************
* Name: mmc_write
*
* Processing:
*       Send information to the specific location on the drive.
*
* Entries:
*       INT16   driveno         Driver number
*
* Returns:
*       YES if successful
*       NO if failure
*       
*****************************************************************************/
SDBOOL mmc_write(INT16 driveno, ULONG startingSector, UCHAR *buff, UCOUNT numSectors) /*__fn__*/
{
	if (mmedia_io(driveno, startingSector, buff, numSectors, WRITING))
		return (NO);

	return(YES);
}



/*****************************************************************************
* Name: mmc_erase
*
* Processing:
*       Clear the LBA to make it ready for the next write
*
* Entries:
*       INT16   driveno         Driver number
*
* Returns:
*       YES if successful
*       NO if failure
*       
*****************************************************************************/
SDBOOL mmc_erase(INT16 driveno, ULONG startingSector, UCOUNT numSectors) /*__fn__*/
{
        if (mmedia_io(driveno, startingSector, SDNULL, numSectors, ERASING))
		return (NO);

	return(YES);
}





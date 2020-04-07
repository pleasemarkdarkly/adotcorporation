/******************************************************************************
* Filename: ioconst.c - Data Intitialzation
*
* SanDisk Host Developer's Toolkit
*
* Copyright (c) 1997-1998 SanDisk Corporation
* All rights reserved.
* This code may not be redistributed in source or linkable object form
* without the consent of its author.
*
* Description:
*       Constant declarations 
*
******************************************************************************/

#define INTERFACE_DATA_STRUCT
#include <dadio/io/drive.h>


/******************************** SPI Section ********************************/
/*****************************************************************************/
#if (USE_SPI || USE_SPI_EMULATION)

#if (USE_MEMMODE)       /* Memory mapped mode */
 const ULONG mem_mapped_addresses[N_INTERFACES] = {

  #if (N_INTERFACES > 0)
   SPI_BASE_ADDR,
  #endif
  #if (N_INTERFACES > 1)
   SPI_BASE_ADDR
  #endif
};
 const FPTR08 mem_mapped_addresses_pointer[N_INTERFACES] = {

  #if (N_INTERFACES > 0)
   (FPTR08)SPI_BASE_ADDR,
  #endif
  #if (N_INTERFACES > 1)
   (FPTR08)SPI_BASE_ADDR
  #endif
};

#else                   /* IO mapped mode */
 const UINT16 io_mapped_addresses[N_INTERFACES] = {

  #if (N_INTERFACES > 0)
   SPI_PRIMARY_IO_ADDRESS,
  #endif
  #if (N_INTERFACES > 1)
   SPI_SECONDARY_IO_ADDRESS
  #endif
};
#endif

#if (USE_INTERRUPTS)
 const INT16 dev_interrupts[N_INTERFACES] = {

  #if (N_INTERFACES > 0)
   SPI_PRIMARY_INTERRUPT,
  #endif
  #if (N_INTERFACES > 1)
   SPI_SECONDARY_INTERRUPT
  #endif
};

#endif	/* USE_INTERRUPTS */

#endif /* (USE_SPI || USE_SPI_EMULATION) */




/******************************** MMC Section ********************************/
/*****************************************************************************/
#if (USE_MMC || USE_MMC_EMULATION)

#if (USE_MEMMODE)       /* Memory mapped mode */
 const ULONG mem_mapped_addresses[N_INTERFACES] = {

  #if (N_INTERFACES > 0)
   MMC_BASE_ADDR,
  #endif
  #if (N_INTERFACES > 1)
   MMC_BASE_ADDR
  #endif
};
 const FPTR08 mem_mapped_addresses_pointer[N_INTERFACES] = {

  #if (N_INTERFACES > 0)
   (FPTR08)MMC_BASE_ADDR,
  #endif
  #if (N_INTERFACES > 1)
   (FPTR08)MMC_BASE_ADDR
  #endif
};

#else                   /* IO mapped mode */
 const UINT16 io_mapped_addresses[N_INTERFACES] = {

  #if (N_INTERFACES > 0)
   MMC_PRIMARY_IO_ADDRESS,
  #endif
  #if (N_INTERFACES > 1)
   MMC_SECONDARY_IO_ADDRESS
  #endif
};
#endif

#if (USE_INTERRUPTS)
 const INT16 dev_interrupts[N_INTERFACES] = {

  #if (N_INTERFACES > 0)
   MMC_PRIMARY_INTERRUPT,
  #endif
  #if (N_INTERFACES > 1)
   MMC_SECONDARY_INTERRUPT
  #endif
};

#endif	/* USE_INTERRUPTS */

#endif /* (USE_MMC || USE_MMC_EMULATION) */



/* To allow more flexible way to configure the sub-system and reduce
** RAM footage.
*/

DEVICE_CONTROLLER controller_s[N_INTERFACES];

/* Allocate space for devices connecting to the first controller */
#if (DRIVES_PER_CONTROLLER1 > 0)
DRIVE_DESC ddrive1[DRIVES_PER_CONTROLLER1];
#endif

/* Allocate space for devices connecting to the second controller */
#if (DRIVES_PER_CONTROLLER2 > 0)
DRIVE_DESC ddrive2[DRIVES_PER_CONTROLLER2];
#endif


/* Device control structure */
/* Drives per controller information array */
INT16 drvs_per_controller[N_INTERFACES] = {

#if (DRIVES_PER_CONTROLLER1 > 0)
 DRIVES_PER_CONTROLLER1,
#endif
#if (DRIVES_PER_CONTROLLER2 > 0)
 DRIVES_PER_CONTROLLER2,
#endif
};

/* Create the drive structure array */
UCHAR *drive_ptr[N_INTERFACES] = {

 #if (DRIVES_PER_CONTROLLER1 > 0)
  (UCHAR *)ddrive1,
 #endif
 #if (DRIVES_PER_CONTROLLER2 > 0)
  (UCHAR *)ddrive2,
 #endif
};


/*****************************************************************************
* Name: clear_controller_structure
*
* Description:
*       Initialize the controller structure to a known value.
*
* Entries:
*       INT16   controllerno    Controller Number
*
* Returns:
*       None
*
******************************************************************************/
SDVOID clear_controller_structure( INT16 controllerno ) /*__fn__*/
{
	PDEVICE_CONTROLLER pc;

	pc = &controller_s[ controllerno ];
	pc->controller_number = -1;
	pc->interrupt_number = -1;
        pc->opencount = NO;
	pc->register_file_address = io_mapped_addresses[controllerno];
	pc->drive = (DRIVE_DESC *)drive_ptr[controllerno];
	
#if 0
	INT16 i;
	/* Initialize internal data structure to a known state */
	for (i = 0; i < N_INTERFACES; i++)
	{
		/* clear internal drive structure */
		pc = &controller_s[i];

		pc->controller_number = -1;
		pc->interrupt_number = -1;
                pc->opencount = NO;
#if (USE_MEMMODE)
                pc->register_file_address = (FPTR08)mem_mapped_addresses_pointer[i];
#else
		pc->register_file_address = io_mapped_addresses[i];
#endif
		/* Get the drive structure infomation */
		pc->drive = (DRIVE_DESC *)drive_ptr[i];

	}
#endif
}


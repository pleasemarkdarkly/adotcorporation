/*****************************************************************************
* Filename: drive.h  -  Controller structure information
*
* SanDisk Host Developer's Toolkit
*
* Copyright (c) 1997 - 1998 SanDisk Corporation
* All rights reserved.
* This code may not be redistributed in source or linkable object form
* without the consent of its author.
*
* Description:
*       Portable Peripheral Bus Interface Device driver.
*
******************************************************************************/

#ifndef _DRIVE_H_

#include "sdtypes.h"
#include "sdconfig.h"


#ifdef __cplusplus
extern "C" {
#endif



/* NOT intended to be CHANGED by user */
#define USE_PREERASE_SECTORS    (PREERASE_ON_DELETE|PREERASE_ON_ALLOC|PREERASE_ON_FORMAT)


/* Don't allow more then 128 blocks (64K) for fear of a segment wrap. */
#define MAX_BLOCKS  128


/* Current active device flag */
#define DRV_ACTIVE      0x80

/* First drive */
#define FIRST_DRIVE     0

#define  N_INTERFACES  N_CONTROLLERS

/* Internal READ, WRITE, ERASE Command operations */
#define READING 1
#define WRITING 2
#define ERASING 3


/* drive_control structure - We use this abstraction to manage the driver. 
	Structure contents:
		.  drive description. logical drive structure (heads/secptrak/secpcyl)
		.  virtual registers: These aren't (can't be) mapped onto the
		the controller but we use virtual representation on the
		register file to drive the system.
		.  virtual output register file (to drive) - We load the registers
		and then ask lower level code to send the command block to the 
		controller.
		.  virtual input register file (from drive) - We offload the register
		file from the drive into these fields after a command complete.
		Note: We only offload the whole register file in certain cases.
			The status register is always offloaded. 
*/

/* The control structure contains one of these per drive [2]. We set this
	up at init time after issueing SET_PARAMS. The values are used to convert
	block number to track:head:sector
*/
typedef struct drive_desc
{
	ULONG       total_lba;
	UINT16      num_heads;
	UINT16      sec_p_track;
	UINT16      block_size;
	UINT16      num_cylinders;
	UINT16      max_multiple;
	UINT16      supports_lba;
} DRIVE_DESC, *PDRIVE_DESC;


typedef struct drive_control
{
	/* Geometry of the drives */
	PDRIVE_DESC drive;
	USERADDRESS user_address;/* Pointer address of the user's buffer */

	/* Address of the controller in memory or IO mode */
#if (USE_MEMMODE)
	FPTR    register_file_address;
#else
	UINT16 register_file_address;
#endif
	SDBOOL command_complete;  /* Set when operation is complete */
	UINT16 opencount;
	INT16 interrupt_number; /* -1 if not being used */
	COUNT controller_number;

	UINT16 error_code;      /* Set if an error occured - This is 
				   the error reported to the user.
				*/
	UINT16 timer;           /* Command fails if this much time elapsed */
	UINT16 block_size;      /* Block size for read/write multiple cmds */
	UCOUNT sectors_remaining;/* Sectors remaining for read/write mult */

	INT16 drive_active;     /* Current active drive */
	UINT16 mode;
	UINT16 currentState;
	UINT16 mmcRdyState;
	UINT16 tempData;
	UINT16 LastResponse[8];
	UINT32 mmcStatus;
} DEVICE_CONTROLLER, *PDEVICE_CONTROLLER;



/* INTERFACE_DATA_STRUCT is defined in IOCONST.C */
/* This flag is needed for some compilers to avoid seeing the following
*  declarations as redefinition.
*/
#ifndef INTERFACE_DATA_STRUCT

/* Allocate space for controllers in the system */
#if (N_INTERFACES)
/* Device control structure information */
 SDIMPORT DEVICE_CONTROLLER controller_s[N_INTERFACES];
 SDIMPORT INT16 drvs_per_controller[N_INTERFACES];
 SDIMPORT UCHAR *drive_ptr[N_INTERFACES];
#endif


/* Allocate space for devices connecting to the first controller */
#if (DRIVES_PER_CONTROLLER1 > 0)
SDIMPORT DRIVE_DESC ddrive1[DRIVES_PER_CONTROLLER1];
#endif


/* Allocate space for devices connecting to the second controller */
#if (DRIVES_PER_CONTROLLER2 > 0)
SDIMPORT DRIVE_DESC ddrive2[DRIVES_PER_CONTROLLER2];
#endif


#if (USE_MEMMODE)
SDIMPORT const ULONG mem_mapped_addresses[N_INTERFACES]; 
SDIMPORT const FPTR mem_mapped_addresses_pointer[N_INTERFACES];
#else
SDIMPORT const UINT16 io_mapped_addresses[N_INTERFACES];
#endif

#if (USE_INTERRUPTS)
SDIMPORT const INT16 dev_interrupts[N_INTERFACES];
#endif

#endif  /* INTERFACE_DATA_STRUCT */



/* For multiple controllers */
#if (N_INTERFACES > 1)
INT16 dev_drno_to_controller_no(INT16 driveno);
PDEVICE_CONTROLLER dev_drno_to_controller(INT16 driveno);
INT16 dev_drno_to_phys(INT16 driveno);
INT16 dev_get_controller_number(INT16 driveno);
/* INT16 controller_to_drvno(INT16 controllerno); */
#endif

/* Controller structure Initialization */
SDVOID clear_controller_structure( INT16 controllerno );


#if (USE_FILE_SYSTEM)
SDVOID system_controller_init(INT16 driveno);
SDVOID system_controller_close(INT16 driveno);

SDIMPORT UINT16 *lock_temporary_buffer(SDVOID);
SDIMPORT SDVOID release_temporary_buffer(SDVOID);
#endif



#ifdef __cplusplus
}
#endif


#define _DRIVE_H_

#endif

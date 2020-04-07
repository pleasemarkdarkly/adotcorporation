/* THIS SOFTWARE IS PROVIDED "AS IS" WITH NO WARRANTIES EXPRESSED OR IMPLIED,
   INCLUDING WITHOUT LIMITATION WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
   PARTICULAR PURPOSE OR NON-INFRINGEMENT. LICENSEE BEARS ALL RISK OF 
   NON-PERFORMANCE, LOSS OF DATA, AND OTHER PROBLEMS AND LICENSOR WILL NOT BE
   LIABLE UNDER ANY CONTRACT, NEGLIGENCE, STRICT LIABILITY OR OTHER THEORY
   FOR ANY DAMAGES INCLUDING, WITHOUT LIMITATION, DIRECT, INCIDENTAL OR 
   CONSEQUENTIAL DAMAGES OR COST OF PROCUREMENT OF SUBSTITUTE GOODS, SERVICES
   OR TECHNOLOGY */

#include <dadio/io/atapi.h>
#include <dadio/io/edk_sw.h>
#include <dadio/io/edk_hw.h>

#ifndef _EDK_H_
#define _EDK_H_

#define _ECOS_

#define SCRATCH_BUFFER_SIZE 14
                                     
/* Feature Register */
#define FEATURE_NODMA 0x00
#define FEATURE_DMA   0x01
                          
/* Drive Select Register */
#define SELECT_MASTER  0xA0                  
#define SELECT_SLAVE 0xB0
#define SELECT_LBA 0x40

/* Error Register */
#define ERROR_WP   0x40
#define ERROR_UNC  0x40
#define ERROR_MC   0x20
#define ERROR_IDNF 0x10
#define ERROR_MCR  0x08
#define ERROR_ABRT 0x04
#define ERROR_NM   0x02

/* Error Register for EXECUTE DRIVE DIAGNOSTIC */
#define NO_ERROR_DETECTED 0x01
/* NOTE the following entries may only be valid for CF */
#define FORMATTER_DEVICE_ERROR 0x02
#define SECTOR_BUFFER_ERROR 0x03
#define ECC_CIRCUITRY_ERROR 0x04
#define CONTROLLING_uP_ERROR 0x05

/* Status Register */                          
#define STATUS_BSY 0x80                          
#define STATUS_DRQ 0x08
#define STATUS_CHECK 0x01

/* Interrupt Reason Register */
#define INTERRUPT_COMMAND   0x01
#define INTERRUPT_IO	    0x02                          

/* Command Register */
#define COMMAND_DEVICE_RESET  0x08
#define COMMAND_ATAPI_PACKET  0xA0
#define COMMAND_ATAPI_ID      0xA1
#define COMMAND_SLEEP         0xE6
                                     
/* Control Register */
#if USE_IRQ
#define CONTROL_NORMAL        0x00
#else
#define CONTROL_NORMAL        0x02    // dc: disable irq off device
#endif
#define CONTROL_SRST          0x04                                     

/* ATA Commands */
#define ATA_EXECUTE_DEVICE_DIAGNOSTIC 0x90
#define ATA_IDENTIFY_DEVICE           0xec
#define ATA_WRITE_SECTORS             0x30
#define ATA_READ_SECTORS              0x20
#define ATA_CHECK_POWER_MODE          0xe5

/* ATAPI Packet Commands */
#define ATAPI_REQUEST_SENSE   0x03
#define ATAPI_NON_SENSE       0x06
#define ATAPI_PROTECT         0x0C
#define ATAPI_START_STOP      0x1B
#define ATAPI_PREVENT_ALLOW   0x1E
#define ATAPI_READ_CAP        0x25
#define ATAPI_READ10          0x28
#define ATAPI_WRITE10         0x2A

              
/* External Procedure Declarations */              
              
/******************************************************************************
SendAtapiPacket - This routine can be used to send any ATAPI command to the 
                  drive.  
                  
	Parameters:  PBUFFER cdb  - pointer to the packet (12 bytes of data)
	             PBUFFER data - pointer to data buffer
	             				Needs to be as large of a buffer as neccessary
	             				for the command being sent to the drive.
	Returns: BYTE - status  All routines that have a BTYE status return the following:   
							00     No Error 
							01-EF  Additional sense code of last packet command sent
							       (see OEM Tech Manual)
							   3A  No Media in drive
                            FC     Drive Asleep
                            FD     Invalid Phase
                            FE     Waiting for device (MULTI_PROCESS)
							FF     Timeout Error

	Calls:	
		ProcessInterrupt     (API)
		WriteFeatures        (edk_hw)
		WriteByteCountHigh   (edk_hw)
		WriteByteCountLow    (edk_hw)
		WriteCommand         (edk_hw)
		WriteDataBulk        (edk_hw)
		PollStatus           (local)
		DeviceSelection      (local)
*/	                      		                  
BYTE SendAtapiPacket(PBUFFER cdb, PBUFFER data);
#define NO_ERROR                 0x00
#define ERROR_READ_ERROR         0x11
#define ERROR_INVALID_COMMAND    0x20
#define ERROR_LBA_OUT_OF_RANGE   0x21
#define ERROR_INVALID_FIELD      0x24
#define ERROR_WRITE_PROTECTED    0x27
#define ERROR_MEDIA_CHANGED      0x28
#define ERROR_NO_MEDIA           0x3A
#define ERROR_DIAGNOSTIC_FAILURE 0x40

#define ERROR_ABORT              0xfb
#define ERROR_SLEEP              0xFC
#define ERROR_OUT_OF_PHASE       0xFD
#define ERROR_WAIT_FOR_DEVICE    0xFE
#define ERROR_TIMEOUT            0xFB


/******************************************************************************
WriteAtapi - This routine will use the SendAtapiPacket routine to write to a
             specified LBA from a specified buffer.
             
	Parameters: DWORD lba    - the LBA on disk where write will occur
	            WORD length  - the number of sectors (512 bytes) to write
	            PBUFFER data - pointer to data buffer
	            
	Returns: BYTE - status  (See SendAtapiPacket)

	Calls:
		SendAtapiPacket        (API)
		memset                 (edk_tool)
*/	                           
BYTE WriteAtapi(DWORD lba, WORD length, PBUFFER data);


/******************************************************************************
ReadAtapi - This routine will use the SendAtapiPacket routine to read from a
             specified LBA to a specified buffer.
             
	Parameters: DWORD lba    - the LBA on disk where read will occur
	            WORD length  - the number of sectors (512 bytes) to read
	            PBUFFER data - pointer to data buffer
	            
	Returns: BYTE - status  (See SendAtapiPacket)

	Calls:
		SendAtapiPacket        (API)
		memset                 (edk_tool)
*/	                           
BYTE ReadAtapi(DWORD lba, WORD length, PBUFFER data);


/******************************************************************************
ResetAtapi - This routine will reset the ATAPI device

	Parameters: none
	
	Returns: BYTE - status  (See SendAtapiPacket)

    Calls:
    	UnmaskInterrupt        (edk_hw)
    	WriteControl           (edk_hw)
    	WriteDriveSelect       (edk_hw)
    	WriteCommand           (edk_hw)
	PollStatus             (local)
*/
BYTE ResetAtapi(void);

/******************************************************************************
InitAtapi - This routine should be called every time the drive is mounted 
            (powered on).
            
	Parameters: none
	
	Returns: BYTE - status  (See SendAtapiPacket)

	Calls:
		SendAtapiPacket       (API)
		ProcessInterrupt      (API)		
		InitializeInterrupt   (edk_hw)
		GetOSTicks            (edk_hw)
		memset                (edk_tool)
*/
BYTE InitAtapi(void);

/******************************************************************************
DriveStatus - This routine will return the status of the drive

	Parameters: none
	                           
	Returns: BYTE - status  (See SendAtapiPacket)

	Calls:
		SendAtapiPacket       (API)
		memset                (edk_tool)
*/
BYTE DriveStatus(void);

/******************************************************************************
IdentifyAtapi - This routine will do a Identify Atapi command to the drive.

	Parameters: PBUFFER data - pointer to data buffer where identify information 
	                           will be stored (512 bytes)
	                           
	Returns: BYTE - status  (See SendAtapiPacket)

	Calls:
		ReadDataBulk         (edk_hw)		
		WriteCommand         (edk_hw)
		DeviceSelection      (local)
		WaitForInterrupt   	 (local)
		PollStatus           (local)
*/
BYTE IdAtapi(PBUFFER data);

/******************************************************************************
SleepDrive - This will send an ATA command make the drive go into sleep mode.

	Parameters: none
	
	Returns: BYTE - status  (See SendAtapiPacket)
	
	Calls:
		 WriteCommand        (edk_hw)
		 MaskInterrupt       (edk_hw)
		 DeviceSelection     (local)
		 WaitForInterrupt    (local)     
		 PollStatus          (local)
*/
BYTE SleepDrive(void);	
              
/******************************************************************************
ProcessInterrupt - Used only in MULTI_PROCESS mode.  In MULTI_PROCESS mode, if 
a command returns with a 0FEh, then it means that the driver is waiting for the
drive to be ready and the processor can service a quick routine and then calls
ProcessInterrupt to finish the command.

	Parameters: none
	
	Returns: BYTE - status  (See SendAtapiPacket)

	Calls:
		SendAtapiPacket       (API)
		ProcessInterrupt      (API)
		ReadStatus            (edk_hw)   
		ReadInterruptReason   (edk_hw) 
		ReadByteCountHigh     (edk_hw)
		ReadByteCountLow      (edk_hw)
		WriteDataBulk         (edk_hw)
		ReadDataBulk          (edk_hw)
		memset                (edk_tool)
		WaitForInterrupt      (local)
		PollStatus            (local)   
*/
BYTE ProcessInterrupt(void);

/******************************************************************************
PreventAllow- This will send a Prevent/Allow  Media Removal Packet Command (1Eh)
to the drive.  This enables or disables the eject button.  

NOTE: Not all Iomega products support this or some do not have an eject button.

	Parameters: BYTE flag      Indicates whether to enable or disable the eject button.
	                           00 Sets the ALLOW mode and enables the cartridge eject
	                              button
	                           01 Sets the PREVENT mode and disables the cartridge
	                              eject button.
	
	Returns: BYTE - status  (See SendAtapiPacket)
	
	Calls:
		SendAtapiPacket  (API)		
		memset           (edk_tool)
*/
BYTE PreventAllow(BYTE flag);

/******************************************************************************
EjectMedia - This command will send an eject command to the drive

	Parameters: None
	
	Returns: BYTE - status  (See SendAtapiPacket)

	Calls:
		SendAtapiPacket  (API)		
		memset           (edk_tool)
*/                                                        
BYTE EjectMedia(void);

/******************************************************************************
CartridgeProtect - This will send a Cartridge Protect Packet Command (0Ch) to
the drive.  This can write protect, read/write protect.  Each of these
can be with or without a password.

	Parameters: BYTE flag      - Indicates the protection mode
	                           00 Unlock
	                           02 Write Protect
	                           03 Password Write Protect
	                           05 Read/Write Password Protect
	                         >=08 Temporary Unlock 
				
				PBUFFER password    - password character buffer
				BYTE passLen        - Length of password buffer (limit=32)	
	
	Returns: BYTE - status  (See SendAtapiPacket)
	
	Calls:
		SendAtapiPacket  (API)		
		memset           (edk_tool)

*/
BYTE CartridgeProtect(BYTE flag, PBUFFER password, BYTE passLen);


/******************************************************************************
GetMediaID - This command will get the unique serial number for the media.

	Parameters: PBUFFER mediaID  - This pointer to buffer where serial number
	                               will return. Needs to be 64 bytes but only
	                               first 40 bytes returned are the media ID. 
	                               The upper 24 bytes used as scratch memory.
	                               
	
	Returns: BYTE - status  (See SendAtapiPacket)
	
	Calls:
		SendAtapiPacket  (API)		
		ProcessInterrupt (API)
		memset           (edk_tool)
		memmove          (edk_tool)
*/                                                        
BYTE GetMediaID(PBUFFER mediaID);

/******************************************************************************
FormatDrive - Does a DOS FAT16 format that Iomega officially supports.  The 
developer does not have to use this format command to format Iomega media.  Any
FAT16 format that supports the size of the media will work.

	Parameters: none
	
	Returns: BYTE - status  (See SendAtapiPacket)

	Calls:
		SendAtapiPacket   (API)
		ProcessInterrupt  (API)
		WriteAtapi        (API)
		memset            (edk_tool)
		memmove           (edk_tool)
*/
BYTE FormatDrive(PBUFFER buffer);

#define SWAP_32(x) ((((x) & 0xff000000) >> 24) | \
                    (((x) & 0x00ff0000) >> 8) | \
                    (((x) & 0x0000ff00) << 8) | \
                    (((x) & 0x000000ff) << 24))

typedef struct non_sense_header_s
{
    cyg_uint8 page_code;
    cyg_uint8 page_length;
}__attribute__((packed)) non_sense_header_t;

typedef struct cartridge_status_s
{
    cyg_uint8 reserved0;
    cyg_uint8 status;
    cyg_uint8 reserved1;
    cyg_uint32 last_lba;	/* big endian */
    cyg_uint32 bytes_per_blk;	/* big endian */
    cyg_uint16 blks_avail_reassign_0; /* big endian */
    cyg_uint16 blks_reassign_0;	/* big endian */
    cyg_uint16 blks_avail_reassign_1; /* big endian */
    cyg_uint16 blks_reassign_1;	/* big endian */
    cyg_uint8 protect_mode :4;
    cyg_uint8 reserved2    :4;
    char serial_num[40];
    cyg_uint8 field_format_flag;
    cyg_uint8 reserved3;
}__attribute__((packed)) cartridge_status_t;

cyg_uint8 get_geometry_atapi(drive_geometry_t * dg);
              
#endif /* _EDK_H_ */

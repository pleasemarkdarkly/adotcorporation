/* THIS SOFTWARE IS PROVIDED "AS IS" WITH NO WARRANTIES EXPRESSED OR IMPLIED,
   INCLUDING WITHOUT LIMITATION WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
   PARTICULAR PURPOSE OR NON-INFRINGEMENT. LICENSEE BEARS ALL RISK OF 
   NON-PERFORMANCE, LOSS OF DATA, AND OTHER PROBLEMS AND LICENSOR WILL NOT BE
   LIABLE UNDER ANY CONTRACT, NEGLIGENCE, STRICT LIABILITY OR OTHER THEORY
   FOR ANY DAMAGES INCLUDING, WITHOUT LIMITATION, DIRECT, INCIDENTAL OR 
   CONSEQUENTIAL DAMAGES OR COST OF PROCUREMENT OF SUBSTITUTE GOODS, SERVICES
   OR TECHNOLOGY */

#include <cyg/kernel/kapi.h>
#include <cyg/hal/hal_edb7xxx.h>
#include <cyg/infra/diag.h>

#include <dadio/io/edk.h>
#include <dadio/io/edk_hw.h>
#include <dadio/io/sectors.h>

/* Local Procedures */
BYTE PollStatus(BYTE clear_bits, BYTE set_bits);                       
BYTE DeviceSelection(void); 
BYTE WaitForInterrupt(void);

/* Global Variables */
BYTE ScratchBuffer[SCRATCH_BUFFER_SIZE];   
BYTE interrupt_reason;
volatile BYTE status;
BYTE sleep_flag;
PBUFFER data_buffer;
extern cyg_ucount32 lba_last_error_index;

#if USE_IRQ
BYTE Interrupt_Occurred;
extern cyg_sem_t clik_sem;
#endif

cyg_uint8
get_geometry_atapi(drive_geometry_t * dg)
{
    cyg_uint8 buf[sizeof(non_sense_header_t) + sizeof(cartridge_status_t)];
    cartridge_status_t * status_page;
    cyg_uint8 ret_val = NO_ERROR;
    unsigned int i;
    
    /* get disk capacity */
    memset(ScratchBuffer, 0, SCRATCH_BUFFER_SIZE);
    ScratchBuffer[0] = ATAPI_READ_CAP;
    ret_val = SendAtapiPacket(ScratchBuffer, (PBUFFER)&buf);
    if (ret_val) {
	return ret_val;
    }
    dg->num_blks = SWAP_32(*(cyg_uint32 *)(&buf[0]));
    dg->bytes_p_sec = SWAP_32(*(cyg_uint32 *)(&buf[4]));
    /* TODO figure out how to get cyl, hd, sec */
    dg->cyl = 0;
    dg->hd = 0;
    dg->sec = 0;
    
    /* get disk serial number, model number */    
    memset(ScratchBuffer, 0, SCRATCH_BUFFER_SIZE);
    ScratchBuffer[0] = ATAPI_NON_SENSE;
    ScratchBuffer[2] = 2;
    ScratchBuffer[4] = 64;

    ret_val = SendAtapiPacket(ScratchBuffer, (PBUFFER)&buf);
    if (ret_val) {
	return ret_val;
    }
    status_page = (cartridge_status_t *)&buf[sizeof(non_sense_header_t)];
    for (i = 0; i < 40; ++i) {
	dg->serial_num[i] = status_page->serial_num[i];
    }
    dg->model_num[0] = 0;	/* TODO get model num from identify info */
    
    return ret_val;
}


/******************************************************************************
  SendATAPIPacket                                                            
  
  Routine follows the flowchart seen in the ATA/ATAPI4 spec Rev 17 Section 9.11 
******************************************************************************/
BYTE SendAtapiPacket(PBUFFER cdb, PBUFFER data)
{       
    BYTE ret_val;
                    
    /* pointer to where data should go saved in global variable to be
       used by process interrupt */

    //diag_printf("SendAtapiPacket\n");
    
    if (sleep_flag) {
	diag_printf("SendAtapiPacket sleep_flag %x\n", ERROR_SLEEP);
	return(ERROR_SLEEP);
    }

    data_buffer = data;                    
                    
    if( (ret_val=DeviceSelection()) )
    {
	diag_printf("SendAtapiPacket DeviceSelection %x\n", ret_val);
	return(ret_val);
    }

    /* Initialize Features Register */
    WriteFeatures(FEATURE_NODMA);         
	
    /* Set the Byte Count which tells the drive the number of bytes per interrupt */
    WriteByteCountHigh(HOST_BUFFER_HI);
    WriteByteCountLow(HOST_BUFFER_LO);
	
    /* Send ATAPI Command */
    WriteCommand(COMMAND_ATAPI_PACKET);
    
    /* Wait for drive ready to accept packet 
       Some devices will cause an interrupt but some will not 
       Currently all zip drives 1/1/2000 cause an interrupt but 
       future zip drive may not so we will poll here. 
       A way to find out if the device does interrupts after the
       Atapi packet command is in the ID table word 0 bits 5-6 */ 
	   
    if (PollStatus(STATUS_BSY, STATUS_DRQ))
    {
	diag_printf("SendAtapiPacket PollStatus %x\n", ERROR_TIMEOUT);
	return(ERROR_TIMEOUT);
    }

#if USE_IRQ
    /*clear interrupt semaphore just in case it did happen */
    //Interrupt_Occurred = (Interrupt_Occurred > 0 ? Interrupt_Occurred-- : 0 );
    //	Interrupt_Occurred = 0;
    cyg_semaphore_trywait(&clik_sem);/* TODO diagnostic output */
#endif	
	
    /* Output the packet */
    WriteDataBulk(cdb,6);

    ret_val = ProcessInterrupt();
    //	UnmaskInterrupt();
    return ret_val;
	
    //	return(ProcessInterrupt());
}                

/******************************************************************************
  WriteAtapi                                                            
******************************************************************************/
BYTE WriteAtapi(DWORD lba, WORD length, PBUFFER data)
{
    memset(ScratchBuffer, 0, SCRATCH_BUFFER_SIZE);
	
    ScratchBuffer[0] = ATAPI_WRITE10;	/* ATAPI Write (10) Packet Command */
    ScratchBuffer[2] = (BYTE) (lba >> 24);
    ScratchBuffer[3] = (BYTE) (lba >> 16);
    ScratchBuffer[4] = (BYTE) (lba >> 8);
    ScratchBuffer[5] = (BYTE) lba;
    ScratchBuffer[7] = (BYTE) (length >> 8);
    ScratchBuffer[8] = (BYTE) length;
	
    return(SendAtapiPacket(ScratchBuffer, data));
}

/******************************************************************************
  ReadAtapi                                                            
******************************************************************************/
BYTE ReadAtapi(DWORD lba, WORD length, PBUFFER data)
{
	//diag_printf("%d\n",length);
    memset(ScratchBuffer, 0, SCRATCH_BUFFER_SIZE);
	
    ScratchBuffer[0] = ATAPI_READ10;	/* ATAPI Read (10) Packet Command */
    ScratchBuffer[2] = (BYTE) (lba >> 24);
    ScratchBuffer[3] = (BYTE) (lba >> 16);
    ScratchBuffer[4] = (BYTE) (lba >> 8);
    ScratchBuffer[5] = (BYTE) lba;
    ScratchBuffer[7] = (BYTE) (length >> 8);
    ScratchBuffer[8] = (BYTE) length;
	
    return(SendAtapiPacket(ScratchBuffer, data));
}

/******************************************************************************
  ResetAtapi (or wake up drive)
******************************************************************************/
BYTE ResetAtapi(void)
{
    WORD i;

#if USE_IRQ
    /* If drive was asleep then the interrupt was masked so we need to unmask
       it */
    UnmaskInterrupt();
#endif	


#if BIT_RESET
    WriteControl(CONTROL_SRST);  
    for(i=0; i<1000; i++);  		/* delay of at least 5 usec */
    WriteControl(CONTROL_NORMAL);
#endif /* BIT RESET */

#if !BIT_RESET
    /* For a reset, do not wait for busy bit to clear */
    /* Select Master or Slave */	                             
#if DRIVE_MASTER
    WriteDriveSelect(SELECT_MASTER);
#else
    WriteDriveSelect(SELECT_SLAVE);
#endif

    WriteCommand(COMMAND_DEVICE_RESET);
#endif /* !BIT_RESET */

	
    /* wait for busy bit to clear */
    if (PollStatus(STATUS_BSY,0))
    {
	return(ERROR_TIMEOUT);
    }

    sleep_flag = 0;

    return(NO_ERROR);
}

/******************************************************************************
  InitAtapi                                                            
******************************************************************************/
BYTE InitAtapi(void)
{
    BYTE ret_val;         
    TIMER timeout,ticks;

#if USE_IRQ
    InitializeInterrupt();
#endif

    timeout = GetOSTicks() + TIMEOUT_5SECONDS;
    do {
	memset(ScratchBuffer, 0, SCRATCH_BUFFER_SIZE);
	ret_val = SendAtapiPacket(ScratchBuffer, ScratchBuffer);
	ticks = GetOSTicks();
#if MULTI_PROCESS
	while ((ret_val == ERROR_WAIT_FOR_DEVICE) && (ticks < timeout))
	{		
	    ret_val = ProcessInterrupt();
	    UnmaskInterrupt();
	    ticks = GetOSTicks();
	}
#endif	   
    }
    while ((ret_val != NO_ERROR) &&
	   (ret_val != ERROR_NO_MEDIA) &&
	   (ret_val != ERROR_MEDIA_CHANGED) &&
	   (ret_val != ERROR_SLEEP) &&
	   (ticks < timeout));

    return(ret_val); 
} 

/******************************************************************************
  DriveStatus                                                            
******************************************************************************/
BYTE DriveStatus(void)
{
    memset(ScratchBuffer, 0, SCRATCH_BUFFER_SIZE);
    return(SendAtapiPacket(ScratchBuffer, ScratchBuffer));
}

/******************************************************************************
  IdAtapi                                                            
******************************************************************************/
BYTE IdAtapi(PBUFFER data)
{
    BYTE ret_val;

    if (sleep_flag)
	return(ERROR_SLEEP);

    if( (ret_val=DeviceSelection()) )
    {
	return(ret_val);
    }

    WriteCommand(COMMAND_ATAPI_ID);    
	

#if USE_IRQ
    if (WaitForInterrupt())
    {
	return(ERROR_TIMEOUT);
    }
    UnmaskInterrupt();
#endif

    if (PollStatus(STATUS_BSY, STATUS_DRQ))
    {
	return(ERROR_TIMEOUT);
    }
	
    ReadDataBulk(data, 256);
	
    if (PollStatus(STATUS_BSY | STATUS_DRQ,0))
    {
	return(ERROR_TIMEOUT);
    }
    return(NO_ERROR);
}

/******************************************************************************
  SleepDrive
******************************************************************************/
BYTE SleepDrive(void)
{
    BYTE ret_val;

    if( (ret_val=DeviceSelection()) )
    {
	return(ret_val);
    }

    WriteCommand(COMMAND_SLEEP);
	
#if USE_IRQ
	
    if (WaitForInterrupt())
    {
	return(ERROR_TIMEOUT);
    }
	
    /* Mask the interrupt because when the drive is asleep then it no longer
       drives the Interrupt line and could cause spurious interrupts */
    /*dc: redundant */
    MaskInterrupt();
#endif

#if !USE_IRQ
    if (PollStatus(STATUS_BSY,0))
    {
	return(ERROR_TIMEOUT);
    }
#endif
	
    sleep_flag = 1;      
		          
    return(NO_ERROR);
}

/******************************************************************************
  ProcessInterrupt
******************************************************************************/
/* ProcessInterrupt
    
   Waits for interrupt (either by polling or by waiting for drive interrupt)
   Read the interrupt reason register
   Takes the appropriate action
   
   Return Values:
   		00 Finished with command successful.
   		01 Data read phase.
   		02 Data write phase
   		03 Invalid phase
   		F0-FF Error 
   			FD Invalid Phase.
   			FE Interrupt not occurred yet.
   			FF Timeout Error.
*/          
BYTE ProcessInterrupt(void)
{
    BYTE pi_status;
    BYTE pi_interrupt_reason;
    BYTE phase_error = 0;
    
    /* infinate loop until return statement is executed */
    while (1)
    {
	/* Waiting for Interrupt to happen (four methods depending on compiler switches) */
	/* Method 1: Interrupt with multi processing */
#if (USE_IRQ && MULTI_PROCESS)
	if (Interrupt_Occurred)
	    /* Interrupt has occured so clear the interrupt and continue on */
	    Interrupt_Occurred = 0;
	else {
	    /* No interrupt so return stating that interrupt has not occured  
	       host needs to call ProcessInterrupt again */
	    diag_printf("ProcessInterrupt Method 1 %x\n", ERROR_WAIT_FOR_DEVICE);
	    return(ERROR_WAIT_FOR_DEVICE);
	}
#endif

	/* Method 2: Interrupt without multi processing */
#if (USE_IRQ && !MULTI_PROCESS)
 	if(WaitForInterrupt()) {
	    diag_printf("ProcessInterrupt Method 2 %x\n", ERROR_TIMEOUT);
	    diag_printf("%x %x\n", status, interrupt_reason);
	    return(ERROR_TIMEOUT);
	}
#endif
	
	/* Method 3: Polling with multi processing */
#if (!USE_IRQ && MULTI_PROCESS)
	ReadStatus(status);
	if (status & STATUS_BSY) {
	    diag_printf("ProcessInterrupt Method 3 %x\n", ERROR_WAIT_FOR_DEVICE);
	    return(ERROR_WAIT_FOR_DEVICE);
	}
#endif

	/* Method 4: Polling without multi processing */
#if (!USE_IRQ && !MULTI_PROCESS)
	if (PollStatus(STATUS_BSY,0)) {
	    diag_printf("ProcessInterrupt Method 4 %x\n", ERROR_TIMEOUT);
	    return(ERROR_TIMEOUT);  /* indicate failure due to time out */
	}
#endif

	/* Read interrupt reason and take appropriate action */
	ReadInterruptReason(interrupt_reason);
	//pi_interrupt_reason = interrupt_reason;
	//pi_status = status;
#if USE_IRQ
	//	UnmaskInterrupt();
#endif
	
	//	switch ((pi_status & STATUS_DRQ) | (pi_interrupt_reason & (INTERRUPT_IO | INTERRUPT_COMMAND)))
	switch ((status & STATUS_DRQ) | (interrupt_reason & (INTERRUPT_IO | INTERRUPT_COMMAND)))
	{
	
	    case STATUS_DRQ:
		/* DRQ, /COD, /IO = Data from Host Phase */
	    {
		WORD count;
		BYTE i;
			
		/* Read the Byte Count Registers to determine loop value and write the data */			
		ReadByteCountHigh(i);
		count = i << 8;
		ReadByteCountLow(i);
		count += i;				/* count contains number of bytes to transfer */
		count >>= 1;			/* convert to words */
		//diag_printf("1 %d\n",count);
		WriteDataBulk(data_buffer, count);
		data_buffer += (count << 1);
	    }                              
	    break;
		
	    case STATUS_DRQ | INTERRUPT_IO:
		/* DRQ, /COD, IO = Data to Host Phase */
	    {
		WORD count;
		BYTE i;

		/* Read the Byte Count Registers to determine loop value and read the data */
		ReadByteCountHigh(i);
		count = i << 8;
		ReadByteCountLow(i);
		count += i;				/* count contains number of bytes to transfer */
		count >>= 1;			/* convert to words */
		//diag_printf("2 %d\n",count);
		ReadDataBulk(data_buffer, count);
		data_buffer += (count << 1);
	    }
	    break;
		
	    case INTERRUPT_COMMAND | INTERRUPT_IO:
		/* /DRQ, COD, IO = Status Phase */
		/* Read Error Register and look for the Check Condition Bit */

		//if (pi_status & STATUS_CHECK)
		if (status & STATUS_CHECK)
		{
		    BYTE i;
	
		    /* Zero the CDB buffer to prepare for Request Sense */    
		    memset(ScratchBuffer, 0, SCRATCH_BUFFER_SIZE);
		    ScratchBuffer[0] = ATAPI_REQUEST_SENSE;
		    ScratchBuffer[4] = 14;	  /* Only request 14 bytes to get ASC */
		    if( (i=SendAtapiPacket(ScratchBuffer, ScratchBuffer)) )
		    {
#if MULTI_PROCESS
			while(i == ERROR_WAIT_FOR_DEVICE) {
			    i = ProcessInterrupt();
			}
				    
			if (i == NO_ERROR) { /* If Multi-Process command successful */
			    diag_printf("ProcessInterrupt NO_ERROR %x\n", ScratchBuffer[12]);
			    return(ScratchBuffer[12]);  /* Then return the ASC */
			}
#endif
			/* if SendAtapiPacket for Request Sense fails (never should)
			   then return the error */
			//diag_printf("ProcessInterrupt i %x\n", i);
			return(i);
		    }
		    else
		    {
#if 0 /* -tm */
			cyg_thread_set_data(lba_last_error_index, (ScratchBuffer[6] |
								   (ScratchBuffer[5] << 8) | (ScratchBuffer[4] << 16) |
								   (ScratchBuffer[3] << 24)));
#endif
			/* Return the ASC */
			//diag_printf("ProcessInterrupt ASC %x\n", ScratchBuffer[12]);
			return(ScratchBuffer[12]);
		    }
		}
		else {
		    return(NO_ERROR);
		}
		
	    default:
#if 1
		//diag_printf("%x %x\n", pi_status, pi_interrupt_reason);
		//*(volatile cyg_uint8 *)PBDR |= 0x10;
		++phase_error;
		if (phase_error > 5) {
		    diag_printf("ProcessInterrupt default %x\n", ERROR_OUT_OF_PHASE);
		    //diag_printf("%x %x\n", pi_status, pi_interrupt_reason);
		    diag_printf("%x %x\n", status, interrupt_reason);
		    return ERROR_OUT_OF_PHASE;
		}
		else {
#if (USE_IRQ)
		    //Interrupt_Occurred = 1;
		    cyg_semaphore_post(&clik_sem);
		    ReadStatus(status);
#endif /* USE_IRQ */
		    break;
		}
#else
		return ERROR_OUT_OF_PHASE;
#endif
	} /* Switch statement */
    } /* end of infinate while loop */
} /* end of ProcessInterrupt */



/******************************************************************************
  PreventAllow                                                            
******************************************************************************/
BYTE PreventAllow(BYTE flag)
{
    memset(ScratchBuffer, 0, SCRATCH_BUFFER_SIZE);

	
    ScratchBuffer[0] = ATAPI_PREVENT_ALLOW;	
    ScratchBuffer[4] = flag;
    return(SendAtapiPacket(ScratchBuffer, ScratchBuffer));
}


/******************************************************************************
  EjectMedia                                                            
******************************************************************************/
BYTE EjectMedia(void)
{
    memset(ScratchBuffer, 0, SCRATCH_BUFFER_SIZE);
	
    ScratchBuffer[0] = ATAPI_START_STOP;
    ScratchBuffer[1] = 0x01;
    ScratchBuffer[4] = 0x02;
    return(SendAtapiPacket(ScratchBuffer, ScratchBuffer));
}

/******************************************************************************
  CartridgeProtect                                                            
******************************************************************************/
BYTE CartridgeProtect(BYTE flag, PBUFFER password, BYTE passLen)
{
    memset(ScratchBuffer, 0, SCRATCH_BUFFER_SIZE);
	
    ScratchBuffer[0] = ATAPI_PROTECT;
    ScratchBuffer[1] = 0x10 + flag;
    ScratchBuffer[4] = passLen;
    return(SendAtapiPacket(ScratchBuffer, password));
}
                       
                       
/******************************************************************************
  GetMediaID                                                            
******************************************************************************/
BYTE GetMediaID(PBUFFER mediaID)
{
    BYTE ret_val;
	
    memset(ScratchBuffer, 0, SCRATCH_BUFFER_SIZE);
	
    ScratchBuffer[0] = ATAPI_NON_SENSE;
    ScratchBuffer[2] = 2;
    ScratchBuffer[4] = 64;
	
    if( (ret_val = SendAtapiPacket(ScratchBuffer, mediaID)) )
    {
#if MULTI_PROCESS
	while(ret_val == ERROR_WAIT_FOR_DEVICE)
	    ret_val = ProcessInterrupt();
	if (ret_val)
#endif		
	    return(ret_val);
    }
		
    memmove(mediaID, mediaID + 22, 40);
    return(NO_ERROR);
}

/******************************************************************************
  FormatDrive                                                            
******************************************************************************/
#ifdef _ECOS_
#define PACKED __attribute__((packed))
#else
#pragma pack(1)
#define PACKED
#endif
typedef PACKED struct _BPB
{
    WORD  bytesPerSector;              /* Bytes per Sector             */
    BYTE  sectorsPerCluster;           /* Sectors per Cluster          */
    WORD  reservedSectors;             /* Number of Reserved Sectors   */
    BYTE  numberOfFATs;                /* Number of FATs               */
    WORD  rootDirEntries;              /* Number of Root Dir Entries   */
    WORD  sectors;                     /* = 0, see hugeSectors         */
    BYTE  mediaType;                   /* Media Type (F8 = Hard Disk)  */
    WORD  sectorsPerFAT;               /* Number of Sectors per FAT    */
    WORD  sectorsPerTrack;             /* Number of Sectors per Track  */
    WORD  heads;                       /* Number of Heads              */
    DWORD hiddenSectors;               /* Number of Hidden Sectors     */
    DWORD hugeSectors;                 /* Number of sectors - real     */
    BYTE  driveNumber;                 /* Drive number (0x80 or 0x81)  */
    BYTE  reserved;                    /* Reserved by DOS              */
    BYTE  extendedBootSig;             /* Extended Boot sig: 0x29      */
    DWORD volumeIDNumber;              /* Volume ID number             */
    BYTE  volumeLabel[11];             /* Volume label                 */
    BYTE  fileSystemType[8];           /* File system type ("FAT16   ")*/
} BPB, *PBPB;


typedef  PACKED struct _PARTITION
{
    BYTE  activeDOSPart;               /* Active DOS partition (0x80)  */
    BYTE  beginHead;                   /* Beginning head (0x01)        */
    BYTE  beginSector;                 /* Beginning sector (0x01)      */
    BYTE  beginCylinder;               /* Beginning cylinder (0x00)    */
    BYTE  partitionType;               /* Partition Type:FAT16 (0x06)  */
    BYTE  endHead;                     /* Ending Head (0x3f)           */
    BYTE  endSector;                   /* Ending Sector (0x20)         */
    BYTE  endCylinder;                 /* Ending Cylinder (0x5f)       */
    DWORD startSector;                 /* Starting Sector (0x00000020) */
    DWORD numberOfSectors;             /* Number of sectors(0x0002FFE0)*/
} PARTITION, *PPARTITION;

#undef PACKED
#ifndef _ECOS_
#pragma pack()
#endif

#define	SIZE_32_MEG         33554432    /* 32 Megabytes                 */
#define ACTIVE              0x80        /* Define an active partition   */
#define DIR_ENTRY_SIZE      32          /* Number of bytes in dir entry */

/* Neil Shaw 4-28-99 New defines  for finding  correct Sectors / Cluster*/
/* (65520 data clusters * (sec/clus)) + (256 FAT sectors * # of FATs) + */
/*  Root DIR sectors + hidden sectors + reserved sectors                */
#define		FAT16_2K_CLUSTERS_MAX_LBAs		(65520*4)+(256*2)+32+32+1
#define		FAT16_4K_CLUSTERS_MAX_LBAs		(65520*8)+(256*2)+32+32+1
#define		FAT16_8K_CLUSTERS_MAX_LBAs 		(65520*16)+(256*2)+32+32+1
#define		FAT16_16K_CLUSTERS_MAX_LBAs		(65520*32)+(256*2)+32+32+1
#define		FAT16_32K_CLUSTERS_MAX_LBAs		(65520*64)+(256*2)+32+63+1
       
BYTE FormatDrive(PBUFFER buffer)
{
    BPB bpb;
    PPARTITION pPart;
    BYTE  *charPtr;
    DWORD sectorsPerDisk;
    DWORD bytesPerSector;
    DWORD sectorsPerCylinder;
    DWORD userSectorsPerDisk; 
    DWORD bytesPerDisk;
    DWORD sectorsPerRoot;
    DWORD bytesPerFAT;
    DWORD maxHeads;
    DWORD maxSectors;
    DWORD maxCylinders;
    DWORD numberOfSectors;
    DWORD sectNum;


    BYTE  ret_val;
	
	
    memset(ScratchBuffer, 0, SCRATCH_BUFFER_SIZE);
   	
    /* Test Unit Ready to make sure disk is present */
    if( (ret_val = SendAtapiPacket(ScratchBuffer, ScratchBuffer)) )
#if MULTI_PROCESS
	while(ret_val == ERROR_WAIT_FOR_DEVICE)
	    ret_val = ProcessInterrupt();
    if (ret_val)
#endif		
	return(ret_val);
		
    memset(ScratchBuffer, 0, SCRATCH_BUFFER_SIZE);
    /* Get the disk capacity */
    ScratchBuffer[0] = ATAPI_READ_CAP;
    if( (ret_val = SendAtapiPacket(ScratchBuffer, ScratchBuffer)) )
#if MULTI_PROCESS
	while(ret_val == ERROR_WAIT_FOR_DEVICE)
	    ret_val = ProcessInterrupt();
    if (ret_val)
#endif		
	return(ret_val);
		
    /* Point to the buffer */
    charPtr = ScratchBuffer;
	
    /* Get the total number of sectore */
	
    /* Get the total number of sectors */
    sectorsPerDisk = ((DWORD)*charPtr++) << 24;
    sectorsPerDisk |= ((DWORD)*charPtr++) << 16;
    sectorsPerDisk |= ((DWORD)*charPtr++) << 8;
    sectorsPerDisk |= ((DWORD)*charPtr++);
    sectorsPerDisk++;
	
    /* Get the sector size */
    bytesPerSector = ((DWORD)*charPtr++) << 24;
    bytesPerSector |= ((DWORD)*charPtr++) << 16;
    bytesPerSector |= ((DWORD)*charPtr++) << 8;
    bytesPerSector |= ((DWORD)*charPtr++);

    /* Max clusters (65520) * Max sectors per cluster (64)*/
    if (sectorsPerDisk > 0x3FFC00)  
    {   /*  Max FAT 16 disk size is 2GB. */
        sectorsPerDisk = 0x3FFC00;	
    }
    /* Clear out the BPB */
    memset((void *)&bpb, 0x00, sizeof(BPB));
		
    /* set up the parameters default to 64 - 32 Geometry */
    bpb.heads = 64;
    bpb.sectorsPerTrack = 32;
		
    // if  greater than 1024/64/32 geometry then go to XXX/255/63
    // Neil Shaw  11-13-98    Max # of Cylinders is 1024.
    if ((sectorsPerDisk / 
         (bpb.heads * bpb.sectorsPerTrack)) > 1024)
    {
        bpb.heads = 255;
        bpb.sectorsPerTrack = 63;
    }
		                                     
    /* Set the number of root directory entries */
    bpb.rootDirEntries = 512;
    
    /* Set the number of reserved sectors */
    bpb.reservedSectors = 1;

    /* Set the number of hidden sectors */
    bpb.hiddenSectors = bpb.sectorsPerTrack;

    /* Set the number of FATs */
    bpb.numberOfFATs = 2;
    
    /* Set the media type */
    bpb.mediaType = 0xf8;
    
    /* Set the number of bytes per sector */
    bpb.bytesPerSector = (WORD)bytesPerSector;
	
    /* Set the sectors per cylinder */
    sectorsPerCylinder = bpb.heads * bpb.sectorsPerTrack;

    /* Get the usable disk sectors */
    userSectorsPerDisk = ((sectorsPerDisk / sectorsPerCylinder) * 
                          sectorsPerCylinder);
    
    /* Get the number of bytes per disk */
    bytesPerDisk = userSectorsPerDisk * bytesPerSector;

    /* Do we have a disk less than 32 MB? */
    if (bytesPerDisk < SIZE_32_MEG)
    {   /* YES.  Use .sectors. */
        bpb.sectors = (WORD)(userSectorsPerDisk - bpb.hiddenSectors);
        bpb.hugeSectors = 0;
    }
    else
    {   /* NO.  Use .hugeSectors */
        bpb.hugeSectors = (userSectorsPerDisk - bpb.hiddenSectors);
        bpb.sectors = 0;
    }

    /* Get the number of sectors in the root directory */
    sectorsPerRoot = (bpb.rootDirEntries * 32) / bytesPerSector;
    
    /* Do the number of entries fall on a sector boundary? */
    if( (bpb.rootDirEntries * 32) % bytesPerSector)
    {   /* NO. Add a sector */
        sectorsPerRoot++;
    }

    /* Determine the number of sectors per cluster */
    if( userSectorsPerDisk < FAT16_2K_CLUSTERS_MAX_LBAs)
        bpb.sectorsPerCluster = 4;
    else if( userSectorsPerDisk < FAT16_4K_CLUSTERS_MAX_LBAs) 	
        bpb.sectorsPerCluster = 8;
    else if( userSectorsPerDisk < FAT16_8K_CLUSTERS_MAX_LBAs) 	
        bpb.sectorsPerCluster = 16;
    else if( userSectorsPerDisk < FAT16_16K_CLUSTERS_MAX_LBAs) 	
        bpb.sectorsPerCluster = 32;
    else if( userSectorsPerDisk < FAT16_32K_CLUSTERS_MAX_LBAs) 	
        bpb.sectorsPerCluster = 64;
    
    /* Get the number of bytes in the FAT */
    bytesPerFAT = (((userSectorsPerDisk - 
                     bpb.hiddenSectors - 
                     bpb.reservedSectors - 
                     sectorsPerRoot) / 
                    bpb.sectorsPerCluster) * 2);
    
    /* Get the number of sectors in the FAT */
    bpb.sectorsPerFAT = (WORD)(bytesPerFAT / bytesPerSector);
    
    /* If it's not on a sector boundary, get another sector */
    if( bytesPerFAT % bytesPerSector ) bpb.sectorsPerFAT++;    
    
    /* 
       Remaining items to be set:
       bpb.driveNumber     -> 0x80
       bpb.reserved        -> leave alone (zero)
       bpb.extendedBootSig -> 0x29
       bpb.volumeIDNumber  -> random
       bpb.volumeLabel     -> leave alone
       bpb.fileSystemType  -> "FAT16   "
    */

    /* Feed the current free running counter into the volumeIDNumber! */
    bpb.volumeIDNumber = GetOSTicks();

    bpb.driveNumber = 0x80;
    bpb.extendedBootSig = 0x29;
    memmove((char *)bpb.fileSystemType, "FAT16   ", 8 );
    
    /* Get the number of heads */
    maxHeads = bpb.heads;

    /* Get the number of sectors */
    maxSectors = bpb.sectorsPerTrack;

    /* Set the maximum number of cylinders */
    maxCylinders = (sectorsPerDisk / (maxHeads * maxSectors));

    /* Neil Shaw  11-13-98    Max Cylinders is 1024. */
    if (maxCylinders > 1024) 
    {   /*  EndCyl is (maxCyl -1) and has a max of 0x3FF or 1023 */
        maxCylinders = 1024;
    }
    /* Get the partition size from our new numbers */
    sectorsPerDisk = maxCylinders * maxHeads * maxSectors;

    /* Get the number of bytes */
    bytesPerDisk = sectorsPerDisk * 512;

    /* Move the partition table into the buffer */
    memmove(buffer, ipart, 512);
    
    /* Point to the partition table area */
    charPtr = buffer + 446;
    
    /* Point the partition table at that spot */
    pPart = (PPARTITION)charPtr;

    /* Point to the fourth entry */
    pPart += 3;

    /* Default to FAT16 */
    pPart->partitionType = 0x06;

    /* Set up the partition entry values */
    pPart->activeDOSPart = (BYTE)ACTIVE;

    /* Set the beginning values */
    pPart->beginHead = 1;
    pPart->beginSector = 1;
    pPart->beginCylinder = 0;

    /* Set the ending values */
    pPart->endHead = (BYTE)maxHeads - 1;
    pPart->endSector = (BYTE)maxSectors & 0x3F;
    pPart->endSector |= (BYTE) (((maxCylinders - 1) & 0x0300)>>2);
    pPart->endCylinder = (BYTE) (maxCylinders - 1) & 0x00FF;

    /* Set the start sector and the number of sectors */
    charPtr = (BYTE *)&pPart->startSector;
    *charPtr++ = (BYTE)(bpb.hiddenSectors & 0x000000ff);
    *charPtr++ = (BYTE)((bpb.hiddenSectors >> 8) & 0x000000ff);
    *charPtr++ = (BYTE)((bpb.hiddenSectors >> 16) & 0x000000ff);
    *charPtr++ = (BYTE)((bpb.hiddenSectors >> 24) & 0x000000ff);
    numberOfSectors =  sectorsPerDisk - bpb.hiddenSectors;
    *charPtr++ = (BYTE)(numberOfSectors & 0x000000ff);
    *charPtr++ = (BYTE)((numberOfSectors >> 8) & 0x000000ff);
    *charPtr++ = (BYTE)((numberOfSectors >> 16) & 0x000000ff);
    *charPtr++ = (BYTE)((numberOfSectors >> 24) & 0x000000ff);

	
    if( (ret_val = WriteAtapi(0, 1, buffer)) )
    {
#if MULTI_PROCESS
	while(ret_val == ERROR_WAIT_FOR_DEVICE)
	    ret_val = ProcessInterrupt();
	if (ret_val)
#endif		
	    return(ret_val);
    }
    
    /* Clear the working buffer */
    memset(buffer, 0xED, 512);

    /* Clear the first 'track' of the disk */
    for (sectNum = 1; sectNum < bpb.hiddenSectors; sectNum++)
    {
	if ( (ret_val = WriteAtapi(sectNum, 1, buffer)) )
	{
#if MULTI_PROCESS
	    while(ret_val == ERROR_WAIT_FOR_DEVICE)
		ret_val = ProcessInterrupt();
	    if (ret_val)
#endif		
		return(ret_val);
	}
    }
	
    /* Now prepare the BPB */
    /* Fill in the buffer with the master boot record or BPB */
    memmove(buffer,MasterBoot,512);
    
    /* Find the start of the BPB. */
    charPtr = buffer + 11;

    /* Fill in our computed values into the buffer */
    memmove(charPtr, (void*)&bpb, sizeof(BPB));

    /* Fill in the file system type */
    charPtr = buffer + 54;
    memmove(charPtr, "FAT16   ", 8);
    
    /* Put in the OS Stamp */
    charPtr = buffer + 3;
    memmove(charPtr, "MSWIN4.0", 8);

    /* Write to the disk */
    if ( (ret_val = WriteAtapi(bpb.hiddenSectors, 1, buffer)) )
    {
#if MULTI_PROCESS
	while(ret_val == ERROR_WAIT_FOR_DEVICE)
	    ret_val = ProcessInterrupt();
	if (ret_val)
#endif		
	    return(ret_val);
    }

    /* Initialize the FATs.  First sector is unique */
    /* First clear the buffer */
    memset(buffer, 0x00, 512);

    /* Write the first bytes */
    buffer[0] = bpb.mediaType;
    buffer[1] = 0xFF;
    buffer[2] = 0xFF;
    buffer[3] = 0xFF;
			
    /* Write 1st FAT to the disk */
    if ( (ret_val = WriteAtapi(bpb.hiddenSectors + bpb.reservedSectors,
			       1, buffer)) )
    {
#if MULTI_PROCESS
	while(ret_val == ERROR_WAIT_FOR_DEVICE)
	    ret_val = ProcessInterrupt();
	if (ret_val)
#endif		
	    return(ret_val);
    }
		
    /* Write 2nd FAT to the disk */
    if ( (ret_val = WriteAtapi(bpb.hiddenSectors + bpb.reservedSectors + 
			       bpb.sectorsPerFAT, 1, buffer)) )
    {
#if MULTI_PROCESS
	while(ret_val == ERROR_WAIT_FOR_DEVICE)
	    ret_val = ProcessInterrupt();
	if (ret_val)
#endif		
	    return(ret_val);
    }
	
    /* Now finish clearing/zeroing the two FATs */
    memset(buffer, 0x00, 512);
	                 
    /* For all remaining sectors in the FAT */
    for (sectNum = bpb.hiddenSectors + bpb.reservedSectors + 1;
         sectNum < (bpb.hiddenSectors + bpb.reservedSectors + 
		    bpb.sectorsPerFAT); sectNum++)
    {
	if( (ret_val = WriteAtapi(sectNum, 1, buffer)) )
	{
#if MULTI_PROCESS
	    while(ret_val == ERROR_WAIT_FOR_DEVICE)
		ret_val = ProcessInterrupt();
	    if (ret_val)
#endif		
		return(ret_val);
	}
	if (( ret_val = WriteAtapi(sectNum + bpb.sectorsPerFAT, 1,
				   buffer)) )
	{
#if MULTI_PROCESS
	    while(ret_val == ERROR_WAIT_FOR_DEVICE)
		ret_val = ProcessInterrupt();
	    if (ret_val)
#endif		
		return(ret_val);
	}
			
    }         
	
    /* Now format the root directory (zero it out) */
    for (sectNum = (bpb.hiddenSectors + bpb.reservedSectors + 
		    (bpb.sectorsPerFAT * 2)); sectNum < (bpb.hiddenSectors +
							 bpb.reservedSectors + (bpb.sectorsPerFAT * 2) + ((bpb.rootDirEntries
													   * DIR_ENTRY_SIZE) / bpb.bytesPerSector)); sectNum++)
    {
	if( (ret_val = WriteAtapi(sectNum, 1, buffer)) )
	{
#if MULTI_PROCESS
	    while(ret_val == ERROR_WAIT_FOR_DEVICE)
		ret_val = ProcessInterrupt();
	    if (ret_val)
#endif		
		return(ret_val);
	}
    }
    return(NO_ERROR);	    	
}
    
/* Device Selection
   This follows the Device Selection protocol as described in the ATA/ATAPI
   spec Rev 17 Section 9.6

   Returns: BYTE - 00  No error
                   FF  Timeout
*/

BYTE DeviceSelection(void)
{
    //diag_printf("DeviceSelection\n");
    
    if (PollStatus(STATUS_BSY | STATUS_DRQ, 0))
    {
	return(ERROR_TIMEOUT);  /* indicate failure due to time out */
    }
	
    /* Select Master or Slave */	                             
#if DRIVE_MASTER
    WriteDriveSelect(SELECT_MASTER);
#else
    WriteDriveSelect(SELECT_SLAVE);
#endif
	
    if (PollStatus(STATUS_BSY | STATUS_DRQ, 0))
    {
	return(ERROR_TIMEOUT);  /* indicate failure due to time out */
    }
    return(NO_ERROR);
}

                        
/* PollStatus
   Reads the status register and waits for the busy bit and DRQ bit to clear.
   
   Parameters:  BYTE clear_bits  - mask of all bit(s) that need to be cleared
                BYTE set_bits    - mask of all bit(s) that need to be set
                
   				(GLOBAL)
   				BYTE status      - status updated with the latest read.
   
   Returns: BYTE - 00  No error
                   FF  Timeout
   
*/

#define WAIT_1MS	0x00004800
#define WAIT_1US	0x0000000b

static void
wait(unsigned int cnt)
{
    while(cnt--)
	;
}

BYTE PollStatus(BYTE clear_bits, BYTE set_bits)
{
    TIMER timeout;
    TIMER ticks;

    ReadStatus(status);  /* do one read of status register 
			    very quickly to get out of the loop 
			    quicker if busy is already cleared */
    if ((status & clear_bits) || ((status & set_bits) != set_bits))
    {
    	timeout = GetOSTicks() + (TIMEOUT_5SECONDS);
	do
	{
	    //diag_printf(".");
	    ReadStatus(status);
	    ticks = GetOSTicks();
	}
	while(((status & clear_bits) || ((status & set_bits) != (set_bits))) &&
	      (ticks < timeout));
			
	if (ticks >= timeout) {
	    //diag_printf("\n");
	    diag_printf("PollStatus exit ERROR_TIMEOUT\n");
	    return(ERROR_TIMEOUT);
	}
    }
    //diag_printf("\n");
    return(NO_ERROR);	
}

#if USE_IRQ
#ifdef _ECOS_
/* in ecos we can block this thread until the interrupt comes in */
BYTE WaitForInterrupt(void)
{
    BYTE ret_val;
    ret_val = (cyg_semaphore_timed_wait(&clik_sem, cyg_current_time() + 500) ?
	       NO_ERROR : ERROR_TIMEOUT);
    //ReadStatus(status);
    //    UnmaskInterrupt();
    return ret_val;
}

#else
BYTE WaitForInterrupt(void)
{
    TIMER timeout, ticks;
	
    if (!Interrupt_Occurred)
    {
	timeout = GetOSTicks() + (TIMEOUT_5SECONDS);
	do
	{
	    ticks = GetOSTicks();
	}
	while((!Interrupt_Occurred) && (ticks < timeout));
		
	if (ticks >= timeout)
	    return(ERROR_TIMEOUT);
    }
    return(NO_ERROR);
}
#endif
#endif
  

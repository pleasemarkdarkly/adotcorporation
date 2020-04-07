#include <cyg/hal/hal_edb7xxx.h>
#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#include <cyg/infra/diag.h>
#include <dadio/io/blk_dev.h>
#include <dadio/io/ata.h>
#include <dadio/io/atapi.h>
#include <dadio/io/pm.h>
#include "_ata.h"
#include "_partition.h"

//#define DEBUG(s...) diag_printf(##s)
#define DEBUG(s...) /**/
//#define DEBUG_FUNCFAIL(s...) diag_printf(##s)
#define DEBUG_FUNCFAIL(s...) /**/
//#define DEBUG_FUN_ENTRY() diag_printf("+%s %d\n", __FUNCTION__, __LINE__)
#define DEBUG_FUN_ENTRY() /**/
//#define DEBUG_FUN_EXIT() diag_printf("-%s %d\n", __FUNCTION__, __LINE__)
#define DEBUG_FUN_EXIT() /**/

#if defined(ENABLE_SLAVE_DEVICE)
static ATAPrivateDeviceInfo_T _PrivateDeviceInfo[2];
#else /* ENABLE_SLAVE_DEVICE */
static ATAPrivateDeviceInfo_T _PrivateDeviceInfo[1];
#endif /* ENABLE_SLAVE_DEVICE */

static ATAPrivatePartitionInfo_T _PrivatePartitionInfo[MAX_PARTITIONS] = {0};

#if defined(USE_IRQ)
static cyg_interrupt _ATAInterrupt;
static cyg_handle_t _ATAInterruptHandle;
cyg_sem_t ATASem;
#endif /* USE_IRQ */

static cyg_sem_t ata_block_sem;

static bool _ATAInit(struct cyg_devtab_entry * Entry);
static Cyg_ErrNo _ATALookup(struct cyg_devtab_entry ** Entry,
			    struct cyg_devtab_entry * SubEntry,
			    const char * Name);
static Cyg_ErrNo _ata_write( cyg_io_handle_t h, const void* buf, cyg_uint32* len );
static Cyg_ErrNo _ata_read( cyg_io_handle_t h, void* buf, cyg_uint32* len );
static Cyg_ErrNo _ATAWrite(cyg_io_handle_t Handle, const void * Buffer, cyg_uint32 * Length, cyg_uint32 Block);
static Cyg_ErrNo _ATARead(cyg_io_handle_t Handle, void * Buffer, cyg_uint32 * Length, cyg_uint32 Block);
static cyg_bool _ATASelect(cyg_io_handle_t Handle, cyg_uint32 Which, CYG_ADDRWORD Info);
static Cyg_ErrNo _ATAGetConfig(cyg_io_handle_t Handle, cyg_uint32 Key, void * Buffer, cyg_uint32 * Length);
static Cyg_ErrNo _ATASetConfig(cyg_io_handle_t Handle, cyg_uint32 Key, const void * Buffer, cyg_uint32 * Length);

static Cyg_ErrNo dev_res_to_errno(int dev_res);

#if defined(USE_IRQ)
static cyg_uint32 _ATAInterruptISR(cyg_vector_t Vector, cyg_addrword_t Data);
static void _ATAInterruptDSR(cyg_vector_t Vector, cyg_ucount32 Count, cyg_addrword_t Data);
#endif /* USE_IRQ */
static int _IdentifyDevice(cyg_io_handle_t Handle);
static void _ProcessIdentifyData(ATAPrivateDeviceInfo_T * DeviceInfo);
static void _UnpackIdentifyDataString(char * Destination, unsigned short * Source, unsigned int Length);
static int _GetPartitionInfo(cyg_io_handle_t Handle);
static int _ProcessATAStatus(int ATAStatus);
static Cyg_ErrNo _Errno(int ATAStatus);

DEVIO_TABLE(ATADevio,
		  _ata_write,
		  _ata_read,
		  _ATAGetConfig,
		  _ATASetConfig
    );

/* Need the trailing slash on the device name so that /dev/hda/0, etc. works */
DEVTAB_ENTRY(ATADevtabMaster,
	     "/dev/hda",
	     0,
	     &ATADevio,
	     _ATAInit,
	     _ATALookup,
	     &_PrivateDeviceInfo[0]
    );

#if defined(ENABLE_SLAVE_DEVICE)
DEVTAB_ENTRY(ATADevtabSlave,
	     "/dev/hdb/",
	     0,
	     &ATADevio,
	     _ATAInit,
	     _ATALookup,
	     &_PrivateDeviceInfo[1]
    );
#endif

static Cyg_ErrNo _ata_write( cyg_io_handle_t h, const void* buf, cyg_uint32* len ) 
{
  Cyg_ErrNo ret;
  blk_request_t* b = (blk_request_t*) buf;

  cyg_semaphore_wait( &ata_block_sem );
  ret = _ATAWrite( h, b->buf, &b->num_blks, b->lba );
  cyg_semaphore_post( &ata_block_sem );
  
  return ret;
}
static Cyg_ErrNo _ata_read( cyg_io_handle_t h, void* buf, cyg_uint32* len ) 
{
  Cyg_ErrNo ret;
  blk_request_t* b = (blk_request_t*) buf;

  cyg_semaphore_wait( &ata_block_sem );
  ret = _ATARead( h, b->buf, &b->num_blks, b->lba );
  cyg_semaphore_post( &ata_block_sem );

  return ret;
}


static bool
_ATAInit(struct cyg_devtab_entry * Entry)
{
    static bool Initialized = false;

    DEBUG_FUN_ENTRY();

    /* Do one initialization for both devices since bus is shared */
    if (!Initialized) {
      int i;

	/* Initialize bus */
	*(volatile cyg_uint32 *)MEMCFG2 &= ~ATA_MEMCFG_MASK;
	*(volatile cyg_uint32 *)MEMCFG2 |= ATA_16BIT_MEMCFG;

	/* TODO Power management */

	/* Power on interface */
	// power for mystic/clik interface
	*(volatile cyg_uint8 *) PDDDR &= ~0x04;
	*(volatile cyg_uint8 *) PDDR &= ~0x04;

	// mystic enable (turned off, negative pin)
	*(volatile cyg_uint8 *) PBDDR |= 0x02;
	*(volatile cyg_uint8 *) PBDR |= 0x02;

	// wall power supply detection
	// *(volatile cyg_uint8 *)PBDDR |= 0x20;
	// *(volatile cyg_uint8 *)PBDR &= ~0x20;

	/* Enable interface */
	  
	// pop guard FET control
	// *(volatile cyg_uint8 *)PBDDR |= 0x40;
	// *(volatile cyg_uint8 *)PBDR &= ~0x40;

	/* Configure card detect GPIO */
	// *(volatile cyg_uint8 *)PBDDR &= ~0x04;/* nCD1 */
	
	/* TODO Configure card detect IRQ */
	/* nEINT1 : nCD2 */

	// issue a hard reset to the drive

	ATAHardReset();
	
	/* Initialize internal state */
	_PrivateDeviceInfo[0].Device.Position = SELECT_MASTER;
	_PrivateDeviceInfo[0].Partition = NULL;

#if defined(ENABLE_SLAVE_DEVICE)
	_PrivateDeviceInfo[1].Device.Position = SELECT_SLAVE;
	_PrivateDeviceInfo[1].Partition = NULL;
#endif /* ENABLE_SLAVE_DEVICE */

	cyg_semaphore_init( &ata_block_sem, 1 );
#if defined(USE_IRQ)
	/* Set up interrupt handler */
	cyg_semaphore_init(&ATASem, 0);
	cyg_drv_interrupt_create(ATA_IRQ,
				 10,
				 0,
				 (cyg_ISR_t *)_ATAInterruptISR,
				 (cyg_DSR_t *)_ATAInterruptDSR,
				 &_ATAInterruptHandle,
				 &_ATAInterrupt);
	cyg_drv_interrupt_attach(_ATAInterruptHandle);
	cyg_drv_interrupt_acknowledge(ATA_IRQ);
#endif /* USE_IRQ */
	
	Initialized = true;
    }
    
    DEBUG_FUN_EXIT();
    return true;
}

static int
_IdentifyDevice(cyg_io_handle_t Handle)
{
    int ATAStatus;
    unsigned char Status;
    unsigned char Error;
    ATAPrivateDeviceInfo_T * DeviceInfo = (ATAPrivateDeviceInfo_T *)((cyg_devtab_entry_t *)Handle)->priv;

    DEBUG_FUN_ENTRY();
    
    ATAStatus = ATAIdentifyDevice(&DeviceInfo->Device, (unsigned char *)DeviceInfo->Info.Raw);
    if (ATAStatus != NO_ERROR) {
	_ReadStatus(Status);
	_ReadError(Error);
	if ((Status & STATUS_ERR) && (Error & ERROR_ABRT)) {

	    /* PACKET device */
	    if (CheckSignature() != PACKET_DEVICE) {
		DEBUG("%s %d Device failed IDENTIFY DEVICE but does not report PACKET signature\n",
		      __FUNCTION__, __LINE__);
	    }
	    
	    ATAStatus = ATAIdentifyPacketDevice(&DeviceInfo->Device, (unsigned char *)DeviceInfo->Info.Raw);
	    if (ATAStatus != NO_ERROR) {
		_ReadStatus(Status);
		_ReadError(Error);
		if ((Status & STATUS_ERR) && (Error & ERROR_ABRT)) {
		    /* TODO Handle ERROR_WAIT_FOR_DEVICE, ERROR_TIMEOUT */
		    DEBUG("%s %d IDENTIFY PACKET DEVICE not supported\n", __FUNCTION__, __LINE__);
		    return !NO_ERROR;
		}
	    }
	    DeviceInfo->Device.Interface = ATAPI;
	}
	/* TODO Handle ERROR_WAIT_FOR_DEVICE, ERROR_TIMEOUT */
    }
    else {
	DeviceInfo->Device.Interface = ATA;
    }

    _ProcessIdentifyData(DeviceInfo);

    DEBUG_FUN_EXIT();
    return NO_ERROR;
}

/* TODO Flesh out ATAInfo and ATAPIInfo structures so that the Raw member is
 * not accessed in this function */
static void
_ProcessIdentifyData(ATAPrivateDeviceInfo_T * DeviceInfo)
{
    if (DeviceInfo->Device.Interface == ATAPI) {

	/* General configuration */
	DeviceInfo->Device.PacketSize = (DeviceInfo->Info.Raw[0] & 0x0003) == 0x0000 ? 12 : 16;

	/* If device is CD-ROM, set 2kB per sector */
	if (((DeviceInfo->Info.Raw[0] & 0x1f00) >> 8) == 0x05) {
	    DeviceInfo->Device.BytesPerSector = 2048;
	}
	/* Else default to 512B per sector */
	else {
	    DeviceInfo->Device.BytesPerSector = 512;
	}
	
	DEBUG("Command packet set is %x\n", (DeviceInfo->Info.Raw[0] & 0x1f00) >> 8);
	DEBUG("Device %s a removable media device\n", (DeviceInfo->Info.Raw[0] & 0x0080) ? "is" : "is not");
	DEBUG("Device shall set DRQ within %s of receiving PACKET command\n",
	      (DeviceInfo->Info.Raw[0] & 0x0060) == 0x0000 ? "3ms" : "50us");
	DEBUG("Command packet size is %d bytes\n", DeviceInfo->Device.PacketSize);
    }
    else {

	/* Geometry */
	DeviceInfo->Cylinders = DeviceInfo->Info.Raw[1];
	DeviceInfo->Heads = DeviceInfo->Info.Raw[3];
	DeviceInfo->SectorsPerTrack = DeviceInfo->Info.Raw[6];
	DeviceInfo->Device.BytesPerSector = 0x200;
	DeviceInfo->TotalSectors = (DeviceInfo->Info.Raw[61] << 16) | DeviceInfo->Info.Raw[60];
    }
    
    /* Strings */
    _UnpackIdentifyDataString(DeviceInfo->SerialNumber, &DeviceInfo->Info.Raw[10], 20);
    _UnpackIdentifyDataString(DeviceInfo->FirmwareRevision, &DeviceInfo->Info.Raw[23], 8);
    _UnpackIdentifyDataString(DeviceInfo->ModelNumber, &DeviceInfo->Info.Raw[27], 40);

    DEBUG("C/H/S: %d/%d/%d\n", DeviceInfo->Cylinders, DeviceInfo->Heads, DeviceInfo->SectorsPerTrack);
    DEBUG("Total sectors: %d\n", DeviceInfo->TotalSectors);
    DEBUG("Model number: %s\n", DeviceInfo->ModelNumber);
    DEBUG("Firmware revision: %s\n", DeviceInfo->FirmwareRevision);
    DEBUG("Serial number: %s\n", DeviceInfo->SerialNumber);
}

static void
_UnpackIdentifyDataString(char * Destination, unsigned short * Source, unsigned int Length)
{
    unsigned int i;
    char * SourcePtr = (char *)Source;

    for (i = 0; i < Length; i += 2) {
	Destination[i] = SourcePtr[i + 1];
	Destination[i + 1] = SourcePtr[i];
    }
    Destination[Length] = 0;
}

static int
_GetPartitionInfo(cyg_io_handle_t Handle)
{
    unsigned char MBR[512];
    int Status;
    int Partition;
    PartitionTable_T * Table;
    ATAPrivateDeviceInfo_T * DeviceInfo;
#if 0
    ATAPrivatePartitionInfo_T * PartitionInfo;
#endif
    
    DEBUG_FUN_ENTRY();
    
    DeviceInfo = (ATAPrivateDeviceInfo_T *)((cyg_devtab_entry_t *)Handle)->priv;

    /* Read master boot record */
    do {
	Status = ATARead(&DeviceInfo->Device, 0, 1, MBR);
	if (Status == NO_ERROR) {

	    /* Check for valid partition table */
	    Table = (PartitionTable_T *)(MBR + 0x1be);
	    if (Table->Signature == VALID_SIGNATURE) {
		
		/* Read primary partition entries */
		DEBUG("Disk Drive: /dev/hd%c\n", DeviceInfo->Device.Position == SELECT_MASTER ? 'a' : 'b');
		for (Partition = 0; Partition < 4; ++Partition) {
		    DEBUG("Name: /dev/hd%c/%d ", DeviceInfo->Device.Position == SELECT_MASTER ? 'a' : 'b', Partition);
		    DEBUG("Flags: %s ", Table->Entry[Partition].Active ? "Boot" : "    ");
		    DEBUG("Part Type: Primary ");
		    DEBUG("FS Type: %2x ", Table->Entry[Partition].Type);
		    DEBUG("Size (MB): %d\n", Table->Entry[Partition].NumberOfSectors / 2048);
		    /* Partition types
		       01 dos fat 12
		       04 dos fat 16 up to 32M
		       06 dos fat 16 over 32M
		       0b win98 fat
		       0c win95 fat 
		       0d ?? fat something
		       0e win95 fat 16

		       07 ntfs

		       05 dos extended partition
		    */

#if 0
		    DeviceInfo->PartitionInfo[Partition].Type = Table->Entry[Partition].Type;
		    DeviceInfo->PartitionInfo[Partition].StartSector = Table->Entry[Partition].RelativeStartingSector;
		    DeviceInfo->PartitionInfo[Partition].EndSector = Table->Entry[Partition].RelativeStartingSector +
			Table->Entry[Partition].NumberOfSectors - 1;
		    DEBUG("%x %x %x\n", DeviceInfo->PartitionInfo[Partition].Type,
			  DeviceInfo->PartitionInfo[Partition].StartSector,
			  DeviceInfo->PartitionInfo[Partition].EndSector);
#endif
		}
	    }
	    else {
		DEBUG("%s %d Invalid partition table signature %x\n", __FUNCTION__, __LINE__, Table->Signature);
		Status = !NO_ERROR;
	    }
	    break;
	}
	DEBUG("%s %d %x\n", __FUNCTION__, __LINE__, Status);
    } while (_ProcessATAStatus(Status) == ERROR_TRY_AGAIN);

    DEBUG_FUN_EXIT();
    return Status;
}

static Cyg_ErrNo
_ATALookup(struct cyg_devtab_entry ** Entry,
	   struct cyg_devtab_entry * SubEntry,
	   const char * Name)
{
    static bool Initialized = false;
    Cyg_ErrNo Status;
    ATAPrivateDeviceInfo_T * DeviceInfo;
    int DiagnosticStatus;
    int IdentifyStatus;

    DEBUG_FUN_ENTRY();

    DeviceInfo = (ATAPrivateDeviceInfo_T *)((cyg_devtab_entry_t *)(*Entry))->priv;

    cyg_semaphore_wait( &ata_block_sem );
    
    /* This block of code will touch both of the devices on the bus */
    if (!Initialized) {

	/* Reset the bus */
	ATAHardReset();
	ATASoftReset();

	// wait a while after the soft reset - this is microdrive specific
	cyg_thread_delay(1);
	
#if defined(USE_IRQ)
	/* Unmask the interrupt */
	cyg_drv_interrupt_unmask(ATA_IRQ);
#endif /* USE_IRQ */
	
	/* Make sure devices are good */
	DiagnosticStatus = ATAExecuteDeviceDiagnostic(&DeviceInfo->Device);
	if (DiagnosticStatus != NO_ERROR) {
	    /* TODO Find out which device(s) are bad.  Then barf all over self */
	}
	Initialized = true;
    }
    
    /* Identify device on the bus */
    IdentifyStatus = _IdentifyDevice(*Entry);
    if (IdentifyStatus == NO_ERROR) {

#if 0
	/* Locate partitions */
	_GetPartitionInfo(*Entry);
#endif

	Status = ENOERR;
    }
    else {

	/* Device not OK */
	Status = -EIO;
    }
    
    cyg_semaphore_post( &ata_block_sem );

    DEBUG_FUN_EXIT();
    return Status;
}

static Cyg_ErrNo
_ATAWrite(cyg_io_handle_t Handle, const void * Buffer, cyg_uint32 * Length, cyg_uint32 Block)
{
    int ATAStatus;
    int RetryCount;
    ATAPrivateDeviceInfo_T * DeviceInfo;
    
    DEBUG_FUN_ENTRY();

    DeviceInfo = (ATAPrivateDeviceInfo_T *)((cyg_devtab_entry_t *)Handle)->priv;
    
    RetryCount = 0;
    do {
	if (DeviceInfo->Device.Interface == ATA) {
	    ATAStatus = ATAWrite(&DeviceInfo->Device, Block, *Length, Buffer);
	}
	else {
	    ATAStatus = ATAPIWrite(&DeviceInfo->Device, Block, *Length, Buffer);
	}
	DEBUG("%s %d %x\n", __FUNCTION__, __LINE__, ATAStatus);
    } while ((++RetryCount) < MAX_RETRIES && _ProcessATAStatus(ATAStatus) == ERROR_TRY_AGAIN);
    
    DEBUG_FUN_EXIT();
    return _Errno(ATAStatus);
}

static Cyg_ErrNo
_ATARead(cyg_io_handle_t Handle, void * Buffer, cyg_uint32 * Length, cyg_uint32 Block)
{
    int ATAStatus;
    int RetryCount;
    ATAPrivateDeviceInfo_T * DeviceInfo;
    
    DEBUG_FUN_ENTRY();

    DeviceInfo = (ATAPrivateDeviceInfo_T *)((cyg_devtab_entry_t *)Handle)->priv;
    
    RetryCount = 0;
    do {
	if (DeviceInfo->Device.Interface == ATA) {
	    ATAStatus = ATARead(&DeviceInfo->Device, Block, *Length, Buffer);
	}
	else {
	    ATAStatus = ATAPIRead(&DeviceInfo->Device, Block, *Length, Buffer);
	}
    } while ((++RetryCount) < MAX_RETRIES && _ProcessATAStatus(ATAStatus) == ERROR_TRY_AGAIN);
    
    DEBUG_FUN_EXIT();
    return _Errno(ATAStatus);
}

static cyg_bool
_ATASelect(cyg_io_handle_t Handle, cyg_uint32 Which, CYG_ADDRWORD Info)
{
    cyg_bool Status;

    DEBUG_FUN_ENTRY();

    Status = false;

    DEBUG_FUN_EXIT();
    return Status;
}

static Cyg_ErrNo
_ATAGetConfig(cyg_io_handle_t Handle, cyg_uint32 Key, void * Buffer, cyg_uint32 * Length)
{
    Cyg_ErrNo Status;
    ATAPrivateDeviceInfo_T * DeviceInfo;
    
    DEBUG_FUN_ENTRY();
    
    DeviceInfo = (ATAPrivateDeviceInfo_T *)((cyg_devtab_entry_t *)Handle)->priv;

    cyg_semaphore_wait( &ata_block_sem );
    
    switch (Key) {
	case IO_BLK_GET_CONFIG_GEOMETRY: 
	{
	    drive_geometry_t * dg = (drive_geometry_t *)Buffer;
	    
	    dg->cyl = DeviceInfo->Cylinders;
	    dg->hd = DeviceInfo->Heads;
	    dg->sec = DeviceInfo->SectorsPerTrack;
	    dg->bytes_p_sec = DeviceInfo->Device.BytesPerSector;
	    dg->num_blks = DeviceInfo->TotalSectors;
	    memcpy(dg->serial_num, DeviceInfo->SerialNumber, 20);
	    memcpy(dg->model_num, DeviceInfo->ModelNumber, 40);
	    Status = ENOERR;
	    break;
	}

      case IO_BLK_GET_CONFIG_MEDIA_STATUS:
      {
	// always say that media is here
	*(int*) Buffer = 1;
	Status = ENOERR;
	break;
      }
      case IO_BLK_GET_CONFIG_LBA_LAST_ERROR:
      {
	// always say we have no error
	*(unsigned int*) Buffer = 0;
	Status = ENOERR;
	break;
      }
      
      default:
	{
	    Status = -ENOSYS;
	    break;
	}
    }

    cyg_semaphore_post( &ata_block_sem );
    
    DEBUG_FUN_EXIT();
    return Status;
}

static Cyg_ErrNo
_ATASetConfig(cyg_io_handle_t Handle, cyg_uint32 Key, const void * Buffer, cyg_uint32 * Length)
{
    Cyg_ErrNo Status;
    ATAPrivateDeviceInfo_T * DeviceInfo;
    int dev_res;
    static int ref_cnt = 0;
    
    DEBUG_FUN_ENTRY();

    DeviceInfo = (ATAPrivateDeviceInfo_T *)((cyg_devtab_entry_t *)Handle)->priv;

    cyg_semaphore_wait( &ata_block_sem );
    
    switch (Key) {
	case IO_BLK_SET_CONFIG_EJECT_MEDIA: 
	{
	    ATAMediaEject(&DeviceInfo->Device);
	    Status = ENOERR;
	    break;
	}

      case IO_BLK_SET_CONFIG_OPEN:
      {
	Status = ENOERR;  // empty
	break;
      }
      case IO_BLK_SET_CONFIG_SLEEP:
      {
	dev_res = ATASleep( &DeviceInfo->Device );
	Status = dev_res_to_errno(dev_res);
	break;
      }
      case IO_BLK_SET_CONFIG_WAKEUP:
      {
	  int num_retries;

	  num_retries = 0;
	  do {
	      
	      //diag_printf("%s %d\n", __FILE__, __LINE__);
	      dev_res = ATASoftReset();
#if 0
	      // can safely avoid doing this i think
	      if (dev_res == NO_ERROR) {
		  dev_res = DEVICE_INIT();
	      }
#endif
	      ++num_retries;
	  } while (dev_res != NO_ERROR && num_retries < MAX_RETRIES);
	  Status = dev_res_to_errno(dev_res);
	  break;
      }
	case IO_PM_SET_CONFIG_POWER_DOWN:
	{
	    /* put the drive in reset mode */
	    *(volatile cyg_uint8 *)PBDR &= ~0x01;
#ifdef PHASE_3
	    /* turn off power to the drive */
	    *(volatile cyg_uint8 *)PDDR |= 0x04;
#endif /* PHASE_3 */
	    Status = ENOERR;
	    break;
	}
	case IO_PM_SET_CONFIG_POWER_UP:
	{
	    unsigned int delay;
	    int dev_res;
	    
#ifdef PHASE_3
	    /* power on drive */
	    *(volatile cyg_uint8 *)PDDR &= ~0x04;
#endif /* PHASE_3 */
	    
	    /* do a hard power on/power off for the device */
	    *(volatile cyg_uint8 *)PBDR |= 0x01;
	    for (delay = 0; delay < 100000; delay++)
		;
	    *(volatile cyg_uint8 *)PBDR &= ~0x01;
	    for (delay = 0; delay < 100000; delay++)
		;
	    *(volatile cyg_uint8 *)PBDR |= 0x01;    

	    /* delay 100ms.  this is only necessary for 16 bit model */
	    cyg_thread_delay(10);
	    
	    dev_res = ATASoftReset();
	    if (dev_res != NO_ERROR) {
		Status = -EIO;
	    }
	    else {
		Status = dev_res_to_errno(dev_res);
	    }
	    break;
	}
	case IO_PM_SET_CONFIG_REGISTER:
	{
	    ++ref_cnt;
	    if (ref_cnt == 1) {
		unsigned int delay;
		int dev_res;
		
#ifdef PHASE_3
		/* power on drive */
		*(volatile cyg_uint8 *)PDDR &= ~0x04;
#endif /* PHASE_3 */
		
		/* do a hard power on/power off for the device */
		*(volatile cyg_uint8 *)PBDR |= 0x01;
		for (delay = 0; delay < 100000; delay++)
		    ;
		*(volatile cyg_uint8 *)PBDR &= ~0x01;
		for (delay = 0; delay < 100000; delay++)
		    ;
		*(volatile cyg_uint8 *)PBDR |= 0x01;    
		
		/* delay 100ms.  this is only necessary for 16 bit model */
		cyg_thread_delay(10);
		
		dev_res = ATASoftReset();
		if (dev_res != NO_ERROR) {
		    Status = -EIO;
		}
		else {
		    Status = dev_res_to_errno(dev_res);
		}
	    }
	    break;
	}
	    
	case IO_PM_SET_CONFIG_UNREGISTER:
	{
	    --ref_cnt;
	    if (ref_cnt < 0) {
		ref_cnt == 0;
	    }
	    if (ref_cnt == 0) {
		/* put the drive in reset mode */
		*(volatile cyg_uint8 *)PBDR &= ~0x01;
#ifdef PHASE_3
		/* turn off power to the drive */
		*(volatile cyg_uint8 *)PDDR |= 0x04;
#endif /* PHASE_3 */
	    }
	    Status = ENOERR;
	    break;
	}
    	default:
	{
	    Status = -ENOSYS;
	    break;
	}
    }

    cyg_semaphore_post( &ata_block_sem );
    
    DEBUG_FUN_EXIT();
    return Status;
}

static int
_ProcessATAStatus(int ATAStatus)
{
    switch (ATAStatus) {
	case ERROR_WAIT_FOR_DEVICE:
	    if (PollStatus(STATUS_BSY, STATUS_DRDY, TIMEOUT_5S)) {
		return ERROR_TIMEOUT;
	    }
	    return ERROR_TRY_AGAIN;
	    break;

	case ERROR_OUT_OF_PHASE:
	case ERROR_TIMEOUT:
	    /* TODO Reset device */
	    return ERROR_TRY_AGAIN;
	    break;
	    
	default:
	    return ATAStatus;
	    break;
    }
}

static Cyg_ErrNo
_Errno(int ATAStatus) 
{
    switch (ATAStatus) {
	case NO_ERROR:
	    return ENOERR;
	    
	case ERROR_NO_MEDIA:
	    DEBUG_FUNCFAIL("%s %d\n", __FUNCTION__, __LINE__);
	    return -ENOMED;
	    
	case ERROR_MEDIA_CHANGED:
	    DEBUG_FUNCFAIL("%s %d\n", __FUNCTION__, __LINE__);
	    return -EMEDCHG;

	case ERROR_SLEEP:
	    DEBUG_FUNCFAIL("%s %d\n", __FUNCTION__, __LINE__);
	    return -ESLEEP;
	    	    
	case ERROR_OUT_OF_PHASE:
	case ERROR_TIMEOUT:
	    DEBUG_FUNCFAIL("%s %d\n", __FUNCTION__, __LINE__);
	    return -EIO;
	    
	default:
	    DEBUG_FUNCFAIL("%s %d Unaccounted for error %x\n", __FUNCTION__, __LINE__, ATAStatus);
	    return -EIO;
    }
}

#if defined(USE_IRQ)
static cyg_uint32
_ATAInterruptISR(cyg_vector_t Vector, cyg_addrword_t Data)
{
    cyg_drv_interrupt_mask(ATA_IRQ);
    return (CYG_ISR_HANDLED | CYG_ISR_CALL_DSR);
}

static void
_ATAInterruptDSR(cyg_vector_t Vector, cyg_ucount32 Count, cyg_addrword_t Data)
{
    unsigned char Status;
    
    cyg_semaphore_post(&ATASem);

    /* Clear the interrupt */
    /* The interrupt would get cleared fine in _ProcessInterrupt without the
     * following two lines, but then this code would have to be repeated in
     * quite a few places.  Short story, cleaner to put it here */
    _ReadStatus(Status);
    cyg_drv_interrupt_unmask(ATA_IRQ);
}
#endif /* USE_IRQ */

static Cyg_ErrNo
dev_res_to_errno(int dev_res) 
{
    switch (dev_res) {
	case NO_ERROR:
	    return ENOERR;
	    
	case ERROR_NO_MEDIA:
	case 4:  // ASC = 0x04, logical unit not ready
	    return -ENOMED;
	    
	case ERROR_MEDIA_CHANGED:
	    return -EMEDCHG;

	case ERROR_SLEEP:
	    return -ESLEEP;
	    	    
	case ERROR_OUT_OF_PHASE:
	case ERROR_TIMEOUT:
	    return -EIO;

#if 0
	case ERROR_READ_ERROR:
	    return -EREAD;
#endif
	default:
	    diag_printf("Unaccounted for error %x\n", dev_res);
	    return -EIO;
    }
}

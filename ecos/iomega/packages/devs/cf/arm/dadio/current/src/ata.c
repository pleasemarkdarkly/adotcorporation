#include <cyg/kernel/kapi.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_edb7xxx.h>
#include <dadio/io/ata.h>
#include "_ata.h"

//#define DEBUG(s...) diag_printf(##s)
#define DEBUG(s...) /**/
//#define _DEBUG(s...) diag_printf(##s)
#define _DEBUG(s...) /**/

/* TODO Media access causes a transition from IDLE or STANDBY to ACTIVE */
/* Order of power consumption, highest first is ACTIVE, IDLE, STANDBY, SLEEP */
typedef enum {ACTIVE, IDLE, STANDBY, SLEEP} PowerState_T;
static PowerState_T _PowerState;
static char * _SenseKeyDescription[] = 
{
    "NO SENSE",
    "RECOVERED ERROR",
    "NOT READY",
    "MEDIUM ERROR",
    "HARDWARE ERROR",
    "ILLEGAL REQUEST",
    "UNIT ATTENTION",
    "DATA PROTECT",
    "BLANK CHECK",
    "VENDOR-SPECIFIC",
    "COPY ABORTED",
    "ABORTED COMMAND",
    "Obsolete",
    "VOLUME OVERFLOW",
    "MISCOMPARE",
    "Reserved"
};

#define WAIT_1MS 0x4800
#define WAIT_1US 0x000b
static void _Wait(unsigned int Delay);

static int _SendCommand(DeviceInfo_T * DeviceInfo, ATATaskFile_T * TaskFile);
static int _ProcessInterrupt(DeviceInfo_T * DeviceInfo, ATATaskFile_T * TaskFile);

int
ATAHardReset(void)
{
    int ATAStatus;
    
    /* Hardware reset protocol */

    *(volatile cyg_uint8 *)PBDDR |= 0x01;
    
    /* Assert reset */

    *(volatile cyg_uint8 *)PBDR |= 0x01;
    _Wait(WAIT_1US * 25);
    
    *(volatile cyg_uint8 *)PBDR &= ~0x01;

    /* Wait for at least 25 us */
    _Wait(WAIT_1US * 25);
    
    /* Negate reset and wait */
    *(volatile cyg_uint8 *)PBDR |= 0x01;

    //    *(volatile cyg_uint8 *)PBDDR &= ~0x01;
    /* Wait for at least 2 ms */
    _Wait(WAIT_1MS * 2);
    
    /* Check status */
    if (PollStatus(STATUS_BSY, 0, TIMEOUT_5S)) {
      diag_printf("PollStatus() timed out\n");
	ATAStatus = ERROR_TIMEOUT;
    }
    else {
	ATAStatus = NO_ERROR;	
	_PowerState = ACTIVE;
    }
    
    return ATAStatus;
}

/* Mandatory for both PACKET and non-PACKET devices */
int
ATASoftReset(void)
{
    unsigned char Error;
    int ATAStatus = NO_ERROR;
    
    /* Software reset protocol */
    
  sr_set_srst:
#if defined(USE_IRQ)
    _WriteDeviceControl((CONTROL_SRST|CONTROL_IRQ));
#else /* USE_IRQ */
    _WriteDeviceControl((CONTROL_SRST|CONTROL_POLLED));
#endif /* USE_IRQ */
    _Wait(WAIT_1US * 5);
    goto sr_clear_wait;
    
  sr_clear_wait:
#if defined(USE_IRQ)
    _WriteDeviceControl(CONTROL_IRQ);
#else /* USE_IRQ */
    _WriteDeviceControl(CONTROL_POLLED);
#endif /* USE_IRQ */
    _Wait(WAIT_1MS * 2);
    goto sr_check_status;

  sr_check_status:
    if (PollStatus(STATUS_BSY, 0, TIMEOUT_5S)) {
	ATAStatus = ERROR_TIMEOUT;
    }
    else {

	/* Check ending status */
	_ReadError(Error);
	DEBUG("%s %d %x\n", __FUNCTION__, __LINE__, Error);

	/* Check signature */
	if (CheckSignature() == NOT_PACKET_DEVICE) {
	    DEBUG("%s %d Device does not support packet interface\n", __FUNCTION__, __LINE__);
	}
	else {
	    DEBUG("%s %d Device supports packet interface\n", __FUNCTION__, __LINE__);
	}

	ATAStatus = NO_ERROR;
	_PowerState = ACTIVE;
    }

    return ATAStatus;
}

int
ATADeviceReset(DeviceInfo_T * DeviceInfo)
{
    ATATaskFile_T TaskFile;

    memset(&TaskFile, 0, sizeof(TaskFile));
    TaskFile.DeviceHead = DeviceInfo->Position;
    TaskFile.Command = ATA_DEVICE_RESET;
    _SendCommand(DeviceInfo, &TaskFile);   

    _PowerState = ACTIVE;

    return NO_ERROR;
}

int
ATAIdle(DeviceInfo_T * DeviceInfo)
{
    int ATAStatus;
    
    /* TODO This command */
    ATAStatus = NO_ERROR;
    _PowerState = IDLE;
    return ATAStatus;
}

int
ATAIdleImmediate(DeviceInfo_T * DeviceInfo)
{
    int ATAStatus;
    
    /* TODO This command */
    ATAStatus = NO_ERROR;
    _PowerState = IDLE;
    return ATAStatus;
}

int
ATAStandby(DeviceInfo_T * DeviceInfo)
{
    int ATAStatus;
    
    /* TODO This command */
    ATAStatus = NO_ERROR;
    _PowerState = STANDBY;
    return ATAStatus;
}

int
ATAStandbyImmediate(DeviceInfo_T * DeviceInfo)
{
    int ATAStatus;
    
    /* TODO This command */
    ATAStatus = NO_ERROR;
    _PowerState = STANDBY;
    return ATAStatus;
}

int
ATASleep(DeviceInfo_T * DeviceInfo)
{
  ATATaskFile_T TaskFile;
    int ATAStatus;

    memset( &TaskFile, 0, sizeof( TaskFile ) );
    TaskFile.Command = ATA_SET_SLEEP_MODE;
    
    ATAStatus = _SendCommand( DeviceInfo, &TaskFile );

    _PowerState = SLEEP;
    return ATAStatus;
}
#if 0
int
ATAWakeup( DeviceInfo_T * DeviceInfo ) 
{
  ATATaskFile_T TaskFile;
  int ATAStatus;

  memset( &TaskFile, 0, sizeof( TaskFile ) );
  TaskFile.Command = ATA_
#endif
int
ATAExecuteDeviceDiagnostic(DeviceInfo_T * DeviceInfo)
{
    ATATaskFile_T TaskFile;
    int ATAStatus;
    unsigned char Error;
    unsigned char DeviceHead;

    // diag_printf("Execute device diagnostic\n");
    

    memset(&TaskFile, 0, sizeof(TaskFile));
    TaskFile.DeviceHead = DeviceInfo->Position; /* This is ignored */
    TaskFile.Command = ATA_EXECUTE_DEVICE_DIAGNOSTIC;

    ATAStatus = _SendCommand(DeviceInfo, &TaskFile);
#if 0
    if (ATAStatus == NO_ERROR) {
#endif
	/* Check diagnostic code */
	/* The meaning of this code varies depending on whether it is in the
	 * Device 0 error register or the Device 1 error register.  I'm assuming that
	 * the DEV bit indicates which error register is being read. */
	_ReadDeviceHead(DeviceHead);
	_ReadError(Error);
	DEBUG("%s %d %x\n", __FUNCTION__, __LINE__, Error);
	if (Error == 0x01) {
	    if (DeviceHead & SELECT_SLAVE) {

		/* This may be from device 0 if device 1 is not present */
		_DEBUG("Device 1 passed\n");
	    }
	    else {
		_DEBUG("Device 0 passed, Device 1 passed or not present\n");
	    }
	}
	else if (Error == 0x00 || (0x02 <= Error && Error <= 0x7f)) {
	    if (DeviceHead & SELECT_SLAVE) {

		/* This may be from device 0 if device 1 is not present */
		_DEBUG("Device 1 failed\n");
		ATAStatus = ERROR_DIAGNOSTIC_FAILURE;
	    }
	    else {
		_DEBUG("Device 0 failed, Device 1 passed or not present\n");
		ATAStatus = ERROR_DIAGNOSTIC_FAILURE;
	    }
	}
	/* The following codes are not listed for Device 1 error register */
	else if (Error = 0x81) {
	    _DEBUG("Device 0 passed, Device 1 failed\n");
	    ATAStatus = ERROR_DIAGNOSTIC_FAILURE;
	}
	else if (Error == 0x80 || (0x82 <= Error && Error <= 0xff)) {
	    _DEBUG("Device 0 failed, Device 1 passed\n");
	    ATAStatus = ERROR_DIAGNOSTIC_FAILURE;
	}

	/* Check signature */
	if (ATAStatus == NO_ERROR) {
	    if (CheckSignature() == NOT_PACKET_DEVICE) {
		_DEBUG("%s %d Device does not support packet interface\n", __FUNCTION__, __LINE__);
	    }
	    else {
		_DEBUG("%s %d Device supports packet interface\n", __FUNCTION__, __LINE__);
	    }
	}
#if 0
    }
    else if (ATAStatus == !NO_ERROR) {
	
	/* Error recovery */
	_ReadError(Error);
	DEBUG("%s %d Execute device dianostic error, 0x%x\n", __FUNCTION__, __LINE__, Error);
	ATAStatus = ERROR_DIAGNOSTIC_FAILURE;
    }
#endif
    return ATAStatus;
}

int
ATAMediaEject(DeviceInfo_T * DeviceInfo)
{
    ATATaskFile_T TaskFile;
    int ATAStatus;
    unsigned char Status;

#if 0
#else
    
    /* Cirrus */
    _WriteCommand(ATA_MEDIA_EJECT);
#endif
}

int
ATAIdentifyDevice(DeviceInfo_T * DeviceInfo, unsigned char * Data)
{
    ATATaskFile_T TaskFile;
    int ATAStatus;
    unsigned char Status;

    /* Check prerequisites */
    _ReadStatus(Status);
    if (!(Status & STATUS_DRDY)) {
	return ERROR_WAIT_FOR_DEVICE;
    }

    memset(&TaskFile, 0, sizeof(TaskFile));
    TaskFile.Data = (short *)Data;
    TaskFile.DataLength = 256;	/* Number of words */
    TaskFile.DeviceHead = DeviceInfo->Position;
    TaskFile.Command = ATA_IDENTIFY_DEVICE;
    ATAStatus = _SendCommand(DeviceInfo, &TaskFile);
    return ATAStatus;
}

int
ATAIdentifyPacketDevice(DeviceInfo_T * DeviceInfo, unsigned char * Data)
{
    ATATaskFile_T TaskFile;
    int ATAStatus;
    
    memset(&TaskFile, 0, sizeof(TaskFile));
    TaskFile.Data = (short *)Data;
    TaskFile.DataLength = 256;	/* Number of words */
    TaskFile.DeviceHead = DeviceInfo->Position;
    TaskFile.Command = ATA_IDENTIFY_PACKET_DEVICE;
    ATAStatus = _SendCommand(DeviceInfo, &TaskFile);
    return ATAStatus;
}

int
ATAPacket(DeviceInfo_T * DeviceInfo, unsigned char * Packet, unsigned char * Data)
{
    ATATaskFile_T TaskFile;
    int ATAStatus;
    unsigned char Error;
    
    memset(&TaskFile, 0, sizeof(TaskFile));

    TaskFile.Data = (unsigned short *)Data;
    TaskFile.PacketCommand = Packet;
    
    /* Set byte count limit to sector size */
    TaskFile.CylinderLow = DeviceInfo->BytesPerSector & 0x00ff;
    TaskFile.CylinderHigh = (DeviceInfo->BytesPerSector & 0xff00) >> 8;

    TaskFile.DeviceHead = DeviceInfo->Position;
    TaskFile.Command = ATA_PACKET;
    ATAStatus = _SendCommand(DeviceInfo, &TaskFile);
    if (ATAStatus == NO_ERROR) {
	return NO_ERROR;
    }
    else if (ATAStatus == !NO_ERROR) {
	_ReadError(Error);
	_DEBUG("%s %d Sense Key: %s\n", __FUNCTION__, __LINE__, _SenseKeyDescription[(Error & 0xf0) >> 4]);
	return !NO_ERROR;
    }
    else {
	return ATAStatus;
    }
}

int
ATAWrite(DeviceInfo_T * DeviceInfo, unsigned int LBA, unsigned short Length, const unsigned char * Data)
{
    ATATaskFile_T TaskFile;
    int ATAStatus;
    unsigned char Status;
    unsigned char Error;

    /* Check prerequisites */
    _ReadStatus(Status);
    if (!(Status & STATUS_DRDY)) {
	return ERROR_WAIT_FOR_DEVICE;
    }
    
    memset(&TaskFile, 0, sizeof(TaskFile));
    TaskFile.Data = (short *)Data;
    TaskFile.DataLength = Length * 256;

    if (Length == 256) {
	TaskFile.SectorCount = 0;
    }
    else {
	TaskFile.SectorCount = Length;
    }
    TaskFile.SectorNumber = LBA & 0xff;
    TaskFile.CylinderLow = (LBA >>= 8) & 0xff;
    TaskFile.CylinderHigh = (LBA >>= 8) & 0xff;
    TaskFile.DeviceHead = DeviceInfo->Position | SELECT_LBA | (LBA & 0xf);
    TaskFile.Command = ATA_WRITE_SECTORS;
    ATAStatus = _SendCommand(DeviceInfo, &TaskFile);
    if (ATAStatus == NO_ERROR) {
	return NO_ERROR;
    }
    else if (ATAStatus == !NO_ERROR) {
	_ReadError(Error);
	if (Error & ERROR_WP) {
	    DEBUG("%s Media is write protected\n", __FUNCTION__);
	    return ERROR_WRITE_PROTECTED;
	}
	if (Error & ERROR_MC) {
	    DEBUG("%s Media has changed\n", __FUNCTION__);
	    return ERROR_MEDIA_CHANGED;
	}
	if (Error & ERROR_IDNF) {
	    DEBUG("%s Address out of range\n", __FUNCTION__);
	    return ERROR_LBA_OUT_OF_RANGE;
	}
	if (Error & ERROR_MCR) {
	    DEBUG("%s Media change request detected\n", __FUNCTION__);
	    return !NO_ERROR;	/* TODO determine appropriate error code */
	}
	if (Error & ERROR_ABRT) {
	    
	    /* Could be either command not supported or address out of range if IDNF is not set */
	    DEBUG("%s Command not supported\n", __FUNCTION__);
	    return ERROR_INVALID_COMMAND;
	}
	if (Error & ERROR_NM) {
	    DEBUG("%s No media present\n", __FUNCTION__);
	    return ERROR_NO_MEDIA;
	}
    }
    return ATAStatus;
}

/* Mandatory for all devices */
int
ATARead(DeviceInfo_T * DeviceInfo, unsigned int LBA, unsigned short Length, unsigned char * Data)
{
    ATATaskFile_T TaskFile;
    int ATAStatus;
    unsigned char Status;
    unsigned char Error;
    unsigned char CylinderHigh, CylinderLow;
    
    /* Check prerequisites */
    _ReadStatus(Status);
    if (!(Status & STATUS_DRDY)) {
	DEBUG("ERROR_WAIT_FOR_DEVICE\n");
	//return ERROR_WAIT_FOR_DEVICE;
    }
    
    memset(&TaskFile, 0, sizeof(TaskFile));
    TaskFile.Data = (short *)Data;
    TaskFile.DataLength = Length * 256;

    if (Length == 256) {
	TaskFile.SectorCount = 0;
    }
    else {
	TaskFile.SectorCount = Length;
    }
    TaskFile.SectorNumber = LBA & 0xff;
    TaskFile.CylinderLow = (LBA >>= 8) & 0xff;
    TaskFile.CylinderHigh = (LBA >>= 8) & 0xff;
    TaskFile.DeviceHead = DeviceInfo->Position | SELECT_LBA | ((LBA >>= 8)& 0x0f);
    TaskFile.Command = ATA_READ_SECTORS;
    ATAStatus = _SendCommand(DeviceInfo, &TaskFile);

    _ReadStatus(Status);
    if (!(Status & STATUS_ERR)) {

	/* Command completed succesfully */
	if (!(Status & STATUS_DRDY)) {
	    DEBUG("%s completed succesfully, but DRDY not set\n");
	}
	ATAStatus = NO_ERROR;
    }
    else {

	/* Error occured */
	_ReadError(Error);
	if (Error & ERROR_NM) {
	    DEBUG("%s No media present\n", __FUNCTION__);
	    ATAStatus = ERROR_NO_MEDIA;
	}
	else if (Error & ERROR_ABRT) {

	    /* Check if read was tried on a PACKET device */
	    _ReadCylinderHigh(CylinderHigh);
	    _ReadCylinderLow(CylinderLow);
	    if (CylinderHigh == 0xeb && CylinderLow == 0x14) {
		DEBUG("%s Device is packet device\n", __FUNCTION__);
		ATAStatus = ERROR_ABORT;
	    }
	    else {
		if (Error & ERROR_IDNF) {
		    DEBUG("%s Command not supported\n", __FUNCTION__);
		    ATAStatus = ERROR_INVALID_COMMAND;
		}
		else {
		    DEBUG("%s Address out of range\n", __FUNCTION__);
		    ATAStatus = ERROR_LBA_OUT_OF_RANGE;
		}
	    }
	}
	else if (Error & ERROR_MCR) {
	    DEBUG("%s Media change request\n", __FUNCTION__);
	    ATAStatus = !NO_ERROR; /* TODO Better error code */
	}
	else if (Error & ERROR_IDNF) {
	    DEBUG("%s Address out of range\n", __FUNCTION__);
	    ATAStatus = ERROR_LBA_OUT_OF_RANGE;
	}
	else if (Error & ERROR_MC) {
	    DEBUG("%s Media has changed\n", __FUNCTION__);
	    ATAStatus = ERROR_MEDIA_CHANGED;
	}
	else if (Error & ERROR_UNC) {
	    DEBUG("%s Data uncorrectable\n");
	    ATAStatus = !NO_ERROR;
	}
    }

    return ATAStatus;
}

int
CheckSignature(void)
{
    unsigned char SectorCount;
    unsigned char SectorNumber;
    unsigned char CylinderLow;
    unsigned char CylinderHigh;
    unsigned char DeviceHead;

    /* read the signature */
    _ReadSectorCount(SectorCount);
    _ReadSectorNumber(SectorNumber);
    _ReadCylinderLow(CylinderLow);
    _ReadCylinderHigh(CylinderHigh);
    _ReadDeviceHead(DeviceHead);
    _DEBUG("%s SC[%x] SN[%x] CL[%x] CH[%x] DH[%x]\n", __FUNCTION__, SectorCount, SectorNumber,
	   CylinderLow, CylinderHigh, DeviceHead);
    if (SectorCount == 0x01 && SectorNumber == 0x01 && CylinderLow == 0x00 &&
	CylinderHigh == 0x00 && DeviceHead == 0x00) {
	return NOT_PACKET_DEVICE;
    }
    else if (SectorCount == 0x01 && SectorNumber == 0x01 && CylinderLow == 0x14 &&
	     CylinderHigh == 0xeb && ((DeviceHead & 0xef) == 0x00)) {
	return PACKET_DEVICE;
    }

    DEBUG("-%s %d Unexpected signature, assuming non-packet device\n", __FUNCTION__, __LINE__);
    return NOT_PACKET_DEVICE;
}

static int
_SendCommand(DeviceInfo_T * DeviceInfo, ATATaskFile_T * TaskFile)
{
    unsigned char DeviceHead;
    
    /* TODO Check for media present before attempting to access task file */
    /* TODO Check power state before accessing registers */
    
    /* Bus idle protocol */
  bi_check_status:
    if (PollStatus(STATUS_BSY | STATUS_DRQ, 0, TIMEOUT_5S)) {
	return ERROR_TIMEOUT;
    }

    _ReadDeviceHead(DeviceHead);
    if ((DeviceHead & 0x10) != (TaskFile->DeviceHead & 0x10)) {
	goto bi_device_select;
    }
    else {
	goto bi_write_parameters;
    }

  bi_device_select:
    _WriteDeviceHead((TaskFile->DeviceHead & 0x10));
    goto bi_check_status;

  bi_write_parameters:
    _WriteFeatures(TaskFile->Features);
    _WriteSectorCount(TaskFile->SectorCount);
    _WriteSectorNumber(TaskFile->SectorNumber);
    _WriteCylinderLow(TaskFile->CylinderLow);
    _WriteCylinderHigh(TaskFile->CylinderHigh);
    _WriteDeviceHead(TaskFile->DeviceHead);
    goto bi_write_command;

  bi_write_command:
    _WriteCommand(TaskFile->Command);
    return _ProcessInterrupt(DeviceInfo, TaskFile);
}

static int
_ProcessInterrupt(DeviceInfo_T * DeviceInfo, ATATaskFile_T * TaskFile)
{
    unsigned char Status;
    unsigned char Error;
    int Length;

    switch (TaskFile->Command) {
	
	/* Non-data command protocol */
	case ATA_CHECK_POWER_MODE:
	case ATA_SET_SLEEP_MODE:
	{
#if defined(USE_IRQ)
	  nd_intrq_wait:
	    cyg_semaphore_wait(&ATASem);
	    goto nd_check_status;
#endif /* USE_IRQ */

	  nd_check_status:
	    if (PollStatus(STATUS_BSY, 0, TIMEOUT_5S)) {
		return ERROR_TIMEOUT;
	    }
	    _ReadStatus(Status);
	    if (Status & STATUS_ERR) {
		return !NO_ERROR;
	    }
	    return NO_ERROR;
	}

	/* PIO data-in command protocol */
	case ATA_IDENTIFY_DEVICE:
	case ATA_IDENTIFY_PACKET_DEVICE:
	case ATA_READ_SECTORS:
	{
#if defined(USE_IRQ)
	  di_intrq_wait:
	    cyg_semaphore_wait(&ATASem);
	    goto di_check_status;
#endif /* USE_IRQ */
	    
	  di_check_status:
	    if (PollStatus(STATUS_BSY, 0, TIMEOUT_5S)) {
		return ERROR_TIMEOUT;
	    }
	    _ReadStatus(Status);
	    if (Status & STATUS_DRQ) {
		goto di_transfer_data;
	    }

	    /* Error occured */
	    return !NO_ERROR;
	    
	  di_transfer_data:
	    Length = TaskFile->DataLength > 256 ? 256 : TaskFile->DataLength;
	    _ReadDataBulk(TaskFile->Data, Length);
	    TaskFile->Data += Length;
	    TaskFile->DataLength -= Length;
	    if (TaskFile->DataLength) {
#if defined(USE_IRQ)
		goto di_intrq_wait;
#endif /* USE_IRQ */

		/* Wait one PIO cycle before entering next state */
		_ReadAlternateStatus(Status);
		goto di_check_status;
	    }
	    
	    /* Optional read for status register */
	    return NO_ERROR;
	}

	/* PIO data-out command protocol */
	case ATA_WRITE_SECTORS:
	{
#if defined(USE_IRQ)
	  do_intrq_wait:
	    cyg_semaphore_wait(&ATASem);
	    goto do_check_status;
#endif /* USE_IRQ */
	    
	  do_check_status:
	    if (PollStatus(STATUS_BSY, 0, TIMEOUT_5S)) {
		return ERROR_TIMEOUT;
	    }
	    _ReadStatus(Status);
	    if (Status & STATUS_DRQ) {
		goto do_transfer_data;
	    }
	    if (Status & STATUS_ERR) {
		
		/* Error occured */
		return !NO_ERROR;
	    }
	    return NO_ERROR;

	  do_transfer_data:
	    Length = TaskFile->DataLength > 256 ? 256 : TaskFile->DataLength;
	    _WriteDataBulk(TaskFile->Data, Length);
	    TaskFile->Data += Length;
	    TaskFile->DataLength -= Length;

#if defined(USE_IRQ)
	    goto di_intrq_wait;
#endif /* USE_IRQ */
	    goto do_check_status;
	}   

	/* DMA command protocol */
	/* TODO Not implemented */

	/* Packet command protocol */
	/* TODO Command queuing and DMA not supported */
	/* TODO Check REL and SERV bits */
	case ATA_PACKET:
	{
	    unsigned char InterruptReason;
	    unsigned char ByteCountLow, ByteCountHigh;
	    unsigned short ByteCount;
	    
	  p_check_status_a:
	    if (PollStatus(STATUS_BSY, 0, TIMEOUT_5S)) {
		return ERROR_TIMEOUT;
	    }
	    _ReadStatus(Status);
	    if (Status & STATUS_DRQ) {
		goto p_send_packet;
	    }

	    /* Command complete */
	    if (Status & STATUS_ERR) {

		/* Error occured */
		return !NO_ERROR;
	    }
	    return NO_ERROR;

	    /* Awaiting command */
	  p_send_packet:

	    /* Check that device is in correct state */
	    _ReadSectorCount(InterruptReason);
	    if ((InterruptReason & INTERRUPT_IO) || !(InterruptReason & INTERRUPT_COMMAND)) {
		return ERROR_OUT_OF_PHASE;
	    }

	    /* Write command packet */
	    _WriteDataBulk((cyg_uint16 *)(TaskFile->PacketCommand), (DeviceInfo->PacketSize / 2));

#if defined(USE_IRQ)
	    goto p_intrq_wait;
#endif /* USE_IRQ */
	    goto p_check_status_b;

	  p_check_status_b:
	    
	    /* Wait one PIO transfer cycle time */
	    _ReadAlternateStatus(Status);
	    
	    if (PollStatus(STATUS_BSY, 0, TIMEOUT_5S)) {
		return ERROR_TIMEOUT;
	    }

#if defined(USE_IRQ)
	    /* TODO If interrupts and host is ready to transfer data or command complete
	     * then goto p_intrq_wait */
	    goto p_intrq_wait;
#endif /* USE_IRQ */
	    _ReadStatus(Status);
	    if (Status & STATUS_DRQ) {
		goto p_transfer_data;
	    }

	    /* Command complete */
	    /* TODO A bunch of cases here for queued commands, etc */
	    if (Status & STATUS_ERR) {
		return !NO_ERROR;
	    }
	    return NO_ERROR;

#if defined(USE_IRQ)
	  p_intrq_wait:
	    cyg_semaphore_wait(&ATASem);
	    goto p_check_status_b;
#endif /* USE_IRQ */

	  p_transfer_data:
	    _ReadSectorCount(InterruptReason);
	    if (InterruptReason & INTERRUPT_COMMAND) {
		return ERROR_OUT_OF_PHASE;
	    }

	    _ReadCylinderLow(ByteCountLow);
	    _ReadCylinderHigh(ByteCountHigh);
	    ByteCount = (ByteCountHigh << 8) | ByteCountLow;

	    /* Transfer to the host */
	    if (InterruptReason & INTERRUPT_IO) {
		_ReadDataBulk(TaskFile->Data, ByteCount);
	    }
	    /* Transfer to the device */
	    else {
		_WriteDataBulk(TaskFile->Data, ByteCount);
	    }
	    
	    goto p_check_status_b;
	}

	/* DMA queued command protocol */
	/* TODO Not implemented */

	/* Execute device diagnostic command protocol */
	case ATA_EXECUTE_DEVICE_DIAGNOSTIC:
	{
#if defined(USE_IRQ)
	  edd_intrq_wait:
	    cyg_semaphore_wait(&ATASem);
	    goto edd_check_status;
#endif /* USE_IRQ */

	  edd_wait:
	    _Wait(WAIT_1MS * 2);
	    goto edd_check_status;
	    
	  edd_check_status:
	    if (PollStatus(STATUS_BSY, 0, TIMEOUT_5S)) {
		return ERROR_TIMEOUT;
	    }
#if 0
	    /* Read the diagnostic code */
	    _ReadError(Error);
	    if (Error == NO_ERROR_DETECTED) {
		return NO_ERROR;
	    }
	    else {
		return !NO_ERROR;
	    }
#else
	    return NO_ERROR;
#endif
	}

#if 0
	/* Device reset command protocol */
	case ATA_DEVICE_RESET:
	{
	  dr_wait:
	    
	    /* TODO Wait 400ns */
	    goto dr_check_status;

	  dr_check_status:
	    if (PollStatus(STATUS_BSY, 0, TIMEOUT_5S)) {
		return ERROR_TIMEOUT;
	    }
	    _ReadStatus(Status);
	    if (Status & STATUS_ERR) {
		return !NO_ERROR;
	    }
	    return NO_ERROR;
	}
#endif
    }
}

int
PollStatus(unsigned char ClearBits, unsigned char SetBits, cyg_tick_count_t Timeout)
{
    cyg_tick_count_t Ticks;
    unsigned char Status;

    /* Do one read of status register very quickly to get out of the loop 
     * quicker if busy is already cleared */
    _ReadStatus(Status);
    
    if ((Status & ClearBits) || ((Status & SetBits) != SetBits)) {
    	Timeout += cyg_current_time();
	do {
	    _ReadStatus(Status);
	    Ticks = cyg_current_time();
	} while(((Status & ClearBits) || ((Status & SetBits) != (SetBits))) &&
		(Ticks < Timeout));
	
	if (Ticks >= Timeout) {
	    DEBUG("%s %d exit ERROR_TIMEOUT\n", __FUNCTION__, __LINE__);
	    return(ERROR_TIMEOUT);
	}
    }
    return(NO_ERROR);
}

static void
_Wait(unsigned int Delay)
{
    volatile unsigned int Count = Delay;
    
    while(Count--)
	;
}

#if !USE_MACROS
void
_ReadDataBulk(cyg_uint16 * Buffer, unsigned int Length)
{
    int i;

    for (i = 0; i < Length; ++i) {
	Buffer[i] = *((volatile cyg_uint16 *)ATA_DATA_REG);
    }
}

void
_WriteDataBulk(cyg_uint16 * Buffer, unsigned int Length)
{
    int i;
    for (i = 0; i < Length; ++i) {
	*((volatile cyg_uint16 *)ATA_DATA_REG) = Buffer[i];
    }

}

#endif /* !USE_MACROS */

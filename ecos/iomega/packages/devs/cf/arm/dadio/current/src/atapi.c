#include <cyg/infra/diag.h>
#include <dadio/io/atapi.h>
#include "_atapi.h"
#include "_ata.h"

//#define DEBUG(s...) diag_printf(##s)
#define DEBUG(s...) /**/

int _ProcessStatus(DeviceInfo_T * DeviceInfo, int Status);

int
ATAPIStartStopUnit(DeviceInfo_T * DeviceInfo,
		   unsigned char PowerCondition,
		   unsigned char LoadEject,
		   unsigned char Start)
{
    unsigned char Command[DeviceInfo->PacketSize];
    int ATAStatus;

    memset(Command, 0, DeviceInfo->PacketSize);
    Command[0] = ATAPI_START_STOP_UNIT;
    Command[4] = (PowerCondition << 4) | (LoadEject << 1) | Start;
    
    ATAStatus = ATAPacket(DeviceInfo, Command, 0);
    return _ProcessStatus(DeviceInfo, ATAStatus);
}

int
ATAPITestUnitReady(DeviceInfo_T * DeviceInfo)
{
    unsigned char Command[DeviceInfo->PacketSize];
    int ATAStatus;

    memset(Command, 0, DeviceInfo->PacketSize);
    Command[0] = ATAPI_TEST_UNIT_READY;

    ATAStatus = ATAPacket(DeviceInfo, Command, 0);
    return _ProcessStatus(DeviceInfo, ATAStatus);
}

int
ATAPIRequestSense(DeviceInfo_T * DeviceInfo, unsigned char Length, unsigned char * Data)
{
    unsigned char Command[DeviceInfo->PacketSize];
    int ATAStatus;

    memset(Command, 0, DeviceInfo->PacketSize);
    Command[0] = ATAPI_REQUEST_SENSE;
    Command[4] = Length;

    ATAStatus = ATAPacket(DeviceInfo, Command, Data);
    return ATAStatus;
}

int
ATAPIInquiry(DeviceInfo_T * DeviceInfo, unsigned char Length, unsigned char * Data)
{
    unsigned char Command[DeviceInfo->PacketSize];
    int ATAStatus;

    memset(Command, 0, DeviceInfo->PacketSize);
    Command[0] = ATAPI_INQUIRY;

    ATAStatus = ATAPacket(DeviceInfo, Command, Data);
    return _ProcessStatus(DeviceInfo, ATAStatus);
}

int
ATAPIRead(DeviceInfo_T * DeviceInfo, unsigned int LBA, unsigned short Length, unsigned char * Data)
{
    unsigned char Command[DeviceInfo->PacketSize];
    int ATAStatus;
    
    /* Perform READ(10) command */
    memset(Command, 0, DeviceInfo->PacketSize);
    Command[0] = ATAPI_READ10;
    Command[2] = (LBA >> 24) & 0xff;
    Command[3] = (LBA >> 16) & 0xff;
    Command[4] = (LBA >> 8) & 0xff;
    Command[5] = LBA & 0xff;
    Command[7] = (Length >> 8) & 0xff;
    Command[8] = Length & 0xff;

    ATAStatus = ATAPacket(DeviceInfo, Command, Data);
    return _ProcessStatus(DeviceInfo, ATAStatus);
}

int
ATAPIWrite(DeviceInfo_T * DeviceInfo, unsigned int LBA, unsigned short Length, const unsigned char * Data)
{
    unsigned char Command[DeviceInfo->PacketSize];
    int ATAStatus;
    
    /* Perform WRITE(10) command */
    memset(Command, 0, DeviceInfo->PacketSize);
    Command[0] = ATAPI_WRITE10;
    Command[2] = (LBA >> 24) & 0xff;
    Command[3] = (LBA >> 16) & 0xff;
    Command[4] = (LBA >> 8) & 0xff;
    Command[5] = LBA & 0xff;
    Command[7] = (Length >> 8) & 0xff;
    Command[8] = Length & 0xff;

    ATAStatus = ATAPacket(DeviceInfo, Command, (unsigned char *)Data);
    return _ProcessStatus(DeviceInfo, ATAStatus);
}

int
_ProcessStatus(DeviceInfo_T * DeviceInfo, int Status)
{
    unsigned char Error;
    unsigned char RequestSenseData[14];
    int ATAStatus;
    
    if (Status != NO_ERROR) {
	_ReadError(Error);
	DEBUG("%s %d Sense Key: %x\n", __FUNCTION__, __LINE__, (Error >> 4));
	switch ((Error >> 4)) {
	    case UNIT_ATTENTION: 
	    {
		memset(RequestSenseData, 0, sizeof(RequestSenseData));
		ATAStatus = ATAPIRequestSense(DeviceInfo, 14, RequestSenseData);
		if (ATAStatus == NO_ERROR) {
		    DEBUG("%s %d ASC: %x ASCQ: %x\n", __FUNCTION__, __LINE__, RequestSenseData[12],
			  RequestSenseData[13]);
		    Status = (int)RequestSenseData[12];
		}
		else {
		    Status = ATAStatus;
		}
		break;
	    }
	}
    }

    return Status;
}

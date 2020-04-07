#ifndef ATA_H
#define ATA_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MAX_PARTITIONS 8
    
/* Status codes returned from ATA functions.  The hex codes are
 * essentially the ASC. */
#define NO_ERROR                 0
#define ERROR_INVALID_COMMAND    0x20
#define ERROR_LBA_OUT_OF_RANGE   0x21
#define ERROR_INVALID_FIELD      0x24
#define ERROR_WRITE_PROTECTED    0x27
#define ERROR_MEDIA_CHANGED      0x28
#define ERROR_NO_MEDIA           0x3A
#define ERROR_DIAGNOSTIC_FAILURE 0x40
#define ERROR_ABORT              -1
#define ERROR_TIMEOUT            -2
#define ERROR_SLEEP              -3
#define ERROR_OUT_OF_PHASE       -4
#define ERROR_WAIT_FOR_DEVICE    -5
#define ERROR_TRY_AGAIN          -6

typedef enum {ATA, ATAPI} DeviceInterface_T;

typedef struct
{
    DeviceInterface_T Interface;
    unsigned char Position;
    unsigned char PacketSize;
    unsigned short BytesPerSector;
} DeviceInfo_T;
    
int ATAHardReset(void);
int ATASoftReset(void);
int ATADeviceReset(DeviceInfo_T * DeviceInfo);
int ATAExecuteDeviceDiagnostic(DeviceInfo_T * DeviceInfo);
int ATAIdentifyDevice(DeviceInfo_T * DeviceInfo, unsigned char * Data);
int ATAIdentifyPacketDevice(DeviceInfo_T * DeviceInfo, unsigned char * Data);
int ATAPacket(DeviceInfo_T * DeviceInfo, unsigned char * Packet, unsigned char * Data);
    
/* Length is number of sectors */
int ATARead(DeviceInfo_T * DeviceInfo, unsigned int LBA, unsigned short Length, unsigned char * Data);
int ATAWrite(DeviceInfo_T * DeviceInfo, unsigned int LBA, unsigned short Length, const unsigned char * Data);

/* Power managment commands */
int ATACheckPowerMode(DeviceInfo_T * DeviceInfo);
int ATAIdle(DeviceInfo_T * DeviceInfo);
int ATAIdleImmediate(DeviceInfo_T * DeviceInfo);
int ATASleep(DeviceInfo_T * DeviceInfo);
int ATAStandby(DeviceInfo_T * DeviceInfo);
int ATAStandbyImmediate(DeviceInfo_T * DeviceInfo);    

/* Removable media commands */
int ATAMediaEject(DeviceInfo_T * DeviceInfo);
    
#ifdef __cplusplus
};
#endif /* __cplusplus */
    
#endif /* ATA_H */

#ifndef ATAPI_H
#define ATAPI_H

#include <cyg/kernel/kapi.h>
#include <cyg/io/io.h>
#include <cyg/infra/cyg_type.h>
#include <dadio/io/blk_dev.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "ata.h"
#define IO_ATAPI_GET_CONFIG_GEOMETRY       0x0301 /* buf = (drive_geometry_t *) */
#define IO_ATAPI_GET_CONFIG_MEDIA_STATUS   0x0302 /* buf = (int*)             */
#define IO_ATAPI_GET_CONFIG_LBA_LAST_ERROR 0x0303 /* buf = (unsigned int *) */
#define IO_ATAPI_SET_CONFIG_SLEEP          0x0381
#define IO_ATAPI_SET_CONFIG_OPEN           0x0382
#define IO_ATAPI_SET_CONFIG_WAKEUP         0x0383

/* TODO move these error codes to cyg/xx/yyy.h */
#define ENOMED	   600
#define EMEDCHG	   601
#define ESLEEP     602
#define EREAD      603
   
int ATAPITestUnitReady(DeviceInfo_T * DeviceInfo);
int ATAPIStartStopUnit(DeviceInfo_T * DeviceInfo,
		       unsigned char PowerCondition,
		       unsigned char LoadEject,
		       unsigned char Start);
int ATAPIRequestSense(DeviceInfo_T * DeviceInfo, unsigned char Length, unsigned char * Data);
int ATAPIInquiry(DeviceInfo_T * DeviceInfo, unsigned char Length, unsigned char * Data);
int ATAPIRead(DeviceInfo_T * DeviceInfo, unsigned int LBA, unsigned short Length, unsigned char * Data);
int ATAPIWrite(DeviceInfo_T * DeviceInfo, unsigned int LBA, unsigned short Length, const unsigned char * Data);
    
#ifdef __cplusplus
};
#endif /* __cplusplus */
    
#endif /* ATAPI_H */


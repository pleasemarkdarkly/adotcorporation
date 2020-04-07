#ifndef BLK_DEV_H
#define BLK_DEV_H

#include <cyg/io/io.h>
#include <cyg/hal/drv_api.h>
#include <cyg/infra/cyg_type.h>

typedef struct blk_request_s
{
  cyg_uint32 lba;
  cyg_uint32 num_blks;
  void * buf;
} blk_request_t;


typedef struct drive_geometry_s
{
  cyg_uint16  cyl;        /* Number of cylinders */
  cyg_uint16  hd;         /* Number of Heads */
  cyg_uint16  sec;        /* Sectors per track */
  cyg_uint16  bytes_p_sec;
  cyg_uint32  num_blks;         /* Total drive logical blocks */
  unsigned char   serial_num[40];  /* Drive serial number */
  unsigned char   model_num[40];   /* Drive model number */  
} drive_geometry_t;

/* TODO these defines should be moved to cyg/io/config_keys.h */
#define IO_BLK_GET_CONFIG_GEOMETRY       0x0301 /* buf = (drive_geometry_t *) */
#define IO_BLK_GET_CONFIG_MEDIA_STATUS   0x0302 /* buf = (int*)             */
#define IO_BLK_GET_CONFIG_LBA_LAST_ERROR 0x0303 /* buf = (unsigned int *) */
#define IO_BLK_SET_CONFIG_SLEEP          0x0381
#define IO_BLK_SET_CONFIG_OPEN           0x0382
#define IO_BLK_SET_CONFIG_WAKEUP         0x0383
#define IO_BLK_SET_CONFIG_EJECT_MEDIA    0x0384

/* TODO move these error codes to cyg/xx/yyy.h */
#define ENOMED	   600
#define EMEDCHG	   601
#define ESLEEP     602
#define EREAD      603

#endif /* BLK_DEV_H */

#ifndef ATAPI_H
#define ATAPI_H
// ====================================================================
//
//      atapi.h
//
//      Compact flash device i/o
//
// ====================================================================
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   danc
// Contributors:       danc toddm
// Date:        2000-5-06
// Purpose:     ATA/ATAPI device IO
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

#include <cyg/io/io.h>
#include <cyg/hal/drv_api.h>
#include <cyg/infra/cyg_type.h>

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
#define IO_ATAPI_GET_CONFIG_GEOMETRY       0x0301 /* buf = (drive_geometry_t *) */
#define IO_ATAPI_GET_CONFIG_MEDIA_STATUS   0x0302 /* buf = (int*)             */
#define IO_ATAPI_SET_CONFIG_SLEEP          0x0381
#define IO_ATAPI_SET_CONFIG_OPEN           0x0382
#define IO_ATAPI_SET_CONFIG_WAKEUP         0x0383

/* TODO move these error codes to cyg/xx/yyy.h */
#define ENOMED	   600
#define EMEDCHG	   601
#define ESLEEP     602

#endif /* ATAPI_H */

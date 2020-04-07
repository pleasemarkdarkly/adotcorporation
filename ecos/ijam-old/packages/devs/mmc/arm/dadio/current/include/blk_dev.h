#ifndef CYGONCE_BLK_DEV_H
#define CYGONCE_BLK_DEV_H
// ====================================================================
//
//      blk_dev.h
//
//      Definition of block request structure
//
// ====================================================================
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   toddm
// Contributors:        toddm
// Date:        1999-12-06
// Purpose:     Block request structure
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

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
#define IO_BLK_GET_CONFIG_GEOMETRY       0x0A01
#if 0
#define IO_BLK_GET_CONFIG_MEDIA_PRESENT  0x0A02
#else
#define IO_BLK_GET_CONFIG_MEDIA_STATUS   0x0A02
#endif
#define IO_BLK_SET_CONFIG_RESET		 0x0A81
#define IO_BLK_SET_CONFIG_OPEN           0x0A82
#define IO_BLK_SET_CONFIG_CLOSE          0x0A83
#define IO_BLK_SET_CONFIG_ERASE          0x0A84

/* TODO move these error codes to cyg/xx/yyy.h */
#define ENOMED	   600
#define EMEDCHG	   601
#define ESLEEP     602

#endif /* CYGONCE_BLK_DEV_H */

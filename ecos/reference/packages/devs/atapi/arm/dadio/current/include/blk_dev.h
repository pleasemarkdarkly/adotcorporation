#ifndef BLK_DEV_H
#define BLK_DEV_H
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

#endif /* BLK_DEV_H */

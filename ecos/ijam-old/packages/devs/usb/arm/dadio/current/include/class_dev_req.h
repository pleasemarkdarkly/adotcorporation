//=============================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// INSERT COPYRIGHT HERE
// -------------------------------------------
//
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   toddm
// Contributors:
// Date:        2000-06-12
// Purpose:     Class specific device requests
//              
//####DESCRIPTIONEND####
//
//=============================================================================
#ifndef CLASS_DEV_REQ_H
#define CLASS_DEV_REQ_H

#include <cyg/io/io.h>
#include "usb100.h"
#include "usb.h"

#define NUM_CLASS_REQUESTS 16

extern cyg_io_handle_t control_endpointH;

/* bulk class requests */
Cyg_ErrNo bulk_only_mass_storage_reset(device_request_t * dev_req);
Cyg_ErrNo get_max_lun(device_request_t * dev_req);
/* dfu class requests */
typedef struct dfu_status_s 
{
    unsigned char bStatus;	/* status of most recent request */
    unsigned char bwPollTimeout[3]; /* minimum time in ms that host should wait before sending DFU_GETSTATUS again */
    unsigned char bState;	/* state that the device will be entering after this command is completed */
    unsigned char iString;	/* index of status description in string table */
}__attribute__((packed)) dfu_status_t;
    
Cyg_ErrNo dfu_detach(device_request_t * dev_req);
Cyg_ErrNo dfu_dnload(device_request_t * dev_req);
Cyg_ErrNo dfu_upload(device_request_t * dev_req);
Cyg_ErrNo dfu_getstatus(device_request_t * dev_req);
Cyg_ErrNo dfu_clrstatus(device_request_t * dev_req);
Cyg_ErrNo dfu_getstate(device_request_t * dev_req);
Cyg_ErrNo dfu_abort(device_request_t * dev_req);
extern Cyg_ErrNo reserved(device_request_t * dev_req); /* defined in std_dev_req.c, stalls endpoint */

extern Cyg_ErrNo (*bulk_class_device_request[NUM_CLASS_REQUESTS])(device_request_t * dev_req);
extern Cyg_ErrNo (*dfu_class_device_request[NUM_CLASS_REQUESTS])(device_request_t * dev_req);

#endif /* CLASS_DEV_REQ_H */

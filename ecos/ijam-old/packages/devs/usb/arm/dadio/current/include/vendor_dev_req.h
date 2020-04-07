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
// Purpose:     Vendor specific device requests
//              
//####DESCRIPTIONEND####
//
//=============================================================================
#ifndef VENDOR_DEV_REQ_H
#define VENDOR_DEV_REQ_H

#include "usb100.h"
#include "usb.h"

#define NUM_VENDOR_REQUESTS 16

typedef struct media_serial_num_s
{
    unsigned int msn_SerialNumberLength;
    unsigned int msn_Result;
    unsigned int msn_AuthCommand;
    unsigned int msn_Reserved;
    unsigned char msn_SerialNumberData[0];
}__attribute__((packed)) media_serial_num_t;

Cyg_ErrNo get_media_serial_number(device_request_t * dev_req);
extern Cyg_ErrNo reserved(device_request_t * dev_req); /* defined in std_dev_req.c, stalls endpoint */

extern Cyg_ErrNo (*vendor_device_request[NUM_VENDOR_REQUESTS])(device_request_t * dev_req);

#endif /* VENDOR_DEV_REQ_H */

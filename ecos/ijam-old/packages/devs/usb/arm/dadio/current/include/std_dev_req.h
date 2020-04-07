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
// Purpose:     Chapter 9 protocol requests
//              
//####DESCRIPTIONEND####
//
//=============================================================================
#ifndef STD_DEV_REQ_H
#define STD_DEV_REQ_H

#include <cyg/io/io.h>
#include "usb100.h"
#include "usb.h"

#define NUM_STD_REQUESTS 16

extern cyg_io_handle_t control_endpointH;

Cyg_ErrNo get_status(device_request_t * dev_req);
Cyg_ErrNo clear_feature(device_request_t * dev_req);
Cyg_ErrNo set_feature(device_request_t * dev_req);
Cyg_ErrNo set_address(device_request_t * dev_req);
Cyg_ErrNo get_descriptor(device_request_t * dev_req);
Cyg_ErrNo get_configuration(device_request_t * dev_req);
Cyg_ErrNo set_configuration(device_request_t * dev_req);
Cyg_ErrNo get_interface(device_request_t * dev_req);
Cyg_ErrNo set_interface(device_request_t * dev_req);
Cyg_ErrNo reserved(device_request_t * dev_req);

extern Cyg_ErrNo (*standard_device_request[NUM_STD_REQUESTS])(device_request_t * dev_req);

#endif /* STD_DEV_REQ_H */

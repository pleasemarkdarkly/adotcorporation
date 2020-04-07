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
// Purpose:     Thread to handle dispatch of control endpoint transactions
//              
//####DESCRIPTIONEND####
//
//=============================================================================
#include <cyg/kernel/kapi.h>
#include <cyg/infra/diag.h>
#include <cyg/io/io.h>

#include <dadio/io/usb100.h>
#include <dadio/io/usb_shared_data.h>
#include <dadio/io/pdiusbd12.h>
#include <dadio/io/std_dev_req.h>
#include <dadio/io/vendor_dev_req.h>
#include <dadio/io/class_dev_req.h>
#include <dadio/io/help_devreq.h>

cyg_io_handle_t control_endpointH;

extern void start_usb_idle_thread(void);

void
control_thread(cyg_addrword_t data)
{
    device_request_t dev_req;
    cyg_uint32 len;
    unsigned char req_type, req;
    Cyg_ErrNo err;
    
    err = cyg_io_lookup("/dev/usb0", &control_endpointH);
    if (err == ENOERR) {
	diag_printf("control_thread: found /dev/usb0 handle\n");
    }
    else{
	diag_printf("ERROR control thread: could not find /dev/usb0 handle\n");
    }
    
    while (1) {
	len = sizeof(dev_req);
	err = cyg_io_read(control_endpointH, &dev_req, &len);

	/* block all non-usb threads */
	start_usb_idle_thread();
	
	/* TODO should stall if read more than sizeof(dev_req), however len will always be
	 * sizeof(dev_req) since that's what is asked for.  need to do this before
	 * acknowledge */
	/* add some ioctls to get last transaction status, etc */

	/* acknowledge setup so that next packet may be recieved */
	d12_acknowledge_endpoint(0);
	d12_acknowledge_endpoint(1); /* must be done for both endpoints */

	req_type = dev_req.bmRequestType & USB_REQUEST_TYPE_MASK;
	req = dev_req.bRequest & USB_REQUEST_MASK;
#ifdef DEBUG
	help_devreq(req_type, req);
#endif /* DEBUG */

	/* dispatch request */
	if (req_type == USB_STANDARD_REQUEST) {
	    (*standard_device_request[req])(&dev_req);
	}
	else if (req_type == USB_VENDOR_REQUEST) {
	    (*vendor_device_request[req])(&dev_req);
	}
	else if (req_type == USB_CLASS_REQUEST) {
	    (*bulk_class_device_request[req])(&dev_req);
	}
	else {
	    /* stall endpoint */
	    d12_set_endpoint_status(0, 1);
	    d12_set_endpoint_status(1, 1);
	}
    }
}

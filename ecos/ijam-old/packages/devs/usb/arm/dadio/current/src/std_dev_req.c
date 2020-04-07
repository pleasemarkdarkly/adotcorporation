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
#include <cyg/hal/drv_api.h>
#include <cyg/infra/diag.h>
#include <cyg/io/io.h>

#include <dadio/io/std_dev_req.h>
#include <dadio/io/pdiusbd12.h>
#include <dadio/io/usb_shared_data.h>
#include <dadio/io/usb100.h>
#include <dadio/io/usb.h>
#include <dadio/io/descriptors.h>

/* unsupported:
 * set_descriptor
 * synch_frame
 */

Cyg_ErrNo (*standard_device_request[NUM_STD_REQUESTS])(device_request_t * dev_req) =
{
    get_status,
    clear_feature,
    reserved,
    set_feature,
    reserved,
    set_address,
    get_descriptor,
    reserved,
    get_configuration,
    set_configuration,
    get_interface,
    set_interface,
    reserved,
    reserved,
    reserved,
    reserved
};

/* Protocol functions */

Cyg_ErrNo
reserved(device_request_t * dev_req)
{
    /* stall */
    d12_set_endpoint_status(0, 1);
    d12_set_endpoint_status(1, 1);
    return ENOERR;
}

Cyg_ErrNo
get_status(device_request_t * dev_req)
{
    unsigned char status[2];
    cyg_uint32 len = 2;
    unsigned char recipient = dev_req->bmRequestType & USB_RECIPIENT;
    unsigned char endpoint, endpoint_status;
    Cyg_ErrNo res = ENOERR;
    
    if (recipient == USB_RECIPIENT_DEVICE) {
	if (usb_remote_wakeup) 
	    status[0] = 0x03;	/* remote wakeup | self powered */
	else
	    status[0] = 0x01;	/* self powered */
	status[1] = 0x00;	/* reserved, set to zero */
	res = cyg_io_write(control_endpointH, status, &len);
    }
    else if (recipient == USB_RECIPIENT_INTERFACE) {
	status[0] = status[1] = 0x00; /* reserved, set to zero */
	res = cyg_io_write(control_endpointH, status, &len);
    }
    else if (recipient == USB_RECIPIENT_ENDPOINT) {
	endpoint = (unsigned char)(dev_req->wIndex & MAX_ENDPOINTS);
	if (dev_req->wIndex & (unsigned char)USB_ENDPOINT_DIRECTION_MASK)
	    endpoint_status = d12_select_endpoint(endpoint * 2 + 1);
	else
	    endpoint_status = d12_select_endpoint(endpoint * 2);
	if (endpoint_status & D12_STALL)
	    status[0] = 0x01;	/* endpoint is currently halted */
	else
	    status[0] = 0x00;	/* endpoint is not halted */
	status[1] = 0x00;	/* reserved, set to zero */
	res = cyg_io_write(control_endpointH, status, &len);
    }
    else {
	/* stall */
	d12_set_endpoint_status(0, 1);
	d12_set_endpoint_status(1, 1);
    }
    return res;
}

Cyg_ErrNo
clear_feature(device_request_t * dev_req)
{
    unsigned char endpoint;
    unsigned char recipient = dev_req->bmRequestType & USB_RECIPIENT;
    cyg_uint32 len = 0;
    Cyg_ErrNo res = ENOERR;
    
    if (recipient == USB_RECIPIENT_DEVICE && dev_req->wValue == USB_FEATURE_REMOTE_WAKEUP) {
	usb_remote_wakeup = 0;
	res = cyg_io_write(control_endpointH, 0, &len);
    }
    else if (recipient == USB_RECIPIENT_ENDPOINT && dev_req->wValue == USB_FEATURE_ENDPOINT_STALL) {
	endpoint = (unsigned char)(dev_req->wIndex & MAX_ENDPOINTS);
	if (dev_req->wIndex & (unsigned char)USB_ENDPOINT_DIRECTION_MASK)
	    d12_set_endpoint_status(endpoint * 2 + 1, 0); /* clear stall */
	else
	    d12_set_endpoint_status(endpoint * 2, 0); /* clear stall */
	res = cyg_io_write(control_endpointH, 0, &len);
    }
    else {
	/* stall */
	d12_set_endpoint_status(0, 1);
	d12_set_endpoint_status(1, 1);
    }
    return res;
}

Cyg_ErrNo
set_feature(device_request_t * dev_req)
{
    unsigned char endpoint;
    unsigned char recipient = dev_req->bmRequestType & USB_RECIPIENT;
    cyg_uint32 len = 0;
    Cyg_ErrNo res = ENOERR;
    
    if (recipient == USB_RECIPIENT_DEVICE && dev_req->wValue == USB_FEATURE_REMOTE_WAKEUP) {    
	usb_remote_wakeup = 1;
	res = cyg_io_write(control_endpointH, 0, &len);
    }
    else if (recipient == USB_RECIPIENT_ENDPOINT && dev_req->wValue == USB_FEATURE_ENDPOINT_STALL) {
	endpoint = (unsigned char)(dev_req->wIndex & MAX_ENDPOINTS);
	if (dev_req->wIndex & (unsigned char)USB_ENDPOINT_DIRECTION_MASK)
	    d12_set_endpoint_status(endpoint * 2 + 1, 1);
	else
	    d12_set_endpoint_status(endpoint * 2, 1);
	res = cyg_io_write(control_endpointH, 0, &len);
    }
    else {
	/* stall */
	d12_set_endpoint_status(0, 1);
	d12_set_endpoint_status(1, 1);	
    }
    return res;
}

Cyg_ErrNo
set_address(device_request_t * dev_req)
{
    cyg_uint32 len = 0;
    Cyg_ErrNo res = ENOERR;
    
    d12_set_address_enable((unsigned char)(dev_req->wValue & USB_DEVICE_ADDRESS_MASK), 1);
    res = cyg_io_write(control_endpointH, 0, &len);	/* ack host with empty packet */
    return res;
}

Cyg_ErrNo
get_descriptor(device_request_t * dev_req)
{
    unsigned char descriptor = (dev_req->wValue >> 8) & 0xff;
    unsigned char index = dev_req->wValue & 0xff;
    unsigned short language_id = dev_req->wIndex;
    cyg_uint32 len;
    Cyg_ErrNo res = ENOERR;
    
    if (descriptor == USB_DEVICE_DESCRIPTOR_TYPE) {
	len = (sizeof(device_descriptor) < dev_req->wLength) ? sizeof(device_descriptor) : dev_req->wLength;
	res = cyg_io_write(control_endpointH, &device_descriptor, &len);
    }
    else if (descriptor == USB_CONFIGURATION_DESCRIPTOR_TYPE) {
	len = (configuration_descriptor.wTotalLength < dev_req->wLength) ?
	    configuration_descriptor.wTotalLength : dev_req->wLength;
	res = cyg_io_write(control_endpointH, &configuration_descriptor, &len);
    }
    else if (descriptor == USB_STRING_DESCRIPTOR_TYPE) {
	if (index == 0 || (index != 0 && language_id == 0x0409)) {
	    /* language id for us english */
	    len = (string_descriptor[index]->bLength < dev_req->wLength) ?
		string_descriptor[index]->bLength : dev_req->wLength;
	    res = cyg_io_write(control_endpointH, string_descriptor[index], &len);
	}
	else {
	    /* stall endpoint */
	    d12_set_endpoint_status(0, 1);
	    d12_set_endpoint_status(1, 1);
	}
    }
    else {
	/* stall endpoint */
	d12_set_endpoint_status(0, 1);
	d12_set_endpoint_status(1, 1);
    }
    return res;
}

Cyg_ErrNo
get_configuration(device_request_t * dev_req)
{
    cyg_uint32 len;
    Cyg_ErrNo res = ENOERR;
    
    len = sizeof(usb_configuration);
    res = cyg_io_write(control_endpointH, &usb_configuration, &len);
    return res;
}

Cyg_ErrNo
set_configuration(device_request_t * dev_req)
{
    cyg_uint32 len;
    Cyg_ErrNo res = ENOERR;
    
    if (dev_req->wValue == 0) {
	len = 0; /* ack host */
	res = cyg_io_write(control_endpointH, 0, &len);

	/* unconfigured state */
	cyg_drv_dsr_lock();
	usb_configuration = 0;
	cyg_drv_dsr_unlock();
	d12_set_endpoint_enable(0);
    }
    else if (dev_req->wValue == 1) { /* only 1 configuration for this device, whose id is 1 */
	len = 0;	/* ack host */
	res = cyg_io_write(control_endpointH, 0, &len);

	/* configure device */
	
	/* enable generic endpoints */
	d12_set_endpoint_enable(0);
	d12_set_endpoint_enable(1);

	cyg_drv_dsr_lock();
	usb_configuration = 1;
	cyg_drv_dsr_unlock();
    }
    else {
	/* stall */
	d12_set_endpoint_status(0, 1);
	d12_set_endpoint_status(1, 1);
    }
    return res;
}

Cyg_ErrNo
get_interface(device_request_t * dev_req)
{
    unsigned char interface = 0; /* only interface is 0 */
    cyg_uint32 len = 1;
    Cyg_ErrNo res = ENOERR;

    res = cyg_io_write(control_endpointH, &interface, &len);
    return res;
}

Cyg_ErrNo
set_interface(device_request_t * dev_req)
{
    cyg_uint32 len;
    Cyg_ErrNo res = ENOERR;
	
    if (dev_req->wValue == 0 && dev_req->wIndex == 0) {	/* no alternate settings for interface */
	len = 0;
	res = cyg_io_write(control_endpointH, 0, &len);
    }
    else {
	/* stall */
	d12_set_endpoint_status(0, 1);
	d12_set_endpoint_status(1, 1);
    }
    return res;
}


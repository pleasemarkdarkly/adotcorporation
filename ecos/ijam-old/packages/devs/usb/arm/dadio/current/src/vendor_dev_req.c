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
#include <cyg/io/io.h>
#include <cyg/infra/diag.h>
#include <dadio/io/blk_dev.h>
#include <stdlib.h>

#include <dadio/io/vendor_dev_req.h>
#include <dadio/io/pdiusbd12.h>

extern cyg_io_handle_t control_endpointH;

Cyg_ErrNo (*vendor_device_request[NUM_VENDOR_REQUESTS])(device_request_t * dev_req) =
{
    get_media_serial_number,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved
};

Cyg_ErrNo
get_media_serial_number(device_request_t * dev_req)
{
    cyg_uint32 len, tmp_len;
    drive_geometry_t dg;
    cyg_io_handle_t blk_devH;
    media_serial_num_t * serial_num;
    int i;
    Cyg_ErrNo res = ENOERR;

    diag_printf("get_media_serial_number\n");
    
    if (dev_req->bmRequestType & (unsigned char)USB_ENDPOINT_DIRECTION_MASK) {
	if (dev_req->wIndex == 0) {
	    /* allocate storage for serial number */
	    len = sizeof(media_serial_num_t) + sizeof(dg.serial_num);
	    serial_num = (media_serial_num_t *)malloc(len);
	    memset(serial_num, 0, len);
	    serial_num->msn_SerialNumberLength = sizeof(dg.serial_num);

	    /* get serial number, if not available fill buffer with spaces */
	    /* TODO multiple drive support somewhere, look at extended inquiry data in SPC spec */
	    cyg_io_lookup("/dev/hda", &blk_devH);
	    tmp_len = sizeof(dg);
	    if (cyg_io_get_config(blk_devH, IO_BLK_GET_CONFIG_GEOMETRY, &dg, &tmp_len) == ENOERR) {
		serial_num->msn_Result = 0; /* this will get remapped into Win32 codes in the desktop driver */
		for (i = 0; i < sizeof(dg.serial_num); ++i)
		    serial_num->msn_SerialNumberData[i] = dg.serial_num[i];
	    }
	    else {
		diag_printf("ERROR: could not get drive geometry\n");
		serial_num->msn_Result = 1; /* this will get remapped into Win32 codes in the desktop driver */
		for (i = 0; i < sizeof(dg.serial_num); ++i)
		    serial_num->msn_SerialNumberData[i] = ' ';
	    }
	    
	    /* send device info to host */
	    len = (len < dev_req->wLength) ? len : dev_req->wLength;
	    res = cyg_io_write(control_endpointH, serial_num, &len);

	    free(serial_num);
	}
	else {
	    d12_set_endpoint_status(0, 1);
	    d12_set_endpoint_status(1, 1);
	}
    }
    else {
	d12_set_endpoint_status(0, 1);
	d12_set_endpoint_status(1, 1);
    }
    return res;
}

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
#include <cyg/infra/diag.h>
#include <cyg/hal/drv_api.h>

#include <dadio/io/class_dev_req.h>
#include <dadio/io/usb_shared_data.h>
#include <dadio/io/mass_storage.h>
#include <dadio/io/pdiusbd12.h>

Cyg_ErrNo (*bulk_class_device_request[NUM_CLASS_REQUESTS])(device_request_t * dev_req) =
{
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
    get_max_lun,
    bulk_only_mass_storage_reset
};

Cyg_ErrNo
get_max_lun(device_request_t * dev_req)
{
    unsigned char lun = (MAX_LUN - 1); /* 0 based */
    cyg_uint32 len;
    Cyg_ErrNo res = ENOERR;
        
    diag_printf("get_max_lun\n");

    len = sizeof(lun);
    res = cyg_io_write(control_endpointH, &lun, &len);
    return res;
}

Cyg_ErrNo
bulk_only_mass_storage_reset(device_request_t * dev_req)
{
    cyg_uint32 len;
    Cyg_ErrNo res = ENOERR;
    
    diag_printf("bulk_only_mass_storage_reset\n");
    /* TODO mass storage reset */
    /* this gets called on a host-issued Reset Recovery */
    /* the device first stalls the bulk-out endpoint */
    /* the host then issues this call, then clear feature halt to the bulk-in and bulk-out endpoint */
    
    /* this might be ok, since the bulk_thread goes back to reading a cmd after stalling both endpoints */
    
    /* reset complete, send ack */
    len = 0;
    res = cyg_io_write(control_endpointH, 0, &len);
    return res;
}

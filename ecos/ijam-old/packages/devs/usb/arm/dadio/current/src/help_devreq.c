#include <cyg/infra/diag.h>

#include <dadio/io/help_devreq.h>
#include <dadio/io/usb100.h>
#include <dadio/io/usb.h>

char * _NAME_USB_REQUEST_RECIPIENT[] =
{
    "Device",
    "Interface",
    "Endpoint(0)",
    "Other"
};

char * _NAME_USB_REQUEST_TYPE[] =
{
    "Standard",
    "Class",
    "Vendor",
    "Reserved"
};

char * _NAME_USB_STANDARD_REQUEST[] =
{
    "GET_STATUS",
    "CLEAR_FEATURE",
    "RESERVED",
    "SET_FEATURE",
    "RESERVED",
    "SET_ADDRESS",
    "GET_DESCRIPTOR",
    "SET_DESCRIPTOR",
    "GET_CONFIGURATION",
    "SET_CONFIGURATION",
    "GET_INTERFACE",
    "SET_INTERFACE",
    "SYNC_FRAME"
};

void
help_devreq(unsigned char typ, unsigned char req)
{
    typ >>= 5;

    if(typ == USB_STANDARD_REQUEST) {
	diag_printf("Request Type = %s, Request = %s.\n", _NAME_USB_REQUEST_TYPE[typ],
	       _NAME_USB_STANDARD_REQUEST[req]);
    }
    else {
	diag_printf("Request Type = %s, bRequest = 0x%x.\n", _NAME_USB_REQUEST_TYPE[typ],
		    req);
    }
}

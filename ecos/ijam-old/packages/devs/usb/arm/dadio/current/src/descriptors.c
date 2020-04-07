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
// Purpose:     USB device descriptors
//              
//####DESCRIPTIONEND####
//
//=============================================================================
#include <dadio/io/descriptors.h>
#include <dadio/io/usb.h>

#define CONST const

/* RUN-TIME descriptors */

CONST
usb_device_descriptor_t device_descriptor = 
{
    sizeof(usb_device_descriptor_t),
    USB_DEVICE_DESCRIPTOR_TYPE,
    0x0100,			/* USB spec release number */
    0x00,			/* device class, specified in interface descriptor */
    0x00,			/* device subclass, in interface descriptor */
    0x00,			/* device protocol, in interface descriptor */
    ENDP0_PACKET_SIZE,		/* max packet size endpoint 0 */
#if 1
    0x0a76,			/* vendor id, i-jam */
    0x0002,			/* product id, i-jam */
    //    0x0003,
    //    0x0004,
    0x0100,			/* device release number */
#else
    0x059b,
    0x0031,
    0x0100
#endif
    MANUFACTURER_STRING,	/* index to manufacturer string descriptor */
    PRODUCT_STRING,	        /* index to product string descriptor */
    SERIAL_NUMBER_STRING,	/* index to serial number string descriptor */
    0x01			/* number of possible configurations */
};

CONST
usb_configuration_descriptor_t configuration_descriptor = 
{
    sizeof(usb_configuration_descriptor_t),
    USB_CONFIGURATION_DESCRIPTOR_TYPE,
    sizeof(configuration_descriptor) + 	/* total length of this configuration */
    sizeof(interface_descriptor) +
    sizeof(bulk_out_descriptor) +
    sizeof(bulk_in_descriptor),
    0x01,			/* number of interfaces, run-time and dfu */
    0x01,			/* value used to set this configuration */
    CONFIGURATION_STRING,	/* index to configuration string descriptor */
    USB_CONFIG_SELF_POWERED,	/* configuration characteristics */
    0x01			/* max power consumption */
};

/* NOTE: these definitions are treated as a linear block of memory,
 * and so must remain in this order */
CONST
usb_interface_descriptor_t interface_descriptor =
{
    sizeof(usb_interface_descriptor_t),
    USB_INTERFACE_DESCRIPTOR_TYPE,
    0x00,			/* number of this interface */
    0x00,			/* value used to select alt setting */
    0x02,			/* number of endpoints excluding control */
#if 0
    USB_DEVICE_CLASS_STORAGE,	/* class code */
#else
    0xff,
#endif
    USB_DEVICE_SUBCLASS_SCSI,	/* subclass code */
    USB_DEVICE_PROTOCOL_BOT,	/* protocol code */
    INTERFACE0_STRING		/* index of string descriptor for this interface */
};

CONST
usb_endpoint_descriptor_t bulk_out_descriptor = 
{
    sizeof(usb_endpoint_descriptor_t),
    USB_ENDPOINT_DESCRIPTOR_TYPE,
    0x82,			/* direction and address of this endpoint */
    USB_ENDPOINT_TYPE_BULK,
    ENDP2_PACKET_SIZE,
    0x00			/* interval for polling endpoint, ignored for bulk */
};

CONST
usb_endpoint_descriptor_t bulk_in_descriptor = 
{
    sizeof(usb_endpoint_descriptor_t),
    USB_ENDPOINT_DESCRIPTOR_TYPE,
    0x02,
    USB_ENDPOINT_TYPE_BULK,
    ENDP2_PACKET_SIZE,
    0
};		      

/* DFU mode descriptors */

CONST
usb_device_descriptor_t dfu_device_descriptor = 
{
    sizeof(usb_device_descriptor_t),
    USB_DEVICE_DESCRIPTOR_TYPE,
    0x0100,			/* USB spec release number */
    USB_DEVICE_CLASS_DFU,	/* device class, specified in interface descriptor */
    USB_DEVICE_SUBCLASS_DFU,	/* device subclass, in interface descriptor */
    0x00,			/* device protocol, in interface descriptor */
    ENDP0_PACKET_SIZE,		/* max packet size endpoint 0 */
    0x0a76,			/* vendor id, i-jam */
    0xffff,			/* product id */
    0x0100,			/* device release number */
    0x00,			/* no descriptive strings */
    0x00,
    0x00,
    0x01			/* number of possible configurations */
};

CONST
usb_configuration_descriptor_t dfu_configuration_descriptor = 
{
    sizeof(usb_configuration_descriptor_t),
    USB_CONFIGURATION_DESCRIPTOR_TYPE,
    sizeof(dfu_configuration_descriptor) + 	/* total length of this configuration */
    sizeof(dfu_interface_descriptor) +
    sizeof(dfu_functional_descriptor),
    0x01,			/* number of interfaces, run-time and dfu */
    0x01,			/* value used to set this configuration */
    0x00,			/* number of endpoints excluding control */
    USB_CONFIG_SELF_POWERED,	/* configuration characteristics */
    0x01			/* max power consumption */
};

/* NOTE: these definitions are treated as a linear block of memory,
 * and so must remain in this order */
CONST
usb_interface_descriptor_t dfu_interface_descriptor =
{
    sizeof(usb_interface_descriptor_t),
    USB_INTERFACE_DESCRIPTOR_TYPE,
    0x00,			/* number of this interface */
    0x00,			/* value used to select alt setting */
    0x00,			/* number of endpoints excluding control */
    USB_DEVICE_CLASS_DFU,	/* class code */
    USB_DEVICE_SUBCLASS_DFU,	/* subclass code */
    0x00,	                /* protocol code */
    0x00
};

CONST
usb_functional_descriptor_t dfu_functional_descriptor =
{
    sizeof(usb_functional_descriptor_t),
    USB_FUNCTIONAL_DESCRIPTOR_DFU_TYPE,
    USB_MANIFESTATION_TOLERANT | USB_CAN_UPLOAD | USB_CAN_DOWNLOAD,
    0x0064,			/* time in ms that device will wait after DFU_DETACH */
    0x0400			/* maximum number of bytes that device can accept per control-write transaction */
};

/* STRING descriptors */

/* NOTE: the definitions of the strings must immediately follow the descriptors */
CONST
usb_string_descriptor_t language_support_codes_string_descriptor = 
{
    sizeof(usb_string_descriptor_t) + 2,
    USB_STRING_DESCRIPTOR_TYPE
};
CONST
char language_support_codes_string[2] = {0x09,0x04}; /* little endian, language code is 0x0409 for American English */

CONST
usb_string_descriptor_t manufacturer_string_descriptor = 
{
    sizeof(usb_string_descriptor_t) + 38,
    USB_STRING_DESCRIPTOR_TYPE
};
CONST
char manufacturer_string[38] = {'I',0,'n',0,'t',0,'e',0,'r',0,'a',0,'c',0,'t',0,'i',0,'v',0,'e',0,' ',0,
				'O',0,'b',0,'j',0,'e',0,'c',0,'t',0,'s',0};

CONST
usb_string_descriptor_t product_string_descriptor = 
{
    sizeof(usb_string_descriptor_t) + 20,
    USB_STRING_DESCRIPTOR_TYPE
};
CONST
char product_string[20] = {'W',0,'M',0,'A',0,' ',0,'P',0,'l',0,'a',0,'y',0,'e',0,'r',0};

CONST
usb_string_descriptor_t serial_number_string_descriptor = 
{
    sizeof(usb_string_descriptor_t) + 2,
    USB_STRING_DESCRIPTOR_TYPE
};
CONST
char serial_number_string[2] = {'?',0}; /* should be determined from chip id, etc */

CONST
usb_string_descriptor_t configuration_string_descriptor = 
{
    sizeof(usb_string_descriptor_t) + 24,
    USB_STRING_DESCRIPTOR_TYPE
};
CONST
char configuration_string[24] = {'A',0,'u',0,'d',0,'i',0,'o',0,' ',0,'P',0,'i',0,'r',0,'a',0,'t',0,'e',0};

CONST
usb_string_descriptor_t interface0_string_descriptor = 
{
    sizeof(usb_string_descriptor_t) + 16,
    USB_STRING_DESCRIPTOR_TYPE
};
CONST
char interface0_string[16] = {'R',0,'u',0,'n',0,'-',0,'t',0,'i',0,'m',0,'e',0};

CONST
usb_string_descriptor_t dfu_interface_string_descriptor = 
{
    sizeof(usb_string_descriptor_t) + 46,
    USB_STRING_DESCRIPTOR_TYPE
};
CONST
char dfu_interface_string[46] = {'D',0,'e',0,'v',0,'i',0,'c',0,'e',0,' ',0,
				 'F',0,'i',0,'r',0,'m',0,'w',0,'a',0,'r',0,'e',0,' ',0,
				 'U',0,'p',0,'g',0,'r',0,'a',0,'d',0,'e',0};

CONST
usb_string_descriptor_t * string_descriptor[] = 
{
    &language_support_codes_string_descriptor,
    &manufacturer_string_descriptor,
    &product_string_descriptor,
    &serial_number_string_descriptor,
    &configuration_string_descriptor,
    &interface0_string_descriptor,
    &dfu_interface_string_descriptor
};

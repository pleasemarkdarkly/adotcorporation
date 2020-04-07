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
// Purpose:     USB 1.0 constants, structures
//              
//####DESCRIPTIONEND####
//
//=============================================================================
#ifndef USB100_H
#define USB100_H

#define MAXIMUM_USB_STRING_LENGTH 255

/* Values for the bits returned by the USB GET_STATUS command */
#define USB_GETSTATUS_SELF_POWERED                0x01
#define USB_GETSTATUS_REMOTE_WAKEUP_ENABLED       0x02

/* Descriptor types */
#define USB_DEVICE_DESCRIPTOR_TYPE                0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE         0x02
#define USB_STRING_DESCRIPTOR_TYPE                0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE             0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE              0x05
#define USB_POWER_DESCRIPTOR_TYPE                 0x06
#define USB_FUNCTIONAL_DESCRIPTOR_DFU_TYPE        0x21

#define USB_DESCRIPTOR_MAKE_TYPE_AND_INDEX(d, i) ((USHORT)((USHORT)d<<8 | i))

/* Values for bmAttributes field of an endpoint descriptor */
#define USB_ENDPOINT_TYPE_MASK                    0x03
#define USB_ENDPOINT_TYPE_CONTROL                 0x00
#define USB_ENDPOINT_TYPE_ISOCHRONOUS             0x01
#define USB_ENDPOINT_TYPE_BULK                    0x02
#define USB_ENDPOINT_TYPE_INTERRUPT               0x03

/* Definitions for bits in the bmAttributes field of a configuration descriptor. */
#define USB_CONFIG_POWERED_MASK                   0xc0
#define USB_CONFIG_BUS_POWERED                    0x80
#define USB_CONFIG_SELF_POWERED                   0x40
#define USB_CONFIG_REMOTE_WAKEUP                  0x20

/* Endpoint direction bit, stored in address */
#define USB_ENDPOINT_DIRECTION_MASK               0x80

/* USB defined request codes
 * See chapter 9 of the USB 1.0 specifcation for
 * more information.
 */
#define USB_REQUEST_GET_STATUS                    0x00
#define USB_REQUEST_CLEAR_FEATURE                 0x01
#define USB_REQUEST_SET_FEATURE                   0x03
#define USB_REQUEST_SET_ADDRESS                   0x05
#define USB_REQUEST_GET_DESCRIPTOR                0x06
#define USB_REQUEST_SET_DESCRIPTOR                0x07
#define USB_REQUEST_GET_CONFIGURATION             0x08
#define USB_REQUEST_SET_CONFIGURATION             0x09
#define USB_REQUEST_GET_INTERFACE                 0x0A
#define USB_REQUEST_SET_INTERFACE                 0x0B
#define USB_REQUEST_SYNC_FRAME                    0x0C

/* Defined USB device classes */
#define USB_DEVICE_CLASS_RESERVED           0x00
#define USB_DEVICE_CLASS_AUDIO              0x01
#define USB_DEVICE_CLASS_COMMUNICATIONS     0x02
#define USB_DEVICE_CLASS_HUMAN_INTERFACE    0x03
#define USB_DEVICE_CLASS_MONITOR            0x04
#define USB_DEVICE_CLASS_PHYSICAL_INTERFACE 0x05
#define USB_DEVICE_CLASS_POWER              0x06
#define USB_DEVICE_CLASS_PRINTER            0x07
#define USB_DEVICE_CLASS_STORAGE            0x08
#define USB_DEVICE_CLASS_HUB                0x09
#define USB_DEVICE_CLASS_DFU                0xFE
#define USB_DEVICE_CLASS_VENDOR_SPECIFIC    0xFF

/* Defined USB device subclass codes */
/* Storage */
#define USB_DEVICE_SUBCLASS_RBC             0x01
#define USB_DEVICE_SUBCLASS_SFF8020I        0x02
#define USB_DEVICE_SUBCLASS_QIC_157         0x03
#define USB_DEVICE_SUBCLASS_UFI             0x04
#define USB_DEVICE_SUBCLASS_SFF8070I        0x05
#define USB_DEVICE_SUBCLASS_SCSI            0x06

/* Device Firmware Upgrade */
#define USB_DEVICE_SUBCLASS_DFU             0x01

/* Defined USB device protocol codes */
/* Storage */
#define USB_DEVICE_PROTOCOL_CBI_CCI         0x00
#define USB_DEVICE_PROTOCOL_CBI             0x01
#define USB_DEVICE_PROTOCOL_BOT             0x50

/* USB defined Feature selectors */
#define USB_FEATURE_ENDPOINT_STALL          0x0000
#define USB_FEATURE_REMOTE_WAKEUP           0x0001
#define USB_FEATURE_POWER_D0                0x0002
#define USB_FEATURE_POWER_D1                0x0003
#define USB_FEATURE_POWER_D2                0x0004
#define USB_FEATURE_POWER_D3                0x0005

typedef struct usb_device_descriptor_s 
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned short bcdUSB;
    unsigned char bDeviceClass;
    unsigned char bDeviceSubClass;
    unsigned char bDeviceProtocol;
    unsigned char bMaxPacketSize0;
    unsigned short idVendor;
    unsigned short idProduct;
    unsigned short bcdDevice;
    unsigned char iManufacturer;
    unsigned char iProduct;
    unsigned char iSerialNumber;
    unsigned char bNumConfigurations;
}__attribute__((packed)) usb_device_descriptor_t;

typedef struct usb_endpoint_descriptor_s
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bEndpointAddress;
    unsigned char bmAttributes;
    unsigned short wMaxPacketSize;
    unsigned char bInterval;
}__attribute__((packed)) usb_endpoint_descriptor_t;

/* Values for bmAttributes Field in USB_CONFIGURATION_DESCRIPTOR */
#define BUS_POWERED                           0x80
#define SELF_POWERED                          0x40
#define REMOTE_WAKEUP                         0x20

typedef struct usb_configuration_descriptor_s
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned short wTotalLength;
    unsigned char bNumInterfaces;
    unsigned char bConfigurationValue;
    unsigned char iConfiguration;
    unsigned char bmAttributes;
    unsigned char MaxPower;
}__attribute__((packed)) usb_configuration_descriptor_t;

typedef struct usb_interface_descriptor_s
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bInterfaceNumber;
    unsigned char bAlternateSetting;
    unsigned char bNumEndpoints;
    unsigned char bInterfaceClass;
    unsigned char bInterfaceSubClass;
    unsigned char bInterfaceProtocol;
    unsigned char iInterface;
}__attribute__((packed)) usb_interface_descriptor_t;

typedef struct usb_string_descriptor_s
{
    unsigned char bLength;
    unsigned char bDescriptorType;
}__attribute__((packed)) usb_string_descriptor_t;

/* USB power descriptor added to core specification */
#define USB_SUPPORT_D0_COMMAND      0x01
#define USB_SUPPORT_D1_COMMAND      0x02
#define USB_SUPPORT_D2_COMMAND      0x04
#define USB_SUPPORT_D3_COMMAND      0x08

#define USB_SUPPORT_D1_WAKEUP       0x10
#define USB_SUPPORT_D2_WAKEUP       0x20

typedef struct usb_power_descriptor_s
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bCapabilitiesFlags;
    unsigned short EventNotification;
    unsigned short D1LatencyTime;
    unsigned short D2LatencyTime;
    unsigned short D3LatencyTime;
    unsigned char PowerUnit;
    unsigned short D0PowerConsumption;
    unsigned short D1PowerConsumption;
    unsigned short D2PowerConsumption;
}__attribute__((packed)) usb_power_descriptor_t;

typedef struct usb_common_descriptor_s
{
    unsigned char bLength;
    unsigned char bDescriptorType;
}__attribute__((packed)) usb_common_descriptor_t;

#define USB_RECIPIENT            (unsigned char)0x1F
#define USB_RECIPIENT_DEVICE     (unsigned char)0x00
#define USB_RECIPIENT_INTERFACE  (unsigned char)0x01
#define USB_RECIPIENT_ENDPOINT   (unsigned char)0x02

#define USB_REQUEST_TYPE_MASK    (unsigned char)0x60
#define USB_STANDARD_REQUEST     (unsigned char)0x00
#define USB_CLASS_REQUEST        (unsigned char)0x20
#define USB_VENDOR_REQUEST       (unsigned char)0x40
#define USB_REQUEST_MASK         (unsigned char)0x0F
#define USB_DEVICE_ADDRESS_MASK  0x7F

#define MAX_ENDPOINTS            0x03

typedef struct device_request_s
{
    unsigned char bmRequestType;
    unsigned char bRequest;
    unsigned short wValue;
    unsigned short wIndex;
    unsigned short wLength;
}__attribute__((packed)) device_request_t;

#define USB_MANIFESTATION_TOLERANT (unsigned char)0x04
#define USB_CAN_UPLOAD             (unsigned char)0x02
#define USB_CAN_DOWNLOAD           (unsigned char)0x01

typedef struct usb_functional_descriptor_t
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bmAttributes;
    unsigned short wDetachTimeOut;
    unsigned short wTransferSize;
}__attribute__((packed)) usb_functional_descriptor_t;

#endif /* USB100_H */

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
#ifndef DESCRIPTORS_H
#define DESCRIPTORS_H

#include "usb100.h"

/* RUN_TIME descriptors */
extern const usb_device_descriptor_t device_descriptor;
extern const usb_configuration_descriptor_t configuration_descriptor;
extern const usb_interface_descriptor_t interface_descriptor;
extern const usb_endpoint_descriptor_t bulk_out_descriptor;
extern const usb_endpoint_descriptor_t bulk_in_descriptor;
/* DFU descriptors */
extern const usb_device_descriptor_t dfu_device_descriptor;
extern const usb_configuration_descriptor_t dfu_configuration_descriptor;
extern const usb_interface_descriptor_t dfu_interface_descriptor;
extern const usb_functional_descriptor_t dfu_functional_descriptor;

/* string descriptor indices */
#define LANGUAGE_SUPPORT_CODES_STRING 0x00
#define MANUFACTURER_STRING           0x01
#define PRODUCT_STRING                0x02
#define SERIAL_NUMBER_STRING          0x03 /* TODO: this must be at least 12 characters per block transfer spec */
#define CONFIGURATION_STRING          0x04
#define INTERFACE0_STRING             0x05
#define DFU_INTERFACE_STRING          0x06

extern const usb_string_descriptor_t * string_descriptor[7];

#endif /* DESCRIPTORS_H */

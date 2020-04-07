//****************************************************************************
//
// USB.C - USB driver for the Philips PDIUSBD12 USB interface device.
//
// Copyright (c) 1999,2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include "globals.h"
#include "../hwport.h"
#include "../hwdefs.h"
#include "time.h"
#include "usb.h"
#include "usbven.h"

//****************************************************************************
//
// Define a NOP instruction, which is compiler dependent (each compiler has
// its own way of doing inline assembly).
//
//****************************************************************************
#if defined(sdt25) || defined(ads)
#define NOP __asm { nop }
#elif defined(gcc)
#define NOP __asm__ ( "nop" )
#else
#error Unknown compiler!
#endif

//****************************************************************************
//
// This structure defines the setup packet received from the host via the
// control out endpoint.
//
//****************************************************************************
typedef struct
{
    unsigned char bmRequestType;
    unsigned char bRequest;
    unsigned short wValue;
    unsigned short wIndex;
    unsigned short wLength;
} ControlTransfer;

//****************************************************************************
//
// Function prototypes for the standard device request handling routines.
//
//****************************************************************************
static void USBGetStatus(void);
static void USBClearFeature(void);
static void USBSetFeature(void);
static void USBSetAddress(void);
static void USBGetDescriptor(void);
static void USBGetConfiguration(void);
static void USBSetConfiguration(void);
static void USBGetInterface(void);
static void USBSetInterface(void);
static void USBReserved(void);

//****************************************************************************
//
// This is the device descriptor for the Internet audio player.  See the USB
// specification for the definition of this descriptor.
//
//****************************************************************************
static const unsigned char ucDeviceDescriptor[] =
{
    0x12,                               // bLength
    0x01,                               // bDescriptorType
    0x00, 0x01,                         // bcdUSB
    0x00,                               // bDeviceClass
    0x00,                               // bDeviceSubClass
    0x00,                               // bDeviceProtocol
    0x10,                               // bMaxPacketSize0
    0x29, 0x04,                         // idVendor
    0x01, 0x10,                         // idProduct
    0x00, 0x01,                         // bcdDevice
    0x01,                               // iManufacturer
    0x02,                               // iProduct
    0x00,                               // iSerial Number
    0x01                                // bNumConfigurations
};

//****************************************************************************
//
// This is the configuration descriptor for the Internet audio player.  See
// the USB specification for the definition of this descriptor.
//
//****************************************************************************
static const unsigned char ucConfigurationDescriptor[] =
{
    //
    // The configuration descriptor structure.
    //
    0x09,                               // bLength
    0x02,                               // bDescriptorType
    0x20, 0x00,                         // wTotalLength
    0x01,                               // bNumInterfaces
    0x01,                               // bConfigurationValue
    0x03,                               // iConfiguration
    0xc0,                               // bmAttributes
    0x01,                               // MaxPower

    //
    // The interface descriptor structure.
    //
    0x09,                               // bLength
    0x04,                               // bDescriptorType
    0x00,                               // bInterfaceNumber
    0x00,                               // bAlternateSetting
    0x02,                               // bNumEndpoints
    0x00,                               // bInterfaceClass
    0x00,                               // bInterfaceSubClass
    0x00,                               // bInterfaceProtocol
    0x00,                               // iInterface

    //
    // The endpoint descriptor structure.
    //
    0x07,                               // bLength
    0x05,                               // bDescriptorType
    0x82,                               // bEndpointAddress
    0x02,                               // bmAttributes
    0x40, 0x00,                         // wMaxPacketSize
    0x00,                               // bInterval

    //
    // The endpoint descriptor structure.
    //
    0x07,                               // bLength
    0x05,                               // bDescriptorType
    0x02,                               // bEndpointAddress
    0x02,                               // bmAttributes
    0x40, 0x00,                         // wMaxPacketSize
    0x00                                // bInterval
};

//****************************************************************************
//
// String descriptor 0 for the Internet audio player.  This defines the
// languages supported by the string descriptors.  See the USB specification
// for the definition of this descriptor.
//
//****************************************************************************
static const unsigned char ucString0[] =
{
    0x04,                               // bLength
    0x03,                               // bDescriptorType
    0x09, 0x04                          // wLANGID[0] -> US English
};

//****************************************************************************
//
// String descriptor 1 for the Internet audio player.  This defines the
// manufacturer of the player.  See the USB specification for the definition
// of this descriptor.
//
//****************************************************************************
static const unsigned char ucString1[] =
{
    0x26,                               // bLength
    0x03,                               // bDescriptorType
    'C', 0x00,                          // wString[]
    'i', 0x00,
    'r', 0x00,
    'r', 0x00,
    'u', 0x00,
    's', 0x00,
    ' ', 0x00,
    'L', 0x00,
    'o', 0x00,
    'g', 0x00,
    'i', 0x00,
    'c', 0x00,
    ',', 0x00,
    ' ', 0x00,
    'I', 0x00,
    'n', 0x00,
    'c', 0x00,
    '.', 0x00
};

//****************************************************************************
//
// String descriptor 2 for the Internet audio player.  This defines the
// product description of the player.  See the USB specification for the
// definition of this descriptor.
//
//****************************************************************************
static const unsigned char ucString2[] =
{
    0x60,                               // bLength
    0x03,                               // bDescriptorType
    'C', 0x00,                          // wString[]
    'i', 0x00,
    'r', 0x00,
    'r', 0x00,
    'u', 0x00,
    's', 0x00,
    ' ', 0x00,
    'L', 0x00,
    'o', 0x00,
    'g', 0x00,
    'i', 0x00,
    'c', 0x00,
    ' ', 0x00,
    'M', 0x00,
    'a', 0x00,
    'v', 0x00,
    'e', 0x00,
    'r', 0x00,
    'i', 0x00,
    'c', 0x00,
    'k', 0x00,
    '(', 0x00,
    't', 0x00,
    'm', 0x00,
    ')', 0x00,
    ' ', 0x00,
    'I', 0x00,
    'n', 0x00,
    't', 0x00,
    'e', 0x00,
    'r', 0x00,
    'n', 0x00,
    'e', 0x00,
    't', 0x00,
    ' ', 0x00,
    'A', 0x00,
    'u', 0x00,
    'd', 0x00,
    'i', 0x00,
    'o', 0x00,
    ' ', 0x00,
    'P', 0x00,
    'l', 0x00,
    'a', 0x00,
    'y', 0x00,
    'e', 0x00,
    'r', 0x00
};

//****************************************************************************
//
// String descriptor 3 for the Internet audio player.  This defines the
// configuration description of the player.  See the USB specification for the
// definition of this descriptor.
//
//****************************************************************************
static const unsigned char ucString3[] =
{
    0x2c,                               // bLength
    0x03,                               // bDescriptorType
    'D', 0x00,                          // wString[]
    'e', 0x00,
    'f', 0x00,
    'a', 0x00,
    'u', 0x00,
    'l', 0x00,
    't', 0x00,
    ' ', 0x00,
    'C', 0x00,
    'o', 0x00,
    'n', 0x00,
    'f', 0x00,
    'i', 0x00,
    'g', 0x00,
    'u', 0x00,
    'r', 0x00,
    'a', 0x00,
    't', 0x00,
    'i', 0x00,
    'o', 0x00,
    'n', 0x00
};

//****************************************************************************
//
// An array of pointers to the USB standard device request handler Functions.
//
//****************************************************************************
static void (* const USBStandardDeviceRequest[])(void) =
{
    USBGetStatus,
    USBClearFeature,
    USBReserved,
    USBSetFeature,
    USBReserved,
    USBSetAddress,
    USBGetDescriptor,
    USBReserved,
    USBGetConfiguration,
    USBSetConfiguration,
    USBGetInterface,
    USBSetInterface,
    USBReserved
};

//****************************************************************************
//
// The following structure contains the persistent state of the USB interface.
//
//****************************************************************************
static struct
{
    //
    // Flags which describe the current state of the USB connection.
    //
    unsigned char ucFlags;

    //
    // Indicates if a vendor specific setup packet has been received on the
    // control out endpoint and is waiting to be processed.
    //
    unsigned char ucHaveVendorMessage;

    //
    // The number of bytes to be sent to the control endpoint.
    //
    unsigned short usControlInCount;

    //
    // The buffer of data that is being sent to the control endpoint.
    //
    const unsigned char *pucControlIn;

    //
    // The buffer of data that is being received from the control endpoint.
    //
    ControlTransfer sControlOut;

    //
    // The number of bytes to be sent to the bulk endpoint.
    //
    unsigned short usBulkInCount;

    //
    // The number of bytes still to be read from the bulk endpoint.
    //
    unsigned short usBulkOutCount;

    //
    // The buffer of data that is being sent to the bulk endpoint.
    //
    const unsigned char *pucBulkIn;

    //
    // The buffer of data that is being received from the bulk endpoint.
    //
    unsigned char *pucBulkOut;
} sUSB;

//****************************************************************************
//
// USBWriteCommand will write the specified value to the command register of
// the PDIUSBD12.
//
//****************************************************************************
static void
USBWriteCommand(unsigned char ucValue)
{
#ifdef HwPortABCD_USB_CS
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;
#endif
    volatile unsigned char *pucReg =
        (unsigned char *)(HwUSBAddress + HwUSBCommand);
    int iIdx;

    //
    // Assert the USB chip select if it is controllable.
    //
#ifdef HwPortABCD_USB_CS
    pulPtr[HwPortABCD >> 2] &= ~HwPortABCD_USB_CS;
#endif

    //
    // Write the value to the command register.
    //
    *pucReg = ucValue;

    //
    // Delay ~500ns to comply with the timing specification of the PDIUSBD12.
    //
    for(iIdx = 0; iIdx < 24; iIdx++)
    {
    }

    //
    // De-assert the USB chip select if it is controllable.
    //
#ifdef HwPortABCD_USB_CS
    pulPtr[HwPortABCD >> 2] |= HwPortABCD_USB_CS;
#endif
}

//****************************************************************************
//
// USBWriteData will write the specified value to the data register of the
// PDIUSBD12.
//
//****************************************************************************
static void
USBWriteData(unsigned char ucValue)
{
#ifdef HwPortABCD_USB_CS
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;
#endif
    volatile unsigned char *pucReg =
        (unsigned char *)(HwUSBAddress + HwUSBData);
    int iIdx;

    //
    // Assert the USB chip select if it is controllable.
    //
#ifdef HwPortABCD_USB_CS
    pulPtr[HwPortABCD >> 2] &= ~HwPortABCD_USB_CS;
#endif

    //
    // Write the value to the data register.
    //
    *pucReg = ucValue;

    //
    // Delay ~500ns to comply with the timing specification of the PDIUSBD12.
    //
    for(iIdx = 0; iIdx < 24; iIdx++)
    {
    }

    //
    // De-assert the USB chip select if it is controllable.
    //
#ifdef HwPortABCD_USB_CS
    pulPtr[HwPortABCD >> 2] |= HwPortABCD_USB_CS;
#endif
}

//****************************************************************************
//
// USBReadData will read a value from the data register of the PDIUSBD12.
//
//****************************************************************************
static unsigned char
USBReadData(void)
{
#ifdef HwPortABCD_USB_CS
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;
#endif
    volatile unsigned char *pucReg =
        (unsigned char *)(HwUSBAddress + HwUSBData);
    int iIdx;
    unsigned char ucRet;

    //
    // Assert the USB chip select if it is controllable.
    //
#ifdef HwPortABCD_USB_CS
    pulPtr[HwPortABCD >> 2] &= ~HwPortABCD_USB_CS;
#endif

    //
    // Write the value to the data register.
    //
    ucRet = *pucReg;

    //
    // Delay ~500ns to comply with the timing specification of the PDIUSBD12.
    //
    for(iIdx = 0; iIdx < 24; iIdx++)
    {
    }

    //
    // De-assert the USB chip select if it is controllable.
    //
#ifdef HwPortABCD_USB_CS
    pulPtr[HwPortABCD >> 2] |= HwPortABCD_USB_CS;
#endif

    //
    // Return the value read.
    //
    return(ucRet);
}

//****************************************************************************
//
// USBForceConfiguration forces the USB state into the specified
// configuration.
//
//****************************************************************************
void
USBForceConfiguration(unsigned long bConfigured)
{
    //
    // See if we should set the state to configured.
    //
    if(bConfigured)
    {
        //
        // Indicate that we are configured.
        //
        sUSB.ucFlags |= 1;
    }
    else
    {
        //
        // Indicate that we are not configured.
        //
        sUSB.ucFlags &= ~1;
    }
}

//****************************************************************************
//
// USBStallEndpoint stalls or un-stalls the specified endpoint.
//
//****************************************************************************
static void
USBStallEndpoint(unsigned long ulEndpoint, unsigned long bStall)
{
    //
    // Send the appropriate set endpoint status command to the PDIUSBD12.
    //
    USBWriteCommand(USB_COMMAND_SET_ENDPOINT_STATUS + ulEndpoint);
    USBWriteData(bStall ? 1 : 0);
}

//****************************************************************************
//
// USBReadEndpoint reads data from the specified endpoint.
//
//****************************************************************************
static unsigned long
USBReadEndpoint(unsigned long ulEndpoint, unsigned char **ppucData,
                unsigned short *pusLength)
{
#ifdef HwPortABCD_USB_CS
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;
#endif
    volatile unsigned char *pucReg =
        (unsigned char *)(HwUSBAddress + HwUSBData);
    unsigned long ulIdx, ulLength = 0;

    //
    // Select the appropriate endpoint.
    //
    USBWriteCommand(USB_COMMAND_SELECT_ENDPOINT + ulEndpoint);

    //
    // Is there buffer space to fill with this data or should we throw the
    // data away?
    //
    if(*pusLength)
    {
        //
        // Send the read buffer command.
        //
        USBWriteCommand(USB_COMMAND_READ_BUFFER);

        //
        // Throw away the reserved byte from the beginning of the buffer.
        //
        USBReadData();

        //
        // Read the length of the data buffer.
        //
        ulLength = USBReadData();

        //
        // Assert the USB chip select if it is controllable.
        //
#ifdef HwPortABCD_USB_CS
        pulPtr[HwPortABCD >> 2] &= ~HwPortABCD_USB_CS;
#endif

        //
        // Read the data into the receive buffer.
        //
        for(ulIdx = 0; (ulIdx < ulLength) && (ulIdx < *pusLength); ulIdx++)
        {
            //
            // Read the next byte from the data buffer.
            //
            *(*ppucData)++ = *pucReg;

            //
            // Delay ~500ns to comply with the timing specification of the
            // PDIUSBD12.
            //
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
            NOP;
        }

        //
        // De-assert the USB chip select if it is controllable.
        //
#ifdef HwPortABCD_USB_CS
        pulPtr[HwPortABCD >> 2] |= HwPortABCD_USB_CS;
#endif

        //
        // Decrement the count of bytes to read.
        //
        *pusLength -= ulIdx;
    }

    //
    // Send the clear buffer command so that the endpoint can receive another
    // packet.
    //
    USBWriteCommand(USB_COMMAND_CLEAR_BUFFER);

    //
    // Return the size of the packet received.
    //
    return(ulLength);
}

//****************************************************************************
//
// USBWriteEndpoint writes data to the specified endpoint.
//
//****************************************************************************
static void
USBWriteEndpoint(unsigned long ulEndpoint, const unsigned char **ppucData,
                 unsigned short *pusLength)
{
#ifdef HwPortABCD_USB_CS
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;
#endif
    volatile unsigned char *pucReg =
        (unsigned char *)(HwUSBAddress + HwUSBData);
    unsigned long ulIdx, ulLength;

    //
    // Determine the size of the packet to be sent based on the endpoint.
    //
    if(ulEndpoint == USB_ENDPOINT_CONTROL_IN)
    {
        //
        // The maximum packet size for the control endpoint is 16.
        //
        ulLength = (*pusLength > 16) ? 16 : *pusLength;
    }
    else
    {
        //
        // The maxmium packet size for the bulk endpoint is 64.
        //
        ulLength = (*pusLength > 64) ? 64 : *pusLength;
    }

    //
    // Select the appropriate endpoint.
    //
    USBWriteCommand(USB_COMMAND_SELECT_ENDPOINT + ulEndpoint);

    //
    // Send the write buffer command.
    //
    USBWriteCommand(USB_COMMAND_WRITE_BUFFER);

    //
    // Write the reserved byte to the buffer.
    //
    USBWriteData(0);

    //
    // Write the length of the data packet.
    //
    USBWriteData(ulLength);

    //
    // Assert the USB chip select if it is controllable.
    //
#ifdef HwPortABCD_USB_CS
    pulPtr[HwPortABCD >> 2] &= ~HwPortABCD_USB_CS;
#endif

    //
    // Write the data into the transmit buffer.
    //
    for(ulIdx = 0; ulIdx < ulLength; ulIdx++)
    {
        //
        // Write the next byte of the data packet.
        //
        *pucReg = *(*ppucData)++;

        //
        // Delay ~500ns to comply with the timing specification of the
        // PDIUSBD12.
        //
        NOP;
        NOP;
        NOP;
        NOP;
        NOP;
        NOP;
        NOP;
        NOP;
        NOP;
        NOP;
        NOP;
    }

    //
    // De-assert the USB chip select if it is controllable.
    //
#ifdef HwPortABCD_USB_CS
    pulPtr[HwPortABCD >> 2] |= HwPortABCD_USB_CS;
#endif

    //
    // Decrement the count of bytes to write.
    //
    *pusLength -= ulLength;

    //
    // Send the validate buffer command so that the endpoint will transmit the
    // packet.
    //
    USBWriteCommand(USB_COMMAND_VALIDATE_BUFFER);
}

//****************************************************************************
//
// USBEnable configures the PDIUSBD12 device.
//
//****************************************************************************
void
USBEnable(void)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;

    //
    // Enable the interrupts for the bulk endpoints.
    //
    USBWriteCommand(USB_COMMAND_SET_DMA);
    USBWriteData(USB_DMA_ENDP4_INT_ENABLE | USB_DMA_ENDP5_INT_ENABLE);

    //
    // Configure the PDIUSBD12 and enable the SoftConnect pull-up.
    //
    USBWriteCommand(USB_COMMAND_SET_MODE);
    USBWriteData(USB_CONFIG1_SOFT_CONNECT | USB_CONFIG1_NONISO_MODE);
    USBWriteData(USB_CONFIG2_SET_TO_ONE | USB_CONFIG2_CLOCK_4M);

    //
    // Enable the USB interrupt.
    //
    pulPtr[HwIntMask >> 2] |= HwIrqUSB;

    //
    // Indicate that the SoftConnect pull-up is active and that we are not
    // configured.
    //
    sUSB.ucFlags = 2;
}

//****************************************************************************
//
// USBDisable de-configures the PDIUSBD12 device.
//
//****************************************************************************
void
USBDisable(void)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;

    //
    // Disable the interrupts for the bulk endpoints.
    //
    USBWriteCommand(USB_COMMAND_SET_DMA);
    USBWriteData(0);

    //
    // Disable the SoftConnect pull-up.
    //
    USBWriteCommand(USB_COMMAND_SET_MODE);
    USBWriteData(USB_CONFIG1_NONISO_MODE);
    USBWriteData(USB_CONFIG2_SET_TO_ONE | USB_CONFIG2_CLOCK_4M);

    //
    // Disable the USB interrupt.
    //
    pulPtr[HwIntMask >> 2] &= ~HwIrqUSB;

    //
    // Indicate that the SoftConnect pull-up is no longer active.
    //
    sUSB.ucFlags &= ~2;
}

//****************************************************************************
//
// USBSendControl transmits a block of data back to the host via the control
// endpoint.
//
//****************************************************************************
static unsigned long
USBSendControl(const unsigned char *pucData, unsigned long ulLength)
{
    //
    // If a block is already being transmitted, then return a failure.
    //
    if(sUSB.usControlInCount)
    {
        return(0);
    }

    //
    // Prepare to transmit this block back to the host.
    //
    sUSB.pucControlIn = pucData;
    sUSB.usControlInCount = ulLength;

    //
    // Send the first packet of this block back to the host.
    //
    USBWriteEndpoint(USB_ENDPOINT_CONTROL_IN, &sUSB.pucControlIn,
                     &sUSB.usControlInCount);

    //
    // Success.
    //
    return(1);
}

//****************************************************************************
//
// USBSendBulk transmits a block of data back to the host via the bulk
// endpoint.
//
//****************************************************************************
static unsigned long
USBSendBulk(const unsigned char *pucData, unsigned long ulLength)
{
    //
    // If a block is already being transmitted, then return a failure.
    //
    if(sUSB.usBulkInCount)
    {
        return(0);
    }

    //
    // Prepare to transmit this block back to the host.
    //
    sUSB.pucBulkIn = pucData;
    sUSB.usBulkInCount = ulLength;

    //
    // Send the first packet of this block back to the host.
    //
    DisableIRQ();
    USBWriteEndpoint(USB_ENDPOINT_BULK_IN, &sUSB.pucBulkIn,
                     &sUSB.usBulkInCount);
    EnableIRQ();

    //
    // Success.
    //
    return(1);
}

//****************************************************************************
//
// USBReceiveBulk reads a block of data from the host via the bulk endpoint.
//
//****************************************************************************
static unsigned long
USBReceiveBulk(unsigned char *pucData, unsigned long ulLength)
{
    //
    // Prepare to read data from the host into this buffer.
    //
    DisableIRQ();
    sUSB.pucBulkOut = pucData;
    sUSB.usBulkOutCount = ulLength;
    EnableIRQ();

    //
    // Success.
    //
    return(1);
}

//****************************************************************************
//
// USBGetStatus implements the USB Get_Status device request.
//
//****************************************************************************
static void
USBGetStatus(void)
{
    unsigned char ucStatus[2];
    unsigned long ulEndpoint;

    //
    // Determine how to handle this request based on the recipient.
    //
    switch(sUSB.sControlOut.bmRequestType & USB_RT_RECIPIENT_MASK)
    {
        //
        // If the recipient is a device, return the state of the device's
        // remote wakeup and self powered states.
        //
        case USB_RT_RECIPIENT_DEVICE:
        {
            //
            // The player is self powered and does not support remote wakeup.
            //
            ucStatus[0] = USB_DEVICE_STATUS_SELF_POWERED;
            ucStatus[1] = 0;

            //
            // Send our response back to the host.
            //
            USBSendControl(ucStatus, 2);

            //
            // We're done handling this request.
            //
            break;
        }

        //
        // If the recipient is a device interface, return a value of
        // 0x00 as required by the USB spec.
        //
        case USB_RT_RECIPIENT_INTERFACE:
        {
            //
            // The USB spec. requires a GetStatus request for an interface
            // return a pair of zero bytes.
            //
            ucStatus[0] = 0;
            ucStatus[1] = 0;

            //
            // Send our response back to the host.
            //
            USBSendControl(ucStatus, 2);

            //
            // We're done handling this request.
            //
            break;
        }

        //
        // If the recipient is an endpoint, determine whether it is stalled or
        // not and return that information to the host.
        //
        case USB_RT_RECIPIENT_ENDPOINT:
        {
            //
            // Find out which endpoint is the recipient of the request.
            //
            ulEndpoint = sUSB.sControlOut.wIndex & USB_ENDPOINT_ADDRESS_MASK;

            //
            // Determine whether the IN or the OUT endpoint is being addressed
            // in the device request.
            //
            ulEndpoint *= 2;
            if(sUSB.sControlOut.wIndex & USB_ENDPOINT_DIRECTION_MASK)
            {
                ulEndpoint++;
            }

            //
            // Make sure that the specified endpoint is valid.
            //
            if((ulEndpoint != USB_ENDPOINT_CONTROL_OUT) &&
               (ulEndpoint != USB_ENDPOINT_CONTROL_IN) &&
               (ulEndpoint != USB_ENDPOINT_BULK_OUT) &&
               (ulEndpoint != USB_ENDPOINT_BULK_IN))
            {
                //
                // An invalid endpoint was specified, so stall both control
                // endpoints.
                //
                USBStallEndpoint(USB_ENDPOINT_CONTROL_OUT, 1);
                USBStallEndpoint(USB_ENDPOINT_CONTROL_IN, 1);

                //
                // There is nothing further to do.
                //
                return;
            }

            //
            // Read the endpoint status.
            //
            USBWriteCommand(USB_COMMAND_SELECT_ENDPOINT + ulEndpoint);
            ulEndpoint = USBReadData();

            //
            // Send the endpoint's status to the host.
            //
            if(ulEndpoint & USB_ENDPOINT_STALL)
            {
                ucStatus[0] = USB_ENDPOINT_STATUS_STALLED;
            }
            else
            {
                ucStatus[0] = 0;
            }
            ucStatus[1] = 0;

            //
            // Send our response back to the host.
            //
            USBSendControl(ucStatus, 2);

            //
            // We're done handling this request.
            //
            break;
        }

        //
        // If an invalid request is received, stall the control endpoint.
        //
        default:
        {
            //
            // Stall the both control endpoints.
            //
            USBStallEndpoint(USB_ENDPOINT_CONTROL_OUT, 1);
            USBStallEndpoint(USB_ENDPOINT_CONTROL_IN, 1);

            //
            // We're done handling this request.
            //
            break;
        }
    }
}

//****************************************************************************
//
// USBClearFeature implements the USB Clear_Feature device request.
//
//****************************************************************************
static void
USBClearFeature(void)
{
    unsigned long ulEndpoint;

    //
    // The only feature we support is stall on an endpoint.
    //
    if(((sUSB.sControlOut.bmRequestType & USB_RT_RECIPIENT_MASK) ==
        USB_RT_RECIPIENT_ENDPOINT) &&
       (sUSB.sControlOut.wValue == USB_FEATURE_ENDPOINT_STALL))
    {
        //
        // Compute the endpoint number.
        //
        ulEndpoint = (sUSB.sControlOut.wIndex & USB_ENDPOINT_ADDRESS_MASK) * 2;
        if(sUSB.sControlOut.wIndex & USB_ENDPOINT_DIRECTION_MASK)
        {
            ulEndpoint++;
        }

        //
        // Make sure that the specified endpoint is valid.
        //
        if((ulEndpoint != USB_ENDPOINT_CONTROL_OUT) &&
           (ulEndpoint != USB_ENDPOINT_CONTROL_IN) &&
           (ulEndpoint != USB_ENDPOINT_BULK_OUT) &&
           (ulEndpoint != USB_ENDPOINT_BULK_IN))
        {
            //
            // An invalid endpoint was specified, so stall both control
            // endpoints.
            //
            USBStallEndpoint(USB_ENDPOINT_CONTROL_OUT, 1);
            USBStallEndpoint(USB_ENDPOINT_CONTROL_IN, 1);

            //
            // There is nothing further to do.
            //
            return;
        }

        //
        // Clear the stall condition on the specified endpoint.
        //
        USBStallEndpoint(ulEndpoint, 0);
    }
    else
    {
        //
        // An unknown feature was specified, so stall both control endpoints.
        //
        USBStallEndpoint(USB_ENDPOINT_CONTROL_OUT, 1);
        USBStallEndpoint(USB_ENDPOINT_CONTROL_IN, 1);
    }
}

//****************************************************************************
//
// USBSetFeature implements the USB Set_Feature device request.
//
//****************************************************************************
static void
USBSetFeature(void)
{
    unsigned long ulEndpoint;

    //
    // The only feature we support is stall on an endpoint.
    //
    if(((sUSB.sControlOut.bmRequestType & USB_RT_RECIPIENT_MASK) ==
        USB_RT_RECIPIENT_ENDPOINT) &&
       (sUSB.sControlOut.wValue == USB_FEATURE_ENDPOINT_STALL))
    {
        //
        // Compute the endpoint number.
        //
        ulEndpoint = (sUSB.sControlOut.wIndex & USB_ENDPOINT_ADDRESS_MASK) * 2;
        if(sUSB.sControlOut.wIndex & USB_ENDPOINT_DIRECTION_MASK)
        {
            ulEndpoint++;
        }

        //
        // Make sure that the specified endpoint is valid.
        //
        if((ulEndpoint != USB_ENDPOINT_CONTROL_OUT) &&
           (ulEndpoint != USB_ENDPOINT_CONTROL_IN) &&
           (ulEndpoint != USB_ENDPOINT_BULK_OUT) &&
           (ulEndpoint != USB_ENDPOINT_BULK_IN))
        {
            //
            // An invalid endpoint was specified, so stall both control
            // endpoints.
            //
            USBStallEndpoint(USB_ENDPOINT_CONTROL_OUT, 1);
            USBStallEndpoint(USB_ENDPOINT_CONTROL_IN, 1);

            //
            // There is nothing further to do.
            //
            return;
        }

        //
        // Set the stall condition on the specified endpoint.
        //
        USBStallEndpoint(ulEndpoint, 1);
    }
    else
    {
        //
        // An unknown feature was specified, so stall both control endpoints.
        //
        USBStallEndpoint(USB_ENDPOINT_CONTROL_OUT, 1);
        USBStallEndpoint(USB_ENDPOINT_CONTROL_IN, 1);
    }
}

//****************************************************************************
//
// USBSetAddress implements the USB Set_Address device request.
//
//****************************************************************************
static void
USBSetAddress(void)
{
    //
    // Configure our UBS controller for the USB address assigned by the host.
    //
    USBWriteCommand(USB_COMMAND_SET_ADDRESS_ENABLE);
    USBWriteData(sUSB.sControlOut.wValue | 0x80);

    //
    // Respond to the host with an empty packet.
    //
    USBSendControl(0, 0);
}

//****************************************************************************
//
// USBGetDescriptor implements the USB Get_Descriptor device request.
//
//****************************************************************************
static void
USBGetDescriptor(void)
{
    const unsigned char *pucDescriptor = 0;
    unsigned long ulLength = 0;

    //
    // Determine how to handle this request based on the requested descriptor.
    //
    switch(sUSB.sControlOut.wValue & USB_DESCRIPTOR_TYPE_MASK)
    {
        //
        // The device descriptor was requested.
        //
        case USB_DESCRIPTOR_DEVICE:
        {
            //
            // Prepare to return the device descriptor.
            //
            pucDescriptor = ucDeviceDescriptor;
            ulLength = sizeof(ucDeviceDescriptor);

            //
            // We're done handling this request.
            //
            break;
        }

        //
        // The configuration descriptor was requested.
        //
        case USB_DESCRIPTOR_CONFIGURATION:
        {
            //
            // Prepare to return the configuration descriptor.
            //
            pucDescriptor = ucConfigurationDescriptor;
            ulLength = sizeof(ucConfigurationDescriptor);

            //
            // We're done handling this request.
            //
            break;
        }

        //
        // A string descriptor was requested.
        //
        case USB_DESCRIPTOR_STRING:
        {
            //
            // Make sure that the language and string index requested are
            // valid.
            //
            if(((sUSB.sControlOut.wValue & USB_DESCRIPTOR_INDEX_MASK) != 0) &&
               ((sUSB.sControlOut.wIndex != 0x0409) ||
                ((sUSB.sControlOut.wValue & USB_DESCRIPTOR_INDEX_MASK) > 3)))
            {
                //
                // Stall both of the control endpoints.
                //
                USBStallEndpoint(USB_ENDPOINT_CONTROL_OUT, 1);
                USBStallEndpoint(USB_ENDPOINT_CONTROL_IN, 1);

                //
                // We're done handling this request.
                //
                return;
            }

            //
            // Prepare to return the requested string.
            //
            switch(sUSB.sControlOut.wValue & USB_DESCRIPTOR_INDEX_MASK)
            {
                //
                // String index 0 is the language ID string.
                //
                case 0x00:
                {
                    pucDescriptor = ucString0;
                    ulLength = sizeof(ucString0);
                    break;
                }

                //
                // String index 1 is the manufacturer name.
                //
                case 0x01:
                {
                    pucDescriptor = ucString1;
                    ulLength = sizeof(ucString1);
                    break;
                }

                //
                // String index 2 is the product name.
                //
                case 0x02:
                {
                    pucDescriptor = ucString2;
                    ulLength = sizeof(ucString2);
                    break;
                }

                //
                // String index 3 is the configuration name.
                //
                case 0x03:
                {
                    pucDescriptor = ucString3;
                    ulLength = sizeof(ucString3);
                    break;
                }
            }

            //
            // We're done handling this request.
            //
            break;
        }

        //
        // An invalid request is received, so stall both control endpoints.
        //
        default:
        {
            //
            // Stall both of the control endpoints.
            //
            USBStallEndpoint(USB_ENDPOINT_CONTROL_OUT, 1);
            USBStallEndpoint(USB_ENDPOINT_CONTROL_IN, 1);

            //
            // We're done handling this request.
            //
            return;
        }
    }

    //
    // If the requested length is less than the length of the descriptor to be
    // returned, then simply return the requested portion of the descriptor.
    //
    if(sUSB.sControlOut.wLength < ulLength)
    {
        ulLength = sUSB.sControlOut.wLength;
    }

    //
    // Send the descriptor back to the host.
    //
    USBSendControl(pucDescriptor, ulLength);
}

//****************************************************************************
//
// USBGetConfiguration implements the USB Get_Configuration device request.
//
//****************************************************************************
static void
USBGetConfiguration(void)
{
    unsigned char ucConfiguration = sUSB.ucFlags & 1;

    //
    // Send the current configuration value back to the host.
    //
    USBSendControl(&ucConfiguration, 1);
}

//****************************************************************************
//
// USBSetConfiguration implements the USB Set_Configuration device request.
//
//****************************************************************************
static void
USBSetConfiguration(void)
{
    //
    // If the requested configuration is zero, then go into the unconfigured
    // state.
    //
    if(sUSB.sControlOut.wValue == 0)
    {
        //
        // Clear the global configuration flag.
        //
        sUSB.ucFlags &= ~1;

        //
        // Disable the generic endpoints.
        //
        USBWriteCommand(USB_COMMAND_SET_ENDPOINT_ENABLE);
        USBWriteData(0);

        //
        // Respond to the host with an empty packet.
        //
        USBSendControl(0, 0);
    }

    //
    // If the requested configuration is one, then go into the configured
    // state.
    //
    else if(sUSB.sControlOut.wValue == 1)
    {
        //
        // Set the global configuration flag.
        //
        sUSB.ucFlags |= 1;

        //
        // Disable the generic endpoints.
        //
        USBWriteCommand(USB_COMMAND_SET_ENDPOINT_ENABLE);
        USBWriteData(0);

        //
        // Enable the generic endpoints.
        //
        USBWriteCommand(USB_COMMAND_SET_ENDPOINT_ENABLE);
        USBWriteData(1);

        //
        // Respond to the host with an empty packet.
        //
        USBSendControl(0, 0);
    }

    //
    // If the requested configuration is anything else, then stall both of the
    // control endpoints.
    //
    else
    {
        USBStallEndpoint(USB_ENDPOINT_CONTROL_OUT, 1);
        USBStallEndpoint(USB_ENDPOINT_CONTROL_IN, 1);
    }
}

//****************************************************************************
//
// USBGetInterface implements the USB Get_Interface device request.
//
//****************************************************************************
static void
USBGetInterface(void)
{
    unsigned char ucInterface = 0;

    //
    // We only support a single interface, so the current interface is always
    // the first one.  Send our response back to the host.
    //
    USBSendControl(&ucInterface, 1);
}

//****************************************************************************
//
// USBSetInterface implements the USB Set_Interface device request.
//
//****************************************************************************
static void
USBSetInterface(void)
{
    //
    // We only support a single interface.
    //
    if((sUSB.sControlOut.wValue == 0) && (sUSB.sControlOut.wIndex == 0))
    {
        //
        // The first interface was requested, so do nothing.
        //
        return;
    }
    else
    {
        //
        // A non-existent interface was requested, so stall both control
        // endpoints.
        //
        USBStallEndpoint(USB_ENDPOINT_CONTROL_OUT, 1);
        USBStallEndpoint(USB_ENDPOINT_CONTROL_IN, 1);
    }
}

//****************************************************************************
//
// USBReserved handles device requests which are not supported by this USB
// device implementation.
//
//****************************************************************************
static void
USBReserved(void)
{
    //
    // Stall both control endpoints.
    //
    USBStallEndpoint(USB_ENDPOINT_CONTROL_OUT, 1);
    USBStallEndpoint(USB_ENDPOINT_CONTROL_IN, 1);
}

//****************************************************************************
//
// USBVendor handles vendor specific device requests.
//
//****************************************************************************
static void
USBVendor(void)
{
    //
    // Make sure that a valid vendor request was received.
    //
    if((sUSB.sControlOut.bRequest & 31) >= USB_Vendor_LastCommand)
    {
        //
        // This is an unsupported vendor request.
        //
        USBReserved();
    }

    //
    // Handle all other requests.
    //
    else
    {
        //
        // If this is a connect request, then we should start the download
        // process.
        //
        if(sUSB.sControlOut.bRequest == USB_Vendor_Connect)
        {
            //
            // Indicate that a download has been requested.
            //
            ulSystemFlags |= SYSFLAG_START_DOWNLOAD;
        }

        //
        // We have a vendor message which we must handled outside the interrupt
        // handler.
        //
        sUSB.ucHaveVendorMessage = 1;
    }
}

//****************************************************************************
//
// USBISR is the interrupt handler routine for the USB.
//
//****************************************************************************
void
USBISR(void)
{
    unsigned long ulIntStatus, ulTransactionStatus;
    unsigned short usLength;
    unsigned char *pucChar;

    //
    // Read the PDIUSBD12 interrupt register.
    //
    USBWriteCommand(USB_COMMAND_READ_INTERRUPT);
    ulIntStatus = USBReadData();
    ulIntStatus |= USBReadData() << 8;

    //
    // Do nothing if there was a bus reset.
    //
    if(ulIntStatus & USB_INT1_BUS_RESET)
    {
        return;
    }

    //
    // Handle an interrupt on the bulk out endpoint.
    //
    if(ulIntStatus & USB_INT1_ENDPOINT2_OUT)
    {
        //
        // Read the status of the last transaction on the bulk out endpoint.
        //
        USBWriteCommand(USB_COMMAND_READ_LAST_XACTION_STATUS +
                        USB_ENDPOINT_BULK_OUT);
        USBReadData();

        //
        // Loop while there is data in the bulk out buffers.
        //
        while(USBWriteCommand(USB_COMMAND_GET_ENDPOINT_STATUS +
                              USB_ENDPOINT_BULK_OUT),
              (USBReadData() & (USB_ENDPOINT_BUFFER0_FULL |
                                USB_ENDPOINT_BUFFER1_FULL)))
        {
            //
            // Read the next data packet.
            //
            USBReadEndpoint(USB_ENDPOINT_BULK_OUT, &sUSB.pucBulkOut,
                            &sUSB.usBulkOutCount);
        }
    }

    //
    // Handle an interrupt on the control out endpoint.
    //
    if(ulIntStatus & USB_INT1_CONTROL_OUT)
    {
        //
        // Read the status of the last transaction on the control out endpoint.
        //
        USBWriteCommand(USB_COMMAND_READ_LAST_XACTION_STATUS +
                        USB_ENDPOINT_CONTROL_OUT);
        ulTransactionStatus = USBReadData();

        //
        // Was a setup packet received?
        //
        if(ulTransactionStatus & USB_XACTION_STATUS_SETUP_PACKET)
        {
            //
            // Read the packet.
            //
            pucChar = (unsigned char *)&sUSB.sControlOut;
            usLength = sizeof(ControlTransfer);
            if(USBReadEndpoint(USB_ENDPOINT_CONTROL_OUT, &pucChar,
                               &usLength) != sizeof(ControlTransfer))
            {
                //
                // The size of the setup packet is incorrect, so stall both of
                // the control endpoints.
                //
                USBStallEndpoint(USB_ENDPOINT_CONTROL_OUT, 1);
                USBStallEndpoint(USB_ENDPOINT_CONTROL_IN, 1);
            }
            else
            {
                //
                // Acknowledge both the control in and control out endpoints,
                // and send a clear buffer command to the control out endpoint.
                //
                USBWriteCommand(USB_COMMAND_SELECT_ENDPOINT +
                                USB_ENDPOINT_CONTROL_OUT);
                USBWriteCommand(USB_COMMAND_ACKNOWLEDGE_ENDPOINT);
                USBWriteCommand(USB_COMMAND_CLEAR_BUFFER);
                USBWriteCommand(USB_COMMAND_SELECT_ENDPOINT +
                                USB_ENDPOINT_CONTROL_IN);
                USBWriteCommand(USB_COMMAND_ACKNOWLEDGE_ENDPOINT);

                //
                // Process the command in the setup packet.
                //
                if(((sUSB.sControlOut.bmRequestType & USB_RT_TYPE_MASK) ==
                    USB_RT_TYPE_STANDARD) && (sUSB.sControlOut.bRequest < 16))
                {
                    //
                    // This is a standard request, so call the appropriate
                    // routine.
                    //
                    (*USBStandardDeviceRequest[sUSB.sControlOut.bRequest])();
                }
                else if((sUSB.sControlOut.bmRequestType & USB_RT_TYPE_MASK) ==
                        USB_RT_TYPE_VENDOR)
                {
                    //
                    // This is a vendor specific request.
                    //
                    USBVendor();
                }
                else
                {
                    //
                    // All other requests are treated as reserved requests.
                    //
                    USBReserved();
                }
            }
        }
        else
        {
            //
            // The packet was not a setup packet, so ignore it.  Send a clear
            // buffer command to the control out endpoint so it will receive
            // new packets.
            //
            USBWriteCommand(USB_COMMAND_SELECT_ENDPOINT +
                            USB_ENDPOINT_CONTROL_OUT);
            USBWriteCommand(USB_COMMAND_CLEAR_BUFFER);

            //
            // Acknowledge both the control in and control out endpoints.
            //
            USBWriteCommand(USB_COMMAND_ACKNOWLEDGE_ENDPOINT);
            USBWriteCommand(USB_COMMAND_SELECT_ENDPOINT +
                            USB_ENDPOINT_CONTROL_IN);
            USBWriteCommand(USB_COMMAND_ACKNOWLEDGE_ENDPOINT);
        }
    }

    //
    // Handle an interrupt on the control in endpoint.
    //
    if(ulIntStatus & USB_INT1_CONTROL_IN)
    {
        //
        // Read the status of the last transaction on the control in endpoint.
        //
        USBWriteCommand(USB_COMMAND_READ_LAST_XACTION_STATUS +
                        USB_ENDPOINT_CONTROL_IN);
        USBReadData();

        //
        // Was this the last block of data to be sent back to the host?
        //
        if(sUSB.usControlInCount != 0)
        {
            //
            // Send the next packet of data to the host.
            //
            USBWriteEndpoint(USB_ENDPOINT_CONTROL_IN, &sUSB.pucControlIn,
                             &sUSB.usControlInCount);
        }
        else
        {
            //
            // There is no more data to send, so send an empty packet.
            //
            usLength = 0;
            USBWriteEndpoint(USB_ENDPOINT_CONTROL_IN, 0, &usLength);
        }
    }

    //
    // Handle an interrupt on the bulk in endpoint.
    //
    if(ulIntStatus & USB_INT1_ENDPOINT2_IN)
    {
        //
        // Read the status of the last transaction on the bulk in endpoint.
        //
        USBWriteCommand(USB_COMMAND_READ_LAST_XACTION_STATUS +
                        USB_ENDPOINT_BULK_IN);
        USBReadData();

        //
        // Was this the last block of data to be sent back to the host?
        //
        if(sUSB.usBulkInCount != 0)
        {
            //
            // Send the next packet of data to the host.
            //
            USBWriteEndpoint(USB_ENDPOINT_BULK_IN, &sUSB.pucBulkIn,
                             &sUSB.usBulkInCount);
        }
    }
}

//****************************************************************************
//
// USBCheck determines if the USB cable has been connected or removed and
// configures the USB controller as appropriate.
//
//****************************************************************************
#ifdef HwRegUSBCable
void
USBCheck(void)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;
    unsigned long bConnected;

    //
    // Determine if the USB cable is connected or removed.
    //
    bConnected =
        ((pulPtr[HwRegUSBCable >> 2] & HwBitUSBCable) == HwUSBCableConnected);

    //
    // If the cable is connected and we have not configured the USB controller,
    // then do so now.
    //
    if(bConnected && !(sUSB.ucFlags & 2))
    {
        //
        // Enable the USB controller.
        //
        USBEnable();
    }

    //
    // Otherwise, if the cable is not connected and we have not unconfigured
    // the USB controller, then do so now.
    //
    else if(!bConnected && (sUSB.ucFlags & 2))
    {
        //
        // Disable the USB controller.
        //
        USBDisable();
    }
}
#endif

//****************************************************************************
//
// USBDownload communicates with the host via the USB to update the content
// stored in the Internet audio player (upload content, download content,
// remove content, etc.).
//
//****************************************************************************
void
USBDownload(unsigned char *pucData)
{
    unsigned char ucFlags = 0;
    unsigned long ulResult[2];
    tFile sFile;
    tDir sDir;

    //
    // Use the first 512 bytes of the scratch space for the write buffer needed
    // by the file system.
    //
    FSSetWriteScratch(pucData);
    pucData += 512;

    //
    // Setup a buffer to receive data from the bulk endpoint.
    //
    USBReceiveBulk(pucData, 16384);

    //
    // Loop forever.  This loop will be explicitly broken out of when the host
    // "disconnects" from the player (i.e. sends the USB_Vendor_Disconnect
    // message).
    //
    while(1)
    {
        //
        // Wait until there is something to do.
        //
        while(!sUSB.ucHaveVendorMessage)
        {
            //
            // Put the EP7209 into HALT mode.
            //
            Halt();
        }

        //
        // The USB_Vendor_Disconnect message is sent when the host application
        // is no longer using the Internet audio player.  In this case, we
        // should exit from the loop.
        //
        if(sUSB.sControlOut.bRequest == USB_Vendor_Disconnect)
        {
            //
            // Respond to the host with an empty packet.
            //
            USBSendControl((unsigned char *)&ulResult, 4);

            //
            // We've handled the vendor specific message.
            //
            sUSB.ucHaveVendorMessage = 0;

            //
            // Break out of the loop.
            //
            break;
        }

        //
        // Handle the vendor specific message.
        //
        switch(sUSB.sControlOut.bRequest & 31)
        {
            //
            // A connect request.
            //
            case USB_Vendor_Connect:
            {
                //
                // Respond to the host with an empty packet.
                //
                USBSendControl((unsigned char *)&ulResult, 4);

                //
                // We're done handling this request.
                //
                break;
            }

            //
            // Return the value of one of our descriptors.
            //
            case USB_Vendor_GetDescriptor:
            {
                //
                // Figure out which descriptor was requested.
                //
                switch(sUSB.sControlOut.wIndex)
                {
                    //
                    // The unique device ID was requested.
                    //
                    case USB_Vendor_Descriptor_DeviceID:
                    {
                        //
                        // We do not support a unique device ID, so return a
                        // failure.
                        //
                        ulResult[0] = 0;
                        ulResult[1] = 0;

                        //
                        // We're done handling this descriptor.
                        //
                        break;
                    }

                    //
                    // The unique media ID was requested.
                    //
                    case USB_Vendor_Descriptor_MediaID:
                    {
                        //
                        // Get the unique media ID of the requested drive.
                        //
                        ulResult[1] = 128 * 8;
                        ulResult[0] =
                            FSGetMediaID(sUSB.sControlOut.bRequest >> 5,
                                         pucData, &ulResult[1]);
                        if(!ulResult[0])
                        {
                            ulResult[1] = 0;
                        }
                        else
                        {
                            ulResult[1] = (ulResult[1] + 7) / 8;
                        }

                        //
                        // We're done handling this descriptor.
                        //
                        break;
                    }

                    //
                    // The name the media was requested.
                    //
                    case USB_Vendor_Descriptor_MediaName:
                    {
                        //
                        // Get the name of the requested drive.
                        //
                        ulResult[0] =
                            FSGetMediaName(sUSB.sControlOut.bRequest >> 5,
                                           (unsigned short *)pucData,
                                           &ulResult[1]);
                        if(!ulResult[0])
                        {
                            ulResult[1] = 0;
                        }

                        //
                        // We're done handling this descriptor.
                        //
                        break;
                    }

                    //
                    // The version of the firmware was requested.
                    //
                    case USB_Vendor_Descriptor_FirmwareVersion:
                    {
                        //
                        // Write the firmware version into the scratch buffer.
                        // This is firmware version '01.00.00.00'.
                        //
                        pucData[0] = 0x01;
                        pucData[1] = 0x00;
                        pucData[2] = 0x00;
                        pucData[3] = 0x00;

                        //
                        // Set the return code to success.
                        //
                        ulResult[0] = 1;

                        //
                        // Set the length of the descriptor to four.
                        //
                        ulResult[1] = 4;

                        //
                        // We're done handling this descriptor.
                        //
                        break;
                    }

                    //
                    // The ID and version of the hardware was requested.
                    //
                    case USB_Vendor_Descriptor_HardwareID:
                    {
                        //
                        // Write the hardware ID into the scratch buffer.
                        //
                        pucData[0] = HardwareID & 0xff;
                        pucData[1] = (HardwareID >> 8) & 0xff;
                        pucData[2] = (HardwareID >> 16) & 0xff;
                        pucData[3] = (HardwareID >> 24) & 0xff;

                        //
                        // Set the return code to success.
                        //
                        ulResult[0] = 1;

                        //
                        // Set the length of the descriptor to four.
                        //
                        ulResult[1] = 4;

                        //
                        // We're done handling this descriptor.
                        //
                        break;
                    }

                    //
                    // One of the group IDs was requested.
                    //
                    case USB_Vendor_Descriptor_GroupID:
                    {
                        //
                        // We do not support group IDs, so return a failure.
                        //
                        ulResult[0] = 0;
                        ulResult[1] = 0;

                        //
                        // We're done handling this descriptor.
                        //
                        break;
                    }

                    //
                    // The list of supported codecs was requested.
                    //
                    case USB_Vendor_Descriptor_CodecList:
                    {
                        //
                        // We do not support the codec list, so return a
                        // failure.
                        //
                        ulResult[0] = 0;
                        ulResult[1] = 0;

                        //
                        // We're done handling this descriptor.
                        //
                        break;
                    }

                    //
                    // The player's notion of the current time.
                    //
                    case USB_Vendor_Descriptor_CurrentTime:
                    {
                        unsigned long ulTime;

                        //
                        // Get the current system time.
                        //
                        ulTime = TimeGetSystemTime();

                        //
                        // Write the current time into the scratch buffer.
                        //
                        pucData[0] = ulTime & 0xff;
                        pucData[1] = (ulTime >> 8) & 0xff;
                        pucData[2] = (ulTime >> 16) & 0xff;
                        pucData[3] = (ulTime >> 24) & 0xff;

                        //
                        // Set the return code to success.
                        //
                        ulResult[0] = 1;

                        //
                        // Set the length of the descriptor to four.
                        //
                        ulResult[1] = 4;

                        //
                        // We're done handling this descriptor.
                        //
                        break;
                    }

                    //
                    // The current remaining battery life.
                    //
                    case USB_Vendor_Descriptor_BatteryLife:
                    {
                        //
                        // For now, set the battery life to 100%.
                        //
                        pucData[0] = 100;
                        pucData[1] = 0;
                        pucData[2] = 0;
                        pucData[3] = 0;

                        //
                        // Set the return code to success.
                        //
                        ulResult[0] = 1;

                        //
                        // Set the length of the descriptor to four.
                        //
                        ulResult[1] = 4;

                        //
                        // We're done handling this descriptor.
                        //
                        break;
                    }

                    //
                    // This is an unknown descriptor.
                    //
                    default:
                    {
                        //
                        // Return a failure.
                        //
                        ulResult[0] = 0;
                        ulResult[1] = 0;

                        //
                        // We're done handling this descriptor.
                        //
                        break;
                    }
                }

                //
                // If there is descriptor data to be returned, then queue it.
                //
                if(ulResult[0] && ulResult[1])
                {
                    USBSendBulk(pucData, ulResult[1]);
                }

                //
                // Respond to the host with the data packet.
                //
                USBSendControl((unsigned char *)&ulResult, 8);

                //
                // We're done handling this request.
                //
                break;
            }

            //
            // Set the value of one of our descriptors.
            //
            case USB_Vendor_SetDescriptor:
            {
                //
                // Figure out which descriptor was requested.
                //
                switch(sUSB.sControlOut.wIndex)
                {
                    //
                    // One of the group IDs was requested.
                    //
                    case USB_Vendor_Descriptor_GroupID:
                    {
                        //
                        // We do not support group IDs, so return a failure.
                        //
                        ulResult[0] = 0;

                        //
                        // We're done handling this descriptor.
                        //
                        break;
                    }

                    //
                    // Set the current system time.
                    //
                    case USB_Vendor_Descriptor_CurrentTime:
                    {
                        unsigned long ulTime;

                        //
                        // Get the time from the scratch buffer.
                        //
                        ulTime = pucData[0] | (pucData[1] << 8) |
                                 (pucData[2] << 16) | (pucData[3] << 24);

                        //
                        // Set the system time.
                        //
                        TimeSetSystemTime(ulTime);

                        //
                        // Set the result to success.
                        //
                        ulResult[0] = 1;

                        //
                        // We're done handling this descriptor.
                        //
                        break;
                    }

                    //
                    // We can not change any of the other descriptors.
                    //
                    default:
                    {
                        //
                        // Return a failure.
                        //
                        ulResult[0] = 0;

                        //
                        // We're done handling this descriptor.
                        //
                        break;
                    }
                }

                //
                // Re-queue the bulk out data buffer.
                //
                USBReceiveBulk(pucData, 16384);

                //
                // Respond to the host with the data packet.
                //
                USBSendControl((unsigned char *)&ulResult, 4);

                //
                // We're done handling this request.
                //
                break;
            }

            //
            // The number of drives in the system.
            //
            case USB_Vendor_NumDrives:
            {
                //
                // The return code is success.
                //
                ulResult[0] = 1;

                //
                // Return the number of drives.
                //
                ulResult[1] = FSNumDrives();

                //
                // Respond to the host with the data packet.
                //
                USBSendControl((unsigned char *)&ulResult, 8);

                //
                // We're done handling this request.
                //
                break;
            }

            //
            // Open a file.
            //
            // XXXXXX We need to be able to fail this request if the file is
            //        marked as no readback by the DRM flags.
            //
            case USB_Vendor_Open:
            {
                //
                // Make sure there is not already a file open.
                //
                if(ucFlags & 1)
                {
                    //
                    // The return code is failure since there is already a
                    // file opened.
                    //
                    ulResult[0] = 0;
                }

                //
                // There is not a file already opened, so attempt to open this
                // file.
                //
                else
                {
                    //
                    // Open the file.
                    //
                    ulResult[0] = FSOpen(&sFile,
                                         sUSB.sControlOut.bRequest >> 5,
                                         (const unsigned short *)pucData,
                                         sUSB.sControlOut.wValue &
                                         ~FS_OPEN_DECRYPT);

                    //
                    // If the file opened successfully, then remember that
                    // there is a file opened.
                    //
                    if(ulResult[0])
                    {
                        ucFlags |= 1;
                    }
                }

                //
                // Re-queue the bulk out data buffer.
                //
                USBReceiveBulk(pucData, 16384);

                //
                // Respond to the host with the data packet.
                //
                USBSendControl((unsigned char *)&ulResult, 4);

                //
                // We're done handling this request.
                //
                break;
            }

            //
            // Create a file.
            //
            case USB_Vendor_Create:
            {
                //
                // Make sure there is not already a file open.
                //
                if(ucFlags & 1)
                {
                    //
                    // The return code is failure since there is already a
                    // file opened.
                    //
                    ulResult[0] = 0;
                }

                //
                // There is not a file already opened, so attempt to create
                // this file.
                //
                else
                {
                    //
                    // Create the file.
                    //
                    ulResult[0] = FSCreate(&sFile,
                                           sUSB.sControlOut.bRequest >> 5,
                                           (const unsigned short *)pucData,
                                           sUSB.sControlOut.wValue |
                                           (sUSB.sControlOut.wIndex << 16));

                    //
                    // If the file created successfully, then remember that
                    // there is a file opened.
                    //
                    if(ulResult[0])
                    {
                        ucFlags |= 1;
                    }
                }

                //
                // Re-queue the bulk out data buffer.
                //
                USBReceiveBulk(pucData, 16384);

                //
                // Respond to the host with the data packet.
                //
                USBSendControl((unsigned char *)&ulResult, 4);

                //
                // We're done handling this request.
                //
                break;
            }

            //
            // Read from the currently opened file.
            //
            case USB_Vendor_Read:
            {
                //
                // Make sure there is a file opened.
                //
                if(!(ucFlags & 1))
                {
                    //
                    // The return code is failure since there is not a file
                    // opened.
                    //
                    ulResult[0] = 0;
                }

                //
                // There is a file opened, so read data from the file.
                //
                else
                {
                    //
                    // Read from the file.
                    //
                    ulResult[0] = FSRead(&sFile, pucData,
                                         sUSB.sControlOut.wValue |
                                         (sUSB.sControlOut.wIndex << 16));

                    //
                    // Queue the data to be sent back to the host.
                    //
                    if(ulResult[0])
                    {
                        USBSendBulk(pucData, ulResult[0]);
                    }
                }

                //
                // Respond to the host with the data packet.
                //
                USBSendControl((unsigned char *)&ulResult, 4);

                //
                // We're done handling this request.
                //
                break;
            }

            //
            // Write to the currently opened file.
            //
            case USB_Vendor_Write:
            {
                //
                // Make sure there is a file opened.
                //
                if(!(ucFlags & 1))
                {
                    //
                    // The return code is failure since there is not a file
                    // opened.
                    //
                    ulResult[0] = 0;
                }

                //
                // There is a file opened, so write data to the file.
                //
                else
                {
                    //
                    // Write to the file.
                    //
                    ulResult[0] = FSWrite(&sFile, pucData,
                                          sUSB.sControlOut.wValue |
                                          (sUSB.sControlOut.wIndex << 16));
                }

                //
                // Re-queue the bulk out data buffer.
                //
                USBReceiveBulk(pucData, 16384);

                //
                // Respond to the host with the data packet.
                //
                USBSendControl((unsigned char *)&ulResult, 4);

                //
                // We're done handling this request.
                //
                break;
            }

            //
            // Seek within the currently opened file.
            //
            case USB_Vendor_Seek:
            {
                //
                // Make sure there is a file opened.
                //
                if(!(ucFlags & 1))
                {
                    //
                    // The return code is failure since there is not a file
                    // opened.
                    //
                    ulResult[0] = 0;
                }

                //
                // There is a file opened, so seek within the file.
                //
                else
                {
                    //
                    // Seek within the file.
                    //
                    ulResult[0] = FSSeek(&sFile,
                                         sUSB.sControlOut.wValue |
                                         (sUSB.sControlOut.wIndex << 16));
                }

                //
                // Respond to the host with the data packet.
                //
                USBSendControl((unsigned char *)&ulResult, 4);

                //
                // We're done handling this request.
                //
                break;
            }

            //
            // Return the current position within the currently opened file.
            //
            case USB_Vendor_Tell:
            {
                //
                // Make sure there is a file opened.
                //
                if(!(ucFlags & 1))
                {
                    //
                    // The return code is failure since there is not a file
                    // opened.
                    //
                    ulResult[0] = 0;
                }

                //
                // There is a file opened, so get the current file position
                //
                else
                {
                    //
                    // The return code is success.
                    //
                    ulResult[0] = 1;

                    //
                    // Get the current file position.
                    //
                    ulResult[1] = FSTell(&sFile);
                }

                //
                // Respond to the host with the data packet.
                //
                USBSendControl((unsigned char *)&ulResult, 8);

                //
                // We're done handling this request.
                //
                break;
            }

            //
            // Get the length of the currently opened file.
            //
            case USB_Vendor_Length:
            {
                //
                // Make sure there is a file opened.
                //
                if(!(ucFlags & 1))
                {
                    //
                    // The return code is failure since there is not a file
                    // opened.
                    //
                    ulResult[0] = 0;
                }

                //
                // There is a file opened, so get the file length.
                //
                else
                {
                    //
                    // The return code is success.
                    //
                    ulResult[0] = 1;

                    //
                    // Get the file length.
                    //
                    ulResult[1] = FSLength(&sFile);
                }

                //
                // Respond to the host with the data packet.
                //
                USBSendControl((unsigned char *)&ulResult, 8);

                //
                // We're done handling this request.
                //
                break;
            }

            //
            // Get the modification date/time of the currently opened file.
            //
            case USB_Vendor_GetDate:
            {
                //
                // Make sure there is a file opened.
                //
                if(!(ucFlags & 1))
                {
                    //
                    // The return code is failure since there is not a file
                    // opened.
                    //
                    ulResult[0] = 0;
                }

                //
                // There is a file opened, so get the modification date/time.
                //
                else
                {
                    //
                    // The return code is success.
                    //
                    ulResult[0] = 1;

                    //
                    // Get the modification date/time.
                    //
                    ulResult[1] = FSGetDate(&sFile);
                }

                //
                // Respond to the host with the data packet.
                //
                USBSendControl((unsigned char *)&ulResult, 8);

                //
                // We're done handling this request.
                //
                break;
            }

            //
            // Close the currently opened file.
            //
            case USB_Vendor_Close:
            {
                //
                // Make sure there is a file opened.
                //
                if(!(ucFlags & 1))
                {
                    //
                    // The return code is failure since there is not a file
                    // opened.
                    //
                    ulResult[0] = 0;
                }

                //
                // There is a file opened, so close the file.
                //
                else
                {
                    //
                    // Close the file.
                    //
                    ulResult[0] = FSClose(&sFile);

                    //
                    // If the file closed successfully, then remember that
                    // there is not a file opened.
                    //
                    if(ulResult[0])
                    {
                        ucFlags &= ~1;
                    }
                }

                //
                // Respond to the host with the data packet.
                //
                USBSendControl((unsigned char *)&ulResult, 4);

                //
                // We're done handling this request.
                //
                break;
            }

            //
            // Delete a file.
            //
            case USB_Vendor_Delete:
            {
                //
                // Delete the file.
                //
                ulResult[0] = FSDelete(sUSB.sControlOut.bRequest >> 5,
                                       (const unsigned short *)pucData);

                //
                // Re-queue the bulk out data buffer.
                //
                USBReceiveBulk(pucData, 16384);

                //
                // Respond to the host with the data packet.
                //
                USBSendControl((unsigned char *)&ulResult, 4);

                //
                // We're done handling this request.
                //
                break;
            }

            //
            // Open a directory.
            //
            case USB_Vendor_OpenDir:
            {
                //
                // Make sure there is not already a directory open.
                //
                if(ucFlags & 2)
                {
                    //
                    // The return code is failure since there is already a
                    // directory opened.
                    //
                    ulResult[0] = 0;
                }

                //
                // There is not a directory already opened, so attempt to open
                // this directory.
                //
                else
                {
                    //
                    // Open the directory.
                    //
                    ulResult[0] = FSOpenDir(&sDir,
                                            sUSB.sControlOut.bRequest >> 5,
                                            (const unsigned short *)pucData);

                    //
                    // If the directory opened successfully, then remember that
                    // there is a directory opened.
                    //
                    if(ulResult[0])
                    {
                        ucFlags |= 2;
                    }
                }

                //
                // Re-queue the bulk out data buffer.
                //
                USBReceiveBulk(pucData, 16384);

                //
                // Respond to the host with the data packet.
                //
                USBSendControl((unsigned char *)&ulResult, 4);

                //
                // We're done handling this request.
                //
                break;
            }

            //
            // Read from the currently opened directory.
            //
            case USB_Vendor_ReadDir:
            {
                //
                // Make sure there is a directory opened.
                //
                if(!(ucFlags & 2))
                {
                    //
                    // The return code is failure since there is not a
                    // directory opened.
                    //
                    ulResult[0] = 0;
                }

                //
                // There is a directory opened, so read the next entry.
                //
                else
                {
                    //
                    // Read the next entry from the directory.
                    //
                    ulResult[0] = FSReadDir(&sDir, (unsigned short *)pucData,
                                            sUSB.sControlOut.wValue);

                    //
                    // Queue the data to be send back to the host.
                    //
                    if(ulResult[0])
                    {
                        USBSendBulk(pucData, 256);
                    }
                }

                //
                // Respond to the host with the data packet.
                //
                USBSendControl((unsigned char *)&ulResult, 4);

                //
                // We're done handling this request.
                //
                break;
            }

            //
            // Close the currently opened directory.
            //
            case USB_Vendor_CloseDir:
            {
                //
                // Make sure there is a directory opened.
                //
                if(!(ucFlags & 2))
                {
                    //
                    // The return code is failure since there is not a
                    // directory opened.
                    //
                    ulResult[0] = 0;
                }

                //
                // There is a directory opened, so close the directory.
                //
                else
                {
                    //
                    // Close the directory.
                    //
                    ulResult[0] = FSCloseDir(&sDir);

                    //
                    // If the directory closed successfully, then remember that
                    // there is not a directory opened.
                    //
                    if(ulResult[0])
                    {
                        ucFlags &= ~2;
                    }
                }

                //
                // Respond to the host with the data packet.
                //
                USBSendControl((unsigned char *)&ulResult, 4);

                //
                // We're done handling this request.
                //
                break;
            }

            //
            // Create a directory.
            //
            case USB_Vendor_MakeDir:
            {
                //
                // Create the directory.
                //
                ulResult[0] = FSMakeDir(sUSB.sControlOut.bRequest >> 5,
                                        (const unsigned short *)pucData);

                //
                // Re-queue the bulk out data buffer.
                //
                USBReceiveBulk(pucData, 16384);

                //
                // Respond to the host with the data packet.
                //
                USBSendControl((unsigned char *)&ulResult, 4);

                //
                // We're done handling this request.
                //
                break;
            }

            //
            // Remove a directory.
            //
            case USB_Vendor_RemoveDir:
            {
                //
                // Remove the directory.
                //
                ulResult[0] = FSRemoveDir(sUSB.sControlOut.bRequest >> 5,
                                          (const unsigned short *)pucData);

                //
                // Re-queue the bulk out data buffer.
                //
                USBReceiveBulk(pucData, 16384);

                //
                // Respond to the host with the data packet.
                //
                USBSendControl((unsigned char *)&ulResult, 4);

                //
                // We're done handling this request.
                //
                break;
            }

            //
            // Get the total capacity of a drive.
            //
            case USB_Vendor_TotalSpace:
            {
                //
                // The return code is success.
                //
                ulResult[0] = 1;

                //
                // Get the total capacity of this drive.
                //
                ulResult[1] = FSTotalSpace(sUSB.sControlOut.bRequest >> 5);

                //
                // Respond to the host with the data packet.
                //
                USBSendControl((unsigned char *)&ulResult, 8);

                //
                // We're done handling this request.
                //
                break;
            }

            //
            // Get the free space on a drive.
            //
            case USB_Vendor_FreeSpace:
            {
                //
                // The return code is success.
                //
                ulResult[0] = 1;

                //
                // Get the free space on this drive.
                //
                ulResult[1] = FSFreeSpace(sUSB.sControlOut.bRequest >> 5);

                //
                // Respond to the host with the data packet.
                //
                USBSendControl((unsigned char *)&ulResult, 8);

                //
                // We're done handling this request.
                //
                break;
            }

            //
            // Format a drive.
            //
            case USB_Vendor_Format:
            {
                //
                // Format this drive.
                //
                ulResult[0] = FSFormat(sUSB.sControlOut.bRequest >> 5);

                //
                // Respond to the host with the data packet.
                //
                USBSendControl((unsigned char *)&ulResult, 4);

                //
                // We're done handling this request.
                //
                break;
            }

            //
            // Start a software update.
            //
            case USB_Vendor_StartUpdate:
            {
                //
                // See if we support updating the software.
                //
                if(SupportUpdate())
                {
                    //
                    // We support software updates, so return success.
                    //
                    ulResult[0] = 1;
                }
                else
                {
                    //
                    // We do not support software updates, so return failure.
                    //
                    ulResult[0] = 0;
                }

                //
                // Respond to the host with the data packet.
                //
                USBSendControl((unsigned char *)&ulResult, 4);

                //
                // If we returned success (i.e. we support update), then branch
                // to the update code now.
                //
                if(ulResult[0] == 1)
                {
                    StartUpdate();
                }

                //
                // We're done handling this request.
                //
                break;
            }

            //
            // An unknown request.
            //
            default:
            {
                //
                // We do not know how to handle this request.
                //
                USBReserved();

                //
                // We're done handling this request.
                //
                break;
            }
        }

        //
        // Indicate that we've handled the vendor specific message.
        //
        sUSB.ucHaveVendorMessage = 0;
    }

    //
    // Clear the bulk send and receive buffers.
    //
    DisableIRQ();
    sUSB.pucBulkIn = 0;
    sUSB.usBulkInCount = 0;
    EnableIRQ();
    USBReceiveBulk(0, 0);

    //
    // Reset the file system's write buffer.
    //
    FSSetWriteScratch(0);

    //
    // We've completed the download.
    //
    DisableIRQ();
    ulSystemFlags &= ~SYSFLAG_START_DOWNLOAD;
    EnableIRQ();
}

//****************************************************************************
//
// USB.H - Defines for the USB specification and the Philips PDIUSBD12 USB
//         controller.
//
// Copyright (c) 1999,2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************

//****************************************************************************
//
// The following defines are specific the USB standard version 1.1.
//
//****************************************************************************

//
// Definitions of the bit fields in the bmRequestType field of a setup packet.
//
#define USB_RT_DEVICE_TO_HOST                   0x80
#define USB_RT_TYPE_MASK                        0x60
#define USB_RT_TYPE_STANDARD                    0x00
#define USB_RT_TYPE_CLASS                       0x20
#define USB_RT_TYPE_VENDOR                      0x40
#define USB_RT_RECIPIENT_MASK                   0x1F
#define USB_RT_RECIPIENT_DEVICE                 0x00
#define USB_RT_RECIPIENT_INTERFACE              0x01
#define USB_RT_RECIPIENT_ENDPOINT               0x02

//
// Definitions of the bit fields in the wIndex field of setup packets where the
// wIndex field is used to specify a endpoint (i.e. Clear_Feature, Get_Status,
// and Set_Feature).
//
#define USB_ENDPOINT_DIRECTION_MASK             0x0080
#define USB_ENDPOINT_ADDRESS_MASK               0x000F

//
// Definitions of the features that can be specified in the wValue field of a
// Clear_Feature or Set_Feature setup packet.
//
#define USB_FEATURE_ENDPOINT_STALL              0x0000
#define USB_FEATURE_REMOTE_WAKEUP               0x0001
#define USB_FEATURE_POWER_D0                    0x0002
#define USB_FEATURE_POWER_D1                    0x0003
#define USB_FEATURE_POWER_D2                    0x0004
#define USB_FEATURE_POWER_D3                    0x0005

//
// Definitions of the wValue field for a Get_Descriptor setup packet.
//
#define USB_DESCRIPTOR_TYPE_MASK                0xFF00
#define USB_DESCRIPTOR_DEVICE                   0x0100
#define USB_DESCRIPTOR_CONFIGURATION            0x0200
#define USB_DESCRIPTOR_STRING                   0x0300
#define USB_DESCRIPTOR_INTERFACE                0x0400
#define USB_DESCRIPTOR_ENDPOINT                 0x0500
#define USB_DESCRIPTOR_INDEX_MASK               0x00FF

//
// Definitions of the device status returned for a Get_Status setup packet.
//
#define USB_DEVICE_STATUS_SELF_POWERED          0x01
#define USB_DEVICE_STATUS_REMOTE_WAKEUP         0x02

//
// Definitions of the endpoint status returned for a Get_Status setup packet.
//
#define USB_ENDPOINT_STATUS_STALLED             0x01

//****************************************************************************
//
// The following defines are specific the the Philips PDIUSBD12 USB
// controller.
//
//****************************************************************************

//
// The offsets to the individual registers in the PDIUSBD12 USB controller.
//
#define HwUSBData                               0x00000000
#define HwUSBCommand                            0x00000001

//
// Definitions of the supported endpoints.
//
#define USB_ENDPOINT_CONTROL_OUT                0x00
#define USB_ENDPOINT_CONTROL_IN                 0x01
#define USB_ENDPOINT_BULK_OUT                   0x04
#define USB_ENDPOINT_BULK_IN                    0x05

//
// Definitions of the commands that can be sent to the PDIUSBD12.
//
#define USB_COMMAND_SELECT_ENDPOINT             0x00
#define USB_COMMAND_READ_LAST_XACTION_STATUS    0x40
#define USB_COMMAND_SET_ENDPOINT_STATUS         0x40
#define USB_COMMAND_GET_ENDPOINT_STATUS         0x80
#define USB_COMMAND_SET_ADDRESS_ENABLE          0xD0
#define USB_COMMAND_SET_ENDPOINT_ENABLE         0xD8
#define USB_COMMAND_READ_BUFFER                 0xF0
#define USB_COMMAND_WRITE_BUFFER                0xF0
#define USB_COMMAND_ACKNOWLEDGE_ENDPOINT        0xF1
#define USB_COMMAND_CLEAR_BUFFER                0xF2
#define USB_COMMAND_SET_MODE                    0xF3
#define USB_COMMAND_READ_INTERRUPT              0xF4
#define USB_COMMAND_VALIDATE_BUFFER             0xFA
#define USB_COMMAND_SET_DMA                     0xFB

//
// Definitions of the bit fields in the first byte of the configuration
// register.
//
#define USB_CONFIG1_NO_LAZY_CLOCK               0x02
#define USB_CONFIG1_CLOCK_RUNNING               0x04
#define USB_CONFIG1_INTERRUPT_MODE              0x08
#define USB_CONFIG1_SOFT_CONNECT                0x10
#define USB_CONFIG1_NONISO_MODE                 0x00
#define USB_CONFIG1_ISOOUT_MODE                 0x40
#define USB_CONFIG1_ISOIN_MODE                  0x80
#define USB_CONFIG1_ISOIO_MODE                  0xC0

//
// Definitions of the bit fields in the second byte of the configuration
// register.
//
#define USB_CONFIG2_CLOCK_12M                   0x03
#define USB_CONFIG2_CLOCK_4M                    0x0B
#define USB_CONFIG2_SET_TO_ONE                  0x40
#define USB_CONFIG2_SOF_ONLY                    0x80

//
// Definitions of the bit fields in the DMA configuration register.
//
#define USB_DMA_SINGLE                          0x00
#define USB_DMA_BURST_4                         0x01
#define USB_DMA_BURST_8                         0x02
#define USB_DMA_BURST_16                        0x03
#define USB_DMA_ENABLE                          0x04
#define USB_DMA_DIRECTION                       0x08
#define USB_DMA_AUTO_RELOAD                     0x10
#define USB_DMA_INTPINMODE                      0x20
#define USB_DMA_ENDP4_INT_ENABLE                0x40
#define USB_DMA_ENDP5_INT_ENABLE                0x80

//
// Definitions of the bit fields in the first byte of the interrupt register.
//
#define USB_INT1_CONTROL_OUT                    0x01
#define USB_INT1_CONTROL_IN                     0x02
#define USB_INT1_ENDPOINT1_OUT                  0x04
#define USB_INT1_ENDPOINT1_IN                   0x08
#define USB_INT1_ENDPOINT2_OUT                  0x10
#define USB_INT1_ENDPOINT2_IN                   0x20
#define USB_INT1_BUS_RESET                      0x40
#define USB_INT1_SUSPEND_CHANGE                 0x80

//
// Definitions of the bit fields in the second byte of the interrupt register.
//
#define USB_INT2_EOT                            0x01

//
// Definitions of the bit fields in the last transaction status register.
//
#define USB_XACTION_STATUS_DATA_RX_TX_SUCCESS   0x01
#define USB_XACTION_STATUS_ERROR_CODE_MASK      0x1E
#define USB_XACTION_STATUS_SETUP_PACKET         0x20
#define USB_XACTION_STATUS_DATA1_PACKET         0x40
#define USB_XACTION_STATUS_PREVIOUS_NOT_READ    0x80

//
// Definitions of the bit fields in the endpoint status register.
//
#define USB_ENDPOINT_STATUS_FULL_EMPTY          0x01
#define USB_ENDPOINT_STALL                      0x02
#define USB_ENDPOINT_BUFFER0_FULL               0x20
#define USB_ENDPOINT_BUFFER1_FULL               0x60

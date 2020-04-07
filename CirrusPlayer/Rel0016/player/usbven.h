//****************************************************************************
//
// USBVEN.H - Defines for the USB vendor specific commands that can be sent to
//            the Internet Audio Player.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
//
//****************************************************************************

//****************************************************************************
//
// The following are the vendor specific commands (bRequest field of the setup
// packet) that can be sent to the Internet Audio Player.
//
//****************************************************************************
enum
{
    USB_Vendor_Connect,
    USB_Vendor_Disconnect,
    USB_Vendor_GetDescriptor,
    USB_Vendor_SetDescriptor,
    USB_Vendor_NumDrives,
    USB_Vendor_Open,
    USB_Vendor_Create,
    USB_Vendor_Read,
    USB_Vendor_Write,
    USB_Vendor_Seek,
    USB_Vendor_Tell,
    USB_Vendor_Length,
    USB_Vendor_GetDate,
    USB_Vendor_Close,
    USB_Vendor_Delete,
    USB_Vendor_OpenDir,
    USB_Vendor_ReadDir,
    USB_Vendor_CloseDir,
    USB_Vendor_MakeDir,
    USB_Vendor_RemoveDir,
    USB_Vendor_TotalSpace,
    USB_Vendor_FreeSpace,
    USB_Vendor_Format,
    USB_Vendor_StartUpdate,
    USB_Vendor_LastCommand
};

//****************************************************************************
//
// The following are the descriptors that can be retrieved and set via the
// Get/SetDescriptor vendor specific commands.
//
//****************************************************************************
enum
{
    USB_Vendor_Descriptor_DeviceID,
    USB_Vendor_Descriptor_MediaID,
    USB_Vendor_Descriptor_MediaName,
    USB_Vendor_Descriptor_FirmwareVersion,
    USB_Vendor_Descriptor_HardwareID,
    USB_Vendor_Descriptor_GroupID,
    USB_Vendor_Descriptor_CodecList,
    USB_Vendor_Descriptor_CurrentTime,
    USB_Vendor_Descriptor_BatteryLife
};

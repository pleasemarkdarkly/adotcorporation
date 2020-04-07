//****************************************************************************
//
// MAVUSB.H - Definitions for the IOCTL interface to the Maverick(tm) USB
//            driver.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
//
//****************************************************************************

#ifndef MAVUSB_H_INC
#define MAVUSB_H_INC

//****************************************************************************
//
// The IOCTLs which can be sent to the driver.
//
//****************************************************************************

//
// Returns the USB configuration descriptor for the device.
//
#define IOCTL_MAVUSB_GET_CONFIG_DESCRIPTOR  CTL_CODE(FILE_DEVICE_UNKNOWN,    \
                                                     0x800,                  \
                                                     METHOD_BUFFERED,        \
                                                     FILE_ANY_ACCESS)

//
// Resets the device.
//
#define IOCTL_MAVUSB_RESET_DEVICE           CTL_CODE(FILE_DEVICE_UNKNOWN,    \
                                                     0x801,                  \
                                                     METHOD_BUFFERED,        \
                                                     FILE_ANY_ACCESS)

//
// Resets the pipe.
//
#define IOCTL_MAVUSB_RESET_PIPE             CTL_CODE(FILE_DEVICE_UNKNOWN,    \
                                                     0x802,                  \
                                                     METHOD_BUFFERED,        \
                                                     FILE_ANY_ACCESS)

//
// Sends a vendor command to the device.
//
#define IOCTL_MAVUSB_DEVICE_REQUEST         CTL_CODE(FILE_DEVICE_UNKNOWN,    \
                                                     0x803,                  \
                                                     METHOD_BUFFERED,        \
                                                     FILE_READ_DATA |        \
                                                     FILE_WRITE_DATA)

//
// Resets the count of bytes transferred.
//
#define IOCTL_MAVUSB_RESET_TRANSFER_COUNT   CTL_CODE(FILE_DEVICE_UNKNOWN,    \
                                                     0x804,                  \
                                                     METHOD_BUFFERED,        \
                                                     FILE_READ_DATA |        \
                                                     FILE_WRITE_DATA)

//
// Get the current count of bytes transferred.
//
#define IOCTL_MAVUSB_GET_TRANSFER_COUNT     CTL_CODE(FILE_DEVICE_UNKNOWN,    \
                                                     0x805,                  \
                                                     METHOD_BUFFERED,        \
                                                     FILE_READ_DATA |        \
                                                     FILE_WRITE_DATA)

//
// Get the name of the device.
//
#define IOCTL_MAVUSB_GET_NAME               CTL_CODE(FILE_DEVICE_UNKNOWN,    \
                                                     0x806,                  \
                                                     METHOD_BUFFERED,        \
                                                     FILE_READ_DATA |        \
                                                     FILE_WRITE_DATA)

//
// Get the name of the device manufacturer.
//
#define IOCTL_MAVUSB_GET_MANUFACTURER_NAME  CTL_CODE(FILE_DEVICE_UNKNOWN,    \
                                                     0x807,                  \
                                                     METHOD_BUFFERED,        \
                                                     FILE_READ_DATA |        \
                                                     FILE_WRITE_DATA)

//****************************************************************************
//
// The requests which can be sent via IOCTL_MAVUSB_DEVICE_REQUEST.
//
//****************************************************************************
enum
{
    REQUEST_GET_DESCRIPTOR,
    REQUEST_SET_DESCRIPTOR,
    REQUEST_NUM_DRIVES,
    REQUEST_OPEN,
    REQUEST_CREATE,
    REQUEST_SEEK,
    REQUEST_TELL,
    REQUEST_LENGTH,
    REQUEST_GETDATE,
    REQUEST_CLOSE,
    REQUEST_DELETE,
    REQUEST_OPENDIR,
    REQUEST_READDIR,
    REQUEST_CLOSEDIR,
    REQUEST_MAKEDIR,
    REQUEST_REMOVEDIR,
    REQUEST_TOTAL_SPACE,
    REQUEST_FREE_SPACE,
    REQUEST_FORMAT,
    REQUEST_START_UPDATE
};

//****************************************************************************
//
// The parameters for REQUEST_GET_DESCRIPTOR.
//
//****************************************************************************
typedef struct
{
    //
    // The descriptor of which to get the value.
    //
    unsigned long ulDescriptor;

    //
    // The index for descriptors which have multiple indexed values.
    //
    unsigned long ulIndex;
} MavUsb_GetDescriptorParams;

//****************************************************************************
//
// The parameters for REQUEST_SET_DESCRIPTOR.
//
//****************************************************************************
typedef struct
{
    //
    // The descriptor of which to set the value.
    //
    unsigned long ulDescriptor;

    //
    // The index for descriptors which have multiple indexed values.
    //
    unsigned long ulIndex;

    //
    // The new value of the descriptor.
    //
    char pcValue[256];

    //
    // The length of the descriptor value.
    //
    unsigned long ulLength;
} MavUsb_SetDescriptorParams;

//****************************************************************************
//
// The parameters for REQUEST_OPEN.
//
//****************************************************************************
typedef struct
{
    //
    // The drive number on which to open the file.
    //
    unsigned long ulDriveNum;

    //
    // The full path name of the file to open.
    //
    unsigned short pusFileName[128];

    //
    // The flags for opening the file.
    //
    unsigned long ulFlags;
} MavUsb_OpenParams;

//****************************************************************************
//
// The parameters for REQUEST_CREATE.
//
//****************************************************************************
typedef struct
{
    //
    // The drive number on which to create the file.
    //
    unsigned long ulDriveNum;

    //
    // The full path name of the file to create.
    //
    unsigned short pusFileName[128];

    //
    // The length of the file to be created.
    //
    unsigned long ulLength;
} MavUsb_CreateParams;

//****************************************************************************
//
// The parameters for REQUEST_SEEK.
//
//****************************************************************************
typedef struct
{
    //
    // The offset in the file to which the read/write pointer will be moved.
    //
    unsigned long ulPosition;
} MavUsb_SeekParams;

//****************************************************************************
//
// The parameters for REQUEST_DELETE.
//
//****************************************************************************
typedef struct
{
    //
    // The drive number from which the file will be deleted.
    //
    unsigned long ulDriveNum;

    //
    // The full path name of the file to be deleted.
    //
    unsigned short pusFileName[128];
} MavUsb_DeleteParams;

//****************************************************************************
//
// The parameters for REQUEST_OPENDIR.
//
//****************************************************************************
typedef struct
{
    //
    // The drive number on which to open the directory.
    //
    unsigned long ulDriveNum;

    //
    // The full path name of the directory to open.
    //
    unsigned short pusDirName[128];
} MavUsb_OpenDirParams;

//****************************************************************************
//
// The parameters for REQUEST_READDIR.
//
//****************************************************************************
typedef struct
{
    //
    // The type of entry to return.
    //
    unsigned long ulType;
} MavUsb_ReadDirParams;

//****************************************************************************
//
// The parameters for REQUESET_MAKEDIR.
//
//****************************************************************************
typedef struct
{
    //
    // The drive number on which to create the directory.
    //
    unsigned long ulDriveNum;

    //
    // The full path name of the directory to create.
    //
    unsigned short pusDirName[128];
} MavUsb_MakeDirParams;

//****************************************************************************
//
// The parameters for REQUEST_REMOVEDIR.
//
//****************************************************************************
typedef struct
{
    //
    // The drive number from which to remove the directory.
    //
    unsigned long ulDriveNum;

    //
    // The full path name of the directory to remove.
    //
    unsigned short pusDirName[128];
} MavUsb_RemoveDirParams;

//****************************************************************************
//
// The parameters for REQUEST_TOTAL_SPACE.
//
//****************************************************************************
typedef struct
{
    //
    // The drive number of which to determine the total space.
    //
    unsigned long ulDriveNum;
} MavUsb_TotalSpaceParams;

//****************************************************************************
//
// The parameters for REQUEST_FREE_SPACE.
//
//****************************************************************************
typedef struct
{
    //
    // The drive number of which to determine the free space.
    //
    unsigned long ulDriveNum;
} MavUsb_FreeSpaceParams;

//****************************************************************************
//
// The parameters for REQUEST_FORMAT.
//
//****************************************************************************
typedef struct
{
    //
    // The drive number to format.
    //
    unsigned long ulDriveNum;
} MavUsb_FormatParams;

//****************************************************************************
//
// The parameters for IOCTL_MAVUSB_DEVICE_REQUEST.
//
//****************************************************************************
typedef struct
{
    //
    // The device request.
    //
    unsigned long ulRequestCode;

    //
    // The parameters for the device request.  The structure in the union which
    // contains the parameters is based on ulRequestCode.
    //
    union
    {
        MavUsb_GetDescriptorParams sGetDescriptor;
        MavUsb_SetDescriptorParams sSetDescriptor;
        MavUsb_OpenParams sOpen;
        MavUsb_CreateParams sCreate;
        MavUsb_SeekParams sSeek;
        MavUsb_DeleteParams sDelete;
        MavUsb_OpenDirParams sOpenDir;
        MavUsb_ReadDirParams sReadDir;
        MavUsb_MakeDirParams sMakeDir;
        MavUsb_RemoveDirParams sRemoveDir;
        MavUsb_TotalSpaceParams sTotalSpace;
        MavUsb_FreeSpaceParams sFreeSpace;
        MavUsb_FormatParams sFormat;
    } uParams;
} MavUsb_IoctlParams;

//****************************************************************************
//
// The results from REQUEST_GET_DESCRIPTOR.
//
//****************************************************************************
typedef struct
{
    //
    // The value of the descriptor.
    //
    char pcValue[256];

    //
    // The length of the descriptor value.
    //
    unsigned long ulLength;
} MavUsb_GetDescriptorResult;

//****************************************************************************
//
// The results from REQUEST_NUM_DRIVES.
//
//****************************************************************************
typedef struct
{
    //
    // The number of drives in the device.
    //
    unsigned long ulNumDrives;
} MavUsb_NumDrivesResult;

//****************************************************************************
//
// The results from REQUEST_TELL.
//
//****************************************************************************
typedef struct
{
    //
    // The current offset into the file of the read/write pointer.
    //
    unsigned long ulPosition;
} MavUsb_TellResult;

//****************************************************************************
//
// The results from REQUEST_LENGTH.
//
//****************************************************************************
typedef struct
{
    //
    // The length of the file.
    //
    unsigned long ulLength;
} MavUsb_LengthResult;

//****************************************************************************
//
// The results from REQUEST_GETDATE.
//
//****************************************************************************
typedef struct
{
    //
    // The modification date/time of the file.
    //
    unsigned long ulDate;
} MavUsb_GetDateResult;

//****************************************************************************
//
// The results from REQEUST_READDIR.
//
//****************************************************************************
typedef struct
{
    //
    // The name of the next file/directory found.
    //
    unsigned short pusName[128];
} MavUsb_ReadDirResult;

//****************************************************************************
//
// The results from REQUEST_TOTAL_SPACE.
//
//****************************************************************************
typedef struct
{
    //
    // The number of bytes of storage on the drive.
    //
    unsigned long ulBytes;
} MavUsb_TotalSpaceResult;

//****************************************************************************
//
// The results from REQUEST_FREE_SPACE.
//
//****************************************************************************
typedef struct
{
    //
    // The number of byte of free space on the drive.
    //
    unsigned long ulBytes;
} MavUsb_FreeSpaceResult;

//****************************************************************************
//
// The results from the device request.
//
//****************************************************************************
typedef struct
{
    //
    // The return code from the device request.
    //
    unsigned long ulReturnCode;

    //
    // The results from the device request.  The structure in the union which
    // contains the results is based on the ulRequestCode supplied in
    // MavUsb_IoctlParams.
    //
    union
    {
        MavUsb_GetDescriptorResult sGetDescriptor;
        MavUsb_NumDrivesResult sNumDrives;
        MavUsb_TellResult sTell;
        MavUsb_LengthResult sLength;
        MavUsb_GetDateResult sGetDate;
        MavUsb_ReadDirResult sReadDir;
        MavUsb_TotalSpaceResult sTotalSpace;
        MavUsb_FreeSpaceResult sFreeSpace;
    } uResults;
} MavUsb_IoctlResults;

#endif // MAVUSB_H_INC

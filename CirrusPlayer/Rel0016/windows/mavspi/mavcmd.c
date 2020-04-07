//****************************************************************************
//
// MAVCMD.C - Functions to communicate with the Maverick(tm) USB driver.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include <windows.h>
#include <winioctl.h>
#include <setupapi.h>
#include "../usbdrvr/mavguid.h"
#include "../usbdrvr/mavusb.h"
#include "mavcmd.h"

//****************************************************************************
//
// The handle to the file object used to communicate with the Maverick(tm) USB
// driver.
//
//****************************************************************************
static HANDLE hFile = INVALID_HANDLE_VALUE;
static HANDLE hStatusFile = INVALID_HANDLE_VALUE;

//****************************************************************************
//
// Maverick_OpenDevice opens the Maverick Digital Audio Player.
//
//****************************************************************************
unsigned long
Maverick_OpenDevice(void)
{
    HDEVINFO hardwareDeviceInfo;
    SP_INTERFACE_DEVICE_DATA deviceInfoData;
    PSP_INTERFACE_DEVICE_DETAIL_DATA functionClassDeviceData = NULL;
    ULONG predictedLength = 0;
    ULONG requiredLength = 0;
    ULONG i;

    //
    // Open a handle to the plug and play dev node.
    // SetupDiGetClassDevs() returns a device information set that contains
    // info on all installed devices of a specified class.
    //
    hardwareDeviceInfo = SetupDiGetClassDevs((LPGUID)&GUID_CLASS_MAVERICK,
                                             NULL, NULL,
                                             (DIGCF_PRESENT |
                                              DIGCF_INTERFACEDEVICE));

    //
    // Set the size of the device info data structure.
    //
    deviceInfoData.cbSize = sizeof(SP_INTERFACE_DEVICE_DATA);

    //
    // Loop through all available USB devices until we find ours or we run out
    // of devices.
    //
    for(i = 0; ; i++)
    {
        //
        // SetupDiEnumDeviceInterfaces() returns information about device
        // interfaces exposed by one or more devices.  Each call returns
        // information about one interface; the routine can be called
        // repeatedly to get information about several interfaces exposed
        // by one or more devices.
        //
        if(SetupDiEnumDeviceInterfaces(hardwareDeviceInfo, 0,
                                       (LPGUID)&GUID_CLASS_MAVERICK, i,
                                       &deviceInfoData))
        {
            //
            // Allocate a function class device data structure to receive the
            // goods about this particular device.
            //
            SetupDiGetInterfaceDeviceDetail(hardwareDeviceInfo,
                                            &deviceInfoData, NULL, 0,
                                            &requiredLength, NULL);

            //
            // Set the predicted structure length to the required length.
            //
            predictedLength = requiredLength;

            //
            // Allocate memory for the function class device data structure.
            //
            functionClassDeviceData = malloc(predictedLength);
            functionClassDeviceData->cbSize =
                sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);

            //
            // Retrieve the information from Plug and Play.
            //
            if(SetupDiGetInterfaceDeviceDetail(hardwareDeviceInfo,
                                               &deviceInfoData,
                                               functionClassDeviceData,
                                               predictedLength,
                                               &requiredLength, NULL))
            {
                //
                // We were able to get the information from Plug and Play, so
                // attempt to open the device.
                //
                hFile = CreateFile(functionClassDeviceData->DevicePath,
                                   GENERIC_READ | GENERIC_WRITE,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                                   OPEN_EXISTING, 0, NULL);

                //
                // Open another handle to the device for performing status
                // checks.
                //
                hStatusFile = CreateFile(functionClassDeviceData->DevicePath,
                                         GENERIC_READ | GENERIC_WRITE,
                                         FILE_SHARE_READ | FILE_SHARE_WRITE,
                                         NULL, OPEN_EXISTING, 0, NULL);
            }

            //
            // Free the memory for the function class device data structure.
            //
            free(functionClassDeviceData);

            //
            // If we were able to open the device, then stop looking.
            //
            if(hFile != INVALID_HANDLE_VALUE)
            {
                break;
            }
        }
        else
        {
            //
            // Stop looking if there are no more items to check.
            //
            if(GetLastError() == ERROR_NO_MORE_ITEMS)
            {
                break;
            }
        }
    }

    //
    // SetupDiDestroyDeviceInfoList() destroys a device information set
    // and frees all associated memory.
    //
    SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);

    //
    // If we were able to open the device, then return success.
    //
    if(hFile != INVALID_HANDLE_VALUE)
    {
        return(1);
    }

    //
    // Return failure.
    //
    return(0);
}

//****************************************************************************
//
// Maverick_CloseDevice closes the Maverick Digital Audio Player.
//
//****************************************************************************
void
Maverick_CloseDevice(void)
{
    //
    // Make sure that the device was opened.
    //
    if(hFile == INVALID_HANDLE_VALUE)
    {
        return;
    }

    //
    // Close the handle to the driver.
    //
    CloseHandle(hFile);
    hFile = INVALID_HANDLE_VALUE;
    CloseHandle(hStatusFile);
    hStatusFile = INVALID_HANDLE_VALUE;
}

//****************************************************************************
//
// Maverick_GetDescriptor returns the value of the specified descriptor.
//
//****************************************************************************
unsigned long
Maverick_GetDescriptor(unsigned long ulDescriptor, unsigned long ulIndex,
                       void *pvValue, unsigned long *pulLength)
{
    MavUsb_IoctlParams sParam;
    MavUsb_IoctlResults sResult;
    unsigned long ulCount, ulResult;

    //
    // Set up the "get_descriptor" request.
    //
    sParam.ulRequestCode = REQUEST_GET_DESCRIPTOR;

    //
    // Fill in the parameters for the request.
    //
    sParam.uParams.sGetDescriptor.ulDescriptor = ulDescriptor;
    sParam.uParams.sGetDescriptor.ulIndex = ulIndex;

    //
    // Send the request to the Maverick(tm) USB driver.
    //
    ulResult = DeviceIoControl(hFile, IOCTL_MAVUSB_DEVICE_REQUEST, &sParam,
                               sizeof(MavUsb_IoctlParams), &sResult,
                               sizeof(MavUsb_IoctlResults), &ulCount, NULL);

    //
    // If the IOCTL was successfuly, the correct number of bytes were returned,
    // and the request was handled by the device, then return the value of the
    // descriptor.
    //
    if(ulResult &&
       (ulCount == sizeof(MavUsb_IoctlResults)) &&
       sResult.ulReturnCode)
    {
        //
        // Return the value of the descriptor.
        //
        memcpy(pvValue, sResult.uResults.sGetDescriptor.pcValue,
               min(*pulLength, sResult.uResults.sGetDescriptor.ulLength));

        //
        // Return the length of the descriptor.
        //
        *pulLength = sResult.uResults.sGetDescriptor.ulLength;

        //
        // Success.
        //
        return(1);
    }

    //
    // Return an error.
    //
    return(0);
}

//****************************************************************************
//
// Maverick_SetDescriptor sets the value of the specified descriptor.
//
//****************************************************************************
unsigned long
Maverick_SetDescriptor(unsigned long ulDescriptor, unsigned long ulIndex,
                       void *pvValue, unsigned long ulLength)
{
    MavUsb_IoctlParams sParam;
    MavUsb_IoctlResults sResult;
    unsigned long ulCount, ulResult;

    //
    // Make sure that the length is valid.
    //
    if(ulLength > sizeof(sParam.uParams.sSetDescriptor.pcValue))
    {
        return(0);
    }

    //
    // Set up the "set_descriptor" request.
    //
    sParam.ulRequestCode = REQUEST_SET_DESCRIPTOR;

    //
    // Fill in the parameters for the request.
    //
    sParam.uParams.sSetDescriptor.ulDescriptor = ulDescriptor;
    sParam.uParams.sSetDescriptor.ulIndex = ulIndex;
    memcpy(sParam.uParams.sSetDescriptor.pcValue, pvValue, ulLength);
    sParam.uParams.sSetDescriptor.ulLength = ulLength;

    //
    // Send the request to the Maverick(tm) USB driver.
    //
    ulResult = DeviceIoControl(hFile, IOCTL_MAVUSB_DEVICE_REQUEST, &sParam,
                               sizeof(MavUsb_IoctlParams), &sResult,
                               sizeof(MavUsb_IoctlResults), &ulCount, NULL);

    //
    // If the IOCTL was successfuly, the correct number of bytes were returned,
    // and the request was handled by the device, then return success.
    //
    if(ulResult &&
       (ulCount == sizeof(MavUsb_IoctlResults)) &&
       sResult.ulReturnCode)
    {
        //
        // Success.
        //
        return(1);
    }

    //
    // Return an error.
    //
    return(0);
}

//****************************************************************************
//
// Maverick_NumDrives returns the number of drives supported by the file
// system layer.
//
//****************************************************************************
unsigned long
Maverick_NumDrives(void)
{
    MavUsb_IoctlParams sParam;
    MavUsb_IoctlResults sResult;
    unsigned long ulCount, ulResult;

    //
    // Set up the "num_drives" request.
    //
    sParam.ulRequestCode = REQUEST_NUM_DRIVES;

    //
    // Send the request to the Maverick(tm) USB driver.
    //
    ulResult = DeviceIoControl(hFile, IOCTL_MAVUSB_DEVICE_REQUEST, &sParam,
                               sizeof(MavUsb_IoctlParams), &sResult,
                               sizeof(MavUsb_IoctlResults), &ulCount, NULL);

    //
    // If the IOCTL was successfuly, the correct number of bytes were returned,
    // and the request was handled by the device, then return the number of
    // drives.
    //
    if(ulResult &&
       (ulCount == sizeof(MavUsb_IoctlResults)) &&
       sResult.ulReturnCode)
    {
        //
        // Return the number of drives we support.
        //
        return(sResult.uResults.sNumDrives.ulNumDrives);
    }

    //
    // There was an error, so return zero.
    //
    return(0);
}

//****************************************************************************
//
// Maverick_Open opens or creates the specified file on the specified drive.
//
//****************************************************************************
unsigned long
Maverick_Open(unsigned long ulDrive, const unsigned short *pusFileName,
              unsigned long ulFlags)
{
    MavUsb_IoctlParams sParam;
    MavUsb_IoctlResults sResult;
    unsigned long ulCount, ulResult;

    //
    // Set up the "open" request.
    //
    sParam.ulRequestCode = REQUEST_OPEN;
    sParam.uParams.sOpen.ulDriveNum = ulDrive;
    memcpy(sParam.uParams.sOpen.pusFileName, pusFileName, 256);
    sParam.uParams.sOpen.ulFlags = ulFlags;

    //
    // Send the request to the Maverick(tm) USB driver.
    //
    ulResult = DeviceIoControl(hFile, IOCTL_MAVUSB_DEVICE_REQUEST, &sParam,
                               sizeof(MavUsb_IoctlParams), &sResult,
                               sizeof(MavUsb_IoctlResults), &ulCount, NULL);

    //
    // If the IOCTL was successfuly, the correct number of bytes were returned,
    // and the request was handled by the device, then return success.
    //
    if(ulResult &&
       (ulCount == sizeof(MavUsb_IoctlResults)) &&
       sResult.ulReturnCode)
    {
        //
        // Success.
        //
        return(1);
    }

    //
    // Return an error.
    //
    return(0);
}

//****************************************************************************
//
// Maverick_Create creates the specified file with the given size on the
// specified drive.
//
//****************************************************************************
unsigned long
Maverick_Create(unsigned long ulDrive, const unsigned short *pusFileName,
                unsigned long ulFileLength)
{
    MavUsb_IoctlParams sParam;
    MavUsb_IoctlResults sResult;
    unsigned long ulCount, ulResult;

    //
    // Set up the "create" request.
    //
    sParam.ulRequestCode = REQUEST_CREATE;
    sParam.uParams.sCreate.ulDriveNum = ulDrive;
    memcpy(sParam.uParams.sCreate.pusFileName, pusFileName, 256);
    sParam.uParams.sCreate.ulLength = ulFileLength;

    //
    // Send the request to the Maverick(tm) USB driver.
    //
    ulResult = DeviceIoControl(hFile, IOCTL_MAVUSB_DEVICE_REQUEST, &sParam,
                               sizeof(MavUsb_IoctlParams), &sResult,
                               sizeof(MavUsb_IoctlResults), &ulCount, NULL);

    //
    // If the IOCTL was successfuly, the correct number of bytes were returned,
    // and the request was handled by the device, then return success.
    //
    if(ulResult &&
       (ulCount == sizeof(MavUsb_IoctlResults)) &&
       sResult.ulReturnCode)
    {
        //
        // Success.
        //
        return(1);
    }

    //
    // Return an error.
    //
    return(0);
}

//****************************************************************************
//
// Maverick_Read reads data from the opened file.
//
//****************************************************************************
unsigned long
Maverick_Read(void *pvBuffer, unsigned long ulNumBytes)
{
    unsigned long ulReturn;

    //
    // Read from the file.
    //
    ReadFile(hFile, pvBuffer, ulNumBytes, &ulReturn, NULL);

    //
    // Return the number of bytes read.
    //
    return(ulReturn);
}

//****************************************************************************
//
// Maverick_Write writes data to the opened file.
//
//****************************************************************************
unsigned long
Maverick_Write(void *pvBuffer, unsigned long ulNumBytes)
{
    unsigned long ulReturn;

    //
    // Write to the file.
    //
    WriteFile(hFile, pvBuffer, ulNumBytes, &ulReturn, NULL);

    //
    // Return the number of bytes written.
    //
    return(ulReturn);
}

//****************************************************************************
//
// Maverick_Seek moves the read/write pointer for the opened file to the
// specified position.
//
//****************************************************************************
unsigned long
Maverick_Seek(unsigned long ulPos)
{
    MavUsb_IoctlParams sParam;
    MavUsb_IoctlResults sResult;
    unsigned long ulCount, ulResult;

    //
    // Set up the "seek" request.
    //
    sParam.ulRequestCode = REQUEST_SEEK;
    sParam.uParams.sSeek.ulPosition = ulPos;

    //
    // Send the request to the Maverick(tm) USB driver.
    //
    ulResult = DeviceIoControl(hFile, IOCTL_MAVUSB_DEVICE_REQUEST, &sParam,
                               sizeof(MavUsb_IoctlParams), &sResult,
                               sizeof(MavUsb_IoctlResults), &ulCount, NULL);

    //
    // If the IOCTL was successfuly, the correct number of bytes were returned,
    // and the request was handled by the device, then return success.
    //
    if(ulResult &&
       (ulCount == sizeof(MavUsb_IoctlResults)) &&
       sResult.ulReturnCode)
    {
        //
        // Success.
        //
        return(1);
    }

    //
    // Return an error.
    //
    return(0);
}

//****************************************************************************
//
// Maverick_Tell returns the current read/write for the opened file.
//
//****************************************************************************
unsigned long
Maverick_Tell(void)
{
    MavUsb_IoctlParams sParam;
    MavUsb_IoctlResults sResult;
    unsigned long ulCount, ulResult;

    //
    // Set up the "tell" request.
    //
    sParam.ulRequestCode = REQUEST_TELL;

    //
    // Send the request to the Maverick(tm) USB driver.
    //
    ulResult = DeviceIoControl(hFile, IOCTL_MAVUSB_DEVICE_REQUEST, &sParam,
                               sizeof(MavUsb_IoctlParams), &sResult,
                               sizeof(MavUsb_IoctlResults), &ulCount, NULL);

    //
    // If the IOCTL was successfuly, the correct number of bytes were returned,
    // and the request was handled by the device, then return the current file
    // position.
    //
    if(ulResult &&
       (ulCount == sizeof(MavUsb_IoctlResults)) &&
       sResult.ulReturnCode)
    {
        //
        // Return the current file position.
        //
        return(sResult.uResults.sTell.ulPosition);
    }

    //
    // Return an error.
    //
    return(0);
}

//****************************************************************************
//
// Maverick_Length returns the length of the currently opened file.
//
//****************************************************************************
unsigned long
Maverick_Length(void)
{
    MavUsb_IoctlParams sParam;
    MavUsb_IoctlResults sResult;
    unsigned long ulCount, ulResult;

    //
    // Set up the "length" request.
    //
    sParam.ulRequestCode = REQUEST_LENGTH;

    //
    // Send the request to the Maverick(tm) USB driver.
    //
    ulResult = DeviceIoControl(hFile, IOCTL_MAVUSB_DEVICE_REQUEST, &sParam,
                               sizeof(MavUsb_IoctlParams), &sResult,
                               sizeof(MavUsb_IoctlResults), &ulCount, NULL);

    //
    // If the IOCTL was successfuly, the correct number of bytes were returned,
    // and the request was handled by the device, then return the file length.
    //
    if(ulResult &&
       (ulCount == sizeof(MavUsb_IoctlResults)) &&
       sResult.ulReturnCode)
    {
        //
        // Return the file length.
        //
        return(sResult.uResults.sLength.ulLength);
    }

    //
    // Return an error.
    //
    return(0);
}

//****************************************************************************
//
// Maverick_GetDate returns the modification date/time of the currently opened
// file.
//
//****************************************************************************
unsigned long
Maverick_GetDate(void)
{
    MavUsb_IoctlParams sParam;
    MavUsb_IoctlResults sResult;
    unsigned long ulCount, ulResult;

    //
    // Set up the "get date" request.
    //
    sParam.ulRequestCode = REQUEST_GETDATE;

    //
    // Send the request to the Maverick(tm) USB driver.
    //
    ulResult = DeviceIoControl(hFile, IOCTL_MAVUSB_DEVICE_REQUEST, &sParam,
                               sizeof(MavUsb_IoctlParams), &sResult,
                               sizeof(MavUsb_IoctlResults), &ulCount, NULL);

    //
    // If the IOCTL was successfuly, the correct number of bytes were returned,
    // and the request was handled by the device, then return the file length.
    //
    if(ulResult &&
       (ulCount == sizeof(MavUsb_IoctlResults)) &&
       sResult.ulReturnCode)
    {
        //
        // Return the modification date/time of the file..
        //
        return(sResult.uResults.sGetDate.ulDate);
    }

    //
    // Return an error.
    //
    return(0);
}

//****************************************************************************
//
// Maverick_Close closes the currently opened file.
//
//****************************************************************************
unsigned long
Maverick_Close(void)
{
    MavUsb_IoctlParams sParam;
    MavUsb_IoctlResults sResult;
    unsigned long ulCount, ulResult;

    //
    // Set up the "close" request.
    //
    sParam.ulRequestCode = REQUEST_CLOSE;

    //
    // Send the request to the Maverick(tm) USB driver.
    //
    ulResult = DeviceIoControl(hFile, IOCTL_MAVUSB_DEVICE_REQUEST, &sParam,
                               sizeof(MavUsb_IoctlParams), &sResult,
                               sizeof(MavUsb_IoctlResults), &ulCount, NULL);

    //
    // If the IOCTL was successfuly, the correct number of bytes were returned,
    // and the request was handled by the device, then return success.
    //
    if(ulResult &&
       (ulCount == sizeof(MavUsb_IoctlResults)) &&
       sResult.ulReturnCode)
    {
        //
        // Return success.
        //
        return(1);
    }

    //
    // Return an error.
    //
    return(0);
}

//****************************************************************************
//
// Maverick_Delete removes the specified file from the file system.
//
//****************************************************************************
unsigned long
Maverick_Delete(unsigned long ulDrive, const unsigned short *pusFileName)
{
    MavUsb_IoctlParams sParam;
    MavUsb_IoctlResults sResult;
    unsigned long ulCount, ulResult;

    //
    // Set up the "delete" request.
    //
    sParam.ulRequestCode = REQUEST_DELETE;
    sParam.uParams.sDelete.ulDriveNum = ulDrive;
    memcpy(sParam.uParams.sDelete.pusFileName, pusFileName, 256);

    //
    // Send the request to the Maverick(tm) USB driver.
    //
    ulResult = DeviceIoControl(hFile, IOCTL_MAVUSB_DEVICE_REQUEST, &sParam,
                               sizeof(MavUsb_IoctlParams), &sResult,
                               sizeof(MavUsb_IoctlResults), &ulCount, NULL);

    //
    // If the IOCTL was successfuly, the correct number of bytes were returned,
    // and the request was handled by the device, then return success.
    //
    if(ulResult &&
       (ulCount == sizeof(MavUsb_IoctlResults)) &&
       sResult.ulReturnCode)
    {
        //
        // Return success.
        //
        return(1);
    }

    //
    // Return an error.
    //
    return(0);
}

//****************************************************************************
//
// Maverick_OpenDir opens the specified directory on the specified drive.
//
//****************************************************************************
unsigned long
Maverick_OpenDir(unsigned long ulDrive, const unsigned short *pusDirName)
{
    MavUsb_IoctlParams sParam;
    MavUsb_IoctlResults sResult;
    unsigned long ulCount, ulResult;

    //
    // Set up the "opendir" request.
    //
    sParam.ulRequestCode = REQUEST_OPENDIR;
    sParam.uParams.sOpenDir.ulDriveNum = ulDrive;
    memcpy(sParam.uParams.sOpenDir.pusDirName, pusDirName, 256);

    //
    // Send the request to the Maverick(tm) USB driver.
    //
    ulResult = DeviceIoControl(hFile, IOCTL_MAVUSB_DEVICE_REQUEST, &sParam,
                               sizeof(MavUsb_IoctlParams), &sResult,
                               sizeof(MavUsb_IoctlResults), &ulCount, NULL);

    //
    // If the IOCTL was successfuly, the correct number of bytes were returned,
    // and the request was handled by the device, then return success.
    //
    if(ulResult &&
       (ulCount == sizeof(MavUsb_IoctlResults)) &&
       sResult.ulReturnCode)
    {
        //
        // Return success.
        //
        return(1);
    }

    //
    // Return an error.
    //
    return(0);
}

//****************************************************************************
//
// Maverick_ReadDir reads the next directory entry from the currently opened
// directory.
//
//****************************************************************************
unsigned long
Maverick_ReadDir(unsigned short *pusFileName, unsigned long ulType)
{
    MavUsb_IoctlParams sParam;
    MavUsb_IoctlResults sResult;
    unsigned long ulCount, ulResult;

    //
    // Set up the "readdir" request.
    //
    sParam.ulRequestCode = REQUEST_READDIR;
    sParam.uParams.sReadDir.ulType = ulType;

    //
    // Send the request to the Maverick(tm) USB driver.
    //
    ulResult = DeviceIoControl(hFile, IOCTL_MAVUSB_DEVICE_REQUEST, &sParam,
                               sizeof(MavUsb_IoctlParams), &sResult,
                               sizeof(MavUsb_IoctlResults), &ulCount, NULL);

    //
    // If the IOCTL was successfuly, the correct number of bytes were returned,
    // and the request was handled by the device, then return the file name.
    //
    if(ulResult &&
       (ulCount == sizeof(MavUsb_IoctlResults)) &&
       sResult.ulReturnCode)
    {
        //
        // Return the name of the file found.
        //
        memcpy(pusFileName, sResult.uResults.sReadDir.pusName, 256);

        //
        // Return success.
        //
        return(1);
    }

    //
    // Return an error.
    //
    return(0);
}

//****************************************************************************
//
// Maverick_CloseDir closes the currently opened directory.
//
//****************************************************************************
unsigned long
Maverick_CloseDir(void)
{
    MavUsb_IoctlParams sParam;
    MavUsb_IoctlResults sResult;
    unsigned long ulCount, ulResult;

    //
    // Set up the "closedir" request.
    //
    sParam.ulRequestCode = REQUEST_CLOSEDIR;

    //
    // Send the request to the Maverick(tm) USB driver.
    //
    ulResult = DeviceIoControl(hFile, IOCTL_MAVUSB_DEVICE_REQUEST, &sParam,
                               sizeof(MavUsb_IoctlParams), &sResult,
                               sizeof(MavUsb_IoctlResults), &ulCount, NULL);

    //
    // If the IOCTL was successfuly, the correct number of bytes were returned,
    // and the request was handled by the device, then return success.
    //
    if(ulResult &&
       (ulCount == sizeof(MavUsb_IoctlResults)) &&
       sResult.ulReturnCode)
    {
        //
        // Return success.
        //
        return(1);
    }

    //
    // Return an error.
    //
    return(0);
}

//****************************************************************************
//
// Maverick_MakeDir creates the specified directory on the specified drive.
//
//****************************************************************************
unsigned long
Maverick_MakeDir(unsigned long ulDrive, const unsigned short *pusDirName)
{
    MavUsb_IoctlParams sParam;
    MavUsb_IoctlResults sResult;
    unsigned long ulCount, ulResult;

    //
    // Set up the "makedir" request.
    //
    sParam.ulRequestCode = REQUEST_MAKEDIR;
    sParam.uParams.sMakeDir.ulDriveNum = ulDrive;
    memcpy(sParam.uParams.sMakeDir.pusDirName, pusDirName, 256);

    //
    // Send the request to the Maverick(tm) USB driver.
    //
    ulResult = DeviceIoControl(hFile, IOCTL_MAVUSB_DEVICE_REQUEST, &sParam,
                               sizeof(MavUsb_IoctlParams), &sResult,
                               sizeof(MavUsb_IoctlResults), &ulCount, NULL);

    //
    // If the IOCTL was successfuly, the correct number of bytes were returned,
    // and the request was handled by the device, then return success.
    //
    if(ulResult &&
       (ulCount == sizeof(MavUsb_IoctlResults)) &&
       sResult.ulReturnCode)
    {
        //
        // Return success.
        //
        return(1);
    }

    //
    // Return an error.
    //
    return(0);
}

//****************************************************************************
//
// Maverick_RemoveDir removes the specified directry from the specified drive.
//
//****************************************************************************
unsigned long
Maverick_RemoveDir(unsigned long ulDrive, const unsigned short *pusDirName)
{
    MavUsb_IoctlParams sParam;
    MavUsb_IoctlResults sResult;
    unsigned long ulCount, ulResult;

    //
    // Set up the "removedir" request.
    //
    sParam.ulRequestCode = REQUEST_REMOVEDIR;
    sParam.uParams.sRemoveDir.ulDriveNum = ulDrive;
    memcpy(sParam.uParams.sRemoveDir.pusDirName, pusDirName, 256);

    //
    // Send the request to the Maverick(tm) USB driver.
    //
    ulResult = DeviceIoControl(hFile, IOCTL_MAVUSB_DEVICE_REQUEST, &sParam,
                               sizeof(MavUsb_IoctlParams), &sResult,
                               sizeof(MavUsb_IoctlResults), &ulCount, NULL);

    //
    // If the IOCTL was successfuly, the correct number of bytes were returned,
    // and the request was handled by the device, then return success.
    //
    if(ulResult &&
       (ulCount == sizeof(MavUsb_IoctlResults)) &&
       sResult.ulReturnCode)
    {
        //
        // Return success.
        //
        return(1);
    }

    //
    // Return an error.
    //
    return(0);
}

//****************************************************************************
//
// Maverick_TotalSpace returns the total capacity of the specified drive.
//
//****************************************************************************
unsigned long
Maverick_TotalSpace(unsigned long ulDrive)
{
    MavUsb_IoctlParams sParam;
    MavUsb_IoctlResults sResult;
    unsigned long ulCount, ulResult;

    //
    // Set up the "totalspace" request.
    //
    sParam.ulRequestCode = REQUEST_TOTAL_SPACE;
    sParam.uParams.sTotalSpace.ulDriveNum = ulDrive;

    //
    // Send the request to the Maverick(tm) USB driver.
    //
    ulResult = DeviceIoControl(hFile, IOCTL_MAVUSB_DEVICE_REQUEST, &sParam,
                               sizeof(MavUsb_IoctlParams), &sResult,
                               sizeof(MavUsb_IoctlResults), &ulCount, NULL);

    //
    // If the IOCTL was successfuly, the correct number of bytes were returned,
    // and the request was handled by the device, then return the drive
    // capacity.
    //
    if(ulResult &&
       (ulCount == sizeof(MavUsb_IoctlResults)) &&
       sResult.ulReturnCode)
    {
        //
        // Return the drive capacity.
        //
        return(sResult.uResults.sTotalSpace.ulBytes);
    }

    //
    // Return an error.
    //
    return(0);
}

//****************************************************************************
//
// Maverick_FreeSpace returns the currently available capacity on the
// specified drive.
//
//****************************************************************************
unsigned long
Maverick_FreeSpace(unsigned long ulDrive)
{
    MavUsb_IoctlParams sParam;
    MavUsb_IoctlResults sResult;
    unsigned long ulCount, ulResult;

    //
    // Set up the "freespace" request.
    //
    sParam.ulRequestCode = REQUEST_FREE_SPACE;
    sParam.uParams.sFreeSpace.ulDriveNum = ulDrive;

    //
    // Send the request to the Maverick(tm) USB driver.
    //
    ulResult = DeviceIoControl(hFile, IOCTL_MAVUSB_DEVICE_REQUEST, &sParam,
                               sizeof(MavUsb_IoctlParams), &sResult,
                               sizeof(MavUsb_IoctlResults), &ulCount, NULL);

    //
    // If the IOCTL was successfuly, the correct number of bytes were returned,
    // and the request was handled by the device, then return the free space.
    //
    if(ulResult &&
       (ulCount == sizeof(MavUsb_IoctlResults)) &&
       sResult.ulReturnCode)
    {
        //
        // Return the free space.
        //
        return(sResult.uResults.sFreeSpace.ulBytes);
    }

    //
    // Return an error.
    //
    return(0);
}

//****************************************************************************
//
// Maverick_Format formats the specified drive.
//
//****************************************************************************
unsigned long
Maverick_Format(unsigned long ulDrive)
{
    MavUsb_IoctlParams sParam;
    MavUsb_IoctlResults sResult;
    unsigned long ulCount, ulResult;

    //
    // Set up the "format" request.
    //
    sParam.ulRequestCode = REQUEST_FORMAT;
    sParam.uParams.sFormat.ulDriveNum = ulDrive;

    //
    // Send the request to the Maverick(tm) USB driver.
    //
    ulResult = DeviceIoControl(hFile, IOCTL_MAVUSB_DEVICE_REQUEST, &sParam,
                               sizeof(MavUsb_IoctlParams), &sResult,
                               sizeof(MavUsb_IoctlResults), &ulCount, NULL);

    //
    // If the IOCTL was successfuly, the correct number of bytes were returned,
    // and the request was handled by the device, then return success.
    //
    if(ulResult &&
       (ulCount == sizeof(MavUsb_IoctlResults)) &&
       sResult.ulReturnCode)
    {
        //
        // Return success.
        //
        return(1);
    }

    //
    // Return an error.
    //
    return(0);
}

//****************************************************************************
//
// Maverick_StartUpdate indicates that a software update is about to be
// performed.
//
//****************************************************************************
unsigned long
Maverick_StartUpdate(void)
{
    MavUsb_IoctlParams sParam;
    MavUsb_IoctlResults sResult;
    unsigned long ulCount, ulResult;

    //
    // Set up the "start_update" request.
    //
    sParam.ulRequestCode = REQUEST_START_UPDATE;

    //
    // Send the request to the Maverick(tm) USB driver.
    //
    ulResult = DeviceIoControl(hFile, IOCTL_MAVUSB_DEVICE_REQUEST, &sParam,
                               sizeof(MavUsb_IoctlParams), &sResult,
                               sizeof(MavUsb_IoctlResults), &ulCount, NULL);

    //
    // If the IOCTL was successfuly, the correct number of bytes were returned,
    // and the request was handled by the device, then return success.
    //
    if(ulResult &&
       (ulCount == sizeof(MavUsb_IoctlResults)) &&
       sResult.ulReturnCode)
    {
        //
        // Return success.
        //
        return(1);
    }

    //
    // Return an error.
    //
    return(0);
}

//****************************************************************************
//
// Maverick_ResetTransferCount resets the count of bytes transferred to/from
// the USB device.
//
//****************************************************************************
void
Maverick_ResetTransferCount(void)
{
    unsigned long ulCount;

    //
    // Reset the transfer byte count.
    //
    DeviceIoControl(hFile, IOCTL_MAVUSB_RESET_TRANSFER_COUNT, NULL, 0, NULL,
                    0, &ulCount, NULL);
}

//****************************************************************************
//
// Maverick_GetTransferCount returns the number of bytes transferred to/from
// the USB device.
//
//****************************************************************************
unsigned long
Maverick_GetTransferCount(void)
{
    unsigned long ulResult, ulCount, ulValue;

    //
    // Get the count of bytes transferred.
    //
    ulResult = DeviceIoControl(hStatusFile, IOCTL_MAVUSB_GET_TRANSFER_COUNT,
                               NULL, 0, &ulValue, sizeof(unsigned long),
                               &ulCount, NULL);

    //
    // If the IOCTL was successful and the correct number of bytes were
    // returned, then return the current transfer count.
    //
    if(ulResult && (ulCount == sizeof(unsigned long)))
    {
        //
        // Return the transfer count.
        //
        return(ulValue);
    }

    //
    // Return zero.
    //
    return(0);
}

//****************************************************************************
//
// Maverick_GetDeviceName gets the name of the device from the USB device
// descriptor.
//
//****************************************************************************
unsigned long
Maverick_GetDeviceName(unsigned short *pusBuffer, unsigned long ulLength)
{
    unsigned long ulResult, ulCount;

    //
    // Get the count of bytes transferred.
    //
    ulResult = DeviceIoControl(hFile, IOCTL_MAVUSB_GET_NAME, NULL, 0,
                               pusBuffer, ulLength, &ulCount, NULL);

    //
    // If the IOCTL was successful then return success.
    //
    if(ulResult)
    {
        //
        // Success.
        //
        return(1);
    }

    //
    // Return failure.
    //
    return(0);
}

//****************************************************************************
//
// Maverick_GetManufacturerName gets the name of the device from the USB
// device descriptor.
//
//****************************************************************************
unsigned long
Maverick_GetManufacturerName(unsigned short *pusBuffer, unsigned long ulLength)
{
    unsigned long ulResult, ulCount;

    //
    // Get the count of bytes transferred.
    //
    ulResult = DeviceIoControl(hFile, IOCTL_MAVUSB_GET_MANUFACTURER_NAME, NULL,
                               0, pusBuffer, ulLength, &ulCount, NULL);

    //
    // If the IOCTL was successful then return success.
    //
    if(ulResult)
    {
        //
        // Success.
        //
        return(1);
    }

    //
    // Return failure.
    //
    return(0);
}

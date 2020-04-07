//****************************************************************************
//
// MAVPROTO.H - Contains structure definitions and function prototypes for
//              the Maverick(tm) USB driver.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
// Copyright (c) 1997-1998 Microsoft Corporation.  All Rights Reserved.
//   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//   PURPOSE.
//
//****************************************************************************

#ifndef MAVPROTOH_INCD
#define MAVPROTOH_INCD

//****************************************************************************
//
// A structure representing the instance information associated with
// this particular device.
//
//****************************************************************************
typedef struct _DEVICE_EXTENSION
{
    //
    // Device object we call when submitting URBs.
    //
    PDEVICE_OBJECT TopOfStackDeviceObject;

    //
    // The bus driver object
    //
    PDEVICE_OBJECT PhysicalDeviceObject;

    //
    // The current power state of the device.
    //
    DEVICE_POWER_STATE CurrentDevicePowerState;

    //
    // USB configuration handle and pointer for the configuration the device is
    // currently in.
    //
    USBD_CONFIGURATION_HANDLE UsbConfigurationHandle;
    PUSB_CONFIGURATION_DESCRIPTOR UsbConfigurationDescriptor;

    //
    // Pointer to the USB device descriptor for this device
    //
    PUSB_DEVICE_DESCRIPTOR UsbDeviceDescriptor;

    //
    // We support one interface.  This is a copy of the info structure returned
    // from select_configuration or select_interface.
    //
    PUSBD_INTERFACE_INFORMATION UsbInterface;

    //
    // Bus drivers set the appropriate values in this structure in response
    // to an IRP_MN_QUERY_CAPABILITIES IRP.  Function and filter drivers might
    // alter the capabilities set by the bus driver.
    //
    DEVICE_CAPABILITIES DeviceCapabilities;

    //
    // Used to save the currently-being-handled system-requested power IRP
    // request.
    //
    PIRP PowerIrp;

    //
    // Count of self-staged bytes read or written so far.
    //
    ULONG StagedBytesTransferred;

    //
    // Set when PendingIoCount goes to 0; flags device can be removed.
    //
    KEVENT RemoveEvent;

    //
    // Set when PendingIoCount goes to 1 (1st increment was on add device).
    // This indicates no IO requests outstanding, either user, system, or
    // self-staged.
    //
    KEVENT NoPendingIoEvent;

    //
    // Set to signal driver-generated power request is finished.
    //
    KEVENT SelfRequestedPowerIrpEvent;

    //
    // Spinlock used to protect increment/decrement IO count logic.
    //
    KSPIN_LOCK IoCountSpinLock;

    //
    // Incremented when device is added and any IO request is received;
    // decremented when any IO request is completed or passed on, and when
    // device is removed.
    //
    ULONG PendingIoCount;

    //
    // Count of open pipes.
    //
    ULONG OpenPipeCount;

    //
    // Name buffer for our named Functional device object link.  The name is
    // generated based on the driver's class GUID.
    //
    WCHAR DeviceLinkNameBuffer[MAXIMUM_FILENAME_LENGTH];

    //
    // Flag set when processing IRP_MN_REMOVE_DEVICE.
    //
    BOOLEAN DeviceRemoved;

    //
    // Flag set when driver has answered success to IRP_MN_QUERY_REMOVE_DEVICE.
    //
    BOOLEAN RemoveDeviceRequested;

    //
    // Flag set when driver has answered success to IRP_MN_QUERY_STOP_DEVICE.
    //
    BOOLEAN StopDeviceRequested;

    //
    // Flag set when device has been successfully started.
    //
    BOOLEAN DeviceStarted;

    //
    // Flag set when IRP_MN_WAIT_WAKE is received and we're in a power state
    // where we can signal a wait.
    //
    BOOLEAN EnabledForWakeup;

    //
    // Used to flag that we're currently handling a self-generated power
    // request.
    //
    BOOLEAN SelfPowerIrp;

    //
    // Used to indicate that a staged read or write operation is pending.
    //
    BOOLEAN StagedIoPending;

    //
    // Default power state to power down to on self-suspend.
    //
    ULONG PowerDownLevel;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

//****************************************************************************
//
// Function Prototypes
//
//****************************************************************************

//
// From mavioctl.c
//
NTSTATUS MavUsb_ProcessIOCTL(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS MavUsb_ResetDevice(IN PDEVICE_OBJECT DeviceObject);
NTSTATUS MavUsb_GetPortStatus(IN PDEVICE_OBJECT DeviceObject,
                              IN PULONG PortStatus);
NTSTATUS MavUsb_ResetParentPort(IN PDEVICE_OBJECT DeviceObject);
NTSTATUS MavUsb_SendVendorCommand(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                                  IN ULONG drive, IN ULONG request,
                                  IN USHORT value, IN USHORT index,
                                  IN PUCHAR buffer, IN ULONG length);
NTSTATUS MavUsb_BulkXfer(IN PDEVICE_OBJECT DeviceObject, IN PUCHAR buffer,
                         IN ULONG length, IN BOOLEAN Read);
NTSTATUS MavUsb_VendorConnect(IN PDEVICE_OBJECT DeviceObject);
NTSTATUS MavUsb_VendorDisconnect(IN PDEVICE_OBJECT DeviceObject);
NTSTATUS MavUsb_VendorGetDescriptor(IN PDEVICE_OBJECT DeviceObject,
                                    IN PURB urb, PUCHAR buffer,
                                    MavUsb_GetDescriptorParams *pIn,
                                    MavUsb_GetDescriptorResult *pOut,
                                    PULONG result);
NTSTATUS MavUsb_VendorSetDescriptor(IN PDEVICE_OBJECT DeviceObject,
                                    IN PURB urb, PUCHAR buffer,
                                    MavUsb_SetDescriptorParams *pIn,
                                    PULONG result);
NTSTATUS MavUsb_VendorNumDrives(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                                PUCHAR buffer, MavUsb_NumDrivesResult *pOut,
                                PULONG result);
NTSTATUS MavUsb_VendorOpen(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                           PUCHAR buffer, MavUsb_OpenParams *pIn,
                           PULONG result);
NTSTATUS MavUsb_VendorCreate(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                             PUCHAR buffer, MavUsb_CreateParams *pIn,
                             PULONG result);
NTSTATUS MavUsb_VendorSeek(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                           PUCHAR buffer, MavUsb_SeekParams *pIn,
                           PULONG result);
NTSTATUS MavUsb_VendorTell(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                           PUCHAR buffer, MavUsb_TellResult *pOut,
                           PULONG result);
NTSTATUS MavUsb_VendorLength(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                             PUCHAR buffer, MavUsb_LengthResult *pOut,
                             PULONG result);
NTSTATUS MavUsb_VendorClose(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                            PUCHAR buffer, PULONG result);
NTSTATUS MavUsb_VendorDelete(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                             PUCHAR buffer, MavUsb_DeleteParams *pIn,
                             PULONG result);
NTSTATUS MavUsb_VendorOpenDir(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                              PUCHAR buffer, MavUsb_OpenDirParams *pIn,
                              PULONG result);
NTSTATUS MavUsb_VendorReadDir(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                              PUCHAR buffer, MavUsb_ReadDirParams *pIn,
                              MavUsb_ReadDirResult *pOut, PULONG result);
NTSTATUS MavUsb_VendorCloseDir(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                               PUCHAR buffer, PULONG result);
NTSTATUS MavUsb_VendorMakeDir(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                              PUCHAR buffer, MavUsb_MakeDirParams *pIn,
                              PULONG result);
NTSTATUS MavUsb_VendorRemoveDir(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                                PUCHAR buffer, MavUsb_RemoveDirParams *pIn,
                                PULONG result);
NTSTATUS MavUsb_VendorTotalSpace(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                                 PUCHAR buffer, MavUsb_TotalSpaceParams *pIn,
                                 MavUsb_TotalSpaceResult *pOut, PULONG result);
NTSTATUS MavUsb_VendorFreeSpace(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                                PUCHAR buffer, MavUsb_FreeSpaceParams *pIn,
                                MavUsb_FreeSpaceResult *pOut, PULONG result);
NTSTATUS MavUsb_VendorFormat(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                             PUCHAR buffer, MavUsb_FormatParams *pIn,
                             PULONG result);
NTSTATUS MavUsb_VendorStartUpdate(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                                  PUCHAR buffer, PULONG result);
NTSTATUS MavUsb_VendorEndUpdate(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                                PUCHAR buffer, PULONG result);
NTSTATUS MavUsb_VendorCommand(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp,
                              PIO_STACK_LOCATION irpStack);

//
// From mavocrw.c
//
NTSTATUS MavUsb_Create(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS MavUsb_Close(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS MavUsb_Read(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS MavUsb_Write(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS MavUsb_AbortPipes(IN PDEVICE_OBJECT DeviceObject);
BOOLEAN MavUsb_CanAcceptIoRequests(IN PDEVICE_OBJECT DeviceObject);

//
// From mavpnp.c
//
NTSTATUS MavUsb_ProcessPnPIrp(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS MavUsb_PnPAddDevice(IN PDRIVER_OBJECT DriverObject,
                             IN PDEVICE_OBJECT PhysicalDeviceObject);
NTSTATUS MavUsb_StartDevice(IN PDEVICE_OBJECT DeviceObject);
NTSTATUS MavUsb_RemoveDevice(IN PDEVICE_OBJECT DeviceObject);
NTSTATUS MavUsb_StopDevice(IN PDEVICE_OBJECT DeviceObject);
NTSTATUS MavUsb_IrpCompletionRoutine(IN PDEVICE_OBJECT DeviceObject,
                                     IN PIRP Irp, IN PVOID Context);

//
// From mavpwr.c
//
NTSTATUS MavUsb_ProcessPowerIrp(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS MavUsb_PoRequestCompletion(IN PDEVICE_OBJECT DeviceObject,
                                    IN UCHAR MinorFunction,
                                    IN POWER_STATE PowerState,
                                    IN PVOID Context,
                                    IN PIO_STATUS_BLOCK IoStatus);
NTSTATUS MavUsb_PowerIrp_Complete(IN PDEVICE_OBJECT NullDeviceObject,
                                  IN PIRP Irp, IN PVOID Context);
NTSTATUS MavUsb_SelfSuspendOrActivate(IN PDEVICE_OBJECT DeviceObject,
                                      IN BOOLEAN fSuspend);
NTSTATUS MavUsb_SelfRequestPowerIrp(IN PDEVICE_OBJECT DeviceObject,
                                    IN POWER_STATE PowerState);
NTSTATUS MavUsb_PoSelfRequestCompletion(IN PDEVICE_OBJECT DeviceObject,
                                        IN UCHAR MinorFunction,
                                        IN POWER_STATE PowerState,
                                        IN PVOID Context,
                                        IN PIO_STATUS_BLOCK IoStatus);
BOOLEAN MavUsb_SetDevicePowerState(IN PDEVICE_OBJECT DeviceObject,
                                   IN DEVICE_POWER_STATE DeviceState);
NTSTATUS MavUsb_QueryCapabilities(IN PDEVICE_OBJECT PdoDeviceObject,
                                  IN PDEVICE_CAPABILITIES DeviceCapabilities);

//
// From mavusb.c
//
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject,
                     IN PUNICODE_STRING RegistryPath);
NTSTATUS MavUsb_ProcessSysControlIrp(IN PDEVICE_OBJECT DeviceObject,
                                     IN PIRP Irp);
VOID MavUsb_Unload(IN PDRIVER_OBJECT DriverObject);
NTSTATUS MavUsb_SymbolicLink(IN PDEVICE_OBJECT DeviceObject,
                             IN OUT PUNICODE_STRING deviceLinkUnicodeString);
NTSTATUS MavUsb_CreateDeviceObject(IN PDRIVER_OBJECT DriverObject,
                                   IN PDEVICE_OBJECT PhysicalDeviceObject,
                                   IN PDEVICE_OBJECT *DeviceObject);
NTSTATUS MavUsb_CallUSBD(IN PDEVICE_OBJECT DeviceObject, IN PURB Urb);
NTSTATUS MavUsb_ConfigureDevice(IN PDEVICE_OBJECT DeviceObject);
NTSTATUS MavUsb_SelectInterface(IN PDEVICE_OBJECT DeviceObject,
                                IN PUSB_CONFIGURATION_DESCRIPTOR ConfigDesc);
NTSTATUS MavUsb_ResetPipe(IN PDEVICE_OBJECT DeviceObject,
                          IN PUSBD_PIPE_INFORMATION PipeInfo);
LONG MavUsb_DecrementIoCount(IN PDEVICE_OBJECT DeviceObject);
VOID MavUsb_IncrementIoCount(IN PDEVICE_OBJECT DeviceObject);

#endif

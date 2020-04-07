//****************************************************************************
//
// MAVUSB.C - Maverick(tm) USB Driver.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
// Copyright (c) 1997-1998 Microsoft Corporation.  All Rights Reserved.
//   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//   PURPOSE.
//
//****************************************************************************
#include "wdm.h"
#include "usbdi.h"
#include "usbdlib.h"
#include "mavusb.h"
#include "mavproto.h"
#include "mavguid.h"

//****************************************************************************
//
// Installable driver initialization entry point.  This entry point is called
// directly by the I/O system.
//
// Arguments:
//
//     DriverObject - pointer to the driver object.
//
//     RegistryPath - pointer to a unicode string representing the path to
//                    the driver-specific key in the registry.
//
// Return Value:
//
//     STATUS_SUCCESS if successful,
//     STATUS_UNSUCCESSFUL otherwise
//
//****************************************************************************
NTSTATUS
DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    //
    // Create dispatch points for create, close, unload.
    //
    DriverObject->MajorFunction[IRP_MJ_CREATE] = MavUsb_Create;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = MavUsb_Close;
    DriverObject->DriverUnload = MavUsb_Unload;

    //
    // User mode DeviceIoControl() calls will be routed here.
    //
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = MavUsb_ProcessIOCTL;

    //
    // User mode ReadFile()/WriteFile() calls will be routed here.
    //
    DriverObject->MajorFunction[IRP_MJ_WRITE] = MavUsb_Write;
    DriverObject->MajorFunction[IRP_MJ_READ] = MavUsb_Read;

    //
    // routines for handling system PnP and power management requests
    //
    DriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL] =
        MavUsb_ProcessSysControlIrp;
    DriverObject->MajorFunction[IRP_MJ_PNP] = MavUsb_ProcessPnPIrp;
    DriverObject->MajorFunction[IRP_MJ_POWER] = MavUsb_ProcessPowerIrp;

    //
    // The Functional Device Object (FDO) will not be created for PNP devices
    // until this routine is called upon device plug-in.
    //
    DriverObject->DriverExtension->AddDevice = MavUsb_PnPAddDevice;

    //
    // Return success.
    //
    return(STATUS_SUCCESS);
}

//****************************************************************************
//
// Main dispatch table routine for IRP_MJ_SYSTEM_CONTROL IRPs.  We basically
// just pass these down to the PDO.
//
// Arguments:
//
//     DeviceObject - pointer to FDO device object
//
//     Irp - pointer to an I/O Request Packet
//
// Return Value:
//
//     Status returned from lower driver
//
//****************************************************************************
NTSTATUS
MavUsb_ProcessSysControlIrp(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{

    PIO_STACK_LOCATION irpStack;
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    NTSTATUS waitStatus;
    PDEVICE_OBJECT stackDeviceObject;

    //
    // Set our status for this IRP to success.
    //
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    //
    // Get a pointer to the current location in the IRP stack.  This is where
    // the function codes and parameters are located.
    //
    irpStack = IoGetCurrentIrpStackLocation(Irp);

    //
    // Get a pointer to the device extension.
    //
    deviceExtension = DeviceObject->DeviceExtension;

    //
    // Get a pointer to the PDO.
    //
    stackDeviceObject = deviceExtension->TopOfStackDeviceObject;

    //
    // Increment the count of pending IRPs.
    //
    MavUsb_IncrementIoCount(DeviceObject);

    //
    // Copy this IRP to the next location in the IRP stack.
    //
    IoCopyCurrentIrpStackLocationToNext(Irp);

    //
    // Send this IRP to the PDO.
    //
    ntStatus = IoCallDriver(stackDeviceObject, Irp);

    //
    // Decrement the count of pending IRPs.
    //
    MavUsb_DecrementIoCount(DeviceObject);

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Free all the allocated resources, etc.
//
// Arguments:
//
//     DriverObject - pointer to a driver object
//
// Return Value:
//
//     None.
//
//****************************************************************************
VOID
MavUsb_Unload(IN PDRIVER_OBJECT DriverObject)
{
    //
    // Free any global resources allocated in DriverEntry.  We have few or none
    // because for a PNP device, almost all allocation is done in
    // PnpAddDevice() and all freeing while handling IRP_MN_REMOVE_DEVICE.
    //
}

//****************************************************************************
//
// This routine is called to create and initialize a GUID-based symbolic link
// to our device to be used to open/create instances of us from user mode.
//
// Called from MavUsb_CreateDeviceObject() to create the link.
//
// Arguments:
//
//     DeviceObject - pointer to our Physical Device Object (PDO)
//
//     deviceLinkUnicodeString - Points to a unicode string structure
//                               allocated by the caller.
//
// If this routine is successful, it initializes the unicode string and
// allocates the string buffer containing the kernel-mode path to the symbolic
// link for this device interface.
//
// Return Value:
//
//     STATUS_SUCCESS if successful,
//     STATUS_UNSUCCESSFUL otherwise
//
//****************************************************************************
NTSTATUS
MavUsb_SymbolicLink(IN PDEVICE_OBJECT DeviceObject,
                    IN OUT PUNICODE_STRING deviceLinkUnicodeString)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;


    //
    // IoRegisterDeviceInterface registers device functionality (a device
    // interface) that a driver will enable for use by applications or other
    // system components.
    //
    ntStatus = IoRegisterDeviceInterface(DeviceObject,
                                         (LPGUID)&GUID_CLASS_MAVERICK,
                                         NULL, deviceLinkUnicodeString);

    //
    // See if it was successful.
    //
    if(NT_SUCCESS(ntStatus))
    {
        //
        // IoSetDeviceInterfaceState enables or disables a previously
        // registered device interface.  Applications and other system
        // components can open only interfaces that are enabled.
        //
        ntStatus = IoSetDeviceInterfaceState(deviceLinkUnicodeString, TRUE);
    }

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Creates a Functional Device Object.
//
// Arguments:
//
//     DriverObject - pointer to the driver object for device
//
//     DeviceObject - pointer to DeviceObject pointer in which to return the
//                    created device object.
//
//     Instance - instance of the device create.
//
// Return Value:
//
//     STATUS_SUCCESS if successful,
//     STATUS_UNSUCCESSFUL otherwise
//
//****************************************************************************
NTSTATUS
MavUsb_CreateDeviceObject(IN PDRIVER_OBJECT DriverObject,
                          IN PDEVICE_OBJECT PhysicalDeviceObject,
                          IN PDEVICE_OBJECT *DeviceObject)
{
    NTSTATUS ntStatus;
    UNICODE_STRING deviceLinkUnicodeString;
    PDEVICE_EXTENSION deviceExtension;
    USHORT i;

    //
    // Create the symbolic link for our device.
    //
    ntStatus = MavUsb_SymbolicLink(PhysicalDeviceObject,
                                   &deviceLinkUnicodeString);

    //
    // See if we were able to create the symbolic link.
    //
    if(NT_SUCCESS(ntStatus))
    {
        //
        // Create the device.
        //
        ntStatus = IoCreateDevice(DriverObject, sizeof(DEVICE_EXTENSION),
                                  NULL, FILE_DEVICE_UNKNOWN,
                                  FILE_AUTOGENERATED_DEVICE_NAME,
                                  FALSE, DeviceObject);

        //
        // See if we were able to create the device.
        //
        if(NT_SUCCESS(ntStatus))
        {
            //
            // Get a pointer to the device extension.
            //
            deviceExtension = (*DeviceObject)->DeviceExtension;
        }
        else
        {
            //
            // We could not create the device, so return the failure.
            //
            return(ntStatus);
        }

        //
        // Save the name of our functional device object link.  The name is
        // generated based on the driver's class GUID.
        //
        RtlCopyMemory(deviceExtension->DeviceLinkNameBuffer,
                      deviceLinkUnicodeString.Buffer,
                      deviceLinkUnicodeString.Length);

        //
        // This event is triggered when there is no pending IO of any kind and
        // device is removed.
        //
        KeInitializeEvent(&deviceExtension->RemoveEvent, NotificationEvent,
                          FALSE);

        //
        // This event is triggered when self-requested power irps complete.
        //
        KeInitializeEvent(&deviceExtension->SelfRequestedPowerIrpEvent,
                          NotificationEvent, FALSE);

        //
        // This event is triggered when there is no pending IO (pending IO
        // count == 1)
        //
        KeInitializeEvent(&deviceExtension->NoPendingIoEvent,
                          NotificationEvent, FALSE);

        //
        // This spinlock used to protect increment/decrement IO count logic.
        //
        KeInitializeSpinLock(&deviceExtension->IoCountSpinLock);

        //
        // Free buffer from unicode string we used to initialize interface.
        //
        RtlFreeUnicodeString(&deviceLinkUnicodeString);
    }

    //
    // Return the status.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Passes a URB to the USBD class driver.  The client device driver passes USB
// request block (URB) structures to the class driver as a parameter in an IRP
// with Irp->MajorFunction set to IRP_MJ_INTERNAL_DEVICE_CONTROL and the next
// IRP stack location Parameters.DeviceIoControl.IoControlCode field set to
// IOCTL_INTERNAL_USB_SUBMIT_URB.
//
// Arguments:
//
//     DeviceObject - pointer to the physical device object (PDO)
//
//     Urb - pointer to an already-formatted Urb request block
//
// Return Value:
//
//     STATUS_SUCCESS if successful,
//     STATUS_UNSUCCESSFUL otherwise
//
//****************************************************************************
NTSTATUS
MavUsb_CallUSBD(IN PDEVICE_OBJECT DeviceObject, IN PURB Urb)
{
    NTSTATUS ntStatus, status = STATUS_SUCCESS;
    PDEVICE_EXTENSION deviceExtension;
    PIRP irp;
    KEVENT event;
    IO_STATUS_BLOCK ioStatus;
    PIO_STACK_LOCATION nextStack;

    //
    // Get a pointer to the device extension.
    //
    deviceExtension = DeviceObject->DeviceExtension;

    //
    // Initialize the completion event.
    //
    KeInitializeEvent(&event, NotificationEvent, FALSE);

    //
    // Build the IRP.
    //
    irp =
        IoBuildDeviceIoControlRequest(IOCTL_INTERNAL_USB_SUBMIT_URB,
                                      deviceExtension->TopOfStackDeviceObject,
                                      NULL, 0, NULL, 0, TRUE, &event,
                                      &ioStatus);

    //
    // As an alternative, we could call KeDelayExecutionThread, wait for some
    // period of time, and try again....but we keep it simple for right now.
    //
    if(!irp)
    {
        return(STATUS_INSUFFICIENT_RESOURCES);
    }

    //
    // Get the next location on the IRP stack.
    //
    nextStack = IoGetNextIrpStackLocation(irp);

    //
    // Place the URB in the arguments of the next location in the IRP stack.
    //
    nextStack->Parameters.Others.Argument1 = Urb;

    //
    // Send the IRP to the PDO.
    //
    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, irp);

    //
    // See if the status is pending.
    //
    if(ntStatus == STATUS_PENDING)
    {
        //
        // The status is pending, so wait for the IRP to complete.
        //
        KeWaitForSingleObject(&event, Suspended, KernelMode, FALSE, NULL);
    }
    else
    {
        //
        // Set the status in the IO block.
        //
        ioStatus.Status = ntStatus;
    }

    //
    // USBD maps the error code for us.
    //
    ntStatus = ioStatus.Status;

    //
    // Return the status.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Initializes a given instance of the device on the USB and selects and saves
// the configuration.
//
// Arguments:
//
//     DeviceObject - pointer to the physical device object for this instance
//                    of the device.
//
// Return Value:
//
//     NT status code
//
//****************************************************************************
NTSTATUS
MavUsb_ConfigureDevice(IN PDEVICE_OBJECT DeviceObject)
{
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus;
    PURB urb;
    ULONG siz;

    //
    // Get a pointer to the device extension.
    //
    deviceExtension = DeviceObject->DeviceExtension;

    //
    // The device has no pending staged IO transactions.
    //
    deviceExtension->StagedIoPending = FALSE;

    //
    // Allocate memory for a URB.
    //
    urb = ExAllocatePool(NonPagedPool,
                         sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));
    if(!urb)
    {
        return(STATUS_INSUFFICIENT_RESOURCES);
    }

    //
    // When USB_CONFIGURATION_DESCRIPTOR_TYPE is specified for DescriptorType
    // in a call to UsbBuildGetDescriptorRequest(), all interface, endpoint,
    // class-specific, and vendor-specific descriptors for the configuration
    // also are retrieved.  The caller must allocate a buffer large enough to
    // hold all of this information or the data is truncated without error.
    // Therefore the 'siz' set below is just a 'good guess', and we may have to
    // retry.
    //
    siz = sizeof(USB_CONFIGURATION_DESCRIPTOR) + 512;

    //
    // We will break out of this 'retry loop' when
    // UsbBuildGetDescriptorRequest() has a big enough
    // deviceExtension->UsbConfigurationDescriptor buffer not to truncate.
    //
    while(1)
    {
        //
        // Allocate memory for the configuration descriptor.
        //
        deviceExtension->UsbConfigurationDescriptor =
            ExAllocatePool(NonPagedPool, siz);
        if(!deviceExtension->UsbConfigurationDescriptor)
        {
            ExFreePool(urb);
            return(STATUS_INSUFFICIENT_RESOURCES);
        }

        //
        // Build the get descriptor URB.
        //
        UsbBuildGetDescriptorRequest(urb,
                                     (USHORT)sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                                     USB_CONFIGURATION_DESCRIPTOR_TYPE,
                                     0,
                                     0,
                                     deviceExtension->UsbConfigurationDescriptor,
                                     NULL,
                                     siz,
                                     NULL);

        //
        // Send the URB to the PDO.
        //
        ntStatus = MavUsb_CallUSBD(DeviceObject, urb);

        //
        // If we got some data see if it was enough.  NOTE: we may get an error
        // in URB because of buffer overrun.
        //
        if((urb->UrbControlDescriptorRequest.TransferBufferLength > 0) &&
           (deviceExtension->UsbConfigurationDescriptor->wTotalLength > siz))
        {
            //
            // Update size based on the actual size of the configuration
            // descriptor.
            //
            siz = deviceExtension->UsbConfigurationDescriptor->wTotalLength;

            //
            // Free the currently allocated memory for the configuration
            // descriptor.
            //
            ExFreePool(deviceExtension->UsbConfigurationDescriptor);
            deviceExtension->UsbConfigurationDescriptor = NULL;
        }
        else
        {
            //
            // We have all the data.
            //
            break;
        }
    }

    //
    // Free up the memory for the URB.
    //
    ExFreePool(urb);

    //
    // We have the configuration descriptor for the configuration we want.
    // Now, we issue the select configuration command to get the pipes
    // associated with this configuration.
    //
    ntStatus =
        MavUsb_SelectInterface(DeviceObject,
                               deviceExtension->UsbConfigurationDescriptor);

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Initializes the device with (possibly) multiple interfaces.  This
// minidriver only supports one interface (with multiple endpoints).
//
// Arguments:
//
//     DeviceObject - pointer to the device object for this instance of the
//                    device.
//
//     ConfigDescriptor - pointer to the USB configuration descriptor
//                        containing the interface and endpoint descriptors.
//
// Return Value:
//
//     NT status code
//
//****************************************************************************
NTSTATUS
MavUsb_SelectInterface(IN PDEVICE_OBJECT DeviceObject,
                       IN PUSB_CONFIGURATION_DESCRIPTOR ConfigDesc)
{
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus;
    PURB urb = NULL;
    ULONG i;
    PUSB_INTERFACE_DESCRIPTOR interfaceDescriptor = NULL;
    PUSBD_INTERFACE_INFORMATION Interface = NULL;
    USHORT siz;

    //
    // Get a pointer to the device extension.
    //
    deviceExtension = DeviceObject->DeviceExtension;

    //
    // The Maverick driver only supports one interface, we must parse the
    // configuration descriptor for the interface and remember the pipes.
    //
    urb = USBD_CreateConfigurationRequest(ConfigDesc, &siz);

    //
    // See if USBD was able to allocate a URB.
    //
    if(urb)
    {
        //
        // USBD_ParseConfigurationDescriptorEx searches a given configuration
        // descriptor and returns a pointer to an interface that matches the
        // given search criteria.  We only support one interface on this
        // device.
        //
        interfaceDescriptor =
            USBD_ParseConfigurationDescriptorEx(ConfigDesc, ConfigDesc, -1, -1,
                                                -1, -1, -1);

        //
        // Make sure we found an interface.
        //
        if(!interfaceDescriptor)
        {
            //
            // We could not find an interface.
            //
            ExFreePool(urb);
            return(STATUS_INSUFFICIENT_RESOURCES);
        }

        //
        // Get a pointer to the interface information structure in the URB.
        //
        Interface = &urb->UrbSelectConfiguration.Interface;

        //
        // Loop through all the pipes on the device.
        //
        for(i = 0; i < Interface->NumberOfPipes; i++)
        {
            //
            // Perform any pipe initialization here.  We set the max transfer
            // size and any pipe flags we use; USBD sets the rest of the
            // interface struct members.
            //
            Interface->Pipes[i].MaximumTransferSize = 16384;
        }

        //
        // Build the select configuration URB.
        //
        UsbBuildSelectConfigurationRequest(urb, (USHORT)siz, ConfigDesc);

        //
        // Send the URB to the PDO.
        //
        ntStatus = MavUsb_CallUSBD(DeviceObject, urb);
    }
    else
    {
        //
        // We could not allocate the URB.
        //
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // See if the select configuration was successful.
    //
    if(NT_SUCCESS(ntStatus))
    {
        //
        // Save the configuration handle for this device.
        //
        deviceExtension->UsbConfigurationHandle =
            urb->UrbSelectConfiguration.ConfigurationHandle;

        //
        // Allocate memory for our saved copy of the interface structure.
        //
        deviceExtension->UsbInterface = ExAllocatePool(NonPagedPool,
                                                       Interface->Length);

        //
        // See if we were able to allocate memory for the interface structure.
        //
        if(deviceExtension->UsbInterface)
        {
            //
            // Save a copy of the interface information returned.
            //
            RtlCopyMemory(deviceExtension->UsbInterface, Interface,
                          Interface->Length);
        }
    }

    //
    // If the URB was allocated, then free it now.
    //
    if(urb)
    {
        ExFreePool(urb);
    }

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Reset a given USB pipe.
//
// NOTE:  This will reset the host to Data0 and should also reset the device
// to Data0.
//
// Arguments:
//
//     Ptrs to our FDO and a USBD_PIPE_INFORMATION struct
//
// Return Value:
//
//     NT status code
//
//****************************************************************************
NTSTATUS
MavUsb_ResetPipe(IN PDEVICE_OBJECT DeviceObject,
                 IN PUSBD_PIPE_INFORMATION PipeInfo)
{
    NTSTATUS ntStatus;
    PURB urb;
    PDEVICE_EXTENSION deviceExtension;

    //
    // Get a pointer to the device extension.
    //
    deviceExtension = DeviceObject->DeviceExtension;

    //
    // Allocate memory for the URB.
    //
    urb = ExAllocatePool(NonPagedPool, sizeof(struct _URB_PIPE_REQUEST));

    //
    // See if we were able to allocate memory for the URB.
    //
    if(urb)
    {
        //
        // Fill in the URB.
        //
        urb->UrbHeader.Length = (USHORT)sizeof(struct _URB_PIPE_REQUEST);
        urb->UrbHeader.Function = URB_FUNCTION_RESET_PIPE;
        urb->UrbPipeRequest.PipeHandle = PipeInfo->PipeHandle;

        //
        // Send the URB to the PDO.
        //
        ntStatus = MavUsb_CallUSBD(DeviceObject, urb);

        //
        // Free the memory for the URB.
        //
        ExFreePool(urb);
    }
    else
    {
        //
        // We were unable to allocate memory for the URB.
        //
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// We keep a pending IO count (extension->PendingIoCount) in the device
// extension.
//
// The first increment of this count is done on adding the device.
// Subsequently, the count is incremented for each new IRP received and
// decremented when each IRP is completed or passed on.
//
// Transition to 'one' therefore indicates no IO is pending and signals
// deviceExtension->NoPendingIoEvent.  This is needed for processing
// IRP_MN_QUERY_REMOVE_DEVICE.
//
// Transition to 'zero' signals an event (deviceExtension->RemoveEvent) to
// enable device removal.  This is used in processing for
// IRP_MN_REMOVE_DEVICE.
//
// Arguments:
//
//     DeviceObject - ptr to our FDO
//
// Return Value:
//
//     deviceExtension->PendingIoCount
//
//****************************************************************************
LONG
MavUsb_DecrementIoCount(IN PDEVICE_OBJECT DeviceObject)
{
    PDEVICE_EXTENSION deviceExtension;
    LONG ioCount;
    KIRQL             oldIrql;

    //
    // Get a pointer to our device extension.
    //
    deviceExtension = DeviceObject->DeviceExtension;

    //
    // Acquire the spin lock which protects the pending IO count.
    //
    KeAcquireSpinLock(&deviceExtension->IoCountSpinLock, &oldIrql);

    //
    // Decrement the pending IO count.
    //
    ioCount = InterlockedDecrement(&deviceExtension->PendingIoCount);

    //
    // See if the IO count is now one.
    //
    if(ioCount == 1)
    {
        //
        // Trigger the no pending IO event.
        //
        KeSetEvent(&deviceExtension->NoPendingIoEvent, 1, FALSE);
    }

    //
    // See if the IO count is now zero.
    //
    if(ioCount == 0)
    {
        //
        // Trigger the remove device event.
        //
        KeSetEvent(&deviceExtension->RemoveEvent, 1, FALSE);
    }

    //
    // Release the spin lock.
    //
    KeReleaseSpinLock(&deviceExtension->IoCountSpinLock, oldIrql);

    //
    // Return the new pending IO count.
    //
    return(ioCount);
}

//****************************************************************************
//
// We keep a pending IO count (extension->PendingIoCount) in the device
// extension.
//
// The first increment of this count is done on adding the device.
// Subsequently, the count is incremented for each new IRP received and
// decremented when each IRP is completed or passed on.
//
// Arguments:
//
//     DeviceObject - ptr to our FDO
//
// Return Value:
//
//     None
//
//****************************************************************************
VOID
MavUsb_IncrementIoCount(IN PDEVICE_OBJECT DeviceObject)
{
    PDEVICE_EXTENSION deviceExtension;
    KIRQL             oldIrql;

    //
    // Get a pointer to the device extension.
    //
    deviceExtension = DeviceObject->DeviceExtension;

    //
    // Acquire the spin lock which protects the pending IO count.
    //
    KeAcquireSpinLock(&deviceExtension->IoCountSpinLock, &oldIrql);

    //
    // Increment the pending IO count.
    //
    InterlockedIncrement(&deviceExtension->PendingIoCount);

    //
    // Release the spin lock.
    //
    KeReleaseSpinLock(&deviceExtension->IoCountSpinLock, oldIrql);
}

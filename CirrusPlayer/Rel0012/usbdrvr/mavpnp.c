//****************************************************************************
//
// MAVPNP.C - Routines to handle Plug-N-Play IRPs.
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

//****************************************************************************
//
// Dispatch table routine for IRP_MJ_PNP.  Process the Plug and Play IRPs sent
// to this device.
//
// Arguments:
//
//     DeviceObject - pointer to our FDO (Functional Device Object)
//
//     Irp - pointer to an I/O Request Packet
//
// Return Value:
//
//     NT status code
//
//****************************************************************************
NTSTATUS
MavUsb_ProcessPnPIrp(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    PIO_STACK_LOCATION irpStack;
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_OBJECT stackDeviceObject;
    KEVENT startDeviceEvent;

    //
    // Get a pointer to the device extension.
    //
    deviceExtension = DeviceObject->DeviceExtension;

    //
    // Get a pointer to the current location in the IRP stack.  This is where
    // the function codes and parameters are located.
    //
    irpStack = IoGetCurrentIrpStackLocation(Irp);

    //
    // Get a pointer to the PDO.
    //
    stackDeviceObject = deviceExtension->TopOfStackDeviceObject;

    //
    // Increment the count of pending IRPs.
    //
    MavUsb_IncrementIoCount(DeviceObject);

    //
    // Determine what to do based on the minor function in the IRP.
    //
    switch(irpStack->MinorFunction)
    {
        //
        // The PnP Manager sends this IRP after it has assigned resources, if
        // any, to the device.  The device may have been recently enumerated
        // and is being started for the first time, or the device may be
        // restarting after being stopped for resource reconfiguration.
        //
        case IRP_MN_START_DEVICE:
        {
            //
            // Initialize an event we can wait on for the PDO to be done with
            // this IRP.
            //
            KeInitializeEvent(&startDeviceEvent, NotificationEvent, FALSE);
            IoCopyCurrentIrpStackLocationToNext(Irp);

            //
            // Set a completion routine so it can signal our event when the PDO
            // is done with the IRP.
            //
            IoSetCompletionRoutine(Irp, MavUsb_IrpCompletionRoutine,
                                   &startDeviceEvent, TRUE, TRUE, TRUE);

            //
            // Pass the IRP to the PDO.
            //
            ntStatus = IoCallDriver(stackDeviceObject, Irp);

            //
            // If PDO is not done yet, wait for the event to be set in our
            // completion routine.
            //
            if(ntStatus == STATUS_PENDING)
            {
                //
                // Wait for IRP to complete.
                //
                KeWaitForSingleObject(&startDeviceEvent, Suspended, KernelMode,
                                      FALSE, NULL);

                //
                // Get the status from the IRP.
                //
                ntStatus = Irp->IoStatus.Status;
            }

            //
            // See if the PDO successfully handled the IRP.
            //
            if(NT_SUCCESS(ntStatus))
            {
                //
                // Now we're ready to do our own startup processing.  USB
                // client drivers such as us set up URBs (USB Request Packets)
                // to send requests to the host controller driver (HCD).  The
                // URB structure defines a format for all possible commands
                // that can be sent to a USB device.  Here, we request the
                // device descriptor and store it, and configure the device.
                //
                ntStatus = MavUsb_StartDevice(DeviceObject);

                //
                // Set the status in the IRP.
                //
                Irp->IoStatus.Status = ntStatus;
            }

            //
            // Complete the IRP.
            //
            IoCompleteRequest(Irp, IO_NO_INCREMENT);

            //
            // Decrement the count of pending requests.
            //
            MavUsb_DecrementIoCount(DeviceObject);

            //
            // Return the result.
            //
            return(ntStatus);
        }

        //
        // The IRP_MN_QUERY_STOP_DEVICE/IRP_MN_STOP_DEVICE sequence only occurs
        // during "polite" shutdowns, such as the user explicitily requesting
        // the service be stopped in, or requesting unplug from the PnP tray
        // icon.  This sequence is NOT received during "impolite" shutdowns,
        // such as someone suddenly yanking the USB cord or otherwise
        // unexpectedly disabling/resetting the device.
        //
        // If a driver sets STATUS_SUCCESS for this IRP, the driver must not
        // start any operations on the device that would prevent that driver
        // from successfully completing an IRP_MN_STOP_DEVICE for the device.
        //
        // For mass storage devices such as disk drives, while the device is in
        // the stop-pending state, the driver holds IRPs that require access to
        // the device, but for most USB devices, there is no 'persistent
        // storage', so we will just refuse any more IO until restarted or the
        // stop is cancelled.
        //
        // If a driver in the device stack determines that the device cannot be
        // stopped for resource reconfiguration, the driver is not required to
        // pass the IRP down the device stack.  If a query-stop IRP fails, the
        // PnP Manager sends an IRP_MN_CANCEL_STOP_DEVICE to the device stack,
        // notifying the drivers for the device that the query has been
        // cancelled and that the device will not be stopped.
        //
        case IRP_MN_QUERY_STOP_DEVICE:
        {
            //
            // It is possible to receive this IRP when the device has not been
            // started (as on a boot device).
            //
            if(!deviceExtension->DeviceStarted)
            {
                //
                // If we've never been started, just pass it on.
                //
                IoSkipCurrentIrpStackLocation(Irp);
                ntStatus =
                    IoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);

                //
                // Decrement the count of pending IRPs.
                //
                MavUsb_DecrementIoCount(DeviceObject);

                //
                // Return the status.
                //
                return(ntStatus);
            }

            //
            // Fail the request if we have any read/write IRPs pending.
            //
            if(deviceExtension->StagedIoPending)
            {
                ntStatus = STATUS_UNSUCCESSFUL;
            }
            else
            {
                //
                // We'll not veto it; pass it on and flag that stop was
                // requested.  Once StopDeviceRequested is set no new IOCTL or
                // read/write IRPs will be passed down the stack to lower
                // drivers; all will be quickly failed.
                //
                deviceExtension->StopDeviceRequested = TRUE;

                //
                // Set the IRP status to success.
                //
                Irp->IoStatus.Status = STATUS_SUCCESS;
            }

            //
            // We're done with this IRP.
            //
            break;
        }

        //
        // The PnP Manager uses this IRP to inform the drivers for a device
        // that the device will not be stopped for resource reconfiguration.
        // This should only be received after a successful
        // IRP_MN_QUERY_STOP_DEVICE.
        //
        case IRP_MN_CANCEL_STOP_DEVICE:
        {
            //
            // It is possible to receive this IRP when the device has not been
            // started.
            //
            if(!deviceExtension->DeviceStarted)
            {
                //
                // If we've never been started, just pass it on.
                //
                IoSkipCurrentIrpStackLocation(Irp);
                ntStatus =
                    IoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);

                //
                // Decrement the count of pending IRPs.
                //
                MavUsb_DecrementIoCount(DeviceObject);

                //
                // Return the status.
                //
                return(ntStatus);
            }

            //
            // Reset this flag so new IOCTL and IO IRP processing will be
            // re-enabled.
            //
            deviceExtension->StopDeviceRequested = FALSE;

            //
            // Set the IRP status to success.
            //
            Irp->IoStatus.Status = STATUS_SUCCESS;

            //
            // We're done with this IRP.
            //
            break;
        }

        //
        // The PnP Manager sends this IRP to stop a device so it can
        // reconfigure its hardware resources.  The PnP Manager only sends this
        // IRP if a prior IRP_MN_QUERY_STOP_DEVICE completed successfully.
        //
        case IRP_MN_STOP_DEVICE:
        {
            //
            // Send the select configuration urb with a NULL pointer for the
            // configuration handle.  This closes the configuration and puts
            // the device in the 'unconfigured' state.
            //
            ntStatus = MavUsb_StopDevice(DeviceObject);

            //
            // Set the status in the IRP.
            //
            Irp->IoStatus.Status = ntStatus;

            //
            // We're done with this IRP.
            //
            break;
        }

        //
        // In response to this IRP, drivers indicate whether the device can be
        // removed without disrupting the system.
        //
        // If a driver determines it is safe to remove the device, the driver
        // completes any outstanding I/O requests, arranges to hold any
        // subsequent read/write requests, and sets Irp->IoStatus.Status to
        // STATUS_SUCCESS.  Function and filter drivers then pass the IRP to
        // the next-lower driver in the device stack.  The underlying bus
        // driver calls IoCompleteRequest.
        //
        // If a driver sets STATUS_SUCCESS for this IRP, the driver must not
        // start any operations on the device that would prevent that driver
        // from successfully completing an IRP_MN_REMOVE_DEVICE for the device.
        // If a driver in the device stack determines that the device cannot be
        // removed, the driver is not required to pass the query-remove IRP
        // down the device stack.  If a query-remove IRP fails, the PnP Manager
        // sends an IRP_MN_CANCEL_REMOVE_DEVICE to the device stack, notifying
        // the drivers for the device that the query has been cancelled and
        // that the device will not be removed.
        //
        case IRP_MN_QUERY_REMOVE_DEVICE:
        {
            //
            // It is possible to receive this IRP when the device has not been
            // started.
            //
            if(!deviceExtension->DeviceStarted)
            {
                //
                // If we've never been started, just pass it on.
                //
                IoSkipCurrentIrpStackLocation(Irp);
                ntStatus =
                    IoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);

                //
                // Decrement the count of pending IRPs.
                //
                MavUsb_DecrementIoCount(DeviceObject);

                //
                // Return the status.
                //
                return(ntStatus);
            }

            //
            // Once RemoveDeviceRequested is set no new IOCTL or read/write
            // IRPs will be passed down the stack to lower drivers; all will be
            // quickly failed.
            //
            deviceExtension->RemoveDeviceRequested = TRUE;

            //
            // Wait for any io request pending in our driver to complete before
            // returning success.  This event is set when
            // deviceExtension->PendingIoCount goes to 1.
            //
            KeWaitForSingleObject(&deviceExtension->NoPendingIoEvent,
                                  Suspended, KernelMode, FALSE, NULL);

            //
            // Set the IRP status to success.
            //
            Irp->IoStatus.Status = STATUS_SUCCESS;

            //
            // We're done with this IRP.
            //
            break;
        }

        //
        // The PnP Manager uses this IRP to inform the drivers for a device
        // that the device will not be removed.
        //
        case IRP_MN_CANCEL_REMOVE_DEVICE:
        {
            //
            // It is sent only after a successful IRP_MN_QUERY_REMOVE_DEVICE.
            //
            if(!deviceExtension->DeviceStarted)
            {
                //
                // If we've never been started, just pass it on.
                //
                IoSkipCurrentIrpStackLocation(Irp);
                ntStatus =
                    IoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);

                //
                // Decrement the count of pending IRPs.
                //
                MavUsb_DecrementIoCount(DeviceObject);

                //
                // Return the status.
                //
                return(ntStatus);
            }

            //
            // Reset this flag so new IOCTL and IO IRP processing will be
            // re-enabled.
            //
            deviceExtension->RemoveDeviceRequested = FALSE;

            //
            // Set the IRP status to success.
            //
            Irp->IoStatus.Status = STATUS_SUCCESS;

            //
            // We're done with this IRP.
            //
            break;
        }

        //
        // For a surprise-style device removal (i.e. sudden cord yank), the
        // physical device has already been removed so the PnP Manager sends
        // the remove IRP without a prior query-remove.  A device can be in any
        // state when it receives a remove IRP as a result of a surprise-style
        // removal.
        //
        case IRP_MN_SURPRISE_REMOVAL:
        {
            //
            // Match the increment at the begining of the dispatch routine.
            //
            MavUsb_DecrementIoCount(DeviceObject);

            //
            // Once DeviceRemoved is set no new IOCTL or read/write IRPs will
            // be passed down the stack to lower drivers; all will be quickly
            // failed.
            //
            deviceExtension->DeviceRemoved = TRUE;

            //
            // If any pipes are still open, call USBD with
            // URB_FUNCTION_ABORT_PIPE.  This call will also close the pipes;
            // if any user close calls get through, they will be noops.
            //
            MavUsb_AbortPipes(DeviceObject);

            //
            // Set the IRP status to success.
            //
            Irp->IoStatus.Status = STATUS_SUCCESS;

            //
            // We don't explicitly wait for the below driver to complete, but
            // just make the call and go on, finishing cleanup.
            //
            IoCopyCurrentIrpStackLocationToNext(Irp);
            ntStatus = IoCallDriver(stackDeviceObject, Irp);

            //
            // Return the status.
            //
            return(ntStatus);
        }

        //
        // The PnP Manager uses this IRP to direct drivers to remove a device.
        // For a "polite" device removal, the PnP Manager sends an
        // IRP_MN_QUERY_REMOVE_DEVICE prior to the remove IRP.  In this case,
        // the device is in the remove-pending state when the remove IRP
        // arrives.  For a surprise-style device removal (i.e. sudden cord
        // yank), the physical device has already been removed and the PnP
        // Manager may not have sent IRP_MN_SURPRISE_REMOVAL.  A device can be
        // in any state when it receives a remove IRP as a result of a
        // surprise-style removal.
        //
        case IRP_MN_REMOVE_DEVICE:
        {
            //
            // Match the increment at the begining of the dispatch routine.
            //
            MavUsb_DecrementIoCount(DeviceObject);

            //
            // Once DeviceRemoved is set no new IOCTL or read/write IRPs will
            // be passed down the stack to lower drivers; all will be quickly
            // failed.
            //
            deviceExtension->DeviceRemoved = TRUE;

            //
            // If any pipes are still open, call USBD with
            // URB_FUNCTION_ABORT_PIPE.  This call will also close the pipes;
            // if any user close calls get through, they will be noops.
            //
            MavUsb_AbortPipes(DeviceObject);

            //
            // We don't explicitly wait for the below driver to complete, but
            // just make the call and go on, finishing cleanup.
            //
            IoCopyCurrentIrpStackLocationToNext(Irp);
            ntStatus = IoCallDriver(stackDeviceObject, Irp);

            //
            // The final decrement to device extension PendingIoCount == 0
            // will set deviceExtension->RemoveEvent, enabling device removal.
            // If there is no pending IO at this point, the below decrement
            // will be it.
            //
            MavUsb_DecrementIoCount(DeviceObject);

            //
            // Wait for any io request pending in our driver to complete before
            // finishing the remove.
            //
            KeWaitForSingleObject(&deviceExtension->RemoveEvent, Suspended,
                                  KernelMode, FALSE, NULL);

            //
            // Delete the link and FDO we created.
            //
            MavUsb_RemoveDevice(DeviceObject);

            //
            // Detach from the PDO.
            //
            IoDetachDevice(deviceExtension->TopOfStackDeviceObject);

            //
            // Delete our FDO.
            //
            IoDeleteDevice(DeviceObject);

            //
            // Return the status.
            //
            return(ntStatus);
        }

        //
        // Handle all other IRPs.
        //
        default:
        {
            //
            // In this case we must not touch the status. As ntstatus is
            // STATUS_SUCCESS, we will skip the failure case and pass down the
            // IRP untouched.
            //
        }
    }

    //
    // Handle any error conditions.
    //
    if(!NT_SUCCESS(ntStatus))
    {
        //
        // Set the status in the IRP.
        //
        Irp->IoStatus.Status = ntStatus;

        //
        // Complete the IRP without passing it down.
        //
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        //
        // Decrement the count of pending IRPs.
        //
        MavUsb_DecrementIoCount(DeviceObject);

        //
        // Return the status.
        //
        return(ntStatus);
    }

    //
    // Prepare to send the IRP down by copying it to the next location on the
    // IRP stack.
    //
    IoCopyCurrentIrpStackLocationToNext(Irp);

    //
    // All PNP_POWER messages get passed to the TopOfStackDeviceObject we were
    // given in PnPAddDevice
    //
    ntStatus = IoCallDriver(stackDeviceObject, Irp);

    //
    // Decrement the count of pending IRPs.
    //
    MavUsb_DecrementIoCount(DeviceObject);

    //
    // Return the status.
    //
    return(ntStatus);
}

//****************************************************************************
//
// This routine is called to create and initialize our Functional Device
// Object (FDO).  For monolithic drivers, this is done in DriverEntry(), but
// Plug and Play devices wait for a PnP event.
//
// Arguments:
//
//     DriverObject - pointer to the driver object for this instance
//
//     PhysicalDeviceObject - pointer to a device object created by the bus
//
// Return Value:
//
//     STATUS_SUCCESS if successful,
//     STATUS_UNSUCCESSFUL otherwise
//
//****************************************************************************
NTSTATUS
MavUsb_PnPAddDevice(IN PDRIVER_OBJECT DriverObject,
             IN PDEVICE_OBJECT PhysicalDeviceObject)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_OBJECT deviceObject = NULL;
    PDEVICE_EXTENSION deviceExtension;
    USBD_VERSION_INFORMATION versionInformation;
    ULONG i;

    //
    // Create our funtional device object (FDO).
    //
    ntStatus = MavUsb_CreateDeviceObject(DriverObject, PhysicalDeviceObject,
                                         &deviceObject);
    if(NT_SUCCESS(ntStatus))
    {
        //
        // Get the pointer to the device extension.
        //
        deviceExtension = deviceObject->DeviceExtension;

        //
        // We support direct io for read/write.
        //
        deviceObject->Flags |= DO_DIRECT_IO;

        //
        // Set this flag causes the driver to not receive a IRP_MN_STOP_DEVICE
        // during suspend and also not get an IRP_MN_START_DEVICE during
        // resume.  This is neccesary because during the start device call,
        // the GetDescriptors() call will be failed by the USB stack.
        //
        deviceObject->Flags |= DO_POWER_PAGABLE;

        //
        // Initialize our device extension.  Remember the physical device
        // object.
        //
        deviceExtension->PhysicalDeviceObject = PhysicalDeviceObject;

        //
        // Attach to the PDO.
        //
        deviceExtension->TopOfStackDeviceObject =
            IoAttachDeviceToDeviceStack(deviceObject, PhysicalDeviceObject);

        //
        // Get a copy of the physical device's capabilities into a
        // DEVICE_CAPABILITIES struct in our device extension;
        // We are most interested in learning which system power states
        // are to be mapped to which device power states for handling
        // IRP_MJ_SET_POWER IRPs.
        //
        MavUsb_QueryCapabilities(deviceExtension->TopOfStackDeviceObject,
                                 &deviceExtension->DeviceCapabilities);

        //
        // We want to determine what level to auto-powerdown to; this is the
        // lowest sleeping level that is LESS than D3; if all are set to D3,
        // auto powerdown/powerup will be disabled.
        //
        deviceExtension->PowerDownLevel = PowerDeviceUnspecified;
        for(i = PowerSystemSleeping1; i <= PowerSystemSleeping3; i++)
        {
            if(deviceExtension->DeviceCapabilities.DeviceState[i] <
               PowerDeviceD3)
            {
                deviceExtension->PowerDownLevel =
                    deviceExtension->DeviceCapabilities.DeviceState[i];
            }
        }

        //
        // We keep a pending IO count (extension->PendingIoCount) in the
        // device extension.  The first increment of this count is done on
        // adding the device.  Subsequently, the count is incremented for each
        // new IRP received and decremented when each IRP is completed or
        // passed on.
        //
        // Transition to 'one' therefore indicates no IO is pending and signals
        // deviceExtension->NoPendingIoEvent. This is needed for processing
        // IRP_MN_QUERY_REMOVE_DEVICE
        //
        // Transition to 'zero' signals an event
        // (deviceExtension->RemoveEvent) to enable device removal. This is
        // used in processing for IRP_MN_REMOVE_DEVICE
        //
        MavUsb_IncrementIoCount(deviceObject);
    }

    //
    // Get the version of the USB driver.
    //
    USBD_GetUSBDIVersion(&versionInformation);
    if(NT_SUCCESS(ntStatus))
    {
        NTSTATUS actStat;

        //
        // Try to power down device until IO actually requested.
        //
        actStat = MavUsb_SelfSuspendOrActivate(deviceObject, TRUE);

        //
        // Inidicte that we are done initializing the device.
        //
        deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
    }

    //
    // Return the status.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Called from MavUsb_ProcessPnPIrp(), the dispatch routine for IRP_MJ_PNP.
// Initializes a given instance of the device on the USB.  USB client drivers
// such as us set up URBs (USB Request Packets) to send requests to the host
// controller driver (HCD). The URB structure defines a format for all
// possible commands that can be sent to a USB device.  Here, we request the
// device descriptor and store it, and configure the device.
//
// Arguments:
//
//     DeviceObject - pointer to the FDO (Functional Device Object)
//
// Return Value:
//
//     NT status code
//
//****************************************************************************
NTSTATUS
MavUsb_StartDevice(IN PDEVICE_OBJECT DeviceObject)
{
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus;
    PUSB_DEVICE_DESCRIPTOR deviceDescriptor = NULL;
    PURB urb;
    ULONG siz1, siz2;

    //
    // Get the pointer to the device extension.
    //
    deviceExtension = DeviceObject->DeviceExtension;

    //
    // Allocate memory from the non-paged pool for a URB.
    //
    siz1 = sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST);
    urb = ExAllocatePool(NonPagedPool, siz1);

    //
    // Make sure we were able to allocate memory.
    //
    if(urb)
    {
        //
        // Allocate memory from the non-paged pool for the device descriptor.
        //
        siz2 = sizeof(USB_DEVICE_DESCRIPTOR);
        deviceDescriptor = ExAllocatePool(NonPagedPool, siz2);

        //
        // Make sure we were able to allocate memory.
        //
        if(deviceDescriptor)
        {
            //
            // Build the get descriptor request in the URB.
            //
            UsbBuildGetDescriptorRequest(urb, (USHORT)siz1,
                                         USB_DEVICE_DESCRIPTOR_TYPE, 0, 0,
                                         deviceDescriptor, NULL, siz2, NULL);

            //
            // Send the URB to the USB driver.
            //
            ntStatus = MavUsb_CallUSBD(DeviceObject, urb);
        }
        else
        {
            //
            // If we got here we failed to allocate deviceDescriptor
            //
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        }

        //
        // If the get descriptor was successful, then store the pointer to the
        // device descriptor in the device extension.
        //
        if(NT_SUCCESS(ntStatus))
        {
            deviceExtension->UsbDeviceDescriptor = deviceDescriptor;
        }

        //
        // Otherwise, free the descriptor memory if it was allocated.
        //
        else if(deviceDescriptor)
        {
            ExFreePool(deviceDescriptor);
        }

        //
        // Free the memory for the URB.
        //
        ExFreePool(urb);
    }
    else
    {
        //
        // If we got here we failed to allocate the URB.
        //
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // If we successfully got the device descriptor, then configure the device.
    //
    if(NT_SUCCESS(ntStatus))
    {
        ntStatus = MavUsb_ConfigureDevice(DeviceObject);
    }

    //
    // If the device was configured successfully, then indicate that the device
    // has been started.
    //
    if(NT_SUCCESS(ntStatus))
    {
        deviceExtension->DeviceStarted = TRUE;
    }

    //
    // Return the status.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Called from MavUsb_ProcessPnPIrp() to clean up our device instance's
// allocated buffers and free symbolic links.
//
// Arguments:
//
//     DeviceObject - pointer to the FDO
//
// Return Value:
//
//     NT status code from free symbolic link operation
//
//****************************************************************************
NTSTATUS
MavUsb_RemoveDevice(IN PDEVICE_OBJECT DeviceObject)
{
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    UNICODE_STRING deviceLinkUnicodeString;

    //
    // Get the pointer to the device extension.
    //
    deviceExtension = DeviceObject->DeviceExtension;

    //
    // Get the name of the symbolic link.
    //
    RtlInitUnicodeString(&deviceLinkUnicodeString,
                         deviceExtension->DeviceLinkNameBuffer);

    //
    // Remove the GUID-based symbolic link.
    //
    ntStatus = IoSetDeviceInterfaceState(&deviceLinkUnicodeString, FALSE);

    //
    // Free device descriptor structure.
    //
    if(deviceExtension->UsbDeviceDescriptor)
    {
        ExFreePool(deviceExtension->UsbDeviceDescriptor);
        deviceExtension->UsbDeviceDescriptor = NULL;
    }

    //
    // Free up the UsbInterface structure.
    //
    if(deviceExtension->UsbInterface)
    {
        ExFreePool(deviceExtension->UsbInterface);
        deviceExtension->UsbInterface = NULL;
    }

    //
    // Free up the USB config discriptor.
    //
    if(deviceExtension->UsbConfigurationDescriptor)
    {
        ExFreePool(deviceExtension->UsbConfigurationDescriptor);
        deviceExtension->UsbConfigurationDescriptor = NULL;
    }

    //
    // Return the status.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Stops a given instance of the device on the USB.  We basically just tell
// USB this device is now 'unconfigured'
//
// Arguments:
//
//     DeviceObject - pointer to the device object for this instance of the
//                    device
//
// Return Value:
//
//     NT status code
//
//****************************************************************************
NTSTATUS
MavUsb_StopDevice(IN PDEVICE_OBJECT DeviceObject)
{
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PURB urb;
    ULONG siz;

    //
    // Get the pointer to the device extension.
    //
    deviceExtension = DeviceObject->DeviceExtension;

    //
    // Send the select configuration urb with a NULL pointer for the
    // configuration handle.  This closes the configuration and puts the device
    // in the 'unconfigured' state.
    //
    siz = sizeof(struct _URB_SELECT_CONFIGURATION);
    urb = ExAllocatePool(NonPagedPool, siz);

    //
    // Make sure we were able to allocate memory.
    //
    if(urb)
    {
        //
        // Build the set configuration request in the URB.
        //
        UsbBuildSelectConfigurationRequest(urb, (USHORT)siz, NULL);

        //
        // Send the URB to the USB driver.
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
        // If we got here we failed to allocate the URB.
        //
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // If the set configuration was successful, then indicate that the device
    // is not started.
    //
    if(NT_SUCCESS(ntStatus))
    {
        deviceExtension->DeviceStarted = FALSE;
    }

    //
    // The stop request has been handled.
    //
    deviceExtension->StopDeviceRequested = FALSE;

    //
    // Return the status.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Used as a general purpose completion routine so it can signal an event,
// passed as the Context, when the next lower driver is done with the input
// IRP.
//
// This routine is used by both PnP and Power Management logic.
//
// Even though this routine does nothing but set an event, it must be defined
// and prototyped as a completetion routine for use as such.
//
// Arguments:
//
//     DeviceObject - Pointer to the device object for the class device.
//
//     Irp - IRP completed.
//
//     Context - Driver defined context, in this case a pointer to an event.
//
// Return Value:
//
//     The function value is the final status from the operation.
//
//****************************************************************************
NTSTATUS
MavUsb_IrpCompletionRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp,
                            IN PVOID Context)
{
    PKEVENT event = Context;

    //
    // Set the input event, with priority increment for the waiting thread and
    // no wait immediately following the set event.
    //
    KeSetEvent(event, 1, FALSE);

    //
    // This routine must return STATUS_MORE_PROCESSING_REQUIRED because we have
    // not yet called IoFreeIrp() on this IRP.
    //
    return(STATUS_MORE_PROCESSING_REQUIRED);
}

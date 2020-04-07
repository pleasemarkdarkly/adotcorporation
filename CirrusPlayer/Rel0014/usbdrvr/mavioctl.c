//****************************************************************************
//
// MAVIOCTL.C - IOCTL handler for the Maverick(tm) USB driver.
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
#include "../player/usbven.h"

//****************************************************************************
//
// Dispatch table handler for IRP_MJ_DEVICE_CONTROL; handles DeviceIoControl()
// calls from User mode.
//
// Arguments:
//
//     DeviceObject - pointer to the FDO for this instance of the device.
//
//     Irp - pointer to the IRP
//
// Return Value:
//
//     NT status code
//
//****************************************************************************
NTSTATUS
MavUsb_ProcessIOCTL(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    PIO_STACK_LOCATION irpStack;
    PVOID ioBuffer;
    ULONG inputBufferLength;
    ULONG outputBufferLength;
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    ULONG length;
    PUSB_CONFIGURATION_DESCRIPTOR configurationDescriptor;

    //
    // Increment the pending IRP count.
    //
    MavUsb_IncrementIoCount(DeviceObject);

    //
    // Get a pointer to the device extension.
    //
    deviceExtension = DeviceObject->DeviceExtension;

    //
    // Can't accept a new IO request if:
    //  1) device is removed,
    //  2) has never been started,
    //  3) is stopped,
    //  4) has a remove request pending,
    //  5) has a stop device pending.
    //
    if(!MavUsb_CanAcceptIoRequests(DeviceObject))
    {
        //
        // Set the status to delete pending.
        //
        ntStatus = STATUS_DELETE_PENDING;

        //
        // Set the status in the IRP.
        //
        Irp->IoStatus.Status = ntStatus;
        Irp->IoStatus.Information = 0;

        //
        // Complete the IRP.
        //
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        //
        // Decrement the pending IRP count.
        //
        MavUsb_DecrementIoCount(DeviceObject);

        //
        // Return the result.
        //
        return(ntStatus);
    }

    //
    // Get a pointer to the current location on the IRP stack.
    //
    irpStack = IoGetCurrentIrpStackLocation(Irp);

    //
    // Set the default status to success.
    //
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    //
    // Get pointers and lengths of the caller's (user's) IO buffer.
    //
    ioBuffer = Irp->AssociatedIrp.SystemBuffer;
    inputBufferLength = irpStack->Parameters.DeviceIoControl.InputBufferLength;
    outputBufferLength =
        irpStack->Parameters.DeviceIoControl.OutputBufferLength;

    //
    // Determine which IOCTL we were sent.
    //
    switch(irpStack->Parameters.DeviceIoControl.IoControlCode)
    {
        //
        // Reset the given pipe.
        //
        case IOCTL_MAVUSB_RESET_PIPE:
        {
            PUSBD_PIPE_INFORMATION pipe;
            PFILE_OBJECT fileObject;

            //
            // Get our context and see if it is a pipe.
            //
            fileObject = irpStack->FileObject;
            pipe = (PUSBD_PIPE_INFORMATION)fileObject->FsContext;

            //
            // See if this is a pipe.
            //
            if(pipe == NULL)
            {
                //
                // Error, this is not a pipe.
                //
                ntStatus = Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
            }
            else
            {
                //
                // Reset the pipe.
                //
                MavUsb_ResetPipe(DeviceObject, pipe);
            }

            //
            // We're done handling this IOCTL.
            //
            break;
        }

        //
        // Get a copy of the configuration descriptor and all endpoint and
        // interface descriptors.
        //
        case IOCTL_MAVUSB_GET_CONFIG_DESCRIPTOR:
        {
            //
            // Get a copy of the configuration descriptor pointer.
            //
            configurationDescriptor =
                deviceExtension->UsbConfigurationDescriptor;

            //
            // See if there is a configuration descriptor.
            //
            if(configurationDescriptor)
            {
                //
                // Get the length of the configuration descriptor.
                //
                length = configurationDescriptor->wTotalLength;

                //
                // See if there is enough space in the output buffer for the
                // entire configuration descriptor.
                //
                if(outputBufferLength >= length)
                {
                    //
                    // Copy the configuration descriptor into the output
                    // buffer.
                    //
                    RtlCopyMemory(ioBuffer, configurationDescriptor, length);

                    //
                    // Return the actual length in the IRP.
                    //
                    Irp->IoStatus.Information = length;
                }
                else
                {
                    //
                    // There is not enough space in the output buffer, so
                    // return an invalid parameter status.
                    //
                    ntStatus = Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
                    Irp->IoStatus.Information = 0;
                }
            }
            else
            {
                //
                // There is no configuration descriptor, so return a device
                // data error.
                //
                ntStatus = Irp->IoStatus.Status = STATUS_DEVICE_DATA_ERROR;
                Irp->IoStatus.Information = 0;
            }

            //
            // We're done with the IOCTL.
            //
            break;
        }

        //
        // Reset the device.
        //
        case IOCTL_MAVUSB_RESET_DEVICE:
        {
            //
            // Reset the device.
            //
            ntStatus = Irp->IoStatus.Status = MavUsb_ResetDevice(DeviceObject);

            //
            // We're done with this IOCTL.
            //
            break;
        }

        //
        // Send a vendor command to the device.
        //
        case IOCTL_MAVUSB_DEVICE_REQUEST:
        {
            //
            // Process the IRP.
            //
            ntStatus = Irp->IoStatus.Status =
                MavUsb_VendorCommand(DeviceObject, Irp, irpStack);

            //
            // We're done with this IOCTL.
            //
            break;
        }

        //
        // Reset the count of bytes transferred.
        //
        case IOCTL_MAVUSB_RESET_TRANSFER_COUNT:
        {
            //
            // Reset the transfer count.
            //
            deviceExtension->StagedBytesTransferred = 0;

            //
            // The status is success.
            //
            ntStatus = Irp->IoStatus.Status = STATUS_SUCCESS;

            //
            // We're done with this IOCTL.
            //
            break;
        }

        //
        // Get the current count of bytes transferred.
        //
        case IOCTL_MAVUSB_GET_TRANSFER_COUNT:
        {
            //
            // Get the transfer count.
            //
            *((PULONG)ioBuffer) = deviceExtension->StagedBytesTransferred;

            //
            // The length of the return is 4.
            //
            Irp->IoStatus.Information = 4;

            //
            // The status is success.
            //
            ntStatus = Irp->IoStatus.Status = STATUS_SUCCESS;

            //
            // We're done with this IOCTL.
            //
            break;
        }

        //
        // Get the name of the device.
        //
        case IOCTL_MAVUSB_GET_NAME:
        {
            unsigned char *pucBuffer = ioBuffer;
            int size, index;
            PURB urb;

            //
            // Allocate memory for a URB.
            //
            size = sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST);
            urb = ExAllocatePool(NonPagedPool, size);
            if(!urb)
            {
                ntStatus = Irp->IoStatus.Status =
                    STATUS_INSUFFICIENT_RESOURCES;
                break;
            }

            //
            // Get the index of the product description string.
            //
            index = deviceExtension->UsbDeviceDescriptor->iProduct;

            //
            // Make sure that there is a product description string.
            //
            if(index == 0)
            {
                ntStatus = Irp->IoStatus.Status =
                    STATUS_INVALID_PARAMETER;
                break;
            }

            //
            // Build the get descriptor request URB.
            //
            UsbBuildGetDescriptorRequest(urb, (USHORT)size,
                                         USB_STRING_DESCRIPTOR_TYPE,
                                         (UCHAR)index, 0x0409, pucBuffer, NULL,
                                         outputBufferLength, NULL);

            //
            // Send the URB to the USB driver.
            //
            ntStatus = Irp->IoStatus.Status =
                MavUsb_CallUSBD(DeviceObject, urb);

            //
            // Free the memory for the URB.
            //
            ExFreePool(urb);

            //
            // See if the request succeeded.
            //
            if(NT_SUCCESS(ntStatus))
            {
                //
                // Determine the length of the string.
                //
                if(pucBuffer[0] > outputBufferLength)
                {
                    size = outputBufferLength - 2;
                }
                else
                {
                    size = pucBuffer[0] - 2;
                }

                //
                // Copy the string to the beginning of the buffer, blasting the
                // two byte USB string descriptor header.
                //
                RtlCopyMemory(pucBuffer, pucBuffer + 2, size);

                //
                // Place an end-of-string character at the end of the string
                // in the buffer.
                //
                pucBuffer[size] = 0;
                pucBuffer[size + 1] = 0;

                //
                // Return the length of the string.
                //
                Irp->IoStatus.Information = size + 2;
            }

            //
            // We're done with this IOCTL.
            //
            break;
        }

        //
        // Get the name of the device manufacturer.
        //
        case IOCTL_MAVUSB_GET_MANUFACTURER_NAME:
        {
            unsigned char *pucBuffer = ioBuffer;
            int size, index;
            PURB urb;

            //
            // Allocate memory for a URB.
            //
            size = sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST);
            urb = ExAllocatePool(NonPagedPool, size);
            if(!urb)
            {
                ntStatus = Irp->IoStatus.Status =
                    STATUS_INSUFFICIENT_RESOURCES;
                break;
            }

            //
            // Get the index of the product description string.
            //
            index = deviceExtension->UsbDeviceDescriptor->iManufacturer;

            //
            // Make sure that there is a product description string.
            //
            if(index == 0)
            {
                ntStatus = Irp->IoStatus.Status =
                    STATUS_INVALID_PARAMETER;
                break;
            }

            //
            // Build the get descriptor request URB.
            //
            UsbBuildGetDescriptorRequest(urb, (USHORT)size,
                                         USB_STRING_DESCRIPTOR_TYPE,
                                         (UCHAR)index, 0x0409, pucBuffer, NULL,
                                         outputBufferLength, NULL);

            //
            // Send the URB to the USB driver.
            //
            ntStatus = Irp->IoStatus.Status =
                MavUsb_CallUSBD(DeviceObject, urb);

            //
            // Free the memory for the URB.
            //
            ExFreePool(urb);

            //
            // See if the request succeeded.
            //
            if(NT_SUCCESS(ntStatus))
            {
                //
                // Determine the length of the string.
                //
                if(pucBuffer[0] > outputBufferLength)
                {
                    size = outputBufferLength - 2;
                }
                else
                {
                    size = pucBuffer[0] - 2;
                }

                //
                // Copy the string to the beginning of the buffer, blasting the
                // two byte USB string descriptor header.
                //
                RtlCopyMemory(pucBuffer, pucBuffer + 2, size);

                //
                // Place an end-of-string character at the end of the string
                // in the buffer.
                //
                pucBuffer[size] = 0;
                pucBuffer[size + 1] = 0;

                //
                // Return the length of the string.
                //
                Irp->IoStatus.Information = size + 2;
            }

            //
            // We're done with this IOCTL.
            //
            break;
        }

        //
        // Return invalid parameter for all other IOCTLs.
        //
        default:
        {
            ntStatus = Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
        }
    }

    //
    // Complete the IRP.
    //
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

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
// Checks port status; if OK, return success and does nothing more; if bad,
// it attempt to reset the device.
//
// Arguments:
//
//     DeviceObject - pointer to the device object for this instance of the
//                    device.
//
// Return Value:
//
//     NT status code
//
//****************************************************************************
NTSTATUS
MavUsb_ResetDevice(IN PDEVICE_OBJECT DeviceObject)
{
    NTSTATUS ntStatus;
    ULONG portStatus;

    //
    // Check the port state, if it is disabled we will need to re-enable it.
    //
    ntStatus = MavUsb_GetPortStatus(DeviceObject, &portStatus);

    //
    // See if the port is not enabled and is connected, in which case we will
    // reset the parent port.
    //
    if(NT_SUCCESS(ntStatus) && !(portStatus & USBD_PORT_ENABLED) &&
       (portStatus & USBD_PORT_CONNECTED))
    {
        //
        // The port is disabled, attempt to reset the parent port.
        //
        ntStatus = MavUsb_ResetParentPort(DeviceObject);
    }

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Returns the port status for our device.
//
// Arguments:
//
//     DeviceObject - pointer to the devic eobject for this instance of the
//                    device.
//
//     PortStatus - status of the port.
//
// Return Value:
//
//     STATUS_SUCCESS if successful,
//     STATUS_UNSUCCESSFUL otherwise
//
//****************************************************************************
NTSTATUS
MavUsb_GetPortStatus(IN PDEVICE_OBJECT DeviceObject, IN PULONG PortStatus)
{
    NTSTATUS ntStatus, status = STATUS_SUCCESS;
    PIRP irp;
    KEVENT event;
    IO_STATUS_BLOCK ioStatus;
    PIO_STACK_LOCATION nextStack;
    PDEVICE_EXTENSION deviceExtension;

    //
    // Get a pointer to the device extension.
    //
    deviceExtension = DeviceObject->DeviceExtension;

    //
    // Initialize the port status to zero.
    //
    *PortStatus = 0;

    //
    // Initialize an IRP completion event.
    //
    KeInitializeEvent(&event, NotificationEvent, FALSE);

    //
    // IoBuildDeviceIoControlRequest allocates and sets up an IRP for a device
    // control request.
    //
    irp =
        IoBuildDeviceIoControlRequest(IOCTL_INTERNAL_USB_GET_PORT_STATUS,
                                      deviceExtension->TopOfStackDeviceObject,
                                      NULL, 0, NULL, 0, TRUE, &event,
                                      &ioStatus);

    //
    // IoGetNextIrpStackLocation gives a higher level driver access to the
    // next-lower driver's I/O stack location in an IRP so the caller can set
    // it up for the lower driver.
    //
    nextStack = IoGetNextIrpStackLocation(irp);

    //
    // Setup the argument in the IRP stack.
    //
    nextStack->Parameters.Others.Argument1 = PortStatus;

    //
    // Send the IRP to the PDO.
    //
    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, irp);

    //
    // See if we should wait for the IRP to complete.
    //
    if(ntStatus == STATUS_PENDING)
    {
        //
        // Wait for the IRP to complete.
        //
        KeWaitForSingleObject(&event, Suspended, KernelMode, FALSE, NULL);
    }
    else
    {
        //
        // Set the status in the IO status block.
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
// Reset our parent port.
//
// Arguments:
//
//     DeviceObject - pointer to the device object for this instance of the
//                    device.
//
// Return Value:
//
//     STATUS_SUCCESS if successful,
//     STATUS_UNSUCCESSFUL otherwise
//
//****************************************************************************
NTSTATUS
MavUsb_ResetParentPort(IN PDEVICE_OBJECT DeviceObject)
{
    NTSTATUS ntStatus, status = STATUS_SUCCESS;
    PIRP irp;
    KEVENT event;
    IO_STATUS_BLOCK ioStatus;
    PIO_STACK_LOCATION nextStack;
    PDEVICE_EXTENSION deviceExtension;

    //
    // Get a pointer to the device extension.
    //
    deviceExtension = DeviceObject->DeviceExtension;

    //
    // Initialize an IRP completion event.
    //
    KeInitializeEvent(&event, NotificationEvent, FALSE);

    //
    // Allocate and setup an IRP for a device control request.
    //
    irp =
        IoBuildDeviceIoControlRequest(IOCTL_INTERNAL_USB_RESET_PORT,
                                      deviceExtension->TopOfStackDeviceObject,
                                      NULL, 0, NULL, 0, TRUE, &event,
                                      &ioStatus);

    //
    // IoGetNextIrpStackLocation gives a higher level driver access to the
    // next-lower driver's I/O stack location in an IRP so the caller can set
    // it up for the lower driver.
    //
    nextStack = IoGetNextIrpStackLocation(irp);

    //
    // Send the IRP to the PDO.
    //
    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, irp);

    //
    // See if we should wait for the IRP to complete.
    //
    if(ntStatus == STATUS_PENDING)
    {
        //
        // Wait for the IRP to complete.
        //
        KeWaitForSingleObject(&event, Suspended, KernelMode, FALSE, NULL);
    }
    else
    {
        //
        // Set the status in the IO status block.
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
// Sends a vendor specific command to the device.
//
// Arguments:
//
//     DeviceObject - pointer to the FDO for this instance of the device.
//
//****************************************************************************
NTSTATUS
MavUsb_SendVendorCommand(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                         IN ULONG drive, IN ULONG request, IN USHORT value,
                         IN USHORT index, IN PUCHAR buffer, IN ULONG length)
{
    NTSTATUS ntStatus;

    //
    // Build the vendor request.
    //
    UsbBuildVendorRequest(urb, URB_FUNCTION_VENDOR_DEVICE,
                          sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST),
                          USBD_TRANSFER_DIRECTION_IN | USBD_SHORT_TRANSFER_OK,
                          0, (UCHAR)((drive << 5) | request), value, index,
                          buffer, 0, length, 0);

    //
    // Send the URB to the device.
    //
    ntStatus = MavUsb_CallUSBD(DeviceObject, urb);

    //
    // Return the status.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Performs a bulk transfer to/from the device.
//
//****************************************************************************
NTSTATUS
MavUsb_BulkXfer(IN PDEVICE_OBJECT DeviceObject, IN PUCHAR buffer,
                IN ULONG length, IN BOOLEAN Read)
{
    NTSTATUS ntStatus;
    PDEVICE_EXTENSION deviceExtension;
    PUSBD_INTERFACE_INFORMATION interface;
    USBD_PIPE_HANDLE PipeHandle;
    ULONG siz, flags;
    PURB urb;

    //
    // Get a pointer to the device extension.
    //
    deviceExtension = DeviceObject->DeviceExtension;

    //
    // Get a pointer to the interface descriptor.
    //
    interface = deviceExtension->UsbInterface;

    //
    // Get the pipe info structure for the pipe to be used.
    //
    PipeHandle = interface->Pipes[Read ? 0 : 1].PipeHandle;

    //
    // Get the flags for the transfer.
    //
    if(Read)
    {
        flags = USBD_TRANSFER_DIRECTION_IN | USBD_SHORT_TRANSFER_OK;
    }
    else
    {
        flags = USBD_SHORT_TRANSFER_OK;
    }

    //
    // Allocate memory for the URB.
    //
    siz = sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER);
    urb = ExAllocatePool(NonPagedPool, siz);
    if(!urb)
    {
        return(STATUS_INSUFFICIENT_RESOURCES);
    }

    //
    // Build the bulk transfer URB.
    //
    UsbBuildInterruptOrBulkTransferRequest(urb, (USHORT)siz, PipeHandle,
                                           buffer, NULL, length, flags, NULL);

    //
    // Send the URB to the USB driver.
    //
    ntStatus = MavUsb_CallUSBD(DeviceObject, urb);

    //
    // Free the memory used for the URB.
    //
    ExFreePool(urb);

    //
    // Return the status.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Sends a "connect" request to the device.
//
//****************************************************************************
NTSTATUS
MavUsb_VendorConnect(IN PDEVICE_OBJECT DeviceObject)
{
    NTSTATUS ntStatus;
    PURB urb;
    ULONG ulTemp;

    //
    // Allocate memory from the non-paged pool for the URB.
    //
    urb = ExAllocatePool(NonPagedPool,
                         sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));
    if(!urb)
    {
        return(STATUS_INSUFFICIENT_RESOURCES);
    }

    //
    // Send the "connect" vendor request.
    //
    ntStatus = MavUsb_SendVendorCommand(DeviceObject, urb, 0,
                                        USB_Vendor_Connect, 0, 0,
                                        (PUCHAR)&ulTemp, 4);

    //
    // Free the URB.
    //
    ExFreePool(urb);

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Sends a "disconnect" request to the device.
//
//****************************************************************************
NTSTATUS
MavUsb_VendorDisconnect(IN PDEVICE_OBJECT DeviceObject)
{
    NTSTATUS ntStatus;
    PURB urb;
    ULONG ulTemp;

    //
    // Allocate memory from the non-paged pool for the URB.
    //
    urb = ExAllocatePool(NonPagedPool,
                         sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));
    if(!urb)
    {
        return(STATUS_INSUFFICIENT_RESOURCES);
    }

    //
    // Send the "disconnect" vendor request.
    //
    ntStatus = MavUsb_SendVendorCommand(DeviceObject, urb, 0,
                                        USB_Vendor_Disconnect, 0, 0,
                                        (PUCHAR)&ulTemp, 4);

    //
    // Free the URB.
    //
    ExFreePool(urb);

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Handles the "get descriptor" request IOCTL.
//
//****************************************************************************
NTSTATUS
MavUsb_VendorGetDescriptor(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                           PUCHAR buffer, MavUsb_GetDescriptorParams *pIn,
                           MavUsb_GetDescriptorResult *pOut, PULONG result)
{
    NTSTATUS ntStatus;

    //
    // Send the "get_descriptor" vendor request.
    //
    ntStatus = MavUsb_SendVendorCommand(DeviceObject, urb, pIn->ulIndex,
                                        USB_Vendor_GetDescriptor, 0,
                                        (USHORT)pIn->ulDescriptor, buffer, 8);

    //
    // If the vendor packet was sent successfully, then return the result.
    //
    if(NT_SUCCESS(ntStatus))
    {
        //
        // Make sure that the correct amount of data was returned.
        //
        if(urb->UrbControlVendorClassRequest.TransferBufferLength != 8)
        {
            //
            // The wrong amount of data was returned, so return a device error.
            //
            ntStatus = STATUS_DEVICE_DATA_ERROR;
        }
        else
        {
            //
            // Get the return code.
            //
            *result = ((ULONG *)buffer)[0];

            //
            // Get the length of the descriptor value.
            //
            pOut->ulLength = ((ULONG *)buffer)[1];

            //
            // Read the value of the descriptor.
            //
            if(*result && pOut->ulLength)
            {
                ntStatus = MavUsb_BulkXfer(DeviceObject, pOut->pcValue,
                                           pOut->ulLength, TRUE);
            }
        }
    }

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Handles the "set descriptor" request IOCTL.
//
//****************************************************************************
NTSTATUS
MavUsb_VendorSetDescriptor(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                           PUCHAR buffer, MavUsb_SetDescriptorParams *pIn,
                           PULONG result)
{
    NTSTATUS ntStatus;

    //
    // Send the new descriptor value.
    //
    ntStatus = MavUsb_BulkXfer(DeviceObject, pIn->pcValue, pIn->ulLength,
                               FALSE);
    if(!NT_SUCCESS(ntStatus))
    {
        return(ntStatus);
    }

    //
    // Send the "set_descriptor" vendor request.
    //
    ntStatus = MavUsb_SendVendorCommand(DeviceObject, urb, pIn->ulIndex,
                                        USB_Vendor_SetDescriptor,
                                        (USHORT)pIn->ulLength,
                                        (USHORT)pIn->ulDescriptor, buffer, 4);

    //
    // If the vendor packet was sent successfully, then return the result.
    //
    if(NT_SUCCESS(ntStatus))
    {
        //
        // Make sure that the correct amount of data was returned.
        //
        if(urb->UrbControlVendorClassRequest.TransferBufferLength != 4)
        {
            //
            // The wrong amount of data was returned, so return a device error.
            //
            ntStatus = STATUS_DEVICE_DATA_ERROR;
        }
        else
        {
            //
            // Get the return code.
            //
            *result = ((ULONG *)buffer)[0];
        }
    }

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Handles the "num drives" request IOCTL.
//
//****************************************************************************
NTSTATUS
MavUsb_VendorNumDrives(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                       PUCHAR buffer, MavUsb_NumDrivesResult *pOut,
                       PULONG result)
{
    NTSTATUS ntStatus;

    //
    // Send the "num_drives" vendor request.
    //
    ntStatus = MavUsb_SendVendorCommand(DeviceObject, urb, 0,
                                        USB_Vendor_NumDrives, 0, 0, buffer, 8);

    //
    // If the vendor packet was sent successfully, then return the result.
    //
    if(NT_SUCCESS(ntStatus))
    {
        //
        // Make sure that the correct amount of data was returned.
        //
        if(urb->UrbControlVendorClassRequest.TransferBufferLength != 8)
        {
            //
            // The wrong amount of data was returned, so return a device error.
            //
            ntStatus = STATUS_DEVICE_DATA_ERROR;
        }
        else
        {
            //
            // Get the return code.
            //
            *result = ((ULONG *)buffer)[0];

            //
            // Get the number of drives.
            //
            pOut->ulNumDrives = ((ULONG *)buffer)[1];
        }
    }

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Handles the "open" request IOCTL.
//
//****************************************************************************
NTSTATUS
MavUsb_VendorOpen(IN PDEVICE_OBJECT DeviceObject, IN PURB urb, PUCHAR buffer,
                  MavUsb_OpenParams *pIn, PULONG result)
{
    NTSTATUS ntStatus;

    //
    // Send the file name.
    //
    ntStatus = MavUsb_BulkXfer(DeviceObject, (char *)pIn->pusFileName, 256,
                               FALSE);
    if(!NT_SUCCESS(ntStatus))
    {
        return(ntStatus);
    }

    //
    // Send the "open" vendor request.
    //
    ntStatus = MavUsb_SendVendorCommand(DeviceObject, urb, pIn->ulDriveNum,
                                        USB_Vendor_Open, (USHORT)pIn->ulFlags,
                                        0, buffer, 4);

    //
    // If the vendor packet was sent successfully, then return the result.
    //
    if(NT_SUCCESS(ntStatus))
    {
        //
        // Make sure that the correct amount of data was returned.
        //
        if(urb->UrbControlVendorClassRequest.TransferBufferLength != 4)
        {
            //
            // The wrong amount of data was returned, so return a device error.
            //
            ntStatus = STATUS_DEVICE_DATA_ERROR;
        }
        else
        {
            //
            // Get the return code.
            //
            *result = ((ULONG *)buffer)[0];
        }
    }

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Handles the "create" request IOCTL.
//
//****************************************************************************
NTSTATUS
MavUsb_VendorCreate(IN PDEVICE_OBJECT DeviceObject, IN PURB urb, PUCHAR buffer,
                    MavUsb_CreateParams *pIn, PULONG result)
{
    NTSTATUS ntStatus;

    //
    // Send the file name.
    //
    ntStatus = MavUsb_BulkXfer(DeviceObject, (char *)pIn->pusFileName, 256,
                               FALSE);
    if(!NT_SUCCESS(ntStatus))
    {
        return(ntStatus);
    }

    //
    // Send the "create" vendor request.
    //
    ntStatus = MavUsb_SendVendorCommand(DeviceObject, urb, pIn->ulDriveNum,
                                        USB_Vendor_Create,
                                        (USHORT)(pIn->ulLength & 0xffff),
                                        (USHORT)(pIn->ulLength >> 16), buffer,
                                        4);

    //
    // If the vendor packet was sent successfully, then return the result.
    //
    if(NT_SUCCESS(ntStatus))
    {
        //
        // Make sure that the correct amount of data was returned.
        //
        if(urb->UrbControlVendorClassRequest.TransferBufferLength != 4)
        {
            //
            // The wrong amount of data was returned, so return a device error.
            //
            ntStatus = STATUS_DEVICE_DATA_ERROR;
        }
        else
        {
            //
            // Get the return code.
            //
            *result = ((ULONG *)buffer)[0];
        }
    }

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Handles the "seek" request IOCTL.
//
//****************************************************************************
NTSTATUS
MavUsb_VendorSeek(IN PDEVICE_OBJECT DeviceObject, IN PURB urb, PUCHAR buffer,
                  MavUsb_SeekParams *pIn, PULONG result)
{
    NTSTATUS ntStatus;

    //
    // Send the "seek" vendor request.
    //
    ntStatus = MavUsb_SendVendorCommand(DeviceObject, urb, 0, USB_Vendor_Seek,
                                        (USHORT)(pIn->ulPosition & 0xffff),
                                        (USHORT)(pIn->ulPosition >> 16),
                                        buffer, 4);

    //
    // If the vendor packet was sent successfully, then return the result.
    //
    if(NT_SUCCESS(ntStatus))
    {
        //
        // Make sure that the correct amount of data was returned.
        //
        if(urb->UrbControlVendorClassRequest.TransferBufferLength != 4)
        {
            //
            // The wrong amount of data was returned, so return a device error.
            //
            ntStatus = STATUS_DEVICE_DATA_ERROR;
        }
        else
        {
            //
            // Get the return code.
            //
            *result = ((ULONG *)buffer)[0];
        }
    }

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Handles the "tell" request IOCTL.
//
//****************************************************************************
NTSTATUS
MavUsb_VendorTell(IN PDEVICE_OBJECT DeviceObject, IN PURB urb, PUCHAR buffer,
                  MavUsb_TellResult *pOut, PULONG result)
{
    NTSTATUS ntStatus;

    //
    // Send the "tell" vendor request.
    //
    ntStatus = MavUsb_SendVendorCommand(DeviceObject, urb, 0, USB_Vendor_Tell,
                                        0, 0, buffer, 8);

    //
    // If the vendor packet was sent successfully, then return the result.
    //
    if(NT_SUCCESS(ntStatus))
    {
        //
        // Make sure that the correct amount of data was returned.
        //
        if(urb->UrbControlVendorClassRequest.TransferBufferLength != 8)
        {
            //
            // The wrong amount of data was returned, so return a device error.
            //
            ntStatus = STATUS_DEVICE_DATA_ERROR;
        }
        else
        {
            //
            // Get the return code.
            //
            *result = ((ULONG *)buffer)[0];

            //
            // Get the current file pointer.
            //
            pOut->ulPosition = ((ULONG *)buffer)[1];
        }
    }

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Handles the "length" request IOCTL.
//
//****************************************************************************
NTSTATUS
MavUsb_VendorLength(IN PDEVICE_OBJECT DeviceObject, IN PURB urb, PUCHAR buffer,
                    MavUsb_LengthResult *pOut, PULONG result)
{
    NTSTATUS ntStatus;

    //
    // Send the "length" vendor request.
    //
    ntStatus = MavUsb_SendVendorCommand(DeviceObject, urb, 0,
                                        USB_Vendor_Length, 0, 0, buffer, 8);

    //
    // If the vendor packet was sent successfully, then return the result.
    //
    if(NT_SUCCESS(ntStatus))
    {
        //
        // Make sure that the correct amount of data was returned.
        //
        if(urb->UrbControlVendorClassRequest.TransferBufferLength != 8)
        {
            //
            // The wrong amount of data was returned, so return a device error.
            //
            ntStatus = STATUS_DEVICE_DATA_ERROR;
        }
        else
        {
            //
            // Get the return code.
            //
            *result = ((ULONG *)buffer)[0];

            //
            // Get the file length.
            //
            pOut->ulLength = ((ULONG *)buffer)[1];
        }
    }

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Handles the "get date" request IOCTL.
//
//****************************************************************************
NTSTATUS
MavUsb_VendorGetDate(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                     PUCHAR buffer, MavUsb_GetDateResult *pOut, PULONG result)
{
    NTSTATUS ntStatus;

    //
    // Send the "get date" vendor request.
    //
    ntStatus = MavUsb_SendVendorCommand(DeviceObject, urb, 0,
                                        USB_Vendor_GetDate, 0, 0, buffer, 8);

    //
    // If the vendor packet was sent successfully, then return the result.
    //
    if(NT_SUCCESS(ntStatus))
    {
        //
        // Make sure that the correct amount of data was returned.
        //
        if(urb->UrbControlVendorClassRequest.TransferBufferLength != 8)
        {
            //
            // The wrong amount of data was returned, so return a device error.
            //
            ntStatus = STATUS_DEVICE_DATA_ERROR;
        }
        else
        {
            //
            // Get the return code.
            //
            *result = ((ULONG *)buffer)[0];

            //
            // Get the modification date/time.
            //
            pOut->ulDate = ((ULONG *)buffer)[1];
        }
    }

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Handles the "close" request IOCTL.
//
//****************************************************************************
NTSTATUS
MavUsb_VendorClose(IN PDEVICE_OBJECT DeviceObject, IN PURB urb, PUCHAR buffer,
                   PULONG result)
{
    NTSTATUS ntStatus;

    //
    // Send the "close" vendor request.
    //
    ntStatus = MavUsb_SendVendorCommand(DeviceObject, urb, 0, USB_Vendor_Close,
                                        0, 0, buffer, 4);

    //
    // If the vendor packet was sent successfully, then return the result.
    //
    if(NT_SUCCESS(ntStatus))
    {
        //
        // Make sure that the correct amount of data was returned.
        //
        if(urb->UrbControlVendorClassRequest.TransferBufferLength != 4)
        {
            //
            // The wrong amount of data was returned, so return a device error.
            //
            ntStatus = STATUS_DEVICE_DATA_ERROR;
        }
        else
        {
            //
            // Get the return code.
            //
            *result = ((ULONG *)buffer)[0];
        }
    }

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Handles the "delete" request IOCTL.
//
//****************************************************************************
NTSTATUS
MavUsb_VendorDelete(IN PDEVICE_OBJECT DeviceObject, IN PURB urb, PUCHAR buffer,
                    MavUsb_DeleteParams *pIn, PULONG result)
{
    NTSTATUS ntStatus;

    //
    // Send the file name.
    //
    ntStatus = MavUsb_BulkXfer(DeviceObject, (char *)pIn->pusFileName, 256,
                               FALSE);
    if(!NT_SUCCESS(ntStatus))
    {
        return(ntStatus);
    }

    //
    // Send the "delete" vendor request.
    //
    ntStatus = MavUsb_SendVendorCommand(DeviceObject, urb, pIn->ulDriveNum,
                                        USB_Vendor_Delete, 0, 0, buffer, 4);

    //
    // If the vendor packet was sent successfully, then return the result.
    //
    if(NT_SUCCESS(ntStatus))
    {
        //
        // Make sure that the correct amount of data was returned.
        //
        if(urb->UrbControlVendorClassRequest.TransferBufferLength != 4)
        {
            //
            // The wrong amount of data was returned, so return a device error.
            //
            ntStatus = STATUS_DEVICE_DATA_ERROR;
        }
        else
        {
            //
            // Get the return code.
            //
            *result = ((ULONG *)buffer)[0];
        }
    }

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Handles the "opendir" request IOCTL.
//
//****************************************************************************
NTSTATUS
MavUsb_VendorOpenDir(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                     PUCHAR buffer, MavUsb_OpenDirParams *pIn, PULONG result)
{
    NTSTATUS ntStatus;

    //
    // Send the directory name.
    //
    ntStatus = MavUsb_BulkXfer(DeviceObject, (char *)pIn->pusDirName, 256,
                               FALSE);
    if(!NT_SUCCESS(ntStatus))
    {
        return(ntStatus);
    }

    //
    // Send the "open_dir" vendor request.
    //
    ntStatus = MavUsb_SendVendorCommand(DeviceObject, urb, pIn->ulDriveNum,
                                        USB_Vendor_OpenDir, 0, 0, buffer, 4);

    //
    // If the vendor packet was sent successfully, then return the result.
    //
    if(NT_SUCCESS(ntStatus))
    {
        //
        // Make sure that the correct amount of data was returned.
        //
        if(urb->UrbControlVendorClassRequest.TransferBufferLength != 4)
        {
            //
            // The wrong amount of data was returned, so return a device error.
            //
            ntStatus = STATUS_DEVICE_DATA_ERROR;
        }
        else
        {
            //
            // Get the return code.
            //
            *result = ((ULONG *)buffer)[0];
        }
    }

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Handles the "readdir" request IOCTL.
//
//****************************************************************************
NTSTATUS
MavUsb_VendorReadDir(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                     PUCHAR buffer, MavUsb_ReadDirParams *pIn,
                     MavUsb_ReadDirResult *pOut, PULONG result)
{
    NTSTATUS ntStatus;

    //
    // Send the "read_dir" vendor request.
    //
    ntStatus = MavUsb_SendVendorCommand(DeviceObject, urb, 0,
                                        USB_Vendor_ReadDir,
                                        (USHORT)pIn->ulType, 0, buffer, 4);

    //
    // If the vendor packet was sent successfully, then return the result.
    //
    if(NT_SUCCESS(ntStatus))
    {
        //
        // Make sure that the correct amount of data was returned.
        //
        if(urb->UrbControlVendorClassRequest.TransferBufferLength != 4)
        {
            //
            // The wrong amount of data was returned, so return a device error.
            //
            ntStatus = STATUS_DEVICE_DATA_ERROR;
        }
        else
        {
            //
            // Get the return code.
            //
            *result = ((ULONG *)buffer)[0];

            //
            // Read the file name.
            //
            if(*result)
            {
                ntStatus = MavUsb_BulkXfer(DeviceObject, (char *)pOut->pusName,
                                           256, TRUE);
            }
        }
    }

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Handles the "closedir" request IOCTL.
//
//****************************************************************************
NTSTATUS
MavUsb_VendorCloseDir(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                      PUCHAR buffer, PULONG result)
{
    NTSTATUS ntStatus;

    //
    // Send the "closedir" vendor request.
    //
    ntStatus = MavUsb_SendVendorCommand(DeviceObject, urb, 0,
                                        USB_Vendor_CloseDir, 0, 0, buffer, 4);

    //
    // If the vendor packet was sent successfully, then return the result.
    //
    if(NT_SUCCESS(ntStatus))
    {
        //
        // Make sure that the correct amount of data was returned.
        //
        if(urb->UrbControlVendorClassRequest.TransferBufferLength != 4)
        {
            //
            // The wrong amount of data was returned, so return a device error.
            //
            ntStatus = STATUS_DEVICE_DATA_ERROR;
        }
        else
        {
            //
            // Get the return code.
            //
            *result = ((ULONG *)buffer)[0];
        }
    }

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Handles the "makedir" request IOCTL.
//
//****************************************************************************
NTSTATUS
MavUsb_VendorMakeDir(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                     PUCHAR buffer, MavUsb_MakeDirParams *pIn, PULONG result)
{
    NTSTATUS ntStatus;

    //
    // Send the directory name.
    //
    ntStatus = MavUsb_BulkXfer(DeviceObject, (char *)pIn->pusDirName, 256,
                               FALSE);
    if(!NT_SUCCESS(ntStatus))
    {
        return(ntStatus);
    }

    //
    // Send the "make_dir" vendor request.
    //
    ntStatus = MavUsb_SendVendorCommand(DeviceObject, urb, pIn->ulDriveNum,
                                        USB_Vendor_MakeDir, 0, 0, buffer, 4);

    //
    // If the vendor packet was sent successfully, then return the result.
    //
    if(NT_SUCCESS(ntStatus))
    {
        //
        // Make sure that the correct amount of data was returned.
        //
        if(urb->UrbControlVendorClassRequest.TransferBufferLength != 4)
        {
            //
            // The wrong amount of data was returned, so return a device error.
            //
            ntStatus = STATUS_DEVICE_DATA_ERROR;
        }
        else
        {
            //
            // Get the return code.
            //
            *result = ((ULONG *)buffer)[0];
        }
    }

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Handles the "removedir" request IOCTL.
//
//****************************************************************************
NTSTATUS
MavUsb_VendorRemoveDir(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                       PUCHAR buffer, MavUsb_RemoveDirParams *pIn,
                       PULONG result)
{
    NTSTATUS ntStatus;

    //
    // Send the directory name.
    //
    ntStatus = MavUsb_BulkXfer(DeviceObject, (char *)pIn->pusDirName, 256,
                               FALSE);
    if(!NT_SUCCESS(ntStatus))
    {
        return(ntStatus);
    }

    //
    // Send the "remove_dir" vendor request.
    //
    ntStatus = MavUsb_SendVendorCommand(DeviceObject, urb, pIn->ulDriveNum,
                                        USB_Vendor_RemoveDir, 0, 0, buffer, 4);

    //
    // If the vendor packet was sent successfully, then return the result.
    //
    if(NT_SUCCESS(ntStatus))
    {
        //
        // Make sure that the correct amount of data was returned.
        //
        if(urb->UrbControlVendorClassRequest.TransferBufferLength != 4)
        {
            //
            // The wrong amount of data was returned, so return a device error.
            //
            ntStatus = STATUS_DEVICE_DATA_ERROR;
        }
        else
        {
            //
            // Get the return code.
            //
            *result = ((ULONG *)buffer)[0];
        }
    }

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Handles the "total space" request IOCTL.
//
//****************************************************************************
NTSTATUS
MavUsb_VendorTotalSpace(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                        PUCHAR buffer, MavUsb_TotalSpaceParams *pIn,
                        MavUsb_TotalSpaceResult *pOut, PULONG result)
{
    NTSTATUS ntStatus;

    //
    // Send the "total_space" vendor request.
    //
    ntStatus = MavUsb_SendVendorCommand(DeviceObject, urb, pIn->ulDriveNum,
                                        USB_Vendor_TotalSpace, 0, 0, buffer,
                                        8);

    //
    // If the vendor packet was sent successfully, then return the result.
    //
    if(NT_SUCCESS(ntStatus))
    {
        //
        // Make sure that the correct amount of data was returned.
        //
        if(urb->UrbControlVendorClassRequest.TransferBufferLength != 8)
        {
            //
            // The wrong amount of data was returned, so return a device error.
            //
            ntStatus = STATUS_DEVICE_DATA_ERROR;
        }
        else
        {
            //
            // Get the return code.
            //
            *result = ((ULONG *)buffer)[0];

            //
            // Get the total space.
            //
            pOut->ulBytes = ((ULONG *)buffer)[1];
        }
    }

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Handles the "free space" request IOCTL.
//
//****************************************************************************
NTSTATUS
MavUsb_VendorFreeSpace(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                       PUCHAR buffer, MavUsb_FreeSpaceParams *pIn,
                       MavUsb_FreeSpaceResult *pOut, PULONG result)
{
    NTSTATUS ntStatus;

    //
    // Send the "free_space" vendor request.
    //
    ntStatus = MavUsb_SendVendorCommand(DeviceObject, urb, pIn->ulDriveNum,
                                        USB_Vendor_FreeSpace, 0, 0, buffer, 8);

    //
    // If the vendor packet was sent successfully, then return the result.
    //
    if(NT_SUCCESS(ntStatus))
    {
        //
        // Make sure that the correct amount of data was returned.
        //
        if(urb->UrbControlVendorClassRequest.TransferBufferLength != 8)
        {
            //
            // The wrong amount of data was returned, so return a device error.
            //
            ntStatus = STATUS_DEVICE_DATA_ERROR;
        }
        else
        {
            //
            // Get the return code.
            //
            *result = ((ULONG *)buffer)[0];

            //
            // Get the free space.
            //
            pOut->ulBytes = ((ULONG *)buffer)[1];
        }
    }

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Handles the "format" request IOCTL.
//
//****************************************************************************
NTSTATUS
MavUsb_VendorFormat(IN PDEVICE_OBJECT DeviceObject, IN PURB urb, PUCHAR buffer,
                    MavUsb_FormatParams *pIn, PULONG result)
{
    NTSTATUS ntStatus;

    //
    // Send the "format" vendor request.
    //
    ntStatus = MavUsb_SendVendorCommand(DeviceObject, urb, pIn->ulDriveNum,
                                        USB_Vendor_Format, 0, 0, buffer, 4);

    //
    // If the vendor packet was sent successfully, then return the result.
    //
    if(NT_SUCCESS(ntStatus))
    {
        //
        // Make sure that the correct amount of data was returned.
        //
        if(urb->UrbControlVendorClassRequest.TransferBufferLength != 4)
        {
            //
            // The wrong amount of data was returned, so return a device error.
            //
            ntStatus = STATUS_DEVICE_DATA_ERROR;
        }
        else
        {
            //
            // Get the return code.
            //
            *result = ((ULONG *)buffer)[0];
        }
    }

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Handles the "start update" request IOCTL.
//
//****************************************************************************
NTSTATUS
MavUsb_VendorStartUpdate(IN PDEVICE_OBJECT DeviceObject, IN PURB urb,
                         PUCHAR buffer, PULONG result)
{
    NTSTATUS ntStatus;

    //
    // Send the "start_update" vendor request.
    //
    ntStatus = MavUsb_SendVendorCommand(DeviceObject, urb, 0,
                                        USB_Vendor_StartUpdate, 0, 0, buffer,
                                        4);

    //
    // If the vendor packet was sent successfully, then return the result.
    //
    if(NT_SUCCESS(ntStatus))
    {
        //
        // Make sure that the correct amount of data was returned.
        //
        if(urb->UrbControlVendorClassRequest.TransferBufferLength != 4)
        {
            //
            // The wrong amount of data was returned, so return a device error.
            //
            ntStatus = STATUS_DEVICE_DATA_ERROR;
        }
        else
        {
            //
            // Get the return code.
            //
            *result = ((ULONG *)buffer)[0];
        }
    }

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Processes vendor command IOCTLs from user mode code.
//
// Arguments:
//
//     DeviceObject - pointer to the FDO for this instance of the device.
//
//     Irp - pointer to the IRP
//
//     irpStack - pointer to the current IRP stack location
//
// Return Value:
//
//     NT status code
//
//****************************************************************************
NTSTATUS
MavUsb_VendorCommand(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp,
                     PIO_STACK_LOCATION irpStack)
{
    NTSTATUS ntStatus;
    PUCHAR buffer;
    ULONG length, siz;
    PURB urb;
    MavUsb_IoctlParams *pParams;
    MavUsb_IoctlResults *pResults;

    //
    // Make sure that the user supplied parameters and results buffers are
    // large enough.
    //
    if((irpStack->Parameters.DeviceIoControl.InputBufferLength <
        sizeof(MavUsb_IoctlParams)) ||
       (irpStack->Parameters.DeviceIoControl.OutputBufferLength <
        sizeof(MavUsb_IoctlResults)))
    {
        return(STATUS_INVALID_PARAMETER);
    }

    //
    // Get a pointer to the user supplied parameters and results.
    //
    pParams = (MavUsb_IoctlParams *)Irp->AssociatedIrp.SystemBuffer;
    pResults = (MavUsb_IoctlResults *)Irp->AssociatedIrp.SystemBuffer;

    //
    // Allocate memory from the non-paged pool for the URB and the result read
    // back from the device.
    //
    siz = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
    buffer = ExAllocatePool(NonPagedPool, siz + 8);
    if(!buffer)
    {
        return(STATUS_INSUFFICIENT_RESOURCES);
    }

    //
    // Build a pointer to the URB.
    //
    urb = (PURB)(buffer + 8);

    //
    // Determine what to do based on the specified request.
    //
    switch(pParams->ulRequestCode)
    {
        //
        // Get the value of a descriptor.
        //
        case REQUEST_GET_DESCRIPTOR:
        {
            //
            // Handle the get descriptor request.
            //
            ntStatus =
                MavUsb_VendorGetDescriptor(DeviceObject, urb, buffer,
                                           &pParams->uParams.sGetDescriptor,
                                           &pResults->uResults.sGetDescriptor,
                                           &pResults->ulReturnCode);

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Set the value of a descriptor.
        //
        case REQUEST_SET_DESCRIPTOR:
        {
            //
            // Handle the get descriptor request.
            //
            ntStatus =
                MavUsb_VendorSetDescriptor(DeviceObject, urb, buffer,
                                           &pParams->uParams.sSetDescriptor,
                                           &pResults->ulReturnCode);

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Get the number of drives on the device.
        //
        case REQUEST_NUM_DRIVES:
        {
            //
            // Handle the num drives request.
            //
            ntStatus = MavUsb_VendorNumDrives(DeviceObject, urb, buffer,
                                              &pResults->uResults.sNumDrives,
                                              &pResults->ulReturnCode);

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Open the specified file.
        //
        case REQUEST_OPEN:
        {
            //
            // Handle the open request.
            //
            ntStatus = MavUsb_VendorOpen(DeviceObject, urb, buffer,
                                         &pParams->uParams.sOpen,
                                         &pResults->ulReturnCode);

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Create the specified file.
        //
        case REQUEST_CREATE:
        {
            //
            // Handle the create request.
            //
            ntStatus = MavUsb_VendorCreate(DeviceObject, urb, buffer,
                                           &pParams->uParams.sCreate,
                                           &pResults->ulReturnCode);

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Move the file read/write pointer to the specified position.
        //
        case REQUEST_SEEK:
        {
            //
            // Handle the seek request.
            //
            ntStatus = MavUsb_VendorSeek(DeviceObject, urb, buffer,
                                         &pParams->uParams.sSeek,
                                         &pResults->ulReturnCode);

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Get the current file read/write pointer.
        //
        case REQUEST_TELL:
        {
            //
            // Handle the tell request.
            //
            ntStatus = MavUsb_VendorTell(DeviceObject, urb, buffer,
                                         &pResults->uResults.sTell,
                                         &pResults->ulReturnCode);

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Get the length of the file.
        //
        case REQUEST_LENGTH:
        {
            //
            // Handle the length request.
            //
            ntStatus = MavUsb_VendorLength(DeviceObject, urb, buffer,
                                           &pResults->uResults.sLength,
                                           &pResults->ulReturnCode);

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Get the modification date/time of the file.
        //
        case REQUEST_GETDATE:
        {
            //
            // Handle the get date request.
            //
            ntStatus = MavUsb_VendorGetDate(DeviceObject, urb, buffer,
                                            &pResults->uResults.sGetDate,
                                            &pResults->ulReturnCode);

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Close the currently opened file.
        //
        case REQUEST_CLOSE:
        {
            //
            // Handle the close request.
            //
            ntStatus = MavUsb_VendorClose(DeviceObject, urb, buffer,
                                          &pResults->ulReturnCode);

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Delete the specified file.
        //
        case REQUEST_DELETE:
        {
            //
            // Handle the delete request.
            //
            ntStatus = MavUsb_VendorDelete(DeviceObject, urb, buffer,
                                           &pParams->uParams.sDelete,
                                           &pResults->ulReturnCode);

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Open the specified directory.
        //
        case REQUEST_OPENDIR:
        {
            //
            // Handle the opendir request.
            //
            ntStatus = MavUsb_VendorOpenDir(DeviceObject, urb, buffer,
                                            &pParams->uParams.sOpenDir,
                                            &pResults->ulReturnCode);

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Read the next directory entry.
        //
        case REQUEST_READDIR:
        {
            //
            // Handle the readdir request.
            //
            ntStatus = MavUsb_VendorReadDir(DeviceObject, urb, buffer,
                                            &pParams->uParams.sReadDir,
                                            &pResults->uResults.sReadDir,
                                            &pResults->ulReturnCode);

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Close the currently opened directory.
        //
        case REQUEST_CLOSEDIR:
        {
            //
            // Handle the closedir request.
            //
            ntStatus = MavUsb_VendorCloseDir(DeviceObject, urb, buffer,
                                             &pResults->ulReturnCode);

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Create the specified directory.
        //
        case REQUEST_MAKEDIR:
        {
            //
            // Handle the makedir request.
            //
            ntStatus = MavUsb_VendorMakeDir(DeviceObject, urb, buffer,
                                            &pParams->uParams.sMakeDir,
                                            &pResults->ulReturnCode);

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Remove the specified directory.
        //
        case REQUEST_REMOVEDIR:
        {
            //
            // Handle the removedir request.
            //
            ntStatus = MavUsb_VendorRemoveDir(DeviceObject, urb, buffer,
                                              &pParams->uParams.sRemoveDir,
                                              &pResults->ulReturnCode);

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Get the capacity of the drive.
        //
        case REQUEST_TOTAL_SPACE:
        {
            //
            // Handle the total space request.
            //
            ntStatus = MavUsb_VendorTotalSpace(DeviceObject, urb, buffer,
                                               &pParams->uParams.sTotalSpace,
                                               &pResults->uResults.sTotalSpace,
                                               &pResults->ulReturnCode);

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Get the free space on the drive.
        //
        case REQUEST_FREE_SPACE:
        {
            //
            // Handle the free space request.
            //
            ntStatus = MavUsb_VendorFreeSpace(DeviceObject, urb, buffer,
                                              &pParams->uParams.sFreeSpace,
                                              &pResults->uResults.sFreeSpace,
                                              &pResults->ulReturnCode);

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Format the drive.
        //
        case REQUEST_FORMAT:
        {
            //
            // Handle the format request.
            //
            ntStatus = MavUsb_VendorFormat(DeviceObject, urb, buffer,
                                           &pParams->uParams.sFormat,
                                           &pResults->ulReturnCode);

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Start a software update.
        //
        case REQUEST_START_UPDATE:
        {
            //
            // Handle the start update request.
            //
            ntStatus = MavUsb_VendorStartUpdate(DeviceObject, urb, buffer,
                                                &pResults->ulReturnCode);

            //
            // We're done with this request.
            //
            break;
        }

        //
        // This is an unknown request.
        //
        default:
        {
            //
            // Indicate that an invalid paramter was specified (i.e. the
            // request).
            //
            ntStatus = STATUS_INVALID_PARAMETER;

            //
            // We're done with this request.
            //
            break;
        }
    }

    //
    // Free the memory for the URB and result buffer.
    //
    ExFreePool(buffer);

    //
    // Set the size of the returned data.
    //
    Irp->IoStatus.Information = sizeof(MavUsb_IoctlResults);

    //
    // Return the result.
    //
    return(ntStatus);
}

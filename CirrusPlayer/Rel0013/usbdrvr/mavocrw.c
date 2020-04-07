//****************************************************************************
//
// MAVOCRW.C - Routines to handle IRP_MJ_CREATE, IRP_MJ_CLOSE, IRP_MJ_READ,
//             and IRP_MJ_WRITE.
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
// This is the dispatch table routine for IRP_MJ_CREATE.  It's the entry point
// for CreateFile() calls.  User mode apps may open "<name genned fron GUID>".
//
// Arguments:
//
//     DeviceObject - pointer to our FDO (Functional Device Object)
//
//     Irp - pointer to the IRP
//
// Return Value:
//
//     NT status code
//
//****************************************************************************
NTSTATUS
MavUsb_Create(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION deviceExtension;

    //
    // Increment the count of pending IRPs.
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
        // Indicate that a delete device is pending.
        //
        ntStatus = STATUS_DELETE_PENDING;
    }

    //
    // Only allow two opens.
    //
    else if(deviceExtension->OpenPipeCount == 2)
    {
        //
        // Indicate that the device is busy.
        //
        ntStatus = STATUS_DEVICE_BUSY;
    }

    //
    // Open the device.
    //
    else
    {
        //
        // Indicate that the device is open.
        //
        deviceExtension->OpenPipeCount++;

        //
        // See if this is the first open on the device.
        //
        if(deviceExtension->OpenPipeCount == 1)
        {
            //
            // Power up the device if its not in D0.
            //
            MavUsb_SelfSuspendOrActivate(DeviceObject, FALSE);

            //
            // Send a connect message to the device.
            //
            if(MavUsb_VendorConnect(DeviceObject) != STATUS_SUCCESS)
            {
                //
                // Power down the device.
                //
                MavUsb_SelfSuspendOrActivate(DeviceObject, TRUE);

                //
                // Indicate that the device is not open.
                //
                deviceExtension->OpenPipeCount--;

                //
                // Indicate that there was a device IO error.
                //
                ntStatus = STATUS_IO_DEVICE_ERROR;
            }
        }
    }

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
// This is the dispatch table routine for IRP_MJ_CLOSE.  It handles user mode
// CloseHandle() calls.
//
// Arguments:
//
//     DeviceObject - pointer to our FDO (Functional Device Object)
//
//     Irp - pointer to the IRP
//
// Return Value:
//
//     NT status code
//
//****************************************************************************
NTSTATUS
MavUsb_Close(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    PDEVICE_EXTENSION deviceExtension;

    //
    // Increment the count of pending IRPs.
    //
    MavUsb_IncrementIoCount(DeviceObject);

    //
    // Get a pointer to the device extension.
    //
    deviceExtension = DeviceObject->DeviceExtension;

    //
    // Indicate that the device is not open.
    //
    deviceExtension->OpenPipeCount--;

    //
    // Send a disconnect message to the device if this is the last opened
    // handle.
    //
    if(deviceExtension->OpenPipeCount == 0)
    {
        MavUsb_VendorDisconnect(DeviceObject);
    }

    //
    // Set the status in the IRP.
    //
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    //
    // Complete the IRP.
    //
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    //
    // Power down the device if this was the last opened handle.
    //
    if(deviceExtension->OpenPipeCount == 0)
    {
        MavUsb_SelfSuspendOrActivate(DeviceObject, TRUE);
    }

    //
    // Decrement the count of pending IRPs.
    //
    MavUsb_DecrementIoCount(DeviceObject);

    //
    // Success.
    //
    return(STATUS_SUCCESS);
}

//****************************************************************************
//
// This is the IRP_MJ_READ routine set in our dispatch table; ReadFile() calls
// from user mode ultimately land here.
//
// Arguments:
//
//     DeviceObject - pointer to the device object for this instance of the
//                    device.
//
//     IRP - pointer to the IRP_MJ_READ
//
// Return Value:
//
//     NT status code
//
//****************************************************************************
NTSTATUS
MavUsb_Read(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus;
    USBD_PIPE_HANDLE pipeHandle;
    PURB urb;
    PMDL mdl;
    PVOID va;
    ULONG size, offset, length, readsize;

    //
    // Set the initial status in the IRP to pending.
    //
    Irp->IoStatus.Information = STATUS_PENDING;

    //
    // Set the initial count of byte read to zero.
    //
    Irp->IoStatus.Information = 0;

    //
    // Can't accept a new IO request if:
    //  1) device is removed,
    //  2) has never been started,
    //  3) is stopped,
    //  4) has a remove request pending,
    //  5) has a stop device pending
    //
    if(!MavUsb_CanAcceptIoRequests(DeviceObject))
    {
        //
        // Set the status in the IRP to delete pending.
        //
        Irp->IoStatus.Status = STATUS_DELETE_PENDING;

        //
        // Complete the IRP.
        //
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        //
        // Return a delete pending status.
        //
        return(STATUS_DELETE_PENDING);
    }

    //
    // Increment the count of pending IRPs.
    //
    MavUsb_IncrementIoCount(DeviceObject);

    //
    // Get a pointer to the device extension.
    //
    deviceExtension = DeviceObject->DeviceExtension;

    //
    // Get the length of the requested transfer.  The MDL address could be NULL
    // for a zero length request.
    //
    if(Irp->MdlAddress)
    {
        length = MmGetMdlByteCount(Irp->MdlAddress);
    }
    else
    {
        length = 0;
    }

    //
    // If the length of the transfer is zero, then return success.
    //
    if(length == 0)
    {
        //
        // Set the status in the IRP to success.
        //
        Irp->IoStatus.Status = STATUS_SUCCESS;

        //
        // Complete the IRP.
        //
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        //
        // Decrement the count of pending IRPs.
        //
        MavUsb_DecrementIoCount(DeviceObject);

        //
        // Return a success status.
        //
        return(STATUS_SUCCESS);
    }

    //
    // Get the virtual address for the buffer described by the input IRP's MDL.
    //
    va = MmGetMdlVirtualAddress(Irp->MdlAddress);

    //
    // Allocate memory for the URB to be sent to the USB driver.
    //
    urb = ExAllocatePool(NonPagedPool, sizeof(struct _URB) + 4);
    if(!urb)
    {
        //
        // Set the status in the IRP to insufficient resources.
        //
        Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;

        //
        // Complete the IRP.
        //
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        //
        // Decrement the count of pending IRPs.
        //
        MavUsb_DecrementIoCount(DeviceObject);

        //
        // Return an insufficient resources status.
        //
        return(STATUS_INSUFFICIENT_RESOURCES);
    }

    //
    // Indicate that a staged IO operation is pending.
    //
    deviceExtension->StagedIoPending = TRUE;

    //
    // Send read requests to the device until we've filled the entire data
    // buffer.
    //
    offset = 0;
    while(offset != length)
    {
        //
        // Make sure that we can still send IO requests.
        //
        if(!MavUsb_CanAcceptIoRequests(DeviceObject))
        {
            break;
        }

        //
        // Determine the length of the next read.
        //
        if((offset + 16384) > length)
        {
            readsize = length - offset;
        }
        else
        {
            readsize = 16384;
        }

        //
        // Build the URB for the vendor read request.
        //
        size = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
        UsbBuildVendorRequest(urb, URB_FUNCTION_VENDOR_DEVICE, (USHORT)size,
                              USBD_TRANSFER_DIRECTION_IN |
                              USBD_SHORT_TRANSFER_OK, 0, USB_Vendor_Read,
                              (USHORT)readsize, 0, urb + 1, NULL, 4, NULL);

        //
        // Send the vendor read command to the USB driver.
        //
        ntStatus = MavUsb_CallUSBD(DeviceObject, urb);
        if(!NT_SUCCESS(ntStatus))
        {
            break;
        }

        //
        // Get the length of the read from the result of the vendor command.
        //
        readsize = *((ULONG *)(urb + 1));

        //
        // Stop reading if the size of the read is zero.
        //
        if(readsize == 0)
        {
            break;
        }

        //
        // Allocate an MDL to describe the portion of the buffer for this bulk
        // transfer.
        //
        mdl = IoAllocateMdl(va, length, FALSE, FALSE, NULL);
        if(!mdl)
        {
            break;
        }

        //
        // Map the next portion of the read buffer to receive the data from
        // this read.
        //
        IoBuildPartialMdl(Irp->MdlAddress, mdl, (PUCHAR)va + offset, readsize);

        //
        // Get the pipe handle for the bulk in pipe.
        //
        pipeHandle = deviceExtension->UsbInterface->Pipes[0].PipeHandle;

        //
        // Build the URB for the bulk in transfer request.
        //
        size = sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER);
        UsbBuildInterruptOrBulkTransferRequest(urb, (USHORT)size, pipeHandle,
                                               NULL, mdl, readsize,
                                               USBD_TRANSFER_DIRECTION_IN |
                                               USBD_SHORT_TRANSFER_OK, NULL);

        //
        // Send this bulk in transfer request to the USB driver.
        //
        ntStatus = MavUsb_CallUSBD(DeviceObject, urb);

        //
        // Free the MDL,
        //
        IoFreeMdl(mdl);

        //
        // Stop reading if the bulk in transfer request failed.
        //
        if(!NT_SUCCESS(ntStatus))
        {
            break;
        }

        //
        // Increment the buffer pointer.
        //
        offset += readsize;

        //
        // Increment the number of bytes transferred.
        //
        deviceExtension->StagedBytesTransferred += readsize;
    }

    //
    // Set the number of bytes read in the IRP status.
    //
    Irp->IoStatus.Information = offset;

    //
    // Set the status of the IRP to success.
    //
    Irp->IoStatus.Status = STATUS_SUCCESS;

    //
    // Complete the IRP.
    //
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    //
    // Free our staged URB.
    //
    ExFreePool(urb);

    //
    // Indicate that a staged IO operation is no longer pending.
    //
    deviceExtension->StagedIoPending = FALSE;

    //
    // Decrement the count of pending IRPs.
    //
    MavUsb_DecrementIoCount(DeviceObject);

    //
    // Return success.
    //
    return(STATUS_SUCCESS);
}

//****************************************************************************
//
// This is the IRP_MJ_WRITE routine set in our dispatch table; WriteFile()
// calls from user mode ultimately land here.
//
// Arguments:
//
//     DeviceObject - pointer to the device object for this instance of the
//                    device.
//
//     IRP - pointer to the IRP_MJ_WRITE
//
// Return Value:
//
//     NT status code
//
//****************************************************************************
NTSTATUS
MavUsb_Write(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus;
    USBD_PIPE_HANDLE pipeHandle;
    PURB urb;
    PMDL mdl;
    PVOID va;
    ULONG size, offset, length, writesize;

    //
    // Set the initial status in the IRP to pending.
    //
    Irp->IoStatus.Information = STATUS_PENDING;

    //
    // Set the initial count of bytes written to zero.
    //
    Irp->IoStatus.Information = 0;

    //
    // Can't accept a new IO request if:
    //  1) device is removed,
    //  2) has never been started,
    //  3) is stopped,
    //  4) has a remove request pending,
    //  5) has a stop device pending
    //
    if(!MavUsb_CanAcceptIoRequests(DeviceObject))
    {
        //
        // Set the status in the IRP to delete pending.
        //
        Irp->IoStatus.Status = STATUS_DELETE_PENDING;

        //
        // Complete the IRP.
        //
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        //
        // Return a delete pending status.
        //
        return(STATUS_DELETE_PENDING);
    }

    //
    // Increment the count of pending IRPs.
    //
    MavUsb_IncrementIoCount(DeviceObject);

    //
    // Get a pointer to the device extension.
    //
    deviceExtension = DeviceObject->DeviceExtension;

    //
    // Get the length of the requested transfer.  The MDL address could be NULL
    // for a zero length request.
    //
    if(Irp->MdlAddress)
    {
        length = MmGetMdlByteCount(Irp->MdlAddress);
    }
    else
    {
        length = 0;
    }

    //
    // If the length of the transfer is zero, then return success.
    //
    if(length == 0)
    {
        //
        // Set the status in the IRP to success.
        //
        Irp->IoStatus.Status = STATUS_SUCCESS;

        //
        // Complete the IRP.
        //
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        //
        // Decrement the count of pending IRPs.
        //
        MavUsb_DecrementIoCount(DeviceObject);

        //
        // Return a success status.
        //
        return(STATUS_SUCCESS);
    }

    //
    // Get the virtual address for the buffer described by the input IRP's MDL.
    //
    va = MmGetMdlVirtualAddress(Irp->MdlAddress);

    //
    // Allocate memory for the URB to be sent to the USB driver.
    //
    urb = ExAllocatePool(NonPagedPool, sizeof(struct _URB) + 4);
    if(!urb)
    {
        //
        // Set the status in the IRP to insufficient resources.
        //
        Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;

        //
        // Complete the IRP.
        //
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        //
        // Decrement the count of pending IRPs.
        //
        MavUsb_DecrementIoCount(DeviceObject);

        //
        // Return an insufficient resources status.
        //
        return(STATUS_INSUFFICIENT_RESOURCES);
    }

    //
    // Indicate that a staged IO operation is pending.
    //
    deviceExtension->StagedIoPending = TRUE;

    //
    // Send write requests to the device until we've sent the entire data
    // buffer.
    //
    offset = 0;
    while(offset != length)
    {
        //
        // Make sure that we can still send IO requests.
        //
        if(!MavUsb_CanAcceptIoRequests(DeviceObject))
        {
            break;
        }

        //
        // Determine the length of the next write.
        //
        if((offset + 16384) > length)
        {
            writesize = length - offset;
        }
        else
        {
            writesize = 16384;
        }

        //
        // Allocate an MDL to describe the portion of the buffer for this bulk
        // transfer.
        //
        mdl = IoAllocateMdl(va, length, FALSE, FALSE, NULL);
        if(!mdl)
        {
            break;
        }

        //
        // Map the next portion of the write buffer to send the data for
        // this write.
        //
        IoBuildPartialMdl(Irp->MdlAddress, mdl, (PUCHAR)va + offset,
                          writesize);

        //
        // Get the pipe handle for the bulk out pipe.
        //
        pipeHandle = deviceExtension->UsbInterface->Pipes[1].PipeHandle;

        //
        // Build the URB for the bulk out transfer request.
        //
        size = sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER);
        UsbBuildInterruptOrBulkTransferRequest(urb, (USHORT)size, pipeHandle,
                                               NULL, mdl, writesize, 0, NULL);

        //
        // Send this bulk out transfer request to the USB driver.
        //
        ntStatus = MavUsb_CallUSBD(DeviceObject, urb);

        //
        // Free the MDL.
        //
        IoFreeMdl(mdl);

        //
        // Stop writing if the bulk out transfer request failed.
        //
        if(!NT_SUCCESS(ntStatus))
        {
            break;
        }

        //
        // Build the URB for the vendor write request.
        //
        size = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
        UsbBuildVendorRequest(urb, URB_FUNCTION_VENDOR_DEVICE, (USHORT)size,
                              USBD_TRANSFER_DIRECTION_IN |
                              USBD_SHORT_TRANSFER_OK, 0, USB_Vendor_Write,
                              (USHORT)writesize, 0, urb + 1, NULL, 4, NULL);

        //
        // Send the vendor write command to the USB driver.
        //
        ntStatus = MavUsb_CallUSBD(DeviceObject, urb);
        if(!NT_SUCCESS(ntStatus))
        {
            break;
        }

        //
        // Get the length of the write from the result of the vendor command.
        //
        writesize = *((ULONG *)(urb + 1));

        //
        // Stop writing if the size of the write is zero.
        //
        if(writesize == 0)
        {
            break;
        }

        //
        // Increment the buffer pointer.
        //
        offset += writesize;

        //
        // Increment the number of bytes transferred.
        //
        deviceExtension->StagedBytesTransferred += writesize;
    }

    //
    // Set the number of bytes read in the IRP status.
    //
    Irp->IoStatus.Information = offset;

    //
    // Set the status of the IRP to success.
    //
    Irp->IoStatus.Status = STATUS_SUCCESS;

    //
    // Complete the IRP.
    //
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    //
    // Free our staged URB.
    //
    ExFreePool(urb);

    //
    // Indicate that a staged IO operation is no longer pending.
    //
    deviceExtension->StagedIoPending = FALSE;

    //
    // Decrement the count of pending IRPs.
    //
    MavUsb_DecrementIoCount(DeviceObject);

    //
    // Return success.
    //
    return(STATUS_SUCCESS);
}

//****************************************************************************
//
// Called as part of sudden device removal handling.  Cancels any pending
// transfers for all open pipes.  If any pipes are still open, call USBD with
// URB_FUNCTION_ABORT_PIPE.  Also marks the pipe 'closed' in our saved
// configuration info.
//
// Arguments:
//
//     Ptrs to our FDO
//
// Return Value:
//
//     NT status code
//
//****************************************************************************
NTSTATUS
MavUsb_AbortPipes(IN PDEVICE_OBJECT DeviceObject)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PURB urb;
    PDEVICE_EXTENSION deviceExtension = DeviceObject->DeviceExtension;
    ULONG i;
    PUSBD_INTERFACE_INFORMATION interface;

    //
    // If the device is not opened, then we do not need to abort the pipes.
    //
    if(deviceExtension->OpenPipeCount == 0)
    {
        return(STATUS_SUCCESS);
    }

    //
    // Allocate memory for an URB.
    //
    urb = ExAllocatePool(NonPagedPool, sizeof(struct _URB_PIPE_REQUEST));
    if(!urb)
    {
        return(STATUS_INSUFFICIENT_RESOURCES);
    }

    //
    // Get a pointer to the USB interface descriptor.
    //
    interface = deviceExtension->UsbInterface;

    //
    // Loop through all the pipes in the device.
    //
    for(i = 0; i < interface->NumberOfPipes; i++)
    {
        //
        // Build the abort pipe request for the USB driver.
        //
        urb->UrbHeader.Length = (USHORT)sizeof(struct _URB_PIPE_REQUEST);
        urb->UrbHeader.Function = URB_FUNCTION_ABORT_PIPE;
        urb->UrbPipeRequest.PipeHandle = interface->Pipes[i].PipeHandle;

        //
        // Send the URB to the USB driver.
        //
        ntStatus = MavUsb_CallUSBD(DeviceObject, urb);

        //
        // If the request failed, then stop sending requests.
        //
        if(!NT_SUCCESS(ntStatus))
        {
            break;
        }
    }

    //
    // Free the URB.
    //
    ExFreePool(urb);

    //
    // Return the status.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Check device extension status flags; can't accept a new IO request if
// device:
//     1) is removed,
//     2) has never been started,
//     3) is stopped,
//     4) has a remove request pending, or
//     5) has a stop device pending
//
// Arguments:
//
//     DeviceObject - pointer to the device object for this instance of the
//                    device.
//
// Return Value:
//
//     return TRUE if can accept new IO requests, else FALSE
//
//****************************************************************************
BOOLEAN
MavUsb_CanAcceptIoRequests(IN PDEVICE_OBJECT DeviceObject)
{
    PDEVICE_EXTENSION deviceExtension;
    BOOLEAN fCan = FALSE;

    //
    // Get a pointer to the device extension.
    //
    deviceExtension = DeviceObject->DeviceExtension;

    //
    // We can accept IO requests when:
    //   1) the device is not removed,
    //   2) the device is started,
    //   3) a device remove has not been requested, and
    //   4) a device stop has not been requested.
    //
    if(!deviceExtension->DeviceRemoved &&
       deviceExtension->DeviceStarted &&
       !deviceExtension->RemoveDeviceRequested &&
       !deviceExtension->StopDeviceRequested)
    {
        fCan = TRUE;
    }

    //
    // Return the result.
    //
    return(fCan);
}

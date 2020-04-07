//****************************************************************************
//
// Mavpwr.C - Routines to handle power management IRPs.
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
// This is our FDO's dispatch table function for IRP_MJ_POWER.  It processes
// the Power IRPs sent to the PDO for this device.
//
// For every power IRP, drivers must call PoStartNextPowerIrp and use
// PoCallDriver to pass the IRP all the way down the driver stack to the
// underlying PDO.
//
// Arguments:
//
//     DeviceObject - pointer to our device object (FDO)
//
//     Irp - pointer to an I/O Request Packet
//
// Return Value:
//
//     NT status code
//
//****************************************************************************
NTSTATUS
MavUsb_ProcessPowerIrp(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    PIO_STACK_LOCATION irpStack;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION deviceExtension;
    BOOLEAN fGoingToD0 = FALSE;
    POWER_STATE sysPowerState, desiredDevicePowerState;
    KEVENT event;

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
    // Increment the count of pending IRPs.
    //
    MavUsb_IncrementIoCount(DeviceObject);

    //
    // Determine what to do based on the minor function in the IRP.
    //
    switch(irpStack->MinorFunction)
    {
        //
        // A driver sends IRP_MN_WAIT_WAKE to indicate that the system should
        // wait for its device to signal a wake event.  The exact nature of the
        // event is device-dependent.
        //
        // Drivers send this IRP for two reasons:
        //   1) To allow a device to wake the system
        //   2) To wake a device that has been put into a sleep state to save
        //      power but still must be able to communicate with its driver
        //      under certain circumstances.
        //
        // When a wake event occurs, the driver completes the IRP and returns
        // STATUS_SUCCESS.  If the device is sleeping when the event occurs,
        // the driver must first wake up the device before completing the IRP.
        // In a completion routine, the driver calls PoRequestPowerIrp to send
        // a PowerDeviceD0 request.  When the device has powered up, the driver
        // can handle the IRP_MN_WAIT_WAKE request.
        //
        case IRP_MN_WAIT_WAKE:
        {
            //
            // deviceExtension->DeviceCapabilities.DeviceWake specifies the
            // lowest device power state (least powered) from which the device
            // can signal a wake event.
            //
            deviceExtension->PowerDownLevel =
                deviceExtension->DeviceCapabilities.DeviceWake;

            //
            // See if the device is currently in a power state that can support
            // wakeup.
            //
            if((deviceExtension->CurrentDevicePowerState == PowerDeviceD0) ||
               (deviceExtension->DeviceCapabilities.DeviceWake >
                deviceExtension->CurrentDevicePowerState))
            {
                //
                // STATUS_INVALID_DEVICE_STATE is returned if the device in the
                // PowerD0 state or a state below which it can support waking,
                // or if the SystemWake state is below a state which can be
                // supported.  A pending IRP_MN_WAIT_WAKE will complete with
                // this error if the device's state is changed to be
                // incompatible with the wake request.
                //
                // If a driver fails this IRP, it should complete the IRP
                // immediately without passing the IRP to the next-lower
                // driver.
                //
                ntStatus = STATUS_INVALID_DEVICE_STATE;

                //
                // Set the status in the IRP.
                //
                Irp->IoStatus.Status = ntStatus;

                //
                // Complete the IRP.
                //
                IoCompleteRequest(Irp, IO_NO_INCREMENT);

                //
                // Decrement the pending IRP count.
                //
                MavUsb_DecrementIoCount(DeviceObject);

                //
                // Return the status.
                //
                return(ntStatus);
            }

            //
            // Flag we're enabled for wakeup.
            //
            deviceExtension->EnabledForWakeup = TRUE;

            //
            // Init an event for our completion routine to signal when PDO is
            // done with this IRP.
            //
            KeInitializeEvent(&event, NotificationEvent, FALSE);

            //
            // If not failing outright, pass this on to our PDO for further
            // handling.
            //
            IoCopyCurrentIrpStackLocationToNext(Irp);

            //
            // Set a completion routine so it can signal our event when the PDO
            // is done with the IRP.
            //
            IoSetCompletionRoutine(Irp, MavUsb_IrpCompletionRoutine, &event,
                                   TRUE, TRUE, TRUE);

            //
            // Prepare to send the IRP to the PDO.
            //
            PoStartNextPowerIrp(Irp);

            //
            // Send the IRP to the PDO.
            //
            ntStatus = PoCallDriver(deviceExtension->TopOfStackDeviceObject,
                                    Irp);

            //
            // If PDO is not done yet, wait for the event to be set in our
            // completion routine.
            //
            if(ntStatus == STATUS_PENDING)
            {
                //
                // Wait for the IRP to complete.
                //
                KeWaitForSingleObject(&event, Suspended, KernelMode, FALSE,
                                      NULL);
            }

            //
            // Now tell the device to actually wake up.
            //
            MavUsb_SelfSuspendOrActivate(DeviceObject, FALSE);

            //
            // Flag we're done with the wakeup IRP.
            //
            deviceExtension->EnabledForWakeup = FALSE;

            //
            // Decrement the count of pending IRPs.
            //
            MavUsb_DecrementIoCount(DeviceObject);

            //
            // We're done with this IRP.
            //
            break;
        }

        //
        // The system power policy manager sends this IRP to set the system
        // power state.  A device power policy manager sends this IRP to set
        // the device power state for a device.
        //
        // Set Irp->IoStatus.Status to STATUS_SUCCESS to indicate that the
        // device has entered the requested state.  Drivers cannot fail this
        // IRP.
        //
        case IRP_MN_SET_POWER:
        {
            //
            // Is the system or device power state being set?
            //
            switch(irpStack->Parameters.Power.Type)
            {
                //
                // The system power state is being set.
                //
                case SystemPowerState:
                {
                    //
                    // Get input system power state.
                    //
                    sysPowerState.SystemState =
                        irpStack->Parameters.Power.State.SystemState;

                    //
                    // If system is in working state always set our device to
                    // D0 regardless of the wait state or system-to-device
                    // state power map.
                    //
                    if(sysPowerState.SystemState == PowerSystemWorking)
                    {
                        desiredDevicePowerState.DeviceState = PowerDeviceD0;
                    }
                    else
                    {
                        //
                        // Set to corresponding system state if a
                        // IRP_MN_WAIT_WAKE is pending.
                        //
                        if(deviceExtension->EnabledForWakeup)
                        {
                            //
                            // Find the device power state equivalent to the
                            // given system state.  We get this info from the
                            // DEVICE_CAPABILITIES struct in our device
                            // extension (initialized in
                            // MavUsb_PnPAddDevice()).
                            //
                            desiredDevicePowerState.DeviceState =
                                deviceExtension->DeviceCapabilities.
                                DeviceState[sysPowerState.SystemState];
                        }
                        else
                        {
                            //
                            // If no wait pending and the system's not in
                            // working state, just turn off.
                            //
                            desiredDevicePowerState.DeviceState =
                                PowerDeviceD3;
                        }
                    }

                    //
                    // We've determined the desired device state; are we
                    // already in this state?
                    //
                    if(desiredDevicePowerState.DeviceState !=
                       deviceExtension->CurrentDevicePowerState)
                    {
                        //
                        // No, request that we be put into this state by
                        // requesting a new Power IRP from the PnP manager.
                        //
                        deviceExtension->PowerIrp = Irp;
                        ntStatus =
                            PoRequestPowerIrp(deviceExtension->
                                              PhysicalDeviceObject,
                                              IRP_MN_SET_POWER,
                                              desiredDevicePowerState,
                                              MavUsb_PoRequestCompletion,
                                              DeviceObject, NULL);
                    }
                    else
                    {
                        //
                        // Yes, just pass it on to PDO (Physical Device
                        // Object).
                        //
                        IoCopyCurrentIrpStackLocationToNext(Irp);
                        PoStartNextPowerIrp(Irp);
                        ntStatus =
                            PoCallDriver(deviceExtension->
                                         TopOfStackDeviceObject, Irp);

                        //
                        // Decrement the count of pending IRPs.
                        //
                        MavUsb_DecrementIoCount(DeviceObject);
                    }

                    //
                    // We're done handling the system power state change.
                    //
                    break;
                }

                //
                // The device power state is being set.
                //
                case DevicePowerState:
                {
                    //
                    // For requests to D1, D2, or D3 (sleep or off states),
                    // sets deviceExtension->CurrentDevicePowerState to
                    // DeviceState immediately.  This enables any code checking
                    // state to consider us as sleeping or off already, as this
                    // will imminently become our state.
                    //
                    // For requests to DeviceState D0 (fully on), sets
                    // fGoingToD0 flag TRUE to flag that we must set a
                    // completion routine and update
                    // deviceExtension->CurrentDevicePowerState there.
                    //
                    // In the case of powering up to fully on, we really want
                    // to make sure the process is completed before updating
                    // our CurrentDevicePowerState, so no IO will be attempted
                    // or accepted before we're really ready.
                    //
                    fGoingToD0 =
                        MavUsb_SetDevicePowerState(DeviceObject,
                                                   irpStack->Parameters.Power.
                                                   State.DeviceState);

                    //
                    // Prepare to send the IRP to the PDO.
                    //
                    IoCopyCurrentIrpStackLocationToNext(Irp);

                    //
                    // If we are change to state D0, then set our completion
                    // routine.
                    //
                    if(fGoingToD0)
                    {
                        IoSetCompletionRoutine(Irp, MavUsb_PowerIrp_Complete,
                                               DeviceObject, TRUE, TRUE, TRUE);
                    }

                    //
                    // Send the IRP to the PDO.
                    //
                    PoStartNextPowerIrp(Irp);
                    ntStatus =
                        PoCallDriver(deviceExtension->TopOfStackDeviceObject,
                                     Irp);

                    //
                    // If we are not changing to state D0, then decrement the
                    // count of pending IRPs.
                    //
                    if(!fGoingToD0)
                    {
                        MavUsb_DecrementIoCount(DeviceObject);
                    }

                    //
                    // We're done handling the device power state change.
                    //
                    break;
                }
            }

            //
            // We're done with this IRP.
            //
            break;
        }

        //
        // A power policy manager sends this IRP to determine whether it can
        // change the system or device power state, typically to go to sleep.
        //
        case IRP_MN_QUERY_POWER:
        {
            //
            // We do nothing special here, just let the PDO handle it.
            //
            IoCopyCurrentIrpStackLocationToNext(Irp);
            PoStartNextPowerIrp(Irp);
            ntStatus = PoCallDriver(deviceExtension->TopOfStackDeviceObject,
                                    Irp);

            //
            // Decrement the count of pending IRPs.
            //
            MavUsb_DecrementIoCount(DeviceObject);

            //
            // We're done with this IRP.
            //
            break;
        }

        //
        // Handle all other IRPs.
        //
        default:
        {
            //
            // All unhandled power messages are passed on to the PDO.
            //
            IoCopyCurrentIrpStackLocationToNext(Irp);
            PoStartNextPowerIrp(Irp);
            ntStatus = PoCallDriver(deviceExtension->TopOfStackDeviceObject,
                                    Irp);

            //
            // Decrement the count of pending IRPs.
            //
            MavUsb_DecrementIoCount(DeviceObject);
        }
    }

    //
    // Return the status.
    //
    return(ntStatus);
}

//****************************************************************************
//
// This is the completion routine set in a call to PoRequestPowerIrp() that
// was made in MavUsb_ProcessPowerIrp() in response to receiving an
// IRP_MN_SET_POWER of type 'SystemPowerState' when the device was not in a
// compatible device power state.  In this case, a pointer to the
// IRP_MN_SET_POWER IRP is saved into the FDO device extension
// (deviceExtension->PowerIrp), and then a call must be made to
// PoRequestPowerIrp() to put the device into a proper power state, and this
// routine is set as the completion routine.
//
// We decrement our pending io count and pass the saved IRP_MN_SET_POWER IRP
// on to the next driver.
//
// Arguments:
//
//     DeviceObject - Pointer to the device object for the class device.
//                    Note that we must get our own device object from the
//                    Context.
//
//     Context - Driver defined context, in this case our own functional
//               device object (FDO).
//
// Return Value:
//
//     The function value is the final status from the operation.
//
//****************************************************************************
NTSTATUS
MavUsb_PoRequestCompletion(IN PDEVICE_OBJECT DeviceObject,
                           IN UCHAR MinorFunction, IN POWER_STATE PowerState,
                           IN PVOID Context, IN PIO_STATUS_BLOCK IoStatus)
{
    PIRP irp;
    PDEVICE_EXTENSION deviceExtension;
    PDEVICE_OBJECT deviceObject = Context;
    NTSTATUS ntStatus;

    //
    // Get a pointer to the device extension.
    //
    deviceExtension = deviceObject->DeviceExtension;

    //
    // Get the IRP we saved for later processing in MavUsb_ProcessPowerIrp()
    // when we decided to request the Power IRP that this routine is the
    // completion routine for.
    //
    irp = deviceExtension->PowerIrp;

    //
    // We will return the status set by the PDO for the power request we're
    // completing.
    //
    ntStatus = IoStatus->Status;

    //
    // We must pass down to the next driver in the stack.
    //
    IoCopyCurrentIrpStackLocationToNext(irp);

    //
    // Calling PoStartNextPowerIrp() indicates that the driver is finished
    // with the previous power IRP, if any, and is ready to handle the next
    // power IRP.  It must be called for every power IRP.  Although power IRPs
    // are completed only once, typically by the lowest-level driver for a
    // device, PoStartNextPowerIrp must be called for every stack location.
    // Drivers must call PoStartNextPowerIrp while the current IRP stack
    // location points to the current driver.  Therefore, this routine must be
    // called before IoCompleteRequest, IoSkipCurrentStackLocation, and
    // PoCallDriver.
    //
    PoStartNextPowerIrp(irp);

    //
    // PoCallDriver is used to pass any power IRPs to the PDO instead of
    // IoCallDriver.  When passing a power IRP down to a lower-level driver,
    // the caller should use IoSkipCurrentIrpStackLocation or
    // IoCopyCurrentIrpStackLocationToNext to copy the IRP to the next stack
    // location, then call PoCallDriver.  Use
    // IoCopyCurrentIrpStackLocationToNext if processing the IRP requires
    // setting a completion routine, or IoSkipCurrentStackLocation if no
    // completion routine is needed.
    //
    PoCallDriver(deviceExtension->TopOfStackDeviceObject, irp);

    //
    // Decrement the count of pending IRPs.
    //
    MavUsb_DecrementIoCount(deviceObject);

    //
    // We're done handling the saved IRP.
    //
    deviceExtension->PowerIrp = NULL;

    //
    // Return the status.
    //
    return(ntStatus);
}

//****************************************************************************
//
// This routine is called when An IRP_MN_SET_POWER of type 'DevicePowerState'
// has been received by MavUsb_ProcessPowerIrp(), and that routine has
// determined:
//     1) the request is for full powerup (to PowerDeviceD0), and
//     2) We are not already in that state
// A call is then made to PoRequestPowerIrp() with this routine set as the
// completion routine.
//
// Arguments:
//
//     DeviceObject - Pointer to the device object for the class device.
//
//     Irp - IRP completed.
//
//     Context - Driver defined context.
//
// Return Value:
//
//     The function value is the final status from the operation.
//
//****************************************************************************
NTSTATUS
MavUsb_PowerIrp_Complete(IN PDEVICE_OBJECT NullDeviceObject, IN PIRP Irp,
                         IN PVOID Context)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_OBJECT deviceObject;
    PIO_STACK_LOCATION irpStack;
    PDEVICE_EXTENSION deviceExtension;

    //
    // Get a pointer to the the device object.
    //
    deviceObject = (PDEVICE_OBJECT)Context;

    //
    // Get a pointer to the device extension.
    //
    deviceExtension = deviceObject->DeviceExtension;

    //
    // If the lower driver returned PENDING, mark our stack location as pending
    // also.
    //
    if(Irp->PendingReturned)
    {
        IoMarkIrpPending(Irp);
    }

    //
    // Get a pointer to the current location on the IRP stack.
    //
    irpStack = IoGetCurrentIrpStackLocation(Irp);

    //
    // Now that we know we've let the lower drivers do what was needed to power
    // up, we can set our device extension flags accordingly.
    //
    deviceExtension->CurrentDevicePowerState = PowerDeviceD0;

    //
    // Set the status in the IRP.
    //
    Irp->IoStatus.Status = ntStatus;

    //
    // Decrement the count of pending IRPs.
    //
    MavUsb_DecrementIoCount(deviceObject);

    //
    // Return the status.
    //
    return(ntStatus);
}

//****************************************************************************
//
// Called on MavUsb_PnPAddDevice() to power down until needed (i.e., until a
// pipe is actually opened).
//
// Called on MavUsb_Create() to power up device to D0 before opening 1st pipe.
//
// Called on MavUsb_Close() to power down device if this is the last pipe.
//
// Arguments:
//
//     DeviceObject - Pointer to the device object
//
//     fSuspend - TRUE to Suspend, FALSE to activate.
//
// Return Value:
//
//     If the operation is not attemtped, SUCCESS is returned.
//
//     If the operation is attemtped, the value is the final status from the
//     operation.
//
//****************************************************************************
NTSTATUS
MavUsb_SelfSuspendOrActivate(IN PDEVICE_OBJECT DeviceObject,
                             IN BOOLEAN fSuspend)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    POWER_STATE PowerState;
    PDEVICE_EXTENSION deviceExtension;

    //
    // Get a pointer to the device extension.
    //
    deviceExtension = DeviceObject->DeviceExtension;

    //
    // Can't accept request if:
    //  1) device is removed,
    //  2) has never been started,
    //  3) is stopped,
    //  4) has a remove request pending,
    //  5) has a stop device pending.
    //
    if(!MavUsb_CanAcceptIoRequests(DeviceObject))
    {
        //
        // Indicate that a delete is pending.
        //
        return(STATUS_DELETE_PENDING);
    }

    //
    // Don't do anything if any System-generated Device PnP IRPs are pending.
    //
    if(deviceExtension->PowerIrp != NULL)
    {
        //
        // Return success.
        //
        return(STATUS_SUCCESS);
    }

    //
    // Don't do anything if any self-generated Device PnP IRPs are pending
    //
    if(deviceExtension->SelfPowerIrp)
    {
        //
        // Return success.
        //
        return(STATUS_SUCCESS);
    }

    //
    // Don't auto-suspend if any pipes are open.
    //
    if(fSuspend && (deviceExtension->OpenPipeCount != 0))
    {
        //
        // Return success.
        //
        return(STATUS_SUCCESS);
    }

    //
    // Don't auto-activate if no pipes are open.
    //
    if(!fSuspend && (deviceExtension->OpenPipeCount == 0))
    {
        //
        // Return success.
        //
        return(STATUS_SUCCESS);
    }

    //
    // Don't do anything if registry
    // CurrentControlSet\Services\BulkUsb\Parameters\PowerDownLevel
    // has been set to zero, PowerDeviceD0 (1), or a bogus high value.
    //
    if((deviceExtension->PowerDownLevel == PowerDeviceD0) ||
       (deviceExtension->PowerDownLevel == PowerDeviceUnspecified) ||
       (deviceExtension->PowerDownLevel >= PowerDeviceMaximum))
    {
        //
        // Return success.
        //
        return(STATUS_SUCCESS);
    }

    //
    // If we should suspend, then set our device state to the power down level.
    //
    if(fSuspend)
    {
        PowerState.DeviceState = deviceExtension->PowerDownLevel;
    }

    //
    // Otherwise, set our device state to D0.
    //
    else
    {
        PowerState.DeviceState = PowerDeviceD0;
    }

    //
    // Self request a power state change to the desired power state.
    //
    ntStatus = MavUsb_SelfRequestPowerIrp(DeviceObject, PowerState);

    //
    // Return the result.
    //
    return(ntStatus);
}

//****************************************************************************
//
// This routine is called by MavUsb_SelfSuspendOrActivate() to actually make
// the system request for a power down/up to PowerState.  It first checks to
// see if we are already in Powerstate and immediately returns SUCCESS with no
// further processing if so.
//
// Arguments:
//
//     DeviceObject - Pointer to the device object.
//
//     PowerState - power state requested, e.g PowerDeviceD0.
//
// Return Value:
//
//     The function value is the final status from the operation.
//
//****************************************************************************
NTSTATUS
MavUsb_SelfRequestPowerIrp(IN PDEVICE_OBJECT DeviceObject,
                           IN POWER_STATE PowerState)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION deviceExtension;
    PIRP pIrp = NULL;

    //
    // Get a pointer to the device extension.
    //
    deviceExtension = DeviceObject->DeviceExtension;

    //
    // If we are already in the correct power state, then do nothing.
    //
    if(deviceExtension->CurrentDevicePowerState == PowerState.DeviceState)
    {
        //
        // Return success.
        //
        return(STATUS_SUCCESS);
    }

    //
    // Increment the count of pending IRPs.
    //
    MavUsb_IncrementIoCount(DeviceObject);

    //
    // Flag we're handling a self-generated power IRP.
    //
    deviceExtension->SelfPowerIrp = TRUE;

    //
    // Actually request the IRP.
    //
    ntStatus = PoRequestPowerIrp(deviceExtension->PhysicalDeviceObject,
                                 IRP_MN_SET_POWER, PowerState,
                                 MavUsb_PoSelfRequestCompletion, DeviceObject,
                                 NULL);

    //
    // Make sure that the status was pending.
    //
    if(ntStatus == STATUS_PENDING)
    {
        //
        // We only need to wait for completion if we're powering up.
        //
        if((ULONG)PowerState.DeviceState < deviceExtension->PowerDownLevel)
        {
            //
            // Wait for the IRP to complete.
            //
            KeWaitForSingleObject(&deviceExtension->SelfRequestedPowerIrpEvent,
                                  Suspended, KernelMode, FALSE, NULL);
        }

        //
        // Set the status to success.
        //
        ntStatus = STATUS_SUCCESS;

        //
        // Indicate that we are no longer processing a self-power IRP.
        //
        deviceExtension->SelfPowerIrp = FALSE;
    }

    //
    // Return the status.
    //
    return(ntStatus);
}

//****************************************************************************
//
// This routine is called when the driver completes a self-originated power
// IRP that was generated by a call to BulkUsb_SelfSuspendOrActivate().
//
// We power down whenever the last pipe is closed and power up when the first
// pipe is opened.
//
// For power-up, we set an event in our FDO extension to signal this IRP done
// so the power request can be treated as a synchronous call.
//
// We need to know the device is powered up before opening the first pipe, for
// example.
//
// For power-down, we do not set the event, as no caller waits for power down
// complete.
//
// Arguments:
//
//     DeviceObject - Pointer to the device object for the class device.
//                    (Physical Device Object)
//
//     Context - Driver defined context, in this case our FDO (functional
//               device object)
//
// Return Value:
//
//     The function value is the final status from the operation.
//
//****************************************************************************
NTSTATUS
MavUsb_PoSelfRequestCompletion(IN PDEVICE_OBJECT DeviceObject,
                               IN UCHAR MinorFunction,
                               IN POWER_STATE PowerState, IN PVOID Context,
                               IN PIO_STATUS_BLOCK IoStatus)
{
    PDEVICE_OBJECT deviceObject = Context;
    PDEVICE_EXTENSION deviceExtension = deviceObject->DeviceExtension;
    NTSTATUS ntStatus = IoStatus->Status;

    //
    // We only need to set the event if we're powering up; no caller waits on
    // power down complete.
    //
    if((ULONG)PowerState.DeviceState < deviceExtension->PowerDownLevel)
    {
        //
        // Trigger self-requested power IRP completed event; the caller is
        // waiting for completion.
        //
        KeSetEvent(&deviceExtension->SelfRequestedPowerIrpEvent, 1, FALSE);
    }

    //
    // Decrement the count of pending IRPs.
    //
    MavUsb_DecrementIoCount(deviceObject);

    //
    // Return the IO completion status.
    //
    return(ntStatus);
}

//****************************************************************************
//
// This routine is called when an IRP_MN_SET_POWER of type 'DevicePowerState'
// has been received by MavUsb_ProcessPowerIrp().
//
// Arguments:
//
//     DeviceObject - Pointer to the device object for the class device.
//
//     DeviceState - Device specific power state to set the device in to.
//
// Return Value:
//
//     For requests to DeviceState D0 (fully on), returns TRUE to signal caller
//     that we must set a completion routine and finish there.
//
//****************************************************************************
BOOLEAN
MavUsb_SetDevicePowerState(IN PDEVICE_OBJECT DeviceObject,
                           IN DEVICE_POWER_STATE DeviceState)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION deviceExtension;
    BOOLEAN fRes = FALSE;

    //
    // Get a pointer to the device extension.
    //
    deviceExtension = DeviceObject->DeviceExtension;

    //
    // Determine the power state we are changing to.
    //
    switch(DeviceState)
    {
        //
        // The device will be turning off.
        //
        case PowerDeviceD3:
        {
            //
            // Set the device power state.
            //
            deviceExtension->CurrentDevicePowerState = DeviceState;

            //
            // We're done handling this power state.
            //
            break;
        }

        //
        // Power states D1 and D2 translate to USB suspend.
        //
        case PowerDeviceD1:
        case PowerDeviceD2:
        {
            //
            // Set the device power state.
            //
            deviceExtension->CurrentDevicePowerState = DeviceState;

            //
            // We're done handling these power states.
            //
            break;
        }

        //
        // The device will be turning on.
        //
        case PowerDeviceD0:
        {
            //
            // We'll need to finish the rest in the completion routine; signal
            // caller we're going to D0 and will need to set a completion
            // routine.
            //
            fRes = TRUE;

            //
            // Caller will pass on to PDO (Physical Device Object).
            //
            break;
        }
    }

    //
    // Return the result.
    //
    return(fRes);
}

//****************************************************************************
//
// This routine generates an internal IRP from this driver to the lower
// portion of the driver stack to obtain information on the Device Object's
// capabilities.  We are most interested in learning which system power states
// are to be mapped to which device power states for honoring IRP_MJ_SET_POWER
// IRPs.
//
// This is a blocking call which waits for the IRP completion routine to set
// an event on finishing.
//
// Arguments:
//
//     LowerDeviceObject - DeviceObject beneath this driver in the stack.
//
// Return Value:
//
//     NTSTATUS value from the IoCallDriver() call.
//
//****************************************************************************
NTSTATUS
MavUsb_QueryCapabilities(IN PDEVICE_OBJECT LowerDeviceObject,
                         IN PDEVICE_CAPABILITIES DeviceCapabilities)
{
    PIO_STACK_LOCATION nextStack;
    PIRP irp;
    NTSTATUS ntStatus;
    KEVENT event;

    //
    // This is a DDK-defined DBG-only macro that ASSERTS we are not running
    // pageable code at higher than APC_LEVEL.
    //
    PAGED_CODE();

    //
    // Build an IRP for us to generate an internal query request to the PDO.
    //
    irp = IoAllocateIrp(LowerDeviceObject->StackSize, FALSE);
    if(!irp)
    {
        return(STATUS_INSUFFICIENT_RESOURCES);
    }

    //
    // Preinit the device capability structures appropriately.
    //
    RtlZeroMemory(DeviceCapabilities, sizeof(DEVICE_CAPABILITIES));
    DeviceCapabilities->Size = sizeof(DEVICE_CAPABILITIES);
    DeviceCapabilities->Version = 1;
    DeviceCapabilities->Address = -1;
    DeviceCapabilities->UINumber = -1;

    //
    // IoGetNextIrpStackLocation gives a higher level driver access to the
    // next-lower driver's I/O stack location in an IRP so the caller can set
    // it up for the lower driver.
    //
    nextStack = IoGetNextIrpStackLocation(irp);
    nextStack->MajorFunction= IRP_MJ_PNP;
    nextStack->MinorFunction= IRP_MN_QUERY_CAPABILITIES;

    //
    // Init an event to tell us when the completion routine's been called.
    //
    KeInitializeEvent(&event, NotificationEvent, FALSE);

    //
    // Set a completion routine so it can signal our event when the next lower
    // driver is done with the IRP.
    //
    IoSetCompletionRoutine(irp, MavUsb_IrpCompletionRoutine, &event, TRUE,
                           TRUE, TRUE);

    //
    // Set our pointer to the DEVICE_CAPABILITIES struct.
    //
    nextStack->Parameters.DeviceCapabilities.Capabilities = DeviceCapabilities;

    //
    // Preset the IRP to report not supported.
    //
    irp->IoStatus.Status = STATUS_NOT_SUPPORTED;

    //
    // Pass the IRP to the lower level device object.
    //
    ntStatus = IoCallDriver(LowerDeviceObject, irp);

    //
    // See if we need to wait for the IRP to complete.
    //
    if(ntStatus == STATUS_PENDING)
    {
        //
        // Wait for the IRP to complete.
        //
        KeWaitForSingleObject(&event, Suspended, KernelMode, FALSE, NULL);

        //
        // Get the status from the IRP.
        //
        ntStatus = irp->IoStatus.Status;
    }

    //
    // Free the IRP.
    //
    IoFreeIrp(irp);

    //
    // Return the result.
    //
    return(ntStatus);
}

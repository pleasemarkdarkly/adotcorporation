//****************************************************************************
//
// UISER.C - User interface using a serial port.
//
// Copyright (c) 2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include "../hwport.h"
#include "../hwdefs.h"

//****************************************************************************
//
// Only include this file if the serial port is being used for the user
// interface.
//
//****************************************************************************
#ifdef UI_IS_SERIAL_PORT

//****************************************************************************
//
// SerialSendString sends a string to the serial port.
//
//****************************************************************************
static void
SerialSendString(char *pcString)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;

    //
    // Send the string character by character.
    //
    while(*pcString)
    {
        //
        // Wait until there is space available in the transmit FIFO.
        //
        while(pulPtr[HwStatus >> 2] & HwStatusUartTxFifoFull)
        {
        }

        //
        // Write the next character to the transmit FIFO.
        //
        pulPtr[HwUartData >> 2] = *pcString++;
    }

    //
    // Wait until the serial port transmit is not busy.
    //
    while(pulPtr[HwStatus >> 2] & HwStatusUartTxBusy)
    {
    }
}

//****************************************************************************
//
// UIEnable initializes the user interface.
//
//****************************************************************************
void
UIEnable(void)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;

    //
    // Configure the serial port for 115,200, 8-N-1.
    //
    pulPtr[HwUartControl >> 2] = HwUartControlFifoEnable |
                                 HwUartControlDataLength8 |
                                 HwUartControlRate115200;

    //
    // Enable the serial port.
    //
    pulPtr[HwControl >> 2] |= HwControlUartEnable;
}

//****************************************************************************
//
// UIDisable shuts down the user interface.
//
//****************************************************************************
void
UIDisable(void)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;

    //
    // Disable the serial port.
    //
    pulPtr[HwControl >> 2] &= ~HwControlUartEnable;
}

//****************************************************************************
//
// UIRequestUpdate displays a message telling the user to update the player
// software.
//
//****************************************************************************
void
UIRequestUpdate(void)
{
    //
    // Send a message to the serial port.
    //
    SerialSendString("\rThe player software needs to be updated.");
}

//****************************************************************************
//
// UIUpdate displays a message on the LCD telling the user that an update is
// in progress.
//
//****************************************************************************
void
UIUpdate(void)
{
    //
    // Send a message to the serial port.
    //
    SerialSendString("\r                                        ");
    SerialSendString("\rUpdating              ");
}

#endif

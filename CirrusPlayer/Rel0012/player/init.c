//****************************************************************************
//
// INIT.C - Initialize the player hardware and software environment.
//
// Copyright (c) 1999-2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include "globals.h"

//****************************************************************************
//
// The following function initializes the hardware and software environment
// expected by the Internet audio player application.  Some very basic
// hardware initialization is already handled by vectors.s before it calls
// this function.
//
//****************************************************************************
void
initialize(void)
{
    //
    // Initialize the file system.
    //
    FSSetWriteScratch((unsigned char *)ulEndOfRAM);
    FSInit();
    FSSetWriteScratch(0);

    //
    // Initialize the output processing.
    //
    OutputInit();

    //
    // Initialize the user interface.
    //
    UIInit();

    //
    // Initialize the USB controller.
    //
#ifndef HwRegUSBCable
    USBEnable();
#endif
}

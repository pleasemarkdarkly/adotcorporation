//****************************************************************************
//
// TWOWIRE.H - Function prototype for the function in twowire.o.
//
// Copyright (c) 2001 Cirrus Logic, Inc.
//
//****************************************************************************

//
// This function performs a write to a device with a two-wire interface.
//
extern void TwoWireWrite(volatile unsigned long *pulGPIOData,
                         volatile unsigned long *pulGPIODir,
                         unsigned long ulClock, unsigned long ulData,
                         unsigned char ucAddr, unsigned char ucReg,
                         unsigned char ucValue);

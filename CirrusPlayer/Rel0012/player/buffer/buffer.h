//****************************************************************************
//
// BUFFER.H - Contains function prototypes for the routines in the circular
//            buffer library.
//
// Copyright (c) 1999 Cirrus Logic, Inc.
//
//****************************************************************************

//****************************************************************************
//
// Make sure that we are included only once.
//
//****************************************************************************
#ifndef _BUFFER_H_
#define _BUFFER_H_

//****************************************************************************
//
// The following values are the IOCTLs which are sent to the buffer specific
// IOCTL handler.
//
//****************************************************************************
enum
{
    IOCTL_BUFFER_SETBUFFER,
    IOCTL_BUFFER_DATAAVAILABLE,
    IOCTL_BUFFER_SPACEAVAILABLE,
    IOCTL_BUFFER_ISEMPTY,
    IOCTL_BUFFER_GETWRITEPOINTER,
    IOCTL_BUFFER_UPDATEWRITEPOINTER,
    IOCTL_BUFFER_GETREADPOINTER,
    IOCTL_BUFFER_UPDATEREADPOINTER
};

//****************************************************************************
//
// The perisistent state of the circular buffer.  Do not re-arrange this
// structure as there is assembly which depends upon the layout not changing.
// There is a function pointer in this structure, making it appear that this
// would be better done as an object in C++.  It is not done that way since
// there is a large space overhead involved in adding the C++ support code/
// memory management.  We therefore implement a C++-like object in regular C
// code.
//
//****************************************************************************
typedef struct sbs
{
    //
    // A pointer to the circular buffer containing the left channel data.
    //
    short *psLeft;

    //
    // A pointer to the circular buffer containing the right channel data.
    //
    short *psRight;

    //
    // The length of the circular buffer.
    //
    long lLength;

    //
    // The next point in the circular buffer where data will be read.
    //
    long lReadPtr;

    //
    // The next point in the circular buffer where data will be written.
    //
    long lWritePtr;

    //
    // pfnIoctl is the function used to handle the buffer IOCTLs.
    //
    long (*pfnIoctl)(unsigned long ulIoctl, struct sbs *psBuffer,
                     unsigned long ulParam1, unsigned long ulParam2,
                     unsigned long ulParam3);

    //
    // The instance data for this FIFO.
    //
    long lInstanceData;
} BufferState;

//****************************************************************************
//
// Function prototypes.
//
//****************************************************************************
extern void BufferInit(BufferState *psBuffer,
                       long (*pfnIoctl)(unsigned long ulIoctl,
                                        BufferState *psBuffer,
                                        unsigned long ulParam1,
                                        unsigned long ulParam2,
                                        unsigned long ulParam3),
                       long lInstanceData);
extern long BufferSetBuffer(BufferState *psBuffer, short *psLeft,
                            short *psRight, long lLength);
extern long BufferDataAvailable(BufferState *psBuffer);
extern long BufferSpaceAvailable(BufferState *psBuffer);
extern long BufferIsEmpty(BufferState *psBuffer);
extern long BufferGetWritePointer(BufferState *psBuffer, short **ppsLeft,
                                  short **ppsRight, long *plLength);
extern long BufferUpdateWritePointer(BufferState *psBuffer, long lLength);
extern long BufferGetReadPointer(BufferState *psBuffer, short **ppsLeft,
                                 short **ppsRight, long *plLength);
extern long BufferUpdateReadPointer(BufferState *psBuffer, long lLength);

//****************************************************************************
//
// Make sure we are included only once.
//
//****************************************************************************
#endif // #ifndef _BUFFER_H_

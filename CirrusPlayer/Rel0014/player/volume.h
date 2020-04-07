//****************************************************************************
//
// VOLUME.H - Contains function prototypes for the routines in the volume
//            control.
//
// Copyright (c) 1999-2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include "buffer/buffer.h"

//****************************************************************************
//
// The persistent state of the volume control.
//
//****************************************************************************
typedef struct
{
    //
    // The scalar for the current volume setting.
    //
    long lVolume;

    //
    // The buffer from which we receive input data.
    //
    BufferState sInputBuffer;

    //
    // The buffer to which we send output data.
    //
    BufferState *pOutputBuffer;
} tVolumeControl;

//****************************************************************************
//
// Function prototypes.
//
//****************************************************************************
extern void VolumeInit(tVolumeControl *pVol, BufferState *pOutputBuffer);
extern void VolumeAdjust(tVolumeControl *pVol, long lGain);
extern BufferState *VolumeGetInputBuffer(tVolumeControl *pVol);

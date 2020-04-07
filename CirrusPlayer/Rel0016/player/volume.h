//****************************************************************************
//
// VOLUME.H - Contains function prototypes for the routines in the volume
//            control.
//
// Copyright (c) 1999,2000,2001 Cirrus Logic, Inc.
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
    unsigned short usVolume;

    //
    // A boolean which is true if the audio should be passed through the volume
    // control.
    //
    unsigned short bProcessAudio;

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
extern void VolumeEnable(tVolumeControl *pVol);
extern void VolumeDisable(tVolumeControl *pVol);
extern BufferState *VolumeGetInputBuffer(tVolumeControl *pVol);

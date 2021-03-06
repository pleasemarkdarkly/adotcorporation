//****************************************************************************
//
// SPATIAL.H - Definitions for the interface to the Spatializer library.
//
// Copyright (c) 2001 Cirrus Logic, Inc.
//
//****************************************************************************
#ifdef SUPPORT_SPATIALIZER
#include "buffer/buffer.h"
#include "spatial/vibe.h"

//****************************************************************************
//
// The persistent state of the QSound control.
//
//****************************************************************************
typedef struct
{
    //
    // The persistent state of the Spatializer control.
    //
    VIBE sVibe;

    //
    // A boolean which is true if the audio should be passed through the
    // Spatializer control.
    //
    unsigned long bProcessAudio;

    //
    // The buffer from which we receive input data.
    //
    BufferState sInputBuffer;

    //
    // The buffer to which we send output data.
    //
    BufferState *pOutputBuffer;
} tSpatializerControl;

//****************************************************************************
//
// Function prototypes.
//
//****************************************************************************
extern void SpatializerInit(tSpatializerControl *pSpatial,
                            BufferState *pOutputBuffer);
extern void SpatializerAdjustBass(tSpatializerControl *pSpatial, long lBass);
extern void SpatializerAdjustVirtualBass(tSpatializerControl *pSpatial,
                                         long lBass);
extern void SpatializerAdjustGain(tSpatializerControl *pSpatial, long lGain);
extern void SpatializerEnable(tSpatializerControl *pSpatial);
extern void SpatializerDisable(tSpatializerControl *pSpatial);
extern BufferState *SpatializerGetInputBuffer(tSpatializerControl *pSpatial);
#endif

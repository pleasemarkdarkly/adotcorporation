//****************************************************************************
//
// SPATIAL.H - Definitions for the interface to the Spatializer library.
//
// Copyright (c) 2001 Cirrus Logic, Inc.
//
//****************************************************************************
#ifdef SUPPORT_SPATIALIZER
#include "buffer/buffer.h"
#include "spatial/digitalfx.h"

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
    DIGITAL_EFFECTS sDigitalFX;

    //
    // A boolean which is true if there are parameters which need to be
    // committed.
    //
    unsigned long bParametersChanged;

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
extern void SpatializerAdjustSpace(tSpatializerControl *pSpatial, long lSpace);
extern void SpatializerAdjustCenter(tSpatializerControl *pSpatial,
                                    long lCenter);
extern void SpatializerAdjustHeadphone(tSpatializerControl *pSpatial,
                                       long lHeadphone);
extern void SpatializerAdjustBass(tSpatializerControl *pSpatial, long lBass);
extern void SpatializerAdjustVirtualBass(tSpatializerControl *pSpatial,
                                         long lBass);
extern void SpatializerAdjustBassColor(tSpatializerControl *pSpatial,
                                       long lBassColor);
extern void SpatializerAdjustLoudness(tSpatializerControl *pSpatial,
                                      long lLoudness);
extern void SpatializerAdjustVolume(tSpatializerControl *pSpatial,
                                    long lVolume);
extern BufferState *SpatializerGetInputBuffer(tSpatializerControl *pSpatial);
#endif

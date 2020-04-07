//****************************************************************************
//
// SRS.H - Definitions for the interface to the SRS Labs embedded device
//         library.
//
// Copyright (c) 2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************
#ifdef SUPPORT_SRS
#include "buffer/buffer.h"
#include "srs/SRS_eda.h"

//****************************************************************************
//
// The persistent state of the SRS control.
//
//****************************************************************************
typedef struct
{
    //
    // The persistent state of the SRS control.
    //
#ifdef SUPPORT_WOW_FULL
    WowChannel sSRS;
#endif
#ifdef SUPPORT_WOW_MINIMAL
    WowChannel sSRS;
#endif
#ifdef SUPPORT_TRU_BASS
    TruBassChannel sSRS;
#endif
#ifdef SUPPORT_SRS_HEADPHONE
    HeadphoneChannel sSRS;
#endif

    //
    // A boolean which is true if the audio should be passed through the SRS
    // control.
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
} tSRSControl;

//****************************************************************************
//
// Function prototypes.
//
//****************************************************************************
extern void SRSInit(tSRSControl *pSRS, BufferState *pOutputBuffer);
extern void SRSAdjustWidth(tSRSControl *pSRS, long lWidth);
extern void SRSAdjustTruBass(tSRSControl *pSRS, long lTruBass);
extern void SRSEnable(tSRSControl *pSRS);
extern void SRSDisable(tSRSControl *pSRS);
extern BufferState *SRSGetInputBuffer(tSRSControl *pSRS);
#endif
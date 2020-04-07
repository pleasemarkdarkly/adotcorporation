//****************************************************************************
//
// QSOUND.H - Definitions for the interface to the QSound Q2Xpander library.
//
// Copyright (c) 2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************
#ifdef SUPPORT_QSOUND
#include "buffer/buffer.h"
#include "qsound/q2x.h"

//****************************************************************************
//
// The persistent state of the QSound control.
//
//****************************************************************************
typedef struct
{
    //
    // The persistent state of the QSound control.
    //
    Q2XState sQ2X;

    //
    // A boolean which is true if the audio should be passed through the QSound
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
} tQSoundControl;

//****************************************************************************
//
// Function prototypes.
//
//****************************************************************************
extern void QSoundInit(tQSoundControl *pQSound, BufferState *pOutputBuffer);
extern void QSoundAdjustWidth(tQSoundControl *pQSound, long lWidth);
extern void QSoundSetMode(tQSoundControl *pQSound, long lMode);
extern void QSoundEnable(tQSoundControl *pQSound);
extern void QSoundDisable(tQSoundControl *pQSound);
extern BufferState *QSoundGetInputBuffer(tQSoundControl *pQSound);
#endif

//****************************************************************************
//
// TONECTRL.H - Public definitions for the tone control library.
//
// Copyright (c) 1999-2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include "../buffer/buffer.h"

//****************************************************************************
//
// The persistent state of the tone control.
//
//****************************************************************************
#ifndef _TONECTRL_I_H
typedef struct
{
    //
    // The internal, persistent state of the tone control.
    //
    unsigned char ucInternal[36];

    //
    // The buffer from which we receive input data.
    //
    BufferState sInputBuffer;
} tToneControl;
#endif

//****************************************************************************
//
// Function prototypes.
//
//****************************************************************************
extern void ToneInit(tToneControl *pTC, BufferState *pOutputBuffer);
extern void ToneAdjust(tToneControl *pTC, long lTreble, long lBass,
                       long lGain);
extern void ToneAdjustTreble(tToneControl *pTC, long lTreble);
extern void ToneAdjustBass(tToneControl *pTC, long lBass);
extern void ToneAdjustGain(tToneControl *pTC, long lGain);
extern BufferState *ToneGetInputBuffer(tToneControl *pTC);

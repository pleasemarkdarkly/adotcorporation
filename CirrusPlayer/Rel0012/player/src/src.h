//****************************************************************************
//
// SRC.H - Public definitions for the sample rate converter library.
//
// Copyright (c) 1999-2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include "../buffer/buffer.h"

//****************************************************************************
//
// The number of taps in the sample rate conversion filters.  This is an
// indication of the number of MACs in the unrolled filter loop.  There is a
// direct correlation between the number of taps and the execution time of
// the sample rate converter.  There is one MAC per tap per output sample.
// This is also the amount of extra memory required before any input data
// buffer passed to the sample rate converter.
//
//****************************************************************************
#define NUMTAPS 13

//****************************************************************************
//
// The perisistent state of the sample rate converter.
//
//****************************************************************************
#ifndef _SRC_I_H
typedef struct
{
    //
    // The internal, persistent state of the sample rate converter.
    //
    unsigned char ucInternal[16];

    //
    // The buffer from which we receive input data.
    //
    BufferState sInputBuffer;
} tSRC;
#endif

//****************************************************************************
//
// Function prototypes.
//
//****************************************************************************
extern int SRCInit(tSRC *pSRC, unsigned long ulInputRate,
                   unsigned long ulOutputRate);
extern BufferState *SRCGetInputBuffer(tSRC *pSRC);
extern long SRCFilter(tSRC *pSRC, short *psLeft, short *psRight,
                      long lNumSamples);
extern long SRCNumSamplesAvailable(tSRC *pSRC);

//****************************************************************************
//
// SPECTRUM.H - Definitions for the spectrum analyzer.
//
// Copyright (c) 2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include "buffer/buffer.h"

//****************************************************************************
//
// The number of bands in the spectrum analyzer.
//
//****************************************************************************
#define NUMBANDS 5

//****************************************************************************
//
// The number of buckets into which filtered samples are accumulated.
//
//****************************************************************************
#define NUMBUCKETS 8

//****************************************************************************
//
// The number of filtered samples accumulated into each bucket.  Should be a
// power of two (or a serious performance penalty will be taken).
//
//****************************************************************************
#define NUMSAMPLES 512

//****************************************************************************
//
// The persistent state of the spectrum analyzer.
//
//****************************************************************************
typedef struct
{
    //
    // The filter used to determine the spectral content of the input sample
    // stream.
    //
    const short *psFilter;

    //
    // The x(n-1), x(n-2), y(n-1), and y(n-2) memories for the IIR filters.
    //
    short sMemory[((NUMBANDS - 1) * 2) + 3];

    //
    // A boolean which is true if the audio should be passed through the
    // spectrum analyzer.
    //
    unsigned short bProcessAudio;

    //
    // Buckets which contain the accumulated filtered samples for each band of
    // the spectrum analyzer.
    //
    unsigned long ulBucket[NUMBANDS][NUMBUCKETS];

    //
    // The bucket which is currently being filled.
    //
    unsigned short usCurrentBucket;

    //
    // The number of samples that have been accumulated in the current bucket.
    //
    unsigned short usSamplesInBucket;

    //
    // The buffer from which we receive input data.
    //
    BufferState sInputBuffer;

    //
    // The buffer to which we send output data.
    //
    BufferState *pOutputBuffer;
} tSpectrumAnalyzer;

//****************************************************************************
//
// Function prototypes.
//
//****************************************************************************
extern void SpectrumInit(tSpectrumAnalyzer *pSpectrum,
                         BufferState *pOutputBuffer);
extern void SpectrumGetBands(tSpectrumAnalyzer *pSpectrum, long *plBands);
extern void SpectrumEnable(tSpectrumAnalyzer *pSpectrum);
extern void SpectrumDisable(tSpectrumAnalyzer *pSpectrum);
extern BufferState *SpectrumGetInputBuffer(tSpectrumAnalyzer *pSpectrum);

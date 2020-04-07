//****************************************************************************
//
// SPECTRUM.C - Software spectrum analyzer.
//
// Copyright (c) 2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include "spectrum.h"

//****************************************************************************
//
// The filters used to extract the spectral content for the various bands.
//
//****************************************************************************
#if NUMBANDS == 5
static const short ppsFilter[3][22] =
{
    //
    // Sample rate = 32000
    //
    {
        //
        // Low pass (0 - 110)
        //
        175, 16034,

        //
        // Band pass (110 - 220)
        //
        -164, 415, -249, 32582, -16214,

        //
        // Band pass (220 - 622)
        //
        -542, 1698, -1154, 32041, -15771,

        //
        // Band pass (622 - 1760)
        //
        18, 1666, -1683, 30176, -14682,

        //
        // High pass (1760 - 16000)
        //
        12816, 24778, -10102
    },

    //
    // Sample rate = 44100
    //
    {
        //
        // Low pass (0 - 110)
        //
        127, 16129,

        //
        // Band pass (110 - 220)
        //
        -176, 415, -237, 32636, -16261,

        //
        // Band pass (220 - 622)
        //
        -627, 1700, -1072, 32262, -15938,

        //
        // Band pass (622 - 1760)
        //
        -219, 1685, -1463, 31047, -15139,

        //
        // High pass (1760 - 22050)
        //
        13718, 26959, -11528
    },

    //
    // Sample rate = 48000
    //
    {
        //
        // Low pass (0 - 110)
        //
        117, 16150,

        //
        // Band pass (110 - 220)
        //
        -178, 415, -235, 32648, -16271,

        //
        // Band pass (220 - 622)
        //
        -645, 1701, -1054, 32307, -15974,

        //
        // Band pass (622 - 1760)
        //
        -271, 1688, -1415, 31219, -15238,

        //
        // High pass (1760 - 24000)
        //
        13919, 27430, -11861
    }
};
#endif

//****************************************************************************
//
// Determines the absolute value of the given value.
//
//****************************************************************************
#define ABS(x) ((x) >= 0 ? (x) : 0 - (x))

//****************************************************************************
//
// Clears the state of the spectrum analyzer.
//
//****************************************************************************
static void
SpectrumClear(tSpectrumAnalyzer *pSpectrum)
{
    unsigned long ulBand, ulBucket;

    //
    // Loop through all the bands in the spectrum analyzer.
    //
    for(ulBand = 0; ulBand < NUMBANDS; ulBand++)
    {
        //
        // Loop through all the buckets in this band.
        //
        for(ulBucket = 0; ulBucket < NUMBUCKETS; ulBucket++)
        {
            //
            // Clear this bucket.
            //
            pSpectrum->ulBucket[ulBand][ulBucket] = 0;
        }
    }

    //
    // Clear the delay lines.
    //
    for(ulBand = 0; ulBand < (((NUMBANDS - 1) * 2) + 3); ulBand++)
    {
        pSpectrum->sMemory[ulBand] = 0;
    }

    //
    // Reset back to the first bucket.
    //
    pSpectrum->usCurrentBucket = 0;

    //
    // There are no samples in the current bucket.
    //
    pSpectrum->usSamplesInBucket = 0;
}

//****************************************************************************
//
// FilterMono applies the tone control filter to the given mono sample stream.
//
//****************************************************************************
static void
FilterMono(tSpectrumAnalyzer *pSpectrum, short *psInData, long lNumSamples)
{
    const short *psCoeffPtr;
    short *psMemPtr, sSample, sValue, sBand;

    //
    // Loop through all the input samples.
    //
    while(lNumSamples--)
    {
        //
        // Get a pointer to the coefficients.
        //
        psCoeffPtr = pSpectrum->psFilter;

        //
        // Get a pointer to the delay line memory.
        //
        psMemPtr = pSpectrum->sMemory + 2;

        //
        // Get the next sample value.
        //
        sSample = *psInData++;

        //
        // The first band is a low pass filter.
        //
        sValue = ((sSample * psCoeffPtr[0]) +
                  (*psMemPtr * psCoeffPtr[1])) >> 14;

        //
        // Save the output value for the next iteration of the filter.
        //
        *psMemPtr++ = sValue;

        //
        // Skip this filter's coefficients.
        //
        psCoeffPtr += 2;

        //
        // Add the absolute value of this sample (which is indicative of it's
        // spectral power) to the current bucket.
        //
        pSpectrum->ulBucket[0][pSpectrum->usCurrentBucket] += ABS(sValue);

        //
        // Loop through the band pass filters.
        //
        for(sBand = 1; sBand < (NUMBANDS - 1); sBand++)
        {
            //
            // Filter this sample.
            //
            sValue = ((sSample * psCoeffPtr[0]) +
                      (pSpectrum->sMemory[0] * psCoeffPtr[1]) +
                      (pSpectrum->sMemory[1] * psCoeffPtr[2]) +
                      (psMemPtr[0] * psCoeffPtr[3]) +
                      (psMemPtr[1] * psCoeffPtr[4])) >> 14;

            //
            // Update the output sample delay line.
            //
            psMemPtr[1] = psMemPtr[0];
            psMemPtr[0] = sValue;

            //
            // Skip this filter's delay line.
            //
            psMemPtr += 2;

            //
            // Skip this filter's coefficients.
            //
            psCoeffPtr += 5;

            //
            // Add the absolute value of this sample (which is indicative of
            // it's spectral power) to the current bucket.
            //
            pSpectrum->ulBucket[sBand][pSpectrum->usCurrentBucket] +=
                ABS(sValue);
        }

        //
        // The last band is a Chebyshev high pass filter.
        //
        sValue = (((sSample - pSpectrum->sMemory[0] - pSpectrum->sMemory[0] +
                    pSpectrum->sMemory[1]) * psCoeffPtr[0]) +
                  (psMemPtr[0] * psCoeffPtr[1]) +
                  (psMemPtr[1] * psCoeffPtr[2])) >> 14;

        //
        // Update the output sample delay line.
        //
        psMemPtr[1] = psMemPtr[0];
        psMemPtr[0] = sValue;

        //
        // Add the absolute value of this sample (which is indicative of it's
        // spectral power) to the current bucket.
        //
        pSpectrum->ulBucket[sBand][pSpectrum->usCurrentBucket] += ABS(sValue);

        //
        // Update the input sample delay line.
        //
        pSpectrum->sMemory[1] = pSpectrum->sMemory[0];
        pSpectrum->sMemory[0] = sSample;

        //
        // Increment the number of samples in this bucket.
        //
        pSpectrum->usSamplesInBucket++;

        //
        // See if this bucket is now full.
        //
        if(pSpectrum->usSamplesInBucket == NUMSAMPLES)
        {
            //
            // The bucket is full, so advance to the next bucket.
            //
            pSpectrum->usCurrentBucket = ((pSpectrum->usCurrentBucket + 1) &
                                          (NUMBUCKETS - 1));

            //
            // There are no samples in the new bucket.
            //
            pSpectrum->usSamplesInBucket = 0;

            //
            // Clear the previous contents of this bucket.
            //
            for(sBand = 0; sBand < NUMBANDS; sBand++)
            {
                pSpectrum->ulBucket[sBand][pSpectrum->usCurrentBucket] = 0;
            }
        }
    }
}

//****************************************************************************
//
// FilterStereo applies the tone control filter to the given pair of mono
// sample streams.
//
//****************************************************************************
static void
FilterStereo(tSpectrumAnalyzer *pSpectrum, short *psInDataLeft,
             short *psInDataRight, long lNumSamples)
{
    const short *psCoeffPtr;
    short *psMemPtr, sSample, sValue, sBand;

    //
    // Loop through all the input samples.
    //
    while(lNumSamples--)
    {
        //
        // Get a pointer to the coefficients.
        //
        psCoeffPtr = pSpectrum->psFilter;

        //
        // Get a pointer to the delay line memory.
        //
        psMemPtr = pSpectrum->sMemory + 2;

        //
        // Get the next sample value.
        //
        sSample = (*psInDataLeft++ + *psInDataRight++) >> 1;

        //
        // The first band is a low pass filter.
        //
        sValue = ((sSample * psCoeffPtr[0]) +
                  (*psMemPtr * psCoeffPtr[1])) >> 14;

        //
        // Save the output value for the next iteration of the filter.
        //
        *psMemPtr++ = sValue;

        //
        // Skip this filter's coefficients.
        //
        psCoeffPtr += 2;

        //
        // Add the absolute value of this sample (which is indicative of it's
        // spectral power) to the current bucket.
        //
        pSpectrum->ulBucket[0][pSpectrum->usCurrentBucket] += ABS(sValue);

        //
        // Loop through the band pass filters.
        //
        for(sBand = 1; sBand < (NUMBANDS - 1); sBand++)
        {
            //
            // Filter this sample.
            //
            sValue = ((sSample * psCoeffPtr[0]) +
                      (pSpectrum->sMemory[0] * psCoeffPtr[1]) +
                      (pSpectrum->sMemory[1] * psCoeffPtr[2]) +
                      (psMemPtr[0] * psCoeffPtr[3]) +
                      (psMemPtr[1] * psCoeffPtr[4])) >> 14;

            //
            // Update the output sample delay line.
            //
            psMemPtr[1] = psMemPtr[0];
            psMemPtr[0] = sValue;

            //
            // Skip this filter's delay line.
            //
            psMemPtr += 2;

            //
            // Skip this filter's coefficients.
            //
            psCoeffPtr += 5;

            //
            // Add the absolute value of this sample (which is indicative of
            // it's spectral power) to the current bucket.
            //
            pSpectrum->ulBucket[sBand][pSpectrum->usCurrentBucket] +=
                ABS(sValue);
        }

        //
        // The last band is a Chebyshev high pass filter.
        //
        sValue = (((sSample - pSpectrum->sMemory[0] - pSpectrum->sMemory[0] +
                    pSpectrum->sMemory[1]) * psCoeffPtr[0]) +
                  (psMemPtr[0] * psCoeffPtr[1]) +
                  (psMemPtr[1] * psCoeffPtr[2])) >> 14;

        //
        // Update the output sample delay line.
        //
        psMemPtr[1] = psMemPtr[0];
        psMemPtr[0] = sValue;

        //
        // Add the absolute value of this sample (which is indicative of it's
        // spectral power) to the current bucket.
        //
        pSpectrum->ulBucket[sBand][pSpectrum->usCurrentBucket] += ABS(sValue);

        //
        // Update the input sample delay line.
        //
        pSpectrum->sMemory[1] = pSpectrum->sMemory[0];
        pSpectrum->sMemory[0] = sSample;

        //
        // Increment the number of samples in this bucket.
        //
        pSpectrum->usSamplesInBucket++;

        //
        // See if this bucket is now full.
        //
        if(pSpectrum->usSamplesInBucket == NUMSAMPLES)
        {
            //
            // The bucket is full, so advance to the next bucket.
            //
            pSpectrum->usCurrentBucket = ((pSpectrum->usCurrentBucket + 1) &
                                          (NUMBUCKETS - 1));

            //
            // There are no samples in the new bucket.
            //
            pSpectrum->usSamplesInBucket = 0;

            //
            // Clear the previous contents of this bucket.
            //
            for(sBand = 0; sBand < NUMBANDS; sBand++)
            {
                pSpectrum->ulBucket[sBand][pSpectrum->usCurrentBucket] = 0;
            }
        }
    }
}

//****************************************************************************
//
// SpectrumBufferIoctl is the buffer IOCTL handler for the tone control input
// buffer.
//
//****************************************************************************
static long
SpectrumBufferIoctl(unsigned long ulIoctl, BufferState *psBuffer,
                    unsigned long ulParam1, unsigned long ulParam2,
                    unsigned long ulParam3)
{
    //
    // Determine what to do based on the specified IOCTL.
    //
    switch(ulIoctl)
    {
        //
        // Set the input buffer.
        //
        case IOCTL_BUFFER_SETBUFFER:
        {
            tSpectrumAnalyzer *pSpectrum;

            //
            // Get a pointer to the tone control state structure.
            //
            pSpectrum = (tSpectrumAnalyzer *)psBuffer->lInstanceData;

            //
            // Clear the sample buckets and delay lines.
            //
            SpectrumClear(pSpectrum);

            //
            // Set the buffer of the output buffer.
            //
            return(BufferSetBuffer(pSpectrum->pOutputBuffer, (short *)ulParam1,
                                   (short *)ulParam2, (long)ulParam3));
        }

        //
        // Determine the amount of space available in the output buffer.
        //
        case IOCTL_BUFFER_SPACEAVAILABLE:
        {
            tSpectrumAnalyzer *pSpectrum;

            //
            // Get a pointer to the tone control state structure.
            //
            pSpectrum = (tSpectrumAnalyzer *)psBuffer->lInstanceData;

            //
            // Return the amount of space available in the output buffer.
            //
            return(BufferSpaceAvailable(pSpectrum->pOutputBuffer));
        }

        //
        // Determine if the output buffer is empty.
        //
        case IOCTL_BUFFER_ISEMPTY:
        {
            tSpectrumAnalyzer *pSpectrum;

            //
            // Get a pointer to the tone control state structure.
            //
            pSpectrum = (tSpectrumAnalyzer *)psBuffer->lInstanceData;

            //
            // Return whether or not the output buffer is empty.
            //
            return(BufferIsEmpty(pSpectrum->pOutputBuffer));
        }

        //
        // Get the write pointer.
        //
        case IOCTL_BUFFER_GETWRITEPOINTER:
        {
            tSpectrumAnalyzer *pSpectrum;

            //
            // Get a pointer to the tone control state structure.
            //
            pSpectrum = (tSpectrumAnalyzer *)psBuffer->lInstanceData;

            //
            // Return the write pointer of the output buffer.
            //
            return(BufferGetWritePointer(pSpectrum->pOutputBuffer,
                                         (short **)ulParam1,
                                         (short **)ulParam2,
                                         (long *)ulParam3));
        }

        //
        // Update the write pointer.
        //
        case IOCTL_BUFFER_UPDATEWRITEPOINTER:
        {
            tSpectrumAnalyzer *pSpectrum;
            short *psLeft, *psRight;
            long lLoop, lLength;

            //
            // Get a pointer to the spectrum analzye state structure.
            //
            pSpectrum = (tSpectrumAnalyzer *)psBuffer->lInstanceData;

            //
            // See if the spectrum analyzer is enabled.
            //
            if(!pSpectrum->bProcessAudio)
            {
                //
                // The spectrum analyzer is disabled, so simply pass this
                // update request to the output buffer.
                //
                return(BufferUpdateWritePointer(pSpectrum->pOutputBuffer,
                                                ulParam1));
            }

            //
            // Get the current write pointer for the output buffer.
            //
            BufferGetWritePointer(pSpectrum->pOutputBuffer, &psLeft, &psRight,
                                  &lLength);

            //
            // See if there is enough space in this buffer.
            //
            if(lLength < (long)ulParam1)
            {
                return(0);
            }

            //
            // If there is a right channel buffer, then perform spectral
            // analysis on the stereo stream.
            //
            if(psLeft != psRight)
            {
                //
                // Loop through the samples in four separate blocks.  This is
                // to prevent starving the downstream task (performing four
                // separate buffer updates instead of one large one).
                //
                for(lLoop = 0; lLoop < (long)ulParam1;
                    lLoop += ((ulParam1 + 15) / 4) & ~3)
                {
                    //
                    // Determine the number of samples to process in this
                    // block.
                    //
                    lLength = ((ulParam1 + 15) / 4) & ~3;
                    if((lLoop + lLength) > ulParam1)
                    {
                        lLength = ulParam1 - lLoop;
                    }

                    //
                    // Process this block of samples.
                    //
                    FilterStereo(pSpectrum, psLeft + lLoop, psRight + lLoop,
                                 lLength);

                    //
                    // Update the buffer write pointer.
                    //
                    BufferUpdateWritePointer(pSpectrum->pOutputBuffer,
                                             lLength);
                }
            }

            //
            // Otherwise, perform spectral analysis on the mono stream.
            //
            else
            {
                //
                // Loop through the samples in four separate blocks.  This is
                // to prevent starving the downstream task (performing four
                // separate buffer updates instead of one large one).
                //
                for(lLoop = 0; lLoop < (long)ulParam1;
                    lLoop += ((ulParam1 + 15) / 4) & ~3)
                {
                    //
                    // Determine the number of samples to process in this
                    // block.
                    //
                    lLength = ((ulParam1 + 15) / 4) & ~3;
                    if((lLoop + lLength) > ulParam1)
                    {
                        lLength = ulParam1 - lLoop;
                    }

                    //
                    // Process this block of samples.
                    //
                    FilterMono(pSpectrum, psLeft + lLoop, lLength);

                    //
                    // Update the buffer write pointer.
                    //
                    BufferUpdateWritePointer(pSpectrum->pOutputBuffer,
                                             lLength);
                }
            }

            //
            // Success.
            //
            return(1);
        }

        //
        // Set the sample rate of the data buffer.
        //
        case IOCTL_BUFFER_SETSAMPLERATE:
        {
            tSpectrumAnalyzer *pSpectrum;

            //
            // Get a pointer to the tone control state structure.
            //
            pSpectrum = (tSpectrumAnalyzer *)psBuffer->lInstanceData;

            //
            // We only support 32kHz, 44.1kHz, and 48kHz sample rates.
            //
            if((ulParam1 != 32000) && (ulParam1 != 44100) &&
               (ulParam1 != 48000))
            {
                return(0);
            }

            //
            // Pass the set sample rate request to the output buffer.
            //
            if(BufferSetSampleRate(pSpectrum->pOutputBuffer,
                                   (long)ulParam1, (long)ulParam2) == 0)
            {
                return(0);
            }

            //
            // Adjust the filter coefficents based on the sample rate.
            //
            switch(ulParam1)
            {
                //
                // The sample rate is 32000Hz.
                //
                case 32000:
                {
                    //
                    // Use the 32kHz filter.
                    //
                    pSpectrum->psFilter = ppsFilter[0];

                    //
                    // We're done with this sample rate.
                    //
                    break;
                }

                //
                // The sample rate is 44100Hz.
                //
                case 44100:
                {
                    //
                    // Use the 44.1kHz filter.
                    //
                    pSpectrum->psFilter = ppsFilter[1];

                    //
                    // We're done with this sample rate.
                    //
                    break;
                }

                //
                // The sample rate is 48000Hz.
                //
                case 48000:
                {
                    //
                    // Use the 48kHz filter.
                    //
                    pSpectrum->psFilter = ppsFilter[2];

                    //
                    // We're done with this sample rate.
                    //
                    break;
                }
            }

            //
            // Clear the sample buckets and delay lines.
            //
            SpectrumClear(pSpectrum);

            //
            // Success.
            //
            return(1);
        }

        //
        // Fail all other IOCTLs as we do not want to allow any other actions
        // to be performed on the input buffer.
        //
        default:
        {
            return(0);
        }
    }
}

//****************************************************************************
//
// SpectrumInit sets the initial state of the tone control filter.
//
//****************************************************************************
void
SpectrumInit(tSpectrumAnalyzer *pSpectrum, BufferState *pOutputBuffer)
{
    //
    // Initialize the delay lines to silence.
    //
    SpectrumClear(pSpectrum);

    //
    // The default filter to use is the 44.1kHz filter.
    //
    pSpectrum->psFilter = ppsFilter[1];

    //
    // The spectrum analyzer is enabled by default.
    //
    pSpectrum->bProcessAudio = 1;

    //
    // Initialize the input buffer.
    //
    BufferInit(&(pSpectrum->sInputBuffer), SpectrumBufferIoctl,
               (long)pSpectrum);

    //
    // Save the pointer to the output buffer.
    //
    pSpectrum->pOutputBuffer = pOutputBuffer;
}

//****************************************************************************
//
// SpectrumGetBands gets the current value for the spectrum analzyer's bands.
//
//****************************************************************************
void
SpectrumGetBands(tSpectrumAnalyzer *pSpectrum, long *plBands)
{
    unsigned long ulBand, ulIdx, ulSum;

    //
    // Loop through all the bands.
    //
    for(ulBand = 0; ulBand < NUMBANDS; ulBand++)
    {
        //
        // Loop through all the buckets in this band.
        //
        for(ulIdx = 0, ulSum = 0; ulIdx < NUMBUCKETS; ulIdx++)
        {
            //
            // See if this is the current bucket.
            //
            if(ulIdx != pSpectrum->usCurrentBucket)
            {
                //
                // This is not the current bucket, so add the samples from this
                // bucket to the running total.
                //
                ulSum += pSpectrum->ulBucket[ulBand][ulIdx];
            }
        }

        //
        // Return the average of the samples in the buckets.
        //
#if NUMBUCKETS == 8
        plBands[ulBand] = (((ulSum / NUMSAMPLES) * 73) + 36) >> 9;
#elif NUMBUCKETS == 16
        plBands[ulBand] = (((ulSum / NUMSAMPLES) * 273) + 136) >> 12;
#else
        plBands[ulBand] /= (NUMSAMPLES * (NUMBUCKETS - 1));
#endif
    }
}

//****************************************************************************
//
// SpectrumEnable enables the spectrum analyzer processing of the audio
// stream.
//
//****************************************************************************
void
SpectrumEnable(tSpectrumAnalyzer *pSpectrum)
{
    //
    // Enable the spectrum analyzer.
    //
    pSpectrum->bProcessAudio = 1;
}

//****************************************************************************
//
// SpectrumDisable disables the spectrum analzyer processing of the audio
// stream.
//
//****************************************************************************
void
SpectrumDisable(tSpectrumAnalyzer *pSpectrum)
{
    //
    // Disable the spectrum analyzer.
    //
    pSpectrum->bProcessAudio = 0;
}

//****************************************************************************
//
// SpectrumGetInputBuffer returns a pointer to the input buffer for the tone
// control.
//
//****************************************************************************
BufferState *
SpectrumGetInputBuffer(tSpectrumAnalyzer *pSpectrum)
{
    //
    // Return a pointer to the input buffer.
    //
    return(&(pSpectrum->sInputBuffer));
}

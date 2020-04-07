//****************************************************************************
//
// SPATIAL.C - Interface to the Spatializer library.
//
// Copyright (c) 2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include "globals.h"
#include "spatial.h"

//****************************************************************************
//
// SpatializerBufferIoctl is the buffer IOCTL handler for the Spatializer
// control input buffer.
//
//****************************************************************************
static long
SpatializerBufferIoctl(unsigned long ulIoctl, BufferState *psBuffer,
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
            tSpatializerControl *pSpatializer;

            //
            // Get a pointer to the Spatializer control state structure.
            //
            pSpatializer = (tSpatializerControl *)psBuffer->lInstanceData;

            //
            // Set the buffer of the output buffer.
            //
            return(BufferSetBuffer(pSpatializer->pOutputBuffer,
                                   (short *)ulParam1, (short *)ulParam2,
                                   (long)ulParam3));
        }

        //
        // Determine the amount of space available in the output buffer.
        //
        case IOCTL_BUFFER_SPACEAVAILABLE:
        {
            tSpatializerControl *pSpatializer;

            //
            // Get a pointer to the Spatializer control state structure.
            //
            pSpatializer = (tSpatializerControl *)psBuffer->lInstanceData;

            //
            // Return the amount of space available in the output buffer.
            //
            return(BufferSpaceAvailable(pSpatializer->pOutputBuffer));
        }

        //
        // Determine if the output buffer is empty.
        //
        case IOCTL_BUFFER_ISEMPTY:
        {
            tSpatializerControl *pSpatializer;

            //
            // Get a pointer to the Spatializer control state structure.
            //
            pSpatializer = (tSpatializerControl *)psBuffer->lInstanceData;

            //
            // Return whether or not the output buffer is empty.
            //
            return(BufferIsEmpty(pSpatializer->pOutputBuffer));
        }

        //
        // Get the write pointer.
        //
        case IOCTL_BUFFER_GETWRITEPOINTER:
        {
            tSpatializerControl *pSpatializer;

            //
            // Get a pointer to the Spatializer control state structure.
            //
            pSpatializer = (tSpatializerControl *)psBuffer->lInstanceData;

            //
            // Return the write pointer of the output buffer.
            //
            return(BufferGetWritePointer(pSpatializer->pOutputBuffer,
                                         (short **)ulParam1,
                                         (short **)ulParam2,
                                         (long *)ulParam3));
        }

        //
        // Update the write pointer.
        //
        case IOCTL_BUFFER_UPDATEWRITEPOINTER:
        {
            tSpatializerControl *pSpatializer;
            short *psLeft, *psRight;
            long lLoop, lLength;

            //
            // Get a pointer to the Spatializer control state structure.
            //
            pSpatializer = (tSpatializerControl *)psBuffer->lInstanceData;

            //
            // See if the Spatializer control is enabled.
            //
            if(!pSpatializer->bProcessAudio)
            {
                //
                // The Spatializer control is disabled, so simply pass this
                // update request to the output buffer.
                //
                return(BufferUpdateWritePointer(pSpatializer->pOutputBuffer,
                                                ulParam1));
            }

            //
            // Get the current write pointer for the output buffer.
            //
            BufferGetWritePointer(pSpatializer->pOutputBuffer, &psLeft,
                                  &psRight, &lLength);

            //
            // See if there is enough space in this buffer.
            //
            if(lLength < (long)ulParam1)
            {
                return(0);
            }

            //
            // If there is a right channel buffer, then perform stereo
            // Spatializer processing.
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
                    vibe_process_block(&(pSpatializer->sVibe),
                                       psLeft + lLoop, psRight + lLoop,
                                       lLength);

                    //
                    // Update the buffer write pointer.
                    //
                    BufferUpdateWritePointer(pSpatializer->pOutputBuffer,
                                             lLength);
                }
            }
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
                    vibe_process_block(&(pSpatializer->sVibe), psLeft + lLoop,
                                       psLeft + lLoop, lLength);

                    //
                    // Update the buffer write pointer.
                    //
                    BufferUpdateWritePointer(pSpatializer->pOutputBuffer,
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
            tSpatializerControl *pSpatializer;
            long lGain, lLpf, lVirtual;

            //
            // Get a pointer to the Spatializer control state structure.
            //
            pSpatializer = (tSpatializerControl *)psBuffer->lInstanceData;

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
            if(BufferSetSampleRate(pSpatializer->pOutputBuffer,
                                   (long)ulParam1, (long)ulParam2) == 0)
            {
                return(0);
            }

            //
            // Get the current settings.
            //
            lGain = vibe_get_gain(&(pSpatializer->sVibe));
            lLpf = vibe_get_gain_lpf(&(pSpatializer->sVibe));
            lVirtual = vibe_get_gain_virt(&(pSpatializer->sVibe));

            //
            // Adjust the Q2X coefficients based on the sample rate.
            //
            switch(ulParam1)
            {
                //
                // The requested sample rate is 32kHz.
                //
                case 32000:
                {
                    //
                    // Set the Spatializer sample rate to 32kHz.
                    //
                    vibe_initialize(&(pSpatializer->sVibe), 32000);

                    //
                    // We're done handling this rate.
                    //
                    break;
                }

                //
                // The requested sample rate is 44.1kHz.
                //
                case 44100:
                {
                    //
                    // Set the Spatializer sample rate to 44.1kHz.
                    //
                    vibe_initialize(&(pSpatializer->sVibe), 44100);

                    //
                    // We're done handling this rate.
                    //
                    break;
                }

                //
                // The requested sample rate is 48kHz.
                //
                case 48000:
                {
                    //
                    // Set the Spatializer sample rate to 48kHz.
                    //
                    vibe_initialize(&(pSpatializer->sVibe), 48000);

                    //
                    // We're done handling this rate.
                    //
                    break;
                }
            }

            //
            // Re-initialize the settings.
            //
            vibe_set_gain(&(pSpatializer->sVibe), lGain);
            vibe_set_effect(&(pSpatializer->sVibe), lLpf, lVirtual);

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
// SpatializerInit sets the initial state of the Spatializer control filter.
//
//****************************************************************************
void
SpatializerInit(tSpatializerControl *pSpatial, BufferState *pOutputBuffer)
{
    //
    // Set the default state of the Spatializer control.
    //
    vibe_initialize(&(pSpatial->sVibe), 44100);
    vibe_set_gain(&(pSpatial->sVibe), 0x1000);
    vibe_set_effect(&(pSpatial->sVibe), 0, 0);

    //
    // The Spatializer control is enabled by default.
    //
    pSpatial->bProcessAudio = 1;

    //
    // Initialize the input buffer.
    //
    BufferInit(&(pSpatial->sInputBuffer), SpatializerBufferIoctl,
               (long)pSpatial);

    //
    // Save the pointer to the output buffer.
    //
    pSpatial->pOutputBuffer = pOutputBuffer;
}

//****************************************************************************
//
// SpatializerAdjustBass adjusts the bass control of the Spatializer
// processing.
//
//****************************************************************************
void
SpatializerAdjustBass(tSpatializerControl *pSpatial, long lBass)
{
    //
    // Make sure that the bass parameter is valid.
    //
    if(lBass < 0)
    {
        lBass = 0;
    }
    else if(lBass > 99)
    {
        lBass = 99;
    }

    //
    // Set the new value for the bass filter.
    //
    vibe_set_effect(&(pSpatial->sVibe), lBass,
                    vibe_get_gain_virt(&(pSpatial->sVibe)));
}

//****************************************************************************
//
// SpatializerAdjustVirtualBass adjusts the virtual bass control of the
// Spatializer processing.
//
//****************************************************************************
void
SpatializerAdjustVirtualBass(tSpatializerControl *pSpatial, long lBass)
{
    //
    // Make sure that the virtual bass parameter is valid.
    //
    if(lBass < 0)
    {
        lBass = 0;
    }
    else if(lBass > 99)
    {
        lBass = 99;
    }

    //
    // Set the new value for the virtual bass filter.
    //
    vibe_set_effect(&(pSpatial->sVibe), vibe_get_gain_lpf(&(pSpatial->sVibe)),
                    lBass);
}

//****************************************************************************
//
// SpatializerAdjustGain adjusts the gain control of the Spatializer
// processing.
//
//****************************************************************************
void
SpatializerAdjustGain(tSpatializerControl *pSpatial, long lGain)
{
    //
    // Make sure that the gain parameter is valid.
    //
    if(lGain < 0)
    {
        lGain = 0;
    }
    else if(lGain > 0x1000)
    {
        lGain = 0x1000;
    }

    //
    // Set the new value for the gain control.
    //
    vibe_set_gain(&(pSpatial->sVibe), lGain);
}

//****************************************************************************
//
// SpatializerEnable enables the Spatializer processing of the audio stream.
//
//****************************************************************************
void
SpatializerEnable(tSpatializerControl *pSpatial)
{
    //
    // Enable the Spatializer processing.
    //
    pSpatial->bProcessAudio = 1;
}

//****************************************************************************
//
// SpatializerDisable disables the Spatializer processing of the audio stream.
//
//****************************************************************************
void
SpatializerDisable(tSpatializerControl *pSpatial)
{
    //
    // Disable the Spatializer processing.
    //
    pSpatial->bProcessAudio = 0;
}

//****************************************************************************
//
// SpatializerGetInputBuffer returns a pointer to the input buffer for the
// Spatializer control.
//
//****************************************************************************
BufferState *
SpatializerGetInputBuffer(tSpatializerControl *pSpatial)
{
    //
    // Return a pointer to the input buffer.
    //
    return(&(pSpatial->sInputBuffer));
}

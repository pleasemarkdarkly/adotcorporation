//****************************************************************************
//
// SPATIAL.C - Interface to the Spatializer library.
//
// Copyright (c) 2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include "globals.h"
#ifdef SUPPORT_SPATIALIZER
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
            // See if any of the parameters were changed and need to be
            // committed before we continue.
            //
            if(pSpatializer->bParametersChanged)
            {
                //
                // Commit the changes to the parameters.
                //
                dfx_engage(&(pSpatializer->sDigitalFX));

                //
                // We've commited the parameters, so indicate that we no longer
                // need to.
                //
                pSpatializer->bParametersChanged = 0;
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
                    dfx_process_block(&(pSpatializer->sDigitalFX),
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
                    dfx_process_mono_block(&(pSpatializer->sDigitalFX),
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
                                   (long)ulParam1) == 0)
            {
                return(0);
            }

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
                    pSpatializer->sDigitalFX.configurator &= 0xf3ffffff;
                    pSpatializer->sDigitalFX.configurator |=
                        DFX_SAMPLING_RATE_32000;

                    //
                    // Commit the new sample rate.
                    //
                    dfx_engage(&(pSpatializer->sDigitalFX));

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
                    pSpatializer->sDigitalFX.configurator &= 0xf3ffffff;
                    pSpatializer->sDigitalFX.configurator |=
                        DFX_SAMPLING_RATE_44100;

                    //
                    // Commit the new sample rate.
                    //
                    dfx_engage(&(pSpatializer->sDigitalFX));

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
                    pSpatializer->sDigitalFX.configurator &= 0xf3ffffff;
                    pSpatializer->sDigitalFX.configurator |=
                        DFX_SAMPLING_RATE_48000;

                    //
                    // Commit the new sample rate.
                    //
                    dfx_engage(&(pSpatializer->sDigitalFX));

                    //
                    // We're done handling this rate.
                    //
                    break;
                }
            }

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
    pSpatial->sDigitalFX.configurator = 0;
#ifdef SUPPORT_SPATIALIZER_VIBE
    pSpatial->sDigitalFX.configurator |= 0x80000000;
#endif
#ifdef SUPPORT_SPATIALIZER_LOUDNESS
    pSpatial->sDigitalFX.configurator |= DFX_LOUDNESS;
#endif
#ifdef SUPPORT_SPATIALIZER_3D
    pSpatial->sDigitalFX.configurator |= DFX_3D;
#endif
    pSpatial->sDigitalFX.space_volume = 0;
    pSpatial->sDigitalFX.center_volume = 0;
    pSpatial->sDigitalFX.headphone_angle = 0x0000;
    pSpatial->sDigitalFX.bass_volume = 0;
    pSpatial->sDigitalFX.virtual_bass_volume = 0;
    pSpatial->sDigitalFX.virtual_bass_color = 0x0800;
    pSpatial->sDigitalFX.loudness_volume = 0;
    pSpatial->sDigitalFX.volume_left = 0x1000;
    pSpatial->sDigitalFX.volume_right = 0x1000;

    //
    // Commit the values of the control.
    //
    dfx_engage(&(pSpatial->sDigitalFX));

    //
    // Indicate that there are no parameters which need to be committed.
    //
    pSpatial->bParametersChanged = 0;

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
// SpatializerAdjustSpace adjusts the space control of the Spatializer
// processing.
//
//****************************************************************************
#ifdef SUPPORT_SPATIALIZER_3D
void
SpatializerAdjustSpace(tSpatializerControl *pSpatial, long lSpace)
{
    //
    // Make sure that the space parameter is valid.
    //
    if(lSpace < 0)
    {
        lSpace = 0;
    }
    else if(lSpace > 0x2000)
    {
        lSpace = 0x2000;
    }

    //
    // Set the new value for the 3D space filter.
    //
    pSpatial->sDigitalFX.space_volume = lSpace;

    //
    // Commit the new value of the 3D space filter.
    //
    pSpatial->bParametersChanged = 1;
}
#endif

//****************************************************************************
//
// SpatializerAdjustCenter adjusts the center control of the Spatializer
// processing.
//
//****************************************************************************
#ifdef SUPPORT_SPATIALIZER_3D
void
SpatializerAdjustCenter(tSpatializerControl *pSpatial, long lCenter)
{
    //
    // Make sure that the space parameter is valid.
    //
    if(lCenter < 0)
    {
        lCenter = 0;
    }
    else if(lCenter > 0x2000)
    {
        lCenter = 0x2000;
    }

    //
    // Set the new value for the 3D center filter.
    //
    pSpatial->sDigitalFX.center_volume = lCenter;

    //
    // Commit the new value of the 3D center filter.
    //
    pSpatial->bParametersChanged = 1;
}
#endif

//****************************************************************************
//
// SpatializerAdjustHeadphone adjusts the virtual speaker angle of the
// headphone control of the Spatializer processing.
//
//****************************************************************************
#ifdef SUPPORT_SPATIALIZER_HEADPHONE
void
SpatializerAdjustHeadphone(tSpatializerControl *pSpatial, long lHeadphone)
{
    //
    // Make sure that the headphone parameter is valid.
    //
    if(lHeadphone < 0)
    {
        lHeadphone = 0;
    }
    else if(lHeadphone > 3)
    {
        lHeadphone = 3;
    }

    //
    // Set the new value for the virtual speaker position.
    //
    if(lHeadphone == 0)
    {
        pSpatial->sDigitalFX.configurator &= ~DFX_HEADPHONES;
    }
    else
    {
        pSpatial->sDigitalFX.configurator |= DFX_HEADPHONES;
        pSpatial->sDigitalFX.headphone_angle = lHeadphone * 30 * 256;
    }

    //
    // Commit the new value of the virtual speaker position.
    //
    pSpatial->bParametersChanged = 1;
}
#endif

//****************************************************************************
//
// SpatializerAdjustBass adjusts the bass control of the Spatializer
// processing.
//
//****************************************************************************
#ifdef SUPPORT_SPATIALIZER_VIBE
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
    else if(lBass > 0x3fff)
    {
        lBass = 0x3fff;
    }

    //
    // Set the new value for the bass filter.
    //
    pSpatial->sDigitalFX.bass_volume = lBass;

    //
    // Commit the new value of the bass filter.
    //
    pSpatial->bParametersChanged = 1;
}
#endif

//****************************************************************************
//
// SpatializerAdjustVirtualBass adjusts the virtual bass control of the
// Spatializer processing.
//
//****************************************************************************
#ifdef SUPPORT_SPATIALIZER_VIBE
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
    else if(lBass > 0x2000)
    {
        lBass = 0x2000;
    }

    //
    // Set the new value for the virtual bass filter.
    //
    pSpatial->sDigitalFX.virtual_bass_volume = lBass;

    //
    // Commit the new value of the virtual bass filter.
    //
    pSpatial->bParametersChanged = 1;
}
#endif

//****************************************************************************
//
// SpatializerAdjustBassColor adjusts the color of the bass control of the
// Spatializer processing.
//
//****************************************************************************
#ifdef SUPPORT_SPATIALIZER_VIBE
void
SpatializerAdjustBassColor(tSpatializerControl *pSpatial, long lBassColor)
{
    //
    // Make sure that the bass color parameter is valid.
    //
    if(lBassColor < 0)
    {
        lBassColor = 0;
    }
    else if(lBassColor > 4)
    {
        lBassColor = 4;
    }

    //
    // Set the new value for the bass color parameter.
    //
    pSpatial->sDigitalFX.virtual_bass_color = 0x400 * lBassColor;

    //
    // Commit the new value of the bass color parameter.
    //
    pSpatial->bParametersChanged = 1;
}
#endif

//****************************************************************************
//
// SpatializerAdjustLoudness adjusts the loudness control of the Spatializer
// processing.
//
//****************************************************************************
#ifdef SUPPORT_SPATIALIZER_LOUDNESS
void
SpatializerAdjustLoudness(tSpatializerControl *pSpatial, long lLoudness)
{
    //
    // Make sure that the loudness parameter is valid.
    //
    if(lLoudness < 0)
    {
        lLoudness = 0;
    }
    else if(lLoudness > 3)
    {
        lLoudness = 3;
    }

    //
    // Set the new value for the loudness filter.
    //
    pSpatial->sDigitalFX.loudness_volume = lLoudness << 12;

    //
    // Commit the new value of the loudness filter.
    //
    pSpatial->bParametersChanged = 1;
}
#endif

//****************************************************************************
//
// SpatializerAdjustVolume adjusts the volume control of the Spatializer
// processing.
//
//****************************************************************************
void
SpatializerAdjustVolume(tSpatializerControl *pSpatial, long lVolume)
{
    //
    // Make sure that the volume parameter is valid.
    //
    if(lVolume < 0)
    {
        lVolume = 0;
    }
    else if(lVolume > 0x1000)
    {
        lVolume = 0x1000;
    }

    //
    // Set the new value for the volume control.
    //
    pSpatial->sDigitalFX.volume_left = lVolume;
    pSpatial->sDigitalFX.volume_right = lVolume;

    //
    // Commit the new value of the volume control.
    //
    pSpatial->bParametersChanged = 1;
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
#endif

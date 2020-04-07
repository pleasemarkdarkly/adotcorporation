//****************************************************************************
//
// VOLUME.C - Software volume control algorithm.
//
// Copyright (c) 1999-2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include "volume.h"

//****************************************************************************
//
// The following array contains the multipliers used to achieve the desired
// volume level.  The array is ordered from silence to maximum scale.
//
//****************************************************************************
static const long lGains[21] =
{
        0,   164,   655,  1475,  2621,  4096,  5898,
     8028, 10486, 13271, 16384, 19825, 23593, 27689,
    32113, 36864, 41943, 47350, 53084, 59146, 65536
};

//****************************************************************************
//
// VolumeBufferIoctl is the buffer IOCTL handler for the volume control input
// buffer.
//
//****************************************************************************
static long
VolumeBufferIoctl(unsigned long ulIoctl, BufferState *psBuffer,
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
            tVolumeControl *pVol;

            //
            // Get a pointer to the volume control state structure.
            //
            pVol = (tVolumeControl *)psBuffer->lInstanceData;

            //
            // Set the buffer of the output buffer.
            //
            return(BufferSetBuffer(pVol->pOutputBuffer, (short *)ulParam1,
                                   (short *)ulParam2, (long)ulParam3));
        }

        //
        // Determine the amount of space available in the output buffer.
        //
        case IOCTL_BUFFER_SPACEAVAILABLE:
        {
            tVolumeControl *pVol;

            //
            // Get a pointer to the volume control state structure.
            //
            pVol = (tVolumeControl *)psBuffer->lInstanceData;

            //
            // Return the amount of space available in the output buffer.
            //
            return(BufferSpaceAvailable(pVol->pOutputBuffer));
        }

        //
        // Determine if the output buffer is empty.
        //
        case IOCTL_BUFFER_ISEMPTY:
        {
            tVolumeControl *pVol;

            //
            // Get a pointer to the volume control state structure.
            //
            pVol = (tVolumeControl *)psBuffer->lInstanceData;

            //
            // Return whether or not the output buffer is empty.
            //
            return(BufferIsEmpty(pVol->pOutputBuffer));
        }

        //
        // Get the write pointer.
        //
        case IOCTL_BUFFER_GETWRITEPOINTER:
        {
            tVolumeControl *pVol;

            //
            // Get a pointer to the volume control state structure.
            //
            pVol = (tVolumeControl *)psBuffer->lInstanceData;

            //
            // Return the write pointer of the output buffer.
            //
            return(BufferGetWritePointer(pVol->pOutputBuffer,
                                         (short **)ulParam1,
                                         (short **)ulParam2,
                                         (long *)ulParam3));
        }

        //
        // Update the write pointer.
        //
        case IOCTL_BUFFER_UPDATEWRITEPOINTER:
        {
            tVolumeControl *pVol;
            short *psLeft, *psRight;
            long lIdx, lLength;

            //
            // Get a pointer to the volume control state structure.
            //
            pVol = (tVolumeControl *)psBuffer->lInstanceData;

            //
            // Get the current write pointer for the output buffer.
            //
            BufferGetWritePointer(pVol->pOutputBuffer, &psLeft, &psRight,
                                  &lLength);

            //
            // See if there is enough space in this buffer.
            //
            if(lLength < (long)ulParam1)
            {
                return(0);
            }

            //
            // If there is a right channel buffer, then perform stereo volume
            // control filtering.
            //
            if(psLeft != psRight)
            {
                //
                // Loop over all the input samples.
                //
                for(lIdx = 0; lIdx < (long)ulParam1; lIdx++)
                {
                    //
                    // Adjust the volume of this sample.
                    //
                    psLeft[lIdx] = (psLeft[lIdx] * pVol->lVolume) / 65536;
                    psRight[lIdx] = (psRight[lIdx] * pVol->lVolume) / 65536;
                }
            }
            else
            {
                //
                // Loop over all the input samples.
                //
                for(lIdx = 0; lIdx < (long)ulParam1; lIdx++)
                {
                    //
                    // Adjust the volume of this sample.
                    //
                    psLeft[lIdx] = (psLeft[lIdx] * pVol->lVolume) / 65536;
                }
            }

            //
            // Update the write pointer of the output buffer.
            //
            return(BufferUpdateWritePointer(pVol->pOutputBuffer,
                                            (long)ulParam1));
        }

        //
        // Set the sample rate of the data buffer.
        //
        case IOCTL_BUFFER_SETSAMPLERATE:
        {
            tVolumeControl *pVol;

            //
            // Get a pointer to the volume control state structure.
            //
            pVol = (tVolumeControl *)psBuffer->lInstanceData;

            //
            // Pass the set sample rate request to the output buffer.
            //
            return(BufferSetSampleRate(pVol->pOutputBuffer, (long)ulParam1));
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
// VolumeInit sets the initial state of the Volume control filter.
//
//****************************************************************************
void
VolumeInit(tVolumeControl *pVol, BufferState *pOutputBuffer)
{
    //
    // Set a default volume of maximum.
    //
    pVol->lVolume = lGains[20];

    //
    // Initialize the input buffer.
    //
    BufferInit(&(pVol->sInputBuffer), VolumeBufferIoctl, (long)pVol);

    //
    // Save the pointer to the output buffer.
    //
    pVol->pOutputBuffer = pOutputBuffer;
}

//****************************************************************************
//
// VolumeAdjust adjusts the volume.  The gain is specified as a linear scalar,
// with zero being silence and twenty being no attenuation.
//
//****************************************************************************
void
VolumeAdjust(tVolumeControl *pVol, long lGain)
{
    //
    // Make sure that the volume is in range.
    //
    if(lGain < 0)
    {
        lGain = 0;
    }
    else if(lGain > 20)
    {
        lGain = 20;
    }

    //
    // Set the new volume scalar.
    //
    pVol->lVolume = lGains[lGain];
}

//****************************************************************************
//
// VolumeGetInputBuffer returns a pointer to the input buffer for the volume
// control.
//
//****************************************************************************
BufferState *
VolumeGetInputBuffer(tVolumeControl *pVol)
{
    //
    // Return a pointer to the input buffer.
    //
    return(&(pVol->sInputBuffer));
}

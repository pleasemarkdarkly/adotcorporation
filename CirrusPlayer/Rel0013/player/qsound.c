//****************************************************************************
//
// QSOUND.C - Interface to the QSound Q2Xpander library.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include "../config.h"
#ifdef SUPPORT_QSOUND
#include "qsound.h"

//****************************************************************************
//
// QSoundBufferIoctl is the buffer IOCTL handler for the QSound control input
// buffer.
//
//****************************************************************************
static long
QSoundBufferIoctl(unsigned long ulIoctl, BufferState *psBuffer,
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
            tQSoundControl *pQSound;

            //
            // Get a pointer to the QSound control state structure.
            //
            pQSound = (tQSoundControl *)psBuffer->lInstanceData;

            //
            // Set the buffer of the output buffer.
            //
            return(BufferSetBuffer(pQSound->pOutputBuffer, (short *)ulParam1,
                                   (short *)ulParam2, (long)ulParam3));
        }

        //
        // Determine the amount of space available in the output buffer.
        //
        case IOCTL_BUFFER_SPACEAVAILABLE:
        {
            tQSoundControl *pQSound;

            //
            // Get a pointer to the QSound control state structure.
            //
            pQSound = (tQSoundControl *)psBuffer->lInstanceData;

            //
            // Return the amount of space available in the output buffer.
            //
            return(BufferSpaceAvailable(pQSound->pOutputBuffer));
        }

        //
        // Determine if the output buffer is empty.
        //
        case IOCTL_BUFFER_ISEMPTY:
        {
            tQSoundControl *pQSound;

            //
            // Get a pointer to the QSound control state structure.
            //
            pQSound = (tQSoundControl *)psBuffer->lInstanceData;

            //
            // Return whether or not the output buffer is empty.
            //
            return(BufferIsEmpty(pQSound->pOutputBuffer));
        }

        //
        // Get the write pointer.
        //
        case IOCTL_BUFFER_GETWRITEPOINTER:
        {
            tQSoundControl *pQSound;

            //
            // Get a pointer to the QSound control state structure.
            //
            pQSound = (tQSoundControl *)psBuffer->lInstanceData;

            //
            // Return the write pointer of the output buffer.
            //
            return(BufferGetWritePointer(pQSound->pOutputBuffer,
                                         (short **)ulParam1,
                                         (short **)ulParam2,
                                         (long *)ulParam3));
        }

        //
        // Update the write pointer.
        //
        case IOCTL_BUFFER_UPDATEWRITEPOINTER:
        {
            tQSoundControl *pQSound;
            short *psLeft, *psRight;
            long lLoop, lIdx, lLength, lLeft, lRight;

            //
            // Get a pointer to the QSound control state structure.
            //
            pQSound = (tQSoundControl *)psBuffer->lInstanceData;

            //
            // Get the current write pointer for the output buffer.
            //
            BufferGetWritePointer(pQSound->pOutputBuffer, &psLeft, &psRight,
                                  &lLength);

            //
            // See if there is enough space in this buffer.
            //
            if(lLength < (long)ulParam1)
            {
                return(0);
            }

            //
            // If there is a right channel buffer, then perform stereo QSound
            // processing.
            //
            if(psLeft != psRight)
            {
                //
                // Loop through the samples in four separate blocks.  This is
                // to prevent starving the downstream task (performing four
                // separate buffer updates instead of one large one).
                //
                for(lLoop = 0; lLoop < (long)ulParam1; lLoop += (ulParam1 / 4))
                {
                    //
                    // Loop over all the input samples in this block.
                    //
                    for(lIdx = 0; lIdx < (long)(ulParam1 / 4); lIdx++)
                    {
                        //
                        // Get this sample.
                        //
                        lLeft = psLeft[lLoop + lIdx];
                        lRight = psRight[lLoop + lIdx];

                        //
                        // Process this sample with QSound.
                        //
                        q2xProcess(&(pQSound->sQ2X), &lLeft, &lRight);

                        //
                        // Place the processed sample back into the buffer.
                        //
                        psLeft[lLoop + lIdx] = lLeft;
                        psRight[lLoop + lIdx] = lRight;
                    }

                    //
                    // Update the buffer write pointer.
                    //
                    BufferUpdateWritePointer(pQSound->pOutputBuffer,
                                             (long)(ulParam1 / 4));
                }
            }
            else
            {
                //
                // For mono streams, we need to scale the volume of the samples
                // so that the level matches that of stereo streams.
                //
                for(lIdx = 0; lIdx < ulParam1; lIdx++)
                {
                    psLeft[lIdx] = (psLeft[lIdx] * 0x3fff) / 65536;
                }

                //
                // Update the buffer write pointer.
                //
                BufferUpdateWritePointer(pQSound->pOutputBuffer,
                                         (long)ulParam1);
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
            tQSoundControl *pQSound;

            //
            // Get a pointer to the QSound control state structure.
            //
            pQSound = (tQSoundControl *)psBuffer->lInstanceData;

            //
            // Pass the set sample rate request to the output buffer.
            //
            return(BufferSetSampleRate(pQSound->pOutputBuffer,
                                       (long)ulParam1));
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
// QSoundInit sets the initial state of the QSound control filter.
//
//****************************************************************************
void
QSoundInit(tQSoundControl *pQSound, BufferState *pOutputBuffer)
{
    //
    // Initialize the Q2Xpander library.
    //
    q2xInitialise(&(pQSound->sQ2X));

    //
    // Initialize the Q2Xpander parameters.
    //
    q2xSetVolume(&(pQSound->sQ2X), 0x3fff, 0x3fff);
    q2xSetSpread(&(pQSound->sQ2X), 0);
    q2xSetMode(&(pQSound->sQ2X), 0);

    //
    // Reset the Q2Xpander internal state.
    //
    q2xReset(&(pQSound->sQ2X));

    //
    // Initialize the input buffer.
    //
    BufferInit(&(pQSound->sInputBuffer), QSoundBufferIoctl, (long)pQSound);

    //
    // Save the pointer to the output buffer.
    //
    pQSound->pOutputBuffer = pOutputBuffer;
}

//****************************************************************************
//
// QSoundAdjustWidth adjusts the stereo separation of the QSound processing.
//
//****************************************************************************
void
QSoundAdjustWidth(tQSoundControl *pQSound, long lWidth)
{
    //
    // Make sure that the width parameter is valid.
    //
    if(lWidth < 0)
    {
        lWidth = 0;
    }
    else if(lWidth > 0x7fff)
    {
        lWidth = 0x7fff;
    }

    //
    // Set the new value for the stereo separation.
    //
    q2xSetSpread(&(pQSound->sQ2X), lWidth);
}

//****************************************************************************
//
// QSoundSetMode changes the Q2Xpander mode to be used.
//
//****************************************************************************
void
QSoundSetMode(tQSoundControl *pQSound, long lMode)
{
    //
    // Make sure that the mode is valid.
    //
    if(lMode < 0)
    {
        lMode = 0;
    }
    else if(lMode > 1)
    {
        lMode = 1;
    }

    //
    // Set the new mode for the Q2Xpander.
    //
    q2xSetMode(&(pQSound->sQ2X), lMode);
}

//****************************************************************************
//
// QSoundGetInputBuffer returns a pointer to the input buffer for the QSound
// control.
//
//****************************************************************************
BufferState *
QSoundGetInputBuffer(tQSoundControl *pQSound)
{
    //
    // Return a pointer to the input buffer.
    //
    return(&(pQSound->sInputBuffer));
}
#endif
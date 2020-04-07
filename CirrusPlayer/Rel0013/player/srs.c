//****************************************************************************
//
// SRS.C - Interface to the SRS Labs embedded device library.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include "../config.h"
#ifdef SUPPORT_SRS
#include "srs.h"

//****************************************************************************
//
// SRSBufferIoctl is the buffer IOCTL handler for the SRS control input
// buffer.
//
//****************************************************************************
static long
SRSBufferIoctl(unsigned long ulIoctl, BufferState *psBuffer,
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
            tSRSControl *pSRS;

            //
            // Get a pointer to the SRS control state structure.
            //
            pSRS = (tSRSControl *)psBuffer->lInstanceData;

            //
            // Set the buffer of the output buffer.
            //
            return(BufferSetBuffer(pSRS->pOutputBuffer, (short *)ulParam1,
                                   (short *)ulParam2, (long)ulParam3));
        }

        //
        // Determine the amount of space available in the output buffer.
        //
        case IOCTL_BUFFER_SPACEAVAILABLE:
        {
            tSRSControl *pSRS;

            //
            // Get a pointer to the SRS control state structure.
            //
            pSRS = (tSRSControl *)psBuffer->lInstanceData;

            //
            // Return the amount of space available in the output buffer.
            //
            return(BufferSpaceAvailable(pSRS->pOutputBuffer));
        }

        //
        // Determine if the output buffer is empty.
        //
        case IOCTL_BUFFER_ISEMPTY:
        {
            tSRSControl *pSRS;

            //
            // Get a pointer to the SRS control state structure.
            //
            pSRS = (tSRSControl *)psBuffer->lInstanceData;

            //
            // Return whether or not the output buffer is empty.
            //
            return(BufferIsEmpty(pSRS->pOutputBuffer));
        }

        //
        // Get the write pointer.
        //
        case IOCTL_BUFFER_GETWRITEPOINTER:
        {
            tSRSControl *pSRS;

            //
            // Get a pointer to the SRS control state structure.
            //
            pSRS = (tSRSControl *)psBuffer->lInstanceData;

            //
            // Return the write pointer of the output buffer.
            //
            return(BufferGetWritePointer(pSRS->pOutputBuffer,
                                         (short **)ulParam1,
                                         (short **)ulParam2,
                                         (long *)ulParam3));
        }

        //
        // Update the write pointer.
        //
        case IOCTL_BUFFER_UPDATEWRITEPOINTER:
        {
            tSRSControl *pSRS;
            AudioChannel sAC;
            short *psLeft, *psRight;
            long lLoop, lIdx, lLength;

            //
            // Get a pointer to the SRS control state structure.
            //
            pSRS = (tSRSControl *)psBuffer->lInstanceData;

            //
            // Get the current write pointer for the output buffer.
            //
            BufferGetWritePointer(pSRS->pOutputBuffer, &psLeft, &psRight,
                                  &lLength);

            //
            // See if there is enough space in this buffer.
            //
            if(lLength < (long)ulParam1)
            {
                return(0);
            }

            //
            // If there is a right channel buffer, then perform stereo SRS
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
                        sAC.mLeft = psLeft[lLoop + lIdx];
                        sAC.mRight = psRight[lLoop + lIdx];

                        //
                        // Process this sample with SRS.
                        //
#ifdef SUPPORT_WOW_FULL
                        Wow_EDA(&(pSRS->sSRS), &sAC);
#endif
#ifdef SUPPORT_WOW_MINIMAL
                        WowM_EDA(&(pSRS->sSRS), &sAC);
#endif
#ifdef SUPPORT_TRU_BASS
                        TruBass_EDA(&(pSRS->sSRS), &sAC);
#endif
#ifdef SUPPORT_SRS_HEADPHONE
                        Headphone_EDA(&(pSRS->sSRS), &sAC);
#endif

                        //
                        // Place the processed sample back into the buffer.
                        //
                        psLeft[lLoop + lIdx] = sAC.mLeft;
                        psRight[lLoop + lIdx] = sAC.mRight;
                    }

                    //
                    // Update the buffer write pointer.
                    //
                    BufferUpdateWritePointer(pSRS->pOutputBuffer,
                                             (long)(ulParam1 / 4));
                }
            }
            else
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
                        sAC.mLeft = psLeft[lLoop + lIdx];
                        sAC.mRight = psLeft[lLoop + lIdx];

                        //
                        // Process this sample with SRS.
                        //
#ifdef SUPPORT_WOW_FULL
                        Wow_EDA(&(pSRS->sSRS), &sAC);
#endif
#ifdef SUPPORT_WOW_MINIMAL
                        WowM_EDA(&(pSRS->sSRS), &sAC);
#endif
#ifdef SUPPORT_TRU_BASS
                        TruBass_EDA(&(pSRS->sSRS), &sAC);
#endif
#ifdef SUPPORT_SRS_HEADPHONE
                        Headphone_EDA(&(pSRS->sSRS), &sAC);
#endif

                        //
                        // Place the processed sample back into the buffer.
                        //
                        psLeft[lLoop + lIdx] = sAC.mLeft;
                    }

                    //
                    // Update the buffer write pointer.
                    //
                    BufferUpdateWritePointer(pSRS->pOutputBuffer,
                                             (long)(ulParam1 / 4));
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
            tSRSControl *pSRS;

            //
            // Get a pointer to the SRS control state structure.
            //
            pSRS = (tSRSControl *)psBuffer->lInstanceData;

            //
            // Pass the set sample rate request to the output buffer.
            //
            return(BufferSetSampleRate(pSRS->pOutputBuffer, (long)ulParam1));
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
// SRSInit sets the initial state of the SRS control filter.
//
//****************************************************************************
void
SRSInit(tSRSControl *pSRS, BufferState *pOutputBuffer)
{
#ifdef SUPPORT_WOW_FULL
    //
    // Initialize the WOW control.
    //
    InitWowControls(&(pSRS->sSRS));

    //
    // Initialize the width and TruBass controls to zero.
    //
    SetWidthControl(&(pSRS->sSRS), 0);
    SetBassControl(&(pSRS->sSRS), 0);
#endif
#ifdef SUPPORT_WOW_MINIMAL
    //
    // Initialize the WOW minimal control.
    //
    InitWowControls(&(pSRS->sSRS));

    //
    // Initialize the width and TruBass controls to zero.
    //
    SetWidthControl(&(pSRS->sSRS), 0);
    SetBassControl(&(pSRS->sSRS), 0);
#endif
#ifdef SUPPORT_TRU_BASS
    //
    // Initialize the TruBass control.
    //
    InitTruBassControls(&(pSRS->sSRS));

    //
    // Initialize the TruBass control to zero.
    //
    SetBassControl(&(pSRS->sSRS), 0);
#endif
#ifdef SUPPORT_SRS_HEADPHONE
    //
    // Initialize the SRS Headphone control.
    //
    InitHeadphoneControls(&(pSRS->sSRS));

    //
    // Initialize the width control to zero.
    //
    SetWidthControl(&(pSRS->sSRS), 0);
#endif

    //
    // Initialize the input buffer.
    //
    BufferInit(&(pSRS->sInputBuffer), SRSBufferIoctl, (long)pSRS);

    //
    // Save the pointer to the output buffer.
    //
    pSRS->pOutputBuffer = pOutputBuffer;
}

//****************************************************************************
//
// SRSAdjustWidth adjusts the stereo separation of the SRS processing.
//
//****************************************************************************
void
SRSAdjustWidth(tSRSControl *pSRS, long lWidth)
{
    //
    // Set the new value for the stereo separation.
    //
    SetWidthControl(&(pSRS->sSRS), lWidth);
}

//****************************************************************************
//
// SRSAdjustTruBass adjusts the TruBass enhancement of the SRS processing.
//
//****************************************************************************
void
SRSAdjustTruBass(tSRSControl *pSRS, long lTruBass)
{
    //
    // Set the new value for the TruBass enhancement.
    //
    SetBassControl(&(pSRS->sSRS), lTruBass);
}

//****************************************************************************
//
// SRSGetInputBuffer returns a pointer to the input buffer for the SRS
// control.
//
//****************************************************************************
BufferState *
SRSGetInputBuffer(tSRSControl *pSRS)
{
    //
    // Return a pointer to the input buffer.
    //
    return(&(pSRS->sInputBuffer));
}
#endif

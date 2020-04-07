//****************************************************************************
//
// BUFFER.C - Routines for handling a circular data buffer.
//
// Copyright (c) 1999-2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include "buffer.h"

//****************************************************************************
//
// BufferInit initializes a circular buffer.
//
//****************************************************************************
void
BufferInit(BufferState *psBuffer, long (*pfnIoctl)(unsigned long ulIoctl,
                                                   BufferState *psBuffer,
                                                   unsigned long ulParam1,
                                                   unsigned long ulParam2,
                                                   unsigned long ulParam3),
           long lInstanceData)
{
    //
    // Save the IOCTL handler and instance data.
    //
    psBuffer->pfnIoctl = pfnIoctl;
    psBuffer->lInstanceData = lInstanceData;

    //
    // Initialize the circular buffer.
    //
    psBuffer->psLeft = 0;
    psBuffer->psRight = 0;
    psBuffer->lLength = 0;

    //
    // Initialize the circular buffer read and write pointers.
    //
    psBuffer->lReadPtr = 0;
    psBuffer->lWritePtr = 0;
}

//****************************************************************************
//
// BufferSetBuffer sets the address and length of the data buffer for this
// circular buffer.
//
//****************************************************************************
long
BufferSetBuffer(BufferState *psBuffer, short *psLeft, short *psRight,
                long lLength)
{
    long lRet;

    //
    // Make sure that the size of the buffer is a multiple of four samples.
    //
    if(lLength & 3)
    {
        return(0);
    }

    //
    // Send the set buffer request to the entry point for this buffer.
    //
    if(psBuffer->pfnIoctl)
    {
        lRet = (psBuffer->pfnIoctl)(IOCTL_BUFFER_SETBUFFER, psBuffer,
                                    (unsigned long)psLeft,
                                    (unsigned long)psRight,
                                    (unsigned long)lLength);
    }
    else
    {
        lRet = -1;
    }

    //
    // If the return value is -1, then we should handle the request.
    //
    if(lRet == -1)
    {
        //
        // Remember the address and size of the buffer.
        //
        psBuffer->psLeft = psLeft;
        psBuffer->psRight = psRight;
        psBuffer->lLength = lLength;

        //
        // Reset the read and write pointers.
        //
        psBuffer->lReadPtr = 0;
        psBuffer->lWritePtr = 0;

        //
        // Success.
        //
        lRet = 1;
    }

    //
    // Return to the caller.
    //
    return(lRet);
}

//****************************************************************************
//
// BufferDataAvailable determines the number of samples that are currently in
// the circular buffer.
//
//****************************************************************************
long
BufferDataAvailable(BufferState *psBuffer)
{
    long lRet, lRead, lWrite;

    //
    // Send the data available request to the entry point for this buffer.
    //
    if(psBuffer->pfnIoctl)
    {
        lRet = (psBuffer->pfnIoctl)(IOCTL_BUFFER_DATAAVAILABLE, psBuffer, 0, 0,
                                    0);
    }
    else
    {
        lRet = -1;
    }

    //
    // If the return value is -1, then we should handle the request.
    //
    if(lRet == -1)
    {
        //
        // Get the current read and write pointers.
        //
        lRead = psBuffer->lReadPtr;
        lWrite = psBuffer->lWritePtr;

        //
        // Determine the samples available.
        //
        if(lWrite >= lRead)
        {
            //
            // The read pointer is before the write pointer in the bufer.
            //
            lRet = lWrite - lRead;
        }
        else
        {
            //
            // The write pointer is before the read pointer in the buffer.
            //
            lRet = psBuffer->lLength - (lRead - lWrite);
        }
    }

    //
    // Return to the caller.
    //
    return(lRet);
}

//****************************************************************************
//
// BufferSpaceAvailable determines the number of samples that can be added to
// the circular buffer.
//
//****************************************************************************
long
BufferSpaceAvailable(BufferState *psBuffer)
{
    long lRet, lRead, lWrite;

    //
    // Send the space available request to the entry point for this buffer.
    //
    if(psBuffer->pfnIoctl)
    {
        lRet = (psBuffer->pfnIoctl)(IOCTL_BUFFER_SPACEAVAILABLE, psBuffer, 0,
                                    0, 0);
    }
    else
    {
        lRet = -1;
    }

    //
    // If the return value is -1, then we should handle the request.
    //
    if(lRet == -1)
    {
        //
        // Get the current read and write pointers.
        //
        lRead = psBuffer->lReadPtr;
        lWrite = psBuffer->lWritePtr;

        //
        // Determine the space available.
        //
        if(lWrite == lRead)
        {
            //
            // The entire buffer is available (minus four samples).
            //
            lRet = psBuffer->lLength - 4;
        }
        else if(lWrite > lRead)
        {
            //
            // The read pointer is before the write pointer in the buffer.
            //
            lRet = psBuffer->lLength - (lWrite - lRead) - 4;
        }
        else
        {
            //
            // The write pointer is before the read pointer in the buffer.
            //
            lRet = lRead - lWrite - 4;
        }
    }

    //
    // Return to the caller.
    //
    return(lRet);
}

//****************************************************************************
//
// BufferIsEmpty determines if the circular buffer is empty.
//
//****************************************************************************
long
BufferIsEmpty(BufferState *psBuffer)
{
    long lRet;

    //
    // Send the is empty request to the entry point for this buffer.
    //
    if(psBuffer->pfnIoctl)
    {
        lRet = (psBuffer->pfnIoctl)(IOCTL_BUFFER_ISEMPTY, psBuffer, 0, 0, 0);
    }
    else
    {
        lRet = -1;
    }

    //
    // If the return value is -1, then we should handle the request.
    //
    if(lRet == -1)
    {
        //
        // The circular buffer is empty if its read pointer and write pointer
        // are identical.
        //
        if(psBuffer->lReadPtr == psBuffer->lWritePtr)
        {
            //
            // The buffer is empty.
            //
            lRet = 1;
        }
        else
        {
            //
            // The buffer is not empty.
            //
            lRet = 0;
        }
    }

    //
    // Return to the caller.
    //
    return(lRet);
}

//****************************************************************************
//
// BufferGetWritePointer provides pointers into the circular buffer where data
// can be written.
//
//****************************************************************************
long
BufferGetWritePointer(BufferState *psBuffer, short **ppsLeft, short **ppsRight,
                      long *plLength)
{
    long lRet, lRead;

    //
    // Send the get write pointer request to the entry point for this buffer.
    //
    if(psBuffer->pfnIoctl)
    {
        lRet = (psBuffer->pfnIoctl)(IOCTL_BUFFER_GETWRITEPOINTER, psBuffer,
                                    (unsigned long)ppsLeft,
                                    (unsigned long)ppsRight,
                                    (unsigned long)plLength);
    }
    else
    {
        lRet = -1;
    }

    //
    // If the return value is -1, then we should handle the request.
    //
    if(lRet == -1)
    {
        //
        // Get the current read pointer.
        //
        lRead = psBuffer->lReadPtr;

        //
        // Get pointers to the next write position within the circular buffer.
        //
        *ppsLeft = psBuffer->psLeft + psBuffer->lWritePtr;
        *ppsRight = psBuffer->psRight + psBuffer->lWritePtr;

        //
        // Determine the number of sample that can be stored at this offset.
        //
        if(lRead > psBuffer->lWritePtr)
        {
            //
            // We can write up to the read pointer.
            //
            *plLength = lRead - psBuffer->lWritePtr - 4;
        }
        else if(lRead == 0)
        {
            //
            // We can write almost to the end of the buffer.
            //
            *plLength = psBuffer->lLength - psBuffer->lWritePtr - 4;
        }
        else
        {
            //
            // We can write up to the end of the buffer.
            //
            *plLength = psBuffer->lLength - psBuffer->lWritePtr;
        }

        //
        // Success.
        //
        lRet = 1;
    }

    //
    // Return to the caller.
    //
    return(lRet);
}

//****************************************************************************
//
// BufferUpdateWritePointer advances the write pointer for the buffer by the
// specified number of samples.
//
//****************************************************************************
long
BufferUpdateWritePointer(BufferState *psBuffer, long lLength)
{
    long lRet, lRead, lMaxLength;

    //
    // Send the update write pointer request to the entry point for this
    // buffer.
    //
    if(psBuffer->pfnIoctl)
    {
        lRet = (psBuffer->pfnIoctl)(IOCTL_BUFFER_UPDATEWRITEPOINTER, psBuffer,
                                    (unsigned long)lLength, 0, 0);
    }
    else
    {
        lRet = -1;
    }

    //
    // If the return value is -1, then we should handle the request.
    //
    if(lRet == -1)
    {
        //
        // Get the current read pointer.
        //
        lRead = psBuffer->lReadPtr;

        //
        // Determine the maximum number of samples we can write.
        //
        if(lRead > psBuffer->lWritePtr)
        {
            //
            // The maximum we can write is up to the read pointer.
            //
            lMaxLength = lRead - psBuffer->lWritePtr - 4;
        }
        else if(lRead == 0)
        {
            //
            // The maximum we can write is almost to the end of the buffer.
            //
            lMaxLength = psBuffer->lLength - psBuffer->lWritePtr - 4;
        }
        else
        {
            //
            // The maximum we can write is to the end of the buffer.
            //
            lMaxLength = psBuffer->lLength - psBuffer->lWritePtr;
        }

        //
        // See if the specified length is valid.
        //
        if(lLength > lMaxLength)
        {
            //
            // An invalid length was specified.
            //
            lRet = 0;
        }
        else
        {
            //
            // Update the write poitner.
            //
            psBuffer->lWritePtr += lLength;
            if(psBuffer->lWritePtr == psBuffer->lLength)
            {
                psBuffer->lWritePtr = 0;
            }

            //
            // Success.
            //
            lRet = 1;
        }
    }

    //
    // Return to the caller.
    //
    return(lRet);
}

//****************************************************************************
//
// BufferGetReadPointer provides pointers into the circular buffer where data
// can be read.
//
//****************************************************************************
long
BufferGetReadPointer(BufferState *psBuffer, short **ppsLeft, short **ppsRight,
                     long *plLength)
{
    long lRet, lWrite;

    //
    // Send the get read pointer request to the entry point for this buffer.
    //
    if(psBuffer->pfnIoctl)
    {
        lRet = (psBuffer->pfnIoctl)(IOCTL_BUFFER_GETREADPOINTER, psBuffer,
                                    (unsigned long)ppsLeft,
                                    (unsigned long)ppsRight,
                                    (unsigned long)plLength);
    }
    else
    {
        lRet = -1;
    }

    //
    // If the return value is -1, then we should handle the request.
    //
    if(lRet == -1)
    {
        //
        // Get the current write pointer.
        //
        lWrite = psBuffer->lWritePtr;

        //
        // Get pointers to the next read position within the circular buffer.
        //
        *ppsLeft = psBuffer->psLeft + psBuffer->lReadPtr;
        *ppsRight = psBuffer->psRight + psBuffer->lReadPtr;

        //
        // Determine the number of sample that can be read at this offset.
        //
        if(lWrite >= psBuffer->lReadPtr)
        {
            //
            // We can read up to the write pointer.
            //
            *plLength = lWrite - psBuffer->lReadPtr;
        }
        else
        {
            //
            // We can read up to the end of the buffer.
            //
            *plLength = psBuffer->lLength - psBuffer->lReadPtr;
        }

        //
        // Success.
        //
        lRet = 1;
    }

    //
    // Return to the caller.
    //
    return(lRet);
}

//****************************************************************************
//
// BufferUpdateReadPointer advances the read pointer for the buffer by the
// specified number of samples.
//
//****************************************************************************
long
BufferUpdateReadPointer(BufferState *psBuffer, long lLength)
{
    long lRet, lWrite, lMaxLength;

    //
    // Send the update read pointer request to the entry point for this buffer.
    //
    if(psBuffer->pfnIoctl)
    {
        lRet = (psBuffer->pfnIoctl)(IOCTL_BUFFER_UPDATEREADPOINTER, psBuffer,
                                    (unsigned long)lLength, 0, 0);
    }
    else
    {
        lRet = -1;
    }

    //
    // If the return value is -1, then we should handle the request.
    //
    if(lRet == -1)
    {
        //
        // Get the current write pointer.
        //
        lWrite = psBuffer->lWritePtr;

        //
        // Determine the maximum number of samples we can read.
        //
        if(lWrite >= psBuffer->lReadPtr)
        {
            //
            // The maximum we can read is up to the write pointer.
            //
            lMaxLength = lWrite - psBuffer->lReadPtr;
        }
        else
        {
            //
            // The maximum we can read is up to the end of the buffer.
            //
            lMaxLength = psBuffer->lLength - psBuffer->lReadPtr;
        }

        //
        // See if the specified length is valid.
        //
        if(lLength > lMaxLength)
        {
            //
            // An invalid length was specified.
            //
            lRet = 0;
        }
        else
        {
            //
            // Update the read pointer.
            //
            psBuffer->lReadPtr += lLength;
            if(psBuffer->lReadPtr == psBuffer->lLength)
            {
                psBuffer->lReadPtr = 0;
            }

            //
            // Success.
            //
            lRet = 1;
        }
    }

    //
    // Return to the caller.
    //
    return(lRet);
}

//****************************************************************************
//
// BufferSetSampleRate specifies the sample rate of the data in the buffer.
//
//****************************************************************************
long
BufferSetSampleRate(BufferState *psBuffer, long lSampleRate)
{
    long lRet;

    //
    // Send the set sample rate request to the entry point for this buffer.
    //
    if(psBuffer->pfnIoctl)
    {
        //
        // Call the entry point for this buffer.
        //
        lRet = (psBuffer->pfnIoctl)(IOCTL_BUFFER_SETSAMPLERATE, psBuffer,
                                    (unsigned long)lSampleRate, 0, 0);

        //
        // If the return code was not -1, then return it now.
        //
        if(lRet != -1)
        {
            return(lRet);
        }
    }

    //
    // Success.
    //
    return(1);
}

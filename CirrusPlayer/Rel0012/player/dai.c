//****************************************************************************
//
// DAI.C - Routines to enable the DAI interface for use with an I2S CODEC, and
//         to playback and record PCM data.
//
// Copyright (c) 1999-2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include "../hwdefs.h"
#include "globals.h"

//****************************************************************************
//
// The play and record buffers.
//
//****************************************************************************
BufferState sPlayBuffer, sRecordBuffer;

//****************************************************************************
//
// DAIBufferIoctl is the buffer IOCTL handler for the digital audio
// interface's data buffers.
//
//****************************************************************************
static long
DAIBufferIoctl(unsigned long ulIoctl, BufferState *psBuffer,
               unsigned long ulParam1, unsigned long ulParam2,
               unsigned long ulParam3)
{
    //
    // Determine what to do based on the specified IOCTL.
    //
    switch(ulIoctl)
    {
        //
        // Update the pointers for the buffer.
        //
        case IOCTL_BUFFER_UPDATEREADPOINTER:
        case IOCTL_BUFFER_UPDATEWRITEPOINTER:
        {
            //
            // Make sure that the amount of data is a multiple of 4 samples.
            //
            if(ulParam1 & 3)
            {
                return(0);
            }

            //
            // Let the main buffer code handle updating the pointer.
            //
            return(-1);
        }

        //
        // Allow the default buffer handler to take care of the remaining
        // IOCTLs.
        //
        default:
        {
            return(-1);
        }
    }
}

//****************************************************************************
//
// DAIInit initializes the buffers for the digital audio interface.
//
//****************************************************************************
void
DAIInit(void)
{
    //
    // Initialize the playback buffer.
    //
    BufferInit(&sPlayBuffer, DAIBufferIoctl, 0);

    //
    // Initialize the record buffer.
    //
    BufferInit(&sRecordBuffer, DAIBufferIoctl, 0);
}

//****************************************************************************
//
// DAIEnable configures the DAI interface for communication with a stereo
// CODEC (such as the CS4340).
//
//****************************************************************************
void
DAIEnable(void)
{
    unsigned long * volatile pulPtr = (unsigned long *)HwBaseAddress;

    //
    // Do nothing if the DAI is already enabled.
    //
    if(pulPtr[HwDAIControl >> 2] & HwDAIControlDAIEN)
    {
        return;
    }

    //
    // Power on the 11.2896MHz crystal oscillator.
    //
#ifdef HwPortABCD_Codec_Power
    pulPtr[HwPortABCD >> 2] ^= HwPortABCD_Codec_Power;
#endif
#ifdef HwPortE_Codec_Power
    pulPtr[HwPortE >> 2] ^= HwPortE_Codec_Power;
#endif

    //
    // Configure the DAI controller.  We select the external clock input,
    // which is a 11.2896MHz clock, which gets divided by 128 to generate a
    // 44.1KHz sample rate.  We enable the right channel transmit interrupt
    // and hold off on enabling the DAI interface until we have completely
    // configured the interface.
    //
    pulPtr[HwDAIControl >> 2] = 0x0404 | HwDAIControlECS | HwDAIControlRCTM;

    //
    // Clear the overrun and underrun bits in the DAI status register.
    //
    pulPtr[HwDAIStatus >> 2] = 0xffffffff;

    //
    // Enable the DAI interface.
    //
    pulPtr[HwDAIControl >> 2] |= HwDAIControlDAIEN;

    //
    // Enable the FIFOs for the left and right channels.
    //
    while(!(pulPtr[HwDAIStatus >> 2] & HwDAIStatusFIFO))
    {
    }
    pulPtr[HwDAIData2 >> 2] = HwDAIData2RightEnable;
    while(!(pulPtr[HwDAIStatus >> 2] & HwDAIStatusFIFO))
    {
    }
    pulPtr[HwDAIData2 >> 2] = HwDAIData2LeftEnable;
    while(!(pulPtr[HwDAIStatus >> 2] & HwDAIStatusFIFO))
    {
    }

    //
    // Unmask the DAI interrupt.
    //
    pulPtr[HwIntMask3 >> 2] |= HwFiqDAI;
}

//****************************************************************************
//
// DAIDisable deconfigures the DAI interface, masking the DAI interrupt and
// removing the DAI interrupt handler.
//
//****************************************************************************
void
DAIDisable(void)
{
    unsigned long * volatile pulPtr = (unsigned long *)HwBaseAddress;

    //
    // Do nothing if the DAI is already disabled.
    //
    if(!(pulPtr[HwDAIControl >> 2] & HwDAIControlDAIEN))
    {
        return;
    }

    //
    // Wait until we are done playing all the data in the output buffer.
    //
    while(!BufferIsEmpty(&sPlayBuffer))
    {
    }

    //
    // Disable the DAI interface.
    //
    pulPtr[HwDAIControl >> 2] &= ~HwDAIControlDAIEN;

    //
    // Mask the DAI interrupt.
    //
    pulPtr[HwIntMask3 >> 2] &= ~HwFiqDAI;

    //
    // Power off the 11.2896MHz crystal oscillator.
    //
#ifdef HwPortABCD_Codec_Power
    pulPtr[HwPortABCD >> 2] ^= HwPortABCD_Codec_Power;
#endif
#ifdef HwPortE_Codec_Power
    pulPtr[HwPortE >> 2] ^= HwPortE_Codec_Power;
#endif
}

//****************************************************************************
//
// DAIGetPlayBuffer returns a pointer to the play buffer for the digital audio
// interface.
//
//****************************************************************************
BufferState *
DAIGetPlayBuffer(void)
{
    //
    // Return a pointer to the play buffer.
    //
    return(&sPlayBuffer);
}

//****************************************************************************
//
// DAIGetRecordBuffer returns a pointer to the record buffer for the digital
// audio interface.
//
//****************************************************************************
BufferState *
DAIGetRecordBuffer(void)
{
    //
    // Return a pointer to the record buffer.
    //
    return(&sRecordBuffer);
}

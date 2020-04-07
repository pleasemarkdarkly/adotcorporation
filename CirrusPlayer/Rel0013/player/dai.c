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
// A table containing the sample rates which we support, along with the DAI
// configuration for those sample rates.
//
//****************************************************************************
#ifdef USE_DAI_CLOCK_GENERATOR
static const struct
{
    unsigned short usSampleRate;
    unsigned short usDAIConfig;
} sSampleRates[] =
{
    { 32000, HwDAI64FsDivisor32kHz | HwDAI64FsClk74MHz },
    { 44100, HwDAI64FsDivisor44kHz | HwDAI64FsClk11MHz },
    { 48000, HwDAI64FsDivisor48kHz | HwDAI64FsClk74MHz }
};
#endif

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
        // Set the sample rate of the data buffer.
        //
        case IOCTL_BUFFER_SETSAMPLERATE:
        {
#ifdef USE_DAI_CLOCK_GENERATOR
            unsigned long * volatile pulPtr = (unsigned long *)HwBaseAddress;
            unsigned long ulIdx;

            //
            // Look up the requested sample rate in our table of supported
            // sample rates.
            //
            for(ulIdx = 0; ulIdx < 3; ulIdx++)
            {
                //
                // If the requested sample rate matches the value in the table,
                // then stop looking.
                //
                if(sSampleRates[ulIdx].usSampleRate == ulParam1)
                {
                    break;
                }
            }

            //
            // If we could not find the requested sample rate in the table,
            // then return a failure.
            //
            if(ulIdx == 3)
            {
                return(0);
            }

            //
            // Configure the digital audio interface for the specified sample
            // rate.
            //
            pulPtr[HwDAI64Fs >> 2] = sSampleRates[ulIdx].usDAIConfig |
                                     HwDAI64FsMclkEn | HwDAI64FsClockEn |
                                     HwDAI64FsEn;

            //
            // Success.
            //
            return(1);
#else
            //
            // The only sample rate we support is 44kHz.
            //
            if(ulParam1 == 44100)
            {
                return(1);
            }

            //
            // We do not support the requested sample rate.
            //
            return(0);
#endif
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

    //
    // Set the default playback rate to 44100Hz.
    //
    BufferSetSampleRate(&sPlayBuffer, 44100);
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

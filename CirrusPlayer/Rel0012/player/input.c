//****************************************************************************
//
// INPUT.C - Performs recording and input processing of a PCM stream.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include "../hwdefs.h"
#include "globals.h"
#include "buffer/buffer.h"
#include "src/src.h"

//****************************************************************************
//
// The following structure contains the state variables for the input
// processing routines.
//
//****************************************************************************
static struct
{
    //
    // The persistent state of the sample rate converter.
    //
    tSRC sSRC;

    //
    // The input buffer for the sample rate converter.
    //
    BufferState *psSRCInput;

    //
    // The buffer from which we consume 44100Hz samples.  This will be used
    // directly for 44100Hz streams, or will be the input of the sample rate
    // converter for non-44100Hz streams.
    //
    BufferState *p44100Buffer;

    //
    // The buffer into which we produce data.
    //
    BufferState sOutput;
} sInput;

//****************************************************************************
//
// InputFilter performs the actual input processing on the data that was
// recorded when a sample rate converter is being used in the system.
//
//****************************************************************************
static void
InputFilter(void)
{
    short *psLeft, *psRight;
    long lSamples, lSpace;

    //
    // Determine the number of samples available in the 44100Hz output buffer.
    //
    lSamples = BufferDataAvailable(sInput.p44100Buffer);

    //
    // If there at least 1/16th of the buffer contains data, then "copy" the
    // data to the sample rate converter's input (by moving the read pointer
    // of the 44100Hz output buffer and the write pointer of the sample rate
    // converter's input buffer).
    //
    if(lSamples >= (sInput.p44100Buffer->lLength / 16))
    {
        //
        // Get the read pointer for the 44100Hz output buffer.
        //
        BufferGetReadPointer(sInput.p44100Buffer, &psLeft, &psRight,
                             &lSamples);

        //
        // Update the read pointer for the 44100Hz output buffer.
        //
        BufferUpdateReadPointer(sInput.p44100Buffer, lSamples);

        //
        // Update the write pointer for the sample rate converter's input
        // buffer.
        //
        BufferUpdateWritePointer(sInput.psSRCInput, lSamples);
    }

    //
    // Determine the amount of space available in the output buffer.
    //
    lSpace = BufferSpaceAvailable(&(sInput.sOutput));

    //
    // Determine the number of samples that can be produced by the sample rate
    // converter.
    //
    lSamples = SRCNumSamplesAvailable(&(sInput.sSRC));

    //
    // If there are samples that can be produced, and space to write them into,
    // then sample rate convert some more samples.
    //
    if(lSpace && lSamples)
    {
        //
        // Get the write pointer for the next block of data in the output
        // buffer.
        //
        BufferGetWritePointer(&(sInput.sOutput), &psLeft, &psRight, &lSpace);

        //
        // If there are more samples to be produced than there is space to
        // write them, then limit the number of samples to be produced.
        //
        if(lSamples > lSpace)
        {
            lSamples = lSpace;
        }

        //
        // Sample rate convert some samples.
        //
        SRCFilter(&(sInput.sSRC), psLeft, psRight, lSamples);

        //
        // Update the write pointer for the output buffer.
        //
        BufferUpdateWritePointer(&(sInput.sOutput), lSamples);
    }
}

//****************************************************************************
//
// InputBufferIoctl is the buffer IOCTL handler for the input processing's
// output buffer.
//
//****************************************************************************
static long
InputBufferIoctl(unsigned long ulIoctl, BufferState *psBuffer,
                 unsigned long ulParam1, unsigned long ulParam2,
                 unsigned long ulParam3)
{
    //
    // Determine what to do based on the specified IOCTL.
    //
    switch(ulIoctl)
    {
        //
        // Determine the amount of data available in the buffer.
        //
        case IOCTL_BUFFER_DATAAVAILABLE:
        {
            //
            // Process the input data stream if necessary.
            //
            InputFilter();

            //
            // Allow the default buffer handler to determine the data
            // available.
            //
            return(-1);
        }

        //
        // Determine if the buffer is empty.
        //
        case IOCTL_BUFFER_ISEMPTY:
        {
            //
            // Process the input data stream if necessary.
            //
            InputFilter();

            //
            // Allow the default buffer handler to determine if the buffer is
            // empty.
            //
            return(-1);
        }

        //
        // Handle getting the current read pointer.
        //
        case IOCTL_BUFFER_GETREADPOINTER:
        {
            //
            // Process the input data stream if necessary.
            //
            InputFilter();

            //
            // Allow the default buffer handler to determine the current read
            // pointer.
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
// InputEnable initializes the input processing.
//
//****************************************************************************
unsigned long
InputEnable(long lSampleRate, short *psBuffer, long lLength)
{
#ifdef HwPortABCD_ADC_Enable
    unsigned long * volatile pulPtr = (unsigned long *)HwBaseAddress;
#endif
    BufferState *psOutBuffer;
    short *psLeft, *psRight;
    long lTemp;

    //
    // There is nothing to do if the input is already enabled.
    //
    if(ulSystemFlags & SYSFLAG_INPUT_ENABLED)
    {
        return(0);
    }

    //
    // Get the output buffer from the digital audio interface.
    //
    psOutBuffer = DAIGetRecordBuffer();

    //
    // Save the pointer to the output buffer for the 44100Hz stream.
    //
    sInput.p44100Buffer = psOutBuffer;

    //
    // See if the sample rate is 44100Hz.
    //
    if(lSampleRate == 44100)
    {
        //
        // The sample rate is 44100Hz, so there is not SRC input buffer.
        //
        sInput.psSRCInput = 0;
    }
    else
    {
        //
        // The sample rate is not 44100Hz, so initialize the sample rate
        // converter.
        //
        if(SRCInit(&(sInput.sSRC), 44100, lSampleRate) == 0)
        {
            //
            // We could not initialize the sample rate converter, so return an
            // error.
            //
            return(0);
        }

        //
        // Get a pointer to the sample rate converter's input buffer.
        //
        sInput.psSRCInput = SRCGetInputBuffer(&(sInput.sSRC));

        //
        // Set the input buffer for the sample rate converter.
        //
        BufferSetBuffer(sInput.psSRCInput, psBuffer, psBuffer, lLength);

        //
        // Get the initial write pointer for the sample rate converter's input
        // buffer.
        //
        BufferGetWritePointer(sInput.psSRCInput, &psLeft, &psRight, &lTemp);

        //
        // Set the output buffer for the 44100Hz stream.
        //
        BufferSetBuffer(sInput.p44100Buffer, psLeft, psLeft, lLength);

        //
        // Initialize our output buffer.
        //
        BufferInit(&(sInput.sOutput), InputBufferIoctl, 0);
    }

    //
    // Enable the ADC.
    //
#ifdef HwPortABCD_ADC_Enable
    pulPtr[HwPortABCD >> 2] ^= HwPortABCD_ADC_Enable;
#endif

    //
    // Enable the digital audio interface.
    //
    DAIEnable();

#ifdef I2CADDR_CS53L32
    //
    // Delay so the ADC can calibrate.
    //
    for(lTemp = 0; lTemp < 4096; lTemp++)
    {
    }

    //
    // Configure the CS53L32.  First, we must enable the I2C port on the
    // CS53L32 by setting the CP_EN bit (bit 0) of register 1.  We also select
    // line 2 through the PGA as the input source and enable the +20dB mic
    // boost.
    //
    I2CWrite(I2CADDR_CS53L32, 0x01, 0x63);

    //
    // Set the data format to left justified.
    //
    I2CWrite(I2CADDR_CS53L32, 0x02, 0x21);

    //
    // Set the volume control to +12dB.
    //
    I2CWrite(I2CADDR_CS53L32, 0x04, 0x0c);
    I2CWrite(I2CADDR_CS53L32, 0x05, 0x0c);

    //
    // Power on the ADC.
    //
    I2CWrite(I2CADDR_CS53L32, 0x01, 0x61);
#endif

    //
    // Indicate that the input is enabled.
    //
    DisableIRQ();
    ulSystemFlags |= SYSFLAG_INPUT_ENABLED;
    EnableIRQ();

    //
    // Success.
    //
    return(1);
}

//****************************************************************************
//
// InputDisable shuts down the input processing.
//
//****************************************************************************
void
InputDisable(void)
{
#ifdef HwPortABCD_ADC_Enable
    unsigned long * volatile pulPtr = (unsigned long *)HwBaseAddress;
#endif
    BufferState *psBuffer;

    //
    // There is nothing to do if the input is already disabled.
    //
    if(!(ulSystemFlags & SYSFLAG_INPUT_ENABLED))
    {
        return;
    }

    //
    // Get a pointer to the digital audio interface record buffer.
    //
    psBuffer = DAIGetRecordBuffer();

    //
    // Set the length of the record buffer for the digital audio interface to
    // zero.
    //
    psBuffer->lLength = 0;

    //
    // Wait until the digital audio interface has completed it's current pass
    // through the record buffer and re-loaded the FIQ banked registers from
    // the record buffer structure.
    //
    while(psBuffer->lWritePtr)
    {
        //
        // Put the EP7209 into HALT mode.
        //
        Halt();
    }

    //
    // Indicate that the output is disabled.
    //
    DisableIRQ();
    ulSystemFlags &= ~SYSFLAG_INPUT_ENABLED;
    EnableIRQ();

#ifdef I2CADDR_CS53L32
    //
    // Power off the ADC.
    //
    I2CWrite(I2CADDR_CS53L32, 0x01, 0x53);
#endif

    //
    // Disable the ADC.
    //
#ifdef HwPortABCD_ADC_Enable
    pulPtr[HwPortABCD >> 2] ^= HwPortABCD_ADC_Enable;
#endif

    //
    // Disable the digital audio interface if appropriate.
    //
    if(!(ulSystemFlags & (SYSFLAG_OUTPUT_ENABLED | SYSFLAG_INPUT_ENABLED)))
    {
        DAIDisable();
    }
}

//****************************************************************************
//
// InputGetOutputBuffer returns a pointer to the output buffer for the input
// processing.
//
//****************************************************************************
BufferState *
InputGetOutputBuffer(void)
{
    //
    // If there is no sample rate converter, then simply return the output
    // buffer for the 44100Hz stream.
    //
    if(!sInput.psSRCInput)
    {
        //
        // Return the 44100Hz output buffer.
        //
        return(sInput.p44100Buffer);
    }

    //
    // Since there is a sample rate converter, return our own output buffer.
    //
    return(&(sInput.sOutput));
}

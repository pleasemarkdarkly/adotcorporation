//****************************************************************************
//
// OUTPUT.C - Performs output processing and playback of a PCM stream.
//
// Copyright (c) 1999-2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include "../hwdefs.h"
#include "globals.h"
#include "buffer/buffer.h"
#include "tonectrl/tonectrl.h"
#include "src/src.h"
#include "volume.h"

//****************************************************************************
//
// These defines determine the type of tone control and volume control in the
// system.  If neither a hardware or software control is selected, then there
// is no control of that type.  The hardware tone and volume controls
// implemented here work with the CS43L42/CS43L43, and are both interdependent
// (i.e. you can not have just the tone control or just the volume control).
//
//****************************************************************************
//#define SOFTWARE_TONE_CONTROL
//#define SOFTWARE_VOLUME_CONTROL
#ifdef I2CADDR_CS43L43
#define HARDWARE_TONE_VOLUME_CONTROL
#endif

//****************************************************************************
//
// The following table maps the volume setting of 0 through 20 to -96dB
// through 0dB.
//
//****************************************************************************
#ifdef HARDWARE_TONE_VOLUME_CONTROL
static const unsigned char ucVolumeMap[] = { 0xa0, 0xc3, 0xcc, 0xd2,
                                             0xd7, 0xdc, 0xe0, 0xe3,
                                             0xe6, 0xe9, 0xea, 0xee,
                                             0xf0, 0xf3, 0xf5, 0xf7,
                                             0xf9, 0xfa, 0xfc, 0xfe, 0xff };
#endif

//****************************************************************************
//
// The following structure contains the state variables for the output
// processing routines.
//
//****************************************************************************
static struct
{
#ifdef SOFTWARE_TONE_CONTROL
    //
    // The persistent state of the software tone control.
    //
    tToneControl sTone;
#endif

#ifdef SOFTWARE_VOLUME_CONTROL
    //
    // The persistent state of the software volume control.
    //
    tVolumeControl sVolume;
#endif

#ifdef HARDWARE_TONE_VOLUME_CONTROL
    //
    // The current treble boost.
    //
    char cTrebleBoost;

    //
    // The current bass boost.
    //
    char cBassBoost;

    //
    // The current volume setting.
    //
    char cVolume;
#endif

    //
    // The persistent state of the sample rate converter.
    //
    tSRC sSRC;

    //
    // The buffer to which we feed 44100Hz samples.  This will be used directly
    // for 44100Hz streams, or will be the output of the sample rate converter
    // for non-44100Hz streams.
    //
    BufferState *p44100Buffer;

    //
    // The buffer to which we send output data.
    //
    BufferState *pOutputBuffer;

    //
    // The buffers used for the sample rate converter's output data.
    //
    short sOutputLeft[384], sOutputRight[384];
} sOutput;

//****************************************************************************
//
// OutputInit initializes the output processing.
//
//****************************************************************************
void
OutputInit(void)
{
    BufferState *psBuffer;

    //
    // Initialize the digital audio interface.
    //
    DAIInit();
    psBuffer = DAIGetPlayBuffer();

#ifdef SOFTWARE_VOLUME_CONTROL
    //
    // Initialize the volume control.
    //
    VolumeInit(&(sOutput.sVolume), psBuffer);
    psBuffer = VolumeGetInputBuffer(&(sOutput.sVolume));
#endif

#ifdef SOFTWARE_TONE_CONTROL
    //
    // Initialize the tone controls.
    //
    ToneInit(&(sOutput.sTone), psBuffer);
    psBuffer = ToneGetInputBuffer(&(sOutput.sTone));
#endif

    //
    // Save the pointer to the input buffer for the 44100Hz stream.
    //
    sOutput.p44100Buffer = psBuffer;

    //
    // For now, the output buffer is the buffer for 44100Hz streams (i.e. we
    // assume that the input data is 44100Hz until told otherwise).
    //
    sOutput.pOutputBuffer = psBuffer;
}

//****************************************************************************
//
// OutputEnable initializes the output processing.
//
//****************************************************************************
void
OutputEnable(void)
{
#ifdef HwPortABCD_DAC_Enable
    unsigned long * volatile pulPtr = (unsigned long *)HwBaseAddress;
#endif
#ifdef HARDWARE_TONE_VOLUME_CONTROL
    unsigned long ulLoop;
#endif

    //
    // There is nothing to do if the output is already enabled.
    //
    if(ulSystemFlags & SYSFLAG_OUTPUT_ENABLED)
    {
        return;
    }

    //
    // If the sample rate is not 44100Hz (i.e. if the 44100Hz buffer it not the
    // same as the output buffer), then enable the output processing interrupt.
    //
    if(sOutput.p44100Buffer != sOutput.pOutputBuffer)
    {
        //
        // Enable the output processing interupt.
        //
        DisableIRQ();
        ulSystemFlags |= SYSFLAG_PROCESS_OUTPUT;
        EnableIRQ();
    }

    //
    // Enable the DAC.
    //
#ifdef HwPortABCD_DAC_Enable
    pulPtr[HwPortABCD >> 2] ^= HwPortABCD_DAC_Enable;
#endif

    //
    // Enable the digital audio interface.
    //
    DAIEnable();

#ifdef HARDWARE_TONE_VOLUME_CONTROL
    //
    // Delay so the DAC can calibrate.
    //
    for(ulLoop = 0; ulLoop < 1024; ulLoop++)
    {
    }

    //
    // Configure the CS43L43.  First, we must enable the I2C port on the
    // CS43L43 by setting the CP_EN bit (bit 0) of register 1.
    //
    I2CWrite(I2CADDR_CS43L43, 0x01, 0xd3);

    //
    // Now, enable the tone controls and limiter.
    //
    I2CWrite(I2CADDR_CS43L43, 0x0a, 0x39);

    //
    // Set the data format to left justified.
    //
    I2CWrite(I2CADDR_CS43L43, 0x0b, 0x02);

    //
    // Power on the DAC.
    //
    I2CWrite(I2CADDR_CS43L43, 0x01, 0xd1);

    //
    // Delay while the DAC initializes.
    //
    for(ulLoop = 0; ulLoop < 65536 * 32; ulLoop++)
    {
    }

    //
    // Write the current tone setting.
    //
    I2CWrite(I2CADDR_CS43L43, 0x06,
             (sOutput.cBassBoost << 4) | sOutput.cTrebleBoost);

    //
    // Write the treble boost corner frequency to 7kHz, the bass boost corner
    // frequency to 200Hz, and set the two channel volumes to be identical.
    //
    I2CWrite(I2CADDR_CS43L43, 0x07, 0xa8);

    //
    // Write the current volume setting.
    //
    I2CWrite(I2CADDR_CS43L43, 0x04, ucVolumeMap[sOutput.cVolume]);
#endif

    //
    // Indicate that the output is enabled.
    //
    DisableIRQ();
    ulSystemFlags |= SYSFLAG_OUTPUT_ENABLED;
    EnableIRQ();
}

//****************************************************************************
//
// OutputDisable shuts down the output processing.
//
//****************************************************************************
void
OutputDisable(void)
{
#ifdef HwPortABCD_DAC_Enable
    unsigned long * volatile pulPtr = (unsigned long *)HwBaseAddress;
#endif
    BufferState *psBuffer;

    //
    // There is nothing to do if the output is already disabled.
    //
    if(!(ulSystemFlags & SYSFLAG_OUTPUT_ENABLED))
    {
        return;
    }

    //
    // Make sure that all the input data has been processed.
    //
    psBuffer = sOutput.pOutputBuffer;
    while(!BufferIsEmpty(psBuffer))
    {
        //
        // Put the EP7209 into HALT mode.
        //
        Halt();
    }

    //
    // Disable the output processing interrupt.
    //
    DisableIRQ();
    ulSystemFlags &= ~SYSFLAG_PROCESS_OUTPUT;
    EnableIRQ();

    //
    // Wait until the the digital audio interface has completed playback.
    //
    psBuffer = DAIGetPlayBuffer();
    while(!BufferIsEmpty(psBuffer))
    {
        //
        // Put the EP7209 into HALT mode.
        //
        Halt();
    }

#ifdef HARDWARE_TONE_VOLUME_CONTROL
    //
    // Power off the DAC.
    //
    I2CWrite(I2CADDR_CS43L43, 0x01, 0xd3);
#endif

    //
    // Disable the DAC.
    //
#ifdef HwPortABCD_DAC_Enable
    pulPtr[HwPortABCD >> 2] ^= HwPortABCD_DAC_Enable;
#endif

    //
    // Indicate that the output is disabled.
    //
    DisableIRQ();
    ulSystemFlags &= ~SYSFLAG_OUTPUT_ENABLED;
    EnableIRQ();

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
// OutputGetInputBuffer returns a pointer to the input buffer for the output
// processing.
//
//****************************************************************************
BufferState *
OutputGetInputBuffer(void)
{
    //
    // Return a pointer to the output buffer.
    //
    return(sOutput.pOutputBuffer);
}

//****************************************************************************
//
// OutputSetFormat sets the sample rate and number of channels of the input
// data stream.
//
//****************************************************************************
unsigned long
OutputSetFormat(long lSampleRate, long lChannels)
{
    BufferState *psBuffer;

    //
    // See if the sample rate is 44100Hz.
    //
    if(lSampleRate == 44100)
    {
        //
        // Since the sample rate is 44100Hz, we do not need to perform a
        // sample rate conversion.  Therefore, our output buffer is the 44100Hz
        // input buffer.
        //
        sOutput.pOutputBuffer = sOutput.p44100Buffer;

        //
        // If the output processing interrupt is currently enabled, then turn
        // it off and wait for the output buffer to drain.
        //
        if(ulSystemFlags & SYSFLAG_PROCESS_OUTPUT)
        {
            //
            // Turn off the output processing interrupt.
            //
            DisableIRQ();
            ulSystemFlags &= ~SYSFLAG_PROCESS_OUTPUT;
            EnableIRQ();

            //
            // Wait until the the digital audio interface has completed
            // playback.
            //
            psBuffer = DAIGetPlayBuffer();
            while(!BufferIsEmpty(psBuffer))
            {
                //
                // Put the EP7209 into HALT mode.
                //
                Halt();
            }
        }
    }
    else
    {
        //
        // The sample rate is not 44100Hz, so initialize the sample rate
        // converter.
        //
        if(SRCInit(&(sOutput.sSRC), lSampleRate, 44100) == 0)
        {
            //
            // We could not initialize the sample rate converter, so return an
            // error.
            //
            return(0);
        }

        //
        // Our output buffer is the sample rate converter's input buffer.
        //
        sOutput.pOutputBuffer = SRCGetInputBuffer(&(sOutput.sSRC));

        //
        // Set the buffer for the output from the SRC.
        //
        if(lChannels == 2)
        {
            BufferSetBuffer(sOutput.p44100Buffer, sOutput.sOutputLeft,
                            sOutput.sOutputRight, 384);
        }
        else
        {
            BufferSetBuffer(sOutput.p44100Buffer, sOutput.sOutputLeft,
                            sOutput.sOutputLeft, 384);
        }

        //
        // If the output is enabled, then we need to enable the output
        // processing interrupt.
        //
        if(ulSystemFlags & SYSFLAG_OUTPUT_ENABLED)
        {
            DisableIRQ();
            ulSystemFlags |= SYSFLAG_PROCESS_OUTPUT;
            EnableIRQ();
        }
    }

    //
    // Success.
    //
    return(1);
}

//****************************************************************************
//
// OutputSetTone adjusts the tone controls.
//
//****************************************************************************
void
OutputSetTone(long lTreble, long lBass, long lGain)
{
#ifdef SOFTWARE_TONE_CONTROL
    //
    // Disable the IRQ interrupt.
    //
    DisableIRQ();

    //
    // Adjust the tone controls.
    //
    ToneAdjust(&(sOutput.sTone), lTreble, lBass, lGain);

    //
    // Re-enable the IRQ interrupt.
    //
    EnableIRQ();
#endif

#ifdef HARDWARE_TONE_VOLUME_CONTROL
    //
    // If the treble boost is out of range, then adjust it.
    //
    if(lTreble < 0)
    {
        lTreble = 0;
    }
    else if(lTreble > 12)
    {
        lTreble = 12;
    }

    //
    // If the bass boost is out of range, then adjust it.
    //
    if(lBass < 0)
    {
        lBass = 0;
    }
    else if(lBass > 12)
    {
        lBass = 0;
    }

    //
    // Remember the new treble and bass boost settings.
    //
    sOutput.cTrebleBoost = lTreble;
    sOutput.cBassBoost = lBass;

    //
    // If the output is enabled, then write this new settings immediately.
    //
    if(ulSystemFlags & SYSFLAG_OUTPUT_ENABLED)
    {
        I2CWrite(I2CADDR_CS43L43, 0x06,
                 (sOutput.cBassBoost << 4) | sOutput.cTrebleBoost);
    }
#endif
}

//****************************************************************************
//
// OutputSetTreble adjusts the treble tone control.
//
//****************************************************************************
void
OutputSetTreble(long lTreble)
{
#ifdef SOFTWARE_TONE_CONTROL
    //
    // Adjust the treble control.
    //
    ToneAdjustTreble(&(sOutput.sTone), lTreble);
#endif

#ifdef HARDWARE_TONE_VOLUME_CONTROL
    //
    // If the treble boost is out of range, then adjust it.
    //
    if(lTreble < 0)
    {
        lTreble = 0;
    }
    else if(lTreble > 12)
    {
        lTreble = 12;
    }

    //
    // Remember the new treble boost setting.
    //
    sOutput.cTrebleBoost = lTreble;

    //
    // If the output is enabled, then write this new settings immediately.
    //
    if(ulSystemFlags & SYSFLAG_OUTPUT_ENABLED)
    {
        I2CWrite(I2CADDR_CS43L43, 0x06,
                 (sOutput.cBassBoost << 4) | sOutput.cTrebleBoost);
    }
#endif
}

//****************************************************************************
//
// OutputSetBass adjusts the bass tone control.
//
//****************************************************************************
void
OutputSetBass(long lBass)
{
#ifdef SOFTWARE_TONE_CONTROL
    //
    // Adjust the bass control.
    //
    ToneAdjustBass(&(sOutput.sTone), lBass);
#endif

#ifdef HARDWARE_TONE_VOLUME_CONTROL
    //
    // If the bass boost is out of range, then adjust it.
    //
    if(lBass < 0)
    {
        lBass = 0;
    }
    else if(lBass > 12)
    {
        lBass = 0;
    }

    //
    // Remember the new bass boost setting.
    //
    sOutput.cBassBoost = lBass;

    //
    // If the output is enabled, then write this new settings immediately.
    //
    if(ulSystemFlags & SYSFLAG_OUTPUT_ENABLED)
    {
        I2CWrite(I2CADDR_CS43L43, 0x06,
                 (sOutput.cBassBoost << 4) | sOutput.cTrebleBoost);
    }
#endif
}

//****************************************************************************
//
// OutputSetGain adjusts the gain control.
//
//****************************************************************************
void
OutputSetGain(long lGain)
{
#ifdef SOFTWARE_TONE_CONTROL
    //
    // Adjust the gain control.
    //
    ToneAdjustGain(&(sOutput.sTone), lGain);
#endif
}

//****************************************************************************
//
// OutputSetVolume adjusts the volume control.
//
//****************************************************************************
void
OutputSetVolume(long lGain)
{
#ifdef SOFTWARE_VOLUME_CONTROL
    //
    // Adjust the volume control.
    //
    VolumeAdjust(&(sOutput.sVolume), lGain);
#endif

#ifdef HARDWARE_TONE_VOLUME_CONTROL
    //
    // If the volume setting is out of range, then adjust it.
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
    // Remember the new volume setting.
    //
    sOutput.cVolume = lGain;

    //
    // If the output is enabled, then write this new setting immediately.
    //
    if(ulSystemFlags & SYSFLAG_OUTPUT_ENABLED)
    {
        I2CWrite(I2CADDR_CS43L43, 0x04, ucVolumeMap[sOutput.cVolume]);
    }
#endif
}

//****************************************************************************
//
// OutputFilter performs the actual output processing and queues the data to
// be played.  This is called at a rate of 5ms.  We produce as much data as
// can be held in the output buffer of the sample rate converter.
//
//****************************************************************************
void
OutputFilter(void)
{
    short *psLeft, *psRight;
    long lLength, lCount = 2;

    //
    // Process blocks until there is no space available.
    //
    while(BufferSpaceAvailable(sOutput.p44100Buffer) && lCount--)
    {
        //
        // Get the write pointer for the output buffer.
        //
        BufferGetWritePointer(sOutput.p44100Buffer, &psLeft, &psRight,
                              &lLength);

        //
        // Call the sample rate converter.
        //
        SRCFilter(&(sOutput.sSRC), psLeft, psRight, lLength);

        //
        // Update the write pointer for the output buffer.
        //
        BufferUpdateWritePointer(sOutput.p44100Buffer, lLength);
    }
}

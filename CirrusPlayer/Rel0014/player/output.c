//****************************************************************************
//
// OUTPUT.C - Performs output processing and playback of a PCM stream.
//
// Copyright (c) 1999-2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include "../hwdefs.h"
#include "globals.h"
#include "buffer/buffer.h"
#include "tonectrl/tonectrl.h"
#include "src/src.h"
#include "volume.h"
#include "srs.h"
#include "qsound.h"
#include "spatial.h"

//****************************************************************************
//
// If the I2C address for the CS43L42/CS43L43 is defined, then do not use the
// software volume and tone controls.
//
//****************************************************************************
#ifdef I2CADDR_CS43L43
#undef SOFTWARE_TONE_CONTROL
#undef SOFTWARE_VOLUME_CONTROL
#endif

//****************************************************************************
//
// The following table maps the volume setting of 0 through 20 to -96dB
// through 0dB.
//
//****************************************************************************
#ifdef I2CADDR_CS43L43
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
#ifdef SUPPORT_QSOUND
    //
    // The persistent state of the QSound control.
    //
    tQSoundControl sQSound;
#endif

#ifdef SUPPORT_SRS
    //
    // The persistent state of the SRS control.
    //
    tSRSControl sSRS;
#endif

#ifdef SUPPORT_SPATIALIZER
    //
    // The persistent state of the Spatializer control.
    //
    tSpatializerControl sSpatializer;
#endif

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

#ifdef I2CADDR_CS43L43
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
    // The buffer to which we feed base rate samples.  This will be used
    // directly for 44100Hz streams (plus 32000Hz and 48000Hz streams on the
    // EP73xx), or will be the output of the sample rate converter for all
    // other streams.
    //
    BufferState *pBaseRateBuffer;

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

#ifdef SUPPORT_SRS
    //
    // Initialize the SRS control.
    //
    SRSInit(&(sOutput.sSRS), psBuffer);
    psBuffer = SRSGetInputBuffer(&(sOutput.sSRS));
#endif

#ifdef SUPPORT_QSOUND
    //
    // Initialize the QSound control.
    //
    QSoundInit(&(sOutput.sQSound), psBuffer);
    psBuffer = QSoundGetInputBuffer(&(sOutput.sQSound));
#endif

#ifdef SUPPORT_SPATIALIZER
    //
    // Initializer the Spatializer control.
    //
    SpatializerInit(&(sOutput.sSpatializer), psBuffer);
    psBuffer = SpatializerGetInputBuffer(&(sOutput.sSpatializer));
#endif

    //
    // Save the pointer to the input buffer for the base rate stream.
    //
    sOutput.pBaseRateBuffer = psBuffer;

    //
    // For now, the output buffer is the buffer for base rate stream.
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
#ifdef I2CADDR_CS43L43
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
    // If the sample rate is a base rate (i.e. if the base rate buffer it not
    // the same as the output buffer), then enable the output processing
    // interrupt.
    //
    if(sOutput.pBaseRateBuffer != sOutput.pOutputBuffer)
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

#ifdef I2CADDR_CS43L43
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

#ifdef I2CADDR_CS43L43
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
    long lBaseRate;

    //
    // If the output processing interrupt is currently enabled, then turn it
    // off and wait for the output buffer to drain.
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
    }

    //
    // Determine the base rate at which to run the digital audio interface.
    //
    switch(lSampleRate)
    {
        //
        // 8000Hz, 16000Hz, and 32000Hz are played back at a base rate of
        // 32000Hz.
        //
        case 8000:
        case 16000:
        case 32000:
        {
            //
            // The base rate is 32000Hz.
            //
            lBaseRate = 32000;

            //
            // We're done with these rates.
            //
            break;
        }

        //
        // 11025Hz, 22050Hz, and 44100Hz are played back at a base rate of
        // 44100Hz.
        //
        case 11025:
        case 22050:
        case 44100:
        {
            //
            // The base rate is 44100Hz.
            //
            lBaseRate = 44100;

            //
            // We're done with these rates.
            //
            break;
        }

        //
        // 12000Hz, 24000Hz, and 48000Hz are played back at a base rate of
        // 48000Hz.
        //
        case 12000:
        case 24000:
        case 48000:
        {
            //
            // The base rate is 48000Hz.
            //
            lBaseRate = 48000;

            //
            // We're done with these rates.
            //
            break;
        }

        //
        // We assume a base rate of 44100Hz for all other sample rates.
        //
        default:
        {
            //
            // The base rate is 44100Hz.
            //
            lBaseRate = 44100;

            //
            // We're done with this rate.
            //
            break;
        }
    }

    //
    // Set the sample rate of the base rate buffer.
    //
    if(BufferSetSampleRate(sOutput.pBaseRateBuffer, lBaseRate) == 0)
    {
        //
        // We could not set the sample rate to the given base rate, so assume
        // a base rate of 44100Hz.
        //
        lBaseRate = 44100;
    }

    //
    // See if the sample rate is the base rate.
    //
    if(lSampleRate == lBaseRate)
    {
        //
        // Since the sample rate is the base rate, we do not need to perform a
        // sample rate conversion.  Therefore, our output buffer is the base
        // rate input buffer.
        //
        sOutput.pOutputBuffer = sOutput.pBaseRateBuffer;
    }
    else
    {
        //
        // The sample rate is not the base rate, so initialize the sample rate
        // converter.
        //
        if(SRCInit(&(sOutput.sSRC), lSampleRate, lBaseRate) == 0)
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
            BufferSetBuffer(sOutput.pBaseRateBuffer, sOutput.sOutputLeft,
                            sOutput.sOutputRight, 384);
        }
        else
        {
            BufferSetBuffer(sOutput.pBaseRateBuffer, sOutput.sOutputLeft,
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

#ifdef I2CADDR_CS43L43
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

#ifdef I2CADDR_CS43L43
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

#ifdef I2CADDR_CS43L43
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

#ifdef I2CADDR_CS43L43
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
// OutputSetSRSWidth sets the width of the stereo separation for the SRS
// processing.
//
//****************************************************************************
#if defined(SUPPORT_WOW_FULL) || defined(SUPPORT_WOW_MINIMAL) || \
    defined(SUPPORT_SRS_HEADPHONE)
void
OutputSetSRSWidth(long lWidth)
{
    //
    // If the width is out of range, then adjust it.
    //
    if(lWidth < 0)
    {
        lWidth = 0;
    }
    else if(lWidth > 20)
    {
        lWidth = 20;
    }

    //
    // Adjust the width of the SRS processing.
    //
    SRSAdjustWidth(&(sOutput.sSRS), 0x06666666 * lWidth);
}
#endif

//****************************************************************************
//
// OutputSetSRSTruBass sets the amount of TruBass enhancement in the SRS
// processing.
//
//****************************************************************************
#if defined(SUPPORT_WOW_FULL) || defined(SUPPORT_WOW_MINIMAL) || \
    defined(SUPPORT_TRU_BASS)
void
OutputSetSRSTruBass(long lBass)
{
    //
    // If the bass enhancement is out of range, then adjust it.
    //
    if(lBass < 0)
    {
        lBass = 0;
    }
    else if(lBass > 20)
    {
        lBass = 20;
    }

    //
    // Adjust the TruBass enhancement in the SRS processing.
    //
    SRSAdjustTruBass(&(sOutput.sSRS), 0x06666666 * lBass);
}
#endif

//****************************************************************************
//
// OutputSetQSoundWidth sets the width of the stereo separation for the QSound
// processing.
//
//****************************************************************************
#ifdef SUPPORT_QSOUND
void
OutputSetQSoundWidth(long lWidth)
{
    //
    // If the width is out of range, then adjust it.
    //
    if(lWidth < 0)
    {
        lWidth = 0;
    }
    else if(lWidth > 20)
    {
        lWidth = 20;
    }

    //
    // Adjust the width of the QSound processing.
    //
    QSoundAdjustWidth(&(sOutput.sQSound), (0x7fff * lWidth) / 20);
}
#endif

//****************************************************************************
//
// OutputSetQSoundMode sets the mode of the QSound processing.
//
//****************************************************************************
#ifdef SUPPORT_QSOUND
void
OutputSetQSoundMode(long lMode)
{
    //
    // If the mode is invalid, then adjust it.
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
    // Set the mode of the QSound processing.
    //
    QSoundSetMode(&(sOutput.sQSound), lMode);
}
#endif

//****************************************************************************
//
// OutputSetSpatializerSpace sets the space control in the Spatializer
// processing.
//
//****************************************************************************
#ifdef SUPPORT_SPATIALIZER_3D
void
OutputSetSpatializerSpace(long lSpace)
{
    //
    // If the space is out of range, then adjust it.
    //
    if(lSpace < 0)
    {
        lSpace = 0;
    }
    else if(lSpace > 20)
    {
        lSpace = 20;
    }

    //
    // Adjust the space control of the Spatializer processing.
    //
    SpatializerAdjustSpace(&(sOutput.sSpatializer), (0x2000 * lSpace) / 20);
}
#endif

//****************************************************************************
//
// OutputSetSpatializerCenter sets the center control in the Spatializer
// processing.
//
//****************************************************************************
#ifdef SUPPORT_SPATIALIZER_3D
void
OutputSetSpatializerCenter(long lCenter)
{
    //
    // If the center is out of range, then adjust it.
    //
    if(lCenter < 0)
    {
        lCenter = 0;
    }
    else if(lCenter > 20)
    {
        lCenter = 20;
    }

    //
    // Adjust the center control of the Spatializer processing.
    //
    SpatializerAdjustCenter(&(sOutput.sSpatializer), (0x2000 * lCenter) / 20);
}
#endif

//****************************************************************************
//
// OutputSetSpatializerHeadphone sets the virtual speaker angle of the
// headphone control of the Spatializer processing.
//
//****************************************************************************
#ifdef SUPPORT_SPATIALIZER_HEADPHONE
void
OutputSetSpatializerHeadphone(long lHeadphone)
{
    //
    // Adjust the headphone control of the Spatializer processing.
    //
    SpatializerAdjustHeadphone(&(sOutput.sSpatializer), lHeadphone);
}
#endif

//****************************************************************************
//
// OutputSetSpatializerBass sets the bass control in the Spatializer
// processing.
//
//****************************************************************************
#ifdef SUPPORT_SPATIALIZER_VIBE
void
OutputSetSpatializerBass(long lBass)
{
    //
    // If the bass is out of range, then adjust it.
    //
    if(lBass < 0)
    {
        lBass = 0;
    }
    else if(lBass > 20)
    {
        lBass = 20;
    }

    //
    // Adjust the bass control of the Spatializer processing.
    //
    SpatializerAdjustBass(&(sOutput.sSpatializer), (0x3fff * lBass) / 20);
}
#endif

//****************************************************************************
//
// OutputSetSpatializerVirtualBass sets the virtual bass control in the
// Spatializer processing.
//
//****************************************************************************
#ifdef SUPPORT_SPATIALIZER_VIBE
void
OutputSetSpatializerVirtualBass(long lBass)
{
    //
    // If the virtual bass is out of range, then adjust it.
    //
    if(lBass < 0)
    {
        lBass = 0;
    }
    else if(lBass > 20)
    {
        lBass = 20;
    }

    //
    // Adjust the virtual bass control of the Spatializer processing.
    //
    SpatializerAdjustVirtualBass(&(sOutput.sSpatializer),
                                 (0x2000 * lBass) / 20);
}
#endif

//****************************************************************************
//
// OutputSetSpatializerBassColor sets the bass color control in the
// Spatializer processing.
//
//****************************************************************************
#ifdef SUPPORT_SPATIALIZER_VIBE
void
OutputSetSpatializerBassColor(long lBassColor)
{
    //
    // Adjust the bass color control of the Spatializer processing.
    //
    SpatializerAdjustBassColor(&(sOutput.sSpatializer), lBassColor);
}
#endif

//****************************************************************************
//
// OutputSetSpatializerLoudness sets the loudness control in the Spatializer
// processing.
//
//****************************************************************************
#ifdef SUPPORT_SPATIALIZER_LOUDNESS
void
OutputSetSpatializerLoudness(long lLoudness)
{
    //
    // Adjust the loudness control of the Spatializer processing.
    //
    SpatializerAdjustLoudness(&(sOutput.sSpatializer), lLoudness);
}
#endif

//****************************************************************************
//
// OutputSetSpatializerVolume sets the volume control in the Spatializer
// processing.
//
//****************************************************************************
#ifdef SUPPORT_SPATIALIZER
void
OutputSetSpatializerVolume(long lVolume)
{
    //
    // If the volume is out of range, then adjust it.
    //
    if(lVolume < 0)
    {
        lVolume = 0;
    }
    else if(lVolume > 20)
    {
        lVolume = 20;
    }

    //
    // Adjust the volume control of the Spatializer processing.
    //
    SpatializerAdjustVolume(&(sOutput.sSpatializer), (0x1000 * lVolume) / 20);
}
#endif

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
    while(BufferSpaceAvailable(sOutput.pBaseRateBuffer) && lCount--)
    {
        //
        // Get the write pointer for the output buffer.
        //
        BufferGetWritePointer(sOutput.pBaseRateBuffer, &psLeft, &psRight,
                              &lLength);

        //
        // Call the sample rate converter.
        //
        SRCFilter(&(sOutput.sSRC), psLeft, psRight, lLength);

        //
        // Update the write pointer for the output buffer.
        //
        BufferUpdateWritePointer(sOutput.pBaseRateBuffer, lLength);
    }
}

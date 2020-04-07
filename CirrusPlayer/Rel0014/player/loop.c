//****************************************************************************
//
// LOOP.C - ADC to DAC loopback code to test the ADC interface.
//
// Copyright (c) 2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include "globals.h"
#ifdef REQUIRE_LOOPBACK

//****************************************************************************
//
// The rate at which the data stream is captured and played back.  The only
// rates which will work are 44.1kHz, 22.5kHz, 11.025kHz, and 8kHz.  On the
// EP73XX, 48kHz, 32kHz, 24kHz, 16kHz, and 12kHz will also work.
//
//****************************************************************************
#define RATE                                    8000

//****************************************************************************
//
// The structure which contains the persistent state of the loopback code.
//
//****************************************************************************
typedef struct
{
    //
    // A buffer to contain the sample rate converted data captured from the
    // ADC.
    //
    short psIn[1024 + 12];

    //
    // A buffer to contain the data to be played to the DAC.
    //
    short psOut[1024 + 12];

    //
    // A buffer to contain the non-sample rate converted data captured from
    // the ADC.
    //
    short psTemp[6144 + 12];
} sLoop;

//****************************************************************************
//
// Loop captures data from the ADC and plays it out via the DAC.
//
//****************************************************************************
unsigned long
Loop(void)
{
    BufferState *psBufferIn, *psBufferOut;
    sLoop *psLoop = (sLoop *)ulEndOfRAM;
    short *psLeftIn, *psRightIn, *psLeftOut, *psRightOut;
    long lRead, lWrite, lFirst = 1;
    unsigned long ulNumSamples = 0, ulButtons, ulRet;

    //
    // Enable the input processing.
    //
    if(InputEnable(RATE, psLoop->psTemp, 6144) == 0)
    {
        return(0);
    }

    //
    // Get the output buffer of the input processing.
    //
    psBufferIn = InputGetOutputBuffer();

    //
    // Supply our data buffer to the input processing.
    //
    BufferSetBuffer(psBufferIn, psLoop->psIn, psLoop->psIn, 1024);

    //
    // Configure the output processing.
    //
    if(OutputSetFormat(RATE, 1) == 0)
    {
        InputDisable();
        return(0);
    }

    //
    // Get the input buffer of the output processing.
    //
    psBufferOut = OutputGetInputBuffer();

    //
    // Supply our data buffer to the output processing.
    //
    BufferSetBuffer(psBufferOut, psLoop->psOut, psLoop->psOut, 1024);

    //
    // Enable the output processing.
    //
    OutputEnable();

    //
    // Inform the user interface that we are in loopback mode.
    //
    UISetMode(MODE_LOOPBACK);

    //
    // Main loopback loop.  We explicitly break out of the loop when we are
    // done with the loopback.
    //
    while(1)
    {
        //
        // Wait until there are 256 sample available in the input buffer.
        //
        while(BufferDataAvailable(psBufferIn) <= 256)
        {
            //
            // Put the EP7209 into IDLE mode.
            //
            Halt();
        }

        //
        // See if this is the first time through the loop.
        //
        if(lFirst)
        {
            //
            // Since this is the first time through the loop, move the write
            // pointer of the output buffer by 256, so there will always be
            // a 256 sample delay between the input and output (to avoid the
            // output underrunning).  This delay equates to 32ms when looping
            // at 8kHz, and 5.8ms when looping at 44.1kHz.
            //
            BufferUpdateWritePointer(psBufferOut, 256);

            //
            // Indicate that we have been through the loop.
            //
            lFirst = 0;
        }

        //
        // Get the read pointer for the input buffer.
        //
        BufferGetReadPointer(psBufferIn, &psLeftIn, &psRightIn, &lRead);

        //
        // Get the write pointer for the output buffer.
        //
        BufferGetWritePointer(psBufferOut, &psLeftOut, &psRightOut, &lWrite);

        //
        // Copy the samples from the input buffer to the output buffer.
        //
        memcpy(psLeftOut, psLeftIn, 256 * sizeof(short));

        //
        // Indicate that we've read 256 samples from the input buffer.
        //
        BufferUpdateReadPointer(psBufferIn, 256);

        //
        // Indicate that there are 256 new samples in the output buffer.
        //
        BufferUpdateWritePointer(psBufferOut, 256);

        //
        // Increment the count of samples looped.
        //
        ulNumSamples += 256;

        //
        // Inform the user interface of the new time.
        //
        UISetCurrentTime(((ulNumSamples / RATE) * 1000) +
                         (((ulNumSamples % RATE) * 1000) / RATE));

        //
        // Get the current set of pressed virtual buttons.
        //
        ulButtons = UIGetButtons();

        //
        // Is a download being requested?
        //
        if(ulSystemFlags & SYSFLAG_START_DOWNLOAD)
        {
            //
            // Indicate that there is nothing further to do.
            //
            ulRet = 0;

            //
            // Break out of the loop.
            //
            break;
        }

        //
        // Is the power button being pressed?
        //
        else if(ulButtons & BUTTON_POWER)
        {
            //
            // Indicate that we should power off the player.
            //
            ulRet = BUTTON_POWER;

            //
            // Break out of the loop.
            //
            break;
        }

        //
        // Is the stop button being pressed?
        //
        else if(ulButtons & BUTTON_STOP)
        {
            //
            // Indicate that there is nothing further to do.
            //
            ulRet = 0;

            //
            // Break out of the loop.
            //
            break;
        }
    }

    //
    // Disable the input processing.
    //
    InputDisable();

    //
    // Disable the output processing.
    //
    OutputDisable();

    //
    // Remove our buffer from the output processing.
    //
    BufferSetBuffer(psBufferOut, 0, 0, 0);

    //
    // Remove our buffer from the input processing.
    //
    BufferSetBuffer(psBufferIn, 0, 0, 0);

    //
    // Return the appropriate value.
    //
    return(ulRet);
}
#endif

//****************************************************************************
//
// UISER.C - Functions for handling the user interface for the player, using
//           the serial port as the display.
//
// Copyright (c) 1999-2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include "../hwdefs.h"
#include "globals.h"

//****************************************************************************
//
// We only include the remainder of this file if the serial port is used for
// our user interface.
//
//****************************************************************************
#ifdef UI_IS_SERIAL_PORT

//****************************************************************************
//
// Definitions for the various delays in the user interface, specified in
// counts of the 50Hz user interface system clock.
//
//     SLEEP_DELAY         The amount of inactivity which will cause the
//                         player to automatically power down.
//     POWER_DELAY         The amount of time the power button must be
//                         pressed to power down the player.
//     RECORD_DELAY        The amount of time the record button must be
//                         pressed to start voice recording.
//     SEEK_DELAY          The amount of time the fast forward and rewind
//                         buttons must be pressed to start seeking within the
//                         track when playing or paused or to start skipping
//                         tracks when stopped.
//     SEEK_ACCEL          The amount of time the fast forward and rewind
//                         buttons must be pressed to continue seeking within
//                         the track when the player is playing or paused.
//                         Should be a power of two.
//     TRACK_ACCEL         The amount of time the fast forward and rewind
//                         buttons must be pressed to continue skipping tracks
//                         when the player is stopped.  Should be a power of
//                         two.
//     VOLUME_DELAY        The amount of time the volume buttons must be
//                         pressed to start adjusting the volume.
//     VOLUME_ACCEL        The amount of time the volume buttons must be
//                         pressed to continue adjusting the volume.  Should
//                         be a power of two.
//
// For values which have a xxx_DELAY and xxx_ACCEL pair, the xxx_DELAY
// determines the amount of the time the button must be pressed to start
// performing that function, and the xxx_ACCEL determines the amount of time
// between iterative operations of that function.  For example, after holding
// the volume up button for VOLUME_DELAY time, the player will turn up the
// volume by one.  Then, every VOLUME_ACCEL time after that until the button
// is released, the player will turn up the volume by one.
//
//****************************************************************************
#define SLEEP_DELAY                             1500    // 30 seconds
#define POWER_DELAY                             75      // 1.5 seconds
#define RECORD_DELAY                            50      // 1 second
#define SEEK_DELAY                              25      // .5 seconds
#define SEEK_ACCEL                              8       // .16 seconds
#define TRACK_ACCEL                             8       // .16 seconds
#define VOLUME_DELAY                            25      // .5 seconds
#define VOLUME_ACCEL                            4       // .08 seconds

//****************************************************************************
//
// The number of clock ticks (which runs at 50Hz) that each button must be
// pressed for the hold function to occur instead of the press function (i.e.
// skipping forward within a song instead of advancing to the next song).
//
//****************************************************************************
static const unsigned char ucHoldTimes[8] = { POWER_DELAY,
                                              0,
                                              0,
                                              SEEK_DELAY,
                                              SEEK_DELAY,
                                              VOLUME_DELAY,
                                              RECORD_DELAY,
                                              VOLUME_DELAY };

//****************************************************************************
//
// The various bit fields in the flags member of the user interface persistent
// state.
//
//****************************************************************************
#define FLAG_VOLUME_MASK                        0x0000001f
#define FLAG_BASS_BOOST                         0x00000020
#define FLAG_TREBLE_BOOST                       0x00000040
#define FLAG_WAIT_FOR_ALL_UP                    0x00000080
#define FLAG_VOLUME_SHIFT                       0

//****************************************************************************
//
// The persistent state of the user interface code.
//
//****************************************************************************
static struct
{
    //
    // Various flags indicating the current state of the user interface.
    //
    unsigned long ulFlags;

    //
    // The count of the number of clock ticks that have occurred.  This is our
    // time base for the various time-based elements of the user interface
    // (such as automatically powering down the player after 30 seconds of
    // inactivity).
    //
    unsigned short usTickCount;

    //
    // The time at which the next time delayed action is to occur.  This is
    // used to automatically power down the player.
    //
    unsigned short usTargetTime;

    //
    // The value of usTickCount when each of the buttons was pressed.  These
    // values only have a real meaning when the corresponding bit in ucState
    // is set.
    //
    unsigned short usPressTime[8];

    //
    // The current set of virtual buttons that are pressed.  This is returned
    // to the player to control the playback of audio.
    //
    unsigned long ulKeys;

    //
    // The current de-bounced state of the buttons.
    //
    unsigned char ucState;

    //
    // A two-cycle clock used to de-bounce the buttons.
    //
    unsigned char ucClock;

    //
    // The current mode of the player.
    //
    unsigned char ucMode;

    //
    // The name of the codec being used to decode the current file.
    //
    char *pcCodec;

    //
    // The bitrate of the current file.
    //
    unsigned long ulBitRate;

    //
    // The length of the current file in milliseconds.
    //
    unsigned long ulTotalMS;

    //
    // The current position within the current file in milliseconds.
    //
    unsigned long ulCurrentMS;
} sUI;

//****************************************************************************
//
// Initializes the serial port and configures it for 115,200, 8-N-1.
//
//****************************************************************************
static void
SerialInit(void)
{
    unsigned long * volatile pulPtr = (unsigned long *)HwBaseAddress;

    //
    // Configure the serial port for 115,200, 8-N-1.
    //
    pulPtr[HwUartControl >> 2] = HwUartControlFifoEnable |
                                 HwUartControlDataLength8 |
                                 HwUartControlRate115200;

    //
    // Enable the serial port.
    //
    pulPtr[HwControl >> 2] |= HwControlUartEnable;
}

//****************************************************************************
//
// Sends a character to the serial port.
//
//****************************************************************************
static void
SerialSendChar(char cChar)
{
    unsigned long * volatile pulPtr = (unsigned long *)HwBaseAddress;

    //
    // Wait until there is space available in the transmit FIFO.
    //
    while(pulPtr[HwStatus >> 2] & HwStatusUartTxFifoFull)
    {
    }

    //
    // Write the character to the transmit FIFO.
    //
    pulPtr[HwUartData >> 2] = cChar;
}

//****************************************************************************
//
// Sends a string of characters to the serial port.
//
//****************************************************************************
static void
SerialSendString(char *pcString)
{
    //
    // Send the string character by character.
    //
    while(*pcString)
    {
        SerialSendChar(*pcString++);
    }
}

//****************************************************************************
//
// Waits until the serial port is done transmitting all data from it's
// transmit FIFO.
//
//****************************************************************************
static void
SerialWaitForTransmit(void)
{
    unsigned long * volatile pulPtr = (unsigned long *)HwBaseAddress;

    //
    // Wait until the serial port transmit is not busy.
    //
    while(pulPtr[HwStatus >> 2] & HwStatusUartTxBusy)
    {
    }
}

//****************************************************************************
//
// UIInit initializes the user interface.
//
//****************************************************************************
void
UIInit(void)
{
    //
    // Initialize the serial port.
    //
    SerialInit();

    //
    // Initialize the user interface flags.  Set the volume to mid-scale and
    // turn off the treble and bass boost.
    //
    sUI.ulFlags = (10 << FLAG_VOLUME_SHIFT);

    //
    // Initialize the tick count to zero.
    //
    sUI.usTickCount = 0;

    //
    // Initialize the state of the button debouncer.  All buttons start in the
    // not pressed state.
    //
    sUI.ulKeys = 0;
    sUI.ucState = 0;
    sUI.ucClock = 0;

    //
    // Initialize the volume.
    //
    OutputSetVolume(10);

    //
    // Initialize the treble and bass boost.
    //
    OutputSetTone(0, 0, -6);
}

//****************************************************************************
//
// UIPrintTime displays the specified time (given in milliseconds).
//
//****************************************************************************
static void
UIPrintTime(unsigned long ulMS)
{
    unsigned long ulMinutes;

    //
    // Compute the number of minutes and seconds.
    //
    ulMS = (ulMS + 500) / 1000;
    ulMinutes = ulMS / 60;
    ulMS -= ulMinutes * 60;

    //
    // Send the time in MM:SS format to the serial port.
    //
    if(ulMinutes >= 10)
    {
        SerialSendChar('0' + ((ulMinutes / 10) % 10));
    }
    else
    {
        SerialSendChar(' ');
    }
    SerialSendChar('0' + (ulMinutes % 10));
    SerialSendChar(':');
    SerialSendChar('0' + ((ulMS / 10) % 10));
    SerialSendChar('0' + (ulMS % 10));
}

//****************************************************************************
//
// UIPrintBitRate displays the specified bitrate.
//
//****************************************************************************
static void
UIPrintBitRate(unsigned long ulBitRate)
{
    //
    // Convert the bitrate to kbps.
    //
    ulBitRate = (ulBitRate + 500) / 1000;

    //
    // Send the bitrate in XXX format to the serial port.
    //
    if(ulBitRate >= 100)
    {
        SerialSendChar('0' + ((ulBitRate / 100) % 10));
    }
    else
    {
        SerialSendChar(' ');
    }
    SerialSendChar('0' + ((ulBitRate / 10) % 10));
    SerialSendChar('0' + (ulBitRate % 10));
}

//****************************************************************************
//
// UIUpdateDisplay redraws the "display" on the serial port based on the
// current mode of the player.
//
//****************************************************************************
static void
UIUpdateDisplay(void)
{
    //
    // Figure out what to display based on the current mode.
    //
    switch(sUI.ucMode)
    {
        //
        // We are going into low power mode.
        //
        case MODE_POWER_OFF:
        {
            //
            // Clear the user display.
            //
            SerialSendString("\r                      ");

            //
            // Wait until the serial port transmit FIFO is empty.
            //
            SerialWaitForTransmit();

            //
            // We're done with this state.
            //
            break;
        }

        //
        // We are coming out of low power mode.
        //
        case MODE_POWER_ON:
        {
            //
            // We're done with this state.
            //
            break;
        }

        //
        // We are downloading data from the host.
        //
        case MODE_DOWNLOAD:
        {
            //
            // Tell the user that we are downloading.
            //
            SerialSendString("\rDownloading           ");

            //
            // We're done with this state.
            //
            break;
        }

        //
        // We are stopped.
        //
        case MODE_STOP:
        {
            //
            // Tell the user that we are stopped.
            //
            SerialSendString("\rStopped ");

            //
            // Print out the name of the codec being used.
            //
            if(sUI.pcCodec)
            {
                SerialSendChar(sUI.pcCodec[0]);
                SerialSendChar(sUI.pcCodec[1]);
                SerialSendChar(sUI.pcCodec[2]);
                SerialSendChar(sUI.pcCodec[3]);
            }
            else
            {
                SerialSendString("    ");
            }

            //
            // Print out the bitrate of the current file.
            //
            if(sUI.ulBitRate)
            {
                SerialSendChar(' ');
                UIPrintBitRate(sUI.ulBitRate);
            }
            else
            {
                SerialSendString("    ");
            }

            //
            // Print out the total length of the file.
            //
            if(sUI.ulTotalMS)
            {
                SerialSendChar(' ');
                UIPrintTime(sUI.ulTotalMS);
            }
            else
            {
                SerialSendString("      ");
            }

            //
            // We're done with this state.
            //
            break;
        }

        //
        // We are playing a file.
        //
        case MODE_PLAY:
        {
            //
            // Tell the user that we are playing.
            //
            SerialSendString("\rPlaying ");

            //
            // Print out the name of the codec being used.
            //
            SerialSendChar(sUI.pcCodec[0]);
            SerialSendChar(sUI.pcCodec[1]);
            SerialSendChar(sUI.pcCodec[2]);
            SerialSendChar(sUI.pcCodec[3]);

            //
            // Print out the bitrate of the current file.
            //
            SerialSendChar(' ');
            UIPrintBitRate(sUI.ulBitRate);

            //
            // Print out the current position in the file.
            //
            SerialSendChar(' ');
            UIPrintTime(sUI.ulCurrentMS);

            //
            // We're done with this state.
            //
            break;
        }

        //
        // We are paused in the middle of the playback of a file.
        //
        case MODE_PAUSE:
        {
            //
            // Tell the user that we are paused.
            //
            SerialSendString("\rPaused  ");

            //
            // Print out the name of the codec being used.
            //
            SerialSendChar(sUI.pcCodec[0]);
            SerialSendChar(sUI.pcCodec[1]);
            SerialSendChar(sUI.pcCodec[2]);
            SerialSendChar(sUI.pcCodec[3]);

            //
            // Print out the bitrate of the current file.
            //
            SerialSendChar(' ');
            UIPrintBitRate(sUI.ulBitRate);

            //
            // Print out the current position in the file.
            //
            SerialSendChar(' ');
            UIPrintTime(sUI.ulCurrentMS);

            //
            // We're done with this state.
            //
            break;
        }

        //
        // We are recording.
        //
        case MODE_RECORD:
        {
            //
            // Tell the user that we are recording.
            //
            SerialSendString("\rRecord  ");

            //
            // Print out the name of the codec being used.
            //
            SerialSendChar(sUI.pcCodec[0]);
            SerialSendChar(sUI.pcCodec[1]);
            SerialSendChar(sUI.pcCodec[2]);
            SerialSendChar(sUI.pcCodec[3]);

            //
            // Print out the bitrate of the current file.
            //
            SerialSendChar(' ');
            UIPrintBitRate(sUI.ulBitRate);

            //
            // Print out the current position in the file.
            //
            SerialSendChar(' ');
            UIPrintTime(sUI.ulCurrentMS);

            //
            // We're done with this state.
            //
            break;
        }
    }
}

//****************************************************************************
//
// UISetMode is called to inform us of changes in the current transport state.
//
//****************************************************************************
void
UISetMode(unsigned long ulMode)
{
    //
    // Remember the current mode.
    //
    sUI.ucMode = ulMode;

    //
    // We need to move the target time so that we do not get an automatic
    // power off.
    //
    sUI.usTargetTime = sUI.usTickCount + SLEEP_DELAY;

    //
    // Update the display.
    //
    UIUpdateDisplay();
}

//****************************************************************************
//
// UIFileLoaded is called when a new file has been opened.  It is up to the UI
// code to determine whatever information it needs about the file and to
// update any indicators which are based on the currently opened file.
//
//****************************************************************************
void
UIFileLoaded(char *pcFileName, unsigned long ulTrack)
{
    //
    // Get the name of the current codec.
    //
    CodecGetName(&sUI.pcCodec);

    //
    // Get the bitrate of the current file.
    //
    CodecGetBitRate(&sUI.ulBitRate);

    //
    // Get the length of the current file.
    //
    CodecGetLength(&sUI.ulTotalMS);

    //
    // When a file is loaded, the current time defaults to zero.
    //
    sUI.ulCurrentMS = 0;

    //
    // Update the display.
    //
    UIUpdateDisplay();
}

//****************************************************************************
//
// UIFileUnloaded is called when there is no file opened.  It is up to the UI
// to clear any variables and/or indicators which are related to a currently
// opened file.
//
//****************************************************************************
void
UIFileUnloaded(void)
{
    //
    // Clear the variables which relate to the currently loaded file.
    //
    sUI.pcCodec = 0;
    sUI.ulBitRate = 0;
    sUI.ulTotalMS = 0;
    sUI.ulCurrentMS = 0;
}

//****************************************************************************
//
// UISetCurrentTime is called to inform us of a change in the current position
// (in milliseconds) within the current file.
//
//****************************************************************************
void
UISetCurrentTime(unsigned long ulMS)
{
    unsigned long ulT1, ulT2;

    //
    // Remember the current position.
    //
    ulT1 = (sUI.ulCurrentMS + 500) / 1000;
    sUI.ulCurrentMS = ulMS;
    ulT2 = (sUI.ulCurrentMS + 500) / 1000;

    //
    // Print the current time.
    //
    if((sUI.ucMode != MODE_STOP) && (ulT1 != ulT2))
    {
        SerialSendString("\b\b\b\b\b");
        UIPrintTime(ulMS);
    }
}

//****************************************************************************
//
// UIGetButtons returns the current state of the buttons.  This performs a
// mapping of the physical buttons to a logical set of buttons.  In fact, a
// physical button may correspond to more than one logical button (based on
// any criteria, such as being pressed in combination with another button or
// being held for a certain amount of time).
//
//****************************************************************************
unsigned long
UIGetButtons(void)
{
    unsigned long ulRet;

    //
    // Disable interrupts so that the buttons can not be modified while we are
    // reading them.
    //
    DisableIRQ();

    //
    // Get the current set of pressed buttons.
    //
    ulRet = sUI.ulKeys;

    //
    // Clear the current set of pressed buttons.
    //
    sUI.ulKeys = 0;

    //
    // Re-enable interrupts.
    //
    EnableIRQ();

    //
    // Return the set of pressed buttons.
    //
    return(ulRet);
}

//****************************************************************************
//
// Scans the buttons on the player and determines which ones were pressed,
// held, and released.
//
//****************************************************************************
static __inline unsigned long
ScanButtons(void)
{
    unsigned long * volatile pulPtr = (unsigned long *)HwBaseAddress;
    unsigned long ulKeys, ulDiff, ulIdx, ulButtons = 0;

    //
    // Get the current state of the keys.
    //
    ulKeys = pulPtr[HwPortABCD >> 2] & HwPortABCD_Button_Mask;

    //
    // See which keys have changed.
    //
    ulDiff = ulKeys ^ sUI.ucState;

    //
    // Advance the clock for all the keys.
    //
    sUI.ucClock ^= 0xff;

    //
    // Mask off the clock for the keys which did not change state.
    //
    sUI.ucClock &= ulDiff;

    //
    // Now, the bit positions of clock which are zero indicate keys that either
    // have not changed state or which have been in their new state for two
    // consecutive sampling periods.  So, update the key state based on the new
    // state of the clock.
    //
    sUI.ucState &= sUI.ucClock;
    sUI.ucState |= ~sUI.ucClock & ulKeys;
 
    //
    // Loop through the keys and perform press, release, and hold processing
    // on them.
    //
    for(ulIdx = 0; ulIdx < 8; ulIdx++)
    {
        //
        // If this key did not change state, then there is nothing to do.
        //
        if(!((ulDiff ^ sUI.ucClock) & (1 << ulIdx)))
        {
            continue;
        }

        //
        // If this key was just pressed, save the time at which it was pressed.
        //
        if(ulKeys & (1 << ulIdx))
        {
            //
            // Save the time at which this button was pressed.
            //
            sUI.usPressTime[ulIdx] = sUI.usTickCount;

            //
            // If there is no hold time for this button (i.e. it does not have
            // separate functions for press and for hold), then indicate that
            // this button has been pressed.
            //
            if(ucHoldTimes[ulIdx] == 0)
            {
                ulButtons |= 0x00000001 << ulIdx;
            }

            //
            // There is a hold time for this button, so at least indicate that
            // it has just been pressed (for buttons that are accelerated, such
            // as the volume up button).
            //
            else
            {
                ulButtons |= 0x00010000 << ulIdx;
            }
        }

        //
        // This key was just released, so determine if it was pressed or held,
        // based on the elapsed time it was pressed.
        //
        else
        {
            //
            // If the button was pressed for less than the hold time, then the
            // button was pressed, not held.
            //
            if((((sUI.usTickCount - sUI.usPressTime[ulIdx]) & 0xffff) <
                ucHoldTimes[ulIdx]) && (ucHoldTimes[ulIdx] != 0))
            {
                ulButtons |= 0x00000001 << ulIdx;
            }

            //
            // Indicate that this button has been released.
            //
            ulButtons |= 0x01000000 << ulIdx;
        }
    }

    //
    // Return the current button state.
    //
    return(ulButtons);
}

//****************************************************************************
//
// Takes care of converting physical button presses/holds into virtual button
// presses.
//
//****************************************************************************
static __inline void
HandleButtons(unsigned long ulButtons)
{
    unsigned long ulTime;

    //
    // See if the treble button has been pressed.
    //
    if(ulButtons & 0x01)
    {
        //
        // Change the state of the treble boost.
        //
        sUI.ulFlags ^= FLAG_TREBLE_BOOST;

        //
        // Set the new state of the treble boost.
        //
        OutputSetTreble(sUI.ulFlags & FLAG_TREBLE_BOOST ? 12 : 0);
    }

    //
    // See if the power off button has been pressed.
    //
    ulTime = (sUI.usTickCount - sUI.usPressTime[0]) & 0xffff;
    if((sUI.ucState & 0x01) && (ulTime >= POWER_DELAY))
    {
        //
        // Indicate that the power button was pressed.
        //
        sUI.ulKeys |= BUTTON_POWER;
    }

    //
    // See if the play button has been pressed.
    //
    if(ulButtons & 0x02)
    {
        //
        // Indicate that the play button was pressed.  This maps to both the
        // play and pause virtual keys.
        //
        sUI.ulKeys |= BUTTON_PLAY | BUTTON_PAUSE;
    }

    //
    // See if the stop button has been pressed.
    //
    if(ulButtons & 0x04)
    {
        //
        // Indicate that the stop button was pressed.
        //
        sUI.ulKeys |= BUTTON_STOP;
    }

    //
    // The handling of the fast forward and rewind buttons are dependent upon
    // the current mode of the player.  See if we are plaing or paused.
    //
    if(((sUI.ucMode & 0x1f) == MODE_PLAY) ||
       ((sUI.ucMode & 0x1f) == MODE_PAUSE))
    {
        //
        // See if the previous track button has been pressed.
        //
        if(ulButtons & 0x08)
        {
            //
            // Indicate that the previous track button was pressed.
            //
            sUI.ulKeys |= BUTTON_PREV;
        }

        //
        // If the rewind button is being held, then we should seek backwards.
        //
        ulTime = (sUI.usTickCount - sUI.usPressTime[3]) & 0xffff;
        if((sUI.ucState & 0x08) &&
           (ulTime >= SEEK_DELAY) && ((ulTime % SEEK_ACCEL) == 0))
        {
            //
            // Indicate that the seek backwards button was pressed.
            //
            sUI.ulKeys |= BUTTON_REW;
        }

        //
        // See if the next track button has been pressed.
        //
        if(ulButtons & 0x10)
        {
            //
            // Indicate that the next track button was pressed.
            //
            sUI.ulKeys |= BUTTON_NEXT;
        }

        //
        // If the fast forward button is being held, then we should seek
        // forward.
        //
        ulTime = (sUI.usTickCount - sUI.usPressTime[4]) & 0xffff;
        if((sUI.ucState & 0x10) &&
           (ulTime >= SEEK_DELAY) && ((ulTime % SEEK_ACCEL) == 0))
        {
            //
            // Indicate that the seek forwards button was pressed.
            //
            sUI.ulKeys |= BUTTON_FFWD;
        }
    }

    //
    // Otherwise, see if we are stopped.
    //
    else if((sUI.ucMode & 0x1f) == MODE_STOP)
    {
        //
        // See if the previous track button has been pressed or is being held.
        //
        ulTime = (sUI.usTickCount - sUI.usPressTime[3]) & 0xffff;
        if((sUI.ucState & 0x08) &&
           ((ulButtons & 0x080000) ||
            ((ulTime >= SEEK_DELAY) && ((ulTime % TRACK_ACCEL) == 0))))
        {
            //
            // Indicate that the previous track button was pressed.
            //
            sUI.ulKeys |= BUTTON_PREV;
        }

        //
        // See if the next track button has been pressed or is being held.
        //
        ulTime = (sUI.usTickCount - sUI.usPressTime[4]) & 0xffff;
        if((sUI.ucState & 0x10) &&
           ((ulButtons & 0x100000) ||
            ((ulTime >= SEEK_DELAY) && ((ulTime % TRACK_ACCEL) == 0))))
        {
            //
            // Indicate that the next track button was pressed.
            //
            sUI.ulKeys |= BUTTON_NEXT;
        }
    }

    //
    // If the volume down button was pressed, or it is being held, then we
    // should turn down the volume.
    //
    ulTime = (sUI.usTickCount - sUI.usPressTime[5]) & 0xffff;
    if((sUI.ucState & 0x20) &&
       ((ulButtons & 0x200000) ||
        ((ulTime >= VOLUME_DELAY) && ((ulTime % VOLUME_ACCEL) == 0))))
    {
        //
        // If the volume is not already at zero, then turn it down.
        //
        if(sUI.ulFlags & FLAG_VOLUME_MASK)
        {
            unsigned long ulVolume;

            //
            // Get the current volume setting and decrement it by one.
            //
            ulVolume = ((sUI.ulFlags & FLAG_VOLUME_MASK) >>
                        FLAG_VOLUME_SHIFT) - 1;

            //
            // Save the new volume setting.
            //
            sUI.ulFlags = (sUI.ulFlags & ~FLAG_VOLUME_MASK) |
                          (ulVolume << FLAG_VOLUME_SHIFT);

            //
            // Update the actual volume control.
            //
            OutputSetVolume(ulVolume);
        }
    }

    //
    // See if the bass button has been pressed.
    //
    if(ulButtons & 0x40)
    {
        //
        // Change the state of the bass boost.
        //
        sUI.ulFlags ^= FLAG_BASS_BOOST;

        //
        // Set the new state of the bass boost.
        //
        OutputSetBass(sUI.ulFlags & FLAG_BASS_BOOST ? 12 : 0);
    }

    //
    // See if the record button has been pressed.
    //
    ulTime = (sUI.usTickCount - sUI.usPressTime[6]) & 0xffff;
    if((sUI.ucState & 0x40) && (ulTime >= RECORD_DELAY))
    {
        //
        // Indicate that the record button was pressed.
        //
        sUI.ulKeys |= BUTTON_RECORD;
    }

    //
    // If the volume up button was pressed, or it is being held, then we should
    // turn up the volume.
    //
    ulTime = (sUI.usTickCount - sUI.usPressTime[7]) & 0xffff;
    if((sUI.ucState & 0x80) &&
       ((ulButtons & 0x800000) ||
        ((ulTime >= VOLUME_DELAY) && ((ulTime % VOLUME_ACCEL) == 0))))
    {
        //
        // If the volume is not already at it's maximum, then turn it up.
        //
        if(((sUI.ulFlags & FLAG_VOLUME_MASK) >> FLAG_VOLUME_SHIFT) < 20)
        {
            unsigned long ulVolume;

            //
            // Get the current volume setting and increment it by one.
            //
            ulVolume = ((sUI.ulFlags & FLAG_VOLUME_MASK) >>
                        FLAG_VOLUME_SHIFT) + 1;

            //
            // Save the new volume setting.
            //
            sUI.ulFlags = (sUI.ulFlags & ~FLAG_VOLUME_MASK) |
                          (ulVolume << FLAG_VOLUME_SHIFT);

            //
            // Update the actual volume control.
            //
            OutputSetVolume(ulVolume);
        }
    }

    //
    // We need to move the target time so that we do not get an automatic
    // power off.
    //
    sUI.usTargetTime = sUI.usTickCount + SLEEP_DELAY;
}

//****************************************************************************
//
// UIISR is the user interface routine which is called approximately 50 times
// per second to handle the periodic needs of the user interface.
//
//****************************************************************************
void
UIISR(void)
{
    unsigned long ulButtons;

    //
    // Increment the number of timer ticks which have occurred.
    //
    sUI.usTickCount++;

    //
    // If the tick count has reached the target time, see what we need to do.
    //
    if((sUI.usTickCount == sUI.usTargetTime) &&
       ((sUI.ucMode == MODE_STOP) || (sUI.ucMode == MODE_PAUSE)))
    {
        //
        // Fake a press of the power button.
        //
        sUI.ulKeys |= BUTTON_POWER;
    }

    //
    // Determine which buttons are pressed and which have been released.
    //
    ulButtons = ScanButtons();

    //
    // See if any buttons are being pressed.
    //
    if(sUI.ucState || ulButtons)
    {
        //
        // See if more than one button is pressed.
        //
        if((sUI.ucState != 0x00) && (sUI.ucState != 0x01) &&
           (sUI.ucState != 0x02) && (sUI.ucState != 0x04) &&
           (sUI.ucState != 0x08) && (sUI.ucState != 0x10) &&
           (sUI.ucState != 0x20) && (sUI.ucState != 0x40) &&
           (sUI.ucState != 0x80))
        {
            //
            // More than one button is currently pressed, so wait for all
            // buttons to be released before we handle any more button presses.
            //
            sUI.ulFlags |= FLAG_WAIT_FOR_ALL_UP;
        }

        //
        // See if we are supposed to wait for all buttons to be released.
        //
        if(!(sUI.ulFlags & FLAG_WAIT_FOR_ALL_UP))
        {
            //
            // Handle the buttons.
            //
            HandleButtons(ulButtons);
        }
    }

    //
    // If there are no buttons pressed, then clear the flag indicating that we
    // should wait for all buttons to be released.
    //
    else
    {
        sUI.ulFlags &= ~FLAG_WAIT_FOR_ALL_UP;
    }
}
#endif

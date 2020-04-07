//****************************************************************************
//
// UISER.C - Functions for handling the user interface for the player, using
//           the serial port as the display.
//
// Copyright (c) 1999,2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include "globals.h"
#include "../hwport.h"
#include "../hwdefs.h"

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
//     SEEK1_ACCEL         The amount of time the fast forward and rewind
//                         buttons must be pressed to start the first notch of
//                         accelerated seeking within the track when the
//                         player is playing or paused.
//     SEEK2_ACCEL         The amount of time the fast forward and rewind
//                         buttons must be pressed to start the second notch
//                         of accelerated seeking within the track when the
//                         player is playing or paused.
//     SEEK3_ACCEL         The amount of time the fast forward and rewind
//                         buttons must be pressed to start the third notch of
//                         accelerated seeking within the track when the
//                         player is playing or paused.
//     TRACK_ACCEL         The amount of time the fast forward and rewind
//                         buttons must be pressed to continue skipping tracks
//                         when the player is stopped.  Should be a power of
//                         two.
//     MENU_DELAY          The amount of time the menu arrow buttons must be
//                         pressed to start scrolling in the given direction.
//     MENU_LR_ACCEL       The amount of time the left and right menu arrow
//                         buttons must be pressed to continue scrolling in
//                         the given direction.  Should be a power of two.
//     MENU_UD_ACCEL       The amount of time the up and down menu arrow
//                         buttons must be pressed to continue adjusting in
//                         the given direction.  Should be a power of two.
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
#define POWER_DELAY                             75      // 1.5 seconds
#define RECORD_DELAY                            50      // 1 second
#define SEEK_DELAY                              25      // .5 seconds
#define SEEK_ACCEL                              8       // .16 seconds
#define SEEK1_ACCEL                             150     // 3 seconds
#define SEEK2_ACCEL                             300     // 6 seconds
#define SEEK3_ACCEL                             450     // 9 seconds
#define TRACK_ACCEL                             8       // .16 seconds
#define MENU_DELAY                              25      // .5 seconds
#define MENU_LR_ACCEL                           8       // .16 seconds
#define MENU_UD_ACCEL                           4       // .08 seconds

//****************************************************************************
//
// The number of clock ticks (which runs at 50Hz) that each button must be
// pressed for the hold function to occur instead of the press function (i.e.
// skipping forward within a song instead of advancing to the next song).
//
//****************************************************************************
static const unsigned char ucHoldTimes[6] = { POWER_DELAY,
                                              RECORD_DELAY,
                                              MENU_DELAY,
                                              MENU_DELAY,
                                              MENU_DELAY,
                                              MENU_DELAY };

//****************************************************************************
//
// The various bit fields in the flags member of the user interface persistent
// state.
//
//****************************************************************************
#define FLAG_VOLUME_MASK                        0x0000001f
#define FLAG_BASS_BOOST_MASK                    0x000001e0
#define FLAG_TREBLE_BOOST_MASK                  0x00001e00
#define FLAG_WAIT_FOR_ALL_UP                    0x00002000
#define FLAG_SLEEP_MASK                         0x0000c000
#define FLAG_SLEEP_1                            0x00000000
#define FLAG_SLEEP_2                            0x00004000
#define FLAG_SLEEP_3                            0x00008000
#define FLAG_SLEEP_4                            0x0000c000
#define FLAG_MENU_ITEM_MASK                     0x001f0000
#define FLAG_UPDATE_MENU                        0x00200000
#define FLAG_VOLUME_SHIFT                       0
#define FLAG_BASS_BOOST_SHIFT                   5
#define FLAG_TREBLE_BOOST_SHIFT                 9
#define FLAG_SLEEP_SHIFT                        14
#define FLAG_MENU_ITEM_SHIFT                    16

//****************************************************************************
//
// The various bit fields in the ulSRSState member of the user interface
// persistent state.
//
//****************************************************************************
#define FLAG_SRS_WIDTH_MASK                     0x0000001f
#define FLAG_SRS_TRU_BASS_MASK                  0x000003e0
#define FLAG_SRS_WIDTH_SHIFT                    0
#define FLAG_SRS_TRU_BASS_SHIFT                 5

//****************************************************************************
//
// The various bit fields in the ulQSoundState member of the user interface
// persistent state.
//
//****************************************************************************
#define FLAG_QSOUND_WIDTH_MASK                  0x0000001f
#define FLAG_QSOUND_MODE_MASK                   0x00000020
#define FLAG_QSOUND_WIDTH_SHIFT                 0
#define FLAG_QSOUND_MODE_SHIFT                  5

//****************************************************************************
//
// The various bit fields in the ulSpatializerState member of the user
// interface persistent state.
//
//****************************************************************************
#define FLAG_SPATIALIZER_BASS_MASK              0x0000001f
#define FLAG_SPATIALIZER_VBASS_MASK             0x000003e0
#define FLAG_SPATIALIZER_GAIN_MASK              0x00007c00
#define FLAG_SPATIALIZER_BASS_SHIFT             0
#define FLAG_SPATIALIZER_VBASS_SHIFT            5
#define FLAG_SPATIALIZER_GAIN_SHIFT             10

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
    unsigned short usPressTime[6];

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
    // An unused byte to pad the next member to a word boundary.
    //
    unsigned char ucUnused;

    //
    // The name of the codec being used to decode the current file.
    //
    const unsigned short *pusCodec;

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

#ifdef SUPPORT_SRS
    //
    // The current settings of the SRS control.
    //
    unsigned long ulSRSState;
#endif

#ifdef SUPPORT_QSOUND
    //
    // The current settings of the QSound control.
    //
    unsigned long ulQSoundState;
#endif

#ifdef SUPPORT_SPATIALIZER
    //
    // The current settings of the Spatializer control.
    //
    unsigned long ulSpatializerState;
#endif
} sUI;

//****************************************************************************
//
// Prototypes for the functions used to display the various menu controls, as
// well as to update the player settings based on the value of a menu item.
//
//****************************************************************************
static unsigned long DisplayStringRing(unsigned long ulIndex);
static unsigned long DisplayValue(unsigned long ulIndex);
static unsigned long DisplayCodecs(unsigned long ulIndex);

//****************************************************************************
//
// Descriptions of the items in the menu.
//
//****************************************************************************
static const struct
{
    //
    // The name of this menu item.
    //
    char *pcName;

    //
    // The routine which prints the menu value to the serial port.
    //
    unsigned long (*pfnDisplay)(unsigned long ulIndex);

    //
    // The routine which updates the player state based on a change in the
    // menu item value.
    //
    void (*pfnUpdate)(long lValue);

    //
    // The string of values for string rings.
    //
    char *pcValues;

    //
    // The number of values in the string ring, or the number of steps in the
    // decible slider.
    //
    unsigned long ulNumValues;

    //
    // A pointer to the unsigned long which contains the flags for this menu
    // item.
    //
    unsigned long *pulFlags;

    //
    // The bitmask of the bits in the flags which should be modified when this
    // menu item is changed.
    //
    unsigned long ulFlagMask;

    //
    // The number of bits to shift to get to the bit field for this menu item.
    //
    unsigned long ulFlagShift;
} sMenuItems[] =
{
    //
    // The "Seek" menu item.
    //
    { "Seek", 0, 0, 0, 0, 0, 0, 0 },

    //
    // The "Volume" menu item.
    //
#if defined(ADDR_CS43L43) || defined(SOFTWARE_VOLUME_CONTROL)
    { "Volume", DisplayValue, OutputSetVolume, 0, 21, &sUI.ulFlags,
      FLAG_VOLUME_MASK, FLAG_VOLUME_SHIFT },
#endif

    //
    // The "Repeat" menu item.
    //
    { "Repeat", DisplayStringRing, 0, "Off\0One\0All", 3,
      (unsigned long *)&ulSystemFlags, SYSFLAG_REPEAT_MASK,
      SYSFLAG_REPEAT_SHIFT },

#ifdef SUPPORT_SRS
#if defined(SUPPORT_WOW_FULL) || defined(SUPPORT_WOW_MINIMAL)
    //
    // The "WOW" menu item.
    //
    { "WOW", DisplayValue, OutputSetSRSWidth, 0, 21, &sUI.ulSRSState,
      FLAG_SRS_WIDTH_MASK, FLAG_SRS_WIDTH_SHIFT },
#endif

#if defined(SUPPORT_WOW_FULL) || defined(SUPPORT_WOW_MINIMAL) || \
    defined(SUPPORT_TRU_BASS)
    //
    // The "TruBass" menu item.
    //
    { "TruBass", DisplayValue, OutputSetSRSTruBass, 0, 21, &sUI.ulSRSState,
      FLAG_SRS_TRU_BASS_MASK, FLAG_SRS_TRU_BASS_SHIFT },
#endif

#ifdef SUPPORT_SRS_HEADPHONE
    //
    // The "SRS" menu item.
    //
    { "SRS", DisplayValue, OutputSetSRSWidth, 0, 21, &sUI.ulSRSState,
      FLAG_SRS_WIDTH_MASK, FLAG_SRS_WIDTH_SHIFT },
#endif
#endif

#ifdef SUPPORT_QSOUND
    //
    // The "Q2X" menu item.
    //
    { "Q2X", DisplayValue, OutputSetQSoundWidth, 0, 21, &sUI.ulQSoundState,
      FLAG_QSOUND_WIDTH_MASK, FLAG_QSOUND_WIDTH_SHIFT },

    //
    // The "Q2X Mode" menu item.
    //
    { "Q2X Mode", DisplayStringRing, OutputSetQSoundMode, "0\0001", 2,
      &sUI.ulQSoundState, FLAG_QSOUND_MODE_MASK, FLAG_QSOUND_MODE_SHIFT },
#endif

#ifdef SUPPORT_SPATIALIZER
    //
    // The "Bass" menu item.
    //
    { "Bass", DisplayValue, OutputSetSpatializerBass, 0, 21,
      &sUI.ulSpatializerState, FLAG_SPATIALIZER_BASS_MASK,
      FLAG_SPATIALIZER_BASS_SHIFT },

    //
    // The "Vi.B.E." menu item.
    //
    { "Vi.B.E.", DisplayValue, OutputSetSpatializerVirtualBass, 0, 21,
      &sUI.ulSpatializerState, FLAG_SPATIALIZER_VBASS_MASK,
      FLAG_SPATIALIZER_VBASS_SHIFT },

    //
    // The "Gain" menu item.
    //
    { "Gain", DisplayValue, OutputSetSpatializerGain, 0, 21,
      &sUI.ulSpatializerState, FLAG_SPATIALIZER_GAIN_MASK,
      FLAG_SPATIALIZER_GAIN_SHIFT },
#endif

    //
    // The "Treble" menu item.
    //
#if defined(ADDR_CS43L43) || defined(SOFTWARE_TONE_CONTROL)
    { "Treble", DisplayValue, OutputSetTreble, 0, 13, &sUI.ulFlags,
      FLAG_TREBLE_BOOST_MASK, FLAG_TREBLE_BOOST_SHIFT },
#endif

    //
    // The "Bass" menu item.
    //
#if defined(ADDR_CS43L43) || defined(SOFTWARE_TONE_CONTROL)
    { "Bass", DisplayValue, OutputSetBass, 0, 13, &sUI.ulFlags,
      FLAG_BASS_BOOST_MASK, FLAG_BASS_BOOST_SHIFT },
#endif

    //
    // The "Sleep" menu item.
    //
    { "Sleep", DisplayStringRing, 0, "1 min\0002 min\0003 min\0004 min",
      4, &sUI.ulFlags, FLAG_SLEEP_MASK, FLAG_SLEEP_SHIFT },

    //
    // The "Version" menu item.
    //
    { "Release 16", 0, 0, 0, 0, 0, 0 },

    //
    // The "Codecs" menu item.
    //
    { "Codecs ", DisplayCodecs, 0, 0, 1, 0, 0 },
};

//****************************************************************************
//
// Initializes the serial port and configures it for 115,200, 8-N-1.
//
//****************************************************************************
static void
SerialInit(void)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;

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
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;

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
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;

    //
    // Wait until the serial port transmit is not busy.
    //
    while(pulPtr[HwStatus >> 2] & HwStatusUartTxBusy)
    {
    }
}

//****************************************************************************
//
// Displays the current value of a string ring on the serial port.
//
//****************************************************************************
static unsigned long
DisplayStringRing(unsigned long ulIndex)
{
    char *pcValue;
    unsigned long ulIdx;

    //
    // Get a pointer to the string ring values.
    //
    pcValue = sMenuItems[ulIndex].pcValues;

    //
    // Get the current value of this menu item.
    //
    ulIdx = (*(sMenuItems[ulIndex].pulFlags) &
             sMenuItems[ulIndex].ulFlagMask) >>
            sMenuItems[ulIndex].ulFlagShift;

    //
    // Skip strings in the string ring until we've reached the current value
    // of this menu item.
    //
    while(ulIdx--)
    {
        //
        // Skip to the end of this string.
        //
        while(*pcValue)
        {
            pcValue++;
        }

        //
        // Skip past the trailing NULL character to get to the beginning of the
        // next string in the ring.
        //
        pcValue++;
    }

    //
    // Display the current value of the string ring, preceeded by a space.
    //
    SerialSendChar(' ');
    for(ulIdx = 0; ulIdx < 16; ulIdx++)
    {
        if(pcValue[ulIdx])
        {
            SerialSendChar(pcValue[ulIdx]);
        }
        else
        {
            break;
        }
    }

    //
    // Return the number of characters displayed.
    //
    return(ulIdx + 1);
}

//****************************************************************************
//
// Displays the current value of a menu item on the serial port.
//
//****************************************************************************
static unsigned long
DisplayValue(unsigned long ulIndex)
{
    int iValue;

    //
    // Get the current value of the slider.
    //
    iValue = (*(sMenuItems[ulIndex].pulFlags) &
              sMenuItems[ulIndex].ulFlagMask) >>
             sMenuItems[ulIndex].ulFlagShift;

    //
    // Display the current value, preceeded by a space.
    //
    SerialSendChar(' ');
    if(iValue > 9)
    {
        SerialSendChar('0' + (iValue / 10));
    }
    else
    {
        SerialSendChar(' ');
    }
    SerialSendChar('0' + (iValue % 10));

    //
    // We displayed three characters.
    //
    return(3);
}

//****************************************************************************
//
// Displays a list of the available codecs within the main body of the menu
// item display.
//
//****************************************************************************
static unsigned long
DisplayCodecs(unsigned long ulIndex)
{
    const unsigned short *pusName;
    char cMsg[40];
    int i, j;

    //
    // Loop through up to eight codecs.
    //
    for(i = 0, j = 0; i < 8; i++)
    {
        //
        // Get the name of this codec.  If we can not get it's name, then we've
        // gone through all the available codecs and should stop looking.
        //
        if(CodecGetNameByIndex(i, &pusName) == 0)
        {
            break;
        }

        //
        // Copy the short name of this codec to the message buffer.
        //
        cMsg[j++] = pusName[0];
        cMsg[j++] = pusName[1];
        cMsg[j++] = pusName[2];
        cMsg[j++] = pusName[3];

        //
        // Remove any trailing spaces from the short name of the codec.
        //
        while(cMsg[j - 1] == ' ')
        {
            j--;
        }

        //
        // Add a comma to separate this codec from the next.
        //
        cMsg[j++] = ',';
    }

    //
    // Replace the last comma with a NULL to terminate the string.
    //
    cMsg[j - 1] = '\0';

    //
    // Display the list of codec names.
    //
    for(i = 0; i < 16; i++)
    {
        if(cMsg[i])
        {
            SerialSendChar(cMsg[i]);
        }
        else
        {
            break;
        }
    }

    //
    // Return the number of characters displayed.
    //
    return(i);
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
    // If we contain one of the audio processing libraries, then inform the
    // user of that fact now.
    //
#ifdef SUPPORT_SRS
#ifdef SUPPORT_WOW_FULL
    SerialSendString("\r\nSRS WOW\r\n");
#endif
#ifdef SUPPORT_WOW_MINIMAL
    SerialSendString("\r\nSRS WOW Minimal\r\n");
#endif
#ifdef SUPPORT_TRU_BASS
    SerialSendString("\r\nSRS TruBass\r\n");
#endif
#ifdef SUPPORT_SRS_HEADPHONE
    SerialSendString("\r\nSRS Headphone\r\n");
#endif
#endif
#ifdef SUPPORT_QSOUND
    SerialSendString("\r\nQSound Q2X\r\n");
#endif
#ifdef SUPPORT_SPATIALIZER
    SerialSendString("\r\nSpatializer\r\n");
#endif

    //
    // Initialize the user interface flags.  Set the volume to mid-scale and
    // turn off the treble and bass boost.
    //
    sUI.ulFlags = (15 << FLAG_VOLUME_SHIFT) | FLAG_UPDATE_MENU | FLAG_SLEEP_4;

    //
    // Initialize the Spatializer flags.
    //
#ifdef SUPPORT_SPATIALIZER
    sUI.ulSpatializerState = ((unsigned long)20 <<
                              FLAG_SPATIALIZER_GAIN_SHIFT);
    OutputSetSpatializerGain(20);
#endif

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
    OutputSetVolume(15);

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
    ulMS /= 1000;
    ulMinutes = ulMS / 60;
    ulMS -= ulMinutes * 60;

    //
    // If there are more than 59 minutes and 59 seconds in the time, then
    // convert the time to hours and minutes.
    //
    if(ulMinutes >= 60)
    {
        ulMS = ulMinutes % 60;
        ulMinutes /= 60;
    }

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
            SerialSendString("                                        ");
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
            // Indicate that we need to update the menu.
            //
            sUI.ulFlags |= FLAG_UPDATE_MENU;

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
            if(sUI.pusCodec)
            {
                SerialSendChar(sUI.pusCodec[0]);
                SerialSendChar(sUI.pusCodec[1]);
                SerialSendChar(sUI.pusCodec[2]);
                SerialSendChar(sUI.pusCodec[3]);
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
            SerialSendChar(sUI.pusCodec[0]);
            SerialSendChar(sUI.pusCodec[1]);
            SerialSendChar(sUI.pusCodec[2]);
            SerialSendChar(sUI.pusCodec[3]);

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
            SerialSendChar(sUI.pusCodec[0]);
            SerialSendChar(sUI.pusCodec[1]);
            SerialSendChar(sUI.pusCodec[2]);
            SerialSendChar(sUI.pusCodec[3]);

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
            SerialSendChar(sUI.pusCodec[0]);
            SerialSendChar(sUI.pusCodec[1]);
            SerialSendChar(sUI.pusCodec[2]);
            SerialSendChar(sUI.pusCodec[3]);

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
    // If the current mode is download or power on, then we need to enable the
    // timer.
    //
    if(((sUI.ucMode & 0x1f) == MODE_DOWNLOAD) ||
       ((sUI.ucMode & 0x1f) == MODE_POWER_ON))
    {
        //
        // Enable the timer interrupt.
        //
        EnableTimer();
    }

    //
    // Remember the current mode.
    //
    sUI.ucMode = ulMode;

    //
    // Some modes we handle directly.  See if we are powering off.
    //
    if(ulMode == MODE_POWER_OFF)
    {
        //
        // Disable the timer interrupt.
        //
        DisableTimer();
    }

    //
    // See if we are powering on.
    //
    else if(ulMode == MODE_POWER_ON)
    {
        //
        // Since we were just powered on, we want to wait until all the buttons
        // are released before we process any buttons.
        //
        sUI.ulFlags |= FLAG_WAIT_FOR_ALL_UP;

        //
        // Clear any keys that might have already been pressed.
        //
        sUI.ulKeys = 0;
    }

    //
    // See if we are starting a download.
    //
    else if(ulMode == MODE_DOWNLOAD)
    {
        //
        // Disable the timer interrupt.
        //
        DisableTimer();
    }

    //
    // We need to move the target time so that we do not get an automatic
    // power off.
    //
    sUI.usTargetTime = sUI.usTickCount +
                       (3000 * (((sUI.ulFlags & FLAG_SLEEP_MASK) >>
                                 FLAG_SLEEP_SHIFT) + 1));

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
UIFileLoaded(unsigned short *pusFileName, unsigned long ulTrack)
{
    //
    // Get the name of the current codec.
    //
    CodecGetName(&sUI.pusCodec);

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
// UINoTracks is called when there are no tracks on the device.  It is up to
// the UI to clear any variables and/or indicators which are related to a
// currently opened file.
//
//****************************************************************************
void
UINoTracks(void)
{
    //
    // Clear the variables which relate to the currently loaded file.
    //
    sUI.pusCodec = 0;
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
    ulT1 = sUI.ulCurrentMS / 1000;
    sUI.ulCurrentMS = ulMS;
    ulT2 = sUI.ulCurrentMS / 1000;

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
    unsigned long ulRet, ulIdx, ulLoop;

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
    // See if we need to update the menu on the serial port.
    //
    if(sUI.ulFlags & FLAG_UPDATE_MENU)
    {
        //
        // Get the current menu item.
        //
        ulIdx = (sUI.ulFlags & FLAG_MENU_ITEM_MASK) >> FLAG_MENU_ITEM_SHIFT;

        //
        // First, clear the previous menu text.
        //
        SerialSendString("                      ");
        SerialSendString("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");

        //
        // Print the current menu item.
        //
        for(ulLoop = 0; ulLoop < 16; ulLoop++)
        {
            if(sMenuItems[ulIdx].pcName[ulLoop])
            {
                SerialSendChar(sMenuItems[ulIdx].pcName[ulLoop]);
            }
            else
            {
                break;
            }
        }

        //
        // If there is a display function for this menu item, then call it now.
        //
        if(sMenuItems[ulIdx].pfnDisplay)
        {
            ulLoop += (sMenuItems[ulIdx].pfnDisplay)(ulIdx);
        }

        //
        // Skip back to the original cursor position.
        //
        ulLoop += 2;
        while(ulLoop--)
        {
            SerialSendChar('\b');
        }
            
        //
        // Indicate that we no longer need to update the menu.
        //
        sUI.ulFlags &= ~FLAG_UPDATE_MENU;
    }

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
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;
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
    for(ulIdx = 0; ulIdx < 6; ulIdx++)
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
    unsigned long ulIndex, ulTime;

    //
    // See if the play button has been pressed.
    //
    if(ulButtons & 0x01)
    {
        //
        // Indicate that the play button was pressed.  This maps to both the
        // play and pause virtual keys.
        //
        sUI.ulKeys |= BUTTON_PLAY | BUTTON_PAUSE;
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
    // See if the stop button has been pressed.
    //
    if(ulButtons & 0x02)
    {
        //
        // Indicate that the stop button was pressed.
        //
        sUI.ulKeys |= BUTTON_STOP;
    }

    //
    // See if the record button has been pressed.
    //
#ifdef REQUIRE_RECORD
    ulTime = (sUI.usTickCount - sUI.usPressTime[1]) & 0xffff;
    if((sUI.ucState & 0x02) && (ulTime >= RECORD_DELAY))
    {
        //
        // Indicate that the record button was pressed.
        //
        sUI.ulKeys |= BUTTON_RECORD;
    }
#endif

    //
    // Get the current menu index.
    //
    ulIndex = (sUI.ulFlags & FLAG_MENU_ITEM_MASK) >> FLAG_MENU_ITEM_SHIFT;

    //
    // See if the menu left button has been pressed.
    //
    ulTime = (sUI.usTickCount - sUI.usPressTime[2]) & 0xffff;
    if((sUI.ucState & 0x04) &&
       ((ulButtons & 0x040000) ||
        ((ulTime >= MENU_DELAY) && ((ulTime % MENU_LR_ACCEL) == 0))))
    {
        //
        // If the menu index is not already at zero, then decrement it.
        //
        if(ulIndex)
        {
            //
            // Decrement the menu index.
            //
            ulIndex--;

            //
            // Save the new menu item index.
            //
            sUI.ulFlags &= ~FLAG_MENU_ITEM_MASK;
            sUI.ulFlags |= ulIndex << FLAG_MENU_ITEM_SHIFT;

            //
            // Indicate that the menu needs to be updated.
            //
            sUI.ulFlags |= FLAG_UPDATE_MENU;
        }
    }

    //
    // See if the menu right button has been pressed.
    //
    ulTime = (sUI.usTickCount - sUI.usPressTime[3]) & 0xffff;
    if((sUI.ucState & 0x08) &&
       ((ulButtons & 0x080000) ||
        ((ulTime >= MENU_DELAY) && ((ulTime % MENU_LR_ACCEL) == 0))))
    {
        //
        // If the menu index is not already at the end, then increment it.
        //
        if((ulIndex + 1) != (sizeof(sMenuItems) / sizeof(sMenuItems[0])))
        {
            //
            // Increment the menu index.
            //
            ulIndex++;

            //
            // Save the new menu item index.
            //
            sUI.ulFlags &= ~FLAG_MENU_ITEM_MASK;
            sUI.ulFlags |= ulIndex << FLAG_MENU_ITEM_SHIFT;

            //
            // Indicate that the menu needs to be updated.
            //
            sUI.ulFlags |= FLAG_UPDATE_MENU;
        }
    }

    //
    // The next two buttons have special handling based on the current menu
    // index.  See if we are in seek mode.
    //
    if(ulIndex == 0)
    {
        //
        // The handling of the fast forward and rewind buttons are dependent
        // upon the current mode of the player.  See if we are plaing or
        // paused.
        //
        if(((sUI.ucMode & 0x1f) == MODE_PLAY) ||
           ((sUI.ucMode & 0x1f) == MODE_PAUSE))
        {
            //
            // See if the previous track button has been pressed.
            //
            if(ulButtons & 0x10)
            {
                //
                // Indicate that the previous track button was pressed.
                //
                sUI.ulKeys |= BUTTON_PREV;
            }

            //
            // If the rewind button is being held, then we should seek
            // backwards.
            //
            ulTime = (sUI.usTickCount - sUI.usPressTime[4]) & 0xffff;
            if((sUI.ucState & 0x10) &&
               (ulTime >= SEEK_DELAY) && ((ulTime % SEEK_ACCEL) == 0))
            {
                //
                // Indicate that the seek backwards button was pressed.
                //
                sUI.ulKeys |= BUTTON_REW;

                //
                // See if the rewind button has been held long enough to
                // accelerate the seek.
                //
                if(ulTime >= SEEK3_ACCEL)
                {
                    //
                    // The rewind button has been held long enough to bump up
                    // to the third acceleration rate.
                    //
                    sUI.ulKeys |= BUTTON_SEEK_RATE_ACC3;
                }
                else if(ulTime >= SEEK2_ACCEL)
                {
                    //
                    // The rewind button has been held long enough to bump up
                    // to the second acceleration rate.
                    //
                    sUI.ulKeys |= BUTTON_SEEK_RATE_ACC2;
                }
                else if(ulTime >= SEEK1_ACCEL)
                {
                    //
                    // The rewind button has been held long enough to bump up
                    // to the first acceleration rate.
                    //
                    sUI.ulKeys |= BUTTON_SEEK_RATE_ACC1;
                }
            }

            //
            // See if the next track button has been pressed.
            //
            if(ulButtons & 0x20)
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
            ulTime = (sUI.usTickCount - sUI.usPressTime[5]) & 0xffff;
            if((sUI.ucState & 0x20) &&
               (ulTime >= SEEK_DELAY) && ((ulTime % SEEK_ACCEL) == 0))
            {
                //
                // Indicate that the seek forwards button was pressed.
                //
                sUI.ulKeys |= BUTTON_FFWD;

                //
                // See if the fast foward button has been held long enough to
                // accelerate the seek.
                //
                if(ulTime >= SEEK3_ACCEL)
                {
                    //
                    // The fast forward button has been held long enough to
                    // bump up to the third acceleration rate.
                    //
                    sUI.ulKeys |= BUTTON_SEEK_RATE_ACC3;
                }
                else if(ulTime >= SEEK2_ACCEL)
                {
                    //
                    // The fast forward button has been held long enough to
                    // bump up to the second acceleration rate.
                    //
                    sUI.ulKeys |= BUTTON_SEEK_RATE_ACC2;
                }
                else if(ulTime >= SEEK1_ACCEL)
                {
                    //
                    // The fast forward button has been held long enough to
                    // bump up to the first acceleration rate.
                    //
                    sUI.ulKeys |= BUTTON_SEEK_RATE_ACC1;
                }
            }
        }

        //
        // Otherwise, see if we are stopped.
        //
        else if((sUI.ucMode & 0x1f) == MODE_STOP)
        {
            //
            // See if the previous track button has been pressed or is being
            // held.
            //
            ulTime = (sUI.usTickCount - sUI.usPressTime[4]) & 0xffff;
            if((sUI.ucState & 0x10) &&
               ((ulButtons & 0x100000) ||
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
            ulTime = (sUI.usTickCount - sUI.usPressTime[5]) & 0xffff;
            if((sUI.ucState & 0x20) &&
               ((ulButtons & 0x200000) ||
                ((ulTime >= SEEK_DELAY) && ((ulTime % TRACK_ACCEL) == 0))))
            {
                //
                // Indicate that the next track button was pressed.
                //
                sUI.ulKeys |= BUTTON_NEXT;
            }
        }
    }

    //
    // Otherwise, we are adjusting a menu item value.
    //
    else
    {
        //
        // See if the adjust down button was pressed.
        //
        ulTime = (sUI.usTickCount - sUI.usPressTime[4]) & 0xffff;
        if((sUI.ucState & 0x10) &&
           ((ulButtons & 0x100000) ||
            ((ulTime >= MENU_DELAY) && ((ulTime % MENU_UD_ACCEL) == 0))))
        {
            unsigned long ulValue;

            //
            // Get the current value of this menu item.
            //
            ulValue = (*(sMenuItems[ulIndex].pulFlags) &
                       sMenuItems[ulIndex].ulFlagMask) >>
                      sMenuItems[ulIndex].ulFlagShift;

            //
            // If the current value is not zero, then we can decrement it.
            //
            if(ulValue > 0)
            {
                //
                // Decrement the current value.
                //
                ulValue--;

                //
                // Save the new value.
                //
                *(sMenuItems[ulIndex].pulFlags) &=
                    ~sMenuItems[ulIndex].ulFlagMask;
                *(sMenuItems[ulIndex].pulFlags) |=
                    ulValue << sMenuItems[ulIndex].ulFlagShift;

                //
                // See if the menu item has a routine for effecting the change
                // in other parts of the player.
                //
                if(sMenuItems[ulIndex].pfnUpdate)
                {
                    //
                    // Reflect the new menu item value in the rest of the
                    // player.
                    //
                    (sMenuItems[ulIndex].pfnUpdate)(ulValue);
                }

                //
                // Indicate that we need to update the menu.
                //
                sUI.ulFlags |= FLAG_UPDATE_MENU;
            }
        }

        //
        // See if the adjust up button was pressed.
        //
        ulTime = (sUI.usTickCount - sUI.usPressTime[5]) & 0xffff;
        if((sUI.ucState & 0x20) &&
           ((ulButtons & 0x200000) ||
            ((ulTime >= MENU_DELAY) && ((ulTime % MENU_UD_ACCEL) == 0))))
        {
            unsigned long ulValue;

            //
            // Get the current value of this menu item.
            //
            ulValue = (*(sMenuItems[ulIndex].pulFlags) &
                       sMenuItems[ulIndex].ulFlagMask) >>
                      sMenuItems[ulIndex].ulFlagShift;

            //
            // If the current value is not at the maximum, then we can
            // increment it.
            //
            if(ulValue < (sMenuItems[ulIndex].ulNumValues - 1))
            {
                //
                // Increment the current value.
                //
                ulValue++;

                //
                // Save the new value.
                //
                *(sMenuItems[ulIndex].pulFlags) &=
                    ~sMenuItems[ulIndex].ulFlagMask;
                *(sMenuItems[ulIndex].pulFlags) |=
                    ulValue << sMenuItems[ulIndex].ulFlagShift;

                //
                // See if the menu item has a routine for effecting the change
                // in other parts of the player.
                //
                if(sMenuItems[ulIndex].pfnUpdate)
                {
                    //
                    // Reflect the new menu item value in the rest of the
                    // player.
                    //
                    (sMenuItems[ulIndex].pfnUpdate)(ulValue);
                }

                //
                // Indicate that we need to update the menu.
                //
                sUI.ulFlags |= FLAG_UPDATE_MENU;
            }
        }
    }

    //
    // We need to move the target time so that we do not get an automatic
    // power off.
    //
    sUI.usTargetTime = sUI.usTickCount +
                       (3000 * (((sUI.ulFlags & FLAG_SLEEP_MASK) >>
                                 FLAG_SLEEP_SHIFT) + 1));
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

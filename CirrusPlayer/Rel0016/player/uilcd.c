//****************************************************************************
//
// UILCD.C - Functions for handling the user interface for the player, using a
//           128x33 LCD controlled by a Hitachi HD66410 as the display.
//
// Copyright (c) 1999,2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include "globals.h"
#include "../hwport.h"
#include "../hwdefs.h"
#include "time.h"

//****************************************************************************
//
// We only include the remainder of this file if the LCD is used for our user
// interface.
//
//****************************************************************************
#ifdef UI_IS_LCD

//****************************************************************************
//
// The indices of the registers in the HD66410.
//
//****************************************************************************
#define LCD_REG_CONTROL1                        0
#define LCD_REG_CONTROL2                        1
#define LCD_REG_X                               2
#define LCD_REG_Y                               3
#define LCD_REG_DATA                            4
#define LCD_REG_START                           5
#define LCD_REG_BLINK1                          6
#define LCD_REG_BLINK2                          7
#define LCD_REG_BLINK_START                     8
#define LCD_REG_BLINK_END                       9
#define LCD_REG_ANNUNCIATOR1                    16
#define LCD_REG_ANNUNCIATOR2                    17
#define LCD_REG_ANNUNCIATOR3                    18
#define LCD_REG_ANNUNCIATOR4                    19
#define LCD_REG_ANNUNCIATOR5                    20
#define LCD_REG_ANNUNCIATOR6                    21
#define LCD_REG_ANNUNCIATOR7                    22
#define LCD_REG_ANNUNCIATOR8                    23
#define LCD_REG_ANNUNCIATOR9                    24
#define LCD_REG_ANNUNCIATOR_BLINK1              25
#define LCD_REG_ANNUNCIATOR_BLINK2              26
#define LCD_REG_ANNUNCIATOR_BLINK3              27

//****************************************************************************
//
// The various bits in the HD66410 control register one.
//
//****************************************************************************
#define LCD_CONTROL1_ADC                        0x01
#define LCD_CONTROL1_CNF                        0x02
#define LCD_CONTROL1_IDTY                       0x04
#define LCD_CONTROL1_OSC                        0x08
#define LCD_CONTROL1_PWR                        0x10
#define LCD_CONTROL1_STBY                       0x20
#define LCD_CONTROL1_DISP                       0x40

//****************************************************************************
//
// The various bits in the HD66410 control register two.
//
//****************************************************************************
#define LCD_CONTROL2_BLK                        0x01
#define LCD_CONTROL2_INC                        0x02
#define LCD_CONTROL2_DDTY                       0x04
#define LCD_CONTROL2_RMW                        0x08

//****************************************************************************
//
// The various icons across the top of the LCD panel.
//
//****************************************************************************
#define LCD_ANNUNC3_PLAY                        0x01
#define LCD_ANNUNC3_LEFT_ARROWCIRCLE            0x02
#define LCD_ANNUNC3_RIGHT_ARROWCIRCLE           0x04
#define LCD_ANNUNC3_SINGLE                      0x08
#define LCD_ANNUNC3_RANDOM                      0x10
#define LCD_ANNUNC3_PROGRAM                     0x20
#define LCD_ANNUNC3_LEFT_SEMICIRCLE             0x40
#define LCD_ANNUNC3_ARROWS                      0x80
#define LCD_ANNUNC4_TAPE                        0x02
#define LCD_ANNUNC5_BOOK                        0x02
#define LCD_ANNUNC5_LOCK                        0x04
#define LCD_ANNUNC5_BATTERY_FILL1               0x08
#define LCD_ANNUNC5_BATTERY_FILL2               0x10
#define LCD_ANNUNC5_BATTERY                     0x20
#define LCD_ANNUNC5_BATTERY_FILL3               0x40
#define LCD_ANNUNC5_TAPEBOX                     0x80
#define LCD_ANNUNC6_RIGHT_SEMICIRCLE            0x80

//****************************************************************************
//
// Include the graphics used for the display.
//
//****************************************************************************
#include "graphics.h"

//****************************************************************************
//
// Definitions for the various delays in the user interface, specified in
// counts of the 50Hz user interface system clock.
//
//     LOGO1_DELAY         The amount of time the first logo is displayed.
//     LOGO2_DELAY         The amount of time the second logo is displayed.
//     POWER_DELAY         The amount of time the power button must be
//                         pressed to power down the player.
//     MENU_EXIT_DELAY     The amount of inactivity which will cause the
//                         player to exit menu mode.
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
//     VOLUME_DELAY        The amount of time the volume buttons must be
//                         pressed to start adjusting the volume.
//     VOLUME_ACCEL        The amount of time the volume buttons must be
//                         pressed to continue adjusting the volume.  Should
//                         be a power of two.
//     MENU_DELAY          The amount of time the menu arrow buttons must be
//                         pressed to start scrolling in the given direction.
//     MENU_LR_ACCEL       The amount of time the left and right menu arrow
//                         buttons must be pressed to continue scrolling in
//                         the given direction.  Should be a power of two.
//     MENU_UD_ACCEL       The amount of time the up and down menu arrow
//                         buttons must be pressed to continue adjusting in
//                         the given direction.  Should be a power of two.
//     BATTERY_DELAY       The amount of time between updates of the battery
//                         meter.  Should be a power of two.
//     VISUAL_DELAY        The amount of time between updates of the visual
//                         area.  Should be a power of two.
//     SCROLL_DELAY        The amount of time between updates of the scrolling
//                         track name.  Should be a power of two.
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
#define LOGO1_DELAY                             100     // 2 seconds
#define LOGO2_DELAY                             100     // 2 seconds
#define POWER_DELAY                             75      // 1.5 seconds
#define MENU_EXIT_DELAY                         250     // 5 seconds
#define RECORD_DELAY                            50      // 1 second
#define SEEK_DELAY                              25      // .5 seconds
#define SEEK_ACCEL                              8       // .16 seconds
#define SEEK1_ACCEL                             150     // 3 seconds
#define SEEK2_ACCEL                             300     // 6 seconds
#define SEEK3_ACCEL                             450     // 9 seconds
#define TRACK_ACCEL                             8       // .16 seconds
#define VOLUME_DELAY                            25      // .5 seconds
#define VOLUME_ACCEL                            4       // .08 seconds
#define MENU_DELAY                              25      // .5 seconds
#define MENU_LR_ACCEL                           8       // .16 seconds
#define MENU_UD_ACCEL                           4       // .08 seconds
#define BATTERY_DELAY                           512     // 10.24 seconds
#define VISUAL_DELAY                            4       // .08 seconds
#define SCROLL_DELAY                            16      // .32 seconds

//****************************************************************************
//
// A set of flags indicating the portions of the display that need updating.
//
//****************************************************************************
#define FLAG_UPDATE_STATIC                      0x00000001
#define FLAG_UPDATE_ICON                        0x00000002
#define FLAG_UPDATE_TRACK                       0x00000004
#define FLAG_UPDATE_TIME                        0x00000008
#define FLAG_UPDATE_VOLUME                      0x00000010
#define FLAG_UPDATE_MENU                        0x00000020
#define FLAG_UPDATE_VALUE                       0x00000040
#define FLAG_UPDATE_VISUAL                      0x00000080

//****************************************************************************
//
// The various bit fields in the ulFlags member of the user interface
// persistent state.
//
//****************************************************************************
#define FLAG_VOLUME_MASK                        0x0000001f
#define FLAG_BASS_BOOST_MASK                    0x000001e0
#define FLAG_TREBLE_BOOST_MASK                  0x00001e00
#define FLAG_UPDATE_SCREEN                      0x00002000
#define FLAG_WAIT_FOR_ALL_UP                    0x00004000
#define FLAG_BACKLIGHT_MASK                     0x00018000
#define FLAG_BACKLIGHT_OFF                      0x00000000
#define FLAG_BACKLIGHT_5                        0x00008000
#define FLAG_BACKLIGHT_10                       0x00010000
#define FLAG_BACKLIGHT_ON                       0x00018000
#define FLAG_SLEEP_MASK                         0x00060000
#define FLAG_SLEEP_1                            0x00000000
#define FLAG_SLEEP_2                            0x00020000
#define FLAG_SLEEP_3                            0x00040000
#define FLAG_SLEEP_4                            0x00060000
#define FLAG_MENU_ITEM_MASK                     0x00f80000
#define FLAG_BACKLIGHT                          0x01000000
#define FLAG_UPDATE_TRACK_NAME                  0x02000000
#define FLAG_VISUAL_MODE_MASK                   0x0c000000
#define FLAG_VISUAL_MODE_NONE                   0x00000000
#define FLAG_VISUAL_MODE_WAVEFORM               0x04000000
#define FLAG_VISUAL_MODE_SPECTRUM               0x08000000
#define FLAG_EFFECT_MASK                        0x30000000
#define FLAG_DISPLAY_STATE_MASK                 0xc0000000
#define FLAG_DISPLAY_STATE_LOGO1                0x00000000
#define FLAG_DISPLAY_STATE_LOGO2                0x40000000
#define FLAG_DISPLAY_STATE_MAIN                 0x80000000
#define FLAG_DISPLAY_STATE_MENU                 0xc0000000
#define FLAG_VOLUME_SHIFT                       0
#define FLAG_BASS_BOOST_SHIFT                   5
#define FLAG_TREBLE_BOOST_SHIFT                 9
#define FLAG_BACKLIGHT_SHIFT                    15
#define FLAG_SLEEP_SHIFT                        17
#define FLAG_MENU_ITEM_SHIFT                    19
#define FLAG_VISUAL_MODE_SHIFT                  26
#define FLAG_EFFECT_SHIFT                       28

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
// The flags in the current player mode which indicate if we are seeking
// forward or backward.
//
//****************************************************************************
#define FLAG_SEEK_FORWARD                       0x20
#define FLAG_SEEK_BACKWARD                      0x40

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
    // The most recent value written to the LCD.
    //
    unsigned short usLCDValue;

    //
    // The count of the number of clock ticks that have occurred.  This is our
    // time base for the various time-based elements of the user interface
    // (such as automatically powering down the player after 30 seconds of
    // inactivity).
    //
    unsigned short usTickCount;

    //
    // The time at which the next time delayed action is to occur.  This is
    // used to advance between the various logos displayed at startup, to
    // automatically exit from the menu, and to automatically power down the
    // player.
    //
    unsigned short usTargetTime;

    //
    // The time at which the backlight should be turned off.
    //
    unsigned short usBacklightTime;

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
    // An unused byte to pad the next member to a word boundary.
    //
    unsigned char ucUnused1;

    //
    // The current position within the current file in seconds.
    //
    unsigned long ulCurrentTime;

    //
    // The string representing the file currently opened.
    //
    unsigned short pusName[80];

    //
    // The length of the track name string.
    //
    unsigned char ucNameLength;

    //
    // The current offset into the track name string that is being displayed.
    //
    unsigned char ucNameOffset;

    //
    // The amount of time that we pause the display before scrolling starts.
    //
    unsigned char ucNamePause;

    //
    // An unused byte to pad the next member to a word boundary.
    //
    unsigned char ucUnused2;

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
// The list of items in the menu.
//
//****************************************************************************
static char * const pcMenu = "Repeat\0"
#if defined(SUPPORT_SRS) || defined(SUPPORT_QSOUND) || \
    defined(SUPPORT_SPATIALIZER)
                             "Effects\0"
#endif
#if defined(SUPPORT_WOW_FULL) || defined(SUPPORT_WOW_MINIMAL)
                             "\200 WOW\0"
#endif
#if defined(SUPPORT_WOW_FULL) || defined(SUPPORT_WOW_MINIMAL) || \
    defined(SUPPORT_TRU_BASS)
                             "\200 TruBass\0"
#endif
#ifdef SUPPORT_SRS_HEADPHONE
                             "\200 SRS\0"
#endif
#ifdef SUPPORT_QSOUND
                             "\201 Q2X\0"
                             "\201 Q2X Mode\0"
#endif
#ifdef SUPPORT_SPATIALIZER
                             "\202 Bass\0"
                             "\202 Vi.B.E.\0"
                             "\202 Gain\0"
#endif
                             "\177 Treble\0"
                             "\177 Bass\0"
                             "Backlight\0"
                             "Sleep\0"
                             "Visual\0"
                             "Clock\0"
                             "Version\0"
                             "Codecs\0"
                             "Geek Info\0";

//****************************************************************************
//
// Prototypes for the functions used to display the various menu controls, as
// well as to update the player settings based on the value of a menu item.
//
//****************************************************************************
static void DisplayStringRing(unsigned long ulIndex);
static void DisplayDecibleSlider(unsigned long ulIndex);
#if defined(SUPPORT_SRS) || defined(SUPPORT_QSOUND) || \
    defined(SUPPORT_SPATIALIZER)
static void DisplayValueSlider(unsigned long ulIndex);
#endif
static void DisplayClock(unsigned long ulIndex);
static void DisplayCodecs(unsigned long ulIndex);
static void DisplayGeekInfo(unsigned long ulIndex);
static void UpdateRepeat(long lValue);
#if defined(SUPPORT_SRS) || defined(SUPPORT_QSOUND) || \
    defined(SUPPORT_SPATIALIZER)
static void UpdateEffects(long lValue);
#endif
static void UpdateBacklight(long lValue);
static void UpdateVisual(long lValue);

//****************************************************************************
//
// Descriptions of the items in the menu.  The order of the items here must
// match the order of the menu items in pcMenu.
//
//****************************************************************************
static const struct
{
    //
    // The routine which draws the menu value on the LCD.
    //
    void (*pfnDisplay)(unsigned long ulIndex);

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
    // The "Repeat" menu item.
    //
    { DisplayStringRing, UpdateRepeat,
      "Repeat off\0Repeat one track\0Repeat all tracks", 3,
      (unsigned long *)&ulSystemFlags, SYSFLAG_REPEAT_MASK,
      SYSFLAG_REPEAT_SHIFT },

#if defined(SUPPORT_SRS) || defined(SUPPORT_QSOUND) || \
    defined(SUPPORT_SPATIALIZER)
    //
    // The "Effects" menu item.
    //
    { DisplayStringRing, UpdateEffects,
#ifdef SUPPORT_SRS
#ifdef SUPPORT_QSOUND
#ifdef SUPPORT_SPATIALIZER
      "None\0SRS\0QSound\0Spatializer", 4,
#else
      "None\0SRS\0QSound", 3,
#endif
#else
#ifdef SUPPORT_SPATIALIZER
      "None\0SRS\0Spatializer", 3,
#else
      "None\0SRS", 2,
#endif
#endif
#else
#ifdef SUPPORT_QSOUND
#ifdef SUPPORT_SPATIALIZER
      "None\0QSound\0Spatializer", 3,
#else
      "None\0QSound", 2,
#endif
#else
      "None\0Spatializer", 2,
#endif
#endif
      &sUI.ulFlags, FLAG_EFFECT_MASK, FLAG_EFFECT_SHIFT },
#endif

#if defined(SUPPORT_WOW_FULL) || defined(SUPPORT_WOW_MINIMAL)
    //
    // The "WOW" menu item.
    //
    { DisplayValueSlider, OutputSetSRSWidth, 0, 21, &sUI.ulSRSState,
      FLAG_SRS_WIDTH_MASK, FLAG_SRS_WIDTH_SHIFT },
#endif

#if defined(SUPPORT_WOW_FULL) || defined(SUPPORT_WOW_MINIMAL) || \
    defined(SUPPORT_TRU_BASS)
    //
    // The "TruBass" menu item.
    //
    { DisplayValueSlider, OutputSetSRSTruBass, 0, 21, &sUI.ulSRSState,
      FLAG_SRS_TRU_BASS_MASK, FLAG_SRS_TRU_BASS_SHIFT },
#endif

#ifdef SUPPORT_SRS_HEADPHONE
    //
    // The "SRS" menu item.
    //
    { DisplayValueSlider, OutputSetSRSWidth, 0, 21, &sUI.ulSRSState,
      FLAG_SRS_WIDTH_MASK, FLAG_SRS_WIDTH_SHIFT },
#endif

#ifdef SUPPORT_QSOUND
    //
    // The "Q2X" menu item.
    //
    { DisplayValueSlider, OutputSetQSoundWidth, 0, 21, &sUI.ulQSoundState,
      FLAG_QSOUND_WIDTH_MASK, FLAG_QSOUND_WIDTH_SHIFT },

    //
    // The "Q2X Mode" menu item.
    //
    { DisplayStringRing, OutputSetQSoundMode, "Mode 0\0Mode 1", 2,
      &sUI.ulQSoundState, FLAG_QSOUND_MODE_MASK, FLAG_QSOUND_MODE_SHIFT },
#endif

#ifdef SUPPORT_SPATIALIZER
    //
    // The "Bass" menu item.
    //
    { DisplayValueSlider, OutputSetSpatializerBass, 0, 21,
      &sUI.ulSpatializerState, FLAG_SPATIALIZER_BASS_MASK,
      FLAG_SPATIALIZER_BASS_SHIFT },

    //
    // The "Vi.B.E." menu item.
    //
    { DisplayValueSlider, OutputSetSpatializerVirtualBass, 0, 21,
      &sUI.ulSpatializerState, FLAG_SPATIALIZER_VBASS_MASK,
      FLAG_SPATIALIZER_VBASS_SHIFT },

    //
    // The "Gain" menu item.
    //
    { DisplayValueSlider, OutputSetSpatializerGain, 0, 21,
      &sUI.ulSpatializerState, FLAG_SPATIALIZER_GAIN_MASK,
      FLAG_SPATIALIZER_GAIN_SHIFT },
#endif

    //
    // The "Treble" menu item.
    //
    { DisplayDecibleSlider, OutputSetTreble, 0, 13, &sUI.ulFlags,
      FLAG_TREBLE_BOOST_MASK, FLAG_TREBLE_BOOST_SHIFT },

    //
    // The "Bass" menu item.
    //
    { DisplayDecibleSlider, OutputSetBass, 0, 13, &sUI.ulFlags,
      FLAG_BASS_BOOST_MASK, FLAG_BASS_BOOST_SHIFT },

    //
    // The "Backlight" menu item.
    //
    { DisplayStringRing, UpdateBacklight,
      "Always off\0On 5 seconds\0On 10 seconds\0Always on", 4, &sUI.ulFlags,
      FLAG_BACKLIGHT_MASK, FLAG_BACKLIGHT_SHIFT },

    //
    // The "Sleep" menu item.
    //
    { DisplayStringRing, 0,
      "After 1 minute\0After 2 minutes\0After 3 minutes\0After 4 minutes",
      4, &sUI.ulFlags, FLAG_SLEEP_MASK, FLAG_SLEEP_SHIFT },

    //
    // The "Visual" menu item.
    //
    { DisplayStringRing, UpdateVisual,
#ifdef SUPPORT_SPECTRUM_ANALYZER
      "None\0Waveform\0Spectrum", 3,
#else
      "None\0Waveform", 2,
#endif
      &sUI.ulFlags, FLAG_VISUAL_MODE_MASK, FLAG_VISUAL_MODE_SHIFT },

    //
    // The "Clock" menu item.
    //
    { DisplayClock, 0, 0, 1, 0, 0 },

    //
    // The "Version" menu item.
    //
    { DisplayStringRing, 0, "Release 16", 1, 0, 0 },

    //
    // The "Codecs" menu item.
    //
    { DisplayCodecs, 0, 0, 1, 0, 0 },

    //
    // The "Geek Info" menu item.
    //
    { DisplayGeekInfo, 0, 0, 1, 0, 0 },

    //
    // The end of menu indicator.
    //
    { 0, 0, 0, 0, 0, 0 }
};

//****************************************************************************
//
// The number of clock ticks (which runs at 50Hz) that each button must be
// pressed for the hold function to occur instead of the press function (i.e.
// skipping forward within a song instead of advancing to the next song).
//
//****************************************************************************
static const unsigned char ucHoldTimesMain[8] = { POWER_DELAY,
                                                  0,
#ifdef REQUIRE_LOOPBACK
                                                  RECORD_DELAY,
#else
                                                  0,
#endif
                                                  SEEK_DELAY,
                                                  SEEK_DELAY,
                                                  VOLUME_DELAY,
                                                  RECORD_DELAY,
                                                  VOLUME_DELAY };
static const unsigned char ucHoldTimesMenu[8] = { POWER_DELAY,
                                                  MENU_DELAY,
                                                  MENU_DELAY,
                                                  MENU_DELAY,
                                                  MENU_DELAY,
                                                  VOLUME_DELAY,
                                                  0,
                                                  VOLUME_DELAY };

//****************************************************************************
//
// LCDWriteIndex writes the given register value to the index register of the
// HD66410.
//
//****************************************************************************
static void
LCDWriteIndex(unsigned long ulIndex)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwLCDAddress;

    //
    // Assert nCS, set RS low, and write the index value.
    //
    sUI.usLCDValue &= ~(HwLCD_nCS | HwLCD_RS | HwLCD_Data);
    sUI.usLCDValue |= ulIndex;
    *pulPtr = sUI.usLCDValue;

    //
    // Assert nWR.
    //
    sUI.usLCDValue &= ~HwLCD_nWR;
    *pulPtr = sUI.usLCDValue;

    //
    // De-assert nWR.
    //
    sUI.usLCDValue |= HwLCD_nWR;
    *pulPtr = sUI.usLCDValue;

    //
    // De-assert nCS.
    //
    sUI.usLCDValue |= HwLCD_nCS;
    *pulPtr = sUI.usLCDValue;
}

//****************************************************************************
//
// LCDWriteData writes the given value to the data register of the HD66410.
//
//****************************************************************************
static void
LCDWriteData(unsigned long ulValue)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwLCDAddress;

    //
    // Assert nCS, set RS high, and write the value.
    //
    sUI.usLCDValue &= ~(HwLCD_nCS | HwLCD_Data);
    sUI.usLCDValue |= HwLCD_RS | ulValue;
    *pulPtr = sUI.usLCDValue;

    //
    // Assert nWR.
    //
    sUI.usLCDValue &= ~HwLCD_nWR;
    *pulPtr = sUI.usLCDValue;

    //
    // De-assert nWR.
    //
    sUI.usLCDValue |= HwLCD_nWR;
    *pulPtr = sUI.usLCDValue;

    //
    // De-assert nCS.
    //
    sUI.usLCDValue |= HwLCD_nCS;
    *pulPtr = sUI.usLCDValue;
}

//****************************************************************************
//
// LCDEnable resets the HD66410, brings it out of low power mode, and
// configures it for operation.
//
//****************************************************************************
static void
LCDEnable(void)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwLCDAddress;
    long lCount;

    //
    // Reset the HD66410.
    //
    sUI.usLCDValue = HwLCD_nCS | HwLCD_nWR | HwLCD_nRD;
    *pulPtr = sUI.usLCDValue;

    //
    // Delay for 1ms.
    //
    for(lCount = 0; lCount < 49152; lCount++)
    {
    }

    //
    // Bring the HD66410 out of reset.
    //
    sUI.usLCDValue |= HwLCD_nRES;
    *pulPtr = sUI.usLCDValue;

    //
    // Set the index register to control register one.
    //
    LCDWriteIndex(LCD_REG_CONTROL1);

    //
    // Power on the HD66410.
    //
    LCDWriteData(LCD_CONTROL1_PWR);

    //
    // Configure the HD66410.
    //
    LCDWriteData(LCD_CONTROL1_PWR | LCD_CONTROL1_CNF);
}

//****************************************************************************
//
// LCDDisable shuts down the HD66410 and puts it into low power mode.
//
//****************************************************************************
static void
LCDDisable(void)
{
    //
    // Turn off the display bit in the first control register.
    //
    LCDWriteIndex(LCD_REG_CONTROL1);
    LCDWriteData(LCD_CONTROL1_PWR | LCD_CONTROL1_CNF);

    //
    // Turn off the power bit in the first control register.
    //
    LCDWriteData(LCD_CONTROL1_CNF);
}

//****************************************************************************
//
// LCDOn turns on the LCD display.
//
//****************************************************************************
static void
LCDOn(void)
{
    //
    // Turn on the display bit in the first control regiser.
    //
    LCDWriteIndex(LCD_REG_CONTROL1);
    LCDWriteData(LCD_CONTROL1_DISP | LCD_CONTROL1_PWR | LCD_CONTROL1_CNF);
}

//****************************************************************************
//
// LCDOff turns off the LCD display.
//
//****************************************************************************
static void
LCDOff(void)
{
    //
    // Turn off the display bit in the first control regiser.
    //
    LCDWriteIndex(LCD_REG_CONTROL1);
    LCDWriteData(LCD_CONTROL1_PWR | LCD_CONTROL1_CNF);
}

//****************************************************************************
//
// LCDBacklightOn turns on the LCD backlighting.
//
//****************************************************************************
static void
LCDBacklightOn(void)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwLCDAddress;

    //
    // Turn on the backlight.
    //
    sUI.usLCDValue |= HwLCD_BL_EN;
    *pulPtr = sUI.usLCDValue;
}

//****************************************************************************
//
// LCDBacklightOff turns off the LCD backlighting.
//
//****************************************************************************
static void
LCDBacklightOff(void)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwLCDAddress;

    //
    // Turn on the backlight.
    //
    sUI.usLCDValue &= ~HwLCD_BL_EN;
    *pulPtr = sUI.usLCDValue;
}

//****************************************************************************
//
// Turns on and off the appropriate annunciators on the LCD based on the
// current settings of the player.
//
//****************************************************************************
static void
LCDUpdateAnnunciators(void)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;
    unsigned long ulStatus, ulAnnunc3 = 0, ulAnnunc5 = 0, ulAnnunc6 = 0;

    //
    // Read the system configuration register of the EP7209.
    //
    ulStatus = pulPtr[HwStatus >> 2];

    //
    // Determine the current battery level.
    //
    switch(ulStatus &
           (HwStatusCts | HwStatusDsr | HwStatusDcd | HwStatusDcPresent))
    {
        //
        // The battery level is >= 1.3V.
        //
        case HwStatusDcPresent:
        {
            //
            // Turn on the battery annunciator, as well as all three fillers.
            //
            ulAnnunc5 = LCD_ANNUNC5_BATTERY | LCD_ANNUNC5_BATTERY_FILL3 |
                        LCD_ANNUNC5_BATTERY_FILL2 | LCD_ANNUNC5_BATTERY_FILL1;

            //
            // We're done handling this battery level.
            //
            break;
        }

        //
        // The battery level is >= 1.2V.
        //
        case (HwStatusDsr | HwStatusDcPresent):
        {
            //
            // Turn on the battery annunciator, as well as the first two
            // fillers.
            //
            ulAnnunc5 = LCD_ANNUNC5_BATTERY | LCD_ANNUNC5_BATTERY_FILL2 |
                        LCD_ANNUNC5_BATTERY_FILL1;

            //
            // We're done handling this battery level.
            //
            break;
        }

        //
        // The battery level is >= 1.1V.
        //
        case (HwStatusDcd | HwStatusDsr | HwStatusDcPresent):
        {
            //
            // Turn on the battery annunciator, as well as the first filler.
            //
            ulAnnunc5 = LCD_ANNUNC5_BATTERY | LCD_ANNUNC5_BATTERY_FILL1;

            //
            // We're done handling this battery level.
            //
            break;
        }

        //
        // The battery level is >= 1.0V.
        //
        case (HwStatusCts | HwStatusDcd | HwStatusDsr | HwStatusDcPresent):
        {
            //
            // Turn on the battery annunciator.
            //
            ulAnnunc5 = LCD_ANNUNC5_BATTERY;

            //
            // We're done handling this battery level.
            //
            break;
        }

        //
        // The battery level is < 1.0V.
        //
        case (HwStatusCts | HwStatusDcd | HwStatusDsr):
        {
            //
            // Do not turn on any of the battery annunciators.
            //
            ulAnnunc5 = 0;

            //
            // We're done handling this battery level.
            //
            break;
        }
    }

    //
    // See if the repeat mode is set to repeat a single track.
    //
    if((ulSystemFlags & SYSFLAG_REPEAT_MASK) == SYSFLAG_REPEAT_ONE)
    {
        //
        // The repeat mode is set to single track, so turn on the repeat arrow
        // circles, as well as the single track annunciator.
        //
        ulAnnunc3 |= LCD_ANNUNC3_SINGLE | LCD_ANNUNC3_RIGHT_ARROWCIRCLE |
                     LCD_ANNUNC3_LEFT_ARROWCIRCLE;
    }

    //
    // See if the repeat mode is set to repeat all tracks.
    //
    else if((ulSystemFlags & SYSFLAG_REPEAT_MASK) == SYSFLAG_REPEAT_ALL)
    {
        //
        // The repeat mode is set to all tracks, so turn on the repeat arrow
        // circles.
        //
        ulAnnunc3 |= LCD_ANNUNC3_RIGHT_ARROWCIRCLE |
                     LCD_ANNUNC3_LEFT_ARROWCIRCLE;
    }

    //
    // Write the annunciator values to the LCD.
    //
    LCDWriteIndex(LCD_REG_ANNUNCIATOR3);
    LCDWriteData(ulAnnunc3);
    LCDWriteIndex(LCD_REG_ANNUNCIATOR5);
    LCDWriteData(ulAnnunc5);
    LCDWriteIndex(LCD_REG_ANNUNCIATOR6);
    LCDWriteData(ulAnnunc6);
}

//****************************************************************************
//
// LCDClear clears the LCD frame buffer.
//
//****************************************************************************
static void
LCDClear(unsigned long ulX, unsigned long ulY, unsigned long ulWidth,
         unsigned long ulHeight)
{
    unsigned long ulDX, ulDY;

    //
    // Loop through the columns.
    //
    for(ulDX = 0; ulDX < ulWidth; ulDX++)
    {
        //
        // Set the column address.
        //
        LCDWriteIndex(LCD_REG_X);
        LCDWriteData(ulX + ulDX);

        //
        // Set the row address.
        //
        LCDWriteIndex(LCD_REG_Y);
        LCDWriteData(ulY);

        //
        // Select the data register.
        //
        LCDWriteIndex(LCD_REG_DATA);

        //
        // Loop through the rows in the image.
        //
        for(ulDY = 0; ulDY < ulHeight; ulDY++)
        {
            //
            // Write the next value to the LCD.
            //
            LCDWriteData(0);
        }
    }
}

//****************************************************************************
//
// LCDDisplayImage draws an image on the LCD.  The image is organized in
// column major order.
//
//****************************************************************************
static void
LCDDisplayImage(const unsigned char *pucData, unsigned long ulX,
                unsigned long ulY, unsigned long ulWidth,
                unsigned long ulHeight)
{
    unsigned long ulDX, ulDY;

    //
    // Loop through the columns in the image.
    //
    for(ulDX = 0; ulDX < ulWidth; ulDX++)
    {
        //
        // Set the column address.
        //
        LCDWriteIndex(LCD_REG_X);
        LCDWriteData(ulX + ulDX);

        //
        // Set the row address.
        //
        LCDWriteIndex(LCD_REG_Y);
        LCDWriteData(ulY);

        //
        // Select the data register.
        //
        LCDWriteIndex(LCD_REG_DATA);

        //
        // Loop through the rows in the image.
        //
        for(ulDY = 0; ulDY < ulHeight; ulDY++)
        {
            //
            // Write the next value to the LCD.
            //
            LCDWriteData(*pucData++);
        }
    }
}

//****************************************************************************
//
// LCDStringLength determines the length of a string when drawn on the LCD.
//
//****************************************************************************
static unsigned long
LCDStringLength(char *pcString, unsigned long bIsUnicode)
{
    unsigned long ulWidth = 0;
    char cChar;

    //
    // While there are more characters in the string.
    //
    while(*pcString)
    {
        //
        // See if we are dealing with a Unicode string.
        //
        if(bIsUnicode)
        {
            //
            // See if the second byte of the character is non-zero, or if the
            // first byte is not a valid ASCII character.
            //
            if((pcString[1] != 0x00) || (pcString[0] < 0x1f) ||
               (pcString[0] > '~'))
            {
                //
                // This is an un-representable Unicode character, so simply
                // display an underbar.
                //
                cChar = '_' - 0x1f;
            }
            else
            {
                //
                // This is a valid ASCII character, so display it.
                //
                cChar = *pcString - 0x1f;
            }

            //
            // Skip past this Unicode character.
            //
            pcString += 2;
        }
        else
        {
            //
            // Get the next character to be drawn.
            //
            cChar = *pcString++ - 0x1f;
            if(cChar >= sizeof(ucCharWidth))
            {
                cChar = '_' - 0x1f;
            }
        }

        //
        // Increment the width of the string by the width of this character.
        //
        ulWidth += ucCharWidth[(unsigned long)cChar];
    }

    //
    // Return the width of the string.
    //
    return(ulWidth);
}

//****************************************************************************
//
// LCDPrintString draws a string on the LCD.
//
//****************************************************************************
static void
LCDPrintString(const char *pcString, unsigned long bIsUnicode,
               unsigned long ulRepeat, unsigned long ulX, unsigned long ulY,
               unsigned long ulEndX, unsigned long ulPreSkip,
               const unsigned char *pucPreset, unsigned long ulNumPreBits,
               const unsigned char *pucPostset, unsigned long ulNumPostBits)
{
    enum
    {
        STATE_COPY_PRESET,
        STATE_GET_CHAR,
        STATE_FINISH_CHAR,
        STATE_CLEAR_LINE,
        STATE_DISPLAY_COLUMN
    };
    unsigned long ulState = STATE_COPY_PRESET, ulNext = 0;
    unsigned char pucBitmap[11];
    unsigned long ulLoop, ulWidth = 0, ulNumBits = ulNumPreBits;
    const char *pcStr = pcString;
    char cChar = 0;

    //
    // Loop forever.  We will explicitly return when we are done drawing the
    // string.
    //
    while(1)
    {
        //
        // Determine the current state.
        //
        switch(ulState)
        {
            //
            // Copy date from the preset bitmap to the LCD.
            //
            case STATE_COPY_PRESET:
            {
                //
                // If there are preset bits before the string, then copy them
                // into the bitmap buffer.
                //
                if(pucPreset)
                {
                    //
                    // Copy the preset bits into the bitmap buffer.
                    //
                    for(ulLoop = 0; ulLoop < 11; ulLoop++)
                    {
                        pucBitmap[ulLoop] = pucPreset[ulLoop];
                    }

                    //
                    // Skip past this column of preset bits.
                    //
                    pucPreset += 11;
                }
                else
                {
                    //
                    // Clear the bitmap buffer.
                    //
                    for(ulLoop = 0; ulLoop < 11; ulLoop++)
                    {
                        pucBitmap[ulLoop] = 0;
                    }
                }

                //
                // See if there are less than 8 bit in the current column.
                //
                if(ulNumBits < 8)
                {
                    //
                    // We've copied the preset data, so start getting
                    // characters from the string.
                    //
                    ulState = STATE_GET_CHAR;
                }
                else
                {
                    //
                    // We've copied a full column of preset bits, so decrement
                    // the number of preset bits by 8.
                    //
                    ulNumBits -= 8;

                    //
                    // Display this column, after which we continue copying
                    // preset bits.
                    //
                    ulState = STATE_DISPLAY_COLUMN;
                    ulNext = STATE_COPY_PRESET;
                }

                //
                // We're done with this state.
                //
                break;
            }

            //
            // Get the next character from the input string.
            //
            case STATE_GET_CHAR:
            {
                //
                // See if there are any more characters to be displayed.
                //
                if(!*pcStr)
                {
                    //
                    // See if we should repeat the string when we get to the
                    // end.
                    //
                    if(ulRepeat)
                    {
                        //
                        // Reset the string pointer to the beginning.
                        //
                        pcStr = pcString;

                        //
                        // See if we are aligned on a four bit boundary,
                        // aligned with the number of prebits.
                        //
                        if((ulNumBits & 3) != (ulNumPreBits & 3))
                        {
                            //
                            // We are not aligned, so force the alignment.  See
                            // if we are before the first boundary.
                            //
                            if(ulNumBits <= (ulNumPreBits & 3))
                            {
                                //
                                // Move to the first boundary.
                                //
                                ulNumBits = (ulNumPreBits & 3);
                            }

                            //
                            // See if we are before the second boundary.
                            //
                            else if(ulNumBits <= ((ulNumPreBits & 3) + 4))
                            {
                                //
                                // Move to the second boundary.
                                //
                                ulNumBits = (ulNumPreBits & 3) + 4;
                            }

                            //
                            // We are after the second boundary.
                            //
                            else
                            {
                                //
                                // Move to the third boundary.
                                //
                                ulNumBits = (ulNumPreBits & 3) + 8;
                            }

                            //
                            // See if the alignment moved past the current
                            // column.
                            //
                            if(ulNumBits >= 8)
                            {
                                //
                                // Decrement the number of bits by eight.
                                //
                                ulNumBits -= 8;

                                //
                                // Display this column, after which we again
                                // attempt to display the next character.
                                //
                                ulState = STATE_DISPLAY_COLUMN;
                                ulNext = STATE_GET_CHAR;

                                //
                                // We're done with this state.
                                //
                                break;
                            }
                        }
                    }
                    else
                    {
                        //
                        // See if there are any residual bits from the last
                        // character.
                        //
                        if(ulNumBits > 1)
                        {
                            //
                            // Display this column, after which we clear the
                            // remainder of the line.
                            //
                            ulState = STATE_DISPLAY_COLUMN;
                            ulNext = STATE_CLEAR_LINE;
                        }
                        else
                        {
                            //
                            // Clear the remainder of the line.
                            //
                            ulState = STATE_CLEAR_LINE;
                        }

                        //
                        // We're done with this state.
                        //
                        break;
                    }
                }

                //
                // See if we are dealing with a Unicode string.
                //
                if(bIsUnicode)
                {
                    //
                    // See if the second byte of the character is non-zero, or
                    // if the first byte is not a valid ASCII character.
                    //
                    if((pcStr[1] != 0x00) || (pcStr[0] < 0x1f) ||
                       (pcStr[0] > '~'))
                    {
                        //
                        // This is an un-representable Unicode character, so
                        // simply display an underbar.
                        //
                        cChar = '_' - 0x1f;
                    }
                    else
                    {
                        //
                        // This is a valid ASCII character, so display it.
                        //
                        cChar = *pcStr - 0x1f;
                    }

                    //
                    // Skip past this Unicode character.
                    //
                    pcStr += 2;
                }
                else
                {
                    //
                    // Get the next character to be drawn.
                    //
                    cChar = *pcStr++ - 0x1f;
                    if(cChar >= sizeof(ucCharWidth))
                    {
                        cChar = '_' - 0x1f;
                    }
                }

                //
                // Get the width of this character.
                //
                ulWidth = ucCharWidth[(unsigned long)cChar];

                //
                // See if we should be skipping bits from the start of the
                // string.
                //
                if(ulPreSkip)
                {
                    //
                    // See if this entire character is to be skipped.
                    //
                    if(ulPreSkip >= ulWidth)
                    {
                        //
                        // Decrement the count of bits to skip by the width of
                        // this character.
                        //
                        ulPreSkip -= ulWidth;

                        //
                        // Go to the next character.
                        //
                        break;
                    }

                    //
                    // Copy the bits for this character into the bitmap buffer,
                    // skipping the first ulPreSkip columns.
                    //
                    for(ulLoop = 0; ulLoop < 11; ulLoop++)
                    {
                        pucBitmap[ulLoop] |=
                            (((ucFont[(unsigned long)cChar][ulLoop] <<
                               ulPreSkip) & 0xff) >> ulNumBits);
                    }

                    //
                    // Decrement the number of bits in the buffer by the number
                    // of bits skipped.
                    //
                    ulNumBits -= ulPreSkip;

                    //
                    // There are no more bits to skip.
                    //
                    ulPreSkip = 0;
                }
                else
                {
                    //
                    // Copy the bits for this character into the bitmap buffer.
                    //
                    for(ulLoop = 0; ulLoop < 11; ulLoop++)
                    {
                        pucBitmap[ulLoop] |=
                            ucFont[(unsigned long)cChar][ulLoop] >> ulNumBits;
                    }
                }

                //
                // Increment the bit position by the width of the character.
                //
                ulNumBits += ulWidth;

                //
                // If we have filled in all eight columns in the current byte,
                // then write it out to the LCD.
                //
                if(ulNumBits >= 8)
                {
                    //
                    // Decrement the number of bits by eight.
                    //
                    ulNumBits -= 8;

                    //
                    // Display this column, after which we copy the remainder
                    // of this character into the next column.
                    //
                    ulState = STATE_DISPLAY_COLUMN;
                    ulNext = STATE_FINISH_CHAR;
                }

                //
                // We're done with this state.
                //
                break;
            }

            //
            // Copy the remainder of the current character into the new column.
            //
            case STATE_FINISH_CHAR:
            {
                //
                // If there are still bits left, then we need to re-process
                // this character to get the right-most portion of it.
                //
                if(ulNumBits)
                {
                    //
                    // Copy the bits for this character into the bitmap buffer.
                    //
                    for(ulLoop = 0; ulLoop < 11; ulLoop++)
                    {
                        pucBitmap[ulLoop] =
                            ucFont[(unsigned long)cChar][ulLoop] <<
                            (ulWidth - ulNumBits);
                    }
                }
                else
                {
                    //
                    // Clear the bitmap buffer.
                    //
                    for(ulLoop = 0; ulLoop < 11; ulLoop++)
                    {
                        pucBitmap[ulLoop] = 0;
                    }
                }

                //
                // Process the next character.
                //
                ulState = STATE_GET_CHAR;

                //
                // We're done with this state.
                //
                break;
            }

            //
            // Clear the remainder of the line.
            //
            case STATE_CLEAR_LINE:
            {
                //
                // Fill the bitmap with zeros.
                //
                for(ulLoop = 0; ulLoop < 11; ulLoop++)
                {
                    pucBitmap[ulLoop] = 0;
                }

                //
                // Display this column repeatedly until we reach the end of
                // the line.
                //
                ulState = STATE_DISPLAY_COLUMN;
                ulNext = STATE_DISPLAY_COLUMN;

                //
                // We're done with this state.
                //
                break;
            }

            //
            // Display the current column to the LCD.
            //
            case STATE_DISPLAY_COLUMN:
            {
                //
                // See if this is the last column and we have postset bits.
                //
                if(((ulX + 1) == ulEndX) && ulNumPostBits)
                {
                    //
                    // Loop through the rows of this column.
                    //
                    for(ulLoop = 0; ulLoop < 11; ulLoop++)
                    {
                        //
                        // Mask off the bits that might be filled into the
                        // postset columns.
                        //
                        pucBitmap[ulLoop] &= 0xff << ulNumPostBits;

                        //
                        // Copy the postset bits into the bitmap buffer.
                        //
                        pucBitmap[ulLoop] |= pucPostset[ulLoop];
                    }
                }

                //
                // Write this column to the LCD.
                //
                LCDDisplayImage(pucBitmap, ulX++, ulY, 1, 11);

                //
                // If we've reached the last column to be displayed, then stop
                // drawing the string.
                //
                if(ulX == ulEndX)
                {
                    return;
                }

                //
                // Advance to the next state.
                //
                ulState = ulNext;

                //
                // We're done with this state.
                //
                break;
            }
        }
    }
}

//****************************************************************************
//
// LCDDrawMenu displays a menu on the LCD, which appears similar to a property
// page in Windows.
//
//****************************************************************************
static void
LCDDrawMenu(unsigned long ulX, unsigned long ulY, const char *pcMenuItems,
            unsigned long ulStart)
{
    const unsigned char *pucImage;
    unsigned char pucBitmap[13], ucFirst, ucSecond, ucLast;
    unsigned long ulLoop, ulWidth, ulNumBits, bIsFirst = 1, bDone = 0;
    char cChar;

    //
    // The first column of the menu is a fixed bitmap, so copy it to our bitmap
    // buffer.
    //
    for(ulLoop = 0; ulLoop < 13; ulLoop++)
    {
        pucBitmap[ulLoop] = ucMenuStart[0][ulLoop];
    }

    //
    // If we are not starting with the first menu item, then we need to add the
    // left arrow to the first column of the menu.
    //
    if(ulStart != 0)
    {
        for(ulLoop = 0; ulLoop < 7; ulLoop++)
        {
            pucBitmap[ulLoop + 3] |= ucArrowLeft[ulLoop];
        }
    }

    //
    // Display the first column of the menu.
    //
    LCDDisplayImage(pucBitmap, ulX++, ulY, 1, 13);

    //
    // The second column of the menu starts with a fixed bitmap, so copy it to
    // our bitmap buffer.
    //
    for(ulLoop = 0; ulLoop < 13; ulLoop++)
    {
        pucBitmap[ulLoop] = ucMenuStart[1][ulLoop];
    }

    //
    // There are two pixels in the second column of the left portion of the
    // menu bitmap.
    //
    ulNumBits = 2;

    //
    // Find the first menu item to be displayed.
    //
    while(ulStart)
    {
        //
        // Skip past the current string in the string list.
        //
        while(*pcMenuItems)
        {
            pcMenuItems++;
        }

        //
        // Skip past the NULL at the end of this string.
        //
        pcMenuItems++;

        //
        // Decrement the count of the number of menu items to skip.
        //
        ulStart--;
    }

    //
    // While there are more characters to draw.
    //
    while(!bDone)
    {
        //
        // See if the next character is an actual character, or a NULL at the
        // end of a menu item.
        //
        if(*pcMenuItems)
        {
            //
            // Get the next character to be drawn.
            //
            cChar = *pcMenuItems++ - 0x1f;
            if(cChar >= sizeof(ucCharWidth))
            {
                cChar = '_' - 0x1f;
            }

            //
            // Get the width of this character.
            //
            ulWidth = ucCharWidth[(unsigned long)cChar];

            //
            // Get the values for the top of the tab.
            //
            ucFirst = 0xff << (8 - ulWidth);
            ucSecond = 0;

            //
            // Get a pointer to the image for this character.
            //
            pucImage = ucFont[(unsigned long)cChar];

            //
            // See if this is the first menu item to be drawn.
            //
            if(bIsFirst)
            {
                //
                // Since this is the first menu item to be drawn, the tab is
                // open at the bottom (connecting the tab with the main display
                // area).
                //
                ucLast = 0;
            }
            else
            {
                //
                // Since this is not the first menu item to be drawn, there is
                // a line at the bottom of the tab.
                //
                ucLast = ucFirst;
            }
        }

        //
        // Otherwise, this character is a NULL, which means we must draw the
        // right side of the tab.
        //
        else
        {
            //
            // See if this is the first menu item to be drawn.
            //
            if(bIsFirst)
            {
                //
                // This is the first menu item to be drawn, so the image to be
                // drawn is the first spearator.
                //
                ucFirst = ucSeparatorFirst[0];
                ucSecond = ucSeparatorFirst[1];
                pucImage = ucSeparatorFirst + 2;

                //
                // There is no line drawn across the bottom.
                //
                ucLast = 0;

                //
                // The width of the first separator is five.
                //
                ulWidth = 5;

                //
                // Indicate that we are no longer drawing the first menu item.
                //
                bIsFirst = 0;
            }

            //
            // Otherwise, this is not the first menu item to be drawn.
            //
            else
            {
                //
                // The image to be drawn is the non-first separator.
                //
                ucFirst = ucSeparator[0];
                ucSecond = ucSeparator[1];
                pucImage = ucSeparator + 2;

                //
                // There is a line drawn across the bottom.
                //
                ucLast = 0xf0;

                //
                // THe width of the non-first separator is four.
                //
                ulWidth = 4;
            }

            //
            // Skip to the next character in the menu item string.
            //
            pcMenuItems++;

            //
            // See if the next character is a NULL.
            //
            if(*pcMenuItems)
            {
                //
                // The next character is not a NULL, so we need to also draw
                // the left portion of the next tab which is not obscured by
                // the overlapping right portion of this tab.
                //
                ucFirst |= 0x10 >> (ulWidth - 4);
                ucSecond |= 0x20 >> (ulWidth - 4);
            }
            else
            {
                //
                // The next character is a NULL, so we've reached the end of
                // the menu and are done.
                //
                bDone = 1;
            }
        }

        //
        // Copy the bits for the top two rows into the bitmap buffer.
        //
        pucBitmap[0] |= ucFirst >> ulNumBits;
        pucBitmap[1] |= ucSecond >> ulNumBits;

        //
        // Copy the bits for this character into the bitmap buffer.
        //
        for(ulLoop = 0; ulLoop < 11; ulLoop++)
        {
            pucBitmap[ulLoop + 2] |= pucImage[ulLoop] >> ulNumBits;
        }

        //
        // Copy the bits for the last row into the bitmap buffer.
        //
        pucBitmap[12] |= ucLast >> ulNumBits;

        //
        // Increment the bit position by the width of the character.
        //
        ulNumBits += ulWidth;

        //
        // If we have filled in all eight columns in the current byte, then
        // write it out to the LCD.
        //
        if(ulNumBits >= 8)
        {
            //
            // See if we've reached the end of the display.
            //
            if(ulX == 15)
            {
                //
                // We've reached the end of the display, so clear out the last
                // five pixels of this column.
                //
                for(ulLoop = 0; ulLoop < 13; ulLoop++)
                {
                    pucBitmap[ulLoop] &= 0xe0;
                }

                //
                // Add the right arrow into this column to indicate that there
                // are further menu items to be displayed.
                //
                for(ulLoop = 0; ulLoop < 7; ulLoop++)
                {
                    pucBitmap[ulLoop + 3] |= ucArrowRight[ulLoop];
                }

                //
                // Fill in the top portion of the main area box.
                //
                pucBitmap[12] |= 0xfc;
            }

            //
            // Write this column to the LCD.
            //
            LCDDisplayImage(pucBitmap, ulX++, ulY, 1, 13);

            //
            // If we've reached the last column of the LCD, then stop drawing
            // the menu.
            //
            if(ulX == 16)
            {
                return;
            }

            //
            // Decrement the number of bits by eight.
            //
            ulNumBits -= 8;

            //
            // If there are still bits left, then we need to re-process this
            // character to get the right-most portion of it.
            //
            if(ulNumBits)
            {
                //
                // Copy the bits for first two rows into the bitmap buffer.
                //
                pucBitmap[0] = ucFirst << (ulWidth - ulNumBits);
                pucBitmap[1] = ucSecond << (ulWidth - ulNumBits);

                //
                // Copy the bits for this character into the bitmap buffer.
                //
                for(ulLoop = 0; ulLoop < 11; ulLoop++)
                {
                    pucBitmap[ulLoop + 2] = pucImage[ulLoop] <<
                                            (ulWidth - ulNumBits);
                }

                //
                // Copy the bits for the last row into the bitmap buffer.
                //
                pucBitmap[12] |= ucLast << (ulWidth - ulNumBits);
            }
            else
            {
                //
                // Clear the bitmap buffer.
                //
                for(ulLoop = 0; ulLoop < 13; ulLoop++)
                {
                    pucBitmap[ulLoop] = 0;
                }
            }
        }
    }

    //
    // See if we are on the last column but have more than four pixels to be
    // displayed.
    //
    if((ulNumBits > 4) && (ulX == 15))
    {
        //
        // Clear the last five pixels from this column.
        //
        for(ulLoop = 0; ulLoop < 13; ulLoop++)
        {
            pucBitmap[ulLoop] &= 0xe0;
        }

        //
        // Add the right arrow into this column to indicate that there are
        // further menu items to be displayed.
        //
        for(ulLoop = 0; ulLoop < 7; ulLoop++)
        {
            pucBitmap[ulLoop + 3] |= ucArrowRight[ulLoop];
        }

        //
        // Fill in the top portion of the main area box.
        //
        pucBitmap[12] |= 0xfc;

        //
        // Write this column to the LCD.
        //
        LCDDisplayImage(pucBitmap, ulX, ulY, 1, 13);

        //
        // Since we've reached the last column of the LCD, stop drawing the
        // menu.
        //
        return;
    }

    //
    // If there are any valid bits in the bitmap buffer, then write the last
    // column to the LCD.
    //
    if(ulNumBits)
    {
        //
        // Fill in the top portion of the main area box.
        //
        pucBitmap[12] |= 0xff >> ulNumBits;

        //
        // Write this column to the LCD.
        //
        LCDDisplayImage(pucBitmap, ulX++, ulY, 1, 13);
    }

    //
    // If we've reached the last column of the LCD, then stop drawing the menu.
    //
    if(ulX == 16)
    {
        return;
    }

    //
    // We need to clear the remaining columns of the LCD, so clear out our
    // bitmap buffer.
    //
    for(ulLoop = 0; ulLoop < 12; ulLoop++)
    {
        pucBitmap[ulLoop] = 0;
    }

    //
    // Fill in the top portion of the main area box.
    //
    pucBitmap[12] = 0xff;

    //
    // Write this column to the remaining columns of the LCD, except for the
    // very last column.
    //
    for(; ulX < 15; ulX++)
    {
        LCDDisplayImage(pucBitmap, ulX, ulY, 1, 13);
    }

    //
    // Fill in the top portion of the main area box.
    //
    pucBitmap[12] = 0xfc;

    //
    // Write this column to the LCD.
    //
    LCDDisplayImage(pucBitmap, ulX, ulY, 1, 13);
}

//****************************************************************************
//
// Draws a slider bar on the LCD, which is similar to a progress control in
// Windows.
//
//****************************************************************************
static void
LCDDrawSlider(unsigned long ulX, unsigned long ulY, unsigned long ulWidth,
              unsigned long ulValue, unsigned long bMarkMidpoint)
{
    unsigned char pucBitmap[11];
    unsigned long ulLoop, ulIdx;

    //
    // The bottom two rows of the slider are always empty (i.e. the decender).
    //
    pucBitmap[9] = pucBitmap[10] = 0;

    //
    // Loop through all the columns in the slider.
    //
    for(ulLoop = 0; ulLoop < ulWidth; ulLoop++)
    {
        //
        // If this is the first column, then we need to draw the left edge of
        // the slider.
        //
        if(ulLoop == 0)
        {
            //
            // Fill in the left edge of the slider.
            //
            for(ulIdx = 0; ulIdx < 9; ulIdx++)
            {
                pucBitmap[ulIdx] = 0x80;
            }

            //
            // Fill in the horizontal line in the center of the slider.
            //
            pucBitmap[4] = 0xff;
        }

        //
        // If this is the last column, then we need to draw the right edge of
        // the slider.
        //
        else if((ulLoop + 1) == ulWidth)
        {
            //
            // Fill in the right edge of the slider.
            //
            for(ulIdx = 0; ulIdx < 9; ulIdx++)
            {
                pucBitmap[ulIdx] = 0x02;
            }

            //
            // Fill in the horizontal line in the center of the slider.
            //
            pucBitmap[4] = 0xfe;
        }

        //
        // Otherwise, this is in the middle of the slider.
        //
        else
        {
            //
            // Clear all the pixels in this column.
            //
            for(ulIdx = 0; ulIdx < 9; ulIdx++)
            {
                pucBitmap[ulIdx] = 0x00;
            }

            //
            // Fill in the horizontal line in the center of the slider.
            //
            pucBitmap[4] = 0xff;
        }

        //
        // See if we should mark the midpoint of the slider, and if this column
        // contains the midpoint.
        //
        if(bMarkMidpoint && (ulLoop == (ulWidth / 2)))
        {
            //
            // This column contains the midpoint, so see if the slider is an
            // even or odd number of columns wide.
            //
            if(ulWidth & 1)
            {
                //
                // The width of the slider is odd, so the center lies in the
                // fourth pixel of this column.
                //
                pucBitmap[0] |= 0x10;
                pucBitmap[1] |= 0x10;
                pucBitmap[7] |= 0x10;
                pucBitmap[8] |= 0x10;
            }
            else
            {
                //
                // The width of the slider is even, so the center lies in the
                // first pixel of this column.
                //
                pucBitmap[0] |= 0x80;
                pucBitmap[1] |= 0x80;
                pucBitmap[7] |= 0x80;
                pucBitmap[8] |= 0x80;
            }
        }

        //
        // If the value is still non-zero, then we need to fill in pixels in
        // the slider until the value reaches zero.
        //
        if(ulValue)
        {
            //
            // See if the value is greater than or equal to eight.
            //
            if(ulValue >= 8)
            {
                //
                // The value is greater than or equal to eight, so fill in all
                // pixels in the column.
                //
                pucBitmap[2] = 0xff;
                pucBitmap[3] = 0xff;
                pucBitmap[5] = 0xff;
                pucBitmap[6] = 0xff;

                //
                // Decrement the value by the eight pixels just filled in.
                //
                ulValue -= 8;
            }
            else
            {
                //
                // The value is less than eight, so fill in ulValue pixels from
                // the left of this column.
                //
                pucBitmap[2] |= 0xff << (8 - ulValue);
                pucBitmap[3] |= 0xff << (8 - ulValue);
                pucBitmap[5] |= 0xff << (8 - ulValue);
                pucBitmap[6] |= 0xff << (8 - ulValue);

                //
                // The value is now zero.
                //
                ulValue = 0;
            }
        }

        //
        // Display this column.
        //
        LCDDisplayImage(pucBitmap, ulX++, ulY, 1, 11);
    }
}

//****************************************************************************
//
// Displays the current value of a string ring within the main body of the
// menu item display.
//
//****************************************************************************
static void
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
    // Display this value on the LCD.
    //
    LCDPrintString(pcValue, 0, 0, 1, 17, 15, 0, 0, 0, 0, 0);
}

//****************************************************************************
//
// Displays the current value of a decible slider within the main body of the
// menu item display.
//
//****************************************************************************
static void
DisplayDecibleSlider(unsigned long ulIndex)
{
    char cMsg[4];
    int iValue;

    //
    // Get the current value of the decible slider.
    //
    iValue = (*(sMenuItems[ulIndex].pulFlags) &
              sMenuItems[ulIndex].ulFlagMask) >>
             sMenuItems[ulIndex].ulFlagShift;

    //
    // Convert the value to a string.
    //
    if(iValue == 0)
    {
        //
        // The value is zero, so the string is "  0".  We use the 0x1f space
        // instead of the 0x20 space so that the position of the "0" digit
        // lines up with the position when a non-zero value is displayed.
        //
        cMsg[0] = 0x1f;
        cMsg[1] = 0x1f;
        cMsg[2] = '0';
    }
    else
    {
        //
        // The value is non-zero, so the first character is a "+".
        //
        cMsg[0] = '+';

        //
        // If the value is greater than 9, then fill in the most significant
        // digit of the value.
        //
        if(iValue > 9)
        {
            cMsg[1] = '0' + (iValue / 10);
        }
        else
        {
            cMsg[1] = 0x1f;
        }

        //
        // Fill in the least significant digit of the value.
        //
        cMsg[2] = '0' + (iValue % 10);
    }

    //
    // NULL terminate the string.
    //
    cMsg[3] = '\0';

    //
    // Draw the actual slider.
    //
    LCDDrawSlider(1, 17, 10,
                  ((iValue * 77) / (sMenuItems[ulIndex].ulNumValues - 1)) + 1,
                  0);

    //
    // Print the value of the slider.
    //
    LCDPrintString(cMsg, 0, 0, 11, 17, 14, 0, 0, 5, 0, 0);

    //
    // Display the decible indicator.
    //
    LCDDisplayImage(ucDB, 14, 17, 1, 11);
}

//****************************************************************************
//
// Displays the current value of a slider within the main body of the menu
// item display.
//
//****************************************************************************
#if defined(SUPPORT_SRS) || defined(SUPPORT_QSOUND) || \
    defined(SUPPORT_SPATIALIZER)
static void
DisplayValueSlider(unsigned long ulIndex)
{
    char cMsg[3];
    int iValue;

    //
    // Get the current value of the slider.
    //
    iValue = (*(sMenuItems[ulIndex].pulFlags) &
              sMenuItems[ulIndex].ulFlagMask) >>
             sMenuItems[ulIndex].ulFlagShift;

    //
    // Convert the value to a string.
    //
    if(iValue == 0)
    {
        //
        // The value is zero, so the string is " 0".  We use the 0x1f space
        // instead of the 0x20 space so that the position of the "0" digit
        // lines up with the position when a non-zero value is displayed.
        //
        cMsg[0] = 0x1f;
        cMsg[1] = '0';
    }
    else
    {
        //
        // If the value is greater than 9, then fill in the most significant
        // digit of the value.
        //
        if(iValue > 9)
        {
            cMsg[0] = '0' + (iValue / 10);
        }
        else
        {
            cMsg[0] = 0x1f;
        }

        //
        // Fill in the least significant digit of the value.
        //
        cMsg[1] = '0' + (iValue % 10);
    }

    //
    // NULL terminate the string.
    //
    cMsg[2] = '\0';

    //
    // Draw the actual slider.
    //
    LCDDrawSlider(1, 17, 12,
                  ((iValue * 93) / (sMenuItems[ulIndex].ulNumValues - 1)) + 1,
                  0);

    //
    // Print the value of the slider.
    //
    LCDPrintString(cMsg, 0, 0, 13, 17, 15, 0, 0, 5, 0, 0);
}
#endif

//****************************************************************************
//
// Displays the current date and time within the main body of the menu item
// display.
//
//****************************************************************************
static void
DisplayClock(unsigned long ulIndex)
{
    tTime sTime;
    char cMsg[9];
    int iHour;

    //
    // Get the current system time and convert it to the broken down time
    // components (i.e. month, day, year, hours, minutes, and seconds).
    //
    TimeFromSeconds(&sTime, TimeGetSystemTime());

    //
    // If the month is greater than 9, then fill in the most significant digit
    // of the month.
    //
    if(sTime.cMonth > 9)
    {
        cMsg[0] = '0' + (sTime.cMonth / 10);
    }
    else
    {
        cMsg[0] = 0x1f;
    }

    //
    // Fill in the least significant digit of the month.
    //
    cMsg[1] = '0' + (sTime.cMonth % 10);

    //
    // Add a separator between the month and day.
    //
    cMsg[2] = '/';

    //
    // If the day is greater than 9, then fill in the most significant digit of
    // the day.
    //
    if(sTime.cDay > 9)
    {
        cMsg[3] = '0' + (sTime.cDay / 10);
    }
    else
    {
        cMsg[3] = 0x1f;
    }

    //
    // Fill in the least significant digit of the day.
    //
    cMsg[4] = '0' + (sTime.cDay % 10);

    //
    // Add a separator between the day and year.
    //
    cMsg[5] = '/';

    //
    // Fill in the most significant digit of the year.
    //
    cMsg[6] = '0' + ((sTime.sYear / 10) % 10);

    //
    // Fill in the least significant digit of the year.
    //
    cMsg[7] = '0' + (sTime.sYear % 10);

    //
    // NULL terminate the string.
    //
    cMsg[8] = '\0';

    //
    // Display the image for the current weekday.
    //
    LCDDisplayImage((unsigned char *)ucDays[(unsigned long)sTime.cWeekday], 1,
                    17, 2, 11);

    //
    // Display the string containing the current date.
    //
    LCDPrintString(cMsg, 0, 0, 3, 17, 9, 0, 0, 0, 0, 0);

    //
    // Determine if it is AM or PM.
    //
    if(sTime.cHour < 12)
    {
        //
        // It is AM, so display the AM image.
        //
        LCDDisplayImage((unsigned char *)ucAM, 13, 17, 2, 11);

        //
        // If the current hour is zero, then it is 12AM.
        //
        if(sTime.cHour == 0)
        {
            iHour = 12;
        }

        //
        // Otherwise, the hour is correct.
        //
        else
        {
            iHour = sTime.cHour;
        }
    }
    else
    {
        //
        // It is PM, so display the PM image.
        //
        LCDDisplayImage((unsigned char *)ucPM, 13, 17, 2, 11);

        //
        // If the current hour is 12, then it is 12PM.
        //
        if(sTime.cHour == 12)
        {
            iHour = 12;
        }

        //
        // Otherwise, the hour is the current value minus 12 (i.e. convert from
        // military time to civilian time).
        //
        else
        {
            iHour = sTime.cHour - 12;
        }
    }

    //
    // If the hour is greater than 9, then fill in the most significant digit
    // of the hour.
    //
    if(iHour > 9)
    {
        cMsg[0] = '0' + (iHour / 10);
    }
    else
    {
        cMsg[0] = 0x1f;
    }

    //
    // Fill in the least significant digit of the hour.
    //
    cMsg[1] = '0' + (iHour % 10);

    //
    // Add a separator between the hour and minute.
    //
    cMsg[2] = ':';

    //
    // Fill in the most significant digit of the minute.
    //
    cMsg[3] = '0' + (sTime.cMin / 10);

    //
    // Fill in the least significant digit of the minute.
    //
    cMsg[4] = '0' + (sTime.cMin % 10);

    //
    // NULL terminate the string.
    //
    cMsg[5] = '\0';

    //
    // Display the string containing the current time.
    //
    LCDPrintString(cMsg, 0, 0, 9, 17, 13, 0, 0, 6, 0, 0);
}

//****************************************************************************
//
// Displays a list of the available codecs within the main body of the menu
// item display.
//
//****************************************************************************
static void
DisplayCodecs(unsigned long ulIndex)
{
    const unsigned short *pusName;
    unsigned short usMsg[40];
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
        usMsg[j++] = pusName[0];
        usMsg[j++] = pusName[1];
        usMsg[j++] = pusName[2];
        usMsg[j++] = pusName[3];

        //
        // Remove any trailing spaces from the short name of the codec.
        //
        while(usMsg[j - 1] == ' ')
        {
            j--;
        }

        //
        // Add a comma to separate this codec from the next.
        //
        usMsg[j++] = ',';
    }

    //
    // Replace the last comma with a NULL to terminate the string.
    //
    usMsg[j - 1] = '\0';

    //
    // Display the list of codec names.
    //
    LCDPrintString((char *)usMsg, 1, 0, 1, 17, 15, 0, 0, 0, 0, 0);
}

//****************************************************************************
//
// Displays "tech-head" information about the current file within the main
// body of the menu item display.
//
//****************************************************************************
static void
DisplayGeekInfo(unsigned long ulIndex)
{
    const unsigned short *pusCodec;
    char cMsg[5];
    unsigned long ulTemp;

    //
    // Clear the display area.
    //
    LCDClear(1, 17, 14, 11);

    //
    // Get the name of the current codec.
    //
    if(CodecGetName(&pusCodec) == 0)
    {
        pusCodec = (unsigned short *)"N\0o\0n\0e";
    }

    //
    // Copy the short name of the codec used to decode this file.
    //
    cMsg[0] = pusCodec[0];
    cMsg[1] = pusCodec[1];
    cMsg[2] = pusCodec[2];
    cMsg[3] = pusCodec[3];

    //
    // NULL terminate the string.
    //
    cMsg[4] = '\0';

    //
    // Display the short name of the codec.
    //
    LCDPrintString(cMsg, 0, 0, 1, 17, 4, 0, 0, 0, 0, 0);

    //
    // Get the sample rate of the current file.
    //
    if(CodecGetSampleRate(&ulTemp) == 0)
    {
        ulTemp = 0;
    }

    //
    // If the sample rate is greater than 9999, then fill in the most
    // significant digit of the kHz sample rate.
    //
    if(ulTemp >= 10000)
    {
        cMsg[0] = '0' + (ulTemp / 10000);
    }
    else
    {
        cMsg[0] = 0x1f;
    }

    //
    // Fill in the least significant digit of the kHz sample rate.
    //
    cMsg[1] = '0' + ((ulTemp / 1000) % 10);

    //
    // NULL terminate the sample rate string.
    //
    cMsg[2] = '\0';

    //
    // Display the sample rate string.
    //
    LCDPrintString(cMsg, 0, 0, 4, 17, 6, 0, 0, 4, 0, 0);

    //
    // Display the KHZ image.
    //
    LCDDisplayImage((unsigned char *)ucKHZ, 6, 17, 1, 11);

    //
    // Get the bit rate of the current file.
    //
    if(CodecGetBitRate(&ulTemp) == 0)
    {
        ulTemp = 0;
    }

    //
    // If the bit rate is greater than 99999, then fill in the most significant
    // digit of the kbps bit rate.
    //
    if(ulTemp >= 100000)
    {
        cMsg[0] = '0' + (ulTemp / 100000);
    }
    else
    {
        cMsg[0] = 0x1f;
    }

    //
    // If the bit rate is greater than 9999, then fill in the next most
    // significant digit of the kbps bit rate.
    //
    if(ulTemp >= 10000)
    {
        cMsg[1] = '0' + ((ulTemp / 10000) % 10);
    }
    else
    {
        cMsg[1] = 0x1f;
    }

    //
    // Fill in the least significant digit of the kbps bit rate.
    //
    cMsg[2] = '0' + ((ulTemp / 1000) % 10);

    //
    // NULL terminate the bit rate string.
    //
    cMsg[3] = '\0';

    //
    // Display the bit rate string.
    //
    LCDPrintString(cMsg, 0, 0, 7, 17, 10, 0, (unsigned char *)(ucKHZ + 1), 6,
                   0, 0);

    //
    // Display the KBPS image.
    //
    LCDDisplayImage((unsigned char *)ucKBPS, 10, 17, 2, 5);

    //
    // Get the number of channels in the current file.
    //
    if(CodecGetChannels(&ulTemp) == 0)
    {
        ulTemp = 1;
    }

    //
    // See if the file is stereo or mono.
    //
    if(ulTemp == 2)
    {
        //
        // The file is stereo, so display the stereo image.
        //
        LCDDisplayImage((unsigned char *)ucStereo, 12, 21, 3, 5);
    }
    else
    {
        //
        // The file is mono, so display the mono image.
        //
        LCDDisplayImage((unsigned char *)ucMono, 12, 21, 3, 5);
    }
}

//****************************************************************************
//
// Updates the annunciators based on the new repeat value.
//
//****************************************************************************
static void
UpdateRepeat(long lValue)
{
    //
    // Update the annunciators on the LCD.
    //
    LCDUpdateAnnunciators();
}

//****************************************************************************
//
// Updates the effects processing which is currently enabled.
//
//****************************************************************************
#if defined(SUPPORT_SRS) || defined(SUPPORT_QSOUND) || \
    defined(SUPPORT_SPATIALIZER)
static void
UpdateEffects(long lValue)
{
    //
    // The following enum is the list of effects which are included in the
    // image.  The order of this enum must match the string ring in the menu.
    //
    enum
    {
        eNone = 0,
#ifdef SUPPORT_SRS
        eSRS,
#endif
#ifdef SUPPORT_QSOUND
        eQSound,
#endif
#ifdef SUPPORT_SPATIALIZER
        eSpatializer,
#endif
        eEndOfList
    };

    //
    // Determine which effect to enable based on the new menu item value.
    //
    switch(lValue)
    {
        //
        // The "None" value is selected.
        //
        case eNone:
        {
            //
            // Disable SRS, QSound, and Spatializer.
            //
#ifdef SUPPORT_SRS
            OutputSRSDisable();
#endif
#ifdef SUPPORT_QSOUND
            OutputQSoundDisable();
#endif
#ifdef SUPPORT_SPATIALIZER
            OutputSpatializerDisable();
#endif

            //
            // We're done with this value.
            //
            break;
        }

        //
        // The "SRS" value is selected.
        //
#ifdef SUPPORT_SRS
        case eSRS:
        {
            //
            // Disable QSound and Spatializer.
            //
#ifdef SUPPORT_QSOUND
            OutputQSoundDisable();
#endif
#ifdef SUPPORT_SPATIALIZER
            OutputSpatializerDisable();
#endif

            //
            // Enable SRS.
            //
            OutputSRSEnable();

            //
            // We're done with this value.
            //
            break;
        }
#endif

        //
        // The "QSound" value is selected.
        //
#ifdef SUPPORT_QSOUND
        case eQSound:
        {
            //
            // Disable SRS and Spatializer.
            //
#ifdef SUPPORT_SRS
            OutputSRSDisable();
#endif
#ifdef SUPPORT_SPATIALIZER
            OutputSpatializerDisable();
#endif

            //
            // Enable QSound.
            //
            OutputQSoundEnable();

            //
            // We're done with this value.
            //
            break;
        }
#endif

        //
        // The "Spatializer" value is selected.
        //
#ifdef SUPPORT_SPATIALIZER
        case eSpatializer:
        {
            //
            // Disable SRS and QSound.
            //
#ifdef SUPPORT_SRS
            OutputSRSDisable();
#endif
#ifdef SUPPORT_QSOUND
            OutputQSoundDisable();
#endif

            //
            // Enable Spatializer.
            //
            OutputSpatializerEnable();

            //
            // We're done with this value.
            //
            break;
        }
#endif
    }
}
#endif

//****************************************************************************
//
// Updates the backlight based on the new backlight value.
//
//****************************************************************************
static void
UpdateBacklight(long lValue)
{
    //
    // See if the new backlight value is always off.
    //
    if((sUI.ulFlags & FLAG_BACKLIGHT_MASK) == FLAG_BACKLIGHT_OFF)
    {
        //
        // The new backlight value is always off, so turn off the backlight
        // immediately.
        //
        LCDBacklightOff();

        //
        // Indicate that the backlight is off.
        //
        sUI.ulFlags &= ~FLAG_BACKLIGHT;
    }

    //
    // See if the new backlight value is always on.
    //
    else if((sUI.ulFlags & FLAG_BACKLIGHT_MASK) == FLAG_BACKLIGHT_ON)
    {
        //
        // The new backlight value is always on, so turn on the backlight
        // immediately.
        //
        LCDBacklightOn();

        //
        // Indicate that the backlight is on.
        //
        sUI.ulFlags |= FLAG_BACKLIGHT;
    }
}

//****************************************************************************
//
// Enables or disables the spectrum analyzer based on changes to the visual
// setting.
//
//****************************************************************************
static void
UpdateVisual(long lValue)
{
#ifdef SUPPORT_SPECTRUM_ANALYZER
    //
    // See if the spectrum analyzer is selected.
    //
    if((sUI.ulFlags & FLAG_VISUAL_MODE_MASK) == FLAG_VISUAL_MODE_SPECTRUM)
    {
        //
        // Enable the spectrum analyzer.
        //
        OutputSpectrumEnable();
    }
    else
    {
        //
        // Disable the spectrum analzyer.
        //
        OutputSpectrumDisable();
    }
#endif
}

//****************************************************************************
//
// UpdateSpectrum displays the current spectral intensities in the visual
// area.
//
//****************************************************************************
#ifdef SUPPORT_SPECTRUM_ANALYZER
static void
UpdateSpectrum(void)
{
    unsigned char ucFill[10];
    long lBands[5];
    unsigned long ulBand, ulColumn, ulRow;

    //
    // Get the current spectral intensities.
    //
    OutputGetSpectralBands(lBands);

    //
    // Loop through all the bands.
    //
    for(ulBand = 0; ulBand < 5; ulBand++)
    {
        //
        // Scale this band.
        //
        lBands[ulBand] /= 512;

        //
        // Clip this band if it is too large.
        //
        if(lBands[ulBand] > 9)
        {
            lBands[ulBand] = 10;
        }
    }

    //
    // Loop through the seven columns on the LCD which contain the visual area.
    //
    for(ulColumn = 0; ulColumn < 7; ulColumn++)
    {
        //
        // Copy the background into our working column image.
        //
        memcpy(ucFill, ucVisualBackground[ulColumn], 10);

        //
        // Loop through the five bands.
        //
        for(ulBand = 0; ulBand < 5; ulBand++)
        {
            //
            // Skip this band if it is not drawn on the current column of the
            // LCD.
            //
            if(!ucBandPattern[ulBand][ulColumn])
            {
                continue;
            }

            //
            // Loop through the 10 rows of this column.
            //
            for(ulRow = 0; ulRow < 10; ulRow++)
            {
                //
                // See if the intensity of this band is greater than or equal
                // to the current row.
                //
                if(lBands[ulBand] >= (10 - ulRow))
                {
                    //
                    // The intensity of this band is greater than or equal to
                    // the current row, so we need to draw this band on this
                    // row.
                    //
                    ucFill[ulRow] |= ucBandPattern[ulBand][ulColumn];
                }
            }
        }

        //
        // Display the current column on the LCD.
        //
        LCDDisplayImage(ucFill, ulColumn, 17, 1, 10);
    }
}
#endif

//****************************************************************************
//
// UpdateWaveform displays a representation of the current waveform in the
// visual area.
//
//****************************************************************************
static void
UpdateWaveform(void)
{
    unsigned char ucFill[10];
    unsigned long ulIdx, ulLoop, ulOffset, ulSample;
    short *psLeft, *psRight;
    BufferState *psBuffer;

    //
    // Get a pointer to the play buffer.
    //
    psBuffer = DAIGetPlayBuffer();

    //
    // Get a pointer to the left and right channels of the play buffer.
    //
    psLeft = psBuffer->psLeft;
    psRight = psBuffer->psRight;

    //
    // Start at the beginning of the player buffer.
    //
    ulOffset = 0;

    //
    // Loop through the seven columns on the LCD which contain the visual area.
    //
    for(ulLoop = 0; ulLoop < 7; ulLoop++)
    {
        //
        // Copy the background into our working column image.
        //
        memcpy(ucFill, ucVisualBackground[ulLoop], 10);

        //
        // Loop through the eight sub-columns in this column.
        //
        for(ulIdx = 0; ulIdx < 8; ulIdx++)
        {
            //
            // If this is the first column, we do not draw waveform data on the
            // first six sub-columns.
            //
            if((ulLoop == 0) && (ulIdx < 6))
            {
                continue;
            }

            //
            // If this is the last column, we do not draw waveform data on the
            // last six sub-columns.
            //
            if((ulLoop == 6) && (ulIdx > 1))
            {
                continue;
            }

            //
            // Get the next sample, averaging the left and right channels and
            // converting to an unsigned value.
            //
            ulSample = (((psLeft[ulOffset] + psRight[ulOffset]) >> 1) + 32768);

            //
            // Skip the next seven samples.
            //
            ulOffset += 8;

            //
            // Set the appropriate row of this sub-column based on the sample
            // value.
            //
            ucFill[(ulSample * 10) >> 16] |= 1 << (7 - ulIdx);
        }

        //
        // Display the current column on the LCD.
        //
        LCDDisplayImage(ucFill, ulLoop, 17, 1, 10);
    }
}

//****************************************************************************
//
// UpdateTrackBar displays the track bar.
//
//****************************************************************************
static void
UpdateTrackBar(unsigned long ulTime, unsigned long ulLength)
{
    unsigned char ucImage[3];
    unsigned long ulColumn, ulPos;

    //
    // Convert the time into the position of the nob on the track bar.
    //
    ulPos = ((ulTime * 61) / ulLength) + 2;
    if(ulPos == 63)
    {
        ulPos--;
    }

    //
    // Loop through the nine columns of the track bar.
    //
    for(ulColumn = 0; ulColumn < 9; ulColumn++)
    {
        //
        // Get the base track bar image for this column.
        //
        ucImage[0] = ucTrack[ulColumn][0];
        ucImage[1] = ucTrack[ulColumn][1];
        ucImage[2] = ucTrack[ulColumn][2];

        //
        // See if the nob starts on this column of the track bar.
        //
        if((ulPos >> 3) == ulColumn)
        {
            //
            // Mask off the portion of the bar that will be covered by the nob.
            //
            ucImage[1] &= ~(0xfe >> (ulPos & 7));

            //
            // Put the start of the nob on this column of the track bar.
            //
            ucImage[0] |= ucTrackBar[0] >> (ulPos & 7);
            ucImage[1] |= ucTrackBar[1] >> (ulPos & 7);
            ucImage[2] |= ucTrackBar[2] >> (ulPos & 7);
        }

        //
        // See if the nob ends on this column of the track bar.
        //
        else if(((ulPos + 6) >> 3) == ulColumn)
        {
            //
            // Mask off the portion of the bar that will be covered by the nob.
            //
            ucImage[1] &= ~(0xfe << (6 - ((ulPos - 2) & 7)));

            //
            // Put the end of the nob on this column of the track bar.
            //
            ucImage[0] |= ucTrackBar[0] << (6 - ((ulPos - 2) & 7));
            ucImage[1] |= ucTrackBar[1] << (6 - ((ulPos - 2) & 7));
            ucImage[2] |= ucTrackBar[2] << (6 - ((ulPos - 2) & 7));
        }

        //
        // Display this column of the track bar on the LCD.
        //
        LCDDisplayImage(ucImage, 7 + ulColumn, 26, 1, 3);
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
    // Initialize the user interface flags.  Set the volume to mid-scale and
    // turn off the treble and bass boost.
    //
    sUI.ulFlags = ((15 << FLAG_VOLUME_SHIFT) | FLAG_DISPLAY_STATE_LOGO1 |
                   FLAG_BACKLIGHT_5 | FLAG_SLEEP_4 | FLAG_BACKLIGHT |
                   FLAG_VISUAL_MODE_WAVEFORM);

    //
    // Initialize the Spatializer flags.
    //
#ifdef SUPPORT_SPATIALIZER
    sUI.ulSpatializerState = ((unsigned long)20 <<
                              FLAG_SPATIALIZER_GAIN_SHIFT);
    OutputSetSpatializerGain(20);
#endif

    //
    // Initialize the tick count and the backlight time to zero.
    //
    sUI.usTickCount = 0;
    sUI.usBacklightTime = 0;

    //
    // Initialize the state of the button debouncer.  All buttons start in the
    // not pressed state.
    //
    sUI.ulKeys = 0;
    sUI.ucState = 0;
    sUI.ucClock = 0;

    //
    // Initialize the mode to stopped.
    //
    sUI.ucMode = MODE_STOP;

    //
    // Initially, there are no tracks.
    //
    UINoTracks();

    //
    // Initialize the volume.
    //
    OutputSetVolume(15);

    //
    // Initialize the treble and bass boost.
    //
    OutputSetTone(0, 0, -6);

    //
    // Disable the spectrum analyzer.
    //
#ifdef SUPPORT_SPECTRUM_ANALYZER
    OutputSpectrumDisable();
#endif

    //
    // Disable the third-party output processing algorithms.
    //
#ifdef SUPPORT_SRS
    OutputSRSDisable();
#endif
#ifdef SUPPORT_QSOUND
    OutputQSoundDisable();
#endif
#ifdef SUPPORT_SPATIALIZER
    OutputSpatializerDisable();
#endif

    //
    // Initialize the LCD.
    //
    LCDEnable();

    //
    // Turn on the backlight.
    //
    LCDBacklightOn();

    //
    // Initialize the annunciators.
    //
    LCDUpdateAnnunciators();

    //
    // Draw the Cirrus Logic logo on the LCD.
    //
    LCDDisplayImage(ucCirrusLogo, 0, 0, 16, 33);
    LCDOn();

    //
    // Set the target time to the amount of time to display the Cirrus logo.
    //
    sUI.usTargetTime = sUI.usTickCount + LOGO1_DELAY;
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
    // If the current mode is download or power on, then we need to update the
    // entire LCD.
    //
    if(((sUI.ucMode & 0x1f) == MODE_DOWNLOAD) ||
       ((sUI.ucMode & 0x1f) == MODE_POWER_ON))
    {
        //
        // Indicate that the entire LCD needs to be updated.
        //
        sUI.ulFlags |= FLAG_UPDATE_SCREEN;

        //
        // Enable the timer interrupt.
        //
        EnableTimer();
    }

    //
    // Remember the new mode, and set the flag indicating that we need to
    // update the LCD.
    //
    sUI.ucMode = ulMode | 0x80;

    //
    // Some modes we handle directly.  See if we are powering off.
    //
    if(ulMode == MODE_POWER_OFF)
    {
        //
        // We are powering off, so turn off the backlight.
        //
        LCDBacklightOff();

        //
        // Indicate that the backlight is off.
        //
        sUI.ulFlags &= ~FLAG_BACKLIGHT;

        //
        // Turn off the LCD display.
        //
        LCDOff();

        //
        // Disable the LCD controller.
        //
        LCDDisable();

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
        // Enable the LCD controller.
        //
        LCDEnable();

        //
        // Initialize the annunciators.
        //
        LCDUpdateAnnunciators();

        //
        // See if the backlight should be on.
        //
        if((sUI.ulFlags & FLAG_BACKLIGHT_MASK) != FLAG_BACKLIGHT_OFF)
        {
            //
            // The backlight should be on, so turn it on now.
            //
            LCDBacklightOn();

            //
            // Indicate that the backlight is on.
            //
            sUI.ulFlags |= FLAG_BACKLIGHT;

            //
            // Determine the target time for turning off the backlight.
            //
            switch(sUI.ulFlags & FLAG_BACKLIGHT_MASK)
            {
                //
                // The backlight should be on for five seconds.
                //
                case FLAG_BACKLIGHT_5:
                {
                    //
                    // Set the backlight target time to five seconds from now.
                    //
                    sUI.usBacklightTime = sUI.usTickCount + 250;

                    //
                    // We're done with this backlight length.
                    //
                    break;
                }

                //
                // The backlight should be on for ten seconds.
                //
                case FLAG_BACKLIGHT_10:
                {
                    //
                    // Set the backlight target time to ten seconds from now.
                    //
                    sUI.usBacklightTime = sUI.usTickCount + 500;

                    //
                    // We're done with this backlight length.
                    //
                    break;
                }
            }
        }

        //
        // Since we were just powered on, we want to wait until all the buttons
        // are released before we process any buttons.  The main reason for
        // this is that the power button is pressed to wake up the player, and
        // we wouldn't want that button press to cause the player to power back
        // off or enter the menu.
        //
        sUI.ulFlags |= FLAG_WAIT_FOR_ALL_UP;

        //
        // Clear any keys that might have already been pressed.
        //
        sUI.ulKeys = 0;

        //
        // Reset the display state to the main screen.
        //
        sUI.ulFlags &= ~FLAG_DISPLAY_STATE_MASK;
        sUI.ulFlags |= FLAG_DISPLAY_STATE_MAIN;
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

        //
        // Turn off the LCD.
        //
        LCDOff();

        //
        // Display the download image.
        //
        LCDDisplayImage(ucDownload, 0, 0, 16, 33);

        //
        // Turn on the LCD.
        //
        LCDOn();

        //
        // See if the backlight should be on all the time.
        //
        if((sUI.ulFlags & FLAG_BACKLIGHT_MASK) != FLAG_BACKLIGHT_ON)
        {
            //
            // Turn off the backlight.
            //
            LCDBacklightOff();

            //
            // Indicate that the backlight is off.
            //
            sUI.ulFlags &= ~FLAG_BACKLIGHT;
        }
    }

    //
    // See if we are starting loopback mode.
    //
#ifdef REQUIRE_LOOPBACK
    else if(ulMode == MODE_LOOPBACK)
    {
        //
        // Set the track name to " Looping...".
        //
        memcpy(sUI.pusName, " \0L\0o\0o\0p\0i\0n\0g\0.\0.\0.\0\0", 24);

        //
        // Set the name length and offset so that the track name does not get
        // scrolled (it is short and therefore doesn't need scrolling).
        //
        sUI.ucNameLength = 1;
        sUI.ucNameOffset = 0;

        //
        // Indicate that the track name needs to be updated.
        //
        DisableIRQ();
        sUI.ulFlags |= FLAG_UPDATE_TRACK_NAME;
        EnableIRQ();
    }
#endif
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
    const unsigned short *pusArtist, *pusSongName;
    unsigned long ulIdx = 0, ulLength;

#ifdef SUPPORT_AUDIBLE
    //
    // See if this is an Audible program.
    //
    if(AudibleIsAudibleProgram())
    {
        //
        // There is no artist for an Audible program.
        //
        pusArtist = 0;

        //
        // Get the title of the Audible program.
        //
        pusSongName = AudibleGetTitle();
    }
    else
#endif
    {
        //
        // Get the name of the artist.
        //
        if(CodecGetArtist(&pusArtist) == 0)
        {
            pusArtist = 0;
        }

        //
        // Get the name of the song.
        //
        if(CodecGetTitle(&pusSongName) == 0)
        {
            pusSongName = 0;
        }
    }

    //
    // We want to display a one based track number, not zero based.
    //
    ulTrack++;

    //
    // Start the information string with the track number.
    //
    if(ulTrack > 99)
    {
        sUI.pusName[ulIdx > 72 ? 72 : ulIdx++] = '0' + (ulTrack / 100);
    }
    if(ulTrack > 9)
    {
        sUI.pusName[ulIdx > 72 ? 72 : ulIdx++] = '0' + ((ulTrack / 10) % 10);
    }
    sUI.pusName[ulIdx > 72 ? 72 : ulIdx++] = '0' + (ulTrack % 10);

    //
    // Follow the track number with ". ".
    //
    sUI.pusName[ulIdx > 72 ? 72 : ulIdx++] = '.';
    sUI.pusName[ulIdx > 72 ? 72 : ulIdx++] = ' ';

    //
    // Add the artist name, if one is present.
    //
    if(pusArtist && *pusArtist)
    {
        //
        // Copy the artist name.
        //
        while(*pusArtist)
        {
            sUI.pusName[ulIdx > 72 ? 72 : ulIdx++] = *pusArtist++;
        }

        //
        // Separate the artist name from the song name with a " - ".
        //
        sUI.pusName[ulIdx > 72 ? 72 : ulIdx++] = ' ';
        sUI.pusName[ulIdx > 72 ? 72 : ulIdx++] = '-';
        sUI.pusName[ulIdx > 72 ? 72 : ulIdx++] = ' ';
    }

    //
    // Add the song name, if one is present.
    //
    if(pusSongName && *pusSongName)
    {
        //
        // Copy the song name.
        //
        while(*pusSongName)
        {
            sUI.pusName[ulIdx > 72 ? 72 : ulIdx++] = *pusSongName++;
        }
    }

    //
    // Otherwise, add the file name.
    //
    else
    {
        //
        // Copy the file name.
        //
        while(*pusFileName)
        {
            sUI.pusName[ulIdx > 72 ? 72 : ulIdx++] = *pusFileName++;
        }
    }

    //
    // Follow the song name with " (".
    // 
    sUI.pusName[ulIdx > 72 ? 72 : ulIdx++] = ' ';
    sUI.pusName[ulIdx > 72 ? 72 : ulIdx++] = '(';

    //
    // Get the length of this track.
    //
    CodecGetLength(&ulLength);

    //
    // Convert the track length into seconds, rounding up.
    //
    ulLength = (ulLength + 500) / 1000;

    //
    // Convert the length into a character representation.  A truncated version
    // of HH:MM:SS is used (i.e. M:SS for a track less than 10 mintues, MM:SS
    // for a track less than 1 hour, etc.).
    //
    if(ulLength > 36000)
    {
        sUI.pusName[ulIdx > 72 ? 72 : ulIdx++] = '0' + ((ulLength / 36000) %
                                                        10);
    }
    if(ulLength > 3600)
    {
        sUI.pusName[ulIdx > 72 ? 72 : ulIdx++] = '0' + ((ulLength / 3600) %
                                                        10);
        sUI.pusName[ulIdx > 72 ? 72 : ulIdx++] = ':';
    }
    if(ulLength > 600)
    {
        sUI.pusName[ulIdx > 72 ? 72 : ulIdx++] = '0' + ((ulLength / 600) % 6);
    }
    sUI.pusName[ulIdx > 72 ? 72 : ulIdx++] = '0' + ((ulLength / 60) % 10);
    sUI.pusName[ulIdx > 72 ? 72 : ulIdx++] = ':';
    sUI.pusName[ulIdx > 72 ? 72 : ulIdx++] = '0' + ((ulLength / 10) % 6);
    sUI.pusName[ulIdx > 72 ? 72 : ulIdx++] = '0' + (ulLength % 10);

    //
    // Follow the track length with ")".
    //
    sUI.pusName[ulIdx > 72 ? 72 : ulIdx++] = ')';

    //
    // Terminate the string.
    //
    sUI.pusName[ulIdx > 72 ? 72 : ulIdx] = '\0';

    //
    // Get the length of the track name string.
    //
    ulLength = LCDStringLength((char *)sUI.pusName, 1);

    //
    // See if the string will fit on the LCD without scrolling.
    //
    if(ulLength < 108)
    {
        //
        // The string will fit without scrolling, so get the length divided by
        // four.
        //
        sUI.ucNameLength = ulLength >> 2;
    }
    else
    {
        //
        // The string will require scrolling, so add "  ***  " to the end.
        //
        sUI.pusName[ulIdx++] = ' ';
        sUI.pusName[ulIdx++] = ' ';
        sUI.pusName[ulIdx++] = '*';
        sUI.pusName[ulIdx++] = '*';
        sUI.pusName[ulIdx++] = '*';
        sUI.pusName[ulIdx++] = ' ';
        sUI.pusName[ulIdx++] = ' ';
        sUI.pusName[ulIdx] = '\0';

        //
        // Get the length of the string, rounding up to the next four bit
        // boundary and dividing by four.
        //
        sUI.ucNameLength = (LCDStringLength((char *)sUI.pusName, 1) + 3) >> 2;
    }

    //
    // Start displaying the track name from the beginning.
    //
    sUI.ucNameOffset = 0;

    //
    // Wait for a bit before the scrolling begins.
    //
    sUI.ucNamePause = 2;

    //
    // When a file is loaded, the current time defaults to zero.
    //
    sUI.ulCurrentTime = 0x80000000;

    //
    // Set the flag to indicate that the track name needs to be updated.
    //
    DisableIRQ();
    sUI.ulFlags |= FLAG_UPDATE_TRACK_NAME;
    EnableIRQ();
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
    sUI.ulCurrentTime = 0;

    //
    // Until another song is loaded, there is no music.
    //
    memcpy(sUI.pusName,
           " \0 \0 \0 \0-\0-\0 \0N\0o\0 \0M\0u\0s\0i\0c\0 \0-\0-\0\0", 38);
    sUI.ucNameOffset = 0;
    sUI.ucNameLength = 0;

    //
    // Set the flag to indicate that the track name needs to be updated.
    //
    DisableIRQ();
    sUI.ulFlags |= FLAG_UPDATE_TRACK_NAME;
    EnableIRQ();
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
    //
    // Convert the millisecond time into seconds.
    //
    ulMS /= 1000;

    //
    // See if the new time is different than the old time.
    //
    if(ulMS != (sUI.ulCurrentTime & 0x7fffffff))
    {
        //
        // The new time is different, so we need to disable interrupts in order
        // to change the time.
        //
        DisableIRQ();

        //
        // Save the update flag on the current time.
        //
        sUI.ulCurrentTime &= 0x80000000;

        //
        // Save the new time.
        //
        sUI.ulCurrentTime |= ulMS;

        //
        // If we are not stopped, then indicate that the time needs to be
        // updated on the LCD.
        //
        if((sUI.ucMode & 0x1f) != MODE_STOP)
        {
            sUI.ulCurrentTime |= 0x80000000;
        }

        //
        // Re-enable interrupts.
        //
        EnableIRQ();
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
// Performs the processing necessary when the target time has been reached.
//
//****************************************************************************
static __inline unsigned long
HandleTimeout(void)
{
    unsigned long ulRet = 0;

    //
    // If we are currently displaying the Cirrus Logic logo, then we need to
    // display the Maverick(tm) logo.
    //
    if((sUI.ulFlags & FLAG_DISPLAY_STATE_MASK) == FLAG_DISPLAY_STATE_LOGO1)
    {
        //
        // Turn off the LCD display while updating the screen.
        //
        LCDOff();

        //
        // Display the Maverick(tm) logo.
        //
        LCDDisplayImage(ucMaverickLogo, 0, 0, 16, 33);

        //
        // Turn the LCD display back on.
        //
        LCDOn();

        //
        // Indicate that we are currently displaying the Maverick(tm) logo.
        //
        sUI.ulFlags &= ~FLAG_DISPLAY_STATE_MASK;
        sUI.ulFlags |= FLAG_DISPLAY_STATE_LOGO2;

        //
        // Set the new target time for the amount of time to display this logo.
        //
        sUI.usTargetTime = sUI.usTickCount + LOGO2_DELAY;
    }

    //
    // If we are currently displaying the Maverick(tm) logo, then we need to
    // change to the main display.
    //
    else if((sUI.ulFlags & FLAG_DISPLAY_STATE_MASK) ==
            FLAG_DISPLAY_STATE_LOGO2)
    {
        //
        // Indicate that we need to update the entire screen.
        //
        ulRet |= FLAG_UPDATE_STATIC;

        //
        // Determine the time at which the backlight should be turned off.
        //
        if((sUI.ulFlags & FLAG_BACKLIGHT_MASK) == FLAG_BACKLIGHT_5)
        {
            //
            // The backlight should be turned off in 5 seconds.
            //
            sUI.usBacklightTime = sUI.usTickCount + 250;
        }
        else if((sUI.ulFlags & FLAG_BACKLIGHT_MASK) == FLAG_BACKLIGHT_10)
        {
            //
            // The backlight should be turned off in 10 seconds.
            //
            sUI.usBacklightTime = sUI.usTickCount + 500;
        }

        //
        // Indicate that we are currently displaying the main screen.
        //
        sUI.ulFlags &= ~FLAG_DISPLAY_STATE_MASK;
        sUI.ulFlags |= FLAG_DISPLAY_STATE_MAIN;
    }

    //
    // If we are displaying the main screen, then we need to fake the press of
    // the power button (i.e. we've been idle for too long).
    //
    else if((sUI.ulFlags & FLAG_DISPLAY_STATE_MASK) == FLAG_DISPLAY_STATE_MAIN)
    {
        //
        // See if we are either stopped or paused.
        //
        if(((sUI.ucMode & 0x1f) == MODE_STOP) ||
           ((sUI.ucMode & 0x1f) == MODE_PAUSE))
        {
            //
            // Indicate that the power button was "pressed".
            //
            sUI.ulKeys |= BUTTON_POWER;
        }
    }

    //
    // If we are displaying the menu, then we should revert back to the main
    // screen.
    //
    else if((sUI.ulFlags & FLAG_DISPLAY_STATE_MASK) == FLAG_DISPLAY_STATE_MENU)
    {
        //
        // Indicate that we need to update the entire screen.
        //
        ulRet |= FLAG_UPDATE_STATIC;

        //
        // Indicate that we are currently displaying the main screen.
        //
        sUI.ulFlags &= ~FLAG_DISPLAY_STATE_MASK;
        sUI.ulFlags |= FLAG_DISPLAY_STATE_MAIN;
    }

    //
    // Return our result.
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
    const unsigned char *pucHoldTimes;

    //
    // Get a pointer to the hold time array based on the current display mode.
    //
    if((sUI.ulFlags & FLAG_DISPLAY_STATE_MASK) == FLAG_DISPLAY_STATE_MAIN)
    {
        pucHoldTimes = ucHoldTimesMain;
    }
    else
    {
        pucHoldTimes = ucHoldTimesMenu;
    }

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
            if(pucHoldTimes[ulIdx] == 0)
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
                pucHoldTimes[ulIdx]) && (pucHoldTimes[ulIdx] != 0))
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
// Updates the target time at which the backlight will be turned off.  Also
// turns on the backlight if it is not currently turned on and should be.
//
//****************************************************************************
static __inline void
HandleBacklight(void)
{
    //
    // Determine the current backlight setting.
    //
    switch(sUI.ulFlags & FLAG_BACKLIGHT_MASK)
    {
        //
        // The backlight should stay on for 5 seconds.
        //
        case FLAG_BACKLIGHT_5:
        {
            //
            // If the backlight is currently off, then turn it on.
            //
            if(!(sUI.ulFlags & FLAG_BACKLIGHT))
            {
                //
                // Turn on the backlight.
                //
                LCDBacklightOn();

                //
                // Indicate that the backlight is on.
                //
                sUI.ulFlags |= FLAG_BACKLIGHT;
            }

            //
            // Set the backlight target time to 5 seconds from now.
            //
            sUI.usBacklightTime = sUI.usTickCount + 250;

            //
            // We're done handling this backlight setting.
            //
            break;
        }

        //
        // The backlight should stay on for 10 seconds.
        //
        case FLAG_BACKLIGHT_10:
        {
            //
            // If the backlight is currently off, then turn it on.
            //
            if(!(sUI.ulFlags & FLAG_BACKLIGHT))
            {
                //
                // Turn on the backlight.
                //
                LCDBacklightOn();

                //
                // Indicate that the backlight is on.
                //
                sUI.ulFlags |= FLAG_BACKLIGHT;
            }

            //
            // Set the backlight time to 10 seconds from now.
            //
            sUI.usBacklightTime = sUI.usTickCount + 500;

            //
            // We're done handling this backlight setting.
            //
            break;
        }
    }
}

//****************************************************************************
//
// Takes care of buttons which behave the same regardless of the state of the
// display.
//
//****************************************************************************
static __inline unsigned long
HandleGlobalButtons(unsigned long ulButtons)
{
    unsigned long ulRet = 0, ulTime;

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

            //
            // Inidicate that the volume indicator needs to be updated.
            //
            ulRet |= FLAG_UPDATE_VOLUME;
        }
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

            //
            // Inidicate that the volume indicator needs to be updated.
            //
            ulRet |= FLAG_UPDATE_VOLUME;
        }
    }

    //
    // Return our result.
    //
    return(ulRet);
}

//****************************************************************************
//
// Takes care of buttons which have specific functions when displaying the
// main screen.
//
//****************************************************************************
static __inline unsigned long
HandleMainButtons(unsigned long ulButtons)
{
    unsigned long ulRet = 0, ulTime;

    //
    // See if the menu button has been pressed.
    //
    if(ulButtons & 0x01)
    {
        //
        // Indicate that we need to update the entire screen.
        //
        ulRet |= FLAG_UPDATE_STATIC;

        //
        // Indicate that we are currently displaying the menu.
        //
        sUI.ulFlags &= ~FLAG_DISPLAY_STATE_MASK;
        sUI.ulFlags |= FLAG_DISPLAY_STATE_MENU;
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
    // See if the loopback button has been pressed.
    //
#ifdef REQUIRE_LOOPBACK
    ulTime = (sUI.usTickCount - sUI.usPressTime[2]) & 0xffff;
    if((sUI.ucState & 0x04) && (ulTime >= 50))
    {
        //
        // Indicate that the loopback button was pressed.
        //
        sUI.ulKeys |= BUTTON_LOOPBACK;
    }
#endif

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

            //
            // See if the rewind button has been held long enough to accelerate
            // the seek.
            //
            if(ulTime >= SEEK3_ACCEL)
            {
                //
                // The rewind button has been held long enough to bump up to
                // the third acceleration rate.
                //
                sUI.ulKeys |= BUTTON_SEEK_RATE_ACC3;
            }
            else if(ulTime >= SEEK2_ACCEL)
            {
                //
                // The rewind button has been held long enough to bump up to
                // the second acceleration rate.
                //
                sUI.ulKeys |= BUTTON_SEEK_RATE_ACC2;
            }
            else if(ulTime >= SEEK1_ACCEL)
            {
                //
                // The rewind button has been held long enough to bump up to
                // the first acceleration rate.
                //
                sUI.ulKeys |= BUTTON_SEEK_RATE_ACC1;
            }

            //
            // If the seek indicator has not been displayed on the screen yet,
            // then do so now.
            //
            if(!(sUI.ucMode & FLAG_SEEK_BACKWARD))
            {
                //
                // Indicate that the seek backward indicator should be
                // displayed, and that the mode indicator needs to be updated.
                //
                sUI.ucMode |= 0x80 | FLAG_SEEK_BACKWARD;
            }
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

            //
            // See if the fast foward button has been held long enough to
            // accelerate the seek.
            //
            if(ulTime >= SEEK3_ACCEL)
            {
                //
                // The fast forward button has been held long enough to bump up
                // to the third acceleration rate.
                //
                sUI.ulKeys |= BUTTON_SEEK_RATE_ACC3;
            }
            else if(ulTime >= SEEK2_ACCEL)
            {
                //
                // The fast forward button has been held long enough to bump up
                // to the second acceleration rate.
                //
                sUI.ulKeys |= BUTTON_SEEK_RATE_ACC2;
            }
            else if(ulTime >= SEEK1_ACCEL)
            {
                //
                // The fast forward button has been held long enough to bump up
                // to the first acceleration rate.
                //
                sUI.ulKeys |= BUTTON_SEEK_RATE_ACC1;
            }

            //
            // If the seek indicator has not been displayed on the screen yet,
            // then do so now.
            //
            if(!(sUI.ucMode & FLAG_SEEK_FORWARD))
            {
                //
                // Indicate that the seek forward indicator should be
                // displayed, and that the mode indicator needs to be updated.
                //
                sUI.ucMode |= 0x80 | FLAG_SEEK_FORWARD;
            }
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
    // See if the record button has been pressed.
    //
#ifdef REQUIRE_RECORD
    ulTime = (sUI.usTickCount - sUI.usPressTime[6]) & 0xffff;
    if((sUI.ucState & 0x40) && (ulTime >= RECORD_DELAY))
    {
        //
        // Indicate that the record button was pressed.
        //
        sUI.ulKeys |= BUTTON_RECORD;
    }
#endif

    //
    // We need to move the target time so that we do not get an automatic
    // power off.
    //
    sUI.usTargetTime = sUI.usTickCount +
                       (3000 * (((sUI.ulFlags & FLAG_SLEEP_MASK) >>
                                 FLAG_SLEEP_SHIFT) + 1));

    //
    // Return our result.
    //
    return(ulRet);
}

//****************************************************************************
//
// Takes care of buttons which have specific functions when displaying the
// menu screen.
//
//****************************************************************************
static __inline unsigned long
HandleMenuButtons(unsigned long ulButtons)
{
    unsigned long ulRet = 0, ulValue, ulMenuItem;

    //
    // See if the menu button has been pressed.
    //
    if(ulButtons & 0x01)
    {
        //
        // Indicate that we need to update the entire screen.
        //
        ulRet |= FLAG_UPDATE_STATIC;

        //
        // Indicate that we are currently displaying the main screen.
        //
        sUI.ulFlags &= ~FLAG_DISPLAY_STATE_MASK;
        sUI.ulFlags |= FLAG_DISPLAY_STATE_MAIN;
    }

    //
    // See if the up arrow has been pressed.
    //
    ulValue = (sUI.usTickCount - sUI.usPressTime[1]) & 0xffff;
    if((sUI.ucState & 0x02) &&
       ((ulButtons & 0x020000) ||
        ((ulValue >= MENU_DELAY) && ((ulValue % MENU_UD_ACCEL) == 0))))
    {
        //
        // Get the current menu item number.
        //
        ulMenuItem = (sUI.ulFlags & FLAG_MENU_ITEM_MASK) >>
                     FLAG_MENU_ITEM_SHIFT;

        //
        // Get the current value of this menu item.
        //
        ulValue = (*(sMenuItems[ulMenuItem].pulFlags) &
                   sMenuItems[ulMenuItem].ulFlagMask) >>
                  sMenuItems[ulMenuItem].ulFlagShift;

        //
        // If the current value is not at the maximum, then we can increment
        // it.
        //
        if(ulValue < (sMenuItems[ulMenuItem].ulNumValues - 1))
        {
            //
            // Increment the current value.
            //
            ulValue++;

            //
            // Save the new value.
            //
            *(sMenuItems[ulMenuItem].pulFlags) &=
                ~sMenuItems[ulMenuItem].ulFlagMask;
            *(sMenuItems[ulMenuItem].pulFlags) |=
                ulValue << sMenuItems[ulMenuItem].ulFlagShift;

            //
            // See if the menu item has a routine for effecting the change in
            // other parts of the player.
            //
            if(sMenuItems[ulMenuItem].pfnUpdate)
            {
                //
                // Reflect the new menu item value in the rest of the player.
                //
                (sMenuItems[ulMenuItem].pfnUpdate)(ulValue);
            }

            //
            // Indicate that we need to update the value display on the LCD.
            //
            ulRet |= FLAG_UPDATE_VALUE;
        }
    }

    //
    // See if the down arrow has been pressed.
    //
    ulValue = (sUI.usTickCount - sUI.usPressTime[2]) & 0xffff;
    if((sUI.ucState & 0x04) &&
       ((ulButtons & 0x040000) ||
        ((ulValue >= MENU_DELAY) && ((ulValue % MENU_UD_ACCEL) == 0))))
    {
        //
        // Get the current menu item number.
        //
        ulMenuItem = (sUI.ulFlags & FLAG_MENU_ITEM_MASK) >>
                     FLAG_MENU_ITEM_SHIFT;

        //
        // Get the current value of this menu item.
        //
        ulValue = (*(sMenuItems[ulMenuItem].pulFlags) &
                   sMenuItems[ulMenuItem].ulFlagMask) >>
                  sMenuItems[ulMenuItem].ulFlagShift;

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
            *(sMenuItems[ulMenuItem].pulFlags) &=
                ~sMenuItems[ulMenuItem].ulFlagMask;
            *(sMenuItems[ulMenuItem].pulFlags) |=
                ulValue << sMenuItems[ulMenuItem].ulFlagShift;

            //
            // See if the menu item has a routine for effecting the change in
            // other parts of the player.
            //
            if(sMenuItems[ulMenuItem].pfnUpdate)
            {
                //
                // Reflect the new menu item value in the rest of the player.
                //
                (sMenuItems[ulMenuItem].pfnUpdate)(ulValue);
            }

            //
            // Indicate that we need to update the value display on the LCD.
            //
            ulRet |= FLAG_UPDATE_VALUE;
        }
    }

    //
    // See if the left arrow has been pressed.
    //
    ulValue = (sUI.usTickCount - sUI.usPressTime[3]) & 0xffff;
    if((sUI.ucState & 0x08) &&
       ((ulButtons & 0x080000) ||
        ((ulValue >= MENU_DELAY) && ((ulValue % MENU_LR_ACCEL) == 0))))
    {
        //
        // Get the current menu item number.
        //
        ulMenuItem = (sUI.ulFlags & FLAG_MENU_ITEM_MASK) >>
                     FLAG_MENU_ITEM_SHIFT;

        //
        // If the current menu item number is not zero, then we can decrement
        // it.
        //
        if(ulMenuItem != 0)
        {
            //
            // Skip to the previous menu item.
            //
            ulMenuItem--;

            //
            // Save the new menu item number.
            //
            sUI.ulFlags &= ~FLAG_MENU_ITEM_MASK;
            sUI.ulFlags |= ulMenuItem << FLAG_MENU_ITEM_SHIFT;

            //
            // Indicate that we need to update the menu.
            //
            ulRet |= FLAG_UPDATE_MENU;
        }
    }

    //
    // See if the right arrow has been pressed.
    //
    ulValue = (sUI.usTickCount - sUI.usPressTime[4]) & 0xffff;
    if((sUI.ucState & 0x10) &&
       ((ulButtons & 0x100000) ||
        ((ulValue >= MENU_DELAY) && ((ulValue % MENU_LR_ACCEL) == 0))))
    {
        //
        // Get the current menu item number.
        //
        ulMenuItem = (sUI.ulFlags & FLAG_MENU_ITEM_MASK) >>
                     FLAG_MENU_ITEM_SHIFT;

        //
        // If the current menu item number is not the maximum, then we can
        // increment it.
        //
        if(sMenuItems[ulMenuItem + 1].pfnDisplay != 0)
        {
            //
            // Skip to the next menu item.
            //
            ulMenuItem++;

            //
            // Save the new menu item number.
            //
            sUI.ulFlags &= ~FLAG_MENU_ITEM_MASK;
            sUI.ulFlags |= ulMenuItem << FLAG_MENU_ITEM_SHIFT;

            //
            // Indicate that we need to update the menu.
            //
            ulRet |= FLAG_UPDATE_MENU;
        }
    }

    //
    // We need to move the target time so that we do not automatically exit
    // the menu.
    //
    sUI.usTargetTime = sUI.usTickCount + MENU_EXIT_DELAY;

    //
    // Return our result.
    //
    return(ulRet);
}

//****************************************************************************
//
// Updates the appropriate portions of the main screen.
//
//****************************************************************************
static __inline void
UpdateMainDisplay(unsigned long ulUpdateFlags)
{
    unsigned long ulIdx, ulTime, ulLength;
    char cStr[6];

    //
    // Turn off the LCD while we are updating the display.
    //
    LCDOff();

    //
    // See if we need to update the static portion of the main display.
    //
    if(ulUpdateFlags & FLAG_UPDATE_STATIC)
    {
        //
        // Draw the track, time, and volume images on the LCD.
        //
        LCDDisplayImage(ucMainImage, 0, 0, 16, 33);

        //
        // We also need to update the icon, track information, time, and
        // volume.
        //
        ulUpdateFlags |= (FLAG_UPDATE_ICON | FLAG_UPDATE_TRACK |
                          FLAG_UPDATE_TIME | FLAG_UPDATE_VOLUME |
                          FLAG_UPDATE_VISUAL);
    }

    //
    // See if we need to update the mode indicator icon.
    //
    if(ulUpdateFlags & FLAG_UPDATE_ICON)
    {
        //
        // Figure out what to display based on the current mode.
        //
        switch(sUI.ucMode)
        {
            //
            // We are stopped.
            //
            case MODE_STOP:
            {
                //
                // Display the stop icon.
                //
                LCDDisplayImage((unsigned char *)ucStop, 0, 2, 2, 9);

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
                // Display the play icon.
                //
                LCDDisplayImage((unsigned char *)ucPlay, 0, 2, 2, 9);

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
                // Display the pause icon.
                //
                LCDDisplayImage((unsigned char *)ucPause, 0, 2, 2, 9);

                //
                // We're done with this state.
                //
                break;
            }

            //
            // We are seeking forward in a file.
            //
            case MODE_PLAY | FLAG_SEEK_FORWARD:
            case MODE_PAUSE | FLAG_SEEK_FORWARD:
            {
                //
                // Display the fast forward icon.
                //
                LCDDisplayImage((unsigned char *)ucFastForward, 0, 2, 2, 9);

                //
                // We're done with this state.
                //
                break;
            }

            //
            // We are seeking backward in a file.
            //
            case MODE_PLAY | FLAG_SEEK_BACKWARD:
            case MODE_PAUSE | FLAG_SEEK_BACKWARD:
            {
                //
                // Display the fast backward icon.
                //
                LCDDisplayImage((unsigned char *)ucFastBackward, 0, 2, 2, 9);

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
                // Display the record icon.
                //
                LCDDisplayImage((unsigned char *)ucRecord, 0, 2, 2, 9);

                //
                // We're done with this state.
                //
                break;
            }

            //
            // We are in loopback mode.
            //
#ifdef REQUIRE_LOOPBACK
            case MODE_LOOPBACK:
            case MODE_LOOPBACK | FLAG_SEEK_FORWARD:
            case MODE_LOOPBACK | FLAG_SEEK_BACKWARD:
            {
                //
                // Display the loopback icon.
                //
                LCDDisplayImage((unsigned char *)ucLoop, 0, 2, 2, 9);

                //
                // We're done with this state.
                //
                break;
            }
#endif
        }
    }

    //
    // See if we need to update the track information.
    //
    if(ulUpdateFlags & FLAG_UPDATE_TRACK)
    {
        //
        // See if the track name needs to be scrolled.
        //
        if(sUI.ucNameLength < 27)
        {
            //
            // The track name is not scrolled, so simply display it.
            //
            LCDPrintString((char *)sUI.pusName, 1, 0, 2, 2, 16, 0, 0, 0,
                           ucMainImage + 497, 4);
        }
        else
        {
            //
            // Display the track name with scrolling, at the current scroll
            // offset.
            //
            LCDPrintString((char *)sUI.pusName, 1, 1, 2, 2, 16,
                           sUI.ucNameOffset << 2, 0, 2, ucMainImage + 497, 4);
        }
    }

    //
    // See if we need to update the song time.
    //
    if(ulUpdateFlags & FLAG_UPDATE_TIME)
    {
        //
        // Get the length of the current file.
        //
        if(CodecGetLength(&ulLength) == 0)
        {
            ulLength = 0;
        }

        //
        // Convert the length from milliseconds to seconds.
        //
        ulLength = (ulLength + 500) / 1000;

        //
        // Get the the current time.
        //
        ulTime = sUI.ulCurrentTime;

        //
        // Convert the length into a character representation.  A truncated
        // version of HH:MM:SS is used (i.e. M:SS for a time less than 10
        // mintues, MM:SS for a time less than 1 hour, etc.).
        //
        ulIdx = 0;
        if(ulLength >= 36000)
        {
            cStr[ulIdx++] = '0' + ((ulTime / 36000) % 10);
        }
        if(ulLength >= 3600)
        {
            cStr[ulIdx++] = '0' + ((ulTime / 3600) % 10);
            cStr[ulIdx++] = ':';
        }
        if(ulLength >= 600)
        {
            cStr[ulIdx++] = '0' + ((ulTime / 600) % 6);
        }
        cStr[ulIdx++] = '0' + ((ulTime / 60) % 10);
        cStr[ulIdx++] = ':';
        cStr[ulIdx++] = '0' + ((ulTime / 10) % 6);
        cStr[ulIdx++] = '0' + (ulTime % 10);
        cStr[ulIdx++] = '\0';

        //
        // Draw the time on the LCD.
        //
        if(ulLength >= 36000)
        {
            LCDPrintString(cStr, 0, 0, 7, 15, 13, 0, 0, 2, 0, 0);
        }
        else if(ulLength >= 3600)
        {
            LCDPrintString(cStr, 0, 0, 7, 15, 13, 0, 0, 6, 0, 0);
        }
        else if(ulLength >= 600)
        {
            LCDPrintString(cStr, 0, 0, 7, 15, 13, 0, 0, 12, 0, 0);
        }
        else
        {
            LCDPrintString(cStr, 0, 0, 7, 15, 13, 0, 0, 16, 0, 0);
        }

        //
        // Update the track bar to indicate the current time.
        //
        UpdateTrackBar(ulTime, ulLength);
    }

    //
    // See if we need to update the volume.
    //
    if(ulUpdateFlags & FLAG_UPDATE_VOLUME)
    {
        //
        // Get the current volume.
        //
        ulIdx = (sUI.ulFlags & FLAG_VOLUME_MASK) >> FLAG_VOLUME_SHIFT;

        //
        // Display the appropriate volume indicator based on the current volume
        // setting.
        //
        LCDDisplayImage(ucVolume[ulIdx], 13, 15, 3, 9);
    }

    //
    // See if we need to update the visual area.
    //
    if(ulUpdateFlags & FLAG_UPDATE_VISUAL)
    {
        //
        // See if we are stopped.
        //
        if(sUI.ucMode == MODE_STOP)
        {
            //
            // There is no visual when we are stopped.
            //
            LCDDisplayImage((unsigned char *)ucVisualBackground, 0, 17, 7, 10);
        }
        else
        {
            //
            // Determine what to do based on the visual mode.
            //
            switch(sUI.ulFlags & FLAG_VISUAL_MODE_MASK)
            {
                //
                // The waveform visual is selected.
                //
                case FLAG_VISUAL_MODE_WAVEFORM:
                {
                    //
                    // Update the waveform display.
                    //
                    UpdateWaveform();

                    //
                    // We're done with this visual.
                    //
                    break;
                }

#ifdef SUPPORT_SPECTRUM_ANALYZER
                //
                // The spectrum analyzer visual is selected.
                //
                case FLAG_VISUAL_MODE_SPECTRUM:
                {
                    //
                    // Update the spectrum analyzer.
                    //
                    UpdateSpectrum();

                    //
                    // We're done with this visual.
                    //
                    break;
                }
#endif
            }
        }
    }

    //
    // Turn on the LCD.
    //
    LCDOn();

    //
    // Set the new target time for automatically powering down the player.
    //
    if((ulUpdateFlags & ~(FLAG_UPDATE_TRACK | FLAG_UPDATE_VISUAL)) != 0)
    {
        sUI.usTargetTime = (sUI.usTickCount +
                            (3000 * (((sUI.ulFlags & FLAG_SLEEP_MASK) >>
                                      FLAG_SLEEP_SHIFT) + 1)));
    }
}

//****************************************************************************
//
// Updates the appropriate portions of the menu screen.
//
//****************************************************************************
static __inline void
UpdateMenuDisplay(unsigned long ulUpdateFlags)
{
    unsigned long ulMenuItem;

    //
    // Get the index of the current menu item.
    //
    ulMenuItem = (sUI.ulFlags & FLAG_MENU_ITEM_MASK) >> FLAG_MENU_ITEM_SHIFT;

    //
    // Turn off the LCD while we are updating the display.
    //
    LCDOff();

    //
    // See if we need to draw the border around the bottom portion of the
    // menu item.
    //
    if(ulUpdateFlags & FLAG_UPDATE_STATIC)
    {
        //
        // Draw the border around the bottom portion of the menu item.
        //
        LCDDisplayImage(ucMenuBottom, 0, 13, 16, 20);

        //
        // We also need to update the menu.
        //
        ulUpdateFlags |= FLAG_UPDATE_MENU;
    }

    //
    // See if we need to update the menu items across the top of the LCD.
    //
    if(ulUpdateFlags & FLAG_UPDATE_MENU)
    {
        //
        // Draw the menu items across the top of the LCD.
        //
        LCDDrawMenu(0, 0, pcMenu, ulMenuItem);

        //
        // We also need to update the value of the current menu item.
        //
        ulUpdateFlags |= FLAG_UPDATE_VALUE;
    }

    //
    // See if we need to update the value of the current menu item in the main
    // body of the menu.
    //
    if(ulUpdateFlags & FLAG_UPDATE_VALUE)
    {
        //
        // Draw the current value of the current menu item.
        //
        (sMenuItems[ulMenuItem].pfnDisplay)(ulMenuItem);
    }

    //
    // Turn on the LCD.
    //
    LCDOn();

    //
    // Set the new target time for automatically exiting the menu.
    //
    sUI.usTargetTime = sUI.usTickCount + MENU_EXIT_DELAY;
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
    unsigned long ulButtons, ulUpdateFlags = 0;

    //
    // Increment the number of timer ticks which have occurred.
    //
    sUI.usTickCount++;

    //
    // Update the state of the battery monitor when appropriate.
    //
    if((sUI.usTickCount % BATTERY_DELAY) == 0)
    {
        LCDUpdateAnnunciators();
    }

    //
    // If the tick count has reached the target time, see what we need to do.
    //
    if(sUI.usTickCount == sUI.usTargetTime)
    {
        //
        // Handle the expired timer.
        //
        ulUpdateFlags |= HandleTimeout();
    }

    //
    // If we are displaying either the Cirrus Logic logo or the Maverick(tm)
    // logo, then we should do nothing further.
    //
    if(((sUI.ulFlags & FLAG_DISPLAY_STATE_MASK) == FLAG_DISPLAY_STATE_LOGO1) ||
       ((sUI.ulFlags & FLAG_DISPLAY_STATE_MASK) == FLAG_DISPLAY_STATE_LOGO2))
    {
        return;
    }

    //
    // If we have reached the backlight target time and the backlight should
    // not be on all the time, then turn it off.
    //
    if((sUI.usTickCount == sUI.usBacklightTime) &&
       ((sUI.ulFlags & FLAG_BACKLIGHT_MASK) != FLAG_BACKLIGHT_ON) &&
       (sUI.ulFlags & FLAG_BACKLIGHT))
    {
        //
        // Turn off the backlight.
        //
        LCDBacklightOff();

        //
        // Indicate that the backlight is off.
        //
        sUI.ulFlags &= ~FLAG_BACKLIGHT;
    }

    //
    // Determine which buttons are pressed and which have been released.
    //
    ulButtons = ScanButtons();

    //
    // If the seek icon is being displayed and the seek button was released,
    // then the original icon should be displayed instead.
    //
    if((ulButtons & 0x18000000) &&
       (sUI.ucMode & (FLAG_SEEK_FORWARD | FLAG_SEEK_BACKWARD)))
    {
        //
        // Turn off the seek icon indicator.
        //
        sUI.ucMode &= ~(FLAG_SEEK_FORWARD | FLAG_SEEK_BACKWARD);

        //
        // Indicate that the icon needs to be updated.
        //
        sUI.ucMode |= 0x80;
    }

    //
    // See if any buttons are being pressed.
    //
    if(sUI.ucState || ulButtons)
    {
        //
        // Make sure that the backlight is on if necessary.
        //
        HandleBacklight();

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

            //
            // If one of the seek indicators is currently being displayed, then
            // make sure it is cleared.
            //
            if(sUI.ucMode & (FLAG_SEEK_FORWARD | FLAG_SEEK_BACKWARD))
            {
                //
                // Clear the seek indicator.
                //
                sUI.ucMode &= ~(FLAG_SEEK_FORWARD | FLAG_SEEK_BACKWARD);

                //
                // Indicate that the icon needs to be updated.
                //
                sUI.ucMode |= 0x80;
            }
        }

        //
        // See if we are supposed to wait for all buttons to be released.
        //
        if(!(sUI.ulFlags & FLAG_WAIT_FOR_ALL_UP))
        {
            //
            // There is only one button pressed, so handle the buttons which
            // do not depend on the display mode.
            //
            ulUpdateFlags |= HandleGlobalButtons(ulButtons);

            //
            // The handling of the remainder of the buttons is dependent upon
            // the current display mode.  Determine the mode we are currently
            // in.
            //
            if((sUI.ulFlags & FLAG_DISPLAY_STATE_MASK) ==
               FLAG_DISPLAY_STATE_MAIN)
            {
                //
                // Handle the main mode buttons.
                //
                ulUpdateFlags |= HandleMainButtons(ulButtons);
            }
            else
            {
                //
                // Handle the menu mode buttons.
                //
                ulUpdateFlags |= HandleMenuButtons(ulButtons);
            }
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

    //
    // Now that we've handled the buttons, we need to make sure that the LCD
    // is up to date.  See if we are displaying the main screen or the menu.
    //
    if((sUI.ulFlags & FLAG_DISPLAY_STATE_MASK) == FLAG_DISPLAY_STATE_MAIN)
    {
        //
        // We are displaying the main screen.  See if the entire LCD needs to
        // be updated.
        //
        if(sUI.ulFlags & FLAG_UPDATE_SCREEN)
        {
            //
            // The entire screen needs to be updated, so add that to the update
            // flags and clear the flag in the global flags.
            //
            sUI.ulFlags &= ~FLAG_UPDATE_SCREEN;
            ulUpdateFlags |= FLAG_UPDATE_STATIC;
        }

        //
        // See if the mode indicator needs to be updated.
        //
        if(sUI.ucMode & 0x80)
        {
            //
            // The mode indicator needs to be updated, so add that to the
            // update flags and clear the update indicator.
            //
            sUI.ucMode &= ~0x80;
            ulUpdateFlags |= (FLAG_UPDATE_ICON | FLAG_UPDATE_TIME |
                              FLAG_UPDATE_VISUAL);
        }

        //
        // See if the track name needs to be updated.
        //
        if(sUI.ulFlags & FLAG_UPDATE_TRACK_NAME)
        {
            //
            // The track name needs to be updated, so add that to the update
            // flags and clear the update indicator.
            //
            sUI.ulFlags &= ~FLAG_UPDATE_TRACK_NAME;
            ulUpdateFlags |= FLAG_UPDATE_TRACK;
        }

        //
        // See if it is time to scroll the track name.
        //
        else if(((sUI.usTickCount % SCROLL_DELAY) == 0) &&
                (sUI.ucNameLength > 26))
        {
            //
            // See if we are currently pausing the scroll.
            //
            if(sUI.ucNamePause)
            {
                //
                // Decrement the number of ticks to wait before we start
                // scrolling.
                //
                sUI.ucNamePause--;
            }
            else
            {
                //
                // Increment the scroll offset.
                //
                sUI.ucNameOffset++;

                //
                // If we've completely scrolled the track name, then start back
                // at the beginning.
                //
                if(sUI.ucNameOffset == sUI.ucNameLength)
                {
                    sUI.ucNameOffset = 0;
                }

                //
                // Indicate that we need to update the track name.
                //
                ulUpdateFlags |= FLAG_UPDATE_TRACK;
            }
        }

        //
        // See if the current time needs to be updated.
        //
        if(sUI.ulCurrentTime & 0x80000000)
        {
            //
            // The current time needs to be updated, so add that to the update
            // flags and clear the update indicator.
            //
            sUI.ulCurrentTime &= ~0x80000000;
            ulUpdateFlags |= FLAG_UPDATE_TIME;
        }

        //
        // See if the visual area needs to be updated.
        //
        if(((sUI.usTickCount % VISUAL_DELAY) == 0) &&
           ((sUI.ucMode & 0x1f) != MODE_STOP))
        {
            //
            // Indicate that the visual area needs to be updated.
            //
            ulUpdateFlags |= FLAG_UPDATE_VISUAL;
        }

        //
        // See if anything on the main screen needs to be updated.
        //
        if(ulUpdateFlags & (FLAG_UPDATE_STATIC | FLAG_UPDATE_ICON |
                            FLAG_UPDATE_TRACK | FLAG_UPDATE_TIME |
                            FLAG_UPDATE_VOLUME | FLAG_UPDATE_VISUAL))
        {
            //
            // Update the appropriate portions of the main screen.
            //
            UpdateMainDisplay(ulUpdateFlags);
        }
    }
    else
    {
        //
        // See if anything on the menu screen needs to be updated.
        //
        if(ulUpdateFlags & (FLAG_UPDATE_STATIC | FLAG_UPDATE_MENU |
                            FLAG_UPDATE_VALUE))
        {
            //
            // Update the appropriate portions of the menu screen.
            //
            UpdateMenuDisplay(ulUpdateFlags);
        }
    }
}
#endif

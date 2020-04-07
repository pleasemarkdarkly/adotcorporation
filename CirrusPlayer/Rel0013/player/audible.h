//****************************************************************************
//
// AUDIBLE.H - Definition of the structure in AUDIBLE.DAT which contains the
//             Audible meta-data for all Audible programs on the Internet
//             Audio Player.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
//
//****************************************************************************

//****************************************************************************
//
// This structure defines the block of meta-data contained in AUDIBLE.DAT.
//
//****************************************************************************
typedef struct
{
    //
    // The title of this program.
    //
    char cTitle[256];

    //
    // The product ID of this program.
    //
    char cProductID[80];

    //
    // The parent product ID of this program (used for subscriptions).
    //
    char cParentProductID[80];

    //
    // Indicates if this program is a subscription.
    //
    unsigned char ucSubscription;

    //
    // Indicates the update mode for non-subscription programs.
    //
    unsigned char ucUpdateModeSingle;

    //
    // Indicates the update mode for subscription programs.
    //
    unsigned char ucUpdateModeRecurring;

    //
    // Indicates that the program has been played through.
    //
    unsigned char ucPlayedThrough;

    //
    // The starting time of the portion of the program contained in this file,
    // specified in milliseconds.
    //
    unsigned long ulProgramStart;

    //
    // The amount of time in the portion of the program contained in this file,
    // specified in milliseconds.
    //
    unsigned long ulAllocatedTime;

    //
    // The amount of time allocated in the file system for this file, specified
    // in milliseconds.
    //
    unsigned long ulRequestedTime;

    //
    // The current play offset into the protion of the program contained in
    // this file, specified in milliseconds.
    //
    unsigned long ulCurrentPosition;

    //
    // The number of sections in the portion of the program contained in this
    // file.
    //
    unsigned long ulSections;

    //
    // The time offset of each section in the portion of the program contained
    // in this file, specified in milliseconds.
    //
    unsigned long ulSectionStart[1];
} tAudibleMetaData;

//****************************************************************************
//
// MAIN.C - The main Internet audio player application.
//
// Copyright (c) 1999-2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include "globals.h"

//****************************************************************************
//
// An array containing information about the various tracks on the media.
//
//****************************************************************************
unsigned short pusTracks[MAX_TRACKS];

//****************************************************************************
//
// The current track.
//
//****************************************************************************
unsigned long ulTrack;

//****************************************************************************
//
// ScanAllTracks determines which files contain audio which we are able to
// decode.
//
//****************************************************************************
void
ScanAllTracks(unsigned long ulNumTracks, unsigned long ulDrive)
{
    unsigned short usFileName[64];
    tDir sDir;
    tFile sFile;
    unsigned long ulFileNum, ulCodec;

    //
    // Loop through all the available drives.
    //
    for(; ulDrive < FSNumDrives(); ulDrive++)
    {
        //
        // Open the root directory of the current drive.
        //
        if(FSOpenDir(&sDir, ulDrive, (unsigned short *)"\\\0\0") == 0)
        {
            continue;
        }

        //
        // Scan through all the files in the root directory.
        //
        ulFileNum = 0;
        while(1)
        {
            //
            // Read the name of the next file in the directory.
            //
            if(FSReadDir(&sDir, usFileName, FS_TYPE_FILE) == 0)
            {
                break;
            }

            //
            // Open this file.
            //
            if(FSOpen(&sFile, ulDrive, usFileName,
                      FS_OPEN_READ | FS_OPEN_DECRYPT) == 0)
            {
                //
                // Go to the next track.
                //
                ulFileNum++;
                continue;
            }

            //
            // See if we are able to decode this file.
            //
            if(CodecOpen(-1, CODEC_OPEN_DECODE, &sFile) == 1)
            {
                //
                // Save the index of the codec which is able to decode this
                // file.
                //
                CodecGetIndex(&ulCodec);

                //
                // Save the track information.
                //
                pusTracks[ulNumTracks++] =
                    ((ulDrive << TRACK_DRIVE_SHIFT) & TRACK_DRIVE_MASK) |
                    ((ulCodec << TRACK_CODEC_SHIFT) & TRACK_CODEC_MASK) |
                    ((ulFileNum << TRACK_FILE_SHIFT) & TRACK_FILE_MASK);

                //
                // Close the codec.
                //
                CodecClose();
            }

            //
            // Close this file.
            //
            FSClose(&sFile);

            //
            // Go to the next file.
            //
            ulFileNum++;

            //
            // If we've found all the tracks that we can handle, then stop
            // looking.
            //
            if(ulNumTracks == (MAX_TRACKS - 1))
            {
                break;
            }
        }

        //
        // Close the directory.
        //
        FSCloseDir(&sDir);

        //
        // If we've found all the tracks that we can handle, then stop looking.
        //
        if(ulNumTracks == (MAX_TRACKS - 1))
        {
            break;
        }
    }

    //
    // Clear the entries for the remaining tracks.
    //
    for(; ulNumTracks < MAX_TRACKS; ulNumTracks++)
    {
        pusTracks[ulNumTracks] = 0xffff;
    }
}

//****************************************************************************
//
// MediaRemoved handles the removal of the external media.
//
//****************************************************************************
void
MediaRemoved(void)
{
    unsigned long ulTrack;

    //
    // Loop through our entire track listing.
    //
    for(ulTrack = 0; ulTrack < MAX_TRACKS; ulTrack++)
    {
        //
        // See if this track is on the external media.
        //
        if(pusTracks[ulTrack] & TRACK_DRIVE_MASK)
        {
            //
            // This track is on the external media, so stop looking.
            //
            break;
        }
    }

    //
    // Clear the remaining tracks from our list.
    //
    for(; ulTrack < MAX_TRACKS; ulTrack++)
    {
        pusTracks[ulTrack] = 0xffff;
    }

    //
    // Indicate that we've handled the removal of the media.
    //
    DisableIRQ();
    ulSystemFlags &= ~SYSFLAG_MEDIA_REMOVE;
    EnableIRQ();
}

//****************************************************************************
//
// MediaInserted handles the insertion of the external media.
//
//****************************************************************************
void
MediaInserted(void)
{
    unsigned long ulTrack;

    //
    // Loop through our entire track listing.
    //
    for(ulTrack = 0; ulTrack < MAX_TRACKS; ulTrack++)
    {
        //
        // See if this track exists.
        //
        if(pusTracks[ulTrack] == 0xffff)
        {
            //
            // This track does not exist, so stop looking.
            //
            break;
        }
    }

    //
    // Scan the files on the removable media.
    //
    ScanAllTracks(ulTrack, 1);

    //
    // Indicate that we've handled the insertion of the media.
    //
    DisableIRQ();
    ulSystemFlags &= ~SYSFLAG_MEDIA_INSERT;
    EnableIRQ();
}

//****************************************************************************
//
// The main loop for the Internet audio player.  This can not return since it
// is branched to by vectors.s.
//
//****************************************************************************
void
entry(void)
{
    unsigned long ulButtons;

    //
    // Scan all the files.
    //
    ScanAllTracks(0, 0);

    //
    // Clear the initial removeable media insert flag.
    //
    DisableIRQ();
    ulSystemFlags &= ~SYSFLAG_MEDIA_INSERT;
    EnableIRQ();

    //
    // We start from the first track.
    //
    ulTrack = 0;

    //
    // Main application loop.  We repeat this forever since we can not return.
    //
    while(1)
    {
        //
        // See if the external media has been inserted.
        //
        if(ulSystemFlags & SYSFLAG_MEDIA_INSERT)
        {
            //
            // Handle the insert of the external media.
            //
            MediaInserted();
        }

        //
        // See if the external media has been removed.
        //
        if(ulSystemFlags & SYSFLAG_MEDIA_REMOVE)
        {
            //
            // Handle the removal of the external media.
            //
            MediaRemoved();

            //
            // If the current track was on the external media, then go back to
            // the first track.
            //
            if(pusTracks[ulTrack] == 0xffff)
            {
                ulTrack = 0;
            }
        }

        //
        // If there is at least one track, then go into play mode.
        //
        if(pusTracks[ulTrack] != 0xffff)
        {
            //
            // Handle play mode and possibly receive some virtual buttons back.
            //
            ulButtons = Play();
        }
        else
        {
            //
            // Put the EP7209 into HALT mode.
            //
            Halt();

            //
            // Get the current set of pressed virtual buttons.
            //
            ulButtons = UIGetButtons();
        }

        //
        // Is the record button being pressed?
        //
#ifdef REQUIRE_RECORD
        if(ulButtons & BUTTON_RECORD)
        {
            //
            // Perform the record.
            //
            ulButtons = Record();

            //
            // If there are no tracks, then tell the user interface that we
            // are in the stopped state.
            //
            if(pusTracks[ulTrack] == 0xffff)
            {
                UISetMode(MODE_STOP);
            }
        }
#endif

        //
        // Is the loopback button being pressed?
        //
#ifdef REQUIRE_LOOPBACK
        if(ulButtons & BUTTON_LOOPBACK)
        {
            //
            // Perform the loopback.
            //
            ulButtons = Loop();

            //
            // If there are no tracks, then tell the user interface that we
            // are in the stopped state.
            //
            if(pusTracks[ulTrack] == 0xffff)
            {
                UISetMode(MODE_STOP);
            }
        }
#endif

        //
        // Is a download being requested?
        //
        if(ulSystemFlags & SYSFLAG_START_DOWNLOAD)
        {
            //
            // Inform the user interface that we are downloading.
            //
            UISetMode(MODE_DOWNLOAD);

            //
            // Download the new data.
            //
            USBDownload((unsigned char *)ulEndOfRAM);

            //
            // Scan all the files.
            //
            ScanAllTracks(0, 0);

            //
            // After a download, we always revert to the first track.
            //
            ulTrack = 0;

            //
            // If there are no tracks, then tell the user interface that we
            // are in the stopped state.
            //
            if(pusTracks[ulTrack] == 0xffff)
            {
                UISetMode(MODE_STOP);
            }
        }

        //
        // Is the power button being pressed?
        //
        else if(ulButtons & BUTTON_POWER)
        {
            //
            // Disable the USB interface.
            //
            USBDisable();

            //
            // Turn off the IRQ interrupt.
            //
            DisableIRQ();

            //
            // Inform the user interface that we are powering off.
            //
            UISetMode(MODE_POWER_OFF);

            //
            // Put the EP7209 into STANDBY mode.
            //
            Standby();

            //
            // Scan all the files.
            //
            ScanAllTracks(0, 0);

            //
            // If the current track number is invalid (i.e. the number of
            // tracks changed during the power off), then revert back to the
            // first track.
            //
            if(pusTracks[ulTrack] == 0xffff)
            {
                ulTrack = 0;
            }

            //
            // Inform the user interface that we are powering on.
            //
            UISetMode(MODE_POWER_ON);

            //
            // If there are no tracks, then tell the user interface that we
            // are in the stopped state.
            //
            if(pusTracks[ulTrack] == 0xffff)
            {
                UISetMode(MODE_STOP);
            }

            //
            // Turn on the IRQ interrupt.
            //
            EnableIRQ();

#ifndef HwRegUSBCable
            //
            // Re-enable the USB interface.
            //
            USBEnable();
#endif
        }
    }
}

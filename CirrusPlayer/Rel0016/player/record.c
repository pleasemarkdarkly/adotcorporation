//****************************************************************************
//
// RECORD.C - Routines for recording and compressing audio into a file.
//
// Copyright (c) 1999,2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include "globals.h"
#include "../hwport.h"

//****************************************************************************
//
// Only include this code if the record capability is needed.
//
//****************************************************************************
#ifdef REQUIRE_RECORD

//****************************************************************************
//
// Finds the file index of the given file.
//
//****************************************************************************
static unsigned long
FindFile(unsigned short *pusName, unsigned long ulDrive)
{
    unsigned short usFileName[64];
    tDir sDir;
    unsigned long ulFileNum;

    //
    // Open the root directory of the specified drive.
    //
    if(FSOpenDir(&sDir, ulDrive, (unsigned short *)"\\\0\0") == 0)
    {
        return(-1);
    }

    //
    // Scan through the files in the root directory.
    //
    ulFileNum = 0;
    while(1)
    {
        //
        // Read the name of the next file in the directory.
        //
        if(FSReadDir(&sDir, usFileName, FS_TYPE_FILE) == 0)
        {
            //
            // We reached the end of the directory, so close the directory and
            // return an error.
            //
            FSCloseDir(&sDir);
            return(-1);
        }

        //
        // Compare the name of this file with the file in question.
        //
        if(wcscmp(usFileName, pusName) == 0)
        {
            //
            // This is the file we are looking for, so stop looking.
            //
            break;
        }

        //
        // Increment the file number.
        //
        ulFileNum++;
    }

    //
    // Close the directory.
    //
    FSCloseDir(&sDir);

    //
    // Return the file number.
    //
    return(ulFileNum);
}

//****************************************************************************
//
// Adds a new track to the list of tracks on the media.
//
//****************************************************************************
static unsigned long
AddTrack(unsigned short *pusName, unsigned long ulDrive, unsigned long ulCodec)
{
    unsigned long ulFileNum, ulTrackNum, ulIdx;

    //
    // Get the index number of this file.
    //
    ulFileNum = FindFile(pusName, ulDrive);
    if(ulFileNum == -1)
    {
        //
        // This should never occur, but just in case...
        //
        return(0);
    }

    //
    // See if this track on the first or second drive.
    //
    if(ulDrive == 0)
    {
        //
        // This track is on the first drive, so loop through all the tracks.
        //
        for(ulTrackNum = 0; ulTrackNum < MAX_TRACKS; ulTrackNum++)
        {
            //
            // If the file index number of the new track is less than the file
            // index number of the current track, or the current track is on
            // the second drive, then this is the place to insert the new
            // track.
            //
            if((((pusTracks[ulTrackNum] & TRACK_FILE_MASK) >>
                 TRACK_FILE_SHIFT) >= ulFileNum) ||
               (pusTracks[ulTrackNum] & TRACK_DRIVE_MASK))
            {
                break;
            }
        }
    }

    //
    // Otherwise, the new track is on the second drive.
    //
    else
    {
        //
        // Find the first track which is on the second drive.
        //
        for(ulTrackNum = 0; ulTrackNum < MAX_TRACKS; ulTrackNum++)
        {
            if(pusTracks[ulTrackNum] & TRACK_DRIVE_MASK)
            {
                break;
            }
        }

        //
        // Loop through all the tracks on the second drive.
        //
        for(; ulTrackNum < MAX_TRACKS; ulTrackNum++)
        {
            //
            // If the file index number of the new track is less than the file
            // index number of the current track, then this is the place to
            // insert the new track.
            //
            if((((pusTracks[ulTrackNum] & TRACK_FILE_MASK) >>
                 TRACK_FILE_SHIFT) >= ulFileNum) ||
               (pusTracks[ulTrackNum] == 0xffff))
            {
                break;
            }
        }
    }

    //
    // If we scanned through all the available tracks and could not find a
    // place for this track, then we have more tracks than we know how to deal
    // with.  In this case, simply return zero.
    //
    if(ulTrackNum == MAX_TRACKS)
    {
        return(0);
    }

    //
    // Move all the tracks that appear after the new track up one entry in the
    // track list.
    //
    for(ulIdx = MAX_TRACKS - 1; ulIdx > ulTrackNum; ulIdx--)
    {
        //
        // Move the previous track entry into this track entry.
        //
        pusTracks[ulIdx] = pusTracks[ulIdx - 1];

        //
        // If the new track is on the first drive and this track is on the
        // first drive, then the file number for this track needs to be changed
        // to reflect the new file which has been created.
        //
        if((ulDrive == 0) && !(pusTracks[ulIdx] & TRACK_DRIVE_MASK))
        {
            pusTracks[ulIdx]++;
        }

        //
        // Otherwise, if the new track is on the second drive and this track is
        // on the second drive, then the file number for this track needs to be
        // changed to reflect the new file which has been created.
        //
        else if((ulDrive == 1) && (pusTracks[ulIdx] & TRACK_DRIVE_MASK) &&
                (pusTracks[ulIdx] != 0xffff))
        {
            pusTracks[ulIdx]++;
        }
    }

    //
    // Insert the entry for this track into the list.
    //
    pusTracks[ulTrackNum] =
        ((ulDrive << TRACK_DRIVE_SHIFT) & TRACK_DRIVE_MASK) |
        ((ulCodec << TRACK_CODEC_SHIFT) & TRACK_CODEC_MASK) |
        ((ulFileNum << TRACK_FILE_SHIFT) & TRACK_FILE_MASK);

    //
    // Return the track number of the new track.
    //
    return(ulTrackNum);
}

//****************************************************************************
//
// Removes a track for the list of tracks on the media.
//
//****************************************************************************
#ifdef NOT_USED_YET
static void
RemoveTrack(unsigned short *pusName, unsigned long ulDrive)
{
    unsigned long ulFileNum, ulTrackNum, ulIdx;

    //
    // Get the index number of this file.
    //
    ulFileNum = FindFile(pusName, ulDrive);
    if(ulFileNum == -1)
    {
        //
        // We could not find the specified file, so there is nothing to do.
        //
        return;
    }

    //
    // Loop through all the tracks.
    //
    for(ulTrackNum = 0; ulTrackNum < MAX_TRACKS; ulTrackNum++)
    {
        //
        // We should stop looking if we found a file on the same drive which
        // is the same file number or greater.
        //
        if(((pusTracks[ulTrackNum] & TRACK_FILE_MASK) >= ulFileNum) &&
           (((pusTracks[ulTrackNum] & TRACK_DRIVE_MASK) >>
             TRACK_DRIVE_SHIFT) == ulDrive) &&
           (pusTracks[ulTrackNum] != 0xffff))
        {
            break;
        }
    }

    //
    // See if we found a track which is effected by the removal of this file.
    //
    if(ulTrackNum == MAX_TRACKS)
    {
        //
        // There is no track corresponding to this file, so there is nothing
        // to do.
        //
        return;
    }

    //
    // See if we found this file in the track list.
    //
    if((pusTracks[ulTrackNum] & TRACK_FILE_MASK) == ulFileNum)
    {
        //
        // Loop through all the tracks after the one corresponding to this
        // file.
        //
        for(ulIdx = ulTrackNum; ulIdx < (MAX_TRACKS - 1); ulIdx++)
        {
            //
            // Copy the subsequent track into the current track.
            //
            pusTracks[ulIdx] = pusTracks[ulIdx + 1];
        }
    }

    //
    // Loop through the remaining tracks.
    //
    for(; ulTrackNum < MAX_TRACKS; ulTrackNum++)
    {
        //
        // Stop scanning if this track is not on the correct drive.
        //
        if((((pusTracks[ulTrackNum] & TRACK_DRIVE_MASK) >>
             TRACK_DRIVE_SHIFT) != ulDrive) ||
           (pusTracks[ulTrackNum] == 0xffff))
        {
            break;
        }

        //
        // Decrement the file number of this track.
        //
        pusTracks[ulTrackNum]--;
    }
}
#endif

//****************************************************************************
//
// Determines a unique file name for storing a voice file.
//
//****************************************************************************
static unsigned long
GetUniqueName(unsigned long ulDrive, unsigned short *pusName)
{
    unsigned short usFileName[64];
    tDir sDir;
    unsigned long ulIdx;

    //
    // The unique name we will generate is "VOICExxx.WAV", where xxx will be
    // 000 through 999.  Generate the fixed portion of the file name in the
    // file name buffer.
    //
    pusName[0] = 'V';
    pusName[1] = 'O';
    pusName[2] = 'I';
    pusName[3] = 'C';
    pusName[4] = 'E';
    pusName[8] = '.';
    pusName[9] = 'W';
    pusName[10] = 'A';
    pusName[11] = 'V';
    pusName[12] = '\0';

    //
    // Loop through the 1000 possible index numbers.
    //
    for(ulIdx = 0; ulIdx < 1000; ulIdx++)
    {
        //
        // Add the index number to the "xxx" portion of the file name.
        //
        pusName[5] = '0' + ((ulIdx / 100) % 10);
        pusName[6] = '0' + ((ulIdx / 10) % 10);
        pusName[7] = '0' + (ulIdx % 10);

        //
        // Open the root directory on the specified drive.
        //
        if(FSOpenDir(&sDir, ulDrive, (unsigned short *)"\\\0\0") == 0)
        {
            return(0);
        }

        //
        // Read every file from the directory.
        //
        while(1)
        {
            //
            // Read the next file name from the directory.
            //
            if(FSReadDir(&sDir, usFileName, FS_TYPE_FILE) == 0)
            {
                //
                // We've checked each file in the directory, and none of them
                // have the current name, so it is unique.  Return success.
                //
                FSCloseDir(&sDir);
                return(1);
            }

            //
            // See if this file name matches the "unique" file name.
            //
            if(wcscmp(usFileName, pusName) == 0)
            {
                //
                // The names match, so stop searching and advance to the next
                // index.
                //
                break;
            }
        }

        //
        // Close the directory.
        //
        FSCloseDir(&sDir);
    }

    //
    // We tried all 1000 possible names and could not find a unique name, so
    // return a failure.
    //
    return(0);
}

//****************************************************************************
//
// Record captures and compresses audio (presumably voice) from the analog
// input and writes it into a file.
//
//****************************************************************************
unsigned long
Record(void)
{
    tFile sFile;
    unsigned short pusName[16];
    unsigned long ulSampleRate, ulTemp;
    short *psData;
    long lLength;

    //
    // Get a unique file name to record into.
    //
    if(GetUniqueName(0, pusName) == 0)
    {
        return(0);
    }

    //
    // Provide a scratch buffer to the file system for use while writing data
    // to the file.
    //
    FSSetWriteScratch((unsigned char *)ulEndOfRAM);
    ulEndOfRAM += 512;

    //
    // Create the file to hold the recorded data.
    //
    if(FSOpen(&sFile, 0, pusName,
              FS_OPEN_CREATE | FS_OPEN_READ | FS_OPEN_WRITE) == 0)
    {
        FSSetWriteScratch(0);
        ulEndOfRAM -= 512;
        return(0);
    }

    //
    // Open the codec.
    //
    if(CodecOpen(-1, CODEC_OPEN_ENCODE, &sFile) == 0)
    {
        FSClose(&sFile);
        FSDelete(0, pusName);
        FSSetWriteScratch(0);
        ulEndOfRAM -= 512;
        return(0);
    }

    //
    // Enable the input processing.
    //
    CodecGetSampleRate(&ulSampleRate);
    CodecGetCaptureBuffer(&psData, &lLength);
    if(InputEnable(ulSampleRate, psData, lLength) == 0)
    {
        FSClose(&sFile);
        FSDelete(0, pusName);
        FSSetWriteScratch(0);
        ulEndOfRAM -= 512;
        return(0);
    }

    //
    // Get the index of the codec.
    //
    CodecGetIndex(&ulTemp);

    //
    // Add the new file to the track list.
    //
    ulTrack = AddTrack(pusName, 0, ulTemp);

    //
    // Inform the user interface that a new file has been loaded.
    //
    UIFileLoaded(pusName, ulTrack);

    //
    // Set the output buffer for the decoder.
    //
    CodecSetBuffer(InputGetOutputBuffer());

    //
    // Inform the user interface that we are recording.
    //
    UISetMode(MODE_RECORD);

    //
    // Main record loop.  We explicitly break out of the loop when we are done
    // recording.
    //
    while(1)
    {
        //
        // See if the encoder is able to encode another frame.
        //
        while(CodecCanEncode() == 0)
        {
            //
            // Put the EP7209 into HALT mode.
            //
            Halt();
        }

        //
        // Tell the encoder to encode another frame.
        //
        if(CodecEncode() == 0)
        {
            ulTemp = 0;
            break;
        }

        //
        // Get the new time.
        //
        CodecGetTime(&ulTemp);

        //
        // Inform the user interface of the new time.
        //
        UISetCurrentTime(ulTemp);

        //
        // Get the current set of pressed virtual buttons.
        //
        ulTemp = UIGetButtons();

        //
        // Is a download being requested?
        //
        if(ulSystemFlags & SYSFLAG_START_DOWNLOAD)
        {
            //
            // Stop recording so we can honor the download request.
            //
            ulTemp = 0;
            break;
        }

        //
        // Is the power button being pressed?
        //
        else if(ulTemp & BUTTON_POWER)
        {
            //
            // Stop recording.
            //
            ulTemp = BUTTON_POWER;
            break;
        }

        //
        // Is the stop button being pressed?
        //
        else if(ulTemp & BUTTON_STOP)
        {
            //
            // Stop recording.
            //
            ulTemp = 0;
            break;
        }

        //
        // Is the loopback button being pressed?
        //
#ifdef REQUIRE_LOOPBACK
        else if(ulTemp & BUTTON_LOOPBACK)
        {
            //
            // Stop recording.
            //
            ulTemp = BUTTON_LOOPBACK;
            break;
        }
#endif
    }

    //
    // Close the codec.
    //
    CodecClose();

    //
    // Disable the input processing.
    //
    InputDisable();

    //
    // Close the file.
    //
    FSClose(&sFile);

    //
    // Take the scratch buffer back from the file system since we are done
    // writing data to the file.
    //
    FSSetWriteScratch(0);
    ulEndOfRAM -= 512;

    //
    // Return the appropriate value.
    //
    return(ulTemp);
}
#endif

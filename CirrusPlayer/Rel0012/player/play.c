//****************************************************************************
//
// PLAY.C - Routines for playing audio from a file.
//
// Copyright (c) 1999-2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include "globals.h"

//****************************************************************************
//
// OpenTrack prepares to decode and play the specified track.
//
//****************************************************************************
static void
OpenTrack(tFile *pFile)
{
    unsigned long ulSampleRate, ulChannels, ulDrive, ulFile, ulCodec;
    char cFileName[64];
    tDir sDir;

    //
    // Get the drive, file number, and codec for this track.
    //
    ulDrive = (pusTracks[ulTrack] & TRACK_DRIVE_MASK) >> TRACK_DRIVE_SHIFT;
    ulFile = (pusTracks[ulTrack] & TRACK_FILE_MASK) >> TRACK_FILE_SHIFT;
    ulCodec = (pusTracks[ulTrack] & TRACK_CODEC_MASK) >> TRACK_CODEC_SHIFT;

    //
    // Open the root directory of the first drive.
    //
    FSOpenDir(&sDir, ulDrive, "\\");

    //
    // Skip to the appropriate file in the root directory.
    //
    do
    {
        FSReadDir(&sDir, cFileName, FS_TYPE_FILE);
    }
    while(ulFile--);

    //
    // Close the directory.
    //
    FSCloseDir(&sDir);

    //
    // Open this file.
    //
    FSOpen(pFile, ulDrive, cFileName, FS_OPEN_READ);

    //
    // See if we are able to decode this file.
    //
    CodecOpen(ulCodec, CODEC_OPEN_DECODE, pFile);

    //
    // Set the sample rate of the output processing.
    //
    CodecGetSampleRate(&ulSampleRate);
    CodecGetChannels(&ulChannels);
    OutputSetFormat(ulSampleRate, ulChannels);

    //
    // Inform the user interface that a new file has been loaded.
    //
    UIFileLoaded(cFileName, ulTrack);

    //
    // Set the output buffer for the decoder.
    //
    CodecSetBuffer(OutputGetInputBuffer());
}

//****************************************************************************
//
// CloseTrack closes the currently opened file.
//
//****************************************************************************
static void
CloseTrack(tFile *pFile)
{
    BufferState *pBuffer;

    //
    // Get the output processing's input buffer.
    //
    pBuffer = OutputGetInputBuffer();

    //
    // Wait until all of the output data has been consumed.
    //
    while(!BufferIsEmpty(pBuffer))
    {
        //
        // Put the EP7209 into IDLE mode.
        //
        Halt();
    }

    //
    // Close the codec.
    //
    CodecClose();

    //
    // Close the file.
    //
    FSClose(pFile);

    //
    // Inform the user interface that the file has been closed.
    //
    UIFileUnloaded();
}

//****************************************************************************
//
// NextTrack closes the current track and advances to the next track.
//
//****************************************************************************
static void
NextTrack(tFile *pFile)
{
    //
    // Close the currently opened track.
    //
    CloseTrack(pFile);

    //
    // Skip to the next track.
    //
    ulTrack++;

    //
    // If this was the last track, then go back to the first track.
    //
    if(pusTracks[ulTrack] == 0xffff)
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
        // See if this is still the last track.
        //
        if(pusTracks[ulTrack] == 0xffff)
        {
            //
            // Go back to the first track.
            //
            ulTrack = 0;
        }
    }

    //
    // Open the next track.
    //
    OpenTrack(pFile);
}

//****************************************************************************
//
// PreviousTrack closes the current track and advances to the previous track.
//
//****************************************************************************
static void
PreviousTrack(tFile *pFile)
{
    //
    // Close the currently opened track.
    //
    CloseTrack(pFile);

    //
    // See if we are at the first track.
    //
    if(ulTrack == 0)
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
        // Find the final track.
        //
        for(ulTrack = 0; ulTrack < 99; ulTrack++)
        {
            //
            // Is the next track the final track?
            //
            if(pusTracks[ulTrack + 1] == 0xffff)
            {
                break;
            }
        }
    }

    //
    // Otherwise, simply skip to the previous track.
    //
    else
    {
        ulTrack--;
    }

    //
    // Open the previous track.
    //
    OpenTrack(pFile);
}

//****************************************************************************
//
// The main loop for the Internet audio player.  This can not return since it
// is branched to by vectors.s.
//
//****************************************************************************
unsigned long
Play(void)
{
    tFile sFile;
    unsigned long ulButtons, ulMode;

    //
    // Set the player mode to stopped.
    //
    ulMode = MODE_STOP;

    //
    // Open the current track.
    //
    OpenTrack(&sFile);

    //
    // Inform the user interface that we are stopped.
    //
    UISetMode(MODE_STOP);

    //
    // Handle playback operations until we encounter a condition which causes
    // us to leave playback mode (i.e. download, the record button, etc.).
    //
    while(1)
    {
        //
        // See if we are playing.
        //
        if(ulMode == MODE_PLAY)
        {
            //
            // Tell the decoder to decode another frame.
            //
            if(CodecDecode() == 0)
            {
                //
                // If we are in repeat one track mode, then we need to skip
                // back to the beginning of this track.
                //
                if((ulSystemFlags & SYSFLAG_REPEAT_MASK) == SYSFLAG_REPEAT_ONE)
                {
                    //
                    // Seek to the beginning of the song.
                    //
                    CodecSeek(0);

                    //
                    // Inform the user interface of the new song position.
                    //
                    UISetCurrentTime(0);
                }
                else
                {
                    //
                    // Otherwise, go to the next track.
                    //
                    NextTrack(&sFile);

                    //
                    // If we are back at track zero, then see if we need to
                    // stop the player (i.e. we are not in repeat all mode).
                    //
                    if((ulTrack == 0) &&
                       ((ulSystemFlags & SYSFLAG_REPEAT_MASK) !=
                        SYSFLAG_REPEAT_ALL))
                    {
                        //
                        // Stop the player.
                        //
                        ulMode = MODE_STOP;

                        //
                        // Inform the user interface that we are now stopped.
                        //
                        UISetMode(MODE_STOP);
                    }
                }
            }
            else
            {
                unsigned long ulTime;

                //
                // Get the new time.
                //
                CodecGetTime(&ulTime);

                //
                // Inform the user interface of the new time.
                //
                UISetCurrentTime(ulTime);
            }
        }
        else
        {
            //
            // Put the EP7209 into HALT mode.
            //
            Halt();
        }

        //
        // See if the removable media has been removed.
        //
        if(ulSystemFlags & SYSFLAG_MEDIA_REMOVE)
        {
            //
            // Handle the removal of the external media.
            //
            MediaRemoved();

            //
            // See if the current track was on the external media.
            //
            if(pusTracks[ulTrack] == 0xffff)
            {
                //
                // The current track is no longer available, so close it now.
                //
                CloseTrack(&sFile);

                //
                // Go back to the first track.
                //
                ulTrack = 0;

                //
                // If there are now no tracks available, then we return.
                //
                if(pusTracks[0] == 0xffff)
                {
                    //
                    // Inform the user interface that we are stopped.
                    //
                    UISetMode(MODE_STOP);

                    //
                    // Return indicating that there is nothing further to do.
                    //
                    return(0);
                }

                //
                // Open the first track.
                //
                OpenTrack(&sFile);

                //
                // If we are not stopped, then we should be unless we are in
                // repeat all mode.
                //
                if((ulMode != MODE_STOP) &&
                   ((ulSystemFlags & SYSFLAG_REPEAT_MASK) !=
                    SYSFLAG_REPEAT_ALL))
                {
                    //
                    // Change the mode to stopped.
                    //
                    ulMode = MODE_STOP;

                    //
                    // Inform the user interface of the new mode.
                    //
                    UISetMode(MODE_STOP);
                }
            }
        }

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
            // Disable the output processing.  This function will wait until
            // all the data has been played before shutting down the digital
            // audio interface and returning.
            //
            OutputDisable();

            //
            // Close the current file.
            //
            CloseTrack(&sFile);

            //
            // Return indicating that there is nothing further to do.
            //
            return(0);
        }

        //
        // Is the power button being pressed?
        //
        else if(ulButtons & BUTTON_POWER)
        {
            //
            // Disable the output processing.  This function will wait until
            // all the data has been played before shutting down the digital
            // audio interface and returning.
            //
            OutputDisable();

            //
            // Close the current file.
            //
            CloseTrack(&sFile);

            //
            // Return indicating that we should power off the player.
            //
            return(BUTTON_POWER);
        }

        //
        // Is the play button being pressed?
        //
        else if((ulButtons & BUTTON_PLAY) && (ulMode != MODE_PLAY))
        {
            //
            // The user wants to start playback of the song.
            //
            ulMode = MODE_PLAY;

            //
            // Enable the output processing.
            //
            OutputEnable();

            //
            // Inform the user interface that we are playing.
            //
            UISetMode(MODE_PLAY);
        }

        //
        // Is the pause button being pressed?
        //
        else if((ulButtons & BUTTON_PAUSE) && (ulMode != MODE_PAUSE))
        {
            //
            // The user wants to pause playback of the song.
            //
            ulMode = MODE_PAUSE;

            //
            // Disable the output processing.  This function will wait until
            // all the data has been played before shutting down the digital
            // audio interface and returning.
            //
            OutputDisable();

            //
            // Inform the user interface that we are paused.
            //
            UISetMode(MODE_PAUSE);
        }

        //
        // Is the stop button being pressed?
        //
        else if((ulButtons & BUTTON_STOP) && (ulMode != MODE_STOP))
        {
            //
            // The user wants to stop playback of the song.
            //
            ulMode = MODE_STOP;

            //
            // Disable the output processing.  This function will wait until
            // all the data has been played before shutting down the digital
            // audio interface and returning.
            //
            OutputDisable();

            //
            // Seek back to the beginning of this track.
            //
            CodecSeek(0);

            //
            // Inform the user interface of the new song position.
            //
            UISetCurrentTime(0);

            //
            // Inform the user interface that we are stopped.
            //
            UISetMode(MODE_STOP);
        }

        //
        // Is the previous track button being pressed?
        //
        else if(ulButtons & BUTTON_PREV)
        {
            unsigned long ulTime;

            //
            // Get the current time.
            //
            CodecGetTime(&ulTime);

            //
            // If we are more than 3 seconds into the track then simply seek
            // back to the beginning of the track.
            //
            if(ulTime > 3000)
            {
                //
                // Seek to the beginning of the track.
                //
                CodecSeek(0);

                //
                // Inform the user interface of the new track position.
                //
                UISetCurrentTime(0);
            }
            else
            {
                //
                // Skip to the previous track.
                //
                PreviousTrack(&sFile);
            }
        }

        //
        // Is the next track button being pressed?
        //
        else if(ulButtons & BUTTON_NEXT)
        {
            //
            // Skip to the next track.
            //
            NextTrack(&sFile);
        }

        //
        // Is the seek backward button being pressed?
        //
        else if((ulButtons & BUTTON_REW) && (ulMode != MODE_STOP))
        {
            unsigned long ulTime, ulSkip;

            //
            // Get the current time.
            //
            CodecGetTime(&ulTime);

            //
            // If we are playing, then we seek backwards by 1.3 seconds.
            // Otherwise, we seek backwards by 1 second.
            //
            if(ulMode == MODE_PLAY)
            {
                //
                // Seek backwards 1.3 seconds.  The extra 300ms is to account
                // for the ~300ms of audio that is played between seeks.
                //
                ulSkip = 1300;
            }
            else
            {
                //
                // Seek backward 1 second, or to the beginning.
                //
                ulSkip = 1000;
            }

            //
            // If the current time is less than the seek time, then skip to the
            // previous track.
            //
            if(ulTime < ulSkip)
            {
                //
                // Skip to the previous track.
                //
                PreviousTrack(&sFile);

                //
                // Seek to the end of the previous track.
                //
                CodecGetLength(&ulTime);
                ulTime -= ulSkip;
                CodecSeek(ulTime);
            }
            else
            {
                //
                // Skip backwards by the specified amount of time, or to the
                // beginning.
                //
                ulTime = ulTime < ulSkip ? 0 : ulTime - ulSkip;

                //
                // Seek to the new position.
                //
                CodecSeek(ulTime);
            }

            //
            // Inform the user interface of the new song position.
            //
            UISetCurrentTime(ulTime);
        }

        //
        // Is the seek forward button being pressed?
        //
        else if((ulButtons & BUTTON_FFWD) && (ulMode != MODE_STOP))
        {
            unsigned long ulTime, ulLength;

            //
            // Get the current time.
            //
            CodecGetTime(&ulTime);

            //
            // Get the length of the song.
            //
            CodecGetLength(&ulLength);

            //
            // If we are more than 1 second from the end of the song, then seek
            // forward.
            //
            if((ulTime + 1300) < ulLength)
            {
                //
                // Seek to the new position.
                //
                CodecSeek(ulTime + 1000);

                //
                // Inform the user interface of the new song position.
                //
                UISetCurrentTime(ulTime + 1000);
            }
            else
            {
                //
                // Skip to the next track.
                //
                NextTrack(&sFile);
            }
        }

        //
        // Is the record button being pressed?
        //
#ifdef REQUIRE_RECORD
        else if(ulButtons & BUTTON_RECORD)
        {
            //
            // Disable the output processing.  This function will wait until
            // all the data has been played before shutting down the digital
            // audio interface and returning.
            //
            OutputDisable();

            //
            // Close the current file.
            //
            CloseTrack(&sFile);

            //
            // Return indicating that we need to record.
            //
            return(BUTTON_RECORD);
        }
#endif
    }
}

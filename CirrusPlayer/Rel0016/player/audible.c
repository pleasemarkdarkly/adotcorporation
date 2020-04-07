//****************************************************************************
//
// AUDIBLE.C - Routines for handling the meta-data associated with an Audible
//             program.
//
// Copyright (c) 2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include "globals.h"
#include "audible.h"

//****************************************************************************
//
// Only include the functions in this file if Audible support is required.
//
//****************************************************************************
#ifdef SUPPORT_AUDIBLE

//****************************************************************************
//
// The name of the file which contains the Audible meta-data.
//
//****************************************************************************
static const unsigned short pusAudibleFile[] =
{
    '\\', 'A', 'U', 'D', 'I', 'B', 'L', 'E', '.', 'D', 'A', 'T', '\0'
};

//****************************************************************************
//
// The local copy of the Audible meta-data for the current program.
//
//****************************************************************************
static struct
{
    //
    // The offset into the meta-data file of the meta-data for the current
    // program.  Since the offset is always a multiple of 512, the lower 8
    // bits of this field are used to store the drive on which this Audible
    // program resides, and the 9th bit is used to store the played through
    // indicator.
    //
    unsigned long ulMetaDataOffset;

    //
    // The title of the current program.
    //
    unsigned short pusTitle[128];

    //
    // The number of sections in the program segment contained in the currently
    // opened file.
    //
    unsigned long ulNumSections;
} sAudible;

//****************************************************************************
//
// AudibleLoadMetaData attempts to locate the Audible meta-data for the
// specified file.
//
//****************************************************************************
void
AudibleLoadMetaData(unsigned long ulDrive, const unsigned short *pusFileName)
{
    tFile sFile;
    unsigned long ulOffset, ulIdx = 0;
    tAudibleMetaData *psMetaData;
    unsigned char *pucBuffer;

    //
    // Get a pointer to the file system's scratch buffer.
    //
    pucBuffer = FSGetScratch();

    //
    // Unless we find the meta-data for this file, there is no meta-data (and
    // therefore this is not an Audible program).
    //
    sAudible.ulMetaDataOffset = 0;

    //
    // Open the Audible meta-data file.
    //
    if(FSOpen(&sFile, ulDrive, pusAudibleFile, FS_OPEN_READ) == 0)
    {
        return;
    }

    //
    // Read the block allocation table from the file.
    //
    if(FSRead(&sFile, pucBuffer, 512) != 512)
    {
        FSClose(&sFile);
        return;
    }

    //
    // Get the offset of the first directory entry block.
    //
    ulOffset = *(unsigned long *)pucBuffer;
    if(ulOffset & 511)
    {
        FSClose(&sFile);
        return;
    }

    //
    // Loop while the offset is not zero.
    //
    while(ulOffset)
    {
        //
        // Seek to the next directory entry block.
        //
        if(FSSeek(&sFile, ulOffset) == 0)
        {
            FSClose(&sFile);
            return;
        }

        //
        // Read the next directory entry block.
        //
        if(FSRead(&sFile, pucBuffer, 512) != 512)
        {
            FSClose(&sFile);
            return;
        }

        //
        // Loop through the four directory entries in this block.
        //
        for(ulIdx = 0; ulIdx < 4; ulIdx++)
        {
            //
            // See if this file name matches the file name for which we are
            // searching.
            //
            if(wcsncmp((unsigned short *)(pucBuffer + 4 + (ulIdx * 127)),
                       pusFileName, 61) == 0)
            {
                break;
            }
        }
        if(ulIdx != 4)
        {
            break;
        }

        //
        // Get the offset of the next directory entry block.
        //
        ulOffset = *(unsigned long *)pucBuffer;
        if(ulOffset & 511)
        {
            FSClose(&sFile);
            return;
        }
    }

    //
    // If we could not find the file in the Audible meta-data file, then return
    // a failure.
    //
    if(ulOffset == 0)
    {
        FSClose(&sFile);
        return;
    }

    //
    // Get the offset of the block containing the Audible meta-data for this
    // file.
    //
    sAudible.ulMetaDataOffset = pucBuffer[(ulIdx * 127) + 127] |
                                (pucBuffer[(ulIdx * 127) + 128] << 8) |
                                (pucBuffer[(ulIdx * 127) + 129] << 16) |
                                (pucBuffer[(ulIdx * 127) + 130] << 24);
    if(sAudible.ulMetaDataOffset & 511)
    {
        sAudible.ulMetaDataOffset = 0;
        FSClose(&sFile);
        return;
    }

    //
    // Seek to the block containing the meta-data.
    //
    if(FSSeek(&sFile, sAudible.ulMetaDataOffset) == 0)
    {
        sAudible.ulMetaDataOffset = 0;
        FSClose(&sFile);
        return;
    }

    //
    // Read the block containing the meta-data.
    //
    if(FSRead(&sFile, pucBuffer, 512) != 512)
    {
        sAudible.ulMetaDataOffset = 0;
        FSClose(&sFile);
        return;
    }

    //
    // Get a pointer to the meta-data.
    //
    psMetaData = (tAudibleMetaData *)(pucBuffer + 4);

    //
    // Save the title of this program.
    //
    memcpy(sAudible.pusTitle, psMetaData->pusTitle, 256);

    //
    // Save the number of sections in this program.
    //
    sAudible.ulNumSections = psMetaData->ulSections;

    //
    // Seek to the saved current offset.
    //
    CodecSeek(psMetaData->ulCurrentPosition);

    //
    // Close the meta-data file.
    //
    FSClose(&sFile);

    //
    // Save the drive on which this Audible program resides.
    //
    sAudible.ulMetaDataOffset |= ulDrive;
}

//****************************************************************************
//
// AudibleUpdateMetaData updates the current position and played-through
// indicator for the current Audible program.
//
//****************************************************************************
void
AudibleUpdateMetaData(unsigned char *pucBuffer, unsigned long ulNewPosition)
{
    tFile sFile;
    tAudibleMetaData *psMetaData;

    //
    // If the current file does not contain an Audible program, then do
    // nothing.
    //
    if(sAudible.ulMetaDataOffset == 0)
    {
        return;
    }

    //
    // Open the Audible meta-data file.
    //
    if(FSOpen(&sFile, sAudible.ulMetaDataOffset & 255, pusAudibleFile,
              FS_OPEN_READ | FS_OPEN_WRITE) == 0)
    {
        return;
    }

    //
    // Seek to the block containing the meta-data.
    //
    if(FSSeek(&sFile, sAudible.ulMetaDataOffset & ~511) == 0)
    {
        FSClose(&sFile);
        return;
    }

    //
    // Read the block containing the meta-data.
    //
    if(FSRead(&sFile, pucBuffer, 512) != 512)
    {
        FSClose(&sFile);
        return;
    }

    //
    // Get a pointer to the meta-data.
    //
    psMetaData = (tAudibleMetaData *)(pucBuffer + 4);

    //
    // Update the current position.
    //
    psMetaData->ulCurrentPosition = ulNewPosition;

    //
    // Update the played-through indicator.
    //
    if(sAudible.ulMetaDataOffset & 0x100)
    {
        psMetaData->ucPlayedThrough = 1;
    }

    //
    // Seek to the block containing the meta-data.
    //
    if(FSSeek(&sFile, sAudible.ulMetaDataOffset & ~511) == 0)
    {
        FSClose(&sFile);
        return;
    }

    //
    // Write the updated block of meta-data.
    //
    if(FSWrite(&sFile, pucBuffer, 512) != 512)
    {
        FSClose(&sFile);
        return;
    }

    //
    // Close the meta-data file.
    //
    FSClose(&sFile);
}

//****************************************************************************
//
// AudibleIsAudibleProgram returns TRUE if the current file contains an
// Audible program and FALSE otherwise.
//
//****************************************************************************
unsigned long
AudibleIsAudibleProgram(void)
{
    //
    // Return TRUE if the current file contains an Audible program.
    //
    if(sAudible.ulMetaDataOffset)
    {
        return(1);
    }

    //
    // The current file does not contain an Audible program, so return FALSE.
    //
    return(0);
}

//****************************************************************************
//
// AudibleGetTitle returns the title of the current Audible program.
//
//****************************************************************************
const unsigned short *
AudibleGetTitle(void)
{
    //
    // Return a pointer to the title of the current program if the current file
    // contains an Audible program.
    //
    if(sAudible.ulMetaDataOffset)
    {
        return(sAudible.pusTitle);
    }

    //
    // The current file does not contain an Audible program, so return NULL.
    //
    return(0);
}

//****************************************************************************
//
// AudibleGetNumSections returns the number of sections in the current Audible
// program.
//
//****************************************************************************
unsigned long
AudibleGetNumSections(void)
{
    //
    // Return the number of sections in the current program if the current file
    // contains an Audible program.
    //
    if(sAudible.ulMetaDataOffset)
    {
        return(sAudible.ulNumSections);
    }

    //
    // The current file does not contain an Audible program, so return 0.
    //
    return(0);
}

//****************************************************************************
//
// AudibleGetPreviousSection returns the time of the section that occurs
// before the specified time in the Audible program.  If the specified time
// is within the first 3 seconds of a section, then the time of the previous
// section will be returned.
//
//****************************************************************************
unsigned long
AudibleGetPreviousSection(unsigned long ulTime)
{
    tFile sFile;
    unsigned long *pulSections, ulIdx, ulAvailable, ulSection;
    unsigned char *pucBuffer;

    //
    // Get a pointer to the file system's scratch buffer.
    //
    pucBuffer = FSGetScratch();

    //
    // If the current file does not contain an Audible program, then return 0
    // to indicate that the previous "section" is at the start of the file.
    //
    if(!sAudible.ulMetaDataOffset)
    {
        return(0);
    }

    //
    // Open the Audible meta-data file.
    //
    if(FSOpen(&sFile, sAudible.ulMetaDataOffset & 255, pusAudibleFile,
              FS_OPEN_READ) == 0)
    {
        return(0);
    }

    //
    // Seek to the block containing the meta-data.
    //
    if(FSSeek(&sFile, sAudible.ulMetaDataOffset & ~511) == 0)
    {
        FSClose(&sFile);
        return(0);
    }

    //
    // Read the block containing the meta-data.
    //
    if(FSRead(&sFile, pucBuffer, 512) != 512)
    {
        FSClose(&sFile);
        return(0);
    }

    //
    // Get a pointer to the table of section offsets.
    //
    pulSections = ((tAudibleMetaData *)(pucBuffer + 4))->pulSectionStart;

    //
    // There is enough space in the first block of meta-data for 17 section
    // offsets.
    //
    ulAvailable = 17;

    //
    // The current previous section offset is zero.
    //
    ulSection = 0;

    //
    // Loop through all of the sections in this program.
    //
    for(ulIdx = 0; ulIdx < sAudible.ulNumSections; ulIdx++)
    {
        //
        // See if there are any more section offsets available in this block of
        // meta-data.
        //
        if(ulAvailable == 0)
        {
            //
            // Get the offset of the next block of meta-data.
            //
            ulAvailable = *(unsigned long *)pucBuffer;
            if(ulAvailable & 511)
            {
                FSClose(&sFile);
                return(ulSection);
            }

            //
            // Seek to the next block of meta-data.
            //
            if(FSSeek(&sFile, ulAvailable) == 0)
            {
                FSClose(&sFile);
                return(ulSection);
            }

            //
            // Read the next block of meta-data.
            //
            if(FSRead(&sFile, pucBuffer, 512) != 512)
            {
                FSClose(&sFile);
                return(ulSection);
            }

            //
            // Get a pointer to the table of section offsets.
            //
            pulSections = (unsigned long *)(pucBuffer + 4);

            //
            // There is enough space in the subsequent blocks of meta-data for
            // 127 section offsets.
            //
            ulAvailable = 127;
        }

        //
        // If this section offset is after the current time, or is less than
        // three seconds before the current time, then the previous section
        // offset is the one we should return.
        //
        if((*pulSections + 3000) > ulTime)
        {
            //
            // Close the meta-data file.
            //
            FSClose(&sFile);

            //
            // Return the time of the previous section.
            //
            return(ulSection);
        }

        //
        // Remember the current section offset as the new previous section
        // offset, and skip to the next section offset in the table.
        //
        ulSection = *pulSections++;

        //
        // Decrement the number of section offsets available.
        //
        ulAvailable--;
    }

    //
    // Close the meta-data file.
    //
    FSClose(&sFile);

    //
    // Since we fell out of the above loop, the last section offset from the
    // table is the previous section in the program.
    //
    return(ulSection);
}

//****************************************************************************
//
// AudibleGetNextSection returns the time of the section that occurs after the
// specified time in the Audible program.  If there is no section after the
// specified time, then a time of -1 will be returned.
//
//****************************************************************************
unsigned long
AudibleGetNextSection(unsigned long ulTime)
{
    tFile sFile;
    unsigned long *pulSections, ulIdx, ulAvailable;
    unsigned char *pucBuffer;

    //
    // Get a pointer to the file system's scratch buffer.
    //
    pucBuffer = FSGetScratch();

    //
    // If the current file does not contain an Audible program, then return 0
    // to indicate that the previous "section" is at the start of the file.
    //
    if(!sAudible.ulMetaDataOffset)
    {
        return(0);
    }

    //
    // Open the Audible meta-data file.
    //
    if(FSOpen(&sFile, sAudible.ulMetaDataOffset & 255, pusAudibleFile,
              FS_OPEN_READ) == 0)
    {
        return(0);
    }

    //
    // Seek to the block containing the meta-data.
    //
    if(FSSeek(&sFile, sAudible.ulMetaDataOffset & ~511) == 0)
    {
        FSClose(&sFile);
        return(0);
    }

    //
    // Read the block containing the meta-data.
    //
    if(FSRead(&sFile, pucBuffer, 512) != 512)
    {
        FSClose(&sFile);
        return(0);
    }

    //
    // Get a pointer to the table of section offsets.
    //
    pulSections = ((tAudibleMetaData *)(pucBuffer + 4))->pulSectionStart;

    //
    // There is enough space in the first block of meta-data for 17 section
    // offsets.
    //
    ulAvailable = 17;

    //
    // Loop through all of the sections in this program.
    //
    for(ulIdx = 0; ulIdx < sAudible.ulNumSections; ulIdx++)
    {
        //
        // See if there are any more section offsets available in this block of
        // meta-data.
        //
        if(ulAvailable == 0)
        {
            //
            // Get the offset of the next block of meta-data.
            //
            ulAvailable = *(unsigned long *)pucBuffer;
            if(ulAvailable & 511)
            {
                FSClose(&sFile);
                return(-1);
            }

            //
            // Seek to the next block of meta-data.
            //
            if(FSSeek(&sFile, ulAvailable) == 0)
            {
                FSClose(&sFile);
                return(-1);
            }

            //
            // Read the next block of meta-data.
            //
            if(FSRead(&sFile, pucBuffer, 512) != 512)
            {
                FSClose(&sFile);
                return(-1);
            }

            //
            // Get a pointer to the table of section offsets.
            //
            pulSections = (unsigned long *)(pucBuffer + 4);

            //
            // There is enough space in the subsequent blocks of meta-data for
            // 127 section offsets.
            //
            ulAvailable = 127;
        }

        //
        // If this section offset is after the current time then it is the
        // section we should return.
        //
        if(*pulSections > (ulTime + 500))
        {
            //
            // Close the meta-data file.
            //
            FSClose(&sFile);

            //
            // Return the time of this section.
            //
            return(*pulSections);
        }

        //
        // Skip to the next section offset in the table.
        //
        pulSections++;

        //
        // Decrement the number of section offsets available.
        //
        ulAvailable--;
    }

    //
    // Close the meta-data file.
    //
    FSClose(&sFile);

    //
    // Since we fell out of the above loop, we are already in the last section
    // of the program.
    //
    return(-1);
}

//****************************************************************************
//
// AudibleSetPlayedThrough is used to indicate that we've played through the
// entire Audible program.
//
//****************************************************************************
void
AudibleSetPlayedThrough(void)
{
    //
    // Set the played through indicator if the current file contains an Audible
    // program.
    //
    if(sAudible.ulMetaDataOffset)
    {
        sAudible.ulMetaDataOffset |= 0x100;
    }
}
#endif

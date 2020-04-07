//****************************************************************************
//
// AUDDAT.CPP - Routines for creating/modifying audible.dat on the
//              Maverick(tm) Internet Audio Player.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include "stdafx.h"

//****************************************************************************
//
// Reads the Audible meta-data from the device.
//
//****************************************************************************
static unsigned long
ReadMetaData(unsigned long ulDrive, char **ppcBuffer, unsigned long *pulLength)
{
    //
    // Open the Audible meta-data file.
    //
    if(Maverick_Open(ulDrive, L"\\AUDIBLE.DAT", 3) == 0)
    {
        //
        // The initial meta-data file is 1024 bytes in length.
        //
        *pulLength = 1024;

        //
        // Allocate memory for the meta-data file.
        //
        *ppcBuffer = (char *)GlobalAlloc(GMEM_FIXED, *pulLength);
        if(!*ppcBuffer)
        {
            Maverick_Close();
            return(0);
        }

        //
        // Zero fill the meta-data.
        //
        memset(*ppcBuffer, 0, 1024);

        //
        // Fill in the offset of the first directory entry.
        //
        (*ppcBuffer)[1] = 0x02;

        //
        // Indicate that the first two blocks of the file are in use.
        //
        (*ppcBuffer)[4] = (char)0xc0;
    }
    else
    {
        //
        // Get the length of the meta-data file.
        //
        *pulLength = Maverick_Length();

        //
        // Allocate memory for the meta-data file.
        //
        *ppcBuffer = (char *)GlobalAlloc(GMEM_FIXED, *pulLength);
        if(!*ppcBuffer)
        {
            Maverick_Close();
            return(0);
        }

        //
        // Read the meta-data from the file.
        //
        Maverick_Read(*ppcBuffer, *pulLength);

        //
        // Close the meta-data file.
        //
        Maverick_Close();
    }

    //
    // Success.
    //
    return(1);
}

//****************************************************************************
//
// Writes the Audible meta-data to the device.
//
//****************************************************************************
static void
WriteMetaData(unsigned long ulDrive, char *pcBuffer, unsigned long ulLength)
{
    //
    // Open the Audible meta-data file.
    //
    if(Maverick_Open(ulDrive, L"\\AUDIBLE.DAT", 3) == 0)
    {
        //
        // The Audible meta-data file does not yet exist, so create it now.
        //
        if(Maverick_Open(ulDrive, L"\\AUDIBLE.DAT", 7) == 0)
        {
            return;
        }
    }

    //
    // Write the meta-data to the file.
    //
    Maverick_Write(pcBuffer, ulLength);

    //
    // Close the file.
    //
    Maverick_Close();

    //
    // Free the meta-data buffer.
    //
    GlobalFree(pcBuffer);
}

//****************************************************************************
//
// Locates the directory entry for the specified file name.
//
//****************************************************************************
static unsigned long
FindFileName(char *pcBuffer, unsigned long ulLength,
             unsigned short *pusFileName)
{
    unsigned long ulOffset, ulIdx;

    //
    // Get the offset of the first directory block.
    //
    ulOffset = *(unsigned long *)pcBuffer;
    if((ulOffset + 512) > ulLength)
    {
        return(-1);
    }

    //
    // Loop while the offset is non-zero.
    //
    while(ulOffset)
    {
        //
        // Loop through the four directory entries in this block.
        //
        for(ulIdx = 0; ulIdx < 4; ulIdx++)
        {
            //
            // See if the file name in this entry matches the file in question.
            //
            if(memcmp(pcBuffer + ulOffset + (ulIdx * 127) + 4, pusFileName,
                      122) == 0)
            {
                break;
            }
        }

        //
        // See if we found the file name.
        //
        if(ulIdx != 4)
        {
            //
            // Return the offset of the directory entry for this file name.
            //
            return(ulOffset + (ulIdx * 127) + 4);
        }

        //
        // Go to the next block of the directory.
        //
        ulOffset = *(unsigned long *)(pcBuffer + ulOffset);
        if((ulOffset + 512) > ulLength)
        {
            return(-1);
        }
    }

    //
    // If we fall out of the above loop the specified file does not exist in
    // the meta-data directory.
    //
    return(-1);
}

//****************************************************************************
//
// Finds an unused block in the meta-data file.
//
//****************************************************************************
unsigned long
GetUnusedBlock(char **ppcBuffer, unsigned long *pulLength)
{
    char *pcNewBuffer;
    unsigned long ulOffset;

    //
    // Loop through the blocks of the meta-data file until we find one that is
    // unused.
    //
    for(ulOffset = 0; (ulOffset << 9) < *pulLength; ulOffset++)
    {
        //
        // See if this block is being used.
        //
        if(((*ppcBuffer)[(ulOffset >> 3) + 4] &
            (1 << (7 - (ulOffset & 7)))) == 0)
        {
            //
            // This block is not being used, so stop searching.
            //
            break;
        }
    }

    //
    // Set the bit to indicate that this block is being used.
    //
    (*ppcBuffer)[(ulOffset >> 3) + 4] |= 1 << (7 - (ulOffset & 7));

    //
    // See if this block already exists in the file.
    //
    if((ulOffset << 9) < *pulLength)
    {
        //
        // This block is already in the file, so simply return it.
        //
        return(ulOffset << 9);
    }

    //
    // This block will be at the end of the file, so extend the length of the
    // file by one block.
    //
    *pulLength += 512;

    //
    // Allocate new memory for the meta-data.
    //
    pcNewBuffer = (char *)GlobalAlloc(GMEM_FIXED, *pulLength);

    //
    // Copy the meta-data from the original buffer to the new buffer.
    //
    memcpy(pcNewBuffer, *ppcBuffer, *pulLength - 512);

    //
    // Clear the new block.
    //
    memset(pcNewBuffer + *pulLength - 512, 0, 512);

    //
    // Release the old meta-data block.
    //
    GlobalFree(*ppcBuffer);

    //
    // Return the new memory buffer.
    //
    *ppcBuffer = pcNewBuffer;

    //
    // Return the offset of the block.
    //
    return(ulOffset << 9);
}

//****************************************************************************
//
// Adds a directory entry for the specified file.
//
//****************************************************************************
unsigned long
AddFileName(char **ppcBuffer, unsigned long *pulLength,
            unsigned short *pusFileName)
{
    unsigned long ulOffset, ulIdx;

    //
    // Get the offset of the first directory block.
    //
    ulOffset = *(unsigned long *)(*ppcBuffer);
    if((ulOffset + 512) > *pulLength)
    {
        return(-1);
    }

    //
    // Loop forever.  We will explicitly break out of this loop when we find an
    // unused directory entry.
    //
    while(1)
    {
        //
        // Loop through the four directory entries in this block.
        //
        for(ulIdx = 0; ulIdx < 4; ulIdx++)
        {
            //
            // See if this entry is unused.
            //
            if((*ppcBuffer)[ulOffset + (ulIdx * 127) + 4] == '\0')
            {
                break;
            }
        }

        //
        // Stop looking if we found an unused directory entry.
        //
        if(ulIdx != 4)
        {
            break;
        }

        //
        // See if there is a next block in the directory.
        //
        if(*(unsigned long *)(*ppcBuffer + ulOffset) == 0)
        {
            //
            // Allocate a new block for the directory.
            //
            *(unsigned long *)(*ppcBuffer + ulOffset) =
                GetUnusedBlock(ppcBuffer, pulLength);
        }

        //
        // Go to the next block of the directory.
        //
        ulOffset = *(unsigned long *)(*ppcBuffer + ulOffset);
        if((ulOffset + 512) > *pulLength)
        {
            return(-1);
        }
    }

    //
    // Copy the file name into the directory entry we just found.
    //
    memcpy((*ppcBuffer) + ulOffset + (ulIdx * 127) + 4, pusFileName, 122);

    //
    // Allocate a new block for the actual meta-data.
    //
    *(unsigned long *)((*ppcBuffer) + ulOffset + (ulIdx * 127) + 127) =
        GetUnusedBlock(ppcBuffer, pulLength);

    //
    // Return the offset of the directory entry.
    //
    return(ulOffset + (ulIdx * 127) + 4);
}

//****************************************************************************
//
// Retrieves the meta-data associated with the given file name.
//
//****************************************************************************
unsigned long
GetAudibleMetaData(unsigned long ulDrive, unsigned short *pusFileName,
                   char *pcData, unsigned long ulSize)
{
    char *pcBuffer;
    unsigned long ulCopy, ulOffset, ulLength;

    //
    // Read the meta-data file from the device.
    //
    if(ReadMetaData(ulDrive, &pcBuffer, &ulLength) == 0)
    {
        return(0);
    }

    //
    // Find the directory entry for this file.
    //
    ulOffset = FindFileName(pcBuffer, ulLength, pusFileName);
    if(ulOffset == -1)
    {
        GlobalFree(pcBuffer);
        return(0);
    }

    //
    // Get the offset of the meta-data block.
    //
    ulOffset = *(unsigned long *)(pcBuffer + ulOffset + 123);
    if(ulOffset > ulLength)
    {
        GlobalFree(pcBuffer);
        return(0);
    }

    //
    // Loop through the meta-data, copying it to the output buffer.
    //
    while(ulOffset && ulSize)
    {
        //
        // Determine the number of bytes to copy from this block.
        //
        ulCopy = ulSize > 508 ? 508 : ulSize;

        //
        // Copy the meta-data from this block.
        //
        memcpy(pcData, pcBuffer + ulOffset + 4, ulCopy);

        //
        // Increment the output buffer pointer and decrement the size by the
        // number of bytes just copied into the buffer.
        //
        pcData += ulCopy;
        ulSize -= ulCopy;

        //
        // Skip to the next block of meta-data.
        //
        ulOffset = *(unsigned long *)(pcBuffer + ulOffset);
        if(ulOffset > ulLength)
        {
            GlobalFree(pcBuffer);
            return(0);
        }
    }

    //
    // Free the buffer containing the meta-data.
    //
    GlobalFree(pcBuffer);

    //
    // Success.
    //
    return(1);
}

//****************************************************************************
//
// Saves the meta-data associated with the given file name.
//
//****************************************************************************
unsigned long
SetAudibleMetaData(unsigned long ulDrive, unsigned short *pusFileName,
                   char *pcData, unsigned long ulSize)
{
    char *pcBuffer;
    unsigned long ulCopy, ulOffset, ulLength;

    //
    // Read the meta-data file from the device.
    //
    if(ReadMetaData(ulDrive, &pcBuffer, &ulLength) == 0)
    {
        return(0);
    }

    //
    // Find the directory entry for this file.
    //
    ulOffset = FindFileName(pcBuffer, ulLength, pusFileName);
    if(ulOffset == -1)
    {
        //
        // A directory entry does not yet exist for this file, so add one now.
        //
        ulOffset = AddFileName(&pcBuffer, &ulLength, pusFileName);
        if(ulOffset == -1)
        {
            GlobalFree(pcBuffer);
            return(0);
        }
    }

    //
    // Get the offset of the meta-data block.
    //
    ulOffset = *(unsigned long *)(pcBuffer + ulOffset + 123);
    if(ulOffset > ulLength)
    {
        GlobalFree(pcBuffer);
        return(0);
    }

    //
    // Loop through the meta-data, copying it from the input buffer.
    //
    while(ulSize)
    {
        //
        // Determine the number of bytes to copy from this block.
        //
        ulCopy = ulSize > 508 ? 508 : ulSize;

        //
        // Copy the meta-data from this block.
        //
        memcpy(pcBuffer + ulOffset + 4, pcData, ulCopy);

        //
        // Increment the input buffer pointer and decrement the size by the
        // number of bytes just copied from the buffer.
        //
        pcData += ulCopy;
        ulSize -= ulCopy;

        //
        // See if this is the end of the meta-data for this file.
        //
        if(!ulSize)
        {
            //
            // Set the next block address to zero.
            //
            *(unsigned long *)(pcBuffer + ulOffset) = 0;
        }
        else
        {
            //
            // Allocate a new block for the next portion of the meta-data.
            //
            *(unsigned long *)(pcBuffer + ulOffset) =
                GetUnusedBlock(&pcBuffer, &ulLength);

            //
            // Get the offset of the new block.
            //
            ulOffset = *(unsigned long *)(pcBuffer + ulOffset);
        }
    }

    //
    // Write the updated meta-data file back to the device.
    //
    WriteMetaData(ulDrive, pcBuffer, ulLength);

    //
    // Success.
    //
    return(1);
}

//****************************************************************************
//
// Deletes the meta-data associated with the given file name.
//
//****************************************************************************
unsigned long
RemoveAudibleMetaData(unsigned long ulDrive, unsigned short *pusFileName)
{
    char *pcBuffer;
    unsigned long ulOffset, ulNewOffset, ulLength;

    //
    // Read the meta-data file from the device.
    //
    if(ReadMetaData(ulDrive, &pcBuffer, &ulLength) == 0)
    {
        return(0);
    }

    //
    // Find the directory entry for this file.
    //
    ulOffset = FindFileName(pcBuffer, ulLength, pusFileName);
    if(ulOffset == -1)
    {
        GlobalFree(pcBuffer);
        return(0);
    }

    //
    // Get the offset of the meta-data block.
    //
    ulNewOffset = *(unsigned long *)(pcBuffer + ulOffset + 123);

    //
    // Clear the directory entry for this file.
    //
    memset(pcBuffer + ulOffset, 0, 127);

    //
    // Advance to the first meta-data block.
    //
    ulOffset = ulNewOffset;

    //
    // Loop through all the meta-data blocks.
    //
    while(ulOffset)
    {
        //
        // Do nothing further if this block is not a valid block.
        //
        if((ulOffset + 512) > ulLength)
        {
            break;
        }

        //
        // Get the offset of the next meta-data block.
        //
        ulNewOffset = *(unsigned long *)(pcBuffer + ulOffset);

        //
        // Clear this block.
        //
        memset(pcBuffer + ulOffset, 0, 512);

        //
        // Mark this block as being unused.
        //
        pcBuffer[(ulOffset >> 12) + 4] &= ~(1 << (7 - ((ulOffset >> 9) & 7)));

        //
        // Advance to the next block.
        //
        ulOffset = ulNewOffset;
    }

    //
    // Write the updated meta-data file back to the device.
    //
    WriteMetaData(ulDrive, pcBuffer, ulLength);

    //
    // Success.
    //
    return(1);
}

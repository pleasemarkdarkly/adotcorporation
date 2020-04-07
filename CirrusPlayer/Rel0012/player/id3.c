//****************************************************************************
//
// ID3.C - ID3v1 and ID3v2.3.0 tag decoder.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include "globals.h"

//****************************************************************************
//
// ID3IsV1Tag determines if the given sequence of bytes constitutes an ID3v1
// tag.
//
//****************************************************************************
static unsigned long
ID3IsV1Tag(unsigned char *pucBuffer)
{
    //
    // The first three characters are "TAG" for an ID3v1 tag.
    //
    if((pucBuffer[0] == 'T') && (pucBuffer[1] == 'A') && (pucBuffer[2] == 'G'))
    {
        return(1);
    }

    //
    // This is not an ID3v1 tag.
    //
    return(0);
}

//****************************************************************************
//
// ID3IsV2Tag determines if the given sequence of bytes constitutes an ID3v2
// tag.  The length of the tag is returned if it is a valid tag.
//
//****************************************************************************
static unsigned long
ID3IsV2Tag(unsigned char *pucBuffer)
{
    //
    // The first three bytes of the tag should be "ID3".
    //
    if((pucBuffer[0] != 'I') || (pucBuffer[1] != 'D') || (pucBuffer[2] != '3'))
    {
        return(0);
    }

    //
    // The next byte should be the value 3 (i.e. we support ID3v2.3.0).
    //
    if(pucBuffer[3] != 3)
    {
        return(0);
    }

    //
    // The next byte should be less than 0xff.
    //
    if(pucBuffer[4] == 0xff)
    {
        return(0);
    }

    //
    // We don't care about the next byte.  The following four bytes should be
    // less than 0x80.
    //
    if((pucBuffer[6] >= 0x80) || (pucBuffer[7] >= 0x80) ||
       (pucBuffer[8] >= 0x80) || (pucBuffer[9] >= 0x80))
    {
        return(0);
    }

    //
    // Return the length of the ID3v2 tag.
    //
    return((pucBuffer[6] << 21) | (pucBuffer[7] << 14) |
           (pucBuffer[8] << 7) | pucBuffer[9]);
}

//****************************************************************************
//
// ID3UnsynchronizeTag performs an unsynchronization of the portion of the
// ID3v2 tag current read into memory.
//
//****************************************************************************
static void
ID3UnsynchronizeTag(unsigned char *pucBuffer, unsigned long ulStart,
                    unsigned long *pulValid)
{
    unsigned long ulPos;

    //
    // Go through all the bytes in the buffer, replacing 0xff 0x00 with 0xff.
    //
    for(ulPos = ulStart; ulPos < *pulValid; ulPos++)
    {
        //
        // See if the current byte pair is 0xff 0x00.
        //
        if((pucBuffer[ulPos] == 0xff) && (pucBuffer[ulPos + 1] == 0x00))
        {
            //
            // We need to replace this with 0xff, so simply copy the remainder
            // of the buffer down.
            //
            memcpy(pucBuffer + ulPos + 1, pucBuffer + ulPos + 2,
                   *pulValid - ulPos - 2);

            //
            // There is one less valid byte of data in the buffer.
            //
            (*pulValid)--;
        }
    }
}

//****************************************************************************
//
// ID3GetMoreTagData reads the next page of data from the file.
//
//****************************************************************************
static void
ID3GetMoreTagData(tFile *pFile, unsigned char *pucBuffer,
                  unsigned long *pulStart, unsigned long *pulValid,
                  unsigned long *pulTagLength, unsigned long ulFlags)
{
    unsigned long ulTemp;

    //
    // If the start pointer is past the end of the valid data in the buffer,
    // then read in data until that is not the case.
    //
    while(*pulStart >= *pulValid)
    {
        //
        // Copy the last four bytes of data to the beginning of the buffer.
        //
        memcpy(pucBuffer, pucBuffer + *pulValid - 4, 4);

        //
        // Update the starting position and count of valid bytes in the buffer.
        //
        *pulStart -= *pulValid - 4;
        *pulValid = 4;

        //
        // Read the next 512 bytes from the file.
        //
        ulTemp = FSRead(pFile, pucBuffer + 4, 512);

        //
        // Update the remaining length of the tag.
        //
        if(ulTemp < *pulTagLength)
        {
            //
            // The remaining data in the tag is greater than the data just
            // read, so decrement the remaining tag length by the number of
            // bytes just read.
            //
            *pulTagLength -= ulTemp;
        }
        else
        {
            //
            // The remaining data in the tag is entirely contained in the data
            // just read, so update the length of the data just read to be the
            // number of bytes remaining in the tag, and set the remaining
            // length of the tag to zero.
            //
            ulTemp = *pulTagLength;
            *pulTagLength = 0;
        }

        //
        // If necessary, unsynchronize this portion of the tag.
        //
        if(ulFlags & 0x80)
        {
            //
            // We need to unsynchronize across the boundary of where we
            // performed the to reads of data, so increment the count of new
            // bytes by one.
            //
            ulTemp++;

            //
            // Unsynchronize the new portion of the file, including the
            // previously read byte.
            //
            ID3UnsynchronizeTag(pucBuffer, 3, &ulTemp);

            //
            // Decrement the count of new bytes by one.
            //
            ulTemp--;
        }

        //
        // Add the number of bytes read to the count of valid bytes.
        //
        *pulValid += ulTemp;
    }

    //
    // Determine the byte that will be at the beginning of the buffer.
    //
    ulTemp = *pulValid - ((*pulValid - *pulStart + 3) & ~3);

    //
    // Copy the remaining data to the beginning of the buffer.
    //
    memcpy(pucBuffer, pucBuffer + ulTemp, *pulValid - ulTemp);

    //
    // Update the starting position and count of valid bytes in the buffer.
    //
    *pulStart -= ulTemp;
    *pulValid -= ulTemp;

    //
    // Read the next 512 bytes from the file.
    //
    ulTemp = FSRead(pFile, pucBuffer + *pulValid, 512);

    //
    // Update the remaining length of the tag.
    //
    if(ulTemp < *pulTagLength)
    {
        //
        // The remaining data in the tag is greater than the data just read,
        // so decrement the remaining tag length by the number of bytes just
        // read.
        //
        *pulTagLength -= ulTemp;
    }
    else
    {
        //
        // The remaining data in the tag is entirely contained in the data just
        // read, so update the length of the data just read to be the number of
        // bytes remaining in the tag, and set the remaining length of the tag
        // to zero.
        //
        ulTemp = *pulTagLength;
        *pulTagLength = 0;
    }

    //
    // If necessary, unsynchronize this portion of the tag.
    //
    if(ulFlags & 0x80)
    {
        //
        // We need to unsynchronize across the boundary of where we performed
        // the to reads of data, so increment the count of new bytes by one.
        //
        ulTemp++;

        //
        // Unsynchronize the new portion of the file, including the previously
        // read byte.
        //
        ID3UnsynchronizeTag(pucBuffer, *pulValid - 1, &ulTemp);

        //
        // Decrement the count of new bytes by one.
        //
        ulTemp--;
    }

    //
    // Increment the count of valid bytes by the number just read.
    //
    *pulValid += ulTemp;
}

//****************************************************************************
//
// ID3FindTags determines if any ID3 tags are present in the given file and
// updates the start position and/or length of the data based on the presence
// of the tags.
//
//****************************************************************************
void
ID3FindTags(tFile *pFile, unsigned long *pulStart, unsigned long *pulLength,
            unsigned char *pucBuffer)
{
    unsigned long ulLength;

    //
    // First, see if we have an ID3v1 tag, which appears at the end of the
    // file.  Seek to the end of the file minus 128 bytes.
    //
    FSSeek(pFile, (*pulStart + *pulLength - 128) & ~511);

    //
    // Read 1K of data from the file.
    //
    FSRead(pFile, pucBuffer, 1024);

    //
    // See if this is an ID3v1 tag.
    //
    if(ID3IsV1Tag(pucBuffer + ((*pulStart + *pulLength - 128) & 511)))
    {
        //
        // There is an ID3v1 tag at the end of the file, so remove the 128
        // bytes in the tag from the length of the file.
        //
        *pulLength -= 128;
    }

    //
    // Now, see if we have an ID3v2 tag.  Seek to the beginning of the file
    // data.
    //
    FSSeek(pFile, *pulStart & ~511);

    //
    // Read 1K of data from the file.
    //
    FSRead(pFile, pucBuffer, 1024);

    //
    // See if this is an ID3v2 tag.
    //
    if((ulLength = ID3IsV2Tag(pucBuffer + (*pulStart & 511))) != 0)
    {
        //
        // There is an ID3v2 tag at the beginning of the file.  The actual file
        // data starts after the ID3v2 tag, and the length of the file is
        // decremented by the length of the tag.
        //
        *pulStart += ulLength + 10;
        *pulLength -= ulLength + 10;
    }
}

//****************************************************************************
//
// ID3GetTagValue reads the value of either the artist or the song title from
// the ID3 tag.
//
//****************************************************************************
unsigned long
ID3GetTagValue(tFile *pFile, unsigned long ulStart, unsigned long ulLength,
               unsigned char *pucBuffer, char *pcTag, unsigned char *pucValue,
               unsigned long ulValueLength)
{
    unsigned long ulFlags, ulValid, ulTagLength, ulTemp;

    //
    // First, see if we have an ID3v2 tag.  Seek to the beginning of the file.
    //
    FSSeek(pFile, ulStart & ~511);

    //
    // Read 1K of data from the file.
    //
    ulValid = FSRead(pFile, pucBuffer, 1024);

    //
    // Is there an ID3v2 tag at the beginning of the file?
    //
    if(ID3IsV2Tag(pucBuffer + (ulStart & 511)))
    {
        //
        // Move to the first byte of the tag header.
        //
        ulStart &= 511;

        //
        // Get the flags from the ID3v2 tag header.
        //
        ulFlags = pucBuffer[ulStart + 5];

        //
        // Get the length of the ID3v2 tag.
        //
        ulTagLength = (pucBuffer[ulStart + 6] << 21) |
                      (pucBuffer[ulStart + 7] << 14) |
                      (pucBuffer[ulStart + 8] << 7) |
                      pucBuffer[ulStart + 9];

        //
        // Move to the first byte after the tag header.
        //
        ulStart += 10;

        //
        // Decrement the tag length by the number of bytes that have already
        // been read into the buffer.
        //
        if(ulTagLength > (ulValid - ulStart))
        {
            ulTagLength -= ulValid - ulStart;
        }
        else
        {
            ulValid = ulStart + ulTagLength;
            ulTagLength = 0;
        }

        //
        // If we have unsynchronization in the tag, then unsynchronize the
        // current portion of the tag.
        //
        if(ulFlags & 0x80)
        {
            ID3UnsynchronizeTag(pucBuffer, ulStart, &ulValid);
        }

        //
        // Start looking for the specified tag.
        //
        while(ulTagLength || (ulStart < ulValid))
        {
            //
            // See if there is enough data in the buffer.
            //
            if(((ulStart + 10) > ulValid) && ulTagLength)
            {
                //
                // Get more data from the file.
                //
                ID3GetMoreTagData(pFile, pucBuffer, &ulStart, &ulValid,
                                  &ulTagLength, ulFlags);
            }

            //
            // If the name of this frame is all zeros, then we have reached
            // the padding at the end of the tag.
            //
            if((pucBuffer[ulStart] == 0) && (pucBuffer[ulStart + 1] == 0) &&
               (pucBuffer[ulStart + 2] == 0) && (pucBuffer[ulStart + 3] == 0))
            {
                break;
            }

            //
            // See if the name of this frame matches the frame we are looking
            // for.
            //
            if((pucBuffer[ulStart] == pcTag[0]) &&
               (pucBuffer[ulStart + 1] == pcTag[1]) &&
               (pucBuffer[ulStart + 2] == pcTag[2]) &&
               (pucBuffer[ulStart + 3] == pcTag[3]))
            {
                //
                // If this frame contains compression or encryption, then we
                // do can not handle it.
                //
                if(!(pucBuffer[ulStart + 9] & 0xc0))
                {
                    //
                    // Is this a text frame (Txxx)?
                    //
                    if(pcTag[0] == 'T')
                    {
                        //
                        // Get the length of this frame.
                        //
                        ulTemp = (pucBuffer[ulStart + 4] << 24) |
                                 (pucBuffer[ulStart + 5] << 16) |
                                 (pucBuffer[ulStart + 6] << 8) |
                                 pucBuffer[ulStart + 7];

                        //
                        // If this frame has a grouping identity, then we need
                        // to skip the grouping id byte.
                        //
                        if(pucBuffer[ulStart + 9] & 0x20)
                        {
                            ulStart += 11;
                            ulTemp--;
                        }
                        else
                        {
                            ulStart += 10;
                        }

                        //
                        // Get more data from the file.
                        //
                        ID3GetMoreTagData(pFile, pucBuffer, &ulStart, &ulValid,
                                          &ulTagLength, ulFlags);

                        //
                        // Extract the string if the text encoding is ASCII.
                        //
                        if(pucBuffer[ulStart] == 0)
                        {
                            //
                            // Copy the string from the tag frame.
                            //
                            memcpy(pucValue, pucBuffer + ulStart + 1,
                                   ulValueLength);

                            //
                            // Null terminate the string at the appropriate
                            // place, either at the end of the string, or at
                            // the end of the buffer used to hold the string,
                            // whichever is less.
                            //
                            if(ulTemp < ulValueLength)
                            {
                                pucValue[ulTemp - 1] = 0;
                            }
                            else
                            {
                                pucValue[ulValueLength - 1] = 0;
                            }

                            //
                            // Success.
                            //
                            return(1);
                        }
                    }
                }
            }

            //
            // Get the length of this frame.
            //
            ulTemp = (pucBuffer[ulStart + 4] << 24) |
                     (pucBuffer[ulStart + 5] << 16) |
                     (pucBuffer[ulStart + 6] << 8) |
                     pucBuffer[ulStart + 7] + 10;

            //
            // Skip this frame.
            //
            ulStart += ulTemp;
        }
    }

    //
    // Now, see if we have an ID3v1 tag.  Seek to the end of the file (the
    // length has already been reduced by 128 bytes if the tag is present).
    //
    FSSeek(pFile, (ulLength - 128) & ~511);

    //
    // Read 1K of data from the file.
    //
    FSRead(pFile, pucBuffer, 1024);

    //
    // Is there an ID3v1 tag at the end of the file?
    //
    if(ID3IsV1Tag(pucBuffer + ((ulLength - 128) & 511)))
    {
        //
        // Should we return the artist name (TPE1)?
        //
        if(memcmp(pcTag, "TPE1", 4) == 0)
        {
            //
            // Get the artist name from the ID3v1 tag.
            //
            ulTemp = 30;
            memcpy(pucValue, pucBuffer + ((ulLength - 128) & 511) + 33,
                   ulValueLength < ulTemp ? ulValueLength : ulTemp);
        }

        //
        // Should we return the album name (TALB)?
        //
        else if(memcmp(pcTag, "TALB", 4) == 0)
        {
            //
            // Get the album name from the ID3v1 tag.
            //
            ulTemp = 30;
            memcpy(pucValue, pucBuffer + ((ulLength - 128) & 511) + 63,
                   ulValueLength < ulTemp ? ulValueLength : ulTemp);
        }

        //
        // Should we return the song name (TIT2)?
        //
        else if(memcmp(pcTag, "TIT2", 4) == 0)
        {
            //
            // Get the song name from the ID3v1 tag.
            //
            ulTemp = 30;
            memcpy(pucValue, pucBuffer + ((ulLength - 128) & 511) + 3,
                   ulValueLength < ulTemp ? ulValueLength : ulTemp);
        }

        //
        // Should we return the year (TYER)?
        //
        else if(memcmp(pcTag, "TYER", 4) == 0)
        {
            //
            // Get the year from the ID3v1 tag.
            //
            ulTemp = 4;
            memcpy(pucValue, pucBuffer + ((ulLength - 128) & 511) + 93,
                   ulValueLength < ulTemp ? ulValueLength : ulTemp);
        }

        //
        // For all other frame types, they either do not exist in an ID3v1 tag
        // or we do not know how to handle them.
        //
        else
        {
            //
            // Return a failure.
            //
            return(0);
        }

        //
        // Make sure that the string ends with a zero.
        //
        if(ulValueLength <= ulTemp)
        {
            pucValue[ulValueLength - 1] = 0;
        }
        else
        {
            pucValue[ulTemp] = 0;
        }

        //
        // Success.
        //
        return(1);
    }

    //
    // There are no ID3 tags, so we could not get the specified tag value.
    //
    return(0);
}

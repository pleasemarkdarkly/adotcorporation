//****************************************************************************
//
// MP3.C - Codec interface driver for the MP3 decoder.
//
// Copyright (c) 1999,2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include "globals.h"
#ifdef SUPPORT_MP3
#include "mp3/mpgdata.h"
#include "mp3/mpgaudio.h"
#include "buffer/buffer.h"
#include "src/src.h"

//****************************************************************************
//
// The name of this codec.
//
//****************************************************************************
static const unsigned short pusCodecName[] =
{
    'M', 'P', '3', ' ', 'M', 'P', 'E', 'G', ' ', 'L', 'a', 'y',
    'e', 'r', ' ', '3', '\0'
};

//****************************************************************************
//
// A structure which defines the persistent state of the MP3 decoder.
//
//****************************************************************************
typedef struct
{
    //
    // A pointer to the buffer containing the persistent internal state of the
    // MP3 decoder library.
    //
    tMPEGInstance *pMPEGInstance;

    //
    // The file from which we read data.
    //
    tFile *pFile;

    //
    // A buffer to contain the encoded MP3 audio.
    //
    char pcEncodedData[2048];

    //
    // Buffers to contain the decoded MP3 audio.
    //
    short psLeft[MP3_MAX_PCM_LENGTH + MP3_MAX_PCM_LENGTH + NUMTAPS - 1];
    short psRight[MP3_MAX_PCM_LENGTH + MP3_MAX_PCM_LENGTH + NUMTAPS - 1];

    //
    // The buffer to which we write decoded MP3 data.
    //
    BufferState *pOutput;

    //
    // The MP3 bitstream pointer.
    //
    tMPEGBitstream sBS;

    //
    // The decoded MP3 header information.
    //
    tMPEGHeader sHdr;

    //
    // The number of bytes in pcEncodedData which contain valid encoded MP3
    // data.
    //
    unsigned short usValid;

    //
    // The sample rate of the decoded PCM stream.
    //
    unsigned short usSampleRate;

    //
    // The number of channels in the file.
    //
    unsigned char ucChannels;

    //
    // A boolean which is true if this file uses VBR.
    //
    unsigned char ucIsVBR;

    //
    // An unused half-word to pad the next member to a word boundary.
    //
    unsigned short usUnused;

    //
    // The offset of the first frame of the file.
    //
    unsigned long ulFirstFrame;

    //
    // The byte length of the file.
    //
    unsigned long ulLength;

    //
    // The time length of the file.
    //
    unsigned long ulTimeLength;

    //
    // The bit rate of the MP3 file.
    //
    unsigned long ulBitRate;

    //
    // The number of samples which have been decoded.
    //
    unsigned long ulTimePos;

    //
    // The seek points for VBR files.
    //
    unsigned char pucVBRSeek[100];

    //
    // A buffer to contain the song title, if known.
    //
    unsigned short pusTitle[64];

    //
    // A buffer to contain the song author, if known.
    //
    unsigned short pusArtist[64];
} tMP3;

//****************************************************************************
//
// The following is a mapping from the sample rate descriptor in the
// tMPEGHeader structure to the integer number of samples per second.
//
//****************************************************************************
static const unsigned short usSRMap[] = { 11025, 12000,  8000, 0,
                                          22050, 24000, 16000, 0,
                                          44100, 48000, 32000, 0 };

//****************************************************************************
//
// The following is a mapping from the bitrate_index and ID bit in the MPEG
// sync header to a the bitrate of the frame.  The first 16 entries are for
// ID=0 (i.e. MPEG-2 or MPEG-2.5 half sample rate) and the second 16 entries
// are for ID=1 (i.e. MPEG-1 full sample rate).
//
//****************************************************************************
static const unsigned short usBRMap[] = {   0,   8,  16,  24,
                                           32,  40,  48,  56,
                                           64,  80,  96, 112,
                                          128, 144, 160,   0,
                                            0,  32,  40,  48,
                                           56,  64,  80,  96,
                                          112, 128, 160, 192,
                                          224, 256, 320,   0 };

//****************************************************************************
//
// MP3InitBitstream initializes the stream to the given byte offset in the
// file.
//
//****************************************************************************
static void
MP3InitBitstream(tMP3 *pMP3, unsigned long ulFilePos)
{
    //
    // Seek to the correct block of data in the file.
    //
    FSSeek(pMP3->pFile, ulFilePos & ~511);

    //
    // Read the first four pages from this position.
    //
    pMP3->usValid = FSReadBS(pMP3->pFile, pMP3->pcEncodedData, 2048);

    //
    // Initialize the bitstream pointer structure.
    //
    pMP3->sBS.bufptr = (unsigned int *)(pMP3->pcEncodedData +
                                        (ulFilePos & 508));
    pMP3->sBS.bitidx = 8 * (ulFilePos & 3);
}

//****************************************************************************
//
// MP3FindNextFrame finds the next frame in the input MP3 bitstream.
//
//****************************************************************************
static unsigned long
MP3FindNextFrame(tMP3 *pMP3)
{
    unsigned long ulRead;

    //
    // Make sure the bitstream pointer structure makes sense (i.e. it is
    // pointing to a valid bit in a word, not at the bit following the last
    // bit of the previous word).
    //
    if(pMP3->sBS.bitidx == 32)
    {
        pMP3->sBS.bufptr++;
        pMP3->sBS.bitidx = 0;
    }

    //
    // If the bitstream pointer is not at the beginning of the local bitstream
    // buffer, then copy the remaining data back to the beginning.
    //
    if((int)pMP3->sBS.bufptr != (int)pMP3->pcEncodedData)
    {
        //
        // Copy the remainder of the data from the end of the local bitstream
        // buffer to the beginning.
        //
        memcpy(pMP3->pcEncodedData, pMP3->sBS.bufptr,
               2048 - ((int)pMP3->sBS.bufptr -
                       (int)pMP3->pcEncodedData));

        //
        // Update the count of valid bytes in the local bitstream buffer.
        //
        ulRead = (int)pMP3->sBS.bufptr - (int)pMP3->pcEncodedData;
        if(ulRead > pMP3->usValid)
        {
            pMP3->usValid = 0;
        }
        else
        {
            pMP3->usValid -= (int)pMP3->sBS.bufptr - (int)pMP3->pcEncodedData;
        }

        //
        // Update the bitstream pointer structure.
        //
        pMP3->sBS.bufptr = (unsigned int *)pMP3->pcEncodedData;
    }

    //
    // See if there is any additional MP3 data that we can copy into the local
    // bitstream buffer.
    //
    if(pMP3->usValid <= (2048 - 512))
    {
        //
        // Compute the number of bytes to read.
        //
        ulRead = (2048 - pMP3->usValid) & ~511;

        //
        // Read MP3 data into the end of the local bitstream buffer.  Since the
        // ARM MP3 decoder needs the bytes within each word swapped, perform
        // the byte swap as we read.
        //
        pMP3->usValid += FSReadBS(pMP3->pFile,
                                  pMP3->pcEncodedData + pMP3->usValid,
                                  ulRead);
    }

    //
    // Find the next sync word.
    //
    if(MP3SearchForSyncword(pMP3->pMPEGInstance, &(pMP3->sBS),
                            pMP3->usValid * 8) != eNoErr)
    {
        //
        // We could not find a sync word, so return an error.
        //
        return(0);
    }

    //
    // Decode the header.
    //
    if(MP3DecodeInfo(pMP3->pMPEGInstance, &(pMP3->sBS),
                     &(pMP3->sHdr)) != eNoErr)
    {
        //
        // Skip back past the word that was consumed by the attempt to decode
        // the header.
        //
        pMP3->sBS.bufptr--;

        //
        // We could not decode the header, so return an error.
        //
        return(0);
    }

    //
    // Success.
    //
    return(1);
}

//****************************************************************************
//
// MP3DecodeVBRHeader determines if the current frame is a Xing VBR header
// frame and decodes it if it is.
//
//****************************************************************************
static unsigned long
MP3DecodeVBRHeader(tMP3 *pMP3)
{
    unsigned char *pucPtr;
    unsigned long ulFlags, ulFrames, ulIdx, ulFrameSize;

    //
    // Get the offset to the beginning of the current frame.
    //
    pucPtr = (unsigned char *)pMP3->sBS.bufptr + (pMP3->sBS.bitidx >> 3);

    //
    // Determine the offset of the header.
    //
    if(pMP3->usSampleRate >= 32000)
    {
        //
        // This is a MPEG-1 file.  Is this a mono or stereo stream?
        //
        if(pMP3->ucChannels == 1)
        {
            //
            // This is a mono stream, so the header starts at the 17st byte of
            // the frame data.
            //
            pucPtr += 17;
        }
        else
        {
            //
            // This is a stereo stream, so the header starts at the 32nd byte
            // of the frame data.
            //
            pucPtr += 32;
        }

        //
        // The frame size for MPEG-1 files is 1152.
        //
        ulFrameSize = 1152;
    }
    else
    {
        //
        // This is a MPEG-2 file.  Is this a mono or stereo stream?
        //
        if(pMP3->ucChannels == 1)
        {
            //
            // This is a mono stream, so the header starts at the 9th byte of
            // the frame data.
            //
            pucPtr += 9;
        }
        else
        {
            //
            // This is a stereo stream, so the header starts at the 17th byte
            // of the frame.
            //
            pucPtr += 17;
        }

        //
        // The frame size for MPEG-2 files is 576.
        //
        ulFrameSize = 576;
    }

    //
    // See if the "Xing" signature appears in the frame.
    //
    if((pucPtr[3] != 'X') || (pucPtr[2] != 'i') ||
       (pucPtr[1] != 'n') || (pucPtr[0] != 'g'))
    {
        //
        // This is not a Xing VBR header, so return a failure.
        //
        return(0);
    }

    //
    // Skip past the signature.
    //
    pucPtr += 4;

    //
    // Get the header flags from the stream.
    //
    ulFlags = (pucPtr[3] << 24) | (pucPtr[2] << 16) |
              (pucPtr[1] << 8) | pucPtr[0];
    pucPtr += 4;

    //
    // If the number of frames exists in the header, then extract it now.
    //
    if(ulFlags & 1)
    {
        ulFrames = (pucPtr[3] << 24) | (pucPtr[2] << 16) |
                   (pucPtr[1] << 8) | pucPtr[0];
        pucPtr += 4;
    }
    else
    {
        //
        // The count of frames does not exist, so we will not be able to
        // properly support this file.
        //
        return(0);
    }

    //
    // If the file size exists in the header, then skip it.
    //
    if(ulFlags & 2)
    {
        pucPtr += 4;
    }

    //
    // If the seek point table exists in the header, then read it in now.
    //
    if(ulFlags & 4)
    {
        //
        // There are 100 entries in the table, so copy them one by one.
        //
        for(ulIdx = 0; ulIdx < 100; ulIdx++)
        {
            pMP3->pucVBRSeek[ulIdx ^ 3] = *pucPtr++;
        }

        //
        // Since the seek point table exists in the header, indicate that this
        // is a VBR file.
        //
        pMP3->ucIsVBR = 1;
    }
    else
    {
        //
        // Since there is no seek point table in the header, treat this file as
        // a non-VBR file.
        //
        pMP3->ucIsVBR = 0;
    }

    //
    // Now, compute the bitrate from the file size and the count of frames in
    // the file.
    //
    pMP3->ulBitRate = ((FSLength(pMP3->pFile) / ulFrames) *
                       pMP3->usSampleRate) / (ulFrameSize / 8);

    //
    // Success.
    //
    return(1);
}

//****************************************************************************
//
// MP3DecodeID3Tag sees if the MP3 stream has an ID3 tag.  If found, the
// song title and artist is extracted from the tag.
//
//****************************************************************************
static void
MP3DecodeID3Tag(tMP3 *pMP3)
{
    unsigned long ulDataStart, ulLength;

    //
    // Save the original position and length of the MP3 data.
    //
    ulDataStart = pMP3->ulFirstFrame;
    ulLength = pMP3->ulLength + pMP3->ulFirstFrame;

    //
    // See if the file contains any ID3 tags.  If it does, ulFirstFrame and
    // ulLength in the MP3 persistent data structure will be updated to reflect
    // the start and length of the actual MP3 data, without the ID3 tags.
    //
    ID3FindTags(pMP3->pFile, &pMP3->ulFirstFrame, &pMP3->ulLength,
                (unsigned char *)pMP3->pcEncodedData);

    //
    // Get the value of the "TPE1" tag, which contains the name of the artist.
    //
    if(ID3GetTagValue(pMP3->pFile, ulDataStart, ulLength,
                      (unsigned char *)pMP3->pcEncodedData, "TPE1",
                      pMP3->pusArtist, 64) == 0)
    {
        pMP3->pusArtist[0] = 0;
    }

    //
    // Get the value of the "TIT2" tag, which contains the name of the song.
    //
    if(ID3GetTagValue(pMP3->pFile, ulDataStart, ulLength,
                      (unsigned char *)pMP3->pcEncodedData, "TIT2",
                      pMP3->pusTitle, 64) == 0)
    {
        pMP3->pusTitle[0] = 0;
    }
}

//****************************************************************************
//
// Determines if the given file is a MP3 file, and returns the offset of the
// first frame if so.
//
//****************************************************************************
static unsigned long
MP3IsMP3File(tFile *pFile, unsigned char *pucScratch, unsigned long ulOffset)
{
    unsigned long ulSamplesPerFrame, ulSampleRate, ulBitRate, ulFrameLength;
    unsigned long ulPos, ulCount, ulLoop, ulHdr1, ulHdr2, ulNext;
    unsigned char pucBuffer[16];

    //
    // Seek to the start of the file data.
    //
    FSSeek(pFile, ulOffset & ~511);

    //
    // Read 512 bytes from the file.
    //
    FSRead(pFile, pucScratch, 512);

    //
    // Get the starting offset of the data in the buffer.
    //
    ulPos = ulOffset & 511;

    //
    // See if there are 10 bytes of valid data in the buffer.
    //
    if((ulPos + 10) > 512)
    {
        //
        // Get the count of valid bytes in the buffer.
        //
        ulCount = 512 - ulPos;

        //
        // Copy the valid bytes from the buffer into our temporary buffer.
        //
        memcpy(pucBuffer, pucScratch + ulPos, ulCount);

        //
        // Read the next 512 bytes from the file.
        //
        FSRead(pFile, pucScratch, 512);

        //
        // Copy the next set of valid bytes from the buffer into our temporary
        // buffer.
        //
        memcpy(pucBuffer + ulCount, pucScratch, 16 - ulCount);
    }
    else
    {
        //
        // There are 10 bytes of valid data in the buffer, so simply copy them
        // into our temporary buffer.
        //
        memcpy(pucBuffer, pucScratch + ulPos, 16);
    }

    //
    // See if this is a valid ID3 V2 tag.
    //
    ulCount = ID3IsV2Tag(pucBuffer);
    if(ulCount != 0)
    {
        //
        // There is an ID3 V2 tag, so skip past it.
        //
        ulOffset += ulCount + 10;
    }

    //
    // Seek to the start of the MP3 stream.
    //
    FSSeek(pFile, ulOffset & ~511);

    //
    // Read 512 bytes from the file.
    //
    FSRead(pFile, pucScratch, 512);

    //
    // Get the starting offset of the data in the buffer.
    //
    ulPos = ulOffset & 511;

    //
    // Get the offset of the start of data in the buffer.
    //
    ulOffset &= ~511;

    //
    // Examine the next 2048 bytes of the file (which is four 512 byte chunks)
    // looking for a frame header.
    //
    for(ulLoop = 4; ; ulPos++)
    {
        //
        // See if there are 4 bytes of valid data in the buffer.
        //
        if((ulPos + 4) > 512)
        {
            //
            // If we're examined 2048 bytes and not found a frame header then
            // we will not be able to decode this file.
            //
            if(ulLoop == 0)
            {
                return(-1);
            }

            //
            // Get the count of valid bytes in the buffer.
            //
            ulCount = 512 - ulPos;

            //
            // Copy the valid bytes from the buffer into our temporary buffer.
            //
            memcpy(pucBuffer, pucScratch + ulPos, ulCount);

            //
            // Read the next 512 bytes from the file.
            //
            FSRead(pFile, pucScratch, 512);

            //
            // Indicate the number of bytes in the temporary buffer.
            //
            ulPos = 0 - ulCount;

            //
            // Update the offset of the start of data in the buffer.
            //
            ulOffset += 512;

            //
            // Decrement the count of pages to search.
            //
            ulLoop--;
        }

        //
        // If there are residual bytes in the temporary buffer, then
        // concatenate them with the bytes from the beginning of the current
        // block.
        //
        if(ulPos > 512)
        {
            //
            // Get the first byte from the temporary buffer.
            //
            ulHdr1 = pucBuffer[0 - ulPos] << 24;

            //
            // See if this is the last byte in the temporary buffer.
            //
            if((ulPos + 1) == 0)
            {
                //
                // This was the last byte in the temporary buffer, so get the
                // remaining three bytes from the beginning of the current
                // block.
                //
                ulHdr1 |= ((pucScratch[0] << 16) |
                           (pucScratch[1] << 8) |
                           pucScratch[2]);
            }
            else
            {
                //
                // There is at least one more byte in the temporary buffer, so
                // get the second byte from the temporary buffer.
                //
                ulHdr1 |= pucBuffer[1 - ulPos] << 16;

                //
                // See if this is the last byte in the temporary buffer.
                //
                if((ulPos + 2) == 0)
                {
                    //
                    // This was the last byte in the temporary buffer, so get
                    // the remaining two bytes from the beginning of the
                    // current block.
                    //
                    ulHdr1 |= ((pucScratch[0] << 8) | pucScratch[1]);
                }
                else
                {
                    //
                    // There is one more byte in the temporary buffer, so get
                    // the last byte from the temporary buffer and the first
                    // byte from the current block.
                    //
                    ulHdr1 |= ((pucBuffer[2 - ulPos] << 8) | pucScratch[0]);
                }
            }
        }
        else
        {
            //
            // Get the next four bytes from the current block.
            //
            ulHdr1 = ((pucScratch[ulPos] << 24) |
                      (pucScratch[ulPos + 1] << 16) |
                      (pucScratch[ulPos + 2] << 8) |
                      pucScratch[ulPos + 3]);
        }

        //
        // See if this word starts with 0xFFE, which is the frame sync for MPEG
        // audio.
        //
        if((ulHdr1 & 0xffe00000) != 0xffe00000)
        {
            continue;
        }

        //
        // See that a valid MPEG version was specified.
        //
        if((ulHdr1 & 0x00180000) == 0x00080000)
        {
            continue;
        }

        //
        // Make sure that the file is encoded with layer 3.
        //
        if((ulHdr1 & 0x00060000) != 0x00020000)
        {
            continue;
        }

        //
        // Make sure that a valid bit rate is specified.
        //
        if((ulHdr1 & 0x0000f000) == 0x0000f000)
        {
            continue;
        }

        //
        // Make sure that a valid sample rate is specified.
        //
        if((ulHdr1 & 0x00000c00) == 0x00000c00)
        {
            continue;
        }

        //
        // If the bit rate index is zero (indicating free-format bit rate), we
        // simply assume that this is a valid frame header in a valid file.
        //
        if((ulHdr1 & 0x0000f000) == 0x00000000)
        {
            return(ulOffset + ulPos);
        }

        //
        // Determine if this frame is MPEG-1 or MPEG-2/2.5.
        //
        if((ulHdr1 & 0x00080000) == 0x00080000)
        {
            //
            // This frame is MPEG-1, which has 1152 samples per frame.
            //
            ulSamplesPerFrame = 1152;

            //
            // Get the sample rate of this frame.
            //
            ulSampleRate = usSRMap[((ulHdr1 & 0x00000c00) >> 10) + 8];

            //
            // Get the bit rate of this frame.
            //
            ulBitRate = usBRMap[((ulHdr1 & 0x0000f000) >> 12) + 16];
        }
        else
        {
            //
            // This frame is MPEG-2/2.5, which has 576 samples per frame.
            //
            ulSamplesPerFrame = 576;

            //
            // Determine if this frame is MPEG-2 or MPEG-2.5.
            //
            if((ulHdr1 & 0x00100000) == 0x00100000)
            {
                //
                // This frame is MPEG-2.  Get the sample rate of this frame.
                //
                ulSampleRate = usSRMap[((ulHdr1 & 0x00000c00) >> 10) + 4];
            }
            else
            {
                //
                // This frame is MPEG-2.5.  Get the sample rate of this frame.
                //
                ulSampleRate = usSRMap[(ulHdr1 & 0x00000c00) >> 10];
            }

            //
            // Get the bit rate of this frame.
            //
            ulBitRate = usBRMap[(ulHdr1 & 0x0000f000) >> 12];
        }

        //
        // Determine the length of this frame.
        //
        ulFrameLength = (ulBitRate * 125 * ulSamplesPerFrame) / ulSampleRate;
        if(ulHdr1 & 0x00000200)
        {
            ulFrameLength++;
        }

        //
        // Determine the offset of the next frame header.
        //
        ulNext = ulOffset + ulPos + ulFrameLength;

        //
        // Determine if the next frame header is in the currently read portion
        // of the file.
        //
        if((ulNext - ulOffset) >= 512)
        {
            //
            // Seek to the position within the file that contains the next
            // frame header.
            //
            FSSeek(pFile, ulNext & ~511);

            //
            // Read the 512 bytes from the file.
            //
            FSRead(pFile, pucScratch, 512);
        }

        //
        // Get the offset into the buffer of the next frame header.
        //
        ulNext &= 511;

        //
        // Get the frame header from the buffer.  Note that this could read
        // past the end of the buffer by three bytes.
        //
        ulHdr2 = ((pucScratch[ulNext] << 24) |
                  (pucScratch[ulNext + 1] << 16) |
                  (pucScratch[ulNext + 2] << 8) |
                  pucScratch[ulNext + 3]);

        //
        // See if the next frame header wraps into the next page of the file.
        //
        if(ulNext > 508)
        {
            //
            // Read the next 512 bytes from the file.
            //
            FSRead(pFile, pucScratch, 512);

            //
            // Determine how many bytes to replace from the newly read page.
            //
            switch(ulNext)
            {
                //
                // We have three valid bytes from the previous page.
                //
                case 509:
                {
                    //
                    // Mask off the valid bytes from the previous page.
                    //
                    ulHdr2 &= 0xffffff00;

                    //
                    // Add in the new byte from this page.
                    //
                    ulHdr2 |= pucScratch[0];

                    //
                    // We're done.
                    //
                    break;
                }

                //
                // We have two valid bytes from the previous page.
                //
                case 510:
                {
                    //
                    // Mask off the valid bytes from the previous page.
                    //
                    ulHdr2 &= 0xffff0000;

                    //
                    // Add in the new bytes from this page.
                    //
                    ulHdr2 |= (pucScratch[0] << 8) | pucScratch[1];

                    //
                    // We're done.
                    //
                    break;
                }

                //
                // We have one valid byte from the previous page.
                //
                case 511:
                {
                    //
                    // Mask off the valid byte from the previous page.
                    //
                    ulHdr2 &= 0xff000000;

                    //
                    // Add in the new bytes from this page.
                    //
                    ulHdr2 |= ((pucScratch[0] << 16) |
                               (pucScratch[1] << 8) | pucScratch[2]);

                    //
                    // We're done.
                    //
                    break;
                }
            }
        }

        //
        // See if the two headers match.  We only compare the sync word, ID,
        // layer, and sample rate fields (since the others can change from
        // frame to frame).
        //
        if((ulHdr2 & 0xfffe0c00) == (ulHdr1 & 0xfffe0c00))
        {
            //
            // The two headers match, so return the offset of the first header
            // as the starting position of MP3 data in the file.
            //
            return(ulOffset + ulPos);
        }

        //
        // See if we performed a read to find the next frame header.
        //
        if(FSTell(pFile) != (ulOffset + 512))
        {
            //
            // Seek back to the original file position.
            //
            FSSeek(pFile, ulOffset);

            //
            // Read 512 bytes from the original file position.
            //
            FSRead(pFile, pucScratch, 512);
        }
    }
}

//****************************************************************************
//
// The codec plug-in entry point for the MP3 decoder.
//
//****************************************************************************
unsigned long
MP3Ioctl(unsigned long ulIoctl, unsigned long ulParam1, unsigned long ulParam2,
         unsigned long ulParam3, unsigned long ulParam4)
{
    //
    // Determine what to do based on the specified IOCTL.
    //
    switch(ulIoctl)
    {
        //
        // Return the name for this codec.
        //
        case IOCTL_CODEC_GETNAME:
        {
            const unsigned short **ppusName;

            //
            // The second parameter is a pointer for the name.
            //
            ppusName = (const unsigned short **)ulParam2;

            //
            // Return the name of this codec.  The first four characters are
            // the short name and the string starting at the fifth character
            // is the long name.
            //
            *ppusName = pusCodecName;

            //
            // Success.
            //
            return(1);
        }

        //
        // Return the name of the artist.
        //
        case IOCTL_CODEC_GETARTIST:
        {
            unsigned short **ppusName;
            tMP3 *pMP3;

            //
            // The first parameter is a pointer to the MP3 persistent data.
            //
            pMP3 = (tMP3 *)ulParam1;

            //
            // The second parameter is a pointer for the name.
            //
            ppusName = (unsigned short **)ulParam2;

            //
            // Return the name of the artist.
            //
            *ppusName = pMP3->pusArtist;

            //
            // Success.
            //
            return(1);
        }

        //
        // Return the name of the song
        //
        case IOCTL_CODEC_GETTITLE:
        {
            unsigned short **ppusName;
            tMP3 *pMP3;

            //
            // The first parameter is a pointer to the MP3 persistent data.
            //
            pMP3 = (tMP3 *)ulParam1;

            //
            // The second parameter is a pointer for the name.
            //
            ppusName = (unsigned short **)ulParam2;

            //
            // Return the name of the song.
            //
            *ppusName = pMP3->pusTitle;

            //
            // Success.
            //
            return(1);
        }

        //
        // Return the bitrate at which this file is encoded.
        //
        case IOCTL_CODEC_GETBITRATE:
        {
            unsigned long *pulBitRate;
            tMP3 *pMP3;

            //
            // The first parameter is a pointer to the MP3 persistent data.
            //
            pMP3 = (tMP3 *)ulParam1;

            //
            // The second parameter is a pointer for the bitrate.
            //
            pulBitRate = (unsigned long *)ulParam2;

            //
            // Return the average bitrate of the file.
            //
            *pulBitRate = pMP3->ulBitRate;

            //
            // Success.
            //
            return(1);
        }

        //
        // Return the sample rate at which this file is encoded.
        //
        case IOCTL_CODEC_GETSAMPLERATE:
        {
            unsigned long *pulSampleRate;
            tMP3 *pMP3;

            //
            // The first parameter is a pointer to the MP3 persistent data.
            //
            pMP3 = (tMP3 *)ulParam1;

            //
            // The second parameter is a pointer for the sample rate.
            //
            pulSampleRate = (unsigned long *)ulParam2;

            //
            // Return the sample rate of the file.
            //
            *pulSampleRate = pMP3->usSampleRate;

            //
            // Success.
            //
            return(1);
        }

        //
        // Return the number of channels in the file.
        //
        case IOCTL_CODEC_GETCHANNELS:
        {
            unsigned long *pulChannels;
            tMP3 *pMP3;

            //
            // The first parameter is a pointer to the MP3 persistent data.
            //
            pMP3 = (tMP3 *)ulParam1;

            //
            // The second parameter is a pointer for the number of channels.
            //
            pulChannels = (unsigned long *)ulParam2;

            //
            // Return the number of channels in the file.
            //
            *pulChannels = pMP3->ucChannels;

            //
            // Success.
            //
            return(1);
        }

        //
        // Return the length (in milliseconds) of the file.
        //
        case IOCTL_CODEC_GETLENGTH:
        {
            unsigned long *pulLength;
            tMP3 *pMP3;

            //
            // The first parameter is a pointer to the MP3 persistent data.
            //
            pMP3 = (tMP3 *)ulParam1;

            //
            // The second parameter is a pointer for the number of
            // milliseconds.
            //
            pulLength = (unsigned long *)ulParam2;

            //
            // Return the length of the file.
            //
            *pulLength = pMP3->ulTimeLength;

            //
            // Success.
            //
            return(1);
        }

        //
        // Return the current position (in milliseconds) within the file.
        //
        case IOCTL_CODEC_GETTIME:
        {
            unsigned long *pulTime;
            tMP3 *pMP3;

            //
            // The first parameter is a pointer to the MP3 persistent data.
            //
            pMP3 = (tMP3 *)ulParam1;

            //
            // The second parameter is a pointer for the number of seconds.
            //
            pulTime = (unsigned long *)ulParam2;

            //
            // Determine the time based on the sample rate.
            //
            *pulTime = ((pMP3->ulTimePos / pMP3->usSampleRate) * 1000) +
                       (((pMP3->ulTimePos % pMP3->usSampleRate) * 1000) /
                        pMP3->usSampleRate);

            //
            // Success.
            //
            return(1);
        }

        //
        // Determine if the given file can be decoded.
        //
        case IOCTL_CODEC_QUERY:
        {
            tFile *pFile;
            unsigned char *pucScratch;

            //
            // The second parameter is the file to be checked.
            //
            pFile = (tFile *)ulParam2;

            //
            // The third parameter is a 512 byte scratch buffer.
            //
            pucScratch = (unsigned char *)ulParam3;

            //
            // Determine if this is a MP3 file.
            //
            if(MP3IsMP3File(pFile, pucScratch, ulParam4) == -1)
            {
                return(0);
            }

            //
            // This is a MP3 file.
            //
            return(1);
        }

        //
        // Prepare the codec to encode or decode a file.
        //
        case IOCTL_CODEC_OPEN:
        {
            tMP3 *pMP3;

            //
            // If we are being asked to encode an MP3 file, then return a
            // failure since we can only decode MP3.
            //
            if(ulParam3 & CODEC_OPEN_ENCODE)
            {
                return(0);
            }

            //
            // The first parameter is a pointer to the MP3 persistent state.
            //
            pMP3 = (tMP3 *)ulParam1;

            //
            // Determine the pointer to the MP3 decoder library's persistent
            // state.  This pointer must be aligned on a 2K byte boundary.
            //
            pMP3->pMPEGInstance = (tMPEGInstance *)(((unsigned long)pMP3 +
                                                     sizeof(tMP3) + 2047) &
                                                    0xfffff800);

            //
            // Make sure that we have enough memory for this decoder.
            //
            if(((unsigned long)pMP3->pMPEGInstance + sizeof(tMPEGInstance)) >
               ulExtentOfRAM)
            {
                return(0);
            }

            //
            // Initialize the MP3 library.
            //
            InitMP3Audio(pMP3->pMPEGInstance);

            //
            // Save the pointer to the file structure.
            //
            pMP3->pFile = (tFile *)ulParam4;

            //
            // Save the position of the beginning of the file.
            //
            pMP3->ulFirstFrame = ulParam2;

            //
            // Get the length of the file.
            //
            pMP3->ulLength = FSLength(pMP3->pFile) - pMP3->ulFirstFrame;

            //
            // Decode the ID3 tag if it is present.
            //
            MP3DecodeID3Tag(pMP3);

            //
            // Determine the starting position of the first frame.
            //
            pMP3->ulFirstFrame =
                MP3IsMP3File(pMP3->pFile, (unsigned char *)pMP3->pcEncodedData,
                             pMP3->ulFirstFrame);
            if(pMP3->ulFirstFrame == -1)
            {
                //
                // We could not find a valid first frame, so return an error.
                //
                return(0);
            }

            //
            // Initialize the bitstream.
            //
            MP3InitBitstream(pMP3, pMP3->ulFirstFrame);

            //
            // Find the first frame of the file.
            //
            if(MP3FindNextFrame(pMP3) == 0)
            {
                //
                // If we could not find the first frame of the file, then
                // return an error.
                //
                return(0);
            }

            //
            // Get the sample rate of the file.
            //
            pMP3->usSampleRate = usSRMap[pMP3->sHdr.sample_rate];

            //
            // Get the number of channels in the file.
            //
            pMP3->ucChannels = pMP3->sHdr.numchans;

            //
            // Attempt to decode the VBR header.
            //
            if(MP3DecodeVBRHeader(pMP3) == 0)
            {
                //
                // There is no VBR header on this file, so get the actual
                // bitrate from the sync header.
                //
                pMP3->ulBitRate = usBRMap[((pMP3->sHdr.packed_info &
                                            0x00080000) >> 15) |
                                         ((pMP3->sHdr.packed_info &
                                           0x0000f000) >> 12)] * 1000;

                //
                // If no bitrate is specified, then return an error.
                //
                if(pMP3->ulBitRate == 0)
                {
                    return(0);
                }

                //
                // Indicate that this is not a VBR file.
                //
                pMP3->ucIsVBR = 0;
            }

            //
            // Get the length of the file.
            //
            pMP3->ulTimeLength = (((pMP3->ulLength * 8) / pMP3->ulBitRate) *
                                  1000) +
                                 ((((pMP3->ulLength * 8) % pMP3->ulBitRate) *
                                   1000) / pMP3->ulBitRate);

            //
            // The position is zero.
            //
            pMP3->ulTimePos = 0;

            //
            // There is no output buffer.
            //
            pMP3->pOutput = 0;

            //
            // Success.
            //
            return(1);
        }

        //
        // Set the output buffer for the decoder.
        //
        case IOCTL_CODEC_SETBUFFER:
        {
            tMP3 *pMP3;

            //
            // The first parameter is a pointer to the MP3 persistent state.
            //
            pMP3 = (tMP3 *)ulParam1;

            //
            // The second parameter is a pointer to the output buffer.
            //
            pMP3->pOutput = (BufferState *)ulParam2;

            //
            // Provide the output buffer with our data buffers.
            //
            if(pMP3->ucChannels == 2)
            {
                BufferSetBuffer(pMP3->pOutput, pMP3->psLeft, pMP3->psRight,
                                MP3_MAX_PCM_LENGTH + MP3_MAX_PCM_LENGTH);
            }
            else
            {
                BufferSetBuffer(pMP3->pOutput, pMP3->psLeft, pMP3->psLeft,
                                MP3_MAX_PCM_LENGTH + MP3_MAX_PCM_LENGTH);
            }

            //
            // Success.
            //
            return(1);
        }

        //
        // Determine if the next frame of data can be decoded.
        //
        case IOCTL_CODEC_CAN_DECODE:
        {
            tMP3 *pMP3;

            //
            // The first parameter is a pointer to the MP3 persistent state.
            //
            pMP3 = (tMP3 *)ulParam1;

            //
            // See if there is space available for decoding the next frame.
            //
            if(BufferSpaceAvailable(pMP3->pOutput) < MP3_MAX_PCM_LENGTH)
            {
                //
                // There is not enough space available, so we can not decode
                // the next frame.
                //
                return(0);
            }

            //
            // We can decode the next frame.
            //
            return(1);
        }

        //
        // Decode a frame of data.
        //
        case IOCTL_CODEC_DECODE:
        {
            tMP3 *pMP3;
            short *psLeft, *psRight;
            long lLength;
            tMPEGStatus sStatus;

            //
            // The first parameter is a pointer to the MP3 persistent state.
            //
            pMP3 = (tMP3 *)ulParam1;

            //
            // Wait until there is space in the output buffer.  This tells us
            // when it is OK to write more data into the output buffer.
            //
            while(BufferSpaceAvailable(pMP3->pOutput) < MP3_MAX_PCM_LENGTH)
            {
                //
                // Busy wait until space is available.
                //
            }

            //
            // Get a pointer to the available space in the output buffer.
            //
            BufferGetWritePointer(pMP3->pOutput, &psLeft, &psRight, &lLength);

            //
            // Decode the data frame.
            //
            sStatus = MP3DecodeData(pMP3->pMPEGInstance, psLeft, psRight,
                                    &(pMP3->sBS));
            if((sStatus != eNoErr) && (sStatus != eFrameDiscarded))
            {
                //
                // We could not decode the frame, so see if there is more data
                // to be docoded.
                //
                if((FSTell(pMP3->pFile) < pMP3->ulLength) ||
                   (((int)pMP3->sBS.bufptr - (int)pMP3->pcEncodedData) <
                    pMP3->usValid))
                {
                    //
                    // There is more data to be decoded, so re-initialize the
                    // MP3 library (to try and minimize the artifacting).
                    //
                    InitMP3Audio(pMP3->pMPEGInstance);
                }
                else
                {
                    //
                    // There is no more data to be decoded, so return an error
                    // to indicate that we are at the end of the file.
                    //
                    return(0);
                }
            }

            //
            // Make sure that this frame did not use garbage at the end of the
            // encoded data buffer to complete the decode.
            //
            if(((int)pMP3->sBS.bufptr - (int)pMP3->pcEncodedData) >
               pMP3->usValid)
            {
                //
                // All the data for this frame was not present, so see if there
                // is more data in to be decoded.
                //
                if(FSTell(pMP3->pFile) < pMP3->ulLength)
                {
                    //
                    // There is more data to be decoded, so move the buffer
                    // pointer back to the end of the valid data.
                    //
                    pMP3->sBS.bufptr = (unsigned int *)(pMP3->pcEncodedData +
                                                        pMP3->usValid);
                    pMP3->sBS.bitidx = 32;

                    //
                    // Force the frame status to discarded.
                    //
                    sStatus = eFrameDiscarded;

                    //
                    // Re-initialize the MP3 library (to try and minimize the
                    // artifacting).
                    //
                    InitMP3Audio(pMP3->pMPEGInstance);
                }
                else
                {
                    //
                    // There is no more data to be decoded, so return an error
                    // to indicate that we are at the end of the file.
                    //
                    return(0);
                }
            }

            //
            // Determine the length of each frame, based on the sample rate of
            // the file.
            //
            lLength = pMP3->usSampleRate >= 32000 ? 1152 : 576;

            //
            // If this frame was not discarded, then update the output buffer's
            // write pointer.
            //
            if(sStatus != eFrameDiscarded)
            {
                //
                // Update the output buffer's write pointer.
                //
                BufferUpdateWritePointer(pMP3->pOutput, lLength);

                //
                // Increment the time based on the number of samples.
                //
                pMP3->ulTimePos += lLength;
            }

            //
            // Find the next frame in the bitstream.
            //
            while(MP3FindNextFrame(pMP3) == 0)
            {
                //
                // We could not find another frame, so see if there is more
                // data to be decoded.
                //
                if((FSTell(pMP3->pFile) < pMP3->ulLength) ||
                   (((int)pMP3->sBS.bufptr - (int)pMP3->pcEncodedData) <
                    pMP3->usValid))
                {
                    //
                    // There is more data to be decoded, so advance the buffer
                    // pointer by one byte.
                    //
                    if(pMP3->sBS.bitidx == 32)
                    {
                        pMP3->sBS.bufptr++;
                        pMP3->sBS.bitidx = 8;
                    }
                    else
                    {
                        pMP3->sBS.bitidx += 8;
                    }

                    //
                    // Re-initialize the MP3 library (to try and minimize the
                    // artifacting).
                    //
                    InitMP3Audio(pMP3->pMPEGInstance);
                }
                else
                {
                    //
                    // There is no more data to be decoded, so return an error
                    // to indicate that we are at the end of the file.
                    //
                    return(0);
                }
            }

            //
            // Success.
            //
            return(1);
        }

        //
        // Encode a frame of data.
        //
        case IOCTL_CODEC_CAN_ENCODE:
        case IOCTL_CODEC_ENCODE:
        {
            //
            // We don't support encode, so return an error.
            //
            return(0);
        }

        //
        // Seek to the specified time position.
        //
        case IOCTL_CODEC_SEEK:
        {
            tMP3 *pMP3;
            unsigned long ulPos, ulPos2, ulFrameSize, ulPercent;

            //
            // The first parameter is a pointer to the MP3 persistent state.
            //
            pMP3 = (tMP3 *)ulParam1;

            //
            // Make sure that the specified time is valid.
            //
            if(ulParam2 > pMP3->ulTimeLength)
            {
                ulParam2 = pMP3->ulTimeLength;
            }

            //
            // If this file uses VBR, then we use the seek point table to
            // compute the new position.
            //
            if(pMP3->ucIsVBR)
            {
                //
                // Convert the seek position into a percentage through the
                // file.
                //
                ulPos = (ulParam2 * 100) / pMP3->ulTimeLength;

                //
                // Compute the fractional percentage between this percentage
                // and the next.
                //
                ulPercent = (((ulParam2 * 100) % pMP3->ulTimeLength) * 100) /
                            pMP3->ulTimeLength;

                //
                // Get the two seek points that surround the requested
                // position.
                //
                if(ulPos >= 99)
                {
                    ulPos2 = 256;
                }
                else
                {
                    ulPos2 = pMP3->pucVBRSeek[ulPos + 1];
                }
                if(ulPos > 99)
                {
                    ulPos = 256;
                }
                else
                {
                    ulPos = pMP3->pucVBRSeek[ulPos];
                }

                //
                // Interpolate between the two seek points.
                //
                ulPos = (ulPos * (pMP3->ulLength / 256)) +
                        (((ulPos2 - ulPos) * ulPercent *
                          (pMP3->ulLength / 256)) / 100) + pMP3->ulFirstFrame;

                //
                // The new time position is the requested seek position.  This
                // may be off by a little bit, but there's nothing that can be
                // done about this (other than to decode the file from the
                // beginning).
                //
                pMP3->ulTimePos = ((ulParam2 / 1000) * pMP3->usSampleRate) +
                                  (((ulParam2 % 1000) * pMP3->usSampleRate) /
                                   1000);
            }
            else
            {
                //
                // Figure out the size of each frame based on the sample rate.
                //
                if(pMP3->usSampleRate >= 32000)
                {
                    ulFrameSize = 1152;
                }
                else
                {
                    ulFrameSize = 576;
                }

                //
                // Compute the number of frames that occur before the requested
                // time position.
                //
                ulPos = (((ulParam2 / 1000) * pMP3->usSampleRate) /
                         ulFrameSize) +
                        (((ulParam2 % 1000) * pMP3->usSampleRate) /
                         (ulFrameSize * 1000));

                //
                // Compute the time for the computed frame number.
                //
                pMP3->ulTimePos = ulPos = ulPos * ulFrameSize;

                //
                // Compute the file position based on the actual seek time.
                //
                ulPos = ((ulPos / pMP3->usSampleRate) *
                         (pMP3->ulBitRate / 8)) +
                        (((ulPos % pMP3->usSampleRate) *
                          (pMP3->ulBitRate / 8)) / pMP3->usSampleRate) +
                        pMP3->ulFirstFrame;
                if(ulPos)
                {
                    ulPos--;
                }
            }

            //
            // Initialize the MP3 library.
            //
            InitMP3Audio(pMP3->pMPEGInstance);

            //
            // Initialize the bitstream to the position.
            //
            MP3InitBitstream(pMP3, ulPos);

            //
            // Find the next frame in the input MP3 bitstream.
            //
            while(1)
            {
                //
                // Look for the next frame.
                //
                if(MP3FindNextFrame(pMP3) == 1)
                {
                    //
                    // Make sure that the packed_info for the frame we found
                    // matches (where appropriate) the packed_info for the
                    // first frame in the file.
                    //
                    if((pMP3->usSampleRate ==
                        usSRMap[pMP3->sHdr.sample_rate]) &&
                       (pMP3->ucChannels == pMP3->sHdr.numchans))
                    {
                        break;
                    }
                }

                //
                // We could not find the next frame, so skip forward in the
                // input bitstream by a byte.
                //
                pMP3->sBS.bitidx += 8;
                if(pMP3->sBS.bitidx >= 32)
                {
                    pMP3->sBS.bufptr++;
                    pMP3->sBS.bitidx -= 32;
                }
            }

            //
            // Success.
            //
            return(1);
        }

        //
        // Cleanup after the codec.
        //
        case IOCTL_CODEC_CLOSE:
        {
            //
            // There's nothing to do, so return success.
            //
            return(1);
        }

        //
        // We should never get here, but just in case...
        //
        default:
        {
            //
            // Return a failure.
            //
            return(0);
        }
    }
}
#endif

//****************************************************************************
//
// AAC.C - Codec interface driver for the AAC decoder.
//
// Copyright (c) 2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include "globals.h"
#ifdef SUPPORT_AAC
#include "aac/aac.h"
#include "buffer/buffer.h"
#include "src/src.h"

//****************************************************************************
//
// This code has the following limitations:
//
// 1) Bitrate and track length are supported only for constant bit rate ADIF
//    format streams.  Variable bit rate ADIF streams encode the maximum peak
//    bitrate for an individual frame; the average bitrate is needed to
//    compute the track length.  Other formats do not encode the bitrate in
//    any way and therefore the track length can not be computed either.
//    Computing the bitrate would require decoding (effectively) the entire
//    file.  The bitrate and track length are returned as 0 for streams for
//    which we can not determine the bitrate and track length.
//
// 2) Raw AAC streams are not supported, only ADIF and ADTS.  The AAC decoder
//    will decode raw AAC streams but we have no way of determining that a
//    random bitstream is a valid raw AAC stream (outside of decoding a frame
//    or two of audio, which takes too much time).
//
// 3) The only position to which seek is supported is the beginning of the
//    file.  It is only possible to seek in an ADTS bitstream since it is the
//    only format that has frame headers (i.e. seek points) throughout the
//    data stream.  But since the bitrate and track length of an ADTS
//    bitstream are not encoded in the stream and are not easily computed, it
//    is not possible to know how to seek to 1500 milliseconds (for example)
//    into the stream.
//
// All of these problems can be solved by encapsulating the AAC bitstream in
// a meta-file structure which provides additional information about the
// stream.  It will never be possible to seek within an ADIF stream, however.
// It is preferable to have a standard meta-file structure used by everybody
// instead of a bunch of home-brew formats, each used only by a particular
// content distribution channel.
//
//****************************************************************************

//****************************************************************************
//
// The name of this codec.
//
//****************************************************************************
static const unsigned short pusCodecName[] =
{
    'A', 'A', 'C', ' ', '2', '.', '0', '.', '0', '.', '0', ' ',
    'C', 'h', 'a', 'n', 'n', 'e', 'l', ' ', 'L', 'o', 'w', ' ',
    'C', 'o', 'm', 'p', 'l', 'e', 'x', 'i', 't', 'y', ' ', 'M',
    'P', 'E', 'G', '-', '2', ' ', 'A', 'A', 'C', ' ', 'D', 'e',
    'c', 'o', 'd', 'e', 'r', '\0'
};

//****************************************************************************
//
// The size of the output buffer that will be used.
//
//****************************************************************************
#define AAC_MAX_PCM_LENGTH              1024

//****************************************************************************
//
// A structure which defines the persistent state of the AAC decoder.
//
//****************************************************************************
typedef struct
{
    //
    // A pointer to the buffer containing the persistent internal state of the
    // AAC decoder library.
    //
    tAAC_Decoder *pAACInstance;

    //
    // The file from which we read data.
    //
    tFile *pFile;

    //
    // A buffer to contain the encoded AAC audio.
    //
    char pcEncodedData[2048];

    //
    // Buffers to contain the decoded AAC audio.
    //
    short psLeft[AAC_MAX_PCM_LENGTH + AAC_MAX_PCM_LENGTH + NUMTAPS - 1];
    short psRight[AAC_MAX_PCM_LENGTH + AAC_MAX_PCM_LENGTH + NUMTAPS - 1];

    //
    // The buffer to which we write decoded AAC data.
    //
    BufferState *pOutput;

    //
    // The AAC bitstream pointer.
    //
    tAAC_Bitstream sBS;

    //
    // The encoded fixed frame header of the AAC stream with which we are
    // dealing.  If we are decoding an ADIF stream, then this is zero.
    //
    unsigned long ulHeader;

    //
    // The decoded AAC header information.
    //
    tAAC_Header sHdr;

    //
    // The sample rate of the decoded PCM stream.
    //
    unsigned short usSampleRate;

    //
    // The number of channels in the file.
    //
    unsigned char ucChannels;

    //
    // An unused byte to pad the next member to a word boundary.
    //
    unsigned char ucUnused;

    //
    // The length of the file.
    //
    unsigned long ulTimeLength;

    //
    // The bit rate of the AAC file.
    //
    unsigned long ulBitRate;

    //
    // The number of samples which have been decoded.
    //
    unsigned long ulTimePos;
} tAAC;

//****************************************************************************
//
// The following is a mapping from the sample rate descriptor in the
// tAAC_Header structure to the integer number of samples per second.
//
//****************************************************************************
static const unsigned long ulSRMap[] = { 96000, 88200, 64000,
                                         48000, 44100, 32000,
                                         24000, 22050, 16000,
                                         12000, 11025,  8000 };

//****************************************************************************
//
// The codec plug-in entry point for the AAC decoder.
//
//****************************************************************************
unsigned long
AACIoctl(unsigned long ulIoctl, unsigned long ulParam1, unsigned long ulParam2,
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
            // The secod parameter is a pointer for the name.
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

            //
            // The second parameter is a pointer for the name.
            //
            ppusName = (unsigned short **)ulParam2;

            //
            // There is no way to store the artist name in an AAC file.
            //
            *ppusName = 0;

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

            //
            // The second parameter is a pointer for the name.
            //
            ppusName = (unsigned short **)ulParam2;

            //
            // There is no way to store the song title in an AAC file.
            //
            *ppusName = 0;

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
            tAAC *pAAC;

            //
            // The first parameter is a pointer to the AAC persistent data.
            //
            pAAC = (tAAC *)ulParam1;

            //
            // The second parameter is a pointer for the bitrate.
            //
            pulBitRate = (unsigned long *)ulParam2;

            //
            // Return the average bitrate of the file.
            //
            *pulBitRate = pAAC->ulBitRate;

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
            tAAC *pAAC;

            //
            // The first parameter is a pointer to the AAC persistent data.
            //
            pAAC = (tAAC *)ulParam1;

            //
            // The second parameter is a pointer for the sample rate.
            //
            pulSampleRate = (unsigned long *)ulParam2;

            //
            // Return the sample rate of the file.
            //
            *pulSampleRate = pAAC->usSampleRate;

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
            tAAC *pAAC;

            //
            // The first parameter is a pointer to the AAC persistent data.
            //
            pAAC = (tAAC *)ulParam1;

            //
            // The second parameter is a pointer for the number of channels.
            //
            pulChannels = (unsigned long *)ulParam2;

            //
            // Return the number of channels in the file.
            //
            *pulChannels = pAAC->ucChannels;

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
            tAAC *pAAC;

            //
            // The first parameter is a pointer to the AAC persistent data.
            //
            pAAC = (tAAC *)ulParam1;

            //
            // The second parameter is a pointer for the number of
            // milliseconds.
            //
            pulLength = (unsigned long *)ulParam2;

            //
            // Return the length of the file.
            //
            *pulLength = pAAC->ulTimeLength;

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
            tAAC *pAAC;

            //
            // The first parameter is a pointer to the AAC persistent data.
            //
            pAAC = (tAAC *)ulParam1;

            //
            // The second parameter is a pointer for the number of
            // milliseconds.
            //
            pulTime = (unsigned long *)ulParam2;

            //
            // Determine the time based on the sample rate.
            //
            *pulTime = ((pAAC->ulTimePos / pAAC->usSampleRate) * 1000) +
                       (((pAAC->ulTimePos % pAAC->usSampleRate) * 1000) /
                        pAAC->usSampleRate);

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
            // Read the first 512 bytes from the file.
            //
            FSRead(pFile, pucScratch, 512);

            //
            // See if this is an ADIF stream.
            //
            if((pucScratch[0] == 'A') && (pucScratch[1] == 'D') &&
               (pucScratch[2] == 'I') && (pucScratch[3] == 'F'))
            {
                //
                // We can decode this file.
                //
                return(1);
            }

            //
            // Make sure that the bitstream starts with 0xFFF.
            //
            if((pucScratch[0] != 0xff) || ((pucScratch[1] & 0xf0) != 0xf0))
            {
                return(0);
            }

            //
            // The bitstream is a MPEG audio stream, so make sure that the
            // layer specified is reserved (i.e. AAC).
            //
            if((pucScratch[1] & 0x0e) != 0x08)
            {
                return(0);
            }

            //
            // Make sure that the file is encoded in the low complexity
            // profile.
            //
            if((pucScratch[2] & 0xc0) != 0x40)
            {
                return(0);
            }

            //
            // Make sure that a valid sample rate is specified (we do not
            // support 64kHz, 88.2kHz, and 96kHz).
            //
            if(((pucScratch[2] & 0x3c) < 0x0c) ||
               ((pucScratch[2] & 0x3c) > 0x2c))
            {
                return(0);
            }

            //
            // We can decode this file.
            //
            return(1);
        }

        //
        // Prepare the codec to encode or decode a file.
        //
        case IOCTL_CODEC_OPEN:
        {
            tAAC *pAAC;

            //
            // If we are being asked to encode an AAC file, then return a
            // failure since we can only decode AAC.
            //
            if(ulParam3 & CODEC_OPEN_ENCODE)
            {
                return(0);
            }

            //
            // The first parameter is a pointer to the AAC persistent state.
            //
            pAAC = (tAAC *)ulParam1;

            //
            // Make sure that we have enough memory for this decoder.
            //
            if((ulParam1 + sizeof(tAAC) + AAC_DecoderSize) > ulExtentOfRAM)
            {
                return(0);
            }

            //
            // Determine the pointer to the AAC decoder library's persistent
            // state.
            //
            pAAC->pAACInstance = (tAAC_Decoder *)(ulParam1 + sizeof(tAAC));

            //
            // Initialize the bitstream structure.
            //
            pAAC->sBS.DataEnd = (unsigned long *)(pAAC->pcEncodedData + 2048);
            pAAC->sBS.DataStart = (unsigned long *)pAAC->pcEncodedData;
            pAAC->sBS.DataIn = (unsigned long *)pAAC->pcEncodedData;

            //
            // Initialize the AAC library.
            //
            if(AAC_Initialise(pAAC->pAACInstance, &(pAAC->sBS), &(pAAC->sHdr),
                              eAAC_44100) != eAAC_NoError)
            {
                return(0);
            }

            //
            // Save the pointer to the file structure.
            //
            pAAC->pFile = (tFile *)ulParam4;

            //
            // Read the first 2K of data from the file.
            //
            FSReadBS(pAAC->pFile, pAAC->pcEncodedData, 2048);

            //
            // See if this is an ADIF stream.
            //
            if((pAAC->pcEncodedData[3] == 'A') &&
               (pAAC->pcEncodedData[2] == 'D') &&
               (pAAC->pcEncodedData[1] == 'I') &&
               (pAAC->pcEncodedData[0] == 'F'))
            {
                //
                // This is an ADIF stream, so there is no fixed frame header.
                //
                pAAC->ulHeader = 0;
            }

            //
            // See if this is an ADTS stream.
            //
            else if((pAAC->pcEncodedData[3] == 0xff) &&
                    ((pAAC->pcEncodedData[2] & 0xfe) == 0xf8))
            {
                //
                // This is an ADTS stream, so remember the fixed frame header.
                //
                pAAC->ulHeader = (pAAC->pcEncodedData[3] << 24) |
                                 (pAAC->pcEncodedData[2] << 16) |
                                 (pAAC->pcEncodedData[1] << 8) |
                                 (pAAC->pcEncodedData[0] & 0xf0);
            }

            //
            // We can not decode this stream if it does not have either an
            // ADIF or ADTS header.
            //
            else
            {
                return(0);
            }

            //
            // Decode the bitstream header.
            //
            if(AAC_DecodeHeader(pAAC->pAACInstance, &(pAAC->sBS),
                                &(pAAC->sHdr)) != eAAC_NoError)
            {
                return(0);
            }

            //
            // Get the number of channels, sample rate and bit rate of the
            // file.
            //
            pAAC->ucChannels = pAAC->sHdr.NumberOfChannels;
            pAAC->usSampleRate = ulSRMap[pAAC->sHdr.Frequency];
            if((pAAC->sHdr.BitstreamFormat == eAAC_ADIF) &&
               (pAAC->sHdr.BitstreamType == eAAC_ConstantRate))
            {
                pAAC->ulBitRate = pAAC->sHdr.BitRate;
            }
            else
            {
                pAAC->ulBitRate = 0;
            }

            //
            // Make sure that the stream has only one or two channels.
            //
            if(pAAC->ucChannels > 2)
            {
                return(0);
            }

            //
            // Get the length of the file.
            //
            if(pAAC->ulBitRate)
            {
                pAAC->ulTimeLength = (((FSLength(pAAC->pFile) * 8) /
                                       pAAC->ulBitRate) * 1000) +
                                     ((((FSLength(pAAC->pFile) * 8) %
                                        pAAC->ulBitRate) * 1000) /
                                      pAAC->ulBitRate);
            }
            else
            {
                pAAC->ulTimeLength = 0;
            }

            //
            // The position is zero.
            //
            pAAC->ulTimePos = 0;

            //
            // There is no output buffer.
            //
            pAAC->pOutput = 0;

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
            tAAC *pAAC;

            //
            // The first parameter is a pointer to the AAC persistent state.
            //
            pAAC = (tAAC *)ulParam1;

            //
            // The second parameter is a pointer to the output buffer.
            //
            pAAC->pOutput = (BufferState *)ulParam2;

            //
            // Provide the output buffer with our data buffers.
            //
            if(pAAC->ucChannels == 2)
            {
                BufferSetBuffer(pAAC->pOutput, pAAC->psLeft, pAAC->psRight,
                                AAC_MAX_PCM_LENGTH + AAC_MAX_PCM_LENGTH);
            }
            else
            {
                BufferSetBuffer(pAAC->pOutput, pAAC->psLeft, pAAC->psLeft,
                                AAC_MAX_PCM_LENGTH + AAC_MAX_PCM_LENGTH);
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
            tAAC *pAAC;

            //
            // The first parameter is a pointer to the AAC persistent state.
            //
            pAAC = (tAAC *)ulParam1;

            //
            // See if there is space available for decoding the next frame.
            //
            if(BufferSpaceAvailable(pAAC->pOutput) < AAC_MAX_PCM_LENGTH)
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
            tAAC *pAAC;
            short *psBuffers[3];
            long lLength;

            //
            // The first parameter is a pointer to the AAC persistent state.
            //
            pAAC = (tAAC *)ulParam1;

            //
            // Wait until there is space in the output buffer.  This tells us
            // when it is OK to write more data into the output buffer.
            //
            while(BufferSpaceAvailable(pAAC->pOutput) < AAC_MAX_PCM_LENGTH)
            {
                //
                // Busy wait until space is available.
                //
            }

            //
            // Get a pointer to the available space in the output buffer.
            //
            BufferGetWritePointer(pAAC->pOutput, &psBuffers[0], &psBuffers[1],
                                  &lLength);
            psBuffers[2] = 0;

            //
            // Decode the data frame.
            //
            if(AAC_DecodeDataBlock(pAAC->pAACInstance, &(pAAC->sBS),
                                   &(pAAC->sHdr), psBuffers) != eAAC_NoError)
            {
                //
                // We could not decode the frame, so return an error.
                //
                return(0);
            }

            //
            // Update the output buffer's write pointer.
            //
            BufferUpdateWritePointer(pAAC->pOutput, AAC_MAX_PCM_LENGTH);

            //
            // Increment the time based on the number of samples.
            //
            pAAC->ulTimePos += AAC_MAX_PCM_LENGTH;

            //
            // See if we can read more data from the file.
            //
            while((pAAC->sBS.DataOut < pAAC->sBS.DataIn) ||
                  (((long)pAAC->sBS.DataOut - (long)pAAC->sBS.DataIn) >= 512))
            {
                //
                // Do we need to read up to the end of the input bitstream
                // buffer?
                //
                if(pAAC->sBS.DataOut < pAAC->sBS.DataIn)
                {
                    //
                    // Read up to the end of the bitstream buffer.
                    //
                    FSReadBS(pAAC->pFile, pAAC->sBS.DataIn,
                             (long)pAAC->sBS.DataEnd - (long)pAAC->sBS.DataIn);

                    //
                    // Move the write pointer in the bitstream structure.
                    //
                    pAAC->sBS.DataIn = pAAC->sBS.DataStart;
                }
                else
                {
                    //
                    // Determine the number of bytes to read.
                    //
                    lLength = ((long)pAAC->sBS.DataOut -
                               (long)pAAC->sBS.DataIn) & ~0x1ff;

                    //
                    // Read data into the data buffer.
                    //
                    FSReadBS(pAAC->pFile, pAAC->sBS.DataIn, lLength);

                    //
                    // Move the write pointer in the bitstream structure.
                    //
                    pAAC->sBS.DataIn += lLength / 4;
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
            tAAC *pAAC;

            //
            // The first parameter is a pointer to the AAC persistent state.
            //
            pAAC = (tAAC *)ulParam1;

            //
            // If we are decoding an ADIF stream, then we can not seek to
            // non-zero positions.
            //
            if((pAAC->ulHeader == 0) && (ulParam2 != 0))
            {
                return(0);
            }

            //
            // If we are seeking to zero, then simply re-position the bitstream
            // to the beginning.
            //
            if(ulParam2 == 0)
            {
                //
                // Seek the file pointer back to the beginning of the file.
                //
                FSSeek(pAAC->pFile, 0);

                //
                // Read the first 2k of data from the file.
                //
                FSReadBS(pAAC->pFile, pAAC->pcEncodedData, 2048);

                //
                // Reset the data write pointer in the bitstream structure.
                //
                pAAC->sBS.DataIn = (unsigned long *)pAAC->pcEncodedData;

                //
                // Initialize the AAC library.
                //
                AAC_Initialise(pAAC->pAACInstance, &(pAAC->sBS), &(pAAC->sHdr),
                               eAAC_44100);

                //
                // The new position is zero.
                //
                pAAC->ulTimePos = 0;

                //
                // Success.
                //
                return(1);
            }

            //
            // We don't support other seeks yet.
            //
            return(0);
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

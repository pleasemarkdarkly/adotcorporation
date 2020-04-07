//****************************************************************************
//
// G721.C - Codec interface driver for the G.721 encoder/decoder.
//
// Copyright (c) 1999-2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include "globals.h"
#ifdef SUPPORT_G721
#include "adpcm/g72x.h"
#include "buffer/buffer.h"
#include "src/src.h"

//****************************************************************************
//
// A structure which defines the persistent state of the G.721 encoder/
// decoder.
//
//****************************************************************************
typedef struct
{
    //
    // The file from which we read data when decoding or to which we write data
    // when encoding.
    //
    tFile *pFile;

    //
    // The buffer which holds the encoded G.721 audio.
    //
    char pcEncodedData[512 + 128];

    //
    // The buffer which holds the 8000Hz PCM audio stream.
    //
    short psSRCData[512 + NUMTAPS - 1];

    //
    // The buffer which holds the 44100Hz PCM audio stream when encoding.
    //
    short psRawData[4096 + NUMTAPS - 1];

    //
    // The persistent state of the G.721 encoder/decoder.
    //
    struct g72x_state sG721;

    //
    // The buffer from which we read the PCM audio stream when encoding or to
    // which we write the PCM audio stream when decoding.
    //
    BufferState *pBuffer;

    //
    // The length of the encoded G.721 file.
    //
    unsigned long ulLength;

    //
    // The number of bytes in pcEncodedData which contain valid encoded
    // G.721 audio.
    //
    unsigned short usValid;

    //
    // The offset into pcEncodedData of the next G.721 sample to be processed.
    //
    unsigned short usOffset;

    //
    // The length of the file in milliseconds.
    //
    unsigned long ulTimeLength;

    //
    // The number of G.721 samples that have been processed, either decoded and
    // played via the DAI interface or recorded via the DAI interface and
    // encoded.
    //
    unsigned long ulTimePos;

    //
    // The current offset into the G.721 file, indicating the number of
    // bytes which have been read from the file into the encoded data buffer.
    //
    unsigned long ulFilePos;
} tG721;

//****************************************************************************
//
// The codec plug-in entry point for the G.721 encoder/decoder.
//
//****************************************************************************
unsigned long
G721Ioctl(unsigned long ulIoctl, unsigned long ulParam1,
          unsigned long ulParam2, unsigned long ulParam3,
          unsigned long ulParam4)
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
            char **ppcName;

            //
            // The secod parameter is a pointer for the name.
            //
            ppcName = (char **)ulParam2;

            //
            // Return the name of this codec.  The first four characters are
            // the short name and the string starting at the fifth character
            // is the long name.
            //
            *ppcName = "G721G.721 ADPCM";

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
            char **ppcName;

            //
            // The second parameter is a pointer for the name.
            //
            ppcName = (char **)ulParam2;

            //
            // There is no way to store the artist name in a G.721 file.
            //
            *ppcName = 0;

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
            char **ppcName;

            //
            // The second parameter is a pointer for the name.
            //
            ppcName = (char **)ulParam2;

            //
            // There is no way to store the song title in a G.721 file.
            //
            *ppcName = 0;

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

            //
            // The second parameter is a pointer for the bitrate.
            //
            pulBitRate = (unsigned long *)ulParam2;

            //
            // The bitrate of G.721 is 32kbps.
            //
            *pulBitRate = 32000;

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

            //
            // The second parameter is a pointer for the sample rate.
            //
            pulSampleRate = (unsigned long *)ulParam2;

            //
            // The sample rate of this G.721 codec is 8kHz.
            //
            *pulSampleRate = 8000;

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

            //
            // The second parameter is a pointer for the number of channels.
            //
            pulChannels = (unsigned long *)ulParam2;

            //
            // The file contains only a single channel.
            //
            *pulChannels = 1;

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
            tG721 *pG721;

            //
            // The first parameter is a pointer ot the G.721 persistent data.
            //
            pG721 = (tG721 *)ulParam1;

            //
            // The second parameter is a pointer for the number of
            // milliseconds.
            //
            pulLength = (unsigned long *)ulParam2;

            //
            // Return the length of the file.
            //
            *pulLength = pG721->ulTimeLength;

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
            tG721 *pG721;

            //
            // The first parameter is a pointer to the G.721 persistent data.
            //
            pG721 = (tG721 *)ulParam1;

            //
            // The second parameter is a pointer for the number of
            // milliseconds.
            //
            pulTime = (unsigned long *)ulParam2;

            //
            // Determine the time based on the sample rate.
            //
            *pulTime = pG721->ulTimePos / 8;

            //
            // Success.
            //
            return(1);
        }

        //
        // Prepare the codec to encode or decode a file.
        //
        case IOCTL_CODEC_OPEN:
        {
            tG721 *pG721;

            //
            // The first parameter is a pointer to the G.721 persistent state.
            //
            pG721 = (tG721 *)ulParam1;

            //
            // Make sure that we have enough memory for this decoder.
            //
            if((((unsigned long)pG721 + sizeof(tG721)) & 0xFFFF) > 0x9600)
            {
                return(0);
            }

            //
            // Initialize the G.721 decoder.
            //
            g72x_init_state(&(pG721->sG721));

            //
            // Save the pointer to the file structure.
            //
            pG721->pFile = (tFile *)ulParam4;

            //
            // See if we should encode or decode.
            //
            if(ulParam3 & CODEC_OPEN_ENCODE)
            {
                //
                // The first four bytes of the file are "G721" to indicate that
                // this is a G.721 file.
                //
                pG721->pcEncodedData[0] = 'G';
                pG721->pcEncodedData[1] = '7';
                pG721->pcEncodedData[2] = '2';
                pG721->pcEncodedData[3] = '1';

                //
                // There are four bytes in the output buffer.
                //
                pG721->usValid = 4;

                //
                // The initial time position and length is zero.
                //
                pG721->ulTimePos = pG721->ulTimeLength = 0;

                //
                // The first write in the file will occur at position 0.
                //
                pG721->ulFilePos = 0;

                //
                // The initial length of the file is zero.
                //
                pG721->ulLength = 0;
            }
            else
            {
                //
                // Read the first page of the file.
                //
                FSRead(pG721->pFile, pG721->pcEncodedData, 512);

                //
                // Make sure that the first four bytes of the file are "G721".
                //
                if((pG721->pcEncodedData[0] != 'G') ||
                   (pG721->pcEncodedData[1] != '7') ||
                   (pG721->pcEncodedData[2] != '2') ||
                   (pG721->pcEncodedData[3] != '1'))
                {
                    return(0);
                }

                //
                // Since we've just read the first page of the file, the next
                // read will occur at location 512 into the file.
                //
                pG721->ulFilePos = 512;

                //
                // Get the length of the G.721 file.
                //
                pG721->ulLength = FSLength(pG721->pFile);

                //
                // Initially, the 512 bytes previously read into the buffer are
                // valid.
                //
                pG721->usValid = 512;

                //
                // Make sure the file position and the valid data size are less
                // than the file length.
                //
                if(pG721->ulFilePos > pG721->ulLength)
                {
                    pG721->ulFilePos = pG721->ulLength;
                    pG721->usValid = pG721->ulLength;
                }

                //
                // The initial time position is zero.
                //
                pG721->ulTimePos = 0;

                //
                // Start play from the fifth byte of the buffer.
                //
                pG721->usOffset = 4;

                //
                // Compute the length of the file in milliseconds.
                //
                pG721->ulTimeLength = (pG721->ulLength - 4) / 4;
            }

            //
            // There is no input/output buffer.
            //
            pG721->pBuffer = 0;

            //
            // Success.
            //
            return(1);
        }

        //
        // Get the capture buffer for the encoder.
        //
        case IOCTL_CODEC_GETCAPTUREBUFFER:
        {
            tG721 *pG721;
            short **ppsBuffer;
            long *plLength;

            //
            // The first parameter is a pointer to the G.721 persistent state.
            //
            pG721 = (tG721 *)ulParam1;

            //
            // The second parameter is a pointer to the capture buffer.
            //
            ppsBuffer = (short **)ulParam2;

            //
            // The third parameter is a pointer to the length of the capture
            // buffer.
            //
            plLength = (long *)ulParam3;

            //
            // Return the capture buffer.
            //
            *ppsBuffer = pG721->psRawData;
            *plLength = 4096;

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
            tG721 *pG721;

            //
            // The first parameter is a pointer to the G.721 persistent state.
            //
            pG721 = (tG721 *)ulParam1;

            //
            // The second parameter is a pointer to the input/output buffer.
            //
            pG721->pBuffer = (BufferState *)ulParam2;

            //
            // Provide the input/output buffer with our data buffer.
            //
            BufferSetBuffer(pG721->pBuffer, pG721->psSRCData, pG721->psSRCData,
                            512);

            //
            // Success.
            //
            return(1);
        }

        //
        // Decode a "frame" of data (which we arbitrarily set as 256 samples).
        //
        case IOCTL_CODEC_DECODE:
        {
            tG721 *pG721;
            short *psLeft, *psRight;
            long lLength, lLoop;

            //
            // The first parameter is a pointer to the G.721 persistent state.
            //
            pG721 = (tG721 *)ulParam1;

            //
            // See if we've reached the end of the file.
            //
            if((pG721->usValid == pG721->usOffset) &&
               (pG721->ulFilePos == pG721->ulLength))
            {
                return(0);
            }

            //
            // See if we need to read more data from the file.
            //
            if(((pG721->usValid - pG721->usOffset) < 128) &&
               (pG721->ulFilePos != pG721->ulLength))
            {
                //
                // Copy the tail of the buffer to the beginning.
                //
                memcpy(pG721->pcEncodedData,
                       pG721->pcEncodedData + pG721->usOffset,
                       pG721->usValid - pG721->usOffset);

                //
                // Read the next 512 bytes from the file.
                //
                FSRead(pG721->pFile,
                       pG721->pcEncodedData + pG721->usValid - pG721->usOffset,
                       512);

                //
                // Update the buffer pointers.
                //
                if((pG721->ulLength - pG721->ulFilePos) < 512)
                {
                    pG721->usValid += pG721->ulLength - pG721->ulFilePos -
                                      pG721->usOffset;
                }
                else
                {
                    pG721->usValid += 512 - pG721->usOffset;
                }
                pG721->usOffset = 0;

                //
                // Update the position within the file.
                //
                pG721->ulFilePos += 512;
                if(pG721->ulFilePos > pG721->ulLength)
                {
                    pG721->ulFilePos = pG721->ulLength;
                }
            }

            //
            // Wait until there is enough space in the output buffer.
            //
            while(BufferSpaceAvailable(pG721->pBuffer) < 256)
            {
                //
                // Put the EP7209 into IDLE mode.
                //
                Halt();
            }

            //
            // Get the write pointer for the output buffer.
            //
            BufferGetWritePointer(pG721->pBuffer, &psLeft, &psRight, &lLength);

            //
            // Determine how many G.721 samples to uncompress, up to a maximum
            // of 256.
            //
            if((pG721->usValid - pG721->usOffset) < 128)
            {
                lLength = (pG721->usValid - pG721->usOffset) * 2;
            }
            else
            {
                lLength = 256;
            }

            //
            // Uncompress the G.721 samples.
            //
            for(lLoop = 0; lLoop < lLength; lLoop += 2, pG721->usOffset++)
            {
                //
                // Uncompress the lower nibble of this byte.
                //
                *psLeft++ =
                    g721_decoder(pG721->pcEncodedData[pG721->usOffset] & 15,
                                 AUDIO_ENCODING_LINEAR, &(pG721->sG721));

                //
                // Uncompress the upper nibble of this byte.
                //
                *psLeft++ =
                    g721_decoder(pG721->pcEncodedData[pG721->usOffset] >> 4,
                                 AUDIO_ENCODING_LINEAR, &(pG721->sG721));
            }

            //
            // Update the output buffer write pointer.
            //
            BufferUpdateWritePointer(pG721->pBuffer, lLength);

            //
            // Increment the time.
            //
            pG721->ulTimePos += lLength;

            //
            // Success.
            //
            return(1);
        }

        //
        // Encode a frame of data.
        //
        case IOCTL_CODEC_ENCODE:
        {
            tG721 *pG721;
            short *psLeft, *psRight;
            long lLength, lLoop;

            //
            // The first parameter is a pointer to the G.721 persistent state.
            //
            pG721 = (tG721 *)ulParam1;

            //
            // Wait until there is enough data in the input buffer.
            //
            while(BufferDataAvailable(pG721->pBuffer) < 256)
            {
                //
                // Put the EP7209 into IDLE mode.
                //
                Halt();
            }

            //
            // Get the read pointer for the input buffer.
            //
            BufferGetReadPointer(pG721->pBuffer, &psLeft, &psRight, &lLength);

            //
            // Compress the samples into G.721 samples.
            //
            for(lLoop = 0; lLoop < 256; lLoop += 2)
            {
                //
                // Compress the next two samples and pack them into a single
                // byte.
                //
                pG721->pcEncodedData[pG721->usValid] =
                    g721_encoder(*psLeft++, AUDIO_ENCODING_LINEAR,
                                 &(pG721->sG721));
                pG721->pcEncodedData[pG721->usValid++] |=
                    g721_encoder(*psLeft++, AUDIO_ENCODING_LINEAR,
                                 &(pG721->sG721)) << 4;
            }

            //
            // Update the read pointer for the input buffer.
            //
            BufferUpdateReadPointer(pG721->pBuffer, 256);

            //
            // If there is enough data in the encoded data buffer, then write
            // it to the file.
            //
            if(pG721->usValid >= 512)
            {
                //
                // Write 512 bytes of data to the file.
                //
                if(FSWrite(pG721->pFile, pG721->pcEncodedData, 512) == 0)
                {
                    return(0);
                }

                //
                // There are now 512 less samples in the encoded data buffer.
                //
                pG721->usValid -= 512;

                //
                // Copy the valid data from the end of the buffer to the
                // beginning.
                //
                memcpy(pG721->pcEncodedData, pG721->pcEncodedData + 512,
                       pG721->usValid);
            }

            //
            // Increment the time.
            //
            pG721->ulTimePos += 256;

            //
            // Success.
            //
            return(1);
        }

        //
        // Seek to the specified time position.
        //
        case IOCTL_CODEC_SEEK:
        {
            tG721 *pG721;

            //
            // The first parameter is a pointer to the G.721 persistent state.
            //
            pG721 = (tG721 *)ulParam1;

            //
            // Make sure that the specified time is valid.
            //
            if(ulParam2 > pG721->ulTimeLength)
            {
                ulParam2 = pG721->ulTimeLength;
            }

            //
            // Compute the position in the file of the seek point (adjusted to
            // a 512 byte boundary).
            //
            pG721->ulFilePos = ((ulParam2 * 4) + 256) & ~511;

            //
            // Compute the time for the computed file position.
            //
            pG721->ulTimePos = (pG721->ulFilePos - 4) * 2;

            //
            // Reposition the file pointer to the computed offset.
            //
            FSSeek(pG721->pFile, pG721->ulFilePos);

            //
            // There is no valid data in the input buffer.
            //
            pG721->usOffset = 0;
            pG721->usValid = 0;

            //
            // Initialize the G.721 decoder.
            //
            g72x_init_state(&(pG721->sG721));

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

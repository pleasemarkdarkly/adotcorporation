//****************************************************************************
//
// CODEC.C - Generic codec interface layer.
//
// Copyright (c) 1999-2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include "globals.h"

//****************************************************************************
//
// The following enum must match the ordinal number of the various decoders
// in the pfnIoctl table.
//
//****************************************************************************
enum
{
#ifdef SUPPORT_WMA
    CODEC_WMA,
#endif

#ifdef SUPPORT_AAC
    CODEC_AAC,
#endif

#ifdef SUPPORT_MP3
    CODEC_MP3,
#endif

#ifdef SUPPORT_ACELPNET
    CODEC_ACELPNET,
#endif

#ifdef SUPPORT_MSADPCM
    CODEC_MSADPCM,
#endif

#ifdef SUPPORT_G721
    CODEC_G721,
#endif

    NUMCODECS
};

//****************************************************************************
//
// An array of entry points for the codecs supported by the codec interface.
//
//****************************************************************************
static unsigned long (* const pfnIoctl[NUMCODECS])(unsigned long ulIoctl,
                                                   unsigned long ulParam1,
                                                   unsigned long ulParam2,
                                                   unsigned long ulParam3,
                                                   unsigned long ulParam4) =
{
    //
    // The WMA decoder.
    //
#ifdef SUPPORT_WMA
    WMAIoctl,
#endif

    //
    // The AAC decoder.
    //
#ifdef SUPPORT_AAC
    AACIoctl,
#endif

    //
    // The MP3 decoder.
    //
#ifdef SUPPORT_MP3
    MP3Ioctl,
#endif

    //
    // The ACELP.net decoder.
    //
#ifdef SUPPORT_ACELPNET
    ACELPnetIoctl,
#endif

    //
    // The MS ADPCM encoder/decoder.
    //
#ifdef SUPPORT_MSADPCM
    MSADPCMIoctl,
#endif

    //
    // The G.721 encoder/decoder.
    //
#ifdef SUPPORT_G721
    G721Ioctl,
#endif
};

//****************************************************************************
//
// The following structure contains the persistent state of the generic codec
// interface layer.
//
//****************************************************************************
static struct
{
    //
    // A boolean which is true when there is a codec currently opened.
    //
    unsigned char bCodecOpen;

    //
    // The codec which is currently opened.
    //
    unsigned char ucCodec;

    //
    // The flags specified when the current codec was opened.
    //
    unsigned char ucFlags;
} sCodec;

//****************************************************************************
//
// FindChunk finds the given chunk in a RIFF file.
//
//****************************************************************************
static unsigned long
FindChunk(unsigned char *pucData, unsigned long ulLength, char *pcName)
{
    unsigned long ulIdx = 12;

    //
    // Loop until we find the specified chunk.
    //
    while(1)
    {
        //
        // Is this the correct chunk?
        //
        if((pucData[ulIdx] == pcName[0]) &&
           (pucData[ulIdx + 1] == pcName[1]) &&
           (pucData[ulIdx + 2] == pcName[2]) &&
           (pucData[ulIdx + 3] == pcName[3]))
        {
            //
            // This is the correct chunk, so return it's offset.
            //
            return(ulIdx);
        }

        //
        // Skip to the next chunk.
        //
        ulIdx += 8 + pucData[ulIdx + 4] + (pucData[ulIdx + 5] << 8) +
                 (pucData[ulIdx + 6] << 16) + (pucData[ulIdx + 7] << 24);

        //
        // If this chunk starts beyond the piece of the file that we've read,
        // then quit looking for the specified chunk.
        //
        if(ulIdx > ulLength)
        {
            return(0);
        }
    }
}

//****************************************************************************
//
// DetermineCodec parses the file to determine the appropriate codec to use
// to decode the file.
//
//****************************************************************************
static unsigned long
DetermineCodec(tFile *pFile, unsigned long *pulDataStart)
{
    unsigned char *pucData = (unsigned char *)ulEndOfRAM;
    unsigned long ulOffset, ulRet;

    //
    // Read the first 4K of data from the file.
    //
    FSRead(pFile, pucData, 4096);

    //
    // Seek back to the beginning of the file.
    //
    FSSeek(pFile, 0);

    //
    // Is this a RIFF WAVE file?
    //
    if((pucData[0] == 'R') && (pucData[1] == 'I') &&
       (pucData[2] == 'F') && (pucData[3] == 'F') &&
       (pucData[8] == 'W') && (pucData[9] == 'A') &&
       (pucData[10] == 'V') && (pucData[11] == 'E'))
    {
        //
        // Find the "fmt " chunk.
        //
        ulOffset = FindChunk(pucData, 4096, "fmt ");

        //
        // If there is no "fmt " chunk, then return an error.
        //
        if(ulOffset == 0)
        {
            return(-1);
        }

        //
        // Determine the codec type based on the format tag.
        //
        switch(pucData[ulOffset + 8] + (pucData[ulOffset + 9] << 8))
        {
            //
            // This file is a MS ADPCM file.
            //
#ifdef SUPPORT_MSADPCM
            case 0x02:
            {
                ulRet = CODEC_MSADPCM;
                break;
            }
#endif

            //
            // This file is an MP3 file.
            //
#ifdef SUPPORT_MP3
            case 0x55:
            {
                ulRet = CODEC_MP3;
                break;
            }
#endif

            //
            // This file is an ACELP.net file.
            //
#ifdef SUPPORT_ACELPNET
            case 0x130:
            {
                ulRet = CODEC_ACELPNET;
                break;
            }
#endif

            //
            // This file is a WMA file.
            //
#ifdef SUPPORT_WMA
            case 0x161:
            {
                ulRet = CODEC_WMA;
                break;
            }
#endif

            //
            // This is an unknown format, so we can not decode this file.
            //
            default:
            {
                return(-1);
            }
        }

        //
        // Find the "data" chunk.
        //
        ulOffset = FindChunk(pucData, 4096, "data");

        //
        // If we could not find the "data" chunk, then return an error.
        //
        if(ulOffset == 0)
        {
            return(-1);
        }

        //
        // Return the start of the file data.
        //
        *pulDataStart = ulOffset + 8;

        //
        // Return the codec to be used.
        //
        return(ulRet);
    }

    //
    // It is not a RIFF file, so query each decoder.
    //
    else
    {
        //
        // For non-RIFF files, the start of the file data is the start of the
        // file.
        //
        *pulDataStart = 0;

        //
        // Check each decoder and see if any of them can decode the file.
        //
        for(ulOffset = 0; ulOffset < NUMCODECS; ulOffset++)
        {
            //
            // Open this codec.
            //
            if(CodecOpen(ulOffset, CODEC_OPEN_DECODE, pFile))
            {
                break;
            }

            //
            // Seek back to the beginning of the file.
            //
            FSSeek(pFile, 0);
        }

        //
        // Seek back to the beginning of the file.
        //
        FSSeek(pFile, 0);

        //
        // If we found a decoder that supports this file, then use it.
        //
        if(ulOffset < NUMCODECS)
        {
            //
            // Close the codec.
            //
            CodecClose();

            //
            // Return this codec number.
            //
            return(ulOffset);
        }
    }

    //
    // We couldn't determine which codec to use, so return an error.
    //
    return(-1);
}

//****************************************************************************
//
// CodecOpen opens a codec.  For decoders, the type of the current file is
// determined and used to determine the codec to open.  For encoders, the
// specified codec is opened.
//
//****************************************************************************
unsigned long
CodecOpen(unsigned long ulCodec, unsigned long ulFlags, tFile *pFile)
{
    unsigned long ulRet, ulOffset = 0;

    //
    // If there is already a codec opened, then return a failure.
    //
    if(sCodec.bCodecOpen)
    {
        return(0);
    }

    //
    // Make sure that a valid set of flags were specified.
    //
    if(!(ulFlags & (CODEC_OPEN_ENCODE | CODEC_OPEN_DECODE)) ||
       ((ulFlags & (CODEC_OPEN_ENCODE | CODEC_OPEN_DECODE)) ==
        (CODEC_OPEN_ENCODE | CODEC_OPEN_DECODE)))
    {
        return(0);
    }

    //
    // Determine the type of the file if this is a request for a decoder.
    //
    if(ulFlags & CODEC_OPEN_DECODE)
    {
        //
        // If a codec number was not specified, then determine the codec to be
        // used by parsing the file.
        //
        if(ulCodec == -1)
        {
            //
            // Determine the codec to use.
            //
            ulCodec = DetermineCodec(pFile, &ulOffset);

            //
            // If we could not determine the codec to use, then return a
            // failure.
            //
            if(ulCodec == -1)
            {
                return(0);
            }
        }
    }
    else
    {
        //
        // If a codec number was not specified, then find the first codec that
        // can encode.
        //
        if(ulCodec == -1)
        {
            //
            // Loop through all the available codecs.
            //
            for(ulCodec = 0; ulCodec < NUMCODECS; ulCodec++)
            {
                //
                // Open this codec.
                //
                if(CodecOpen(ulCodec, CODEC_OPEN_ENCODE, pFile))
                {
                    break;
                }
            }

            //
            // If we found a codec, then close it now.
            //
            if(ulCodec != NUMCODECS)
            {
                //
                // Close the codec.
                //
                CodecClose();

                //
                // Seek back to the beginning of the file.
                //
                FSSeek(pFile, 0);
            }
        }
    }

    //
    // Make sure that a valid codec was specified.
    //
    if(ulCodec >= NUMCODECS)
    {
        return(0);
    }

    //
    // Pass the open request to the entry point for the codec.
    //
    ulRet = (pfnIoctl[ulCodec])(IOCTL_CODEC_OPEN, ulEndOfRAM, ulOffset,
                                ulFlags, (unsigned long)pFile);

    //
    // If the open succeeded, then keep track of the fact that a codec is
    // opened.
    //
    if(ulRet)
    {
        //
        // A codec is opened.
        //
        sCodec.bCodecOpen = 1;

        //
        // The codec which is opened.
        //
        sCodec.ucCodec = ulCodec;

        //
        // The flags specified when the codec was opened.
        //
        sCodec.ucFlags = ulFlags;
    }

    //
    // Return the result to the caller.
    //
    return(ulRet);
}

//****************************************************************************
//
// CodecGetIndex returns the codec number of the current codec.  This can then
// be used on subsequent calls to CodecOpen for the given file, avoiding the
// codec determination process.
//
//****************************************************************************
unsigned long
CodecGetIndex(unsigned long *pulIndex)
{
    //
    // If there is not a codec opened, then return an error.
    //
    if(!sCodec.bCodecOpen)
    {
        return(0);
    }

    //
    // Return the current codec number.
    //
    *pulIndex = sCodec.ucCodec;

    //
    // Success.
    //
    return(1);
}

//****************************************************************************
//
// CodecGetName gets the name of the current codec.  The first four characters
// of the string returned are the short name of the codec, and the remaining
// characters are the full name of the codec.
//
//****************************************************************************
unsigned long
CodecGetName(char **ppcName)
{
    //
    // If there is not a codec opened, then return an error.
    //
    if(!sCodec.bCodecOpen)
    {
        return(0);
    }

    //
    // Pass the codec name request to the entry point for the specified codec.
    //
    return((pfnIoctl[sCodec.ucCodec])(IOCTL_CODEC_GETNAME, ulEndOfRAM,
                                      (unsigned long)ppcName, 0, 0));
}

//****************************************************************************
//
// CodecGetNameByIndex gets the name of the specified codec.  The first four
// characters of the string returned are the short name of the codec, and the
// remaining characters are the full name of the codec.
//
//****************************************************************************
unsigned long
CodecGetNameByIndex(unsigned long ulIndex, char **ppcName)
{
    //
    // Make sure that the codec index is valid.
    //
    if(ulIndex >= NUMCODECS)
    {
        return(0);
    }

    //
    // Pass the codec name request to the entry point for the specified codec.
    //
    return((pfnIoctl[ulIndex])(IOCTL_CODEC_GETNAME, ulEndOfRAM,
                               (unsigned long)ppcName, 0, 0));
}

//****************************************************************************
//
// CodecGetArtist gets the name of the artist associated with the file being
// decoded by the currently opened codec.
//
//****************************************************************************
unsigned long
CodecGetArtist(char **ppcName)
{
    //
    // If there is not a codec opened, then return an error.
    //
    if(!sCodec.bCodecOpen)
    {
        return(0);
    }

    //
    // Pass the artist name request to the entry point for the specified codec.
    //
    return((pfnIoctl[sCodec.ucCodec])(IOCTL_CODEC_GETARTIST, ulEndOfRAM,
                                      (unsigned long)ppcName, 0, 0));
}

//****************************************************************************
//
// CodecGetTitle gets the name of the song associated with the file being
// decoded by the currently opened codec.
//
//****************************************************************************
unsigned long
CodecGetTitle(char **ppcName)
{
    //
    // If there is not a codec opened, then return an error.
    //
    if(!sCodec.bCodecOpen)
    {
        return(0);
    }

    //
    // Pass the song name request to the entry point for the specified codec.
    //
    return((pfnIoctl[sCodec.ucCodec])(IOCTL_CODEC_GETTITLE, ulEndOfRAM,
                                      (unsigned long)ppcName, 0, 0));
}

//****************************************************************************
//
// CodecGetBitRate returns the bit rate of the encoded file.
//
//****************************************************************************
unsigned long
CodecGetBitRate(unsigned long *pulBitRate)
{
    //
    // If there is not a codec opened, then return an error.
    //
    if(!sCodec.bCodecOpen)
    {
        return(0);
    }

    //
    // Pass the bitrate request to the entry point for the specified codec.
    //
    return((pfnIoctl[sCodec.ucCodec])(IOCTL_CODEC_GETBITRATE, ulEndOfRAM,
                                      (unsigned long)pulBitRate, 0, 0));
}

//****************************************************************************
//
// CodecGetSampleRate returns the sample rate of the encoded file.
//
//****************************************************************************
unsigned long
CodecGetSampleRate(unsigned long *pulSampleRate)
{
    //
    // If there is not a codec opened, then return an error.
    //
    if(!sCodec.bCodecOpen)
    {
        return(0);
    }

    //
    // Pass the sample rate request to the entry point for the specified codec.
    //
    return((pfnIoctl[sCodec.ucCodec])(IOCTL_CODEC_GETSAMPLERATE, ulEndOfRAM,
                                      (unsigned long)pulSampleRate, 0, 0));
}

//****************************************************************************
//
// CodecGetChannels returns the number of channels in the encoded file.
//
//****************************************************************************
unsigned long
CodecGetChannels(unsigned long *pulChannels)
{
    //
    // If there is not a codec opened, then return an error.
    //
    if(!sCodec.bCodecOpen)
    {
        return(0);
    }

    //
    // Pass the channels request to the entry point for the specified codec.
    //
    return((pfnIoctl[sCodec.ucCodec])(IOCTL_CODEC_GETCHANNELS, ulEndOfRAM,
                                      (unsigned long)pulChannels, 0, 0));
}

//****************************************************************************
//
// CodecGetLength returns the length (in milliseconds) of the encoded file.
//
//****************************************************************************
unsigned long
CodecGetLength(unsigned long *pulLength)
{
    //
    // If there is not a codec opened, then return an error.
    //
    if(!sCodec.bCodecOpen)
    {
        return(0);
    }

    //
    // Pass the length request to the entry point for the specified codec.
    //
    return((pfnIoctl[sCodec.ucCodec])(IOCTL_CODEC_GETLENGTH, ulEndOfRAM,
                                      (unsigned long)pulLength, 0, 0));
}

//****************************************************************************
//
// CodecGetTime returns the current position (in milliseconds) within the
// file.
//
//****************************************************************************
unsigned long
CodecGetTime(unsigned long *pulTime)
{
    //
    // If there is not a codec opened, then return an error.
    //
    if(!sCodec.bCodecOpen)
    {
        return(0);
    }

    //
    // Pass the time request to the entry point for the specified codec.
    //
    return((pfnIoctl[sCodec.ucCodec])(IOCTL_CODEC_GETTIME, ulEndOfRAM,
                                      (unsigned long)pulTime, 0, 0));
}

//****************************************************************************
//
// CodecGetCaptureBuffer gets the buffer to be used by the digital audio
// interface for capturing data for the encoder.
//
//****************************************************************************
unsigned long
CodecGetCaptureBuffer(short **ppsBuffer, long *plLength)
{
    //
    // If there is not a codec opened, then return an error.
    //
    if(!sCodec.bCodecOpen)
    {
        return(0);
    }

    //
    // If this codec was not opened for encoding, then return an error.
    //
    if(!(sCodec.ucFlags & CODEC_OPEN_ENCODE))
    {
        return(0);
    }

    //
    // Pass the get capture buffer request to the entry point for the specified
    // codec.
    //
    return((pfnIoctl[sCodec.ucCodec])(IOCTL_CODEC_GETCAPTUREBUFFER, ulEndOfRAM,
                                      (unsigned long)ppsBuffer,
                                      (unsigned long)plLength, 0));
}

//****************************************************************************
//
// CodecSetBuffer sets the buffer to which decoders write their output data or
// from which encoders take their input data.
//
//****************************************************************************
unsigned long
CodecSetBuffer(BufferState *psBuffer)
{
    //
    // If there is not a codec opened, then return an error.
    //
    if(!sCodec.bCodecOpen)
    {
        return(0);
    }

    //
    // Pass the set buffer request to the entry point for the specified codec.
    //
    return((pfnIoctl[sCodec.ucCodec])(IOCTL_CODEC_SETBUFFER, ulEndOfRAM,
                                      (unsigned long)psBuffer, 0, 0));
}

//****************************************************************************
//
// CodecDecode decodes a "frame" of data with the current codec.  The
// definition of "frame" is codec dependent and relates to the amount of data
// that can be decoded given the availablity of memory and to the natural
// framing with the encoded stream (if there is any).
//
//****************************************************************************
unsigned long
CodecDecode(void)
{
    //
    // If there is not a codec opened, then return an error.
    //
    if(!sCodec.bCodecOpen)
    {
        return(0);
    }

    //
    // If this codec was not opened for decoding, then return an error.
    //
    if(!(sCodec.ucFlags & CODEC_OPEN_DECODE))
    {
        return(0);
    }

    //
    // Pass the decode request to the entry point for the specified codec.
    //
    return((pfnIoctl[sCodec.ucCodec])(IOCTL_CODEC_DECODE, ulEndOfRAM, 0, 0,
                                      0));
}

//****************************************************************************
//
// CodecEncode encodes a "frame" of data with the current codec.  The
// definition of "frame" is codec dependent and relates to the amount of data
// that can be encoded given the availablity of memory and to the natural
// framing with the encoded stream (if there is any).
//
//****************************************************************************
unsigned long
CodecEncode(void)
{
    //
    // If there is not a codec opened, then return an error.
    //
    if(!sCodec.bCodecOpen)
    {
        return(0);
    }

    //
    // If this codec was not opened for encoding, then return an error.
    //
    if(!(sCodec.ucFlags & CODEC_OPEN_ENCODE))
    {
        return(0);
    }

    //
    // Pass the encode request to the entry point for the specified codec.
    //
    return((pfnIoctl[sCodec.ucCodec])(IOCTL_CODEC_ENCODE, ulEndOfRAM, 0, 0,
                                      0));
}

//****************************************************************************
//
// CodecSeek repositions the input data stream to the specified time.
//
//****************************************************************************
unsigned long
CodecSeek(unsigned long ulTime)
{
    //
    // If there is not a codec opened, then return an error.
    //
    if(!sCodec.bCodecOpen)
    {
        return(0);
    }

    //
    // If this codec was not opened for decoding, then return an error.
    //
    if(!(sCodec.ucFlags & CODEC_OPEN_DECODE))
    {
        return(0);
    }

    //
    // Pass the seek request to the entry point for the specified codec.
    //
    return((pfnIoctl[sCodec.ucCodec])(IOCTL_CODEC_SEEK, ulEndOfRAM, ulTime, 0,
                                      0));
}

//****************************************************************************
//
// CodecClose closes the currently opened codec.
//
//****************************************************************************
unsigned long
CodecClose(void)
{
    unsigned long ulRet;

    //
    // If a codec is not currently opened, then return a failure.
    //
    if(!sCodec.bCodecOpen)
    {
        return(0);
    }

    //
    // Pass the close request to the entry point for the specified codec.
    //
    ulRet = (pfnIoctl[sCodec.ucCodec])(IOCTL_CODEC_CLOSE, ulEndOfRAM,
                                       sCodec.ucFlags, 0, 0);

    //
    // If the close succeeded, then keep track of the fact that a codec is not
    // currently opened.
    //
    if(ulRet)
    {
        sCodec.bCodecOpen = 0;
    }

    //
    // Return the result to the caller.
    //
    return(ulRet);
}

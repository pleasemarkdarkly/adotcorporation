//****************************************************************************
//
// WMA.C - Codec interface driver for the MSAudio decoder.
//
// Copyright (c) 1999,2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include "globals.h"
#ifdef SUPPORT_WMA
#include "wma/include/WMA_DEC_EMB_ARM.h"
#include "buffer/buffer.h"
#include "src/src.h"

//****************************************************************************
//
// The name of this codec.
//
//****************************************************************************
static const unsigned short pusCodecName[] =
{
    'W', 'M', 'A', ' ', 'W', 'i', 'n', 'd', 'o', 'w', 's', ' ',
    'M', 'e', 'd', 'i', 'a', '(', 't', 'm', ')', ' ', 'A', 'u',
    'd', 'i', 'o', '\0'
};

//****************************************************************************
//
// The GUID of the security chunk in a DRM protected file.
//
//****************************************************************************
static const unsigned char pucSecurityGUID[16] =
{
    0xfb, 0xb3, 0x11, 0x22, 0x23, 0xbd, 0xd2, 0x11,
    0xb4, 0xb7, 0x00, 0xa0, 0xc9, 0x55, 0xfc, 0x6e
};

//****************************************************************************
//
// The size of the output buffer that will be used.
//
//****************************************************************************
#define WMA_MAX_PCM_LENGTH              1536

//****************************************************************************
//
// A structure which defines the persistent state of the WMA decoder.
//
//****************************************************************************
typedef struct
{
    //
    // The persistent internal state of the WMA decoder library.  This must be
    // the first member of this structure.
    //
    tWMAFileState sWMAState;

    //
    // The file from which we read data.
    //
    tFile *pFile;

    //
    // A buffer to contain the WMA file contents
    //
    unsigned char pucWMAData[1024];

    //
    // The offset of the data currently in the pucWMAData buffer.
    //
    tWMA_U32 dwOffset;

    //
    // The length of the data currently in the pucWMAData buffer.
    //
    tWMA_U32 dwLength;

    //
    // Buffers to contain the decoded WMA audio.
    //
    short psLeft[WMA_MAX_PCM_LENGTH + NUMTAPS - 1];
    short psRight[WMA_MAX_PCM_LENGTH + NUMTAPS - 1];

    //
    // The buffer to which we write decoded WMA audio.
    //
    BufferState *pOutput;

    //
    // The number of samples that have been played.
    //
    tWMA_U32 dwTimePos;

    //
    // The sample rate of the decoded PCM data.
    //
    tWMA_U32 dwSampleRate;

    //
    // The file header information structure.
    //
    tWMAFileHeader sWMAHeader;

    //
    // The file content descriptor structure.
    //
    tWMAFileContDesc sWMADesc;

    //
    // A buffer to contain the song title.
    //
    unsigned short pusTitle[64];

    //
    // A buffer to contain the song author.
    //
    unsigned short pusAuthor[64];

    //
    // A boolean that is true when there is more PCM data to be decoded for
    // the current WMA frame.
    //
    tWMA_U32 bMorePCMToGenerate;
} tWMA;

//****************************************************************************
//
// WMADebugMessage is called by the WMA decoder to display a human readable
// error message intended for the developer, not the end user.
//
//****************************************************************************
void
WMADebugMessage(const char *pszFmt, ...)
{
    //
    // Do nothing.
    //
}

//****************************************************************************
//
// Microsoft insists upon releasing debug versions of their code...
//
//****************************************************************************
void
__assert(void)
{
    //
    // Do nothing.
    //
}

//****************************************************************************
//
// Microsoft can't figure out how to #ifdef out the calls the malloc that the
// embedded version of the WMA library never makes since it never takes those
// code paths.
//
//****************************************************************************
void *
malloc(void)
{
    //
    // Return a NULL pointer, since we do not support memory allocation.  This
    // is making a bold assumption that Microsoft actually checks the returned
    // value...
    //
    return(0);
}

//****************************************************************************
//
// WMAFileCBGetLicenseData is called by the WMA decoder when it needs to read
// more bytes from the license file.
//
//****************************************************************************
tWMA_U32
WMAFileCBGetLicenseData(tWMAFileState *psState, tWMA_U32 dwOffset,
                        tWMA_U32 dwNumBytes, unsigned char **ppData)
{
    //
    // We don't support a separate license file, so return zero bytes.
    //
    return(0);
}

//****************************************************************************
//
// WMAFileCBGetData is called by the WMA decoder when it needs to read more
// data for decoding the file.
//
//****************************************************************************
tWMA_U32
WMAFileCBGetData(tWMAFileState *psState, tWMA_U32 dwOffset,
                 tWMA_U32 dwNumBytes, unsigned char **ppData)
{
    tWMA *pWMA;
    tWMA_U32 dwIndex;

    //
    // Get a pointer to the persistent state structure.
    //
    pWMA = (tWMA *)psState;

    //
    // Make sure that the requested read is not for too many bytes.
    //
    if(dwNumBytes > WMA_MAX_DATA_REQUESTED)
    {
        return(0);
    }

    //
    // Determine if the requested data already exists in the buffer.
    //
    if((dwOffset >= pWMA->dwOffset) &&
       ((dwOffset + dwNumBytes) < (pWMA->dwOffset + pWMA->dwLength)))
    {
        //
        // The requested data already exists in the buffer, so do not read it
        // from the file system.
        //
        *ppData = pWMA->pucWMAData + (dwOffset - pWMA->dwOffset);

        //
        // Return the number of bytes "read".
        //
        return(dwNumBytes);
    }

    //
    // Return the offset into the buffer of the requested byte of file data.
    // This could be non-zero since we only read on 512 byte boundaries from
    // the filesystem.
    // 
    dwIndex = dwOffset & 0x1ff;

    //
    // Return a pointer to the buffer containing the newly read file data.
    //
    *ppData = pWMA->pucWMAData + dwIndex;

    //
    // Seek to the 512 byte boundary in the file before the requested file
    // position.
    //
    FSSeek(pWMA->pFile, dwOffset & ~0x1ff);

    //
    // Read the requested number of bytes from the file.
    //
    FSRead(pWMA->pFile, pWMA->pucWMAData,
           (dwNumBytes + dwIndex + 511) & ~0x1ff);

    //
    // Keep track of the portion of the file that is currently in the buffer.
    //
    pWMA->dwOffset = dwOffset & ~0x1ff;
    pWMA->dwLength = (dwNumBytes + dwIndex + 511) & ~0x1ff;

    //
    // Return the number of bytes read (i.e. requested).
    //
    return(dwNumBytes);
}

//****************************************************************************
//
// The codec plug-in entry point for the WMA decoder.
//
//****************************************************************************
unsigned long
WMAIoctl(unsigned long ulIoctl, unsigned long ulParam1, unsigned long ulParam2,
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
            tWMA *pWMA;

            //
            // The first parameter is a pointer to the WMA persistent data.
            //
            pWMA = (tWMA *)ulParam1;

            //
            // The second parameter is a pointer for the name.
            //
            ppusName = (unsigned short **)ulParam2;

            //
            // Return the name of the artist.
            //
            *ppusName = pWMA->pusAuthor;

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
            tWMA *pWMA;

            //
            // The first parameter is a pointer to the WMA persistent data.
            //
            pWMA = (tWMA *)ulParam1;

            //
            // The second parameter is a pointer for the name.
            //
            ppusName = (unsigned short **)ulParam2;

            //
            // Return the name of the song.
            //
            *ppusName = pWMA->pusTitle;

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
            tWMA *pWMA;

            //
            // The first parameter is a pointer to the WMA persistent data.
            //
            pWMA = (tWMA *)ulParam1;

            //
            // The second parameter is a pointer for the bitrate.
            //
            pulBitRate = (unsigned long *)ulParam2;

            //
            // Return the average bitrate of the file.
            //
            *pulBitRate = pWMA->sWMAHeader.bitrate;

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
            tWMA *pWMA;

            //
            // The first parameter is a pointer to the WMA persistent data.
            //
            pWMA = (tWMA *)ulParam1;

            //
            // The second parameter is a pointer for the sample rate.
            //
            pulSampleRate = (unsigned long *)ulParam2;

            //
            // Return the sample rate of the file.
            //
            *pulSampleRate = pWMA->dwSampleRate;

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
            tWMA *pWMA;

            //
            // The first parameter is a pointer to the WMA persistent data.
            //
            pWMA = (tWMA *)ulParam1;

            //
            // The second parameter is a pointer for the number of channels.
            //
            pulChannels = (unsigned long *)ulParam2;

            //
            // Return the number of channels in the file.
            //
            *pulChannels = pWMA->sWMAHeader.num_channels;

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
            tWMA *pWMA;

            //
            // The first parameter is a pointer to the WMA persistent data.
            //
            pWMA = (tWMA *)ulParam1;

            //
            // The second parameter is a pointer for the number of
            // milliseconds.
            //
            pulLength = (unsigned long *)ulParam2;

            //
            // Return the length of the file.
            //
            *pulLength = pWMA->sWMAHeader.duration;

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
            tWMA *pWMA;

            //
            // The first parameter is a pointer to the WMA persistent data.
            //
            pWMA = (tWMA *)ulParam1;

            //
            // The second parameter is a pointer for the number of
            // milliseconds.
            //
            pulTime = (unsigned long *)ulParam2;

            //
            // Determine the time based on the sample rate.
            //
            *pulTime = ((pWMA->dwTimePos / pWMA->dwSampleRate) * 1000) +
                       (((pWMA->dwTimePos % pWMA->dwSampleRate) * 1000) /
                        pWMA->dwSampleRate);

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
            // See if this is an ASF file.
            //
            if(CodecIsASFFile(pFile, pucScratch) == 0)
            {
                //
                // This is not an ASF file, so we will not be able to decode
                // it.
                //
                return(0);
            }

            //
            // See if the security chunk is in the file.
            //
            if(CodecFindASFChunk(pFile, pucScratch, pucSecurityGUID) == -1)
            {
                //
                // This file is not protected by DRM, so we will be able to
                // decode it.
                //
                return(1);
            }

            //
            // This file is protectedby DRM, and we can not determine if we
            // will be able to decode it.  So, just assume that we can not.
            // This really isn't correct...
            //
            return(0);
        }

        //
        // Prepare the codec to encode or decode a file.
        //
        case IOCTL_CODEC_OPEN:
        {
            tWMA *pWMA;
            tWMAFileLicParams sDeviceID;
            unsigned char ucID[32];
            unsigned long ulIDLength;

            //
            // If we are being asked to encode a WMA file, then return a
            // failure since we can only decode WMA.
            //
            if(ulParam2 & CODEC_OPEN_ENCODE)
            {
                return(0);
            }

            //
            // The first parameter is a pointer to the WMA persistent state.
            //
            pWMA = (tWMA *)ulParam1;

            //
            // Make sure that we have enough memory for this decoder.
            //
            if((ulParam1 + sizeof(tWMA)) > ulExtentOfRAM)
            {
                return(0);
            }

            //
            // Clear the internal structure.
            //
            memset((void *)&pWMA->sWMAState, 0, sizeof(pWMA->sWMAState));

            //
            // Save the pointer to the file structure.
            //
            pWMA->pFile = (tFile *)ulParam4;

            //
            // Indicate that there is no file data in the internal read buffer.
            //
            pWMA->dwOffset = 0;
            pWMA->dwLength = 0;

            //
            // Initialize the WMA decoder.
            //
            if(WMAFileDecodeInit(&pWMA->sWMAState) != cWMA_NoErr)
            {
                return(0);
            }

            //
            // Decode the file header.
            //
            if(WMAFileDecodeInfo(&pWMA->sWMAState,
                                 &pWMA->sWMAHeader) != cWMA_NoErr)
            {
                return(0);
            }

            //
            // If the file is protected by DRM, then we can not play it (at
            // this time).
            //
            if(pWMA->sWMAHeader.has_DRM)
            {
                //
                // If the license is not part of the file, then we can not
                // play the file.
                //
                if(pWMA->sWMAHeader.LicenseLength == 0)
                {
                    return(0);
                }

                //
                // Get the unique media ID for the drive on which this file
                // resides.
                //
                ulIDLength = 32 * 8;
                if(FSGetMediaID(FSDriveNum(pWMA->pFile), ucID,
                                &ulIDLength) == 0)
                {
                    return(0);
                }

                //
                // The ID supplied to the Microsoft DRM must be a multiple of
                // 8 bits in length.
                //
                if(ulIDLength & 7)
                {
                    return(0);
                }

                //
                // Supply the unique media ID to the DRM.
                //
                sDeviceID.pPMID = ucID;
                sDeviceID.cbPMID = ulIDLength / 8;

                //
                // Provide our device ID to the DRM library and if the license
                // does now allow the file to be played on this device then we
                // can not play the file.
                //
                if(WMAFileLicenseInit(&pWMA->sWMAState, &sDeviceID,
                                      CHECK_ALL_LICENSE) != cWMA_NoErr)
                {
                    return(0);
                }
            }

            //
            // Get the file content description.
            //
            pWMA->sWMADesc.title_len = 128;
            pWMA->sWMADesc.author_len = 128;
            pWMA->sWMADesc.copyright_len = 0;
            pWMA->sWMADesc.description_len = 0;
            pWMA->sWMADesc.rating_len = 0;
            pWMA->sWMADesc.pTitle = (unsigned char *)pWMA->pusTitle;
            pWMA->sWMADesc.pAuthor = (unsigned char *)pWMA->pusAuthor;
            pWMA->pusTitle[0] = '\0';
            pWMA->pusAuthor[0] = '\0';
            if(WMAFileContentDesc(&pWMA->sWMAState,
                                  &pWMA->sWMADesc) != cWMA_NoErr)
            {
                pWMA->sWMADesc.title_len = 0;
                pWMA->sWMADesc.author_len = 0;
            }

            //
            // The initial time position is zero.
            //
            pWMA->dwTimePos = 0;

            //
            // Get the sample rate of the file.
            //
            switch(pWMA->sWMAHeader.sample_rate)
            {
                //
                // The sample rate is 22.05kHz.
                //
                case cWMA_SR_22_05kHz:
                {
                    pWMA->dwSampleRate = 22050;
                    break;
                }

                //
                // The sample rate is 32kHz.
                //
                case cWMA_SR_32kHz:
                {
                    pWMA->dwSampleRate = 32000;
                    break;
                }

                //
                // The sample rate is 44.1kHz.
                //
                case cWMA_SR_44_1kHz:
                {
                    pWMA->dwSampleRate = 44100;
                    break;
                }

                //
                // This file has an unknown sample rate.
                //
                default:
                {
                    return(0);
                }
            }

            //
            // Initially, there is not any PCM data to be generated.
            //
            pWMA->bMorePCMToGenerate = 0;

            //
            // There is no output buffer.
            //
            pWMA->pOutput = 0;

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
            tWMA *pWMA;

            //
            // The first parameter is a pointer to the WMA persistent state.
            //
            pWMA = (tWMA *)ulParam1;

            //
            // The second parameter is a pointer to the output buffer.
            //
            pWMA->pOutput = (BufferState *)ulParam2;

            //
            // Provide the output buffer with our data buffers.
            //
            if(pWMA->sWMAHeader.num_channels == cWMA_C_Stereo)
            {
                BufferSetBuffer(pWMA->pOutput, pWMA->psLeft, pWMA->psRight,
                                WMA_MAX_PCM_LENGTH);
            }
            else
            {
                BufferSetBuffer(pWMA->pOutput, pWMA->psLeft, pWMA->psLeft,
                                WMA_MAX_PCM_LENGTH);
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
            tWMA *pWMA;

            //
            // The first parameter is a pointer to the WMA persistent state
            //
            pWMA = (tWMA *)ulParam1;

            //
            // See if there is space available for decoding the next frame.
            //
            if(BufferSpaceAvailable(pWMA->pOutput) < 256)
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
            tWMA *pWMA;
            short *psLeft, *psRight;
            long lSamples;

            //
            // The first parameter is a pointer to the WMA persistent state
            //
            pWMA = (tWMA *)ulParam1;

            //
            // The loop will only return in case of an error or when it
            // generated an output.
            //
            while(1)
            {
                //
                // See if there is more PCM data to be generated.
                //
                if(!pWMA->bMorePCMToGenerate)
                {
                    //
                    // There is no more PCM data available, so decode the next
                    // frame of the WMA file.
                    //
                    if(WMAFileDecodeData(&pWMA->sWMAState) != cWMA_NoErr)
                    {
                        //
                        // Either there was a file error or we've reached the
                        // end of the file.
                        //
                        return(0);
                    }

                    //
                    // There should be more PCM to be generated.
                    //
                    pWMA->bMorePCMToGenerate = 1;
                }

                //
                // Wait until there is an adequate amount of space available
                // in the output buffer.
                //
                while(BufferSpaceAvailable(pWMA->pOutput) < 256)
                {
                    //
                    // Busy wait until space is available.
                    //
                }

                //
                // Get a pointer to the output buffer
                //
                BufferGetWritePointer(pWMA->pOutput, &psLeft, &psRight,
                                      &lSamples);

                //
                // Generate PCM data into the output buffer.
                //
                lSamples = WMAFileGetPCM(&pWMA->sWMAState, psLeft, psRight,
                                         lSamples);

                //
                // See if we actually generated any samples.
                //
                if(lSamples > 0)
                {
                    //
                    // Advance the write pointer of the output buffer by the
                    // number of samples we produced.
                    //
                    BufferUpdateWritePointer(pWMA->pOutput, lSamples);

                    //
                    // Increment the time position by the number of samples
                    // we produced.
                    //
                    pWMA->dwTimePos += lSamples;

                    //
                    // Success
                    //
                    return(1);
                }
                else
                {
                    //
                    // There is no more PCM data, so indicate that we must
                    // decode the next frame.
                    //
                    pWMA->bMorePCMToGenerate = 0;
                }
            }

            //
            // We should never get here, but just in case...
            //
            break;
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
        /// Seek to the specified time position.
        //
        case IOCTL_CODEC_SEEK:
        {
            tWMA *pWMA;

            //
            // The first parameter is a pointer to the WMA persistent state
            //
            pWMA = (tWMA *)ulParam1;

            //
            // Seek to the specified position.
            //
            ulParam2 = WMAFileSeek(&pWMA->sWMAState, ulParam2);

            //
            // Set the time position correctly.
            //
            pWMA->dwTimePos = ((ulParam2 / 1000) * pWMA->dwSampleRate) +
                              (((ulParam2 % 1000) * pWMA->dwSampleRate) /
                               1000);

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

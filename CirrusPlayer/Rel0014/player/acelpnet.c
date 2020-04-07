//****************************************************************************
//
// ACELPNET.C - Codec interface driver for the ACELP.net decoder.
//
// This material is strictly confidential and shall remain as such.
//
// Copyright (c) 1999 VoiceAge Corporation.  All Rights Reserved.  Any
// unauthorized use, distribution, reproduction or storage of this material or
// any part thereof is strictly prohibited.
//
// ACELP is registered trademark of VoiceAge Corporation in Canada and/or
// other countries.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include "globals.h"
#ifdef SUPPORT_ACELPNET
#include "acelpnet/acelp.h"
#include "acelpnet/acelpdec.h"
#include "buffer/buffer.h"
#include "src/src.h"

//****************************************************************************
//
// The name of this codec.
//
//****************************************************************************
static const unsigned short pusCodecName[] =
{
    'A', 'N', 'E', 'T', 'A', 'C', 'E', 'L', 'P', '.', 'n', 'e',
    't', '\0'
};

//****************************************************************************
//
// A structure which defines the persistent state of the ACELP.net decoder.
//
//****************************************************************************
typedef struct
{
    //
    // The persistent internal state of the ACELP.net decoder library.
    //
    tANETInstance sANETInstance;

    //
    // The file from which we read data.
    //
    tFile *pFile;

    //
    // A buffer to contain the encoded ACELP.net audio.
    //
    char pcEncodedData[512 + 64];

    //
    // A buffer to contain the decoded ACELP.net audio.
    //
    short psPCM[(L_FRAME_5K0 * 4) + NUMTAPS - 1];

    //
    // The buffer to which we write decoded ACELP.net data.
    //
    BufferState *pOutput;

    //
    // The length of the encoded ACELP.net file.
    //
    unsigned long ulLength;

    //
    // The number of bytes in pcEncodedData which contain valid encoded
    // ACELP.net audio.
    //
    unsigned short usValid;

    //
    // The offset into pcEncodedData of the next ACELP.net frame to be
    // processed.
    //
    unsigned short usOffset;

    //
    // The length of the file in milliseconds.
    //
    unsigned long ulTimeLength;

    //
    // The number of samples that have been played.
    //
    unsigned long ulTimePos;

    //
    // The current offset into the ACELP.net file, indicating the number of
    // bytes which have been read from the file into the encoded data buffer.
    //
    unsigned long ulFilePos;

    //
    // The sample rate of the file.
    //
    unsigned short usSampleRate;

    //
    // The bit rate of the file.
    //
    unsigned short usBitRate;

    //
    // The number of samples per frame.
    //
    unsigned short usSamplesPerFrame;
} tANET;

//****************************************************************************
//
// The number of bits in the various fields of a frame of a 5kbps stream.
//
//****************************************************************************
#ifdef SUPPORT_ACELPNET_5K0
static const char bit_5k0[NB_PRM_5K0] = BIT_5K0;
#endif

//****************************************************************************
//
// The number of bits in the various fields of a frame of a 6.5kbps stream.
//
//****************************************************************************
#ifdef SUPPORT_ACELPNET_6K5
static const char bit_6k5[NB_PRM_6K5] = BIT_6K5;
#endif

//****************************************************************************
//
// The number of bits in the various fields of a frame of a 8.5kbps stream.
//
//****************************************************************************
#ifdef SUPPORT_ACELPNET_8K5
static const char bit_8k5[NB_PRM_8K5] = BIT_8K5;
#endif

//****************************************************************************
//
// The number of bits in the various fields of a frame of a 16kbps stream.
//
//****************************************************************************
#ifdef SUPPORT_ACELPNET_16K0
static const char bit_16k0[NB_PRM_16K0] = BIT_16K0;
#endif

//****************************************************************************
//
// copybits performs a bit copy from one arbitrary bit position to another.
//
//****************************************************************************
static void
copybits(unsigned char *toPtr, unsigned char *fromPtr, int bitOffsetTo,
         int bitOffsetFrom, int numBits)
{
    int i;
    int currBitOffsetTo, currBitOffsetFrom;
    int currBitOffsetToMod8, currBitOffsetFromMod8;
    unsigned char *currToPtr, *currFromPtr;

    currToPtr = toPtr + (bitOffsetTo >> 3);
    currBitOffsetTo = bitOffsetTo;
    currBitOffsetToMod8 = currBitOffsetTo % 8;

    currFromPtr = fromPtr + (bitOffsetFrom >> 3);
    currBitOffsetFrom = bitOffsetFrom;
    currBitOffsetFromMod8 = currBitOffsetFrom % 8;

    for(i = 0; i < numBits; i++)
    {
        if((0x01 & ((*currFromPtr) >> currBitOffsetFromMod8)) == 0x01)
        {
            (*currToPtr) |= ((unsigned char)(1 << currBitOffsetToMod8));
        }
        else
        {
            (*currToPtr) &= ((unsigned char)~(1 << currBitOffsetToMod8));
        }
        if(++currBitOffsetFromMod8 == 8)
        {
            currBitOffsetFromMod8 = 0;
            currFromPtr++;
        }
        if(++currBitOffsetToMod8 == 8)
        {
            currBitOffsetToMod8 = 0;
            currToPtr++;
        }
    }
}

//****************************************************************************
//
// binary2prm converts the input bitstream into a parameter vector, based on
// the bitrate of the input bitstream.
//
//****************************************************************************
static void
binary2prm(short prm[], unsigned char bits[], int numPrm, int startPrm,
           const char bitsTable[])
{
    int i, totBits = 0;
    unsigned char tmpPrm[4];

    for(i = startPrm; i < numPrm; i++)
    {
        *((int *)tmpPrm) = 0;
        copybits(tmpPrm, bits, 0, totBits, bitsTable[i]);

        prm[i] = (short)(*((int *)(tmpPrm)));

        totBits += bitsTable[i];
    }
}

//****************************************************************************
//
// Decode5k0 decodes two frames from a 5kbps stream.
//
//****************************************************************************
#ifdef SUPPORT_ACELPNET_5K0
static long
Decode5k0(tANET *pANET, short *psOutput)
{
    short sPRM[NB_PRM_5K0];

    if((pANET->usOffset + BLOCK_ALIGNMENT_5K0) > pANET->usValid)
    {
        pANET->usOffset = pANET->usValid;
        return(0);
    }

    binary2prm(sPRM, (unsigned char *)pANET->pcEncodedData + pANET->usOffset,
               NB_PRM_5K0, 0, bit_5k0);
    decoder_5k(sPRM, psOutput, 0, &(pANET->sANETInstance));

    copybits((unsigned char *)pANET->pcEncodedData + pANET->usOffset,
             (unsigned char *)pANET->pcEncodedData + pANET->usOffset, 0, 148,
             148);

    binary2prm(sPRM, (unsigned char *)pANET->pcEncodedData + pANET->usOffset,
               NB_PRM_5K0, 0, bit_5k0);
    decoder_5k(sPRM, psOutput + L_FRAME_5K0, 0, &(pANET->sANETInstance));

    pANET->usOffset += BLOCK_ALIGNMENT_5K0;

    return(L_FRAME_5K0 * 2);
}
#endif

//****************************************************************************
//
// Decode6k5 decodes two frames from a 6.5kbps stream.
//
//****************************************************************************
#ifdef SUPPORT_ACELPNET_6K5
static long
Decode6k5(tANET *pANET, short *psOutput)
{
    short sPRM[NB_PRM_6K5];

    if((pANET->usOffset + BLOCK_ALIGNMENT_6K5) > pANET->usValid)
    {
        pANET->usOffset = pANET->usValid;
        return(0);
    }

    sPRM[0] = RATE_6K5;

    binary2prm(sPRM, (unsigned char *)pANET->pcEncodedData + pANET->usOffset,
               NB_PRM_6K5, 1, bit_6k5);
    dec_swt(sPRM, psOutput, 0, &(pANET->sANETInstance));

    copybits((unsigned char *)pANET->pcEncodedData + pANET->usOffset,
             (unsigned char *)pANET->pcEncodedData + pANET->usOffset, 0, 116,
             116);

    binary2prm(sPRM, (unsigned char *)pANET->pcEncodedData + pANET->usOffset,
               NB_PRM_6K5, 1, bit_6k5);
    dec_swt(sPRM, psOutput + L_FRAME_6K5, 0, &(pANET->sANETInstance));

    pANET->usOffset += BLOCK_ALIGNMENT_6K5;

    return(L_FRAME_6K5 * 2);
}
#endif

//****************************************************************************
//
// Decode8k5 decodes two frames from a 8.5kbps stream.
//
//****************************************************************************
#ifdef SUPPORT_ACELPNET_8K5
static long
Decode8k5(tANET *pANET, short *psOutput)
{
    short sPRM[NB_PRM_8K5];

    if((pANET->usOffset + (BLOCK_ALIGNMENT_8K5 * 2)) > pANET->usValid)
    {
        pANET->usOffset = pANET->usValid;
        return(0);
    }

    sPRM[0] = RATE_8K5;

    binary2prm(sPRM, (unsigned char *)pANET->pcEncodedData + pANET->usOffset,
               NB_PRM_8K5, 1, bit_8k5);
    dec_swt(sPRM, psOutput, 0, &(pANET->sANETInstance));

    pANET->usOffset += BLOCK_ALIGNMENT_8K5;

    binary2prm(sPRM, (unsigned char *)pANET->pcEncodedData + pANET->usOffset,
               NB_PRM_8K5, 1, bit_8k5);
    dec_swt(sPRM, psOutput + L_FRAME_8K5, 0, &(pANET->sANETInstance));

    pANET->usOffset += BLOCK_ALIGNMENT_8K5;

    return(L_FRAME_8K5 * 2);
}
#endif

//****************************************************************************
//
// Decode16k0 decodes two frames from a 16kbps stream.
//
//****************************************************************************
#ifdef SUPPORT_ACELPNET_16K0
static long
Decode16k0(tANET *pANET, short *psOutput)
{
    short sPRM[NB_PRM_16K0];

    if((pANET->usOffset + (BLOCK_ALIGNMENT_16K0 * 2)) > pANET->usValid)
    {
        pANET->usOffset = pANET->usValid;
        return(0);
    }

    binary2prm(sPRM, (unsigned char *)pANET->pcEncodedData + pANET->usOffset,
               NB_PRM_16K0, 0, bit_16k0);
    decod_wb(sPRM, psOutput, 0, &(pANET->sANETInstance));

    pANET->usOffset += BLOCK_ALIGNMENT_16K0;

    binary2prm(sPRM, (unsigned char *)pANET->pcEncodedData + pANET->usOffset,
               NB_PRM_16K0, 0, bit_16k0);
    decod_wb(sPRM, psOutput + L_FRAME_16K0, 0, &(pANET->sANETInstance));

    pANET->usOffset += BLOCK_ALIGNMENT_16K0;

    return(L_FRAME_16K0 * 2);
}
#endif

//****************************************************************************
//
// The codec plug-in entry point for the ACELP.net decoder.
//
//****************************************************************************
unsigned long
ACELPnetIoctl(unsigned long ulIoctl, unsigned long ulParam1,
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

            //
            // The second parameter is a pointer for the name.
            //
            ppusName = (unsigned short **)ulParam2;

            //
            // There is no way to store the artist name in an ACELP.net file.
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
            // There is no way to store the song title in an ACELP.net file.
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
            tANET *pANET;

            //
            // The first parameter is a pointer to the ACELP.net persistent
            // data.
            //
            pANET = (tANET *)ulParam1;

            //
            // The second parameter is a pointer for the bitrate.
            //
            pulBitRate = (unsigned long *)ulParam2;

            //
            // Return the bitrate of the file.
            //
            *pulBitRate = pANET->usBitRate;

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
            tANET *pANET;

            //
            // The first parameter is a pointer to the ACELP.net persistent
            // data.
            //
            pANET = (tANET *)ulParam1;

            //
            // The second parameter is a pointer for the sample rate.
            //
            pulSampleRate = (unsigned long *)ulParam2;

            //
            // Return the sample rate of the file.
            //
            *pulSampleRate = pANET->usSampleRate;

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
            tANET *pANET;

            //
            // The first parameter is a pointer to the ACELP.net persistent
            // data.
            //
            pANET = (tANET *)ulParam1;

            //
            // The second parameter is a pointer for the number of channels.
            //
            pulChannels = (unsigned long *)ulParam2;

            //
            // Return the number of channels in the file.
            //
            *pulChannels = 1;

            //
            // Success.
            //
            return(1);
        }

        //
        // Return the length (in milliseconds) of the output file.
        //
        case IOCTL_CODEC_GETLENGTH:
        {
            unsigned long *pulLength;
            tANET *pANET;

            //
            // The first parameter is a pointer to the ACELP.net persistent
            // data.
            //
            pANET = (tANET *)ulParam1;

            //
            // The second parameter is a pointer for the number of
            // milliseconds.
            //
            pulLength = (unsigned long *)ulParam2;

            //
            // Return the length of the file.
            //
            *pulLength = pANET->ulTimeLength;

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
            tANET *pANET;

            //
            // The first parameter is a pointer to the ACELP.net persistent
            // data.
            //
            pANET = (tANET *)ulParam1;

            //
            // The second parameter is a pointer for the number of
            // milliseconds.
            //
            pulTime = (unsigned long *)ulParam2;

            //
            // Determine the time based on the sample rate.
            //
            *pulTime = pANET->ulTimePos / (pANET->usSampleRate / 1000);

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
            tANET *pANET;

            //
            // If we are being asked to encode an ACELP.net file, then return a
            // failure since we can only decode ACELP.net.
            //
            if(ulParam3 & CODEC_OPEN_ENCODE)
            {
                return(0);
            }

            //
            // The first parameter is a pointer to the ACELP.net persistent
            // state.
            //
            pANET = (tANET *)ulParam1;

            //
            // Make sure that we have enough memory for this decoder.
            //
            if(((ulParam1 + sizeof(tANET)) & 0xFFFF) > 0x9600)
            {
                return(0);
            }

            //
            // Save the pointer to the file structure.
            //
            pANET->pFile = (tFile *)ulParam4;

            //
            // Read the first page of the file.
            //
            FSRead(pANET->pFile, pANET->pcEncodedData, 512);

            //
            // Make sure that the first four bytes of the file are "RIFF".
            //
            if((pANET->pcEncodedData[0] != 'R') ||
               (pANET->pcEncodedData[1] != 'I') ||
               (pANET->pcEncodedData[2] != 'F') ||
               (pANET->pcEncodedData[3] != 'F'))
            {
                return(0);
            }

            //
            // Make sure that the the third set of four bytes of the file are
            // "WAVE".
            //
            if((pANET->pcEncodedData[8] != 'W') ||
               (pANET->pcEncodedData[9] != 'A') ||
               (pANET->pcEncodedData[10] != 'V') ||
               (pANET->pcEncodedData[11] != 'E'))
            {
                return(0);
            }

            //
            // Make sure that the first chunk of the file is the "fmt " chunk.
            //
            if((pANET->pcEncodedData[12] != 'f') ||
               (pANET->pcEncodedData[13] != 'm') ||
               (pANET->pcEncodedData[14] != 't') ||
               (pANET->pcEncodedData[15] != ' '))
            {
                return(0);
            }

            //
            // Make sure that the format of the file is ACELP.net.
            //
            if((pANET->pcEncodedData[20] != 0x30) ||
               (pANET->pcEncodedData[21] != 0x01))
            {
                return(0);
            }

            //
            // Make sure that the "data" chunk starts where we expect it to.
            //
            if((pANET->pcEncodedData[54] != 'd') ||
               (pANET->pcEncodedData[55] != 'a') ||
               (pANET->pcEncodedData[56] != 't') ||
               (pANET->pcEncodedData[57] != 'a'))
            {
                return(0);
            }

            //
            // Since we've just read the first page of the file, the next read
            // will occur at location 512 into the file.
            //
            pANET->ulFilePos = 512;

            //
            // Get the length of the ACELP.net file.
            //
            pANET->ulLength = FSLength(pANET->pFile);

            //
            // Initially, the 512 bytes previously read into the buffer are
            // valid.
            //
            pANET->usValid = 512;

            //
            // Make sure the file position and the valid data size are less
            // than the file length.
            //
            if(pANET->ulFilePos > pANET->ulLength)
            {
                pANET->ulFilePos = pANET->ulLength;
                pANET->usValid = pANET->ulLength;
            }

            //
            // The initial time position is zero.
            //
            pANET->ulTimePos = 0;

            //
            // Start play from the sixth-second byte of the buffer.
            //
            pANET->usOffset = 62;

            //
            // Get the bit rate from the RIFF header.
            //
            switch(pANET->pcEncodedData[38])
            {
                //
                // The bitrate is 5kbps.
                //
#ifdef SUPPORT_ACELPNET_5K0
                case ACELPNET_5K0:
                {
                    //
                    // Remember the bitrate of the file.
                    //
                    pANET->usBitRate = 4933;

                    //
                    // Remember the sample rate of the file.
                    //
                    pANET->usSampleRate = 8000;

                    //
                    // Remember the number of samples per frame.
                    //
                    pANET->usSamplesPerFrame = L_FRAME_5K0;

                    //
                    // Compute the length of the file in milliseconds.
                    //
                    pANET->ulTimeLength = (((pANET->ulLength - 62) /
                                            BLOCK_ALIGNMENT_5K0) *
                                           SAMPLES_PER_FRAME_5K0) / 8;

                    //
                    // Initialize the ACELP.net decoder.
                    //
                    init_decoder_5k(&(pANET->sANETInstance));

                    //
                    // We're done handling this bit rate.
                    //
                    break;
                }
#endif

                //
                // The bitrate is 6.5kbps.
                //
#ifdef SUPPORT_ACELPNET_6K5
                case ACELPNET_6K5:
                {
                    //
                    // Remember the bitrate of the file.
                    //
                    pANET->usBitRate = 6444;

                    //
                    // Remember the sample rate of the file.
                    //
                    pANET->usSampleRate = 8000;

                    //
                    // Remember the number of samples per frame.
                    //
                    pANET->usSamplesPerFrame = L_FRAME_6K5;

                    //
                    // Compute the length of the file in milliseconds.
                    //
                    pANET->ulTimeLength = (((pANET->ulLength - 62) /
                                            BLOCK_ALIGNMENT_6K5) *
                                           SAMPLES_PER_FRAME_6K5) / 8;

                    //
                    // Initialize the ACELP.net decoder.
                    //
                    init_dec_swt(&(pANET->sANETInstance));

                    //
                    // We're done handling this bit rate.
                    //
                    break;
                }
#endif

                //
                // The bitrate is 8.5kbps.
                //
#ifdef SUPPORT_ACELPNET_8K5
                case ACELPNET_8K5:
                {
                    //
                    // Remember the bitrate of the file.
                    //
                    pANET->usBitRate = 8444;

                    //
                    // Remember the sample rate of the file.
                    //
                    pANET->usSampleRate = 8000;

                    //
                    // Remember the number of samples per frame.
                    //
                    pANET->usSamplesPerFrame = L_FRAME_8K5;

                    //
                    // Compute the length of the file in milliseconds.
                    //
                    pANET->ulTimeLength = (((pANET->ulLength - 62) /
                                            BLOCK_ALIGNMENT_8K5) *
                                           SAMPLES_PER_FRAME_8K5) / 8;

                    //
                    // Initialize the ACELP.net decoder.
                    //
                    init_dec_swt(&(pANET->sANETInstance));

                    //
                    // We're done handling this bit rate.
                    //
                    break;
                }
#endif

                //
                // The bitrate is 16kbps.
                //
#ifdef SUPPORT_ACELPNET_16K0
                case ACELPNET_16K0:
                {
                    //
                    // Remember the bitrate of the file.
                    //
                    pANET->usBitRate = 16000;

                    //
                    // Remember the sample rate of the file.
                    //
                    pANET->usSampleRate = 16000;

                    //
                    // Remember the number of samples per frame.
                    //
                    pANET->usSamplesPerFrame = L_FRAME_16K0;

                    //
                    // Compute the length of the file in milliseconds.
                    //
                    pANET->ulTimeLength = (((pANET->ulLength - 62) /
                                            BLOCK_ALIGNMENT_16K0) *
                                           SAMPLES_PER_FRAME_16K0) / 16;

                    //
                    // Initialize the ACELP.net decoder.
                    //
                    init_decod_wb(&(pANET->sANETInstance));

                    //
                    // We're done handling this bit rate.
                    //
                    break;
                }
#endif

                //
                // This is an unknown bitrate, so return an error.
                //
                default:
                {
                    return(0);
                }
            }

            //
            // There is no output buffer.
            //
            pANET->pOutput = 0;

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
            tANET *pANET;

            //
            // The first parameter is a pointer to the ACELP.net persistent
            // state.
            //
            pANET = (tANET *)ulParam1;

            //
            // The second parameter is a pointer to the output buffer.
            //
            pANET->pOutput = (BufferState *)ulParam2;

            //
            // Provide the output buffer with our data buffers.
            //
            BufferSetBuffer(pANET->pOutput, pANET->psPCM, pANET->psPCM,
                            (pANET->usSamplesPerFrame * 4));

            //
            // Success.
            //
            return(1);
        }

        //
        // Decode data.
        //
        case IOCTL_CODEC_DECODE:
        {
            tANET *pANET;
            short *psLeft, *psRight;
            long lLength;

            //
            // The first parameter is a pointer to the ACELP.net persistent
            // state.
            //
            pANET = (tANET *)ulParam1;

            //
            // See if we've reached the end of the file.
            //
            if((pANET->usValid == pANET->usOffset) &&
               (pANET->ulFilePos == pANET->ulLength))
            {
                return(0);
            }

            //
            // See if we need to read more data from the file.
            //
            if(((pANET->usValid - pANET->usOffset) < 64) &&
               (pANET->ulFilePos != pANET->ulLength))
            {
                //
                // Copy the tail of the buffer to the beginning.
                //
                memcpy(pANET->pcEncodedData,
                       pANET->pcEncodedData + (pANET->usOffset & ~3),
                       pANET->usValid - (pANET->usOffset & ~3));

                //
                // Read the next 512 bytes from the file.
                //
                FSRead(pANET->pFile,
                       pANET->pcEncodedData + pANET->usValid -
                       (pANET->usOffset & ~3),
                       512);

                //
                // Update the buffer pointers.
                //
                if((pANET->ulLength - pANET->ulFilePos) < 512)
                {
                    pANET->usValid += pANET->ulLength - pANET->ulFilePos -
                                      (pANET->usOffset & ~3);
                }
                else
                {
                    pANET->usValid += 512 - (pANET->usOffset & ~3);
                }
                pANET->usOffset &= 3;

                //
                // Update the position within the file.
                //
                pANET->ulFilePos += 512;
                if(pANET->ulFilePos > pANET->ulLength)
                {
                    pANET->ulFilePos = pANET->ulLength;
                }
            }

            //
            // Wait until there is space in the output buffer.  This tells us
            // when it is OK to write more data into the output buffer.
            //
            while(BufferSpaceAvailable(pANET->pOutput) <
                  (pANET->usSamplesPerFrame * 2))
            {
                //
                // Put the EP7209 into IDLE mode.
                //
                Halt();
            }

            //
            // Get the write pointer for the output buffer.
            //
            BufferGetWritePointer(pANET->pOutput, &psLeft, &psRight, &lLength);

            //
            // Call the appropriate decoder based on the bitrate.
            //
            switch(pANET->usBitRate)
            {
                //
                // Decode a 5kbps stream.
                //
#ifdef SUPPORT_ACELPNET_5K0
                case 4933:
                {
                    //
                    // Call the 5kbps decoder.
                    //
                    lLength = Decode5k0(pANET, psLeft);

                    //
                    // We're done handling this stream.
                    //
                    break;
                }
#endif

                //
                // Decode a 6.5kbps stream.
                //
#ifdef SUPPORT_ACELPNET_6K5
                case 6444:
                {
                    //
                    // Call the 6.5kbps decoder.
                    //
                    lLength = Decode6k5(pANET, psLeft);

                    //
                    // We're done handling this stream.
                    //
                    break;
                }
#endif

                //
                // Decode a 8.5kbps stream.
                //
#ifdef SUPPORT_ACELPNET_8K5
                case 8444:
                {
                    //
                    // Call the 8.5kbps decoder.
                    //
                    lLength = Decode8k5(pANET, psLeft);

                    //
                    // We're done handling this stream.
                    //
                    break;
                }
#endif

                //
                // Decode at 16kbps stream.
                //
#ifdef SUPPORT_ACELPNET_16K0
                case 16000:
                {
                    //
                    // Call the 16kbps decoder.
                    //
                    lLength = Decode16k0(pANET, psLeft);

                    //
                    // We're done handling this stream.
                    //
                    break;
                }
#endif
            }

            //
            // Update the output buffer write pointer.
            //
            BufferUpdateWritePointer(pANET->pOutput, lLength);

            //
            // Increment the time.
            //
            pANET->ulTimePos += lLength;

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
            tANET *pANET;

            //
            // The first parameter is a pointer to the ACELP.net persistent
            // state.
            //
            pANET = (tANET *)ulParam1;

            //
            // Make sure that the specified time is valid.
            //
            if(ulParam2 > pANET->ulTimeLength)
            {
                ulParam2 = pANET->ulTimeLength;
            }

            //
            // Compute the seek position based on the bitrate.
            //
            switch(pANET->usBitRate)
            {
                //
                // The bitrate is 5kbps.
                //
#ifdef SUPPORT_ACELPNET_5K0
                case 4933:
                {
                    //
                    // Convert the seek point from time to bytes.
                    //
                    pANET->ulFilePos = ((ulParam2 / MS_PER_FRAME_TOTAL_5K0) *
                                        BLOCK_ALIGNMENT_5K0) + 62;

                    //
                    // Convert the seek point from bytes to samples.
                    //
                    pANET->ulTimePos = ((pANET->ulFilePos - 62) /
                                        BLOCK_ALIGNMENT_5K0) *
                                       SAMPLES_PER_FRAME_5K0;

                    //
                    // Initialize the ACELP.net decoder.
                    //
                    init_decoder_5k(&(pANET->sANETInstance));

                    //
                    // We're done handling this bitrate.
                    //
                    break;
                }
#endif

                //
                // The bitrate is 6.5kbps.
                //
#ifdef SUPPORT_ACELPNET_6K5
                case 6444:
                {
                    //
                    // Convert the seek point from time to bytes.
                    //
                    pANET->ulFilePos = ((ulParam2 / MS_PER_FRAME_TOTAL_6K5) *
                                        BLOCK_ALIGNMENT_6K5) + 62;

                    //
                    // Convert the seek point from bytes to samples.
                    //
                    pANET->ulTimePos = ((pANET->ulFilePos - 62) /
                                        BLOCK_ALIGNMENT_6K5) *
                                       SAMPLES_PER_FRAME_6K5;

                    //
                    // Initialize the ACELP.net decoder.
                    //
                    init_dec_swt(&(pANET->sANETInstance));

                    //
                    // We're done handling this bitrate.
                    //
                    break;
                }
#endif

                //
                // The bitrate is 8.5kbps.
                //
#ifdef SUPPORT_ACELPNET_8K5
                case 8444:
                {
                    //
                    // Convert the seek point from time to bytes.
                    //
                    pANET->ulFilePos = ((ulParam2 /
                                         (MS_PER_FRAME_TOTAL_8K5 * 2)) *
                                        (BLOCK_ALIGNMENT_8K5 * 2)) + 62;

                    //
                    // Convert the seek point from bytes to samples.
                    //
                    pANET->ulTimePos = ((pANET->ulFilePos - 62) /
                                        (BLOCK_ALIGNMENT_8K5 * 2)) *
                                       (SAMPLES_PER_FRAME_8K5 * 2);

                    //
                    // Initialize the ACELP.net decoder.
                    //
                    init_dec_swt(&(pANET->sANETInstance));

                    //
                    // We're done handling this bitrate.
                    //
                    break;
                }
#endif

                //
                // The bitrate is 16kbps.
                //
#ifdef SUPPORT_ACELPNET_16K0
                case 16000:
                {
                    //
                    // Convert the seek point from time to bytes.
                    //
                    pANET->ulFilePos = ((ulParam2 /
                                         (MS_PER_FRAME_TOTAL_16K0 * 2)) *
                                        (BLOCK_ALIGNMENT_16K0 * 2)) + 62;

                    //
                    // Convert the seek point from bytes to samples.
                    //
                    pANET->ulTimePos = ((pANET->ulFilePos - 62) /
                                        (BLOCK_ALIGNMENT_16K0 * 2)) *
                                       (SAMPLES_PER_FRAME_16K0 * 2);

                    //
                    // Initialize the ACELP.net decoder.
                    //
                    init_decod_wb(&(pANET->sANETInstance));

                    //
                    // We're done handling this bitrate.
                    //
                    break;
                }
#endif
            }

            //
            // Seek to the specified position.
            //
            FSSeek(pANET->pFile, pANET->ulFilePos & ~511);

            //
            // Read the next 512 bytes from the file.
            //
            FSRead(pANET->pFile, pANET->pcEncodedData, 512);

            //
            // Update the buffer pointers.
            //
            if((pANET->ulLength - pANET->ulFilePos) < 512)
            {
                pANET->usValid = pANET->ulLength - pANET->ulFilePos;
            }
            else
            {
                pANET->usValid = 512;
            }
            pANET->usOffset = pANET->ulFilePos & 511;

            //
            // Update the position within the file.
            //
            pANET->ulFilePos = (pANET->ulFilePos + 512) & ~511;
            if(pANET->ulFilePos > pANET->ulLength)
            {
                pANET->ulFilePos = pANET->ulLength;
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

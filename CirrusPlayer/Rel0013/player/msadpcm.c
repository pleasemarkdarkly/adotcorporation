//****************************************************************************
//
// MSADPCM.C - Codec interface driver for the Microsoft ADPCM encoder/decoder.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
//
//  msadpcm.c
//
//   (C) Copyright Microsoft Corp. 1993.  All rights reserved.
//
//   You have a royalty-free right to use, modify, reproduce and 
//   distribute the Sample Files (and/or any modified version) in 
//   any way you find useful, provided that you agree that 
//   Microsoft has no warranty obligations or liability for any 
//   Sample Application Files which are modified. 
//
//   If you did not get this from Microsoft Sources, then it may not be the
//   most current version.  This sample code in particular will be updated
//   and include more documentation.  
//
//   Sources are:
//   	The MM Sys File Transfer BBS: The phone number is 206 936-4082.
//      CompuServe: WINSDK forum, MDK section.
//      Anonymous FTP from ftp.uu.net vendors\microsoft\multimedia
//
//****************************************************************************
#include "globals.h"
#ifdef SUPPORT_MSADPCM
#include "buffer/buffer.h"
#include "src/src.h"

//****************************************************************************
//
// This structure contains the coefficients used by the MS ADPCM algorithm as
// stored in the wave format structure in the "fmt " chunk of the WAVE file.
//
//****************************************************************************
typedef struct
{
    short iCoef1;
    short iCoef2;
} ADPCMCoefSet;

//****************************************************************************
//
// This structure contains the definition of the wave format structure as
// stored in the "fmt " chunk of the WAVE file.  Note that this is packed
// to avoid having the compiler insert 2 bytes of padding between wNumCoef
// and aCoef.
//
//****************************************************************************
typedef __packed struct
{
    unsigned short wFormatTag;
    unsigned short nChannels;
    unsigned long nSamplesPerSec;
    unsigned long nAvgBytesPerSec;
    unsigned short nBlockAlign;
    unsigned short wBitsPerSample;
    unsigned short cbSize;
    unsigned short wSamplesPerBlock;
    unsigned short wNumCoef;
    ADPCMCoefSet aCoef[7];
} ADPCMWaveFormat;

//****************************************************************************
//
//  constants used by the Microsoft 4 Bit ADPCM algorithm
//
//****************************************************************************
#define MSADPCM_CSCALE                          8
#define MSADPCM_PSCALE                          8
#define MSADPCM_CSCALE_NUM                      (1 << MSADPCM_CSCALE)
#define MSADPCM_PSCALE_NUM                      (1 << MSADPCM_PSCALE)
#define MSADPCM_DELTA4_MIN                      16
#define MSADPCM_OUTPUT4_MAX                     7
#define MSADPCM_OUTPUT4_MIN                     -8

//****************************************************************************
//
// The maximum number of PCM samples per block which is supported by the
// decoder.
//
//****************************************************************************
#define MSADPCM_MAX_PCM_LENGTH                  2048

//****************************************************************************
//
// The coefficient pairs that should be in the wave format header for the
// Microsoft 4 Bit ADPCM algorithm.
//
//****************************************************************************
static const short psCoefficient1[] = { 256,  512,  0, 192, 240,  460,  392 };
static const short psCoefficient2[] = {   0, -256,  0,  64,   0, -208, -232 };

//****************************************************************************
//
// Fixed point Delta adaption table:
//
//     Next Delta = Delta * psP4[ this output ] / MSADPCM_PSCALE
//
//****************************************************************************
static const short psP4[] = { 230, 230, 230, 230, 307, 409, 512, 614,
                              768, 614, 512, 409, 307, 230, 230, 230 };

//****************************************************************************
//
// A structure which defines the perisitent state of the MS ADPCM encoder/
// decoder.
//
//****************************************************************************
typedef struct
{
    //
    // The file containing the encoded data.
    //
    tFile *pFile;

    //
    // The buffer containing MS ADPCM data.
    //
    unsigned char pucEncodedData[4096 + 512];

    //
    // The buffers containing PCM data.
    //
    short psLeft[(MSADPCM_MAX_PCM_LENGTH * 2) + NUMTAPS - 1];
    short psRight[(MSADPCM_MAX_PCM_LENGTH * 2) + NUMTAPS - 1];

    //
    // The buffer from which we read the PCM audio stream when encoding or to
    // which we write the PCM audio stream when decoding.
    //
    BufferState *pBuffer;

    //
    // The current offset into the encoded data buffer.
    //
    unsigned short usOffset;

    //
    // The number of valid bytes in the encoded data buffer.
    //
    unsigned short usValid;

    //
    // The offset into the file of the first byte of encoded data.
    //
    unsigned long ulDataStart;

    //
    // The length of the encoded data.
    //
    unsigned long ulLength;

    //
    // The number of bytes of encoded data remaining in the file.
    //
    unsigned long ulDataValid;

    //
    // The byte rate of the encoded file.
    //
    unsigned short usByteRate;

    //
    // The sample rate of the decoded file.
    //
    unsigned short usSampleRate;

    //
    // The number of channels of audio in the file.
    //
    unsigned char ucChannels;

    //
    // The number of bytes in each encoded block of audio.
    //
    unsigned short usBytesPerBlock;

    //
    // The number of samples in each encoded block of audio.
    //
    unsigned short usSamplesPerBlock;

    //
    // The length of the file in milliseconds.
    //
    unsigned long ulTimeLength;

    //
    // The number of samples that have been encoded/decoded.
    //
    unsigned long ulTimePos;
} tMSADPCM;

//****************************************************************************
//
// Decodes a block of MS ADPCM into 16-bit PCM.
//
//****************************************************************************
unsigned long
adpcmDecode4Bit(tMSADPCM *pMSADPCM, unsigned char *pucSrc, short *psLeft,
                short *psRight, unsigned long ulSrcLen)
{
    short   sInput;
    short   sNextInput;
    short   sFirstNibble;
    short   sDelta;
    long    lSamp;
    long    lPrediction;
    unsigned char    ucChannel;
    unsigned short   usSample;
    unsigned short   usSamplesPerBlock;
    short   psSamp1[2];
    short   psSamp2[2];
    short   psCoef1[2];
    short   psCoef2[2];
    short   psDelta[2];

    //
    //  the first thing we need to do with each block of ADPCM data is
    //  to read the header which consists of 7 bytes of data per channel.
    //  so our first check is to make sure that we have _at least_
    //  enough input data for a complete ADPCM block header--if there
    //  is not enough data for the header, then exit.
    //
    //  the header looks like this:
    //      1 byte predictor per channel
    //      2 byte delta per channel
    //      2 byte first sample per channel
    //      2 byte second sample per channel
    //
    //  this gives us (7 * pMSADPCM->ucChannels) bytes of header information.
    //  note that as long as there is _at least_ (7 * pMSADPCM->ucChannels) of
    //  header info, we will grab the two samples from the header (and if no
    //  data exists following the header we will exit in the decode
    //  loop below).
    //
    if(ulSrcLen < (pMSADPCM->ucChannels * 7))
    {
        return(0);
    }
            
    //
    //  grab and validate the predictor for each channel
    //
    for(ucChannel = 0; ucChannel < pMSADPCM->ucChannels; ucChannel++)
    {
        unsigned char bPredictor;

        bPredictor = *pucSrc++;
        if(bPredictor >= 7)
        {
            //
            //  the predictor is out of range--this is considered a
            //  fatal error with the ADPCM data, so we fail by returning
            //  zero bytes decoded
            //
            return(0);
        }

        //
        //  get the coefficients for the predictor index
        //
        psCoef1[ucChannel] = psCoefficient1[bPredictor];
        psCoef2[ucChannel] = psCoefficient2[bPredictor];
    }
        
    //
    //  get the starting delta for each channel
    //
    for(ucChannel = 0; ucChannel < pMSADPCM->ucChannels; ucChannel++)
    {
        psDelta[ucChannel] = pucSrc[0] | (pucSrc[1] << 8);
        pucSrc += 2;
    }

    //
    //  get the sample just previous to the first encoded sample per
    //  channel
    //
    for(ucChannel = 0; ucChannel < pMSADPCM->ucChannels; ucChannel++)
    {
        psSamp1[ucChannel] = pucSrc[0] | (pucSrc[1] << 8);
        pucSrc += 2;
    }

    //
    //  get the sample previous to psSamp1[x] per channel
    //
    for(ucChannel = 0; ucChannel < pMSADPCM->ucChannels; ucChannel++)
    {
        psSamp2[ucChannel] = pucSrc[0] | (pucSrc[1] << 8);
        pucSrc += 2;
    }

    //
    //  write out first 2 samples for each channel.
    //
    //  NOTE: the samples are written to the destination PCM buffer
    //  in the _reverse_ order that they are in the header block:
    //  remember that psSamp2[x] is the _previous_ sample to psSamp1[x].
    //
    *psLeft++ = psSamp2[0];
    *psLeft++ = psSamp1[0];
    if(pMSADPCM->ucChannels == 2)
    {
        *psRight++ = psSamp2[1];
        *psRight++ = psSamp1[1];
    }

    //
    //  determine the number of samples to be decoded.  this could be less than
    //  the number specified in the wave header at the end of the file.
    //
    usSamplesPerBlock = (((ulSrcLen - (pMSADPCM->ucChannels * 7)) * 2) /
                         pMSADPCM->ucChannels) + 2;
    if(usSamplesPerBlock > pMSADPCM->usSamplesPerBlock)
    {
        usSamplesPerBlock = pMSADPCM->usSamplesPerBlock;
    }

    //
    //  we now need to decode the 'data' section of the ADPCM block.
    //  this consists of packed 4 bit nibbles.
    //
    //  NOTE: we start our count for the number of data bytes to decode
    //  at 2 because we have already decoded the first 2 samples in
    //  this block.
    //
    sFirstNibble = 1;
    for(usSample = 2; usSample < usSamplesPerBlock; usSample++)
    {
        for(ucChannel = 0; ucChannel < pMSADPCM->ucChannels; ucChannel++)
        {
            if (sFirstNibble)
            {
                //
                //  grab the next two nibbles and create sign extended
                //  integers out of them:
                //
                //      sInput is the first nibble to decode
                //      sNextInput will be the next nibble decoded
                //
                sNextInput  = (short)*pucSrc++;
                sInput = (sNextInput << 24) >> 28;
                sNextInput = (sNextInput << 28) >> 28;
                sFirstNibble = 0;
            }
            else
            {
                //
                //  put the next sign extended nibble into sInput and
                //  decode it--also set sFirstNibble back to 1 so we
                //  will read another byte from the source stream on
                //  the next iteration...
                //
                sInput = sNextInput;
                sFirstNibble = 1;
            }

            //
            //  compute the next Adaptive Scale Factor (ASF) and put
            //  this value in psDelta for the current channel.
            //
            sDelta = psDelta[ucChannel];
            psDelta[ucChannel] = (short)((psP4[sInput & 15] * (long)sDelta) >>
                                         MSADPCM_PSCALE);
            if(psDelta[ucChannel] < MSADPCM_DELTA4_MIN)
            {
                psDelta[ucChannel] = MSADPCM_DELTA4_MIN;
            }

            //
            //  decode sInput (the sign extended 4 bit nibble)--there are
            //  two steps to this:
            //
            //  1.  predict the next sample using the previous two
            //      samples and the predictor coefficients:
            //
            //      Prediction = (psSamp1[channel] * psCoef1[channel] + 
            //              psSamp2[channel] * psCoef2[channel]) / 256;
            //
            //  2.  reconstruct the original PCM sample using the encoded
            //      sample (sInput), the Adaptive Scale Factor (psDelta)
            //      and the prediction value computed in step 1 above.
            //
            //      Sample = (sInput * psDelta[channel]) + Prediction;
            //
            lPrediction = (((long)psSamp1[ucChannel] *
                            (long)psCoef1[ucChannel]) +
                           ((long)psSamp2[ucChannel] *
                            (long)psCoef2[ucChannel])) >>
                          MSADPCM_CSCALE;
            lSamp = ((long)sInput * (long)sDelta) + lPrediction;

            //
            //  now we need to clamp lSamp to [-32768..32767]--this value
            //  will then be a valid 16 bit sample.
            //
            if(lSamp > 32767)
            {
                lSamp = 32767;
            }
            else if(lSamp < -32768)
            {
                lSamp = -32768;
            }
        
            //
            //  lSamp contains the decoded sInput sample--now write it
            //  out to the destination buffer
            //
            if(ucChannel == 0)
            {
                *psLeft++ = (short)lSamp;
            }
            else
            {
                *psRight++ = (short)lSamp;
            }
                
            //
            //  ripple our previous samples down making the new psSamp1
            //  equal to the sample we just decoded
            //
            psSamp2[ucChannel] = psSamp1[ucChannel];
            psSamp1[ucChannel] = (short)lSamp;
        }
    }

    //
    //  if this is a short block, then fill the remainder of the output block
    //  with silence.
    //
    if(usSamplesPerBlock < pMSADPCM->usSamplesPerBlock)
    {
        memset(psLeft, 0,
               (pMSADPCM->usSamplesPerBlock - usSamplesPerBlock) * 2);
        if(pMSADPCM->ucChannels == 2)
        {
            memset(psRight, 0,
                   (pMSADPCM->usSamplesPerBlock - usSamplesPerBlock) * 2);
        }
    }

    //
    //  we're done decoding the input data.  return the number of samples
    //  decoded.
    //
    return(usSamplesPerBlock);
}

//****************************************************************************
//
// Computes the first delta value for a MS ADPCM block.
//
//****************************************************************************
short
adpcmEncode4Bit_FirstDelta(short iCoef1, short iCoef2, short iP5, short iP4,
                           short iP3, short iP2, short iP1)
{
    long    lTotal;
    short   iRtn;
    long    lTemp;

    //
    //  use average of 3 predictions
    //
    lTemp  = (((long)iP5 * iCoef2) + ((long)iP4 * iCoef1)) >> MSADPCM_CSCALE;
    lTotal = (lTemp > iP3) ? (lTemp - iP3) : (iP3 - lTemp);

    lTemp   = (((long)iP4 * iCoef2) + ((long)iP3 * iCoef1)) >> MSADPCM_CSCALE;
    lTotal += (lTemp > iP2) ? (lTemp - iP2) : (iP2 - lTemp);

    lTemp   = (((long)iP3 * iCoef2) + ((long)iP2 * iCoef1)) >> MSADPCM_CSCALE;
    lTotal += (lTemp > iP1) ? (lTemp - iP1) : (iP1 - lTemp);
    
    //
    //  optimal iDelta is 1/4 of prediction error
    //
    iRtn = (short)(lTotal / 12);
    if(iRtn < MSADPCM_DELTA4_MIN)
    {
        iRtn = MSADPCM_DELTA4_MIN;
    }

    return(iRtn);
}

//****************************************************************************
//
// Encodes a block of 16-bit PCM into MS ADPCM.
//
//****************************************************************************
unsigned long
adpcmEncode4Bit(tMSADPCM *pMSADPCM, short *psLeft, short *psRight,
                unsigned char *pucDst)
{
    short *ppsSamples[2];

    unsigned char pucBestPredictor[2];
    unsigned long ppulTotalError[7][2];

    short   ppsFirstDelta[7][2];
    short   psDelta[2];
    short   psSamp1[2];
    short   psSamp2[2];

    short   sCoef1;
    short   sCoef2;
    short   sDelta;
    short   sSample;
    short   sOutput;

    long    lSamp;

    unsigned long ulTemp;
    unsigned long ulTotalConverted;

    long    lError;
    long    lPrediction;

    unsigned char ucNextWrite;
    unsigned char ucFirstNibble;
    unsigned short usSample;
    unsigned char ucChannel;
    unsigned char ucPredictor;

    //
    //  save the pointers to the left and right channel data
    //
    ppsSamples[0] = psLeft;
    ppsSamples[1] = psRight;

    //
    //  initialize the number of output bytes to zero
    //
    ulTotalConverted = 0;

    //
    //  find the optimal predictor for each channel: to do this, we
    //  must step through and encode using each coefficient set (one
    //  at a time) and determine which one has the least error from
    //  the original data. the one with the least error is then used
    //  for the final encode (the 8th pass done below).
    //
    //  NOTE: keeping the encoded data of the one that has the least
    //  error at all times is an obvious optimization that should be
    //  done. in this way, we only need to do 7 passes instead of 8.
    //
    for(ucPredictor = 0; ucPredictor < 7; ucPredictor++)
    {
        //
        //  copy the coefficient pair for the current coefficient set
        //  we are using into more convenient/cheaper variables
        //
        sCoef1 = psCoefficient1[ucPredictor];
        sCoef2 = psCoefficient2[ucPredictor];

        //
        //  now choose the first iDelta and setup the first two samples
        //
        for(ucChannel = 0; ucChannel < pMSADPCM->ucChannels; ucChannel++)
        {
            //
            //  reset total error calculation for new block
            //
            ppulTotalError[ucPredictor][ucChannel] = 0;

            //
            //  first 2 samples will come from real data--compute
            //  starting from there
            //
            psSamp1[ucChannel] = ppsSamples[ucChannel][1];
            psSamp2[ucChannel] = ppsSamples[ucChannel][0];
                        
            //
            //  calculate initial iDelta
            //
            sDelta = adpcmEncode4Bit_FirstDelta(sCoef1, sCoef2,
                                                ppsSamples[ucChannel][0],
                                                ppsSamples[ucChannel][1],
                                                ppsSamples[ucChannel][2],
                                                ppsSamples[ucChannel][3],
                                                ppsSamples[ucChannel][4]);
            psDelta[ucChannel] = sDelta;
            ppsFirstDelta[ucPredictor][ucChannel] = sDelta;
        }

        //
        //  now encode the rest of the PCM data in this block--note
        //  we start 2 samples ahead because the first two samples are
        //  simply copied into the ADPCM block header...
        //
        for(usSample = 2; usSample < pMSADPCM->usSamplesPerBlock; usSample++)
        {
            //
            //  each channel gets encoded independently... obviously.
            //
            for(ucChannel = 0; ucChannel < pMSADPCM->ucChannels; ucChannel++)
            {
                //
                //  yes, copy into cheaper variables because we access
                //  them a lot
                //
                sDelta = psDelta[ucChannel];

                //
                //  calculate the prediction based on the previous two
                //  samples
                //
                lPrediction = (((long)psSamp1[ucChannel] * sCoef1) +
                               ((long)psSamp2[ucChannel] * sCoef2)) >>
                              MSADPCM_CSCALE;

                //
                //  grab the sample (for the current channel) to encode
                //  from the source
                //
                sSample = ppsSamples[ucChannel][usSample];

                //
                //  encode it
                //
                lError = (long)sSample - lPrediction;
                sOutput = (short)(lError / sDelta);
                if(sOutput > MSADPCM_OUTPUT4_MAX)
                {
                    sOutput = MSADPCM_OUTPUT4_MAX;
                }
                else if(sOutput < MSADPCM_OUTPUT4_MIN)
                {
                    sOutput = MSADPCM_OUTPUT4_MIN;
                }

                lSamp = lPrediction + ((long)sDelta * sOutput);

                if(lSamp > 32767)
                {
                    lSamp = 32767;
                }
                else if(lSamp < -32768)
                {
                    lSamp = -32768;
                }

                //
                //  compute the next iDelta
                //
                sDelta = (short)((psP4[sOutput & 15] * (long)sDelta) >>
                                 MSADPCM_PSCALE);
                if(sDelta < MSADPCM_DELTA4_MIN)
                {
                    sDelta = MSADPCM_DELTA4_MIN;
                }
        
                //
                //  save updated values for this channel back into the
                //  original arrays...
                //
                psDelta[ucChannel] = sDelta;
                psSamp2[ucChannel] = psSamp1[ucChannel];
                psSamp1[ucChannel] = (short)lSamp;

                //
                //  keep a running status on the error for the current
                //  coefficient pair for this channel
                //
                lError = lSamp - sSample;
                ppulTotalError[ucPredictor][ucChannel] +=
                    (lError * lError) >> 7;
            }
        }
    }


    //
    //  WHEW! we have now made 7 passes over the data and calculated
    //  the error for each--so it's time to find the one that produced
    //  the lowest error and use that predictor (this is for each
    //  channel of course)
    //
    for(ucChannel = 0; ucChannel < pMSADPCM->ucChannels; ucChannel++)
    {
        pucBestPredictor[ucChannel] = 0;
        ulTemp = ppulTotalError[0][ucChannel];
        for(ucPredictor = 1; ucPredictor < 7; ucPredictor++)
        {
            if(ppulTotalError[ucPredictor][ucChannel] < ulTemp)
            {
                pucBestPredictor[ucChannel] = ucPredictor;
                ulTemp = ppulTotalError[ucPredictor][ucChannel];
            }
        }
    }    

    //
    //  grab first iDelta from our precomputed first deltas that we
    //  calculated above
    //
    for(ucChannel = 0; ucChannel < pMSADPCM->ucChannels; ucChannel++)
    {
        ucPredictor = pucBestPredictor[ucChannel];
        psDelta[ucChannel] = ppsFirstDelta[ucPredictor][ucChannel];
    }

    //
    //  get the first two samples from the source data (so we can write
    //  them into the ADPCM block header and use them in our prediction
    //  calculation when encoding the rest of the data).
    //
    for(ucChannel = 0; ucChannel < pMSADPCM->ucChannels; ucChannel++)
    {
        psSamp2[ucChannel] = ppsSamples[ucChannel][0];
        psSamp1[ucChannel] = ppsSamples[ucChannel][1];
    }

    //
    //  write the block header for the encoded data
    //
    //  the block header is composed of the following data:
    //      1 byte predictor per channel
    //      2 byte delta per channel
    //      2 byte first sample per channel
    //      2 byte second sample per channel
    //
    //  this gives us (7 * nChannels) bytes of header information
    //
    //  so first write the 1 byte predictor for each channel into the
    //  destination buffer
    //
    for(ucChannel = 0; ucChannel < pMSADPCM->ucChannels; ucChannel++)
    {
        *pucDst++ = pucBestPredictor[ucChannel];
    }

    //
    //  now write the 2 byte delta per channel...
    //
    for(ucChannel = 0; ucChannel < pMSADPCM->ucChannels; ucChannel++)
    {
        *pucDst++ = psDelta[ucChannel] & 255;
        *pucDst++ = psDelta[ucChannel] >> 8;
    }

    //
    //  finally, write the first two samples (2 bytes each per channel)
    //
    for(ucChannel = 0; ucChannel < pMSADPCM->ucChannels; ucChannel++)
    {
        *pucDst++ = psSamp1[ucChannel] & 255;
        *pucDst++ = psSamp1[ucChannel] >> 8;
    }
    for(ucChannel = 0; ucChannel < pMSADPCM->ucChannels; ucChannel++)
    {
        *pucDst++ = psSamp2[ucChannel] & 255;
        *pucDst++ = psSamp2[ucChannel] >> 8;
    }

    //
    //  the number of bytes that we have written to the destination
    //  buffer is (7 * nChannels)--so add this to our total number of
    //  bytes written to the destination.. for our return value.
    //
    ulTotalConverted += pMSADPCM->ucChannels * 7;

    //
    //  we have written the header for this block--now write the data
    //  chunk (which consists of a bunch of encoded nibbles). note that
    //  we start our count at 2 because we already wrote the first
    //  two samples into the destination buffer as part of the header
    //
    ucFirstNibble = 1;
    for(usSample = 2; usSample < pMSADPCM->usSamplesPerBlock; usSample++)
    {
        //
        //  each channel gets encoded independently... obviously.
        //
        for(ucChannel = 0; ucChannel < pMSADPCM->ucChannels; ucChannel++)
        {
            //
            //  use our chosen best predictor and grab the coefficient
            //  pair to use for this channel...
            //
            ucPredictor = pucBestPredictor[ucChannel];
            sCoef1 = psCoefficient1[ucPredictor];
            sCoef2 = psCoefficient2[ucPredictor];

            //
            //  copy into cheaper variables because we access them a lot
            //
            sDelta = psDelta[ucChannel];

            //
            //  calculate the prediction based on the previous two samples
            //
            lPrediction = (((long)psSamp1[ucChannel] * sCoef1) +
                           ((long)psSamp2[ucChannel] * sCoef2)) >>
                          MSADPCM_CSCALE;

            //
            //  grab the sample to encode
            //
            sSample = ppsSamples[ucChannel][usSample];

            //
            //  encode the sample
            //
            lError = (long)sSample - lPrediction;
            sOutput = (short)(lError / sDelta);
            if(sOutput > MSADPCM_OUTPUT4_MAX)
            {
                sOutput = MSADPCM_OUTPUT4_MAX;
            }
            else if(sOutput < MSADPCM_OUTPUT4_MIN)
            {
                sOutput = MSADPCM_OUTPUT4_MIN;
            }

            lSamp = lPrediction + ((long)sDelta * sOutput);

            if(lSamp > 32767)
            {
                lSamp = 32767;
            }
            else if(lSamp < -32768)
            {
                lSamp = -32768;
            }

            //
            //  compute the next iDelta
            //
            sDelta = (short)((psP4[sOutput & 15] * (long)sDelta) >>
                             MSADPCM_PSCALE); 
            if(sDelta < MSADPCM_DELTA4_MIN)
            {
                sDelta = MSADPCM_DELTA4_MIN;
            }

            //
            //  save updated values for this channel back into the
            //  original arrays...
            //
            psDelta[ucChannel] = sDelta;
            psSamp2[ucChannel] = psSamp1[ucChannel];
            psSamp1[ucChannel] = (short)lSamp;

            //
            //  we have another nibble of encoded data--either combine
            //  this with the previous nibble and write out a full 
            //  byte, or save this nibble for the next nibble to be
            //  combined into a full byte and written to the destination
            //  buffer... uhg!
            //
            if(ucFirstNibble)
            {
                ucNextWrite = (sOutput & 15) << 4;
                ucFirstNibble = 0;
            }
            else
            {
                *pucDst++ = ucNextWrite | (sOutput & 15);
                ulTotalConverted++;
                ucFirstNibble = 1;
            }
        }
    }

    //
    //  return the number of bytes in the encoded ADPCM block
    //
    return (ulTotalConverted);
}

//****************************************************************************
//
// MSADPCMSetupDecoder prepares to decode a file using the MS ADPCM decoder.
//
//****************************************************************************
unsigned long
MSADPCMSetupDecoder(tMSADPCM *pMSADPCM)
{
    ADPCMWaveFormat sWaveFormat;
    unsigned long ulLength;

    //
    // Seek to the beginning of the file.
    //
    FSSeek(pMSADPCM->pFile, 0);

    //
    // Read the first portion of the file.
    //
    pMSADPCM->usValid = FSRead(pMSADPCM->pFile, pMSADPCM->pucEncodedData,
                               4096);

    //
    // Make sure that this is a RIFF WAVE file.
    //
    if((pMSADPCM->usValid < 12) ||
       (pMSADPCM->pucEncodedData[0] != 'R') ||
       (pMSADPCM->pucEncodedData[1] != 'I') ||
       (pMSADPCM->pucEncodedData[2] != 'F') ||
       (pMSADPCM->pucEncodedData[3] != 'F') ||
       (pMSADPCM->pucEncodedData[8] != 'W') ||
       (pMSADPCM->pucEncodedData[9] != 'A') ||
       (pMSADPCM->pucEncodedData[10] != 'V') ||
       (pMSADPCM->pucEncodedData[11] != 'E'))
    {
        return(0);
    }

    //
    // Set the sample rate to zero.  This will be set correctly if we find a
    // "fmt " chunk; if we process all the chunks in the file and the sample
    // rate is still zero, it indicates that we did not find the "fmt " chunk.
    //
    pMSADPCM->usSampleRate = 0;

    //
    // The first chunk is at offset 12.
    //
    pMSADPCM->usOffset = 12;

    //
    // Look for chunks in the file.  We need the "fmt " chunk and the "data"
    // chunk.  We ignore the "fact" chunk since the Microsoft's Sound Recorder
    // on the PC (for example) does not put the correct value into this chunk
    // (it over-states the number of samples in the file).  We instead compute
    // the correct value from the size of the data chunk.
    //
    while(1)
    {
        //
        // Make sure that there is enough data in the data buffer.
        //
        if((pMSADPCM->usOffset + 8) > pMSADPCM->usValid)
        {
            //
            // Copy the tail of the buffer to the beginning.
            //
            memcpy(pMSADPCM->pucEncodedData,
                   pMSADPCM->pucEncodedData + (pMSADPCM->usOffset & ~3),
                   pMSADPCM->usValid - (pMSADPCM->usOffset & ~3));

            //
            // Update the number of valid bytes in the buffer.
            //
            pMSADPCM->usValid -= (pMSADPCM->usOffset & ~3);

            //
            // Read the next 3.5K of data into the buffer.
            //
            pMSADPCM->usValid += FSRead(pMSADPCM->pFile,
                                        pMSADPCM->pucEncodedData +
                                        pMSADPCM->usValid, 3584);

            //
            // Set the current offset to the beginning of the buffer.
            //
            pMSADPCM->usOffset &= 3;
        }

        //
        // If there is no more data in the file, then return a failure since we
        // could not find the "data" chunk.
        //
        if(pMSADPCM->usValid < 8)
        {
            return(0);
        }

        //
        // Is this the "fmt " chunk?
        //
        if((pMSADPCM->pucEncodedData[pMSADPCM->usOffset] == 'f') &&
           (pMSADPCM->pucEncodedData[pMSADPCM->usOffset + 1] == 'm') &&
           (pMSADPCM->pucEncodedData[pMSADPCM->usOffset + 2] == 't') &&
           (pMSADPCM->pucEncodedData[pMSADPCM->usOffset + 3] == ' '))
        {
            //
            // Get the length of the "fmt " chunk.
            //
            ulLength =
                (pMSADPCM->pucEncodedData[pMSADPCM->usOffset + 7] << 24) |
                (pMSADPCM->pucEncodedData[pMSADPCM->usOffset + 6] << 16) |
                (pMSADPCM->pucEncodedData[pMSADPCM->usOffset + 5] << 8) |
                pMSADPCM->pucEncodedData[pMSADPCM->usOffset + 4];

            //
            // If the length of the "fmt " chunk is not the same as the size
            // of the MS ADPCM wave format sturcture, then we can not decode
            // this file.
            //
            if(ulLength != sizeof(ADPCMWaveFormat))
            {
                return(0);
            }

            //
            // Make sure that the entire "fmt " chunk is in the data buffer.
            //
            if((pMSADPCM->usOffset + ulLength + 8) > pMSADPCM->usValid)
            {
                //
                // Copy the tail of the buffer to the beginning.
                //
                memcpy(pMSADPCM->pucEncodedData,
                       pMSADPCM->pucEncodedData + (pMSADPCM->usOffset & ~3),
                       pMSADPCM->usValid - (pMSADPCM->usOffset & ~3));

                //
                // Update the number of valid bytes in the buffer.
                //
                pMSADPCM->usValid -= (pMSADPCM->usOffset & ~3);

                //
                // Read the next 3.5K of data into the buffer.
                //
                pMSADPCM->usValid += FSRead(pMSADPCM->pFile,
                                            pMSADPCM->pucEncodedData +
                                            pMSADPCM->usValid, 3584);

                //
                // Set the current offset to the beginning of the buffer.
                //
                pMSADPCM->usOffset &= 3;
            }

            //
            // Copy the wave format structure from the "fmt " chunk into a
            // local.  We do this instead of grabbing the values directly from
            // the data buffer since the "fmt " chunk can occur with any
            // alignment, making it a lot easier to simply copy the structure.
            //
            memcpy((void *)&sWaveFormat,
                   pMSADPCM->pucEncodedData + pMSADPCM->usOffset + 8,
                   sizeof(ADPCMWaveFormat));

            //
            // Perform some sanity checks on the wave format structure.
            //
            if((sWaveFormat.wFormatTag != 2) ||
               (sWaveFormat.nChannels > 2) ||
               (sWaveFormat.nSamplesPerSec > 48000) ||
               (sWaveFormat.nBlockAlign > 4096) ||
               (sWaveFormat.wBitsPerSample != 4) ||
               (sWaveFormat.cbSize != 32) ||
               (sWaveFormat.wSamplesPerBlock > MSADPCM_MAX_PCM_LENGTH) ||
               (sWaveFormat.wNumCoef != 7))
            {
                //
                // The wave format does not pass the sanity checks, so we can
                // not decode this file.
                //
                return(0);
            }

            //
            // Make sure that the coefficients match the MS ADPCM coefficients.
            //
            for(ulLength = 0; ulLength < 7; ulLength++)
            {
                if((sWaveFormat.aCoef[ulLength].iCoef1 !=
                    psCoefficient1[ulLength]) ||
                   (sWaveFormat.aCoef[ulLength].iCoef2 !=
                    psCoefficient2[ulLength]))
                {
                    //
                    // The coefficients do not match the MS ADPCM coefficients,
                    // so we can not decode this file.
                    //
                    return(0);
                }
            }

            //
            // Save information about this file which we will need.
            //
            pMSADPCM->usByteRate = sWaveFormat.nAvgBytesPerSec;
            pMSADPCM->usSampleRate = sWaveFormat.nSamplesPerSec;
            pMSADPCM->ucChannels = sWaveFormat.nChannels;
            pMSADPCM->usBytesPerBlock = sWaveFormat.nBlockAlign;
            pMSADPCM->usSamplesPerBlock = sWaveFormat.wSamplesPerBlock;
        }

        //
        // Is this the "data" chunk?
        //
        else if((pMSADPCM->pucEncodedData[pMSADPCM->usOffset] == 'd') &&
                (pMSADPCM->pucEncodedData[pMSADPCM->usOffset + 1] == 'a') &&
                (pMSADPCM->pucEncodedData[pMSADPCM->usOffset + 2] == 't') &&
                (pMSADPCM->pucEncodedData[pMSADPCM->usOffset + 3] == 'a'))
        {
            //
            // Get the starting position of the "data" chunk.
            //
            pMSADPCM->ulDataStart = FSTell(pMSADPCM->pFile) -
                                    pMSADPCM->usValid + pMSADPCM->usOffset + 8;

            //
            // Get the length of the "data" chunk.
            //
            pMSADPCM->ulLength =
                (pMSADPCM->pucEncodedData[pMSADPCM->usOffset + 7] << 24) |
                (pMSADPCM->pucEncodedData[pMSADPCM->usOffset + 6] << 16) |
                (pMSADPCM->pucEncodedData[pMSADPCM->usOffset + 5] << 8) |
                pMSADPCM->pucEncodedData[pMSADPCM->usOffset + 4];

            //
            // Advance to the current buffer offset to the first byte of the
            // "data" chunk.
            //
            pMSADPCM->usOffset += 8;

            //
            // We've found the "data" chunk, so stop searching for chunks.
            //
            break;
        }

        //
        // Skip to the next chunk.  Get the length of this chunk.
        //
        ulLength =
            (pMSADPCM->pucEncodedData[pMSADPCM->usOffset + 7] << 24) |
            (pMSADPCM->pucEncodedData[pMSADPCM->usOffset + 6] << 16) |
            (pMSADPCM->pucEncodedData[pMSADPCM->usOffset + 5] << 8) |
            pMSADPCM->pucEncodedData[pMSADPCM->usOffset + 4] + 8;

        //
        // Does the next chunk start in the currently read portion of the file?
        //
        if((pMSADPCM->usOffset + ulLength) < pMSADPCM->usValid)
        {
            //
            // Skip to the next chunk, which is already in memory.
            //
            pMSADPCM->usOffset += ulLength;
        }

        //
        // Does the next chunk start in the next 4K of the file?
        //
        else if((pMSADPCM->usOffset + ulLength) < (pMSADPCM->usValid + 4096))
        {
            //
            // Increment the buffer offset by the size of the chunk and
            // subtract the size of the data currently in the buffer.
            //
            pMSADPCM->usOffset += ulLength - pMSADPCM->usValid;

            //
            // Read the next 4K of the file.
            //
            pMSADPCM->usValid = FSRead(pMSADPCM->pFile,
                                       pMSADPCM->pucEncodedData, 4096);
        }

        //
        // Otherwise, seek to the portion of the file that contains the next
        // chunk.
        //
        else
        {
            //
            // Determine the absolute offset into the file of the next chunk.
            //
            ulLength += FSTell(pMSADPCM->pFile) - pMSADPCM->usValid +
                        pMSADPCM->usOffset;

            //
            // Seek to the new position in the file.
            //
            FSSeek(pMSADPCM->pFile, ulLength & ~511);

            //
            // Read the next 4K from the file.
            //
            pMSADPCM->usValid = FSRead(pMSADPCM->pFile,
                                       pMSADPCM->pucEncodedData, 4096);

            //
            // Go the the beginning of the next chunk in the memory buffer.
            //
            pMSADPCM->usOffset = ulLength & 511;
        }
    }

    //
    // If the sample rate is still zero, then we did not find a "fmt " chunk.
    // Therefore, we can not decode this file.
    //
    if(pMSADPCM->usSampleRate == 0)
    {
        return(0);
    }

    //
    // Get the time length of the file.
    //
    pMSADPCM->ulTimeLength = ((pMSADPCM->ulLength / pMSADPCM->usByteRate) *
                              1000) +
                             (((pMSADPCM->ulLength % pMSADPCM->usByteRate) *
                               1000) / pMSADPCM->usByteRate);

    //
    // The position is zero.
    //
    pMSADPCM->ulTimePos = 0;

    //
    // Initially, all the data in the file is valid.
    //
    pMSADPCM->ulDataValid = pMSADPCM->ulLength + pMSADPCM->usOffset;

    //
    // There is no output buffer.
    //
    pMSADPCM->pBuffer = 0;

    //
    // Success.
    //
    return(1);
}

//****************************************************************************
//
// MSADPCMSetupEncoder prepares to encode a file using the MS ADPCM encoder.
//
//****************************************************************************
unsigned long
MSADPCMSetupEncoder(tMSADPCM *pMSADPCM)
{
    ADPCMWaveFormat sWaveFormat;
    unsigned long ulIdx;

    //
    // We encode files at 8kHz, mono.  So, the following parameters are fixed.
    //
    pMSADPCM->usByteRate = 4096;
    pMSADPCM->usSampleRate = 8000;
    pMSADPCM->ucChannels = 1;
    pMSADPCM->usBytesPerBlock = 256;
    pMSADPCM->usSamplesPerBlock = 500;

    //
    // Build the RIFF header in the encoded data buffer.
    //
    pMSADPCM->pucEncodedData[0] = 'R';
    pMSADPCM->pucEncodedData[1] = 'I';
    pMSADPCM->pucEncodedData[2] = 'F';
    pMSADPCM->pucEncodedData[3] = 'F';
    pMSADPCM->pucEncodedData[4] = 0;
    pMSADPCM->pucEncodedData[5] = 0;
    pMSADPCM->pucEncodedData[6] = 0;
    pMSADPCM->pucEncodedData[7] = 0;
    pMSADPCM->pucEncodedData[8] = 'W';
    pMSADPCM->pucEncodedData[9] = 'A';
    pMSADPCM->pucEncodedData[10] = 'V';
    pMSADPCM->pucEncodedData[11] = 'E';

    //
    // Fill in the wave format structure.
    //
    sWaveFormat.wFormatTag = 2;
    sWaveFormat.nChannels = pMSADPCM->ucChannels;
    sWaveFormat.nSamplesPerSec = pMSADPCM->usSampleRate;
    sWaveFormat.nAvgBytesPerSec = pMSADPCM->usByteRate;
    sWaveFormat.nBlockAlign = pMSADPCM->usBytesPerBlock;
    sWaveFormat.wBitsPerSample = 4;
    sWaveFormat.cbSize = 32;
    sWaveFormat.wSamplesPerBlock = pMSADPCM->usSamplesPerBlock;
    sWaveFormat.wNumCoef = 7;
    for(ulIdx = 0; ulIdx < 7; ulIdx++)
    {
        sWaveFormat.aCoef[ulIdx].iCoef1 = psCoefficient1[ulIdx];
        sWaveFormat.aCoef[ulIdx].iCoef2 = psCoefficient2[ulIdx];
    }

    //
    // Add the "fmt " chunk in the encoded data buffer.
    //
    pMSADPCM->pucEncodedData[12] = 'f';
    pMSADPCM->pucEncodedData[13] = 'm';
    pMSADPCM->pucEncodedData[14] = 't';
    pMSADPCM->pucEncodedData[15] = ' ';
    pMSADPCM->pucEncodedData[16] = 50;
    pMSADPCM->pucEncodedData[17] = 0;
    pMSADPCM->pucEncodedData[18] = 0;
    pMSADPCM->pucEncodedData[19] = 0;
    memcpy(pMSADPCM->pucEncodedData + 20, (void *)&sWaveFormat, 50);

    //
    // Add the "fact" chunk in the encoded data buffer.
    //
    pMSADPCM->pucEncodedData[70] = 'f';
    pMSADPCM->pucEncodedData[71] = 'a';
    pMSADPCM->pucEncodedData[72] = 'c';
    pMSADPCM->pucEncodedData[73] = 't';
    pMSADPCM->pucEncodedData[74] = 4;
    pMSADPCM->pucEncodedData[75] = 0;
    pMSADPCM->pucEncodedData[76] = 0;
    pMSADPCM->pucEncodedData[77] = 0;
    pMSADPCM->pucEncodedData[78] = 0;
    pMSADPCM->pucEncodedData[79] = 0;
    pMSADPCM->pucEncodedData[80] = 0;
    pMSADPCM->pucEncodedData[81] = 0;

    //
    // Add the "data" chunk in the encoded data buffer.
    //
    pMSADPCM->pucEncodedData[82] = 'd';
    pMSADPCM->pucEncodedData[83] = 'a';
    pMSADPCM->pucEncodedData[84] = 't';
    pMSADPCM->pucEncodedData[85] = 'a';
    pMSADPCM->pucEncodedData[86] = 0;
    pMSADPCM->pucEncodedData[87] = 0;
    pMSADPCM->pucEncodedData[88] = 0;
    pMSADPCM->pucEncodedData[89] = 0;

    //
    // There are 90 bytes of valid data in the encoded data buffer.
    //
    pMSADPCM->usValid = 90;

    //
    // The initial length of the encoded data is 0.
    //
    pMSADPCM->ulLength = 0;
    pMSADPCM->ulTimePos = 0;
    pMSADPCM->ulDataValid = 0;

    //
    // There is no input buffer.
    //
    pMSADPCM->pBuffer = 0;

    //
    // Success.
    //
    return(1);
}

//****************************************************************************
//
// The codec plug-in entry point for the MS ADPCM encoder/decoder.
//
//****************************************************************************
unsigned long
MSADPCMIoctl(unsigned long ulIoctl, unsigned long ulParam1,
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
            *ppcName = "APCMMicrosoft ADPCM";

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
            // There is no way to store the artist name in a MS ADPCM file.
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
            // There is no way to store the song title in a MS ADPCM file.
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
            tMSADPCM *pMSADPCM;

            //
            // The first parameter is a pointer to the MS ADPCM persistent
            // data.
            //
            pMSADPCM = (tMSADPCM *)ulParam1;

            //
            // The second parameter is a pointer for the bitrate.
            //
            pulBitRate = (unsigned long *)ulParam2;

            //
            // Return the bitrate of the file.
            //
            *pulBitRate = pMSADPCM->usByteRate * 8;

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
            tMSADPCM *pMSADPCM;

            //
            // The first parameter is a pointer to the MS ADPCM persistent
            // data.
            //
            pMSADPCM = (tMSADPCM *)ulParam1;

            //
            // The second parameter is a pointer for the sample rate.
            //
            pulSampleRate = (unsigned long *)ulParam2;

            //
            // Return the sample rate of the file.
            //
            *pulSampleRate = pMSADPCM->usSampleRate;

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
            tMSADPCM *pMSADPCM;

            //
            // The first parameter is a pointer to the MS ADPCM persistent
            // data.
            //
            pMSADPCM = (tMSADPCM *)ulParam1;

            //
            // The second parameter is a pointer for the number of channels.
            //
            pulChannels = (unsigned long *)ulParam2;

            //
            // Return the number of channels in the file.
            //
            *pulChannels = pMSADPCM->ucChannels;

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
            tMSADPCM *pMSADPCM;

            //
            // The first parameter is a pointer to the MS ADPCM persistent
            // data.
            //
            pMSADPCM = (tMSADPCM *)ulParam1;

            //
            // The second parameter is a pointer for the number of
            // milliseconds.
            //
            pulLength = (unsigned long *)ulParam2;

            //
            // Return the length of the file.
            //
            *pulLength = pMSADPCM->ulTimeLength;

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
            tMSADPCM *pMSADPCM;

            //
            // The first parameter is a pointer to the MS ADPCM persistent
            // data.
            //
            pMSADPCM = (tMSADPCM *)ulParam1;

            //
            // The second parameter is a pointer for the number of seconds.
            //
            pulTime = (unsigned long *)ulParam2;

            //
            // Determine the time based on the sample rate.
            //
            *pulTime = ((pMSADPCM->ulTimePos / pMSADPCM->usSampleRate) *
                        1000) +
                       (((pMSADPCM->ulTimePos % pMSADPCM->usSampleRate) *
                         1000) / pMSADPCM->usSampleRate);

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
            tMSADPCM *pMSADPCM;

            //
            // The first parameter is a pointer to the MS ADPCM persistent
            // state.
            //
            pMSADPCM = (tMSADPCM *)ulParam1;

            //
            // Make sure that we have enough memory for this decoder.
            //
            if((((unsigned long)pMSADPCM + sizeof(tMSADPCM)) & 0xFFFF) >
               0x9600)
            {
                return(0);
            }

            //
            // Save the pointer to the file structure.
            //
            pMSADPCM->pFile = (tFile *)ulParam4;

            //
            // See if we are encoding or decoding a file.
            //
            if(ulParam3 & CODEC_OPEN_ENCODE)
            {
                //
                // Setup the encoder.
                //
                return(MSADPCMSetupEncoder(pMSADPCM));
            }
            else
            {
                //
                // Setup the decoder.
                //
                return(MSADPCMSetupDecoder(pMSADPCM));
            }
        }

        //
        // Get the capture buffer for the encoder.
        //
        case IOCTL_CODEC_GETCAPTUREBUFFER:
        {
            tMSADPCM *pMSADPCM;
            short **ppsBuffer;
            long *plLength;

            //
            // The first parameter is a pointer to the MS ADPCM persistent
            // state.
            //
            pMSADPCM = (tMSADPCM *)ulParam1;

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
            *ppsBuffer = pMSADPCM->psRight;
            *plLength = MSADPCM_MAX_PCM_LENGTH * 2;

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
            tMSADPCM *pMSADPCM;

            //
            // The first parameter is a pointer to the MS ADPCM persistent
            // state.
            //
            pMSADPCM = (tMSADPCM *)ulParam1;

            //
            // The second parameter is a pointer to the buffer object.
            //
            pMSADPCM->pBuffer = (BufferState *)ulParam2;

            //
            // Provide the buffer object with our data buffers.
            //
            if(pMSADPCM->ucChannels == 2)
            {
                BufferSetBuffer(pMSADPCM->pBuffer, pMSADPCM->psLeft,
                                pMSADPCM->psRight,
                                pMSADPCM->usSamplesPerBlock +
                                pMSADPCM->usSamplesPerBlock);
            }
            else
            {
                BufferSetBuffer(pMSADPCM->pBuffer, pMSADPCM->psLeft,
                                pMSADPCM->psLeft,
                                pMSADPCM->usSamplesPerBlock +
                                pMSADPCM->usSamplesPerBlock);
            }

            //
            // Success.
            //
            return(1);
        }

        //
        // Decode a frame of data.
        //
        case IOCTL_CODEC_DECODE:
        {
            tMSADPCM *pMSADPCM;
            short *psLeft, *psRight;
            long lLength, lBlockSize;

            //
            // The first parameter is a pointer to the MS ADPCM persistent
            // state.
            //
            pMSADPCM = (tMSADPCM *)ulParam1;

            //
            // See if there is enough data in the encoded data buffer.
            //
            if(((pMSADPCM->usOffset + pMSADPCM->usBytesPerBlock) >
                pMSADPCM->usValid) &&
               (pMSADPCM->usValid < pMSADPCM->ulDataValid))
            {
                //
                // Copy the tail of the buffer to the beginning.
                //
                memcpy(pMSADPCM->pucEncodedData,
                       pMSADPCM->pucEncodedData + (pMSADPCM->usOffset & ~3),
                       pMSADPCM->usValid - (pMSADPCM->usOffset & ~3));

                //
                // Update the number of valid bytes in the buffer.
                //
                pMSADPCM->usValid -= (pMSADPCM->usOffset & ~3);

                //
                // Update the number of valid byte remaining in the file.
                //
                pMSADPCM->ulDataValid -= (pMSADPCM->usOffset & ~3);

                //
                // Read more data into the buffer.
                //
                pMSADPCM->usValid += FSRead(pMSADPCM->pFile,
                                            pMSADPCM->pucEncodedData +
                                            pMSADPCM->usValid,
                                            (4096 - pMSADPCM->usValid) & ~511);

                //
                // Make sure that the number of valid byte in the buffer does
                // not exceed the number of valid bytes in the "data" chunk.
                //
                if(pMSADPCM->usValid > pMSADPCM->ulDataValid)
                {
                    pMSADPCM->usValid = pMSADPCM->ulDataValid;
                }

                //
                // Set the current offset to the beginning of the buffer.
                //
                pMSADPCM->usOffset &= 3;
            }

            //
            // Compute the size of the next encoded MS ADPCM block.
            //
            if((pMSADPCM->usValid - pMSADPCM->usOffset) >
               pMSADPCM->usBytesPerBlock)
            {
                //
                // There is a full block in the buffer.
                //
                lBlockSize = pMSADPCM->usBytesPerBlock;
            }
            else
            {
                //
                // There is a partial block in the buffer, which contains the
                // end of the file.
                //
                lBlockSize = pMSADPCM->usValid - pMSADPCM->usOffset;
            }

            //
            // Wait until there is space in the output buffer.  This tells us
            // when it is OK to write more data into the output buffer.
            //
            while(BufferSpaceAvailable(pMSADPCM->pBuffer) <
                  pMSADPCM->usSamplesPerBlock)
            {
                //
                // Put the EP7209 into IDLE mode until the next interrupt
                // occurs.
                //
                Halt();
            }

            //
            // Get a pointer to the available space in the output buffer.
            //
            BufferGetWritePointer(pMSADPCM->pBuffer, &psLeft, &psRight,
                                  &lLength);

            //
            // Decode the next block.
            //
            if(adpcmDecode4Bit(pMSADPCM,
                               pMSADPCM->pucEncodedData + pMSADPCM->usOffset,
                               psLeft, psRight, lBlockSize) == 0)
            {
                //
                // We could not decode the block, so return an error.
                //
                return(0);
            }

            //
            // Update the output buffer's write pointer.
            //
            BufferUpdateWritePointer(pMSADPCM->pBuffer,
                                     pMSADPCM->usSamplesPerBlock);

            //
            // Increment the encoded data buffer pointer.
            //
            pMSADPCM->usOffset += lBlockSize;

            //
            // Increment the time based on the number of samples.
            //
            pMSADPCM->ulTimePos += pMSADPCM->usSamplesPerBlock;

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
            tMSADPCM *pMSADPCM;
            short *psLeft, *psRight;
            long lLength;

            //
            // The first parameter is a pointer to the MS ADPCM persistent
            // state.
            //
            pMSADPCM = (tMSADPCM *)ulParam1;

            //
            // Wait until there is enough data in the input buffer.
            //
            while(BufferDataAvailable(pMSADPCM->pBuffer) <
                  pMSADPCM->usSamplesPerBlock)
            {
                //
                // Put the EP7209 into IDLE mode.
                //
                Halt();
            }

            //
            // Get the read pointer for the input buffer.
            //
            BufferGetReadPointer(pMSADPCM->pBuffer, &psLeft, &psRight,
                                 &lLength);

            //
            // Compress the samples with MS ADPCM.
            //
            adpcmEncode4Bit(pMSADPCM, psLeft, psRight,
                            pMSADPCM->pucEncodedData + pMSADPCM->usValid);
            pMSADPCM->usValid += pMSADPCM->usBytesPerBlock;
            pMSADPCM->ulDataValid += pMSADPCM->usBytesPerBlock;

            //
            // Update the read pointer for the input buffer.
            //
            BufferUpdateReadPointer(pMSADPCM->pBuffer,
                                    pMSADPCM->usSamplesPerBlock);

            //
            // If there is enough data in the encoded data buffer, then write
            // it to the file.
            //
            if(pMSADPCM->usValid >= 4096)
            {
                //
                // Write 4096 bytes of data to the file.
                //
                if(FSWrite(pMSADPCM->pFile, pMSADPCM->pucEncodedData,
                           4096) == 0)
                {
                    return(0);
                }

                //
                // There are now 4096 less bytes in the encoded data buffer.
                //
                pMSADPCM->usValid -= 4096;

                //
                // Copy the valid data from the end of the buffer to the
                // beginning.
                //
                memcpy(pMSADPCM->pucEncodedData,
                       pMSADPCM->pucEncodedData + 4096, pMSADPCM->usValid);
            }

            //
            // Increment the time.
            //
            pMSADPCM->ulTimePos += pMSADPCM->usSamplesPerBlock;

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
            tMSADPCM *pMSADPCM;
            unsigned long ulPos;

            //
            // The first parameter is a pointer to the MS ADPCM persistent
            // state.
            //
            pMSADPCM = (tMSADPCM *)ulParam1;

            //
            // Make sure that the specified time is valid.
            //
            if(ulParam2 > pMSADPCM->ulTimeLength)
            {
                ulParam2 = pMSADPCM->ulTimeLength;
            }

            //
            // Compute the number of blocks that occur before the requested
            // time position.
            //
            ulPos = (((ulParam2 / 1000) * pMSADPCM->usSampleRate) /
                     pMSADPCM->usSamplesPerBlock) +
                    (((ulParam2 % 1000) * pMSADPCM->usSampleRate) /
                     (pMSADPCM->usSamplesPerBlock * 1000));

            //
            // Compute the time for the computed block number.
            //
            pMSADPCM->ulTimePos = ulPos * pMSADPCM->usSamplesPerBlock;

            //
            // Compute the file position for the computed block number.
            //
            ulPos = (ulPos * pMSADPCM->usBytesPerBlock) +
                    pMSADPCM->ulDataStart;

            //
            // Seek to the new position in the file.
            //
            FSSeek(pMSADPCM->pFile, ulPos & ~511);

            //
            // Read 4K from the file.
            //
            pMSADPCM->usValid = FSRead(pMSADPCM->pFile,
                                       pMSADPCM->pucEncodedData, 4096);

            //
            // Set the current position in the file.
            //
            pMSADPCM->usOffset = ulPos & 511;

            //
            // Set the number of valid byte remaining in the file.
            //
            pMSADPCM->ulDataValid = pMSADPCM->ulLength +
                                    pMSADPCM->ulDataStart - (ulPos & ~511);

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
            tMSADPCM *pMSADPCM;

            //
            // There is nothing to do if we were not encoding a file.
            //
            if(ulParam2 & CODEC_OPEN_ENCODE)
            {
                //
                // The first parameter is a pointer to the MS ADPCM persistent
                // state.
                //
                pMSADPCM = (tMSADPCM *)ulParam1;

                //
                // Write the remaining data from the encoded data buffer.
                //
                FSWrite(pMSADPCM->pFile, pMSADPCM->pucEncodedData,
                        pMSADPCM->usValid);
                
                //
                // Seek back to the beginning of the file.
                //
                FSSeek(pMSADPCM->pFile, 0);

                //
                // Read the first 512 bytes from the file.
                //
                FSRead(pMSADPCM->pFile, pMSADPCM->pucEncodedData, 512);

                //
                // Seek back to the beginning of the file.
                //
                FSSeek(pMSADPCM->pFile, 0);

                //
                // Fill in the number of samples in the "fact" chunk.
                //
                pMSADPCM->pucEncodedData[78] = pMSADPCM->ulTimePos;
                pMSADPCM->pucEncodedData[79] = pMSADPCM->ulTimePos >> 8;
                pMSADPCM->pucEncodedData[80] = pMSADPCM->ulTimePos >> 16;
                pMSADPCM->pucEncodedData[81] = pMSADPCM->ulTimePos >> 24;

                //
                // Fill in the number of data bytes in the "data" chunk.
                //
                pMSADPCM->pucEncodedData[86] = pMSADPCM->ulDataValid;
                pMSADPCM->pucEncodedData[87] = pMSADPCM->ulDataValid >> 8;
                pMSADPCM->pucEncodedData[88] = pMSADPCM->ulDataValid >> 16;
                pMSADPCM->pucEncodedData[89] = pMSADPCM->ulDataValid >> 24;

                //
                // Compute the length of the entire file.
                //
                pMSADPCM->ulDataValid += 82;

                //
                // Fill in the length of the file.
                //
                pMSADPCM->pucEncodedData[4] = pMSADPCM->ulDataValid;
                pMSADPCM->pucEncodedData[5] = pMSADPCM->ulDataValid >> 8;
                pMSADPCM->pucEncodedData[6] = pMSADPCM->ulDataValid >> 16;
                pMSADPCM->pucEncodedData[7] = pMSADPCM->ulDataValid >> 24;

                //
                // Write the first 512 bytes back to the file.
                //
                FSWrite(pMSADPCM->pFile, pMSADPCM->pucEncodedData, 512);
            }

            //
            // Success.
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

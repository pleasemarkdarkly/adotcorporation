//****************************************************************************
//
// ITRUST.C - Functions for opening and decrypting an InterTrust DigiFile.
//
// This software is provided pursuant to an agreement with InterTrust
// Technologies Corporation ("InterTrust"). This software may be used only
// as expressly allowed in such agreement.
//
// Copyright (c) 2001 by InterTrust. All rights reserved.
//
//****************************************************************************
#include "globals.h"
#ifdef SUPPORT_RIGHTS_PD
#define CONSUME_ONLY
#include "..\hwdefs.h"
#include "itrust\primal.h"
#include "itrust\prime.h"
#include "itrust\noheapreader.h"
#include "itrust\err.h"

//****************************************************************************
//
// HEAPSIZE has been chosen assuming that there is only one file open at a
// time.
//
//****************************************************************************
#define HEAPSIZE                                2048

//****************************************************************************
//
// The address of the heap used by the InterTrust kernel.
//
//****************************************************************************
static unsigned long ulAddrHeap;

//****************************************************************************
//
// Open the digifile.
// Inputs:
// pFile = file handle
// ulIntent = reason for opening file
// Outputs:
// pFile->ulSizeh = total size of header (in bytes).
//   Content begins at pucBuf+ulSizeH.
//   Will return zero if input is not a digifile.
// pFile->pvRPDUS = UserSession
// pFile->pvRPDCS = ConsumingSession
// pFile->pvRPDRD = NoHeapReader
// iRC = return code (0 if success, non-0 if failure)
//
//****************************************************************************
unsigned long
RPDLib_Open(tFile *pFile)
{
    int iRC;                  // return code
    unsigned char *pucBuf;    // addr of buffer containing first part of file
    unsigned long ulBufSize;  // size of buffer pointed to by pucBuf
    unsigned long ulOK;       // ulOK = 1 iff digifile
    unsigned long ulSizeH;    // size of header
    unsigned long ulCount;
    UserSession *pUS;
    ConsumingSession *pCS;
    NoHeapReader *pRD;
    DeviceInfo sDev;

    //
    // Will break out of this loop if there is an error.
    //
    do
    {
        //
        // Assume this is not a SCIF file.
        //
        ulSizeH = 0;
        pUS = 0;
        pCS = 0;
        pRD = 0;
        iRC = 0;

        //
        // Read the header.
        //
        pucBuf = FSGetScratch();
        ulBufSize = FSRead(pFile, (void *)pucBuf, 512);
        FSSeek(pFile, 0);

        //
        // Test if this is a SCIF file.
        //
        ulOK = NoHeapReader_isScif(pucBuf, ulBufSize);
        if(ulOK != 1)
        {
            break;
        }

        //
        // Allocate the kernel heap memory from the end of RAM.
        //
        ulAddrHeap = ulEndOfRAM;
        ulEndOfRAM += HEAPSIZE;

        //
        // Get the serial number to which this content should be bound.
        //
        memset(sDev.deviceId.bytes, 0, sizeof(sDev.deviceId.bytes));
        ulCount = sizeof(sDev.deviceId.bytes) * 8;
        if(FSGetMediaID(pFile->ucDrive, sDev.deviceId.bytes, &ulCount) == 0)
        {
            break;
        }
        sDev.name = "Maverick(tm)";

        //
        // Initialize the Prime kernel.
        //
        iRC = Prime_init(&sDev, (Byte *)ulAddrHeap, HEAPSIZE);
        if(iRC != 0)
        {
            break;
        }

        //
        // Open a user session.
        // This code could be moved to RPDLib_Once,
        // but it is logistically simpler to put it here.
        // Strictly speaking, it is not necessary to re-create
        // the user session each time a new file is opened.
        //
        iRC = Prime_openAnonymousUserSession(&pUS);
        if(iRC != 0)
        {
            break;
        }

        //
        // Open a consuming session.
        //
        iRC = UserSession_openConsumingSession(pUS, &pCS);
        if(iRC != 0)
        {
            break;
        }

        //
        // Create the reader
        //
        iRC = NoHeapReader_create(pCS, &pRD);
        if(iRC != 0)
        {
            break;
        }

        //
        // Read the SCIF header blocks from the buffer.
        //
        NoHeapReader_readBlocks(pRD, pucBuf, ulBufSize);

        //
        // Try and release the content to this device.
        //
        iRC = NoHeapReader_release(pRD);
        if(iRC != 0)
        {
            break;
        }

        //
        // Size of header (in bytes)
        //
        ulSizeH = (unsigned long) pRD->contentBlock->offset;
    }
    while(0);

    //
    // Save the InterTrust information associated with this file.
    //
    pFile->pvRPDUS = (void *)pUS;
    pFile->pvRPDCS = (void *)pCS;
    pFile->pvRPDRD = (void *)pRD;
    pFile->ulHeaderSize = ulSizeH;

    //
    // Return the result.
    //
    return(iRC);
}

//****************************************************************************
//
// Close the digifile.
// Inputs:
// pFile = file handle
// Outputs: none
//
//****************************************************************************
unsigned long
RPDLib_Close(tFile *pFile)
{
    //
    // Destroy the reader.
    //
    if(pFile->pvRPDRD != 0)
    {
        NoHeapReader_destroy((NoHeapReader *)pFile->pvRPDRD);
        pFile->pvRPDRD = 0;
    }

    //
    // Destroy the Consuming Session.
    //
    if(pFile->pvRPDCS != 0)
    {
        ConsumingSession_close((ConsumingSession *)pFile->pvRPDCS);
        pFile->pvRPDCS = 0;
    }

    //
    // Destroy the User Session.
    //
    if(pFile->pvRPDUS != 0)
    {
        UserSession_close((UserSession *)pFile->pvRPDUS);
        pFile->pvRPDUS = 0;
    }

    //
    // Set the size of the header to zero.
    //
    pFile->ulHeaderSize = 0;

    //
    // Shut down the InterTrust kernel.
    //
    if(ulAddrHeap)
    {
        Prime_shutdown();
        ulEndOfRAM = ulAddrHeap;
        ulAddrHeap = 0;
    }

    //
    // Success.
    //
    return(0);
}

//*****************************************************************
// Decrypt one buffer worth of content.
// Inputs:
// pFile = file handle
// pucAddr = addr of current buffer
// ulSize = size of buffer (in bytes)
// ulPos = position of current buffer relative to content
// Outputs:
// ulNumDec = number of bytes successfully decrypted
//
//*****************************************************************
unsigned long
RPDLib_Decrypt(tFile *pFile, unsigned char *pucAddr, unsigned long ulSize,
               unsigned long ulPos)
{
    unsigned long ulNumDec;  // number of bytes successfully decrypted
    NoHeapReader *pRD;

    //
    // Get the pointer to the no heap reader.
    //
    pRD = (NoHeapReader *) pFile->pvRPDRD;

    //
    // Decrypt the data.
    //
    ulNumDec = NoHeapReader_decryptBytes(pRD, pucAddr, ulSize, 0, ulPos);

    //
    // Return the number of byte decrypted.
    //
    return(ulNumDec);
}
#endif

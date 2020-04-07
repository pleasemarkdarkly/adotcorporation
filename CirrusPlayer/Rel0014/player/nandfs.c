//****************************************************************************
//
// NANDFS.C - File system driver for the on-board NAND FLASH.
//
// Copyright (c) 1999-2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include "../hwdefs.h"
#include "globals.h"

//****************************************************************************
//
// The persistent state of the NAND file system.
//
//****************************************************************************
static struct
{
    //
    // A list of the bad blocks in the on-board NAND FLASH, in numerical order.
    //
    unsigned short usBadBlocks[64];

    //
    // The number of bad blocks in the on-board NAND FLASH.
    //
    unsigned short usNumBadBlocks;

    //
    // The number of blocks in the on-board NAND FLASH.
    //
    unsigned short usNumBlocks;

    //
    // The number of pages per block of the on-board NAND FLASH.
    //
    unsigned char ucPagesPerBlock;

    //
    // The length of the one file in the on-board NAND FLASH.
    //
    unsigned long ulFileLength;

    //
    // A pointer to the function used to read a page from the on-board NAND
    // FLASH.
    //
    void (*pfnRead)(unsigned long ulNANDAddress, unsigned long ulPage,
                    unsigned char *pucData, unsigned char *pucRedundant);

    //
    // A pointer to the function used to write a page to the on-board NAND
    // FLASH.
    //
    void (*pfnWrite)(unsigned long ulNANDAddress, unsigned long ulPage,
                     unsigned char *pucData, unsigned char *pucRedundant);

    //
    // A pointer to the function used to erase a block of the on-board NAND
    // FLASH.
    //
    void (*pfnErase)(unsigned long ulNANDAddress, unsigned long ulBlock);
} sNAND;

//****************************************************************************
//
// The following structure contains the parameters of an opened file.
//
//****************************************************************************
typedef struct
{
    //
    // The next page of the file to be read.
    //
    unsigned long ulPage;

    //
    // The current byte offset into the file.
    //
    unsigned long ulFilePos;
} tNANDFile;

//****************************************************************************
//
// The following structure contains the parameters of an opened directory.
//
//****************************************************************************
typedef struct
{
    //
    // An indicator that is true when there have been no IOCTL_FS_READDIR
    // Ioctls after an IOCTL_FS_OPENDIR Ioctl.
    //
    unsigned long bIsFirstDirEntry;
} tNANDDir;

//****************************************************************************
//
// NextPage increments to the next valid page of the NAND FLASH.  The next
// valid page is "*pulPage++", unless this falls on an invalid block.  The
// current value of the page variable is returned, so that this function can
// be used as a post-increment reference (i.e. NextPage(&x) is roughly
// equivalent to x++).
//
//****************************************************************************
static unsigned long
NextPage(unsigned long *pulPage)
{
    unsigned long ulRet;
    int iIdx;

    //
    // Save the current page number.
    //
    ulRet = *pulPage;

    //
    // Increment the page number.
    //
    (*pulPage)++;

    //
    // If this is not the first page of a block, then it is not part of an
    // invalid block.
    //
    if((*pulPage & (sNAND.ucPagesPerBlock - 1)) != 0)
    {
        //
        // Return the previous page number.
        //
        return(ulRet);
    }

    //
    // Increment the page number until we reach a page in a valid block.
    //
    while(1)
    {
        //
        // See if this page lies in one of our invalid blocks.
        //
        for(iIdx = 0; iIdx < sNAND.usNumBadBlocks; iIdx++)
        {
            if((*pulPage / sNAND.ucPagesPerBlock) == sNAND.usBadBlocks[iIdx])
            {
                break;
            }
        }

        //
        // If this page does not lie in one of the invalid blocks, then stop
        // incrementing the page number.
        //
        if(iIdx == sNAND.usNumBadBlocks)
        {
            break;
        }

        //
        // Skip to the first page of the next block.
        //
        *pulPage += sNAND.ucPagesPerBlock;
    }

    //
    // Return the previous page number.
    //
    return(ulRet);
}

//****************************************************************************
//
// CheckNAND determines if the NAND FLASH on the board has been "formatted"
// and reads the bad block table if it has been formatted.
//
//****************************************************************************
static int
CheckNAND(unsigned char *pucBuffer, unsigned char *pucWriteBuffer)
{
    int iLoop;

    //
    // Read the first page of the NAND FLASH.
    //
    (sNAND.pfnRead)(HwNANDAddress, 0, pucBuffer, pucWriteBuffer);

    //
    // See if the NAND FLASH has been formatted by looking for the signature
    // at the beginning of the first page.
    //
    if((pucBuffer[0] != 'B') || (pucBuffer[1] != 'K') ||
       (pucBuffer[2] != 'O') || (pucBuffer[3] != 'K'))
    {
        //
        // The signature does not exist, so the NAND FLASH has not been
        // formatted yet.
        //
        return(0);
    }

    //
    // Get the number of bad blocks.
    //
    sNAND.usNumBadBlocks = pucBuffer[4] | (pucBuffer[5] << 8);

    //
    // Make sure that the number of bad blocks is valid.
    //
    if(sNAND.usNumBadBlocks > 64)
    {
        sNAND.usNumBadBlocks = 0;
        return(0);
    }

    //
    // Read the bad block table.
    //
    for(iLoop = 0; iLoop < sNAND.usNumBadBlocks; iLoop++)
    {
        sNAND.usBadBlocks[iLoop] = pucBuffer[6 + (iLoop << 1)] |
                                   (pucBuffer[7 + (iLoop << 1)] << 8);
    }

    //
    // The media has been formatted.
    //
    return(1);
}

//****************************************************************************
//
// FormatNAND prepares the NAND FLASH for use to record/download file data.
// It checks the NAND FLASH for bad blocks and builds a bad block table.
//
//****************************************************************************
static void
FormatNAND(unsigned char *pucBuffer, unsigned char *pucWriteBuffer)
{
    unsigned long ulValue;
    int iBlock, iPage, iIdx;

    //
    // We only need to check for bad blocks if the NAND FLASH has not already
    // been formatted.
    //
    if(!CheckNAND(pucBuffer, pucWriteBuffer))
    {
        //
        // Initially, there are no bad blocks in the NAND FLASH (until we find
        // bad blocks).
        //
        sNAND.usNumBadBlocks = 0;

        //
        // Loop through all the blocks in the on-board NAND FLASH.
        //
        for(iBlock = 1; iBlock < sNAND.usNumBlocks; iBlock++)
        {
            //
            // Read the first two pages of each block.  The page will be all
            // 0xff if it is good, and will contain zeros if it is bad.
            //
            for(iPage = 0; iPage < 2; iPage++)
            {
                //
                // Read the page from NAND FLASH.
                //
                (sNAND.pfnRead)(HwNANDAddress,
                                (iBlock * sNAND.ucPagesPerBlock) + iPage,
                                pucBuffer, pucWriteBuffer);

                //
                // Verify that the page contains all 0xff.
                //
                for(iIdx = 0; iIdx < 512; iIdx++)
                {
                    if(pucBuffer[iIdx] != 0xff)
                    {
                        break;
                    }
                }

                //
                // If this page did not contain all 0xff, then break out of the
                // loop.
                //
                if(iIdx != 512)
                {
                    break;
                }

                //
                // Verify that the spare area contains all 0xff.
                //
                for(iIdx = 0; iIdx < 16; iIdx++)
                {
                    if(pucWriteBuffer[iIdx] != 0xff)
                    {
                        break;
                    }
                }

                //
                // If the spare area did not contain all 0xff, then break out
                // of the loop.
                //
                if(iIdx != 16)
                {
                    break;
                }
            }

            //
            // If both pages did not contain all 0xff, then this block is bad.
            //
            if(iPage != 2)
            {
                //
                // Save this block in the bad block table.
                //
                sNAND.usBadBlocks[sNAND.usNumBadBlocks++] = iBlock;

                //
                // Quit scanning if we've found too many bad blocks.  This
                // ignores any following bad blocks, but what can you do (this
                // isn't exactly a "filesystem" and is certainly not product
                // worthy)?
                //
                if(sNAND.usNumBadBlocks == 64)
                {
                    break;
                }
            }
        }

        //
        // Build the signature and bad block table for the first page of the
        // NAND FLASH.
        //
        pucBuffer[0] = 'B';
        pucBuffer[1] = 'K';
        pucBuffer[2] = 'O';
        pucBuffer[3] = 'K';

        //
        // Fill in the bad block table.
        //
        pucBuffer[4] = sNAND.usNumBadBlocks & 0xFF;
        pucBuffer[5] = (sNAND.usNumBadBlocks >> 8) & 0xFF;
        for(iIdx = 0; iIdx < sNAND.usNumBadBlocks; iIdx++)
        {
            pucBuffer[6 + (iIdx << 1)] = sNAND.usBadBlocks[iIdx] & 0xFF;
            pucBuffer[7 + (iIdx << 1)] = (sNAND.usBadBlocks[iIdx] >> 8) & 0xFF;
        }

        //
        // Write the bad block table to the beginning of the NAND FLASH.
        //
        (sNAND.pfnErase)(HwNANDAddress, 0);
        NANDWaitTilNotBusy(HwNANDAddress);
        (sNAND.pfnWrite)(HwNANDAddress, 0, pucBuffer, pucWriteBuffer);
        NANDWaitTilNotBusy(HwNANDAddress);
    }

    //
    // Set the file size to zero, so that we do not try to play the empty
    // contents of the NAND FLASH.
    //
    pucBuffer[0] = 0;
    pucBuffer[1] = 0;
    pucBuffer[2] = 0;
    pucBuffer[3] = 0;
    ulValue = sNAND.ucPagesPerBlock - 1;
    NextPage(&ulValue);
    (sNAND.pfnErase)(HwNANDAddress, ulValue / sNAND.ucPagesPerBlock);
    NANDWaitTilNotBusy(HwNANDAddress);
    (sNAND.pfnWrite)(HwNANDAddress, ulValue, pucBuffer, pucWriteBuffer);
    NANDWaitTilNotBusy(HwNANDAddress);
}

//****************************************************************************
//
// The file system entry point for the on-board NAND FLASH.
//
//****************************************************************************
unsigned long
NANDIoctl(unsigned char *pucScratch, unsigned char *pucWriteBuffer,
          unsigned long ulIoctl, unsigned long ulInstance,
          unsigned long ulParam1, unsigned long ulParam2)
{
    unsigned long * volatile pulGPIO = (unsigned long *)HwBaseAddress;

    //
    // If we are being initialized, then we need to setup our global variables.
    //
    if(ulIoctl == IOCTL_FS_INIT)
    {
        unsigned long ulDeviceID;

        //
        // Select the on-board NAND.
        //
        pulGPIO[HwPortABCD >> 2] &= ~HwPortABCD_NAND1_CS;

        //
        // Get the ID of the on-board NAND FLASH.
        //
        ulDeviceID = NANDGetID(HwNANDAddress);

        //
        // Determine the size and geometry of the on-board NAND FLASH.
        //
        switch(ulDeviceID)
        {
            //
            // The device capacity is 4MB.
            //
            case 0x6B:
            case 0xE3:
            case 0xE5:
            {
                //
                // There are 512 blocks in this device.
                //
                sNAND.usNumBlocks = 512;

                //
                // There are 16 pages per block in this device.
                //
                sNAND.ucPagesPerBlock = 16;

                //
                // Fill in the function pointers for accessing this FLASH
                // device.
                //
                sNAND.pfnRead = NANDRead_512_3;
                sNAND.pfnWrite = NANDWrite_512_3;
                sNAND.pfnErase = NANDErase_16;

                //
                // We've interpreted the device ID.
                //
                break;
            }

            //
            // The device capacity is 8MB.
            //
            case 0xE6:
            {
                //
                // There are 1024 blocks in this device.
                //
                sNAND.usNumBlocks = 1024;

                //
                // There are 16 pages per block in this device.
                //
                sNAND.ucPagesPerBlock = 16;

                //
                // Fill in the function pointers for accessing this FLASH
                // device.
                //
                sNAND.pfnRead = NANDRead_512_3;
                sNAND.pfnWrite = NANDWrite_512_3;
                sNAND.pfnErase = NANDErase_16;

                //
                // We've interpreted the device ID.
                //
                break;
            }

            //
            // The device capacity is 16MB.
            //
            case 0x73:
            {
                //
                // There are 1024 blocks in this device.
                //
                sNAND.usNumBlocks = 1024;

                //
                // There are 32 pages per block in this device.
                //
                sNAND.ucPagesPerBlock = 32;

                //
                // Fill in the function pointers for accessing this FLASH
                // device.
                //
                sNAND.pfnRead = NANDRead_512_3;
                sNAND.pfnWrite = NANDWrite_512_3;
                sNAND.pfnErase = NANDErase_32_3;

                //
                // We've interpreted the device ID.
                //
                break;
            }

            //
            // The device capacity is 32MB.
            //
            case 0x75:
            {
                //
                // There are 2048 blocks in this device.
                //
                sNAND.usNumBlocks = 2048;

                //
                // There are 32 pages per block in this device.
                //
                sNAND.ucPagesPerBlock = 32;

                //
                // Fill in the function pointers for accessing this FLASH
                // device.
                //
                sNAND.pfnRead = NANDRead_512_3;
                sNAND.pfnWrite = NANDWrite_512_3;
                sNAND.pfnErase = NANDErase_32_3;

                //
                // We've interpreted the device ID.
                //
                break;
            }

            //
            // The device capacity is 64MB.
            //
            case 0x76:
            {
                //
                // There are 4096 blocks in this device.
                //
                sNAND.usNumBlocks = 4096;

                //
                // There are 32 pages pre block in this device.
                //
                sNAND.ucPagesPerBlock = 32;

                //
                // Fill in the function pointers for accessing this FLASH
                // device.
                //
                sNAND.pfnRead = NANDRead_512_4;
                sNAND.pfnWrite = NANDWrite_512_4;
                sNAND.pfnErase = NANDErase_32_4;

                //
                // We've interpreted the device ID.
                //
                break;
            }

            //
            // The device capacity is 128MB.
            //
            case 0x79:
            {
                //
                // There are 8192 blocks in this device.
                //
                sNAND.usNumBlocks = 8192;

                //
                // There are 32 pages per block in this device.
                //
                sNAND.ucPagesPerBlock = 32;

                //
                // Fill in the function pointers for accessing this FLASH
                // device.
                //
                sNAND.pfnRead = NANDRead_512_4;
                sNAND.pfnWrite = NANDWrite_512_4;
                sNAND.pfnErase = NANDErase_32_4;

                //
                // We've interpreted the device ID.
                //
                break;
            }

            //
            // Either the FLASH device could not be found or it has an unknown
            // device ID.
            //
            default:
            {
                //
                // Return a failure.
                //
                sNAND.usNumBlocks = 0;
                sNAND.ucPagesPerBlock = 0;
                pulGPIO[HwPortABCD >> 2] |= HwPortABCD_NAND1_CS;
                return(0);
            }
        }

        //
        // See if the on-board NAND FLASH is formatted.
        //
        if(!CheckNAND(pucScratch, pucWriteBuffer))
        {
            //
            // The on-board NAND FLASH is not formatted, so format it now.
            //
            FormatNAND(pucScratch, pucWriteBuffer);
        }

        //
        // Read the length of the file.
        //
        ulDeviceID = sNAND.ucPagesPerBlock - 1;
        NextPage(&ulDeviceID);
        (sNAND.pfnRead)(HwNANDAddress, ulDeviceID, pucScratch, pucWriteBuffer);
        sNAND.ulFileLength = pucScratch[0] | (pucScratch[1] << 8) |
                             (pucScratch[2] << 16) | (pucScratch[3] << 24);

        //
        // De-select the on-board NAND FLASH.
        //
        pulGPIO[HwPortABCD >> 2] |= HwPortABCD_NAND1_CS;

        //
        // Success.
        //
        return(1);
    }

    //
    // Make sure that we were able to successfully initialize the on-board NAND
    // FLASH.  Return a failure if we could not.
    //
    if(sNAND.usNumBlocks == 0)
    {
        return(0);
    }

    //
    // Determine what to do based on the specified IOCTL.
    //
    switch(ulIoctl)
    {
        //
        // Return the media unique ID.
        //
        case IOCTL_FS_GETMEDIAID:
        {
            //
            // We do not support a media unique ID, so return failure.
            //
            return(0);
        }

        //
        // Open the specified file.  We do not support multiple files, so we
        // also don't care what the file name is...any open will work for the
        // single file in the file system.
        //
        case IOCTL_FS_OPEN:
        {
            tNANDFile *pFile;

            //
            // We do not support writing to a file that we do not create.
            //
            if((ulParam2 & FS_OPEN_CREATE) && !(ulParam2 & FS_OPEN_WRITE))
            {
                return(0);
            }

            //
            // Get a pointer to the file structure.
            //
            pFile = (tNANDFile *)ulInstance;

            //
            // Get the first page used to store file data.
            //
            pFile->ulPage = sNAND.ucPagesPerBlock - 1;
            NextPage(&(pFile->ulPage));

            //
            // Select the on-board NAND FLASH.
            //
            pulGPIO[HwPortABCD >> 2] &= ~HwPortABCD_NAND1_CS;

            //
            // Are we reading or writing?
            //
            if(ulParam2 & FS_OPEN_READ)
            {
                //
                // If the file length is zero, then there is no file in the
                // on-board NAND FLASH, so return a failure.
                //
                if(sNAND.ulFileLength == 0)
                {
                    pulGPIO[HwPortABCD >> 2] |= HwPortABCD_NAND1_CS;
                    return(0);
                }
            }
            else
            {
                //
                // Erase the first block of the on-board NAND FLASH.
                //
                NANDWaitTilNotBusy(HwNANDAddress);
                (sNAND.pfnErase)(HwNANDAddress,
                                 pFile->ulPage / sNAND.ucPagesPerBlock);

                //
                // Skip to the next page of the on-board NAND FLASH.
                //
                NextPage(&(pFile->ulPage));

                //
                // The initial file length is zero.
                //
                sNAND.ulFileLength = 0;
            }

            //
            // Set the file position to the beginning.
            //
            pFile->ulFilePos = 0;

            //
            // De-select the on-board NAND FLASH.
            //
            pulGPIO[HwPortABCD >> 2] |= HwPortABCD_NAND1_CS;

            //
            // Success.
            //
            return(1);
        }

        //
        // Create the specified file.
        //
        case IOCTL_FS_CREATE:
        {
            tNANDFile *pFile;

            //
            // Get a pointer to the file structure.
            //
            pFile = (tNANDFile *)ulInstance;

            //
            // Get the first page used to store file data.
            //
            pFile->ulPage = sNAND.ucPagesPerBlock - 1;
            NextPage(&(pFile->ulPage));

            //
            // Select the on-board NAND FLASH.
            //
            pulGPIO[HwPortABCD >> 2] &= ~HwPortABCD_NAND1_CS;

            //
            // Erase the first block of the on-board NAND FLASH.
            //
            NANDWaitTilNotBusy(HwNANDAddress);
            (sNAND.pfnErase)(HwNANDAddress,
                             pFile->ulPage / sNAND.ucPagesPerBlock);

            //
            // Skip to the next page of the on-board NAND FLASH.
            //
            NextPage(&(pFile->ulPage));

            //
            // The initial file length is zero.
            //
            sNAND.ulFileLength = 0;

            //
            // Set the file position to the beginning.
            //
            pFile->ulFilePos = 0;

            //
            // De-select the on-board NAND FLASH.
            //
            pulGPIO[HwPortABCD >> 2] |= HwPortABCD_NAND1_CS;

            //
            // Success.
            //
            return(1);
        }

        //
        // Read data from the currently opened file.
        //
        case IOCTL_FS_READ:
        {
            tNANDFile *pFile;
            unsigned long ulCount;

            //
            // Make sure that the number of bytes requested is a multiple of
            // 512.
            //
            if(ulParam2 & 511)
            {
                return(0);
            }

            //
            // Get a pointer to the file structure.
            //
            pFile = (tNANDFile *)ulInstance;

            //
            // Select the on-board NAND FLASH.
            //
            pulGPIO[HwPortABCD >> 2] &= ~HwPortABCD_NAND1_CS;

            //
            // Read pages from the on-board NAND FLASH.
            //
            ulCount = 0;
            while(ulParam2)
            {
                //
                // Stop reading if we've reached the end of the file.
                //
                if(pFile->ulFilePos >= sNAND.ulFileLength)
                {
                    break;
                }

                //
                // Read the next page from the on-board NAND FLASH.
                //
                (sNAND.pfnRead)(HwNANDAddress, NextPage(&(pFile->ulPage)),
                                (unsigned char *)ulParam1, pucScratch);

                //
                // Increment the read buffer pointer.
                //
                ulParam1 += 512;

                //
                // Decrement the count of bytes to read.
                //
                ulParam2 -= 512;

                //
                // Increment the file position.
                //
                pFile->ulFilePos += 512;

                //
                // Increment the count of bytes read.
                //
                ulCount += 512;
            }

            //
            // De-select the on-board NAND FLASH.
            //
            pulGPIO[HwPortABCD >> 2] |= HwPortABCD_NAND1_CS;

            //
            // Success.
            //
            return(ulCount);
        }

        //
        // Read data from the currently opened file, performing a byte-swap.
        //
        case IOCTL_FS_READ_BS:
        {
            tNANDFile *pFile;
            unsigned long ulCount, ulIdx;

            //
            // Make sure that the number of bytes requested is a multiple of
            // 512.
            //
            if(ulParam2 & 511)
            {
                return(0);
            }

            //
            // Get a pointer to the file structure.
            //
            pFile = (tNANDFile *)ulInstance;

            //
            // Select the on-board NAND FLASH.
            //
            pulGPIO[HwPortABCD >> 2] &= ~HwPortABCD_NAND1_CS;

            //
            // Read pages from the on-board NAND FLASH.
            //
            ulCount = 0;
            while(ulParam2)
            {
                //
                // Stop reading if we've reached the end of the file.
                //
                if(pFile->ulFilePos >= sNAND.ulFileLength)
                {
                    break;
                }

                //
                // Read the next page from the on-board NAND FLASH, byte
                // swapping the data as we read it.
                //
                (sNAND.pfnRead)(HwNANDAddress, NextPage(&(pFile->ulPage)),
                                (unsigned char *)ulParam1, pucScratch);

                //
                // Swap the bytes in the page.
                //
                for(ulIdx = 0; ulIdx < 512; ulIdx += 4)
                {
                    unsigned long ulTemp;

                    ulTemp = ((unsigned long *)ulParam1)[ulIdx >> 2];
                    ulTemp = ((ulTemp << 24) & 0xff000000) |
                             ((ulTemp << 8) & 0x00ff0000) |
                             ((ulTemp >> 8) & 0x0000ff00) |
                             ((ulTemp >> 24) & 0x000000ff);
                    ((unsigned long *)ulParam1)[ulIdx >> 2] = ulTemp;
                }

                //
                // Increment the read buffer pointer.
                //
                ulParam1 += 512;

                //
                // Decrement the count of bytes to read.
                //
                ulParam2 -= 512;

                //
                // Increment the file position.
                //
                pFile->ulFilePos += 512;

                //
                // Increment the count of bytes read.
                //
                ulCount += 512;
            }

            //
            // De-select the on-board NAND FLASH.
            //
            pulGPIO[HwPortABCD >> 2] |= HwPortABCD_NAND1_CS;

            //
            // Success.
            //
            return(ulCount);
        }

        //
        // Write data to the currently opened file.
        //
        case IOCTL_FS_WRITE:
        {
            tNANDFile *pFile;
            unsigned long ulCount;

            //
            // Get a pointer to the file structure.
            //
            pFile = (tNANDFile *)ulInstance;

            //
            // Make sure that the current file pointer is a multiple of 512.
            //
            if(pFile->ulFilePos & 511)
            {
                return(0);
            }

            //
            // Select the on-board NAND FLASH.
            //
            pulGPIO[HwPortABCD >> 2] &= ~HwPortABCD_NAND1_CS;

            //
            // Write pages to the on-board NAND FLASH.
            //
            ulCount = 0;
            while(ulParam2)
            {
                //
                // Stop writing data if we've reached the end of the on-board
                // NAND FLASH.
                //
                if(pFile->ulPage ==
                   (sNAND.usNumBlocks * sNAND.ucPagesPerBlock))
                {
                    break;
                }

                //
                // If this is the first page of a block, then erase the block.
                //
                if((pFile->ulPage & (sNAND.ucPagesPerBlock - 1)) == 0)
                {
                    NANDWaitTilNotBusy(HwNANDAddress);
                    (sNAND.pfnErase)(HwNANDAddress,
                                     pFile->ulPage / sNAND.ucPagesPerBlock);
                }

                //
                // Write the next page to the on-board NAND FLASH.
                //
                NANDWaitTilNotBusy(HwNANDAddress);
                (sNAND.pfnWrite)(HwNANDAddress, NextPage(&(pFile->ulPage)),
                                 (unsigned char *)ulParam1, pucScratch);

                //
                // See if this was a partial sector at the end of the write.
                //
                if(ulParam2 < 512)
                {
                    //
                    // Increment the file length.
                    //
                    sNAND.ulFileLength += ulParam2;

                    //
                    // Increment the count of the bytes written.
                    //
                    ulCount += ulParam2;

                    //
                    // There are no byte remaining to be written.
                    //
                    ulParam2 = 0;
                }

                //
                // Otherwise, it was a full sector.
                //
                else
                {
                    //
                    // Increment the write buffer pointer.
                    //
                    ulParam1 += 512;

                    //
                    // Decrement the count of bytes to write.
                    //
                    ulParam2 -= 512;

                    //
                    // Increment the file length.
                    //
                    sNAND.ulFileLength += 512;

                    //
                    // Increment the count of the bytes written.
                    //
                    ulCount += 512;
                }
            }

            //
            // De-select the on-board NAND FLASH.
            //
            pulGPIO[HwPortABCD >> 2] |= HwPortABCD_NAND1_CS;

            //
            // Success.
            //
            return(ulCount);
        }

        //
        // Seek to the specified position in the currently opened file.
        //
        case IOCTL_FS_SEEK:
        {
            tNANDFile *pFile;
            unsigned long ulCount;

            //
            // Make sure that the seek position is a multiple of 512.
            //
            if(ulParam1 & 511)
            {
                return(0);
            }

            //
            // Make sure that the seek position is within the file.
            //
            if(ulParam1 > sNAND.ulFileLength)
            {
                return(0);
            }

            //
            // Get a pointer to the file structure.
            //
            pFile = (tNANDFile *)ulInstance;

            //
            // Set the file position to the seek position.
            //
            pFile->ulFilePos = ulParam1;

            //
            // Compute the page number for this position within the file.
            //
            pFile->ulPage = sNAND.ucPagesPerBlock + 1 + (ulParam1 / 512);

            //
            // Skip the bad blocks in the NAND FLASH.  This works by adding a
            // block's worth of offset when the offset is greater than the
            // offset of a bad block.
            //
            for(ulCount = 0; ulCount < sNAND.usNumBadBlocks; ulCount++)
            {
                //
                // Does this page start at or after a bad block?
                //
                if((pFile->ulPage / sNAND.ucPagesPerBlock) >=
                   sNAND.usBadBlocks[ulCount])
                {
                    //
                    // It does, so skip a block's worth of data.
                    //
                    pFile->ulPage += sNAND.ucPagesPerBlock;
                }
            }

            //
            // Success.
            //
            return(1);
        }

        //
        // Return the current read/write pointer of the file.
        //
        case IOCTL_FS_TELL:
        {
            tNANDFile *pFile;

            //
            // Get a pointer to the file structure.
            //
            pFile = (tNANDFile *)ulInstance;

            //
            // Return the current read/write pointer.
            //
            return(pFile->ulFilePos);
        }

        //
        // Return the length of the currently opened file.
        //
        case IOCTL_FS_LENGTH:
        {
            //
            // Return the file length.
            //
            return(sNAND.ulFileLength);
        }

        //
        // Close the currently opened file.
        //
        case IOCTL_FS_CLOSE:
        {
            tNANDFile *pFile;

            //
            // Get a pointer to the file structure.
            //
            pFile = (tNANDFile *)ulInstance;

            //
            // See if we were writing this file.
            //
            if(ulParam1 & FS_OPEN_WRITE)
            {
                //
                // Select the on-board NAND FLASH.
                //
                pulGPIO[HwPortABCD >> 2] &= ~HwPortABCD_NAND1_CS;

                //
                // Go back to the first page of the file.
                //
                pFile->ulPage = sNAND.ucPagesPerBlock - 1;
                NextPage(&(pFile->ulPage));

                //
                // Write the first page of the file.
                //
                NANDWaitTilNotBusy(HwNANDAddress);
                (sNAND.pfnWrite)(HwNANDAddress, pFile->ulPage,
                                 (unsigned char *)&(sNAND.ulFileLength),
                                 pucScratch);

                //
                // Make sure that the on-board NAND FLASH is done programming
                // the page before we continue.
                //
                NANDWaitTilNotBusy(HwNANDAddress);

                //
                // De-select the on-board NAND FLASH.
                //
                pulGPIO[HwPortABCD >> 2] |= HwPortABCD_NAND1_CS;
            }

            //
            // Success.
            //
            return(1);
        }

        //
        // Deletes the specified file from the file system.  We do not support
        // multiple files, so we also don't care what the file name is...any
        // delete will work for the single file in the file system.
        //
        // We also use this to handle formatting the file system, since a
        // format will simply blast the singe file.
        //
        case IOCTL_FS_DELETE:
        case IOCTL_FS_FORMAT:
        {
            unsigned long ulPage;

            //
            // Select the on-board NAND FLASH.
            //
            pulGPIO[HwPortABCD >> 2] &= ~HwPortABCD_NAND1_CS;

            //
            // Go to the first page of the file.
            //
            ulPage = sNAND.ucPagesPerBlock - 1;
            NextPage(&ulPage);

            //
            // Erase the first block of the file.
            //
            NANDWaitTilNotBusy(HwNANDAddress);
            (sNAND.pfnErase)(HwNANDAddress, ulPage / sNAND.ucPagesPerBlock);

            //
            // Set the file length to zero.
            //
            sNAND.ulFileLength = 0;

            //
            // Write a file length of zero.
            //
            NANDWaitTilNotBusy(HwNANDAddress);
            (sNAND.pfnWrite)(HwNANDAddress, ulPage,
                             (unsigned char *)&(sNAND.ulFileLength),
                             pucScratch);

            //
            // Make sure that the on-board NAND FLASH is done programming the
            // page before we continue.
            //
            NANDWaitTilNotBusy(HwNANDAddress);

            //
            // De-select the on-board NAND FLASH.
            //
            pulGPIO[HwPortABCD >> 2] |= HwPortABCD_NAND1_CS;

            //
            // Success.
            //
            return(1);
        }

        //
        // Opens the directory of the file system.
        //
        case IOCTL_FS_OPENDIR:
        {
            tNANDDir *pDir;

            //
            // Get a pointer to the directory structure.
            //
            pDir = (tNANDDir *)ulInstance;

            //
            // Indicate that the next IOCTL_FS_READDIR will be the first.
            //
            if(sNAND.ulFileLength)
            {
                pDir->bIsFirstDirEntry = 1;
            }
            else
            {
                pDir->bIsFirstDirEntry = 0;
            }

            //
            // Success.
            //
            return(1);
        }

        //
        // Read the next directory entry from the file system.
        //
        case IOCTL_FS_READDIR:
        {
            tNANDDir *pDir;

            //
            // Get a pointer to the directory structure.
            //
            pDir = (tNANDDir *)ulInstance;

            //
            // If this is not the first IOCTL_FS_READDIR, then return a failure
            // since we only support a single file in the on-board NAND FLASH.
            //
            if(!pDir->bIsFirstDirEntry)
            {
                return(0);
            }

            //
            // Fill in the file name with a bogus name.
            //
            memcpy((void *)ulParam1, "n\0a\0n\0d\0.\0d\0a\0t\0\0", 18);

            //
            // Indicate that the next IOCTL_FS_READDIR will not be the first.
            //
            pDir->bIsFirstDirEntry = 0;

            //
            // Success.
            //
            return(1);
        }

        //
        // Close the directory of the file system.
        //
        case IOCTL_FS_CLOSEDIR:
        {
            //
            // Success.
            //
            return(1);
        }

        //
        // Create a new directory in the file system.
        //
        case IOCTL_FS_MAKEDIR:
        {
            //
            // We do not support subdirectories, so return a failure.
            //
            return(0);
        }

        //
        // Remove the specified directory from the file system.
        //
        case IOCTL_FS_REMOVEDIR:
        {
            //
            // We do not support subdirectories, so return a failure.
            //
            return(0);
        }

        //
        // Determine the total capacity of the file system.
        //
        case IOCTL_FS_TOTALSPACE:
        {
            //
            // The total capacity of the file system is the number of good
            // blocks minus 1, times the number of pages per block, minus 1,
            // time 512 bytes per page.
            //
            return((((sNAND.usNumBlocks - sNAND.usNumBadBlocks - 1) *
                     sNAND.ucPagesPerBlock) - 1) * 512);
        }

        //
        // Determine the available capacity in the file system.
        //
        case IOCTL_FS_FREESPACE:
        {
            //
            // The free space is the total capacity minus the space used by our
            // one file.
            //
            return(((((sNAND.usNumBlocks - sNAND.usNumBadBlocks - 1) *
                      sNAND.ucPagesPerBlock) - 1) * 512) - sNAND.ulFileLength);
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

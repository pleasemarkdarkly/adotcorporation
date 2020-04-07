//****************************************************************************
//
// NAND.C - File system driver for the on-board NAND FLASH.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include "../hwdefs.h"
#include "globals.h"
#include "fat/fat.h"
#include "ftl/ftl.h"

//****************************************************************************
//
// The name of this drive.
//
//****************************************************************************
static const unsigned short pusDriveName[] =
{
    'I', 'n', 't', 'e', 'r', 'n', 'a', 'l', ' ', 'N', 'A', 'N', 'D', '\0'
};

//****************************************************************************
//
// The persistent state of the NAND driver.
//
//****************************************************************************
static struct
{
    //
    // The persistent state of the FTL layer used for the on-board NAND.
    //
    tFTL sFTL;

    //
    // The persistent state of the FAT layer used for the on-board NAND.
    //
    tFAT sFAT;

    //
    // Flags indicating the state of the NAND FLASH driver.
    //
    unsigned long ulFlags;
} sNAND;

//****************************************************************************
//
// The set of flags that indicate the state of the NAND FLASH driver.
//
//****************************************************************************
#define FLAG_ID_MASK                            0x000000ff
#define FLAG_IS_BUSY1                           0x40000000
#define FLAG_IS_BUSY2                           0x80000000

//****************************************************************************
//
// The FLASH driver IOCTL entry point for the on-board NAND.
//
//****************************************************************************
static unsigned long
NANDFLASHIoctl(unsigned long ulIoctl, unsigned long ulParam1,
               unsigned long ulParam2, unsigned long ulParam3,
               unsigned long ulParam4)
{
    unsigned long * volatile pulGPIO = (unsigned long *)HwBaseAddress;
    unsigned long ulCS, ulBusyFlag;

    //
    // If the first on-board NAND could be busy, then wait until it is not.
    //
    if(sNAND.ulFlags & FLAG_IS_BUSY1)
    {
        //
        // Select the first on-board NAND.
        //
        pulGPIO[HwPortABCD >> 2] &= ~HwPortABCD_NAND1_CS;

        //
        // Wait until the first on-board NAND is not busy.
        //
        NANDWaitTilNotBusy(HwNANDAddress);

        //
        // Indicate that the first on-board NAND is not busy.
        //
        sNAND.ulFlags &= ~FLAG_IS_BUSY1;

        //
        // Deselect the first on-board NAND.
        //
        pulGPIO[HwPortABCD >> 2] |= HwPortABCD_NAND1_CS;
    }

    //
    // If the second on-board NAND could be busy, then wait until it is not.
    //
#ifdef HwPortABCD_NAND2_CS
    if(sNAND.ulFlags & FLAG_IS_BUSY2)
    {
        //
        // Select the second on-board NAND.
        //
        pulGPIO[HwPortABCD >> 2] &= ~HwPortABCD_NAND2_CS;

        //
        // Wait until the second on-board NAND is not busy.
        //
        NANDWaitTilNotBusy(HwNANDAddress);

        //
        // Indicate that the second on-board NAND is not busy.
        //
        sNAND.ulFlags &= ~FLAG_IS_BUSY2;

        //
        // Deselect the second on-board NAND.
        //
        pulGPIO[HwPortABCD >> 2] |= HwPortABCD_NAND2_CS;
    }
#endif

    //
    // Determine which of the on-board NAND FLASH devices to select.
    //
#ifdef HwPortABCD_NAND2_CS
    if(sNAND.ulFlags & FLAG_ID_MASK)
    {
        //
        // The on-board NAND FLASH device to select is dependent upon the
        // IOCTL being executed.
        //
        switch(ulIoctl)
        {
            //
            // Handle the IOCTLs which work on a page basis.
            //
            case IOCTL_FLASH_READ:
            case IOCTL_FLASH_WRITE:
            case IOCTL_FLASH_READ_REDT:
            case IOCTL_FLASH_WRITE_REDT:
            {
                unsigned long ulNumPages;

                //
                // Determine the number of pages in each of the NAND FLASH
                // devices.
                //
                switch(sNAND.ulFlags & FLAG_ID_MASK)
                {
                    //
                    // A pair of 16MB NAND FLASH devices have 32768 pages in
                    // each device.
                    //
                    case 0x75:
                    {
                        ulNumPages = 32768;
                        break;
                    }

                    //
                    // A pair of 32MB NAND FLASH devices have 65536 pages in
                    // each device.  Also, each individual device is accessed
                    // with only three address cycles.
                    //
                    case 0x76:
                    {
                        ulNumPages = 65536;
                        ulParam4 &= ~FLASH_ADDR_4_CYCLE;
                        break;
                    }

                    //
                    // A pair of 64MB NAND FLASH devices have 131072 pages in
                    // each device.
                    //
                    case 0x79:
                    {
                        ulNumPages = 131072;
                        break;
                    }
                }

                //
                // Determine the NAND FLASH device to select based on the page
                // number being accessed.
                //
                if(ulParam1 < ulNumPages)
                {
                    ulCS = HwPortABCD_NAND1_CS;
                    ulBusyFlag = FLAG_IS_BUSY1;
                }
                else
                {
                    ulParam1 -= ulNumPages;
                    ulCS = HwPortABCD_NAND2_CS;
                    ulBusyFlag = FLAG_IS_BUSY2;
                }

                //
                // We've determined the correct device.
                //
                break;
            }

            //
            // Handle the IOCTLs which work on a block basis.
            //
            case IOCTL_FLASH_ERASE:
            {
                unsigned long ulNumBlocks;

                //
                // Determine the number of blocks in each of the NAND FLASH
                // devices.
                //
                switch(sNAND.ulFlags & FLAG_ID_MASK)
                {
                    //
                    // A pair of 16MB NAND FLASH devices have 1024 blocks in
                    // each device.
                    //
                    case 0x75:
                    {
                        ulNumBlocks = 1024;
                        break;
                    }

                    //
                    // A pair of 32MB NAND FLASH devices have 2048 blocks in
                    // each device.  Also, each individual device is accessed
                    // with only three address cycles.
                    //
                    case 0x76:
                    {
                        ulNumBlocks = 2048;
                        ulParam4 &= ~FLASH_ADDR_4_CYCLE;
                        break;
                    }

                    //
                    // A pair of 64MB NAND FLASH devices have 4096 blocks in
                    // each device.
                    //
                    case 0x79:
                    {
                        ulNumBlocks = 4096;
                        break;
                    }
                }

                //
                // Determine the NAND FLASH device to select based on the block
                // number being accessed.
                //
                if(ulParam1 < ulNumBlocks)
                {
                    ulCS = HwPortABCD_NAND1_CS;
                    ulBusyFlag = FLAG_IS_BUSY1;
                }
                else
                {
                    ulParam1 -= ulNumBlocks;
                    ulCS = HwPortABCD_NAND2_CS;
                    ulBusyFlag = FLAG_IS_BUSY2;
                }

                //
                // We've determined the correct device.
                //
                break;
            }

            //
            // Handle all other IOCTLs.
            //
            default:
            {
                //
                // Do not assert a chip select for all other IOCTLs.
                //
                ulCS = 0;

                //
                // Not not set a busy flag for all other IOCTLs.
                //
                ulBusyFlag = 0;

                //
                // We've determined the correct device.
                //
                break;
            }
        }
    }
    else
#endif
    {
        //
        // Get the chip select for the first on-board NAND.
        //
        ulCS = HwPortABCD_NAND1_CS;

        //
        // Get the busy flag for the first on-board NAND.
        //
        ulBusyFlag = FLAG_IS_BUSY1;
    }

    //
    // Select the on-board NAND.
    //
    pulGPIO[HwPortABCD >> 2] &= ~ulCS;

    //
    // Determine what to do based on the requested IOCTL.
    //
    switch(ulIoctl)
    {
        //
        // Get the ID of the on-board NAND.
        //
        case IOCTL_FLASH_GETID:
        {
            unsigned long *pulDeviceID = (unsigned long *)ulParam1;

            //
            // If we are faking a larger NAND FLASH, then return the faked ID.
            //
            if(sNAND.ulFlags & FLAG_ID_MASK)
            {
                //
                // Return the faked ID.
                //
                *pulDeviceID = sNAND.ulFlags & FLAG_ID_MASK;
            }
            else
            {
                //
                // Read the device ID from the on-board NAND.
                //
                *pulDeviceID = NANDGetID(HwNANDAddress);
            }

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Read a page of data from the on-board NAND.
        //
        case IOCTL_FLASH_READ:
        {
            //
            // Read the page of data from the on-board NAND.
            //
#ifdef SUPPORT_TINY_MEDIA
            if(ulParam4 & FLASH_PS256)
            {
                //
                // This NAND device has 256 bytes per page.
                //
                NANDRead_256(HwNANDAddress, ulParam1 << 1,
                             (unsigned char *)ulParam2,
                             (unsigned char *)ulParam3);
            }
            else
#endif
            if(ulParam4 & FLASH_ADDR_4_CYCLE)
            {
                //
                // This NAND device has 512 bytes per page and requires 4
                // address cycles.
                //
                NANDRead_512_4(HwNANDAddress, ulParam1,
                               (unsigned char *)ulParam2,
                               (unsigned char *)ulParam3);
            }
            else
            {
                //
                // This NAND device has 512 bytes per page and requires 3
                // address cycles.
                //
                NANDRead_512_3(HwNANDAddress, ulParam1,
                               (unsigned char *)ulParam2,
                               (unsigned char *)ulParam3);
            }

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Write a page of data to the on-board NAND.
        //
        case IOCTL_FLASH_WRITE:
        {
            //
            // Write the page of data to the on-board NAND.
            //
#ifdef SUPPORT_TINY_MEDIA
            if(ulParam4 & FLASH_PS256)
            {
                //
                // This NAND device has 256 bytes per page.
                //
                NANDWrite_256(HwNANDAddress, ulParam1 << 1,
                              (unsigned char *)ulParam2,
                              (unsigned char *)ulParam3);
                NANDWrite_256(HwNANDAddress, (ulParam1 << 1) + 1,
                              (unsigned char *)ulParam2 + 256,
                              (unsigned char *)ulParam3 + 8);
            }
            else
#endif
            if(ulParam4 & FLASH_ADDR_4_CYCLE)
            {
                //
                // This NAND device has 512 bytes per page and requires 4
                // address cycles.
                //
                NANDWrite_512_4(HwNANDAddress, ulParam1,
                                (unsigned char *)ulParam2,
                                (unsigned char *)ulParam3);
            }
            else
            {
                //
                // This NAND device has 512 bytes per page and requires 3
                // address cycles.
                //
                NANDWrite_512_3(HwNANDAddress, ulParam1,
                                (unsigned char *)ulParam2,
                                (unsigned char *)ulParam3);
            }

            //
            // Indicate that the on-board NAND is busy.
            //
            sNAND.ulFlags |= ulBusyFlag;

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Erase a block of the on-board NAND.
        //
        case IOCTL_FLASH_ERASE:
        {
            //
            // Erase this block of the on-board NAND.
            //
            if(ulParam2 & FLASH_BS16)
            {
                //
                // This NAND device has 16 pages per block.
                //
                NANDErase_16(HwNANDAddress, ulParam1);
            }
            else if(ulParam2 & FLASH_ADDR_4_CYCLE)
            {
                //
                // This NAND device has 32 pages per block and requires 4
                // address cycles.
                //
                NANDErase_32_4(HwNANDAddress, ulParam1);
            }
            else
            {
                //
                // This NAND device has 32 pages per block and requires 3
                // address cycles.
                //
                NANDErase_32_3(HwNANDAddress, ulParam1);
            }

            //
            // Indicate that the on-board NAND is busy.
            //
            sNAND.ulFlags |= ulBusyFlag;

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Read the redundant data from a page of the on-board NAND.
        //
        case IOCTL_FLASH_READ_REDT:
        {
            //
            // Read the redundant data from the on-board NAND.
            //
#ifdef SUPPORT_TINY_MEDIA
            if(ulParam3 & FLASH_PS256)
            {
                //
                // This NAND device has 256 bytes per page.
                //
                NANDReadRedt_256(HwNANDAddress, ulParam1 << 1,
                                 (unsigned char *)ulParam2);
            }
            else
#endif
            if(ulParam3 & FLASH_ADDR_4_CYCLE)
            {
                //
                // This NAND device has 512 bytes per page and requires 4
                // address cycles.
                //
                NANDReadRedt_512_4(HwNANDAddress, ulParam1,
                                   (unsigned char *)ulParam2);
            }
            else
            {
                //
                // This NAND device has 512 bytes per page and requires 3
                // address cycles.
                //
                NANDReadRedt_512_3(HwNANDAddress, ulParam1,
                                   (unsigned char *)ulParam2);
            }

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Write the redundant data to a page of the on-board NAND.
        //
        case IOCTL_FLASH_WRITE_REDT:
        {
            //
            // Write the redundant data to the on-board NAND.
            //
#ifdef SUPPORT_TINY_MEDIA
            if(ulParam3 & FLASH_PS256)
            {
                //
                // This NAND device has 256 bytes per page.
                //
                NANDWriteRedt_256(HwNANDAddress, ulParam1 << 1,
                                  (unsigned char *)ulParam2);
                NANDWriteRedt_256(HwNANDAddress, (ulParam1 << 1) + 1,
                                  (unsigned char *)ulParam2 + 8);
            }
            else
#endif
            if(ulParam3 & FLASH_ADDR_4_CYCLE)
            {
                //
                // This NAND device has 512 bytes per page and requires 4
                // address cycles.
                //
                NANDWriteRedt_512_4(HwNANDAddress, ulParam1,
                                    (unsigned char *)ulParam2);
            }
            else
            {
                //
                // This NAND device has 512 bytes per page and requires 3
                // address cycles.
                //
                NANDWriteRedt_512_3(HwNANDAddress, ulParam1,
                                    (unsigned char *)ulParam2);
            }

            //
            // Indicate that the on-board NAND is busy.
            //
            sNAND.ulFlags |= ulBusyFlag;

            //
            // We're done with this request.
            //
            break;
        }

        //
        // We should never get here, but just in case...
        //
        default:
        {
            //
            // Deselect the on-board NAND.
            //
            pulGPIO[HwPortABCD >> 2] |= ulCS;

            //
            // Return a failure.
            //
            return(0);
        }
    }

    //
    // Deselect the on-board NAND.
    //
    pulGPIO[HwPortABCD >> 2] |= ulCS;

    //
    // Success.
    //
    return(1);
}

//****************************************************************************
//
// ConfigureNAND determines if we have a pair of NAND FLASH devices which can
// be treated as a single, larger NAND FLASH device.
//
//****************************************************************************
static void
ConfigureNAND(void)
{
#ifdef HwPortABCD_NAND2_CS
    unsigned long * volatile pulGPIO = (unsigned long *)HwBaseAddress;
    unsigned long ulDeviceID1, ulDeviceID2;

    //
    // Select the first on-board NAND FLASH.
    //
    pulGPIO[HwPortABCD >> 2] &= ~HwPortABCD_NAND1_CS;

    //
    // Get the device ID of the first NAND FLASH.
    //
    ulDeviceID1 = NANDGetID(HwNANDAddress);

    //
    // Deselect the first on-board NAND FLASH.
    //
    pulGPIO[HwPortABCD >> 2] |= HwPortABCD_NAND1_CS;

    //
    // Select the second on-board NAND FLASH.
    //
    pulGPIO[HwPortABCD >> 2] &= ~HwPortABCD_NAND2_CS;

    //
    // Get the device ID of the second NAND FLASH.
    //
    ulDeviceID2 = NANDGetID(HwNANDAddress);

    //
    // Deselect the second on-board NAND FLASH.
    //
    pulGPIO[HwPortABCD >> 2] |= HwPortABCD_NAND2_CS;

    //
    // We only support the case of two identical NAND FLASH devices.
    //
    if(ulDeviceID1 != ulDeviceID2)
    {
        return;
    }

    //
    // See if we can join the two NAND FLASH devices as a single, larger NAND
    // FLASH.
    //
    switch(ulDeviceID1)
    {
        //
        // A pair of 16MB NAND FLASH devices combining to form a single 32MB
        // NAND FLASH device.
        //
        case 0x73:
        {
            //
            // Save the faked device ID.
            //
            sNAND.ulFlags |= 0x75;

            //
            // We're done handling this configuration.
            //
            break;
        }

        //
        // A pair of 32MB NAND FLASH devices combining to form a single 64MB
        // NAND FLASH device.
        //
        case 0x75:
        {
            //
            // Save the faked device ID.
            //
            sNAND.ulFlags |= 0x76;

            //
            // We're done handling this configuration.
            //
            break;
        }

        //
        // A pair of 64MB NAND FLASH devices combining to form a single 128MB
        // NAND FLASH device.
        //
        case 0x76:
        {
            //
            // Save the faked device ID.
            //
            sNAND.ulFlags |= 0x79;

            //
            // We're done handling this configuration.
            //
            break;
        }
    }
#endif
}

//****************************************************************************
//
// The file system IOCTL entry point for the on-board NAND.
//
//****************************************************************************
unsigned long
NANDIoctl(unsigned char *pucScratch, unsigned char *pucWriteBuffer,
          unsigned long ulIoctl, unsigned long ulInstance,
          unsigned long ulParam1, unsigned long ulParam2)
{
    //
    // If we are being initialized, then we need to setup our global variables.
    //
    if(ulIoctl == IOCTL_FS_INIT)
    {
        //
        // Set the address of the FLASH driver to be used by the FTL layer.
        //
        sNAND.sFTL.pfnFlashDriver = NANDFLASHIoctl;

        //
        // Set the address of the FTL layer to be used by the FAT layer.
        //
        sNAND.sFAT.pfnBlockDriver = FTLIoctl;
        sNAND.sFAT.pvBlockDriverState = &sNAND.sFTL;

        //
        // Clear the state of the NAND FLASH driver's state flags.
        //
        sNAND.ulFlags = 0;

        //
        // See if we have a pair of NAND FLASH devices that we want to treat as
        // a single, larger NAND FLASH device.
        //
        ConfigureNAND();

        //
        // Call the FAT code.
        //
        if(FATIoctl(&sNAND.sFAT, pucScratch, pucWriteBuffer, IOCTL_FS_INIT,
                    ulInstance, ulParam1, ulParam2) == 0)
        {
            //
            // The FAT failed to initialize, so we need to format the on-board
            // NAND FLASH.
            //
            return(FATIoctl(&sNAND.sFAT, pucScratch, pucWriteBuffer,
                            IOCTL_FS_FORMAT, 0, 0, 0));
        }

        //
        // Success.
        //
        return(1);
    }

    //
    // If the name of the media is being requested, then return it.
    //
    if(ulIoctl == IOCTL_FS_GETMEDIANAME)
    {
        //
        // Return the name of the drive.
        //
        memcpy((void *)ulParam1, pusDriveName, sizeof(pusDriveName));

        //
        // Return the length of the drive name.
        //
        *((unsigned long *)ulParam2) = sizeof(pusDriveName);

        //
        // Success.
        //
        return(1);
    }

    //
    // Call the FAT code.
    //
    return(FATIoctl(&sNAND.sFAT, pucScratch, pucWriteBuffer, ulIoctl,
                    ulInstance, ulParam1, ulParam2));
}

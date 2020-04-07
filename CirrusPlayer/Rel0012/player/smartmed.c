//****************************************************************************
//
// SMARTMED.C - File system driver for the SmartMedia card.
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
// The persistent state of the SmartMedia driver.
//
//****************************************************************************
static struct
{
    //
    // The persistent state of the FTL layer used for the SmartMedia card.
    //
    tFTL sFTL;

    //
    // The persistent state of the FAT layer used for the SmartMedia card.
    //
    tFAT sFAT;

    //
    // Flags indicating the state of the SmartMedia FLASH driver.
    //
    unsigned long ulFlags;
} sSmartMedia;

//****************************************************************************
//
// The set of flags that indicate the state of the SmartMedia FLASH driver.
//
//****************************************************************************
#define FLAG_CARD_INSERTED                      0x00000001
#define FLAG_CARD_INITIALIZED                   0x00000002
#define FLAG_IS_BUSY                            0x80000000

//****************************************************************************
//
// The FLASH driver IOCTL entry point for the SmartMedia card.
//
//****************************************************************************
static unsigned long
SMFLASHIoctl(unsigned long ulIoctl, unsigned long ulParam1,
             unsigned long ulParam2, unsigned long ulParam3,
             unsigned long ulParam4)
{
    unsigned long * volatile pulGPIO = (unsigned long *)HwBaseAddress;

    //
    // Select the SmartMedia card.
    //
    pulGPIO[HwPortABCD >> 2] &= ~HwPortABCD_SmartMedia_CS;

    //
    // If the SmartMedia card could be busy, then wait until it is not.
    //
    if(sSmartMedia.ulFlags & FLAG_IS_BUSY)
    {
        //
        // Wait until the SmartMedia card is not busy.
        //
        NANDWaitTilNotBusy(HwNANDAddress);

        //
        // Indicate that the SmartMedia card is not busy.
        //
        DisableIRQ();
        sSmartMedia.ulFlags &= ~FLAG_IS_BUSY;
        EnableIRQ();
    }

    //
    // Determine what to do based on the requested IOCTL.
    //
    switch(ulIoctl)
    {
        //
        // Get the ID of the SmartMedia card.
        //
        case IOCTL_FLASH_GETID:
        {
            unsigned long *pulDeviceID = (unsigned long *)ulParam1;

            //
            // Read the device ID from the SmartMedia card.
            //
            *pulDeviceID = NANDGetID(HwNANDAddress);

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Read a page of data from the SmartMedia card.
        //
        case IOCTL_FLASH_READ:
        {
            //
            // Read the page of data from the SmartMedia card.
            //
#ifdef SUPPORT_TINY_MEDIA
            if(ulParam4 & FLASH_PS256)
            {
                //
                // This SmartMedia card has 256 bytes per page.
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
                // This SmartMedia card has 512 bytes per page and requires 4
                // address cycles.
                //
                NANDRead_512_4(HwNANDAddress, ulParam1,
                               (unsigned char *)ulParam2,
                               (unsigned char *)ulParam3);
            }
            else
            {
                //
                // This SmartMedia card has 512 bytes per page and requires 3
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
        // Write a page of data to the SmartMedia card.
        //
        case IOCTL_FLASH_WRITE:
        {
            //
            // Write the page of data to the SmartMedia card.
            //
#ifdef SUPPORT_TINY_MEDIA
            if(ulParam4 & FLASH_PS256)
            {
                //
                // This SmartMedia card has 256 bytes per page.
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
                // This SmartMedia card has 512 bytes per page and requires 4
                // address cycles.
                //
                NANDWrite_512_4(HwNANDAddress, ulParam1,
                                (unsigned char *)ulParam2,
                                (unsigned char *)ulParam3);
            }
            else
            {
                //
                // This SmartMedia card has 512 bytes per page and requires 3
                // address cycles.
                //
                NANDWrite_512_3(HwNANDAddress, ulParam1,
                                (unsigned char *)ulParam2,
                                (unsigned char *)ulParam3);
            }

            //
            // Indicate that the SmartMedia card is busy.
            //
            DisableIRQ();
            sSmartMedia.ulFlags |= FLAG_IS_BUSY;
            EnableIRQ();

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Erase a block of the SmartMedia card.
        //
        case IOCTL_FLASH_ERASE:
        {
            //
            // Erase this block of the SmartMedia card.
            //
            if(ulParam2 & FLASH_BS16)
            {
                //
                // This SmartMedia card has 16 pages per block.
                //
                NANDErase_16(HwNANDAddress, ulParam1);
            }
            else if(ulParam2 & FLASH_ADDR_4_CYCLE)
            {
                //
                // This SmartMedia card has 32 pages per block and requires 4
                // address cycles.
                //
                NANDErase_32_4(HwNANDAddress, ulParam1);
            }
            else
            {
                //
                // This SmartMedia card has 32 pages per block and requires 3
                // address cycles.
                //
                NANDErase_32_3(HwNANDAddress, ulParam1);
            }

            //
            // Indicate that the SmartMedia card is busy.
            //
            DisableIRQ();
            sSmartMedia.ulFlags |= FLAG_IS_BUSY;
            EnableIRQ();

            //
            // We're done with this request.
            //
            break;
        }

        //
        // Read the redundant data from a page of the SmartMedia card.
        //
        case IOCTL_FLASH_READ_REDT:
        {
            //
            // Read the redundant data from the SmartMedia card.
            //
#ifdef SUPPORT_TINY_MEDIA
            if(ulParam3 & FLASH_PS256)
            {
                //
                // This SmartMedia card has 256 bytes per page.
                //
                NANDReadRedt_256(HwNANDAddress, ulParam1 << 1,
                                 (unsigned char *)ulParam2);
            }
            else
#endif
            if(ulParam3 & FLASH_ADDR_4_CYCLE)
            {
                //
                // This SmartMedia card has 512 bytes per page and requires 4
                // address cycles.
                //
                NANDReadRedt_512_4(HwNANDAddress, ulParam1,
                                   (unsigned char *)ulParam2);
            }
            else
            {
                //
                // This SmartMedia card has 512 bytes per page and requires 3
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
        // Write the redundant data to a page of the SmartMedia card.
        //
        case IOCTL_FLASH_WRITE_REDT:
        {
            //
            // Write the redundant data to the SmartMedia card.
            //
#ifdef SUPPORT_TINY_MEDIA
            if(ulParam3 & FLASH_PS256)
            {
                //
                // This SmartMedia card has 256 bytes per page.
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
                // This SmartMedia card has 512 bytes per page and requires 4
                // address cycles.
                //
                NANDWriteRedt_512_4(HwNANDAddress, ulParam1,
                                    (unsigned char *)ulParam2);
            }
            else
            {
                //
                // This SmartMedia card has 512 bytes per page and requires 3
                // address cycles.
                //
                NANDWriteRedt_512_3(HwNANDAddress, ulParam1,
                                    (unsigned char *)ulParam2);
            }

            //
            // Indicate that the SmartMedia card is busy.
            //
            DisableIRQ();
            sSmartMedia.ulFlags |= FLAG_IS_BUSY;
            EnableIRQ();

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
            // Deselect the SmartMedia card.
            //
            pulGPIO[HwPortABCD >> 2] |= HwPortABCD_SmartMedia_CS;

            //
            // Return a failure.
            //
            return(0);
        }
    }

    //
    // Deselect the SmartMedia card.
    //
    pulGPIO[HwPortABCD >> 2] |= HwPortABCD_SmartMedia_CS;

    //
    // Success.
    //
    return(1);
}

//****************************************************************************
//
// The file system IOCTL entry point for the SmartMedia card.
//
//****************************************************************************
unsigned long
SMIoctl(unsigned char *pucScratch, unsigned char *pucWriteBuffer,
        unsigned long ulIoctl, unsigned long ulInstance,
        unsigned long ulParam1, unsigned long ulParam2)
{
    unsigned long * volatile pulPtr = (unsigned long *)HwBaseAddress;
    unsigned long ulRet;

    //
    // If we are being initialized, then we need to setup our global variables.
    //
    if(ulIoctl == IOCTL_FS_INIT)
    {
        //
        // Set the address of the FLASH driver to be used by the FTL layer.
        //
        sSmartMedia.sFTL.pfnFlashDriver = SMFLASHIoctl;

        //
        // Set the address of the FTL layer to be used by the FAT layer.
        //
        sSmartMedia.sFAT.pfnBlockDriver = FTLIoctl;
        sSmartMedia.sFAT.pvBlockDriverState = &sSmartMedia.sFTL;

        //
        // Clear the state of the SmartMedia FLASH driver's state flags.
        //
        sSmartMedia.ulFlags = 0;

#ifdef HwIrqSmartMediaInsert
        //
        // Enable the SmartMedia insert interrupt.
        //
        pulPtr[HwIntMask >> 2] |= HwIrqSmartMediaInsert;

        //
        // Success.
        //
        return(1);
#else
        //
        // Since we do not have an insert/remove interrupt, we simply assume
        // the card is inserted all the time.  This means that the card must be
        // inserted at bootup time and can never be removed (without a cold
        // boot).
        //
        sSmartMedia.ulFlags = FLAG_CARD_INSERTED | FLAG_CARD_INITIALIZED;
#endif
    }

    //
    // See if there is a SmartMedia card inserted.
    //
    if(!(sSmartMedia.ulFlags & FLAG_CARD_INSERTED))
    {
        //
        // See if this is a close request.
        //
        if((ulIoctl == IOCTL_FS_CLOSE) || (ulIoctl == IOCTL_FS_CLOSEDIR))
        {
            //
            // Return success.
            //
            return(1);
        }

        //
        // Fail all other requests.
        //
        return(0);
    }

    //
    // Power on the SmartMedia card.
    //
    pulPtr[HwPortABCD >> 2] &= ~HwPortABCD_SmartMedia_Power;

    //
    // If the SmartMedia card has not been initialized, then do so now.
    //
    if(!(sSmartMedia.ulFlags & FLAG_CARD_INITIALIZED))
    {
        //
        // Initiailize the FAT code.
        //
        if(FATIoctl(&sSmartMedia.sFAT, pucScratch, 0, IOCTL_FS_INIT, 0, 0,
                    0) == 0)
        {
            pulPtr[HwPortABCD >> 2] |= HwPortABCD_SmartMedia_Power;
            return(0);
        }

        //
        // Indicate that the card has been initialized.
        //
        sSmartMedia.ulFlags |= FLAG_CARD_INITIALIZED;
    }

    //
    // Call the FAT code.
    //
    ulRet = FATIoctl(&sSmartMedia.sFAT, pucScratch, pucWriteBuffer, ulIoctl,
                     ulInstance, ulParam1, ulParam2);

    //
    // If the SmartMedia card could be busy, then wait until it is not.
    //
    if(sSmartMedia.ulFlags & FLAG_IS_BUSY)
    {
        //
        // Select the SmartMedia card.
        //
        pulPtr[HwPortABCD >> 2] &= ~HwPortABCD_SmartMedia_CS;

        //
        // Wait until the SmartMedia card is not busy.
        //
        NANDWaitTilNotBusy(HwNANDAddress);

        //
        // Deselect the SmartMedia card.
        //
        pulPtr[HwPortABCD >> 2] |= HwPortABCD_SmartMedia_CS;

        //
        // Indicate that the SmartMedia card is not busy.
        //
        DisableIRQ();
        sSmartMedia.ulFlags &= ~FLAG_IS_BUSY;
        EnableIRQ();
    }

    //
    // Power off the SmartMedia card.
    //
    pulPtr[HwPortABCD >> 2] |= HwPortABCD_SmartMedia_Power;

    //
    // Return to the caller.
    //
    return(ulRet);
}

//****************************************************************************
//
// SMISR is the interrupt service routine that handles the SmartMedia
// insertion/removal interrupt.
//
//****************************************************************************
#ifdef HwIrqSmartMediaInsert
void
SMISR(void)
{
    unsigned long * volatile pulPtr = (unsigned long *)HwBaseAddress;
    unsigned long ulStatus;

    //
    // Get the current interrupt source status.
    //
    ulStatus = pulPtr[HwIntStatus >> 2] & pulPtr[HwIntMask >> 2];

    //
    // If the SmartMedia insert interrupt is asserted, then handle it now.
    //
    if(ulStatus & HwIrqSmartMediaInsert)
    {
        //
        // Indicate that we have a SmartMedia card inserted in the slot.
        //
        sSmartMedia.ulFlags |= FLAG_CARD_INSERTED;

        //
        // Mask the insert interrupt and unmask the remove interrupt so we can
        // detect a removal.
        //
        pulPtr[HwIntMask >> 2] ^= HwIrqSmartMediaInsert |
                                  HwIrqSmartMediaRemove;

        //
        // Indicate that a removeable media device has been inserted.
        //
        ulSystemFlags |= SYSFLAG_MEDIA_INSERT;
    }

    //
    // If the SmartMedia remove interrupt is asserted, then handle it now.
    //
    else if(ulStatus & HwIrqSmartMediaRemove)
    {
        //
        // The first thing we need to do is to turn off the power to the
        // SmartMedia card.
        //
        pulPtr[HwPortABCD >> 2] |= HwPortABCD_SmartMedia_Power;

        //
        // Indicate that we no longer have a SmartMedia card inserted in the
        // slot.
        //
        sSmartMedia.ulFlags &= ~(FLAG_CARD_INSERTED | FLAG_CARD_INITIALIZED);

        //
        // Mask the remove interrupt and unmask the insert interrupt so we can
        // detect an insert.
        //
        pulPtr[HwIntMask >> 2] ^= HwIrqSmartMediaInsert |
                                  HwIrqSmartMediaRemove;

        //
        // If there is a pending removeable media insert, then clear it.
        //
        if(ulSystemFlags & SYSFLAG_MEDIA_INSERT)
        {
            ulSystemFlags &= ~SYSFLAG_MEDIA_INSERT;
        }
        
        //
        // Otherwise, indicate that a removeable media device has been removed.
        //
        else
        {
            ulSystemFlags |= SYSFLAG_MEDIA_REMOVE;
        }
    }
}
#endif

//****************************************************************************
//
// SM

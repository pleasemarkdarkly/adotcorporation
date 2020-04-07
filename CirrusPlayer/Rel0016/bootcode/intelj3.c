//****************************************************************************
//
// INTELJ3.C - Routines for programming Intel J3 FLASH-es.
//
// Copyright (c) 2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include "../hwport.h"
#include "../hwdefs.h"

//****************************************************************************
//
// Only include these routines if we are using an Intel J3 FLASH.
//
//****************************************************************************
#ifdef FLASH_INTEL_J3

//****************************************************************************
//
// EraseFLASH erases the portion of the FLASH that will contain the newly
// downloaded image.
//
//****************************************************************************
void
EraseFLASH(unsigned long ulBase, unsigned long ulLength,
           unsigned long bIs16BitWide)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwProgramAddress;
    volatile unsigned short *pusPtr = (unsigned short *)HwProgramAddress;
    unsigned long ulSize, ulOffset;

    //
    // The size of the FLASH blocks is based on the width of the program FLASH.
    //
    if(bIs16BitWide)
    {
        ulSize = 0x20000;
    }
    else
    {
        ulSize = 0x40000;
    }

    //
    // Loop through the blocks that need to be erased.
    //
    for(ulOffset = 0; ulOffset <= ulLength; ulOffset += ulSize)
    {
        //
        // Erase this block.
        //
        if(bIs16BitWide)
        {
            //
            // Erase this block.
            //
#ifdef DATA_BUS_REVERSED
            pusPtr[(ulBase + ulOffset) >> 1] = 0x0400;
            pusPtr[(ulBase + ulOffset) >> 1] = 0x0b00;
#else
            pusPtr[(ulBase + ulOffset) >> 1] = 0x0020;
            pusPtr[(ulBase + ulOffset) >> 1] = 0x00d0;
#endif

            //
            // Wait until the erase has completed.
            //
#ifdef DATA_BUS_REVERSED
            while((*pusPtr & 0x0100) != 0x0100)
#else
            while((*pusPtr & 0x0080) != 0x0080)
#endif
            {
            }

            //
            // Clear the status register.
            //
#ifdef DATA_BUS_REVERSED
            *pusPtr = 0x0a00;
#else
            *pusPtr = 0x0050;
#endif
        }
        else
        {
            //
            // Erase this block.
            //
            pulPtr[(ulBase + ulOffset) >> 2] = 0x00200020;
            pulPtr[(ulBase + ulOffset) >> 2] = 0x00d000d0;

            //
            // Wait until the erase has completed.
            //
            while((*pulPtr & 0x00800080) != 0x00800080)
            {
            }

            //
            // Clear the status register.
            //
            *pulPtr = 0x00500050;
        }
    }

    //
    // Put the FLASH into read array mode.
    //
    if(bIs16BitWide)
    {
#ifdef DATA_BUS_REVERSED
        *pusPtr = 0xff00;
#else
        *pusPtr = 0x00ff;
#endif
    }
    else
    {
        *pulPtr = 0x00ff00ff;
    }
}

//****************************************************************************
//
// WriteFLASH writes a block of data into the FLASH.
//
//****************************************************************************
void
WriteFLASH(unsigned char *pucData, unsigned long ulOffset,
           unsigned long ulLength, unsigned long bIs16BitWide)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwProgramAddress;
    volatile unsigned short *pusPtr = (unsigned short *)HwProgramAddress;
    unsigned long ulData, ulIdx;
    unsigned short usData;

    //
    // Is the program FLASH 16 or 32 bits wide?
    //
    if(bIs16BitWide)
    {
        //
        // Loop through the entire bock of data to be programmed.
        //
        while(ulLength)
        {
            //
            // Prepare to program the next block of data.
            //
#ifdef DATA_BUS_REVERSED
            pusPtr[ulOffset >> 1] = 0x1700;
            pusPtr[ulOffset >> 1] = 0xf000;
#else
            pusPtr[ulOffset >> 1] = 0x00e8;
            pusPtr[ulOffset >> 1] = 0x000f;
#endif

            //
            // Write the next 16 half words to the FLASH.
            //
            for(ulIdx = 0; ulIdx < 16; ulIdx++)
            {
                //
                // Get the next half word to program.
                //
                usData = ((unsigned short *)pucData)[ulIdx];

                //
                // Write this half word to the FLASH.
                //
                pusPtr[(ulOffset >> 1) + ulIdx] = usData;
            }

            //
            // Tell the FLASH to program the block.
            //
#ifdef DATA_BUS_REVERSED
            pusPtr[ulOffset >> 1] = 0x0b00;
#else
            pusPtr[ulOffset >> 1] = 0x00d0;
#endif

            //
            // Wait until the program has completed.
            //
#ifdef DATA_BUS_REVERSED
            while((*pusPtr & 0x0100) != 0x0100)
#else
            while((*pusPtr & 0x0080) != 0x0080)
#endif
            {
            }

            //
            // Clear the status register.
            //
#ifdef DATA_BUS_REVERSED
            *pusPtr = 0x0a00;
#else
            *pusPtr = 0x0050;
#endif

            //
            // Skip to the next block.
            //
            pucData += 32;
            ulOffset += 32;
            ulLength -= 32;
        }

        //
        // Put the FLASH into read array mode.
        //
#ifdef DATA_BUS_REVERSED
        *pusPtr = 0xff00;
#else
        *pusPtr = 0x00ff;
#endif
    }
    else
    {
        //
        // Loop through the entire bock of data to be programmed.
        //
        while(ulLength)
        {
            //
            // Prepare to program the next block of data.
            //
            pulPtr[ulOffset >> 2] = 0x00e800e8;
            pulPtr[ulOffset >> 2] = 0x000f000f;

            //
            // Write the next 16 words to the FLASH.
            //
            for(ulIdx = 0; ulIdx < 16; ulIdx++)
            {
                //
                // Get the next word to program.
                //
                ulData = ((unsigned long *)pucData)[ulIdx];

                //
                // Write this word to the FLASH.
                //
                pulPtr[(ulOffset >> 2) + ulIdx] = ulData;
            }

            //
            // Tell the FLASH to program the block.
            //
            pulPtr[ulOffset >> 2] = 0x00d000d0;

            //
            // Wait until the program has completed.
            //
            while((*pulPtr & 0x00800080) != 0x00800080)
            {
            }

            //
            // Clear the status register.
            //
            *pulPtr = 0x00500050;

            //
            // Skip to the next half word.
            //
            pucData += 64;
            ulOffset += 64;
            ulLength -= 64;
        }

        //
        // Put the FLASH into read array mode.
        //
        *pulPtr = 0x00ff00ff;
    }
}
#endif

//****************************************************************************
//
// INTELB3.C - Routines for programming Intel B3 FLASH-es.
//
// Copyright (c) 2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include "../hwport.h"
#include "../hwdefs.h"

//****************************************************************************
//
// Only include these routines if we are using an Intel B3 FLASH.
//
//****************************************************************************
#ifdef FLASH_INTEL_B3

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
        ulSize = 0x10000;
    }
    else
    {
        ulSize = 0x20000;
    }

    //
    // Loop through the blocks that need to be erased.
    //
    for(ulOffset = 0; ulOffset <= ulLength; )
    {
        //
        // Erase this block.
        //
        if(bIs16BitWide)
        {
            //
            // Erase this block.
            //
            pusPtr[(ulBase + ulOffset) >> 1] = 0x0020;
            pusPtr[(ulBase + ulOffset) >> 1] = 0x00d0;

            //
            // Wait until the erase has completed.
            //
            while((*pusPtr & 0x0080) != 0x0080)
            {
            }

            //
            // Clear the status register.
            //
            *pusPtr = 0x0050;
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

        //
        // Advance to the next block.
        //
        if((ulBase + ulOffset) < ulSize)
        {
            ulOffset += ulSize >> 3;
        }
        else
        {
            ulOffset += ulSize;
        }
    }

    //
    // Put the FLASH into read array mode.
    //
    if(bIs16BitWide)
    {
        *pusPtr = 0x00ff;
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
    unsigned long ulData;
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
            // Get the next half word to program.
            //
            usData = *(unsigned short *)pucData;

            //
            // Program this half word.
            //
            pusPtr[ulOffset >> 1] = 0x0040;
            pusPtr[ulOffset >> 1] = usData;

            //
            // Wait until the program has completed.
            //
            while((*pusPtr & 0x0080) != 0x0080)
            {
            }

            //
            // Clear the status register.
            //
            *pusPtr = 0x0050;

            //
            // Skip to the next half word.
            //
            pucData += 2;
            ulOffset += 2;
            ulLength -= 2;
        }

        //
        // Put the FLASH into read array mode.
        //
        *pusPtr = 0x00ff;
    }
    else
    {
        //
        // Loop through the entire bock of data to be programmed.
        //
        while(ulLength)
        {
            //
            // Get the next word to program.
            //
            ulData = *(unsigned long *)pucData;

            //
            // Program this word.
            //
            pulPtr[ulOffset >> 2] = 0x00400040;
            pulPtr[ulOffset >> 2] = ulData;

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
            pucData += 4;
            ulOffset += 4;
            ulLength -= 4;
        }

        //
        // Put the FLASH into read array mode.
        //
        *pulPtr = 0x00ff00ff;
    }
}
#endif

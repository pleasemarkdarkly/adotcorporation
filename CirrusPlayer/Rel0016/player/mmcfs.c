//****************************************************************************
//
// MMCFS.C - File system driver for the external MMC FLASH.
//
// Copyright (c) 2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include "globals.h"
#include "../hwport.h"
#include "../hwdefs.h"
#include "mmc.h"

//****************************************************************************
//
// The following four values must be defined in the section of hwport.h for
// the selected board to tell the MMC code the GPIO being used for the CMD and
// DAT pins (this is dependent upon how the MMC card is wired into the board):
//
//     HwPortABCD_MMC_CMD - The bit in the HwPortABCD register for the MMC
//                          CMD pin.
//         - or -
//     HwPortE_MMC_CMD - The bit in the HwPortE register for the MMC CMD pin.
//
//     HwPortABCD_MMC_CMD_Shift - The number of bits to shift the HwPortABCD
//                                register value to get the MMC CMD bit in the
//                                least significant bit.
//         - or -
//     HwPortE_MMC_CMD_Shift - The number of bits to shift the HwPortE
//                             register value to get the MMC CMD bit in the
//                             least significant bit.
//
//     HwPortABCD_MMC_DAT - The bit in the HwPortABCD register for the MMC
//                          DAT pin.
//         - or -
//     HwPortE_MMC_DAT - The bit in the HwPortE register for the MMC DAT pin.
//
//     HwPortABCD_MMC_DAT_Shift - The number of bits to shift the HwPortABCD
//                                register value to get the MMC DAT bit in the
//                                least significant bit.
//         - or -
//     HwPortE_MMC_DAT_Shift - The number of bits to shift the HwPortE
//                             register value to get the MMC DAT bit in the
//                             least significant bit.
//
//****************************************************************************

//****************************************************************************
//
// Determine the register, bit, and shift required to access the command line
// of the MMC bus.
//
//****************************************************************************
#ifdef HwPortABCD_MMC_CMD
#define MMC_CMD_PORT                            HwPortABCD
#define MMC_CMD_DIR                             HwDdrABCD
#define MMC_CMD_BIT                             HwPortABCD_MMC_CMD
#define MMC_CMD_SHIFT                           HwPortABCD_MMC_CMD_Shift
#endif
#ifdef HwPortE_MMC_CMD
#define MMC_CMD_PORT                            HwPortE
#define MMC_CMD_DIR                             HwDdrE
#define MMC_CMD_BIT                             HwPortE_MMC_CMD
#define MMC_CMD_SHIFT                           HwPortE_MMC_CMD_Shift
#endif

//****************************************************************************
//
// Determine the register, bit, and shift required to access the data line of
// the MMC bus.
//
//****************************************************************************
#ifdef HwPortABCD_MMC_DAT
#define MMC_DAT_PORT                            HwPortABCD
#define MMC_DAT_DIR                             HwDdrABCD
#define MMC_DAT_BIT                             HwPortABCD_MMC_DAT
#define MMC_DAT_SHIFT                           HwPortABCD_MMC_DAT_Shift
#endif
#ifdef HwPortE_MMC_DAT
#define MMC_DAT_PORT                            HwPortE
#define MMC_DAT_DIR                             HwDdrE
#define MMC_DAT_BIT                             HwPortE_MMC_DAT
#define MMC_DAT_SHIFT                           HwPortE_MMC_DAT_Shift
#endif

//****************************************************************************
//
// Only include this code if we have MMC data and command lines defined.
//
//****************************************************************************
#if defined(MMC_DAT_PORT) && defined(MMC_CMD_PORT)

//****************************************************************************
//
// A macro to make the MMC command line an output.
//
//****************************************************************************
#if MMC_CMD_SHIFT >= 24
#define MMCCmdOutput            pulPtr[MMC_CMD_DIR >> 2] &= ~MMC_CMD_BIT
#else
#define MMCCmdOutput            pulPtr[MMC_CMD_DIR >> 2] |= MMC_CMD_BIT
#endif

//****************************************************************************
//
// A macro to make the MMC command line an input.
//
//****************************************************************************
#if MMC_CMD_SHIFT >= 24
#define MMCCmdInput             pulPtr[MMC_CMD_DIR >> 2] |= MMC_CMD_BIT
#else
#define MMCCmdInput             pulPtr[MMC_CMD_DIR >> 2] &= ~MMC_CMD_BIT
#endif

//****************************************************************************
//
// A macro to make the MMC data line an output.
//
//****************************************************************************
#if MMC_DAT_SHIFT >= 24
#define MMCDatOutput            pulPtr[MMC_DAT_DIR >> 2] &= ~MMC_DAT_BIT
#else
#define MMCDatOutput            pulPtr[MMC_DAT_DIR >> 2] |= MMC_DAT_BIT
#endif

//****************************************************************************
//
// A macro to make the MMC data line an input.
//
//****************************************************************************
#if MMC_DAT_SHIFT >= 24
#define MMCDatInput             pulPtr[MMC_DAT_DIR >> 2] |= MMC_DAT_BIT
#else
#define MMCDatInput             pulPtr[MMC_DAT_DIR >> 2] &= ~MMC_DAT_BIT
#endif

//****************************************************************************
//
// The name of these drives.
//
//****************************************************************************
static const unsigned short pusDriveName1[] =
{
    'M', 'u', 'l', 't', 'i', 'M', 'e', 'd', 'i', 'a', 'C', 'a',
    'r', 'd', ' ', '1', '\0'
};
static const unsigned short pusDriveName2[] =
{
    'M', 'u', 'l', 't', 'i', 'M', 'e', 'd', 'i', 'a', 'C', 'a',
    'r', 'd', ' ', '2', '\0'
};

//****************************************************************************
//
// The persistent state of the MMC driver.
//
//****************************************************************************
static struct
{
    //
    // The number of MMC cards found.
    //
    unsigned short usNumCards;

    //
    // The currently selected MMC card.
    //
    unsigned short usCurrent;

    //
    // The persistent state of the FAT layer used for the first MMC card.
    //
    tFAT sFAT1;

    //
    // The persistent state of the FAT layer used for the second MMC card.
    //
    tFAT sFAT2;
} sMMC;

//****************************************************************************
//
// MMCClock clocks the MMC bus the given number of times.
//
//****************************************************************************
static void
MMCClock(unsigned long ulCount)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwMMCAddress;

    //
    // Loop while there are more clocks cycles to create.
    //
    while(ulCount--)
    {
        //
        // Clock the MMC bus.
        //
        *pulPtr = 0;
    }
}

//****************************************************************************
//
// MMCGetCRC7 computes the CRC7 value for the given MMC command.
//
//****************************************************************************
static unsigned char
MMCGetCRC7(unsigned char *pucCommand)
{
    unsigned long ulByte, ulBit, ulValue;
    unsigned char ucCRC7 = 0;

    //
    // Loop through the five bytes in the command.
    //
    for(ulByte = 0; ulByte < 5; ulByte++)
    {
        //
        // Get this byte.
        //
        ulValue = pucCommand[ulByte];

        //
        // Loop through the eight bits in this byte.
        //
        for(ulBit = 0; ulBit < 8; ulBit++)
        {
            //
            // Shift the running CRC7 up by one bit.
            //
            ucCRC7 <<= 1;

            //
            // Compute the next iteration of the CRC7.
            //
            ucCRC7 ^= ((((ulValue << ulBit) ^ ucCRC7) & 0x80) ? 0x09 : 0x00);
        }
    }

    //
    // Return the computed CRC7 value.
    //
    return(ucCRC7 & 0x7f);
}

//****************************************************************************
//
// MMCGetCRC16 computes the CRC16 value for the data buffer.
//
//****************************************************************************
static unsigned short
MMCGetCRC16(unsigned long *pulBuffer, unsigned long bIsByteSwapped)
{
    unsigned long ulIdx, ulByte, ulBit, ulValue, ulWord;
    unsigned short usCRC16 = 0;

    //
    // Loop through the 128 words in the data buffer.
    //
    for(ulIdx = 0; ulIdx < 128; ulIdx++)
    {
        //
        // Get this word.
        //
        ulWord = pulBuffer[ulIdx];

        //
        // Byte swap this word if it is byte swapped in memory.
        //
        if(bIsByteSwapped)
        {
            ulWord = (((ulWord >> 24) & 0x000000ff) |
                      ((ulWord >> 8) & 0x0000ff00) |
                      ((ulWord << 8) & 0x00ff0000) |
                      ((ulWord << 24) & 0xff000000));
        }

        //
        // Loop through the four bytes in this word.
        //
        for(ulByte = 0; ulByte < 4; ulByte++)
        {
            //
            // Get this byte, shifting it up by eight bits so that it lines up
            // with the MSB of the running CRC16.
            //
            ulValue = (ulWord >> (ulByte * 8)) << 8;

            //
            // Loop through the eight bits in this byte.
            //
            for(ulBit = 0; ulBit < 8; ulBit++)
            {
                //
                // Compute the next iteration of the CRC16.
                //
                if((usCRC16 ^ (ulValue << ulBit)) & 0x8000)
                {
                    usCRC16 = (usCRC16 << 1) ^ 0x1021;
                }
                else
                {
                    usCRC16 <<= 1;
                }
            }
        }
    }

    //
    // Return the computed CRC16 value.
    //
    return(usCRC16);
}

//****************************************************************************
//
// MMCSendCommand constructs and sends a command to the MMC bus.
//
//****************************************************************************
static void
MMCSendCommand(unsigned char ucCmd, unsigned long ulArg)
{
    unsigned char ucCommand[6];

    //
    // Construct the MMC command packet.
    //
    ucCommand[0] = 0x40 | (ucCmd & 0x3f);
    ucCommand[1] = (ulArg & 0xff000000) >> 24;
    ucCommand[2] = (ulArg & 0x00ff0000) >> 16;
    ucCommand[3] = (ulArg & 0x0000ff00) >> 8;
    ucCommand[4] = ulArg & 0x000000ff;
    ucCommand[5] = (MMCGetCRC7(ucCommand) << 1) | 0x01;

    //
    // Send the command packet to the MMC bus.
    //
    MMCWriteCommand(ucCommand);
}

//****************************************************************************
//
// MMCInit initializes the MMC cards, performing isolation of the cards on the
// MMC bus and assigning unique relative addresses to each card found.
//
//****************************************************************************
static unsigned long
MMCInit(void)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;
    unsigned long ulAddress;
    unsigned char ucRes[18];

    //
    // Initially, there are no cards.
    //
    sMMC.usNumCards = 0;

    //
    // The maximum MMC clock during enumeration is 400kHz, so slow down the
    // clock to meet this spec.
    //
    pulPtr[HwMMCMemConfig >> 2] &= ~HwMMCMemValue;

    //
    // Clock the MMC bus for a bit to clear out any residual state.
    //
    MMCClock(100);

    //
    // Put the MMC cards into idle mode.
    //
    MMCSendCommand(GO_IDLE_STATE, 0x00000000);

    //
    // Clock the MMC bus for a bit.
    //
    MMCClock(60);

    //
    // Inform the MMC cards of the operating voltage.
    //
    do
    {
        //
        // Tell the MMC cards that they will be operating in the 3.1V to 3.2V
        // range.
        //
        MMCSendCommand(SEND_OP_COND, OCR_31_32);
        if(MMCReadResponse(ucRes, 6) == 0)
        {
            //
            // No response was read, so there are no MMC cards.
            //
            return(0);
        }

        //
        // Clock the MMC bus.
        //
        MMCClock(8);
    }
    while((ucRes[1] & 0x80) == 0);

    //
    // Clock the MMC bus.
    //
    MMCClock(100);

    //
    // Set the address of the first MMC card found.
    //
    ulAddress = 0x00010000;

    //
    // Get the CIDs until we've found all the MMC cards.
    //
    while(1)
    {
        //
        // Get the CID from the MMC card(s).
        //
        MMCSendCommand(ALL_SEND_CID, 0x00000000);

        //
        // Read back the CID.
        //
        if(MMCReadResponse(ucRes, 17) == 0)
        {
            break;
        }

        //
        // Clock the MMC bus.
        //
        MMCClock(8);

        //
        // Set the relative address of this MMC card.
        //
        MMCSendCommand(SET_RELATIVE_ADDR, ulAddress);
        if(MMCReadResponse(ucRes, 6) == 0)
        {
            break;
        }

        //
        // Clock the MMC bus.
        //
        MMCClock(8);

        //
        // Select this MMC card.
        //
        MMCSendCommand(SELECT_CARD, ulAddress);
        if(MMCReadResponse(ucRes, 6) == 0)
        {
            break;
        }

        //
        // Clock the MMC bus.
        //
        MMCClock(8);

        //
        // Set the block length to 512 bytes.
        //
        MMCSendCommand(SET_BLOCKLEN, 512);
        if(MMCReadResponse(ucRes, 6) == 0)
        {
            break;
        }

        //
        // Clock the MMC bus.
        //
        MMCClock(8);

        //
        // Increment the address.
        //
        ulAddress += 0x00010000;

        //
        // Increment the number of cards found.
        //
        sMMC.usNumCards++;
    }

    //
    // Clock the MMC bus.
    //
    MMCClock(8);

    //
    // Speed up the MMC clock.
    //
    pulPtr[HwMMCMemConfig >> 2] |= HwMMCMemValue;

    //
    // Success.
    //
    return(1);
}

//****************************************************************************
//
// MMCSelectCardOne selects the first MMC card found during the enumeration
// process.  Selecting the card a second time will de-select it, which must
// be done before another card can be selected.
//
//****************************************************************************
static void
MMCSelectCardOne(void)
{
    unsigned char ucRes[6];

    //
    // Activate the first card found.
    //
    MMCSendCommand(SELECT_CARD, 0x00010000);
    MMCReadResponse(ucRes, 6);

    //
    // Clock the MMC bus.
    //
    MMCClock(8);
}

//****************************************************************************
//
// MMCSelectCardTwo selects the second MMC card found during the enumeration
// process.  Selecting the card a second time will de-select it, which must
// be done before another card can be selected.
//
//****************************************************************************
static void
MMCSelectCardTwo(void)
{
    unsigned char ucRes[6];

    //
    // Activate the first card found.
    //
    MMCSendCommand(SELECT_CARD, 0x00020000);
    if(MMCReadResponse(ucRes, 6) == 0)
    {
        return;
    }

    //
    // Clock the MMC bus.
    //
    MMCClock(8);
}

//****************************************************************************
//
// MMCRead reads one or more pages from the MMC card, performing byte swapping
// if necessary.
//
//****************************************************************************
static unsigned long
MMCRead(unsigned long ulStart, unsigned long ulCount,
        unsigned long *pulBuffer, unsigned long bByteSwap)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;
    unsigned char ucRes[6];
    unsigned long ulData = 0, ulLoop, ulCRC = 0;

    //
    // See if we are to read only one page.
    //
    if(ulCount == 1)
    {
        //
        // Configure the data line as an output.
        //
        MMCDatOutput;

        //
        // Tell the MMC card that we want to read a single block.
        //
        MMCSendCommand(READ_SINGLE_BLOCK, ulStart * 512);

        //
        // Read back the response.
        //
        if(MMCReadResponse(ucRes, 6) == 0)
        {
            return(0);
        }

        //
        // Configure the data line as an input.
        //
        MMCDatInput;

        //
        // Clock the MMC bus until the card is ready to send back the data (as
        // indicated by it pulling the data line low).
        //
        do
        {
            MMCClock(1);
        }
        while((pulPtr[MMC_DAT_PORT >> 2] & MMC_DAT_BIT) != 0);

        //
        // See if the data should be byte swapped as it is read.
        //
        if(bByteSwap)
        {
            //
            // Loop through the 128 words (512 bytes) of this block.
            //
            for(ulLoop = 0; ulLoop < 128; ulLoop++)
            {
                //
                // Read four bytes, byte swapping them and packing them into a
                // word.
                //
                ulData = (unsigned long)MMCReadByte() << 24;
                ulData |= (unsigned long)MMCReadByte() << 16;
                ulData |= (unsigned long)MMCReadByte() << 8;
                ulData |= (unsigned long)MMCReadByte();

                //
                // Write this word to the data buffer.
                //
                pulBuffer[ulLoop] = ulData;
            }

            //
            // Compute the CRC16 of this block.
            //
            ulCRC = MMCGetCRC16(pulBuffer, 1);
        }
        else
        {
            //
            // Loop through the 128 words (512 bytes) of this block.
            //
            for(ulLoop = 0; ulLoop < 128; ulLoop++)
            {
                //
                // Read four bytes, packing them into a word.
                //
                ulData = (unsigned long)MMCReadByte();
                ulData |= (unsigned long)MMCReadByte() << 8;
                ulData |= (unsigned long)MMCReadByte() << 16;
                ulData |= (unsigned long)MMCReadByte() << 24;

                //
                // Write this word to the data buffer.
                //
                pulBuffer[ulLoop] = ulData;
            }

            //
            // Compute the CRC16 of this block.
            //
            ulCRC = MMCGetCRC16(pulBuffer, 0);
        }

        //
        // Read the CRC from the card.
        //
        ulData = (unsigned long)MMCReadByte() << 8;
        ulData |= (unsigned long)MMCReadByte();

        //
        // Clock the MMC bus.
        //
        MMCClock(9);

        //
        // See if the CRC matched.
        //
        if(ulCRC != ulData)
        {
            //
            // The CRC did not match, so return a failure.
            //
            return(0);
        }

        //
        // Success.
        //
        return(1);
    }
    else
    {
        //
        // Configure the data line as an output.
        //
        MMCDatOutput;

        //
        // Tell the MMC card that we want to start reading a set of sequential
        // blocks.
        //
        MMCSendCommand(READ_MULTIPLE_BLOCK, ulStart * 512);

        //
        // Read back the response.
        //
        if(MMCReadResponse(ucRes, 6) == 0)
        {
            return(0);
        }

        //
        // Configure the data line as an input.
        //
        MMCDatInput;

        //
        // Loop through the requested number of blocks.
        //
        while(ulCount--)
        {
            //
            // Clock the MMC bus until the card is ready to send back the data
            // (as indicated by it pulling the data line low).
            //
            do
            {
                MMCClock(1);
            }
            while((pulPtr[MMC_DAT_PORT >> 2] & MMC_DAT_BIT) != 0);

            //
            // See if the data should be byte swapped as it is read.
            //
            if(bByteSwap)
            {
                //
                // Loop through the 128 words (512 bytes) of this block.
                //
                for(ulLoop = 0; ulLoop < 128; ulLoop++)
                {
                    //
                    // Read four bytes, byte swapping them and packing them
                    // into a word.
                    //
                    ulData = (unsigned long)MMCReadByte() << 24;
                    ulData |= (unsigned long)MMCReadByte() << 16;
                    ulData |= (unsigned long)MMCReadByte() << 8;
                    ulData |= (unsigned long)MMCReadByte();

                    //
                    // Write this word to the data buffer.
                    //
                    pulBuffer[ulLoop] = ulData;
                }

                //
                // Compute the CRC16 of this block.
                //
                ulCRC = MMCGetCRC16(pulBuffer, 1);
            }
            else
            {
                //
                // Loop through the 128 words (512 bytes) of this block.
                //
                for(ulLoop = 0; ulLoop < 128; ulLoop++)
                {
                    //
                    // Read four bytes, packing them into a word.
                    //
                    ulData = (unsigned long)MMCReadByte();
                    ulData |= (unsigned long)MMCReadByte() << 8;
                    ulData |= (unsigned long)MMCReadByte() << 16;
                    ulData |= (unsigned long)MMCReadByte() << 24;

                    //
                    // Write this word to the data buffer.
                    //
                    pulBuffer[ulLoop] = ulData;
                }

                //
                // Compute the CRC16 of this block.
                //
                ulCRC = MMCGetCRC16(pulBuffer, 0);
            }

            //
            // Read the CRC from the card.
            //
            ulData = (unsigned long)MMCReadByte() << 8;
            ulData |= (unsigned long)MMCReadByte();

            //
            // See if the CRC matched.
            //
            if(ulCRC != ulData)
            {
                //
                // The CRC did not match, so break out of this loop.
                //
                break;
            }

            //
            // Skip to the next portion of the data buffer.
            //
            pulBuffer += 128;
        }

        //
        // Configure the data line as an output.
        //
        MMCDatOutput;

        //
        // Tell the MMC card that we are done reading data.
        //
        MMCSendCommand(STOP_TRANSMISSION, 0);

        //
        // Read back the response.
        //
        if(MMCReadResponse(ucRes, 6) == 0)
        {
            return(0);
        }

        //
        // Clock the MMC bus.
        //
        MMCClock(8);

        //
        // See if the CRC matched.
        //
        if(ulCRC != ulData)
        {
            //
            // The CRC did not match, so return a failure.
            //
            return(0);
        }

        //
        // Success.
        //
        return(1);
    }
}

//****************************************************************************
//
// MMCWrite writes one or more pages to the MMC card.
//
//****************************************************************************
static unsigned long
MMCWrite(unsigned long ulStart, unsigned long ulCount,
         unsigned long *pulBuffer)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;
    unsigned char ucRes[6];
    unsigned long ulData, ulLoop;

    //
    // Loop through the requested number of blocks.
    //
    while(ulCount--)
    {
        //
        // Configure the data line as an output.
        //
        MMCDatOutput;

        //
        // Tell the MMC card that we want to write a single page.
        //
        MMCSendCommand(WRITE_BLOCK, ulStart * 512);

        //
        // Read back the response.
        //
        if(MMCReadResponse(ucRes, 6) == 0)
        {
            return(0);
        }

        //
        // Check the response to see that the card is ready to write a page.
        //
        if((ucRes[3] & 0x01) == 0)
        {
            MMCClock(8);
            return(0);
        }

        //
        // The card is expecting at least two clocks, and then will start
        // clocking in data the bit after it sees the first zero bit.
        // Therefore, send 0xFE to generate 7 clocks and then a start bit.
        //
        MMCWriteByte(0xfe);

        //
        // Loop through the 128 words (512 bytes) of this page.
        //
        for(ulLoop = 0; ulLoop < 128; ulLoop++)
        {
            //
            // Get the next word.
            //
            ulData = pulBuffer[ulLoop];

            //
            // Write the four bytes of this word.
            //
            MMCWriteByte(ulData & 0x000000ff);
            MMCWriteByte((ulData & 0x0000ff00) >> 8);
            MMCWriteByte((ulData & 0x00ff0000) >> 16);
            MMCWriteByte((ulData & 0xff000000) >> 24);
        }

        //
        // Compute the CRC16 for this block.
        //
        ulData = MMCGetCRC16(pulBuffer, 0);

        //
        // Send the CRC16 to the MMC card.
        //
        MMCWriteByte((ulData & 0x0000ff00) >> 8);
        MMCWriteByte(ulData & 0x000000ff);

        //
        // Send a single one bit the MMC card.
        //
        pulPtr[MMC_DAT_PORT >> 2] |= MMC_DAT_BIT;
        MMCClock(1);

        //
        // Configure the data line as an input.
        //
        MMCDatInput;

        //
        // Wait for the start of the CRC status.
        //
        while((pulPtr[MMC_DAT_PORT >> 2] & MMC_DAT_BIT) != 0)
        {
            //
            // Clock the MMC bus.
            //
            MMCClock(1);
        }

        //
        // Loop through the four bits of the CRC status.
        //
        for(ulLoop = 0, ulData = 0; ulLoop < 4; ulLoop++)
        {
            //
            // Clock the MMC bus.
            //
            MMCClock(1);

            //
            // See if the data line is high.
            //
            if((pulPtr[MMC_DAT_PORT >> 2] & MMC_DAT_BIT) != 0)
            {
                ulData |= 0x08 >> ulLoop;
            }
        }

        //
        // Continue to clock the MMC bus until the data line goes high.
        //
        MMCClock(1);
        while((pulPtr[MMC_DAT_PORT >> 2] & MMC_DAT_BIT) == 0)
        {
            MMCClock(100);
        }

        //
        // Check the CRC status.
        //
        if(ulData != 0x05)
        {
            //
            // The CRC status was bad, so return a failure.
            //
            return(0);
        }

        //
        // Advance to the next page.
        //
        ulStart++;

        //
        // Skip to the next portion of the data buffer.
        //
        pulBuffer += 128;
    }

    //
    // Success.
    //
    return(1);
}

//****************************************************************************
//
// MMCGetCapacity determines the capacity of the MMC card.
//
//****************************************************************************
unsigned long
MMCGetCapacity(void)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;
    unsigned long ulLength, ulSize, ulSizeMult;
    unsigned char ucRes[18];

    //
    // Configure the data line as an output.
    //
    MMCDatOutput;

    //
    // Deselect all the cards.
    //
    MMCSendCommand(SELECT_CARD, 0x00000000);

    //
    // Read back the response.
    //
    MMCReadResponse(ucRes, 6);

    //
    // Clock the MMC bus.
    //
    MMCClock(8);

    //
    // Tell the MMC card that we want to read it's card specific data (CSD)
    // register.
    //
    MMCSendCommand(SEND_CSD, (sMMC.usCurrent == 0) ? 0x00010000 : 0x00020000);

    //
    // Read back the CSD.
    //
    ulSize = MMCReadResponse(ucRes, 17);

    //
    // Clock the MMC bus.
    //
    MMCClock(8);

    //
    // Reselect the current card.
    //
    MMCSendCommand(SELECT_CARD,
                   (sMMC.usCurrent == 0) ? 0x00010000 : 0x00020000);

    //
    // Read back the response.
    //
    MMCReadResponse(ucRes, 6);

    //
    // Clock the MMC bus.
    //
    MMCClock(8);

    //
    // Make sure that the MMC card sent its CSD register.
    //
    if(ulSize == 0)
    {
        //
        // The MMC card did not send its CSD register, so return the capacity
        // as zero.
        //
        return(0);
    }

    //
    // Extract the read block length field from the CSD.
    //
    ulLength = 1 << (ucRes[6] & 0x0F);

    //
    // Extract the device size field from the CSD.
    //
    ulSize = (((ucRes[7] & 0x03) << 10) + (ucRes[8] << 2) +
              ((ucRes[9] & 0xc0) >> 6) + 1);

    //
    // Extract the device size multiplier field from the CSD.
    //
    ulSizeMult = 4 << (((ucRes[10] & 0x3) << 1) | ((ucRes[11] & 0x80) >> 7));

    //
    // Return the capacity of this device.
    //
    return(ulLength * ulSize * ulSizeMult);
}

//****************************************************************************
//
// The block driver for the MMC card.
//
//****************************************************************************
unsigned long
MMCBlockIoctl(void *pvState, unsigned char *pucScratch, unsigned long ulIoctl,
              unsigned long ulParam1, unsigned long ulParam2,
              unsigned long ulParam3)
{
    //
    // Determine what to do based on the requested IOCTL.
    //
    switch(ulIoctl)
    {
        //
        // Initialize the block driver.
        //
        case IOCTL_BLOCK_INIT:
        {
            //
            // There is nothing to be done, so return success.
            //
            return(1);
        }

        //
        // Return the media unique ID.
        //
        case IOCTL_BLOCK_GETMEDIAID:
        {
            //
            // MMC cards do not support a media unique ID, so return a
            // failure.
            //
            return(0);
        }

        //
        // Read a block (or blocks) from the media.
        //
        case IOCTL_BLOCK_READ:
        {
            //
            // Read from the MMC card.
            //
            return(MMCRead(ulParam1, ulParam2, (unsigned long *)ulParam3, 0));
        }

        //
        // Read a block (or blocks) from the media with byte swapping.
        //
        case IOCTL_BLOCK_READ_BS:
        {
            //
            // Read from the MMC card.
            //
            return(MMCRead(ulParam1, ulParam2, (unsigned long *)ulParam3, 1));
        }

        //
        // Write a block (or blocks) to the media.
        //
        case IOCTL_BLOCK_WRITE:
        {
            //
            // Write to the MMC card.
            //
            return(MMCWrite(ulParam1, ulParam2, (unsigned long *)ulParam3));
        }

        //
        // Return the capacity of the media.
        //
        case IOCTL_BLOCK_CAPACITY:
        {
            //
            // Not used at this point.  The assumption is made that FAT
            // formatted MMC cards are being used.
            //
            return(MMCGetCapacity());
        }

        //
        // Format the media.
        //
        case IOCTL_BLOCK_FORMAT:
        {
            //
            // We do not need to block format a MMC card, so return success.
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

//****************************************************************************
//
// The file system IOCTL entry point for the first MMC card.
//
//****************************************************************************
unsigned long
MMCIoctl1(unsigned char *pucScratch, unsigned char *pucWriteBuffer,
          unsigned long ulIoctl, unsigned long ulInstance,
          unsigned long ulParam1, unsigned long ulParam2)
{
    //
    // If we are being initialized, then we need to setup our global variables.
    //
    if(ulIoctl == IOCTL_FS_INIT)
    {
        //
        // Set the address of the block driver to be used by the FAT layer.
        //
        sMMC.sFAT1.pfnBlockDriver = MMCBlockIoctl;
        sMMC.sFAT1.pvBlockDriverState = 0;

        //
        // Initialize the MMC cards.
        //
        if(MMCInit() == 0)
        {
            return(0);
        }

        //
        // Select the first MMC card.
        //
        MMCSelectCardOne();

        //
        // The first card is the currently selected card.
        //
        sMMC.usCurrent = 0;
    }

    //
    // If the name of the media is being requested, then return it.
    //
    if(ulIoctl == IOCTL_FS_GETMEDIANAME)
    {
        //
        // Return the name of the drive.
        //
        memcpy((void *)ulParam1, pusDriveName1, sizeof(pusDriveName1));

        //
        // Return the length of the drive name.
        //
        *((unsigned long *)ulParam2) = sizeof(pusDriveName1);

        //
        // Success.
        //
        return(1);
    }

    //
    // See if there are any MMC cards.
    //
    if(sMMC.usNumCards == 0)
    {
        //
        // There are no cards, so return a failure.
        //
        return(0);
    }

    //
    // See if the first card is the one currently selected.
    //
    if(sMMC.usCurrent != 0)
    {
        //
        // The second card is currently selected, so select the first card.
        //
        MMCSelectCardOne();

        //
        // The first card is the currently selected card.
        //
        sMMC.usCurrent = 0;
    }

    //
    // Call the FAT code.
    //
    return(FATIoctl(&sMMC.sFAT1, pucScratch, pucWriteBuffer, ulIoctl,
                    ulInstance, ulParam1, ulParam2));
}

//****************************************************************************
//
// The file system IOCTL entry point for the second MMC card.
//
//****************************************************************************
unsigned long
MMCIoctl2(unsigned char *pucScratch, unsigned char *pucWriteBuffer,
          unsigned long ulIoctl, unsigned long ulInstance,
          unsigned long ulParam1, unsigned long ulParam2)
{
    //
    // If we are being initialized, then we need to setup our global variables.
    //
    if(ulIoctl == IOCTL_FS_INIT)
    {
        //
        // Set the address of the block driver to be used by the FAT layer.
        //
        sMMC.sFAT2.pfnBlockDriver = MMCBlockIoctl;
        sMMC.sFAT2.pvBlockDriverState = 0;
    }

    //
    // If the name of the media is being requested, then return it.
    //
    if(ulIoctl == IOCTL_FS_GETMEDIANAME)
    {
        //
        // Return the name of the drive.
        //
        memcpy((void *)ulParam1, pusDriveName2, sizeof(pusDriveName2));

        //
        // Return the length of the drive name.
        //
        *((unsigned long *)ulParam2) = sizeof(pusDriveName2);

        //
        // Success.
        //
        return(1);
    }

    //
    // See if there are two MMC cards.
    //
    if(sMMC.usNumCards < 2)
    {
        //
        // There are not two MMC cards, so return a failure.
        //
        return(0);
    }

    //
    // See if the second cards is the one currently selected.
    //
    if(sMMC.usCurrent != 1)
    {
        //
        // The first card is currently selected, so select the second card.
        //
        MMCSelectCardTwo();

        //
        // The second card is the currently selected card.
        //
        sMMC.usCurrent = 1;
    }

    //
    // Call the FAT code.
    //
    return(FATIoctl(&sMMC.sFAT2, pucScratch, pucWriteBuffer, ulIoctl,
                    ulInstance, ulParam1, ulParam2));
}
#endif

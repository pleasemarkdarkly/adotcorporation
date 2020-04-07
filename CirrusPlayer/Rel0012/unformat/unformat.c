//****************************************************************************
//
// UNFORMAT.C - Erases the contents of the NAND FLASH on the EP7209 board.
//
// Copyright (c) 1999-2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include <stdio.h>
#include "bootcode.h"

//****************************************************************************
//
// SendChar sends a character to a comm port.
//
//****************************************************************************
void
SendChar(long lPort, char cChar)
{
    __asm
    {
        mov  edx,lPort
        add  dx,5
    SC:
        in   al,dx
        test al,40h
        jz   SC
        sub  dx,5
        mov  al,cChar
        out  dx,al
    }
}

//****************************************************************************
//
// ReceiveChar reads a character from a comm port.
//
//****************************************************************************
char
ReceiveChar(long lPort)
{
    char cRet;

    __asm
    {
        mov  edx,lPort
        add  dx,5
    RC:
        in   al,dx
        test al,01h
        jz   RC
        sub  dx,5
        in   al,dx
        mov  cRet,al
    }
    return(cRet);
}

//****************************************************************************
//
// CharReady determines if there is a character ready to be read from the
// comm port.
//
//****************************************************************************
int
CharReady(long lPort)
{
    long lReady;

    __asm
    {
        mov  edx,lPort
        add  dx,5
        xor  eax,eax
        in   al,dx
        and  al,01h
        mov  lReady,eax
    }
    return(lReady);
}

//****************************************************************************
//
// WaitFor waits until a specific character is read from the comm port.
//
//****************************************************************************
void
WaitFor(long lPort, char cWaitChar)
{
    char cChar;

    //
    // Wait until we read a specific character from the comm port.
    //
    while(1)
    {
        //
        // Read a character.
        //
        cChar = ReceiveChar(lPort);

        //
        // Stop waiting if we received the character.
        //
        if(cChar == cWaitChar)
        {
            break;
        }
    }
}

//****************************************************************************
//
// This program waits for the '<' character from the boot ROM, sends the boot
// code, waits for the '>' from the boot ROM, waits for the '?' from the boot
// code, changes the serial port rate (preferably to 115200), downloads the
// user data file, and then prints out progress status as the boot code writes
// the user data file to the NOR FLASH.
//
//****************************************************************************
void
main(int argc, char *argv[])
{
    long lPortAddr[4] = { 0x3f8, 0x2f8, 0x3e8, 0x2e8 };
    long lPort = 1, lFileSize, lIdx, lLoop, lSum;
    char cChar, cFirstChar, cRateChar, cBuffer[1024];
    int bError = 0;
    FILE *pFile;

    //
    // If a comm port was specified, then read it and make sure it is valid.
    //
    if(argc > 1)
    {
        lPort = atoi(argv[1]);
        if((lPort != 1) && (lPort != 2) && (lPort != 3) && (lPort != 4))
        {
            printf("Invalid comm port.\n");
            return;
        }
    }

    //
    // Get the port address for the comm port we will use.
    //
    lPort = lPortAddr[lPort - 1];

    //
    // Program a baud rate of 9600, 8 data bits, and no parity.
    //
    __asm
    {
        mov edx,lPort
        mov al,80h
        add dx,3
        out dx,al
        sub dx,3
        mov al,12
        out dx,al
        add dx,1
        xor al,al
        out dx,al
        add dx,2
        mov al,3
        out dx,al
        sub dx,2
        xor al,al
        out dx,al
        add dx,1
        mov al,7
        out dx,al
    }

    //
    // Empty out the input queue.
    //
    while(CharReady(lPort))
    {
        cChar = ReceiveChar(lPort);
    }

    //
    // Tell the user to reset the board.
    //
    printf("Waiting for the board to wakeup...");

    //
    // Wait until we read a '<' from the comm port.
    //
    WaitFor(lPort, '<');

    //
    // Tell the user that we are downloading the boot code.
    //
    printf("\r                                  \r");
    printf("Downloading unformat code...(  0%%)");

    //
    // Write the boot code to the comm port.
    //
    for(lIdx = 0; lIdx < 2048; lIdx++)
    {
        //
        // Write this character.
        //
        SendChar(lPort, pcBoot[lIdx]);

        //
        // Periodically print out our progress.
        //
        if((lIdx & 127) == 127)
        {
            printf("\b\b\b\b\b%3d%%)", ((lIdx + 1) * 100) / 2048);
        }
    }

    //
    // Wait until we read a '>' from the comm port.
    //
    WaitFor(lPort, '>');

    //
    // Indicate that the unformat is proceeding.
    //
    printf("\r                                  \r");
    printf("Unformatting...");

    //
    // Wait until we read a '!' from the comm port.
    //
    WaitFor(lPort, '!');

    //
    // Indicate that the unformat succeeded.
    //
    printf("\r               \r");
    printf("Successfully unformatted the NAND FLASH.\n");
}

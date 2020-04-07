//****************************************************************************
//
// MAKEBOOT.C - Converts the boot code image into a header file for inclusion
//              into download.c
//
// Copyright (c) 1999-2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include <stdio.h>

//****************************************************************************
//
// This program simply reads image/bootcode.rom and writes it out as a C
// character array definition in bootcode.h.
//
//****************************************************************************
void
main(int argc, char *argv[])
{
    FILE *pFileIn, *pFileOut;
    int iIdx;
    unsigned char ucChar;
    char cBuffer[256];

    //
    // Open the boot code image file.
    //
    strcpy(cBuffer, argv[1]);
    strcat(cBuffer, "\\bootcode.rom");
    //pFileIn = fopen("image/bootcode.rom", "rb");
    pFileIn = fopen(cBuffer, "rb");
    if(!pFileIn)
    {
        //printf("Failed to open 'image/bootcode.rom'.\n");
        printf("Failed to open '%s'.\n", cBuffer);
        return;
    }

    //
    // Open the output C header file.
    //
    strcpy(cBuffer, argv[1]);
    strcat(cBuffer, "\\bootcode.h");
    //pFileOut = fopen("bootcode.h", "w");
    pFileOut = fopen(cBuffer, "w");
    if(!pFileOut)
    {
        //printf("Failed to open 'bootcode.h'.\n");
        printf("Failed to open '%s'.\n", cBuffer);
        return;
    }

    //
    // Put the header C code into bootcode.h.
    //
    fprintf(pFileOut, "// THIS IS A GENERATED FILE...DO NOT EDIT!\n");
    fprintf(pFileOut, "char pcBoot[2048] = {\n");

    //
    // Write 2K bytes of data into the boot code header file.
    //
    for(iIdx = 0; iIdx < 2048; iIdx++)
    {
        //
        // Read a byte from the boot code image file.
        //
        if(fread(&ucChar, 1, 1, pFileIn) == 0)
        {
            //
            // We were unable to read a byte (most likely due to the end of the
            // file), so we want to write out a '0'.
            //
            ucChar = 0;
        }

        //
        // Write out this character to the header file.
        //
        fprintf(pFileOut, "%3d, ", ucChar);

        //
        // Keep the lines reasonably short.
        //
        if((iIdx & 15) == 15)
        {
            fprintf(pFileOut, "\n");
        }
    }

    //
    // See if there is more data in the file.  There should not be...
    //
    if(fread(&ucChar, 1, 1, pFileIn) != 0)
    {
        printf("Error: boot code is too large.\n");
    }

    //
    // Put the trailer C code into bootcode.h.
    //
    fprintf(pFileOut, "};\n");

    //
    // Close the input and output files.
    //
    fclose(pFileIn);
    fclose(pFileOut);
}

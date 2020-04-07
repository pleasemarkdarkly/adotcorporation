//****************************************************************************
//
// BUILDIMG.C - Builds a ROM image from the boot code and the application
//              image.
//
// Copyright (c) 2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include <stdio.h>

//****************************************************************************
//
// This program takes a boot code image file and an application image file and
// builds a single ROM image.
//
//****************************************************************************
int
main(int argc, char *argv[])
{
    FILE *fBoot, *fImage, *fOut;
    unsigned long ulOffset, ulLength, ulSize;
    unsigned char ucBuffer[4096];

    //
    // Make sure that enough command line arguments were supplied.
    //
    if(argc < 4)
    {
        printf("Usage: %s <bootcode> <image> <outfile>\n", argv[0]);
        return(1);
    }

    //
    // Open the boot code image file.
    //
    fBoot = fopen(argv[1], "rb");
    if(!fBoot)
    {
        printf("Unable to open boot code file '%s'.\n", argv[1]);
        return(1);
    }

    //
    // Open the application code image file.
    //
    fImage = fopen(argv[2], "rb");
    if(!fImage)
    {
        printf("Unable to open image file '%s'.\n", argv[2]);
        return(1);
    }

    //
    // Open the output file.
    //
    fOut = fopen(argv[3], "wb");
    if(!fOut)
    {
        printf("Unable to open output file '%s'.\n", argv[4]);
        return(1);
    }

    //
    // Get the length of the boot code file.
    //
    fseek(fBoot, 0, SEEK_END);
    ulLength = ftell(fBoot);
    fseek(fBoot, 0, SEEK_SET);

    //
    // Get the starting position of the application image.
    //
    fread(ucBuffer, 1, 0x24, fImage);
    fseek(fImage, 0, SEEK_SET);
    ulOffset = *(unsigned long *)(ucBuffer + 0x20) - 0x20;

    //
    // Make sure that the boot code image is not too large.
    //
    if(ulLength > ulOffset)
    {
        printf("Boot image is too large!\n");
        return(1);
    }

    //
    // Fill the first portion of the output file with the boot code image, zero
    // padding the unused space.
    //
    for(ulLength = 0; ulLength < ulOffset; ulLength += 4096)
    {
        //
        // Read the next portion of the boot code image file.
        //
        ulSize = fread(ucBuffer, 1, 4096, fBoot);

        //
        // Zero fill any unused space at the end of the buffer.
        //
        while(ulSize != 4096)
        {
            ucBuffer[ulSize++] = 0;
        }

        //
        // Write the next portion of the output file.
        //
        if(fwrite(ucBuffer, 1, 4096, fOut) != 4096)
        {
            printf("Error writing output file.\n");
            return(1);
        }
    }

    //
    // Get the length of the application image file.
    //
    fseek(fImage, 0, SEEK_END);
    ulLength = ftell(fImage);
    fseek(fImage, 0, SEEK_SET);

    //
    // Loop while there is more data in the application image file.
    //
    while(ulLength)
    {
        //
        // Get the size of the next block to read.  We copy the image in 4K
        // chunks.
        //
        ulSize = (ulLength > 4096) ? 4096 : ulLength;

        //
        // Read the next portion of the application image file.
        //
        if(fread(ucBuffer, 1, ulSize, fImage) != ulSize)
        {
            printf("Error reading application image file.\n");
            return(1);
        }

        //
        // Write the next portion of the output file.
        //
        if(fwrite(ucBuffer, 1, ulSize, fOut) != ulSize)
        {
            printf("Error writing output file.\n");
            return(1);
        }

        //
        // Decrement the remaining length of the application image file.
        //
        ulLength -= ulSize;
    }

    //
    // Success.
    //
    return(0);
}

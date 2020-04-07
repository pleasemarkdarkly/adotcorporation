//****************************************************************************
//
// FAT.H - Public definitions for the FAT12 and FAT16 code.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
//
//****************************************************************************

#ifndef _FAT_H
#define _FAT_H

//****************************************************************************
//
// The following values are the IOCTLs which are sent to the block drivers by
// the FAT layer.
//
//****************************************************************************
enum
{
    IOCTL_BLOCK_INIT,
    IOCTL_BLOCK_GETMEDIAID,
    IOCTL_BLOCK_READ,
    IOCTL_BLOCK_READ_BS,
    IOCTL_BLOCK_WRITE,
    IOCTL_BLOCK_CAPACITY,
    IOCTL_BLOCK_FORMAT
};

//****************************************************************************
//
// The following structure contains the persistent state of the FAT file
// system parser/writer.
//
//****************************************************************************
#ifndef _FAT_I_H
typedef struct
{
    //
    // A pointer to the driver for reading and writing blocks from the actual
    // media.
    //
    unsigned long (*pfnBlockDriver)(void *pvBlockDriverState,
                                    unsigned char *pucScratch,
                                    unsigned long ulIoctl,
                                    unsigned long ulParam1,
                                    unsigned long ulParam2,
                                    unsigned long ulParam3);

    //
    // A pointer to the persistent state of the block driver.
    //
    void *pvBlockDriverState;

    //
    // The internal, persistent state of the FAT code.
    //
    unsigned char ucInternal[16];
} tFAT;
#endif

//****************************************************************************
//
// The following structure contains the persistent state of a file opened by
// the FAT file system parser/writer.
//
//****************************************************************************
#ifndef _FAT_I_H
typedef struct
{
    //
    // The internal, persistent state of an open file in the FAT file system.
    //
    unsigned char ucInternal[20];
} tFATFile;
#endif

//****************************************************************************
//
// The following structure contains the persistent state of a directory opened
// by the FAT file system parser/writer.
//
//****************************************************************************
#ifndef _FAT_I_H
typedef struct
{
    unsigned char ucInternal[4];
} tFATDir;
#endif

//****************************************************************************
//
// Function prototypes and global variables.
//
//****************************************************************************
extern unsigned long FATIoctl(tFAT *pFAT, unsigned char *pucScratch,
                              unsigned char *pucWriteBuffer,
                              unsigned long ulIoctl, unsigned long ulInstance,
                              unsigned long ulParam1, unsigned long ulParam2);

#endif // _FAT_H

//****************************************************************************
//
// FAT.H - Public definitions for the FAT12 and FAT16 code.
//
// Copyright (c) 2000,2001 Cirrus Logic, Inc.
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
// The following are the flags passed to FATOpen.
//
//****************************************************************************
#define FAT_OPEN_READ                           0x00000001
#define FAT_OPEN_WRITE                          0x00000002

//****************************************************************************
//
// The following are the types passed to FATReadDir.
//
//****************************************************************************
#define FAT_TYPE_FILE                           0x00000001
#define FAT_TYPE_SUBDIR                         0x00000002

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
// Function prototypes.
//
//****************************************************************************
extern unsigned long FATInit(tFAT *pFAT, unsigned char *pucScratch);
extern unsigned long FATGetMediaID(tFAT *pFAT, unsigned char *pucScratch,
                                   unsigned char *pucMediaIDBuffer,
                                   unsigned long *pulLength);
extern unsigned long FATOpenFile(tFAT *pFAT, unsigned char *pucScratch,
                                 tFATFile *pFile,
                                 const unsigned short *pusFileName,
                                 unsigned long ulFlags);
extern unsigned long FATCreateFile(tFAT *pFAT, unsigned char *pucScratch,
                                   unsigned char *pucWriteBuffer,
                                   tFATFile *pFile,
                                   const unsigned short *pusFileName,
                                   unsigned long ulFileLength);
extern unsigned long FATRead(tFAT *pFAT, tFATFile *pFile,
                             unsigned char *pucBuffer, unsigned long ulLength,
                             unsigned long bByteSwap);
extern unsigned long FATWrite(tFAT *pFAT, unsigned char *pucScratch,
                              unsigned char *pucWriteBuffer, tFATFile *pFile,
                              unsigned char *pucBuffer,
                              unsigned long ulLength);
extern unsigned long FATSeek(tFAT *pFAT, unsigned char *pucScratch,
                             unsigned char *pucWriteBuffer, tFATFile *pFile,
                             unsigned long ulFilePos,
                             unsigned long ulFileFlags);
extern unsigned long FATTell(tFATFile *pFile);
extern unsigned long FATLength(tFATFile *pFile);
extern unsigned long FATGetDate(tFAT *pFAT, unsigned char *pucScratch,
                                tFATFile *pFile);
extern unsigned long FATCloseFile(tFAT *pFAT, unsigned char *pucScratch,
                                  unsigned char *pucWriteBuffer,
                                  tFATFile *pFile, unsigned long ulFlags);
extern unsigned long FATDeleteFile(tFAT *pFAT, unsigned char *pucScratch,
                                   unsigned char *pucWriteBuffer,
                                   const unsigned short *pusFileName);
extern unsigned long FATOpenDirectory(tFAT *pFAT, unsigned char *pucScratch,
                                      tFATDir *pDir,
                                      const unsigned short *pusDirName);
extern unsigned long FATReadDirectory(tFAT *pFAT, unsigned char *pucScratch,
                                      tFATDir *pDir, unsigned short *pusName,
                                      unsigned long ulType);
extern unsigned long FATCloseDirectory(tFATDir *pDir);
extern unsigned long FATCreateDirectory(tFAT *pFAT, unsigned char *pucScratch,
                                        unsigned char *pucWriteBuffer,
                                        const unsigned short *pusDirName);
extern unsigned long FATRemoveDirectory(tFAT *pFAT, unsigned char *pucScratch,
                                        unsigned char *pucWriteBuffer,
                                        const unsigned short *pusDirName);
extern unsigned long FATTotalSpace(tFAT *pFAT);
extern unsigned long FATFreeSpace(tFAT *pFAT, unsigned char *pucScratch);
extern unsigned long FATFormat(tFAT *pFAT, unsigned char *pucScratch,
                               unsigned char *pucWriteBuffer);

#endif // _FAT_H

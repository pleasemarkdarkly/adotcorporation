//****************************************************************************
//
// FS.C - Generic file system translation layer.
//
// Copyright (c) 1999-2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include "globals.h"

//****************************************************************************
//
// The number of file system drivers which are supported by the file system
// layer.
//
//****************************************************************************
#define NUMFS                           2

//****************************************************************************
//
// An array of entry points for the file system drivers supported by the file
// system layer.
//
//****************************************************************************
static unsigned long (* const pfnIoctl[NUMFS])(unsigned char *pucScratch,
                                               unsigned char *pucWriteBuffer,
                                               unsigned long ulIoctl,
                                               unsigned long ulParam1,
                                               unsigned long ulParam2,
                                               unsigned long ulParam3) =
{
    //
    // The on-board NAND FLASH file system.
    //
    NANDIoctl,

    //
    // The removeable SmartMedia card.
    //
    SMIoctl
};

//****************************************************************************
//
// The following structure contains the persistent state of the generic
// file system layer.
//
//****************************************************************************
static struct
{
    //
    // The number of files that are currently opened.
    //
    unsigned short usNumFiles;

    //
    // The number of directories that are currently opened.
    //
    unsigned short usNumDirs;

    //
    // A scratch buffer to be used during read or write operations by the file
    // system drivers.  This is shared between all file system drivers and no
    // driver should assume that the data they store in this buffer will be
    // maintained between calls to the file system driver.
    //
    unsigned char ucScratch[512];

    //
    // A pointer to the additional scratch buffer to be used during write
    // operation by the file system drivers.  The size of this buffer is 512
    // bytes.  This is also shared between all file system drivers with the
    // same assumptions about data persistence.
    //
    unsigned char *pucWriteScratch;
} sFS;

//****************************************************************************
//
// FSInit initializes the generic file system layer.
//
//****************************************************************************
void
FSInit(void)
{
    unsigned long ulDrive;

    //
    // Initialize each "drive" in the system.
    //
    for(ulDrive = 0; ulDrive < NUMFS; ulDrive++)
    {
        (pfnIoctl[ulDrive])(sFS.ucScratch, sFS.pucWriteScratch, IOCTL_FS_INIT,
                            0, 0, 0);
    }

    //
    // Initially, there is not a file or directory opened.
    //
    sFS.usNumFiles = 0;
    sFS.usNumDirs = 0;
}

//****************************************************************************
//
// FSSetWriteScratch sets the address of the scratch buffer to be used during
// write operations.
//
//****************************************************************************
void
FSSetWriteScratch(unsigned char *pucWriteScratch)
{
    //
    // Remember the address of the write scratch buffer.
    //
    sFS.pucWriteScratch = pucWriteScratch;
}

//****************************************************************************
//
// FSNumDrives returns the number of drives supported by the file system
// layer.
//
//****************************************************************************
unsigned long
FSNumDrives(void)
{
    //
    // Return the number of drives we support.
    //
    return(NUMFS);
}

//****************************************************************************
//
// FSGetMediaID returns the media unique ID for the given drive, if it exists.
//
//****************************************************************************
unsigned long
FSGetMediaID(unsigned long ulDrive, unsigned char *pucMediaIDBuffer,
             unsigned long *pulLength)
{
    unsigned long ulRet;

    //
    // If the drive number is invalid, then return a failure.
    //
    if(ulDrive >= NUMFS)
    {
        return(0);
    }

    //
    // Pass the get media ID request to the entry point for the specified
    // drive.
    //
    ulRet = (pfnIoctl[ulDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                                IOCTL_FS_GETMEDIAID, 0,
                                (unsigned long)pucMediaIDBuffer,
                                (unsigned long)pulLength);

    //
    // If the request failed, then we need to return the device's unique ID.
    //
    if(ulRet == 0)
    {
#if 1
        //
        // This is the device ID to which the sample content in the WMA
        // distribution was bound.  First, make sure the ID buffer is large
        // enough.
        //
        if(*pulLength < (20 * 8))
        {
            return(0);
        }

        //
        // Return the Microsoft generated ID.
        //
        memcpy(pucMediaIDBuffer, "        M 3T10050095", 20);
        *pulLength = 20 * 8;
#else
        //
        // We have no way of specifying a unique device ID at this time.
        //
        return(0);
#endif
    }

    //
    // Success.
    //
    return(1);
}

//****************************************************************************
//
// FSOpen opens or creates the specified file on the specified drive.
//
//****************************************************************************
unsigned long
FSOpen(tFile *pFile, unsigned long ulDrive, const char *pcFileName,
       unsigned long ulFlags)
{
    unsigned long ulRet;

    //
    // If the drive number is invalid, then return a failure.
    //
    if(ulDrive >= NUMFS)
    {
        return(0);
    }

    //
    // Pass the open request to the entry point for the specified drive.
    //
    ulRet = (pfnIoctl[ulDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                                IOCTL_FS_OPEN,
                                (unsigned long)&pFile->uInternal,
                                (unsigned long)pcFileName, ulFlags);

    //
    // If the open succeeded, then increment the count of opened files.
    //
    if(ulRet)
    {
        //
        // Remember the drive on which this file resides.
        //
        pFile->ucDrive = ulDrive;

        //
        // Remember the flags used to open this file.
        //
        pFile->ucFlags = ulFlags;

        //
        // Increment the count of opened files.
        //
        sFS.usNumFiles++;
    }

    //
    // Return the result to the caller.
    //
    return(ulRet);
}

//****************************************************************************
//
// FSCreate creates the specified file with the given size on the specified
// drive.
//
//****************************************************************************
unsigned long
FSCreate(tFile *pFile, unsigned long ulDrive, const char *pcFileName,
         unsigned long ulFileLength)
{
    unsigned long ulRet;

    //
    // If the drive number is invalid, then return a failure.
    //
    if(ulDrive >= NUMFS)
    {
        return(0);
    }

    //
    // Pass the create request to the entry point for the specified drive.
    //
    ulRet = (pfnIoctl[ulDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                                IOCTL_FS_CREATE,
                                (unsigned long)&pFile->uInternal,
                                (unsigned long)pcFileName, ulFileLength);

    //
    // If the create succeeded, then increment the count of opened files.
    //
    if(ulRet)
    {
        //
        // Remember the drive on which this file resides.
        //
        pFile->ucDrive = ulDrive;

        //
        // Remember the flags used to open this file.
        //
        pFile->ucFlags = FS_OPEN_READ | FS_OPEN_WRITE | FS_OPEN_CREATE;

        //
        // Increment the count of opened files.
        //
        sFS.usNumFiles++;
    }

    //
    // Return the result to the caller.
    //
    return(ulRet);
}

//****************************************************************************
//
// FSRead reads data from the opened file.
//
//****************************************************************************
unsigned long
FSRead(tFile *pFile, void *pvBuffer, unsigned long ulNumBytes)
{
    //
    // If the file was not opened for reading, then return an error.
    //
    if(!(pFile->ucFlags & FS_OPEN_READ))
    {
        return(0);
    }

    //
    // Pass the read request to the entry point for the specified drive.
    //
    return((pfnIoctl[pFile->ucDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                                      IOCTL_FS_READ,
                                      (unsigned long)&pFile->uInternal,
                                      (unsigned long)pvBuffer, ulNumBytes));
}

//****************************************************************************
//
// FSReadBS reads data from the opened file, performing a byte-swap on each
// 32-bit word in the read data.
//
//****************************************************************************
unsigned long
FSReadBS(tFile *pFile, void *pvBuffer, unsigned long ulNumBytes)
{
    //
    // If the file was not opened for reading, then return an error.
    //
    if(!(pFile->ucFlags & FS_OPEN_READ))
    {
        return(0);
    }

    //
    // Pass the read request to the entry point for the specified drive.
    //
    return((pfnIoctl[pFile->ucDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                                      IOCTL_FS_READ_BS,
                                      (unsigned long)&pFile->uInternal,
                                      (unsigned long)pvBuffer, ulNumBytes));
}

//****************************************************************************
//
// FSWrite writes data to the opened file.
//
//****************************************************************************
unsigned long
FSWrite(tFile *pFile, void *pvBuffer, unsigned long ulNumBytes)
{
    //
    // If the file was not opened for writing, then return an error.
    //
    if(!(pFile->ucFlags & FS_OPEN_WRITE))
    {
        return(0);
    }

    //
    // Pass the write request to the entry point for the specified drive.
    //
    return((pfnIoctl[pFile->ucDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                                      IOCTL_FS_WRITE,
                                      (unsigned long)&pFile->uInternal,
                                      (unsigned long)pvBuffer, ulNumBytes));
}

//****************************************************************************
//
// FSSeek moves the read/write pointer for the opened file to the specified
// position.
//
//****************************************************************************
unsigned long
FSSeek(tFile *pFile, unsigned long ulPos)
{
    //
    // Pass the seek request to the entry point for the specified drive.
    //
    return((pfnIoctl[pFile->ucDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                                      IOCTL_FS_SEEK,
                                      (unsigned long)&pFile->uInternal, ulPos,
                                      pFile->ucFlags));
}

//****************************************************************************
//
// FSDriveNum returns the number of the drive on which the currently opened
// file resides.
//
//****************************************************************************
unsigned long
FSDriveNum(tFile *pFile)
{
    //
    // Return the drive number.
    //
    return(pFile->ucDrive);
}

//****************************************************************************
//
// FSTell returns the current read/write for the opened file.
//
//****************************************************************************
unsigned long
FSTell(tFile *pFile)
{
    //
    // Pass the pointer position request to the entry point for the specified
    // drive.
    //
    return((pfnIoctl[pFile->ucDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                                      IOCTL_FS_TELL,
                                      (unsigned long)&pFile->uInternal, 0, 0));
}

//****************************************************************************
//
// FSLength returns the length of the currently opened file.
//
//****************************************************************************
unsigned long
FSLength(tFile *pFile)
{
    //
    // Pass the length request to the entry point for the specified drive.
    //
    return((pfnIoctl[pFile->ucDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                                      IOCTL_FS_LENGTH,
                                      (unsigned long)&pFile->uInternal, 0, 0));
}

//****************************************************************************
//
// FSClose closes the currently opened file.
//
//****************************************************************************
unsigned long
FSClose(tFile *pFile)
{
    unsigned long ulRet;

    //
    // Pass the close request to the entry point for the specified drive.
    //
    ulRet = (pfnIoctl[pFile->ucDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                                       IOCTL_FS_CLOSE,
                                       (unsigned long)&pFile->uInternal,
                                       pFile->ucFlags, 0);

    //
    // If the close succeeded, then decrement the count of opened files.
    //
    if(ulRet)
    {
        sFS.usNumFiles--;
    }

    //
    // Return the result to the caller.
    //
    return(ulRet);
}

//****************************************************************************
//
// FSDelete removes the specified file from the file system.
//
//****************************************************************************
unsigned long
FSDelete(unsigned long ulDrive, const char *pcFileName)
{
    //
    // If there is a file opened, then return an error.  This is to simplify
    // file system code development since it does not have to handle the case
    // of deleting a currently opened file.
    //
    if(sFS.usNumFiles)
    {
        return(0);
    }

    //
    // If the drive number is invalid, then return a failure.
    //
    if(ulDrive >= NUMFS)
    {
        return(0);
    }

    //
    // Pass the delete request to the entry point for the specified drive.
    //
    return((pfnIoctl[ulDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                               IOCTL_FS_DELETE, 0, (unsigned long)pcFileName,
                               0));
}

//****************************************************************************
//
// FSOpenDir opens the specified directory on the specified drive.
//
//****************************************************************************
unsigned long
FSOpenDir(tDir *pDir, unsigned long ulDrive, const char *pcDirName)
{
    unsigned long ulRet;

    //
    // If the drive number is invalid, then return a failure.
    //
    if(ulDrive >= NUMFS)
    {
        return(0);
    }

    //
    // Pass the open directory request to the entry point for the specified
    // drive.
    //
    ulRet = (pfnIoctl[ulDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                                IOCTL_FS_OPENDIR,
                                (unsigned long)&pDir->uInternal,
                                (unsigned long)pcDirName, 0);

    //
    // If the open directory succeeded, then increment the count of opened
    // directories.
    //
    if(ulRet)
    {
        //
        // Remember the drive on which this directory resides.
        //
        pDir->ucDrive = ulDrive;

        //
        // Increment the count of opened directories.
        //
        sFS.usNumDirs++;
    }

    //
    // Return the result to the caller.
    //
    return(ulRet);
}

//****************************************************************************
//
// FSReadDir reads the next directory entry from the currently opened
// directory.
//
//****************************************************************************
unsigned long
FSReadDir(tDir *pDir, char *pcFileName, unsigned long ulType)
{
    //
    // Pass the read directory request to the entry point for the specified
    // drive.
    //
    return((pfnIoctl[pDir->ucDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                                     IOCTL_FS_READDIR,
                                     (unsigned long)&pDir->uInternal,
                                     (unsigned long)pcFileName, ulType));
}

//****************************************************************************
//
// FSCloseDir closes the currently opened directory.
//
//****************************************************************************
unsigned long
FSCloseDir(tDir *pDir)
{
    unsigned long ulRet;

    //
    // Pass the close directory request to the entry point for the specified
    // drive.
    //
    ulRet = (pfnIoctl[pDir->ucDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                                      IOCTL_FS_CLOSEDIR,
                                      (unsigned long)&pDir->uInternal, 0, 0);

    //
    // If the close directory succeeded, then decrement the count of opened
    // directories.
    //
    if(ulRet)
    {
        sFS.usNumDirs--;
    }

    //
    // Return the result to the caller.
    //
    return(ulRet);
}

//****************************************************************************
//
// FSMakeDir creates the specified directory on the specified drive.  This
// will create any/all directories in the specified path.
//
//****************************************************************************
unsigned long
FSMakeDir(unsigned long ulDrive, const char *pcDirName)
{
    //
    // If the drive number is invalid, then return a failure.
    //
    if(ulDrive >= NUMFS)
    {
        return(0);
    }

    //
    // Pass the create directory request to the entry point for the specified
    // drive.
    //
    return((pfnIoctl[ulDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                               IOCTL_FS_MAKEDIR, 0, (unsigned long)pcDirName,
                               0));
}

//****************************************************************************
//
// FSRemoveDir removes the specified directry from the specified drive.
//
//****************************************************************************
unsigned long
FSRemoveDir(unsigned long ulDrive, const char *pcDirName)
{
    //
    // Do not allow directories to be removed if a directory scan is in
    // progress (to avoid removing the directory that is currently being
    // scanned).
    //
    if(sFS.usNumDirs)
    {
        return(0);
    }

    //
    // If the drive number is invalid, then return a failure.
    //
    if(ulDrive >= NUMFS)
    {
        return(0);
    }

    //
    // Pass the remove directory request to the entry point for the specified
    // drive.
    //
    return((pfnIoctl[ulDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                               IOCTL_FS_REMOVEDIR, 0, (unsigned long)pcDirName,
                               0));
}

//****************************************************************************
//
// FSTotalSpace returns the total capacity of the specified drive.
//
//****************************************************************************
unsigned long
FSTotalSpace(unsigned long ulDrive)
{
    //
    // If the drive number is invalid, then return a failure.
    //
    if(ulDrive >= NUMFS)
    {
        return(0);
    }

    //
    // Pass the total space request to the entry point for the specified drive.
    //
    return((pfnIoctl[ulDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                               IOCTL_FS_TOTALSPACE, 0, 0, 0));
}

//****************************************************************************
//
// FSFreeSpace returns the currently available capacity on the specified
// drive.
//
//****************************************************************************
unsigned long
FSFreeSpace(unsigned long ulDrive)
{
    //
    // If the drive number is invalid, then return a failure.
    //
    if(ulDrive >= NUMFS)
    {
        return(0);
    }

    //
    // Pass the free space request to the entry point for the specified drive.
    //
    return((pfnIoctl[ulDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                               IOCTL_FS_FREESPACE, 0, 0, 0));
}

//****************************************************************************
//
// FSFormat formats the specified drive.
//
//****************************************************************************
unsigned long
FSFormat(unsigned long ulDrive)
{
    //
    // If there is a file or directory opened, then return a failure.
    //
    if(sFS.usNumFiles || sFS.usNumDirs)
    {
        return(0);
    }

    //
    // If the drive number is invalid, then return a failure.
    //
    if(ulDrive >= NUMFS)
    {
        return(0);
    }

    //
    // Pass the format request to the entry point for the specified drive.
    //
    return((pfnIoctl[ulDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                               IOCTL_FS_FORMAT, 0, 0, 0));
}

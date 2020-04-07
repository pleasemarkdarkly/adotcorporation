//****************************************************************************
//
// FS.C - Generic file system translation layer.
//
// Copyright (c) 1999-2001 Cirrus Logic, Inc.
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
// FSGetScratch returns the address of the scratch buffer used by the file
// system for read and write operations.  The contents of this buffer are not
// guaranteed to remain constant across calls to the file system, but can be
// used as the read buffer for a call to FSRead().
//
//****************************************************************************
unsigned char *
FSGetScratch(void)
{
    //
    // Return the address of the scratch buffer.
    //
    return(sFS.ucScratch);
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
#ifdef SUPPORT_RIGHTS_PD
        static const unsigned char ucID[16] = RIGHTS_PD_DEVICE_ID;

        //
        // Make sure the ID buffer is large enough.
        //
        if(*pulLength < (16 * 8))
        {
            return(0);
        }

        //
        // Return our device ID.
        //
        memcpy(pucMediaIDBuffer, ucID, 16);
        *pulLength = 16 * 8;
#else
#ifdef SUPPORT_WMA
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
#endif
    }

    //
    // Success.
    //
    return(1);
}

//****************************************************************************
//
// FSGetMediaName returns the name of the specified drive.
//
//****************************************************************************
unsigned long
FSGetMediaName(unsigned long ulDrive, unsigned short *pusName,
               unsigned long *pulLength)
{
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
    return((pfnIoctl[ulDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                               IOCTL_FS_GETMEDIANAME, 0,
                               (unsigned long)pusName,
                               (unsigned long)pulLength));
}

//****************************************************************************
//
// FSOpen opens or creates the specified file on the specified drive.
//
//****************************************************************************
unsigned long
FSOpen(tFile *pFile, unsigned long ulDrive, const unsigned short *pusFileName,
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
                                (unsigned long)pusFileName, ulFlags);

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

#ifdef SUPPORT_RIGHTS_PD
        //
        // The size of the header defaults to zero.
        //
        pFile->ulHeaderSize = 0;

        //
        // See if we should decrypt this file (if it is encrypted).
        //
        if(ulFlags == (FS_OPEN_READ | FS_OPEN_DECRYPT))
        {
            //
            // Have the Rights/PD library open this file.
            //
            ulRet = RPDLib_Open(pFile);
            if(ulRet != 0)
            {
                //
                // This is a DigiFile which is bound to a different device,
                // so we can not open this file.  Close the Rights/PD library.
                //
                RPDLib_Close(pFile);

                //
                // Close this file.
                //
                (pfnIoctl[pFile->ucDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                                           IOCTL_FS_CLOSE,
                                           (unsigned long)&pFile->uInternal,
                                           ulFlags, 0);

                //
                // Return a failure.
                //
                return(0);
            }

            //
            // See if this is a DigiFile.
            //
            if(pFile->ulHeaderSize == 0)
            {
                //
                // This is not a DigiFile, so close the Rights/PD library.
                //
                RPDLib_Close(pFile);
            }

            //
            // The file has been successfully opened.
            //
            ulRet = 1;
        }
#endif

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
FSCreate(tFile *pFile, unsigned long ulDrive,
         const unsigned short *pusFileName, unsigned long ulFileLength)
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
                                (unsigned long)pusFileName, ulFileLength);

    //
    // If the create succeeded, then increment the count of opened files.
    //
    if(ulRet)
    {
#ifdef SUPPORT_RIGHTS_PD
        //
        // The size of the header is always zero.
        //
        pFile->ulHeaderSize = 0;
#endif

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
    unsigned long ulCount;
#ifdef SUPPORT_RIGHTS_PD
    unsigned long ulPos;
#endif

    //
    // If the file was not opened for reading, then return an error.
    //
    if(!(pFile->ucFlags & FS_OPEN_READ))
    {
        return(0);
    }

#ifdef SUPPORT_RIGHTS_PD
    //
    // Get the current file pointer if this is a DigiFile.
    //
    if(pFile->ulHeaderSize != 0)
    {
        ulPos = FSTell(pFile);
    }
#endif

    //
    // Pass the read request to the entry point for the specified drive.
    //
    ulCount = (pfnIoctl[pFile->ucDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                                         IOCTL_FS_READ,
                                         (unsigned long)&pFile->uInternal,
                                         (unsigned long)pvBuffer, ulNumBytes);

#ifdef SUPPORT_RIGHTS_PD
    //
    // If this is a DigiFile, then decrypt the bytes that were just read.
    //
    if((pFile->ulHeaderSize != 0) && (ulCount != 0))
    {
        ulCount = RPDLib_Decrypt(pFile, (unsigned char *)pvBuffer, ulCount,
                                 ulPos);
    }
#endif

    //
    // Return the number of bytes read.
    //
    return(ulCount);
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
#ifdef SUPPORT_RIGHTS_PD
    unsigned long *pulPtr;
    unsigned long ulCount, ulIdx, ulData;
#endif

    //
    // If the file was not opened for reading, then return an error.
    //
    if(!(pFile->ucFlags & FS_OPEN_READ))
    {
        return(0);
    }

#ifdef SUPPORT_RIGHTS_PD
    //
    // See if this is a DigiFile.
    //
    if(pFile->ulHeaderSize != 0)
    {
        //
        // Read data from the file, which will also decrypt the bytes.
        //
        ulCount = FSRead(pFile, pvBuffer, ulNumBytes);

        //
        // Byte swap the bytes.
        //
        for(pulPtr = (unsigned long *)pvBuffer, ulIdx = 0;
            ulIdx < (ulCount / 4); )
        {
            //
            // Read the next word.
            //
            ulData = pulPtr[ulIdx];

            //
            // Write back the byte swapped word.
            //
            pulPtr[ulIdx++] = (((ulData << 24) & 0xff000000) |
                               ((ulData << 8) & 0x00ff0000) |
                               ((ulData >> 8) & 0x0000ff00) |
                               ((ulData >> 24) & 0x000000ff));
        }

        //
        // Return the number of bytes read.
        //
        return(ulCount);
    }
    else
#endif
    {
        //
        // Pass the read request to the entry point for the specified drive.
        //
        return((pfnIoctl[pFile->ucDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                                          IOCTL_FS_READ_BS,
                                          (unsigned long)&pFile->uInternal,
                                          (unsigned long)pvBuffer,
                                          ulNumBytes));
    }
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
                                      (unsigned long)&pFile->uInternal,
#ifdef SUPPORT_RIGHTS_PD
                                      ulPos + pFile->ulHeaderSize,
#else
                                      ulPos,
#endif
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
    unsigned long ulPos;

    //
    // Pass the pointer position request to the entry point for the specified
    // drive.
    //
    ulPos = (pfnIoctl[pFile->ucDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                                       IOCTL_FS_TELL,
                                       (unsigned long)&pFile->uInternal, 0, 0);

    //
    // Return the current file pointer.
    //
#ifdef SUPPORT_RIGHTS_PD
    return(ulPos - pFile->ulHeaderSize);
#else
    return(ulPos);
#endif
}

//****************************************************************************
//
// FSLength returns the length of the currently opened file.
//
//****************************************************************************
unsigned long
FSLength(tFile *pFile)
{
    unsigned long ulLength;

    //
    // Pass the length request to the entry point for the specified drive.
    //
    ulLength = (pfnIoctl[pFile->ucDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                                          IOCTL_FS_LENGTH,
                                          (unsigned long)&pFile->uInternal, 0,
                                          0);

    //
    // Return the length of the file.
    //
#ifdef SUPPORT_RIGHTS_PD
    return(ulLength - pFile->ulHeaderSize);
#else
    return(ulLength);
#endif
}

//****************************************************************************
//
// FSGetDate returns the modification date/time of the currently opened file.
//
//****************************************************************************
unsigned long
FSGetDate(tFile *pFile)
{
    //
    // Pass the modification date/time request to the entry point for the
    // specified drive.
    //
    return((pfnIoctl[pFile->ucDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                                      IOCTL_FS_GETDATE,
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

#ifdef SUPPORT_RIGHTS_PD
    //
    // See if this is a DigiFile.
    //
    if(pFile->ulHeaderSize != 0)
    {
        //
        // Close the Rights/PD library.
        //
        RPDLib_Close(pFile);
    }
#endif

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
FSDelete(unsigned long ulDrive, const unsigned short *pusFileName)
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
                               IOCTL_FS_DELETE, 0, (unsigned long)pusFileName,
                               0));
}

//****************************************************************************
//
// FSOpenDir opens the specified directory on the specified drive.
//
//****************************************************************************
unsigned long
FSOpenDir(tDir *pDir, unsigned long ulDrive, const unsigned short *pusDirName)
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
                                (unsigned long)pusDirName, 0);

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
FSReadDir(tDir *pDir, unsigned short *pusFileName, unsigned long ulType)
{
    //
    // Pass the read directory request to the entry point for the specified
    // drive.
    //
    return((pfnIoctl[pDir->ucDrive])(sFS.ucScratch, sFS.pucWriteScratch,
                                     IOCTL_FS_READDIR,
                                     (unsigned long)&pDir->uInternal,
                                     (unsigned long)pusFileName, ulType));
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
FSMakeDir(unsigned long ulDrive, const unsigned short *pusDirName)
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
                               IOCTL_FS_MAKEDIR, 0, (unsigned long)pusDirName,
                               0));
}

//****************************************************************************
//
// FSRemoveDir removes the specified directry from the specified drive.
//
//****************************************************************************
unsigned long
FSRemoveDir(unsigned long ulDrive, const unsigned short *pusDirName)
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
                               IOCTL_FS_REMOVEDIR, 0,
                               (unsigned long)pusDirName, 0));
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

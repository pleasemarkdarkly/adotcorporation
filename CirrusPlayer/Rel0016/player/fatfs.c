//****************************************************************************
//
// FATFS.C - File system-link interface to the FAT library.
//
// Copyright (c) 1999,2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include "fat/fat.h"
#include "globals.h"

//****************************************************************************
//
// Make sure that the definitions of FS_OPEN_??? and FAT_OPEN_??? match (and
// FS_TYPE_??? and FAT_TYPE_???).
//
//****************************************************************************
#if (FS_OPEN_READ != FAT_OPEN_READ)
#error FS_OPEN_READ is not the same as FAT_OPEN_READ!
#endif
#if (FS_OPEN_WRITE != FAT_OPEN_WRITE)
#error FS_OPEN_WRITE is not the same as FAT_OPEN_WRITE!
#endif
#if (FS_TYPE_FILE != FAT_TYPE_FILE)
#error FS_TYPE_FILE is not the same as FAT_TYPE_FILE!
#endif
#if (FS_TYPE_SUBDIR != FAT_TYPE_SUBDIR)
#error FS_TYPE_SUBDIR is not the same as FAT_TYPE_SUBDIR!
#endif

//****************************************************************************
//
// A file system-like entry point for a FAT12 or FAT16 file system.  This
// interface is very similar to the file system plug-in and is intended to be
// called by a file system plug-in for actual access to a FAT file system.
// There is a caller supplied function that is used to read and write blocks
// from the drive, allowing the actual blocks in the FAT file system to be
// stored on any media in any format.
//
//****************************************************************************
unsigned long
FATIoctl(tFAT *pFAT, unsigned char *pucScratch, unsigned char *pucWriteBuffer,
         unsigned long ulIoctl, unsigned long ulInstance,
         unsigned long ulParam1, unsigned long ulParam2)
{
    //
    // Determine what to do based on the specified IOCTL.
    //
    switch(ulIoctl)
    {
        //
        // Initialize the FAT file system.
        //
        case IOCTL_FS_INIT:
        {
            //
            // Initialize the FAT library.
            //
            return(FATInit(pFAT, pucScratch));
        }

        //
        // Return the media unique ID.
        //
        case IOCTL_FS_GETMEDIAID:
        {
            //
            // Get the media ID.
            //
            return(FATGetMediaID(pFAT, pucScratch, (unsigned char *)ulParam1,
                                 (unsigned long *)ulParam2));
        }

        //
        // Open the specified file.
        //
        case IOCTL_FS_OPEN:
        {
            //
            // See if we should create the file.
            //
            if(ulParam2 & FS_OPEN_CREATE)
            {
                //
                // Create a new file.
                //
                return(FATCreateFile(pFAT, pucScratch, pucWriteBuffer,
                                     (tFATFile *)ulInstance,
                                     (unsigned short *)ulParam1, 0));
            }
            else
            {
                //
                // Open an existing file.
                //
                return(FATOpenFile(pFAT, pucScratch, (tFATFile *)ulInstance,
                                   (unsigned short *)ulParam1, ulParam2));
            }
        }

        //
        // Create the specified file.
        //
        case IOCTL_FS_CREATE:
        {
            //
            // Create the file.
            //
            return(FATCreateFile(pFAT, pucScratch, pucWriteBuffer,
                                 (tFATFile *)ulInstance,
                                 (unsigned short *)ulParam1, ulParam2));
        }

        //
        // Read data from the currently opened file.
        //
        case IOCTL_FS_READ:
        case IOCTL_FS_READ_BS:
        {
            //
            // Read the data from the file.
            //
            return(FATRead(pFAT, (tFATFile *)ulInstance,
                           (unsigned char *)ulParam1, ulParam2,
                           (ulIoctl == IOCTL_FS_READ) ? 0 : 1));
        }

        //
        // Write data to the currently opened file.
        //
        case IOCTL_FS_WRITE:
        {
            //
            // Write the data to the file.
            //
            return(FATWrite(pFAT, pucScratch, pucWriteBuffer,
                            (tFATFile *)ulInstance, (unsigned char *)ulParam1,
                            ulParam2));
        }

        //
        // Seek to the specified position in the currently opened file.
        //
        case IOCTL_FS_SEEK:
        {
            //
            // Seek to the specified position.
            //
            return(FATSeek(pFAT, pucScratch, pucWriteBuffer,
                           (tFATFile *)ulInstance, ulParam1, ulParam2));
        }

        //
        // Return the current read/write pointer of the file.
        //
        case IOCTL_FS_TELL:
        {
            //
            // Get the current read/write pointer.
            //
            return(FATTell((tFATFile *)ulInstance));
        }

        //
        // Return the length of the currently opened file.
        //
        case IOCTL_FS_LENGTH:
        {
            //
            // Get the file length.
            //
            return(FATLength((tFATFile *)ulInstance));
        }

        //
        // Return the modification date/time of the currently opened file.
        //
        case IOCTL_FS_GETDATE:
        {
            //
            // Return the modification date/time.
            //
            return(FATGetDate(pFAT, pucScratch, (tFATFile *)ulInstance));
        }

        //
        // Close the currently opened file.
        //
        case IOCTL_FS_CLOSE:
        {
            //
            // Close the file.
            //
            return(FATCloseFile(pFAT, pucScratch, pucWriteBuffer,
                                (tFATFile *)ulInstance, ulParam1));
        }

        //
        // Deletes the specified file from the file system.
        //
        case IOCTL_FS_DELETE:
        {
            //
            // Delete the file.
            //
            return(FATDeleteFile(pFAT, pucScratch, pucWriteBuffer,
                                 (unsigned short *)ulParam1));
        }

        //
        // Opens the specified directory in the file system.
        //
        case IOCTL_FS_OPENDIR:
        {
            //
            // Open the directory.
            //
            return(FATOpenDirectory(pFAT, pucScratch, (tFATDir *)ulInstance,
                                    (unsigned short *)ulParam1));
        }

        //
        // Read the next directory entry from the file system.
        //
        case IOCTL_FS_READDIR:
        {
            //
            // Read the next entry from this directory.
            //
            return(FATReadDirectory(pFAT, pucScratch, (tFATDir *)ulInstance,
                                    (unsigned short *)ulParam1, ulParam2));
        }

        //
        // Close the directory of the file system.
        //
        case IOCTL_FS_CLOSEDIR:
        {
            //
            // Clsoe the directory.
            //
            return(FATCloseDirectory((tFATDir *)ulInstance));
        }

        //
        // Create a new directory in the file system.
        //
        case IOCTL_FS_MAKEDIR:
        {
            //
            // Create the directory.
            //
            return(FATCreateDirectory(pFAT, pucScratch, pucWriteBuffer,
                                      (unsigned short *)ulParam1));
        }

        //
        // Deletes the specifed directory from the file system.
        //
        case IOCTL_FS_REMOVEDIR:
        {
            //
            // Remove the directory.
            //
            return(FATRemoveDirectory(pFAT, pucScratch, pucWriteBuffer,
                                      (unsigned short *)ulParam1));
        }

        //
        // Determine the total capacity of the file system.
        //
        case IOCTL_FS_TOTALSPACE:
        {
            //
            // Get the total capacity of the file system.
            //
            return(FATTotalSpace(pFAT));
        }

        //
        // Determine the available capacity in the file system.
        //
        case IOCTL_FS_FREESPACE:
        {
            //
            // Get the free space.
            //
            return(FATFreeSpace(pFAT, pucScratch));
        }

        //
        // Format the file system.
        //
        case IOCTL_FS_FORMAT:
        {
            //
            // Format the drive.
            //
            return(FATFormat(pFAT, pucScratch, pucWriteBuffer));
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

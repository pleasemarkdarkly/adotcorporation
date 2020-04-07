//****************************************************************************
//
// UPDATE.C - Handles updating of the Internet audio player software.
//
// Copyright (c) 2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************
#include "../hwport.h"
#include "../hwdefs.h"

//****************************************************************************
//
// Prototypes for the functions in the USB code which we call.
//
//****************************************************************************
extern void USBEnable(void);
extern void USBDisable(void);
extern void USBForceConfiguration(unsigned long bConfigured);
extern void USBISR(void);
extern void USBDownload(unsigned char *pucData);

//****************************************************************************
//
// Prototypes for the functions in the user interface code which we will call.
//
//****************************************************************************
extern void UIEnable(void);
extern void UIDisable(void);
extern void UIRequestUpdate(void);
extern void UIUpdate(void);

//****************************************************************************
//
// Prototypes for the FLASH programming routines.
//
//****************************************************************************
extern void EraseFLASH(unsigned long ulBase, unsigned long ulLength,
                       unsigned long bIs16BitWide);
extern void WriteFLASH(unsigned char *pucData, unsigned long ulOffset,
                       unsigned long ulLength, unsigned long bIs16BitWide);

//****************************************************************************
//
// The variable which contains the address of the first unused word in
// internal SRAM.
//
//****************************************************************************
extern unsigned long ulEndOfRAM;

//****************************************************************************
//
// A boolean which is true if the program FLASH is 16 bits wide and false if
// it is 32 bits wide.
//
//****************************************************************************
unsigned long bIs16Bit;

//****************************************************************************
//
// The system flags used in the player, which is updated by the USB code.
// This exists here simply to satisfy the linker; the value in not used in any
// way.
//
//****************************************************************************
unsigned long ulSystemFlags;

//****************************************************************************
//
// The offset into the NOR FLASH where the next block of data will be written.
//
//****************************************************************************
unsigned long ulOffset;

//****************************************************************************
//
// Disable the IRQ interrupt.
//
//****************************************************************************
void
DisableIRQ(void)
{
    //
    // Do nothing since we never enable the IRQ interrupt.
    //
}

//****************************************************************************
//
// Enable the IRQ interrupt.
//
//****************************************************************************
void
EnableIRQ(void)
{
    //
    // Do nothing since we never enable the IRQ interrupt (we poll the USB
    // controller instead).
    //
}

//****************************************************************************
//
// Wait for and handle an interrupt from the USB controller.
//
//****************************************************************************
void
Halt(void)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;

    //
    // Wait until the USB interrupt has been asserted.
    //
    while(!(pulPtr[HwIntStatus >> 2] & HwIrqUSB))
    {
        //
        // Put the EP7209 into HALT mode.
        //
        pulPtr[HwHalt >> 2] = 0;
    }

    //
    // Call the USB interrupt handler.
    //
    USBISR();
}

//****************************************************************************
//
// SupportUpdate determines if we support updating the software.
//
//****************************************************************************
unsigned long
SupportUpdate(void)
{
    //
    // Since we are the update software, then we by implication do support
    // updates.
    //
    return(1);
}

//****************************************************************************
//
// StartUpdate branches to the code which performs the software update.
//
//****************************************************************************
void
StartUpdate(void)
{
    //
    // Indicate that an update is in progress.
    //
    UIUpdate();

    //
    // Since we are the update software, there is nothing to be done.  We
    // therefore simply return.
    //
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
    // We do not need to use the write scratch buffer, so don't bother
    // remembering where it is located.
    //
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
    //
    // We do not support a media unique ID for the NOR FLASH.
    //
    return(0);
}

//****************************************************************************
//
// FSGetMediaName returns the name of the drive.
//
//****************************************************************************
unsigned long
FSGetMediaName(unsigned long ulDrive, unsigned short *pusName,
               unsigned long *pulLength)
{
    //
    // We do not support a name for the NOR FLASH.
    //
    return(0);
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
    // There is only one drive (the NOR FLASH).
    //
    return(0);
}

//****************************************************************************
//
// FSOpen opens or creates the specified file on the specified drive.
//
//****************************************************************************
unsigned long
FSOpen(void *pFile, unsigned long ulDrive, const char *pcFileName,
       unsigned long ulFlags)
{
    //
    // We do not support opening a file, or creating a file through the open
    // interface.
    //
    return(0);
}

//****************************************************************************
//
// FSCreate creates the specified file with the given size on the specified
// drive.
//
//****************************************************************************
unsigned long
FSCreate(void *pFile, unsigned long ulDrive, const char *pcFileName,
         unsigned long ulFileLength)
{
    //
    // Make sure that the specified length is valid.
    //
    if((HwPlayerAddress + ulFileLength) > 0x00100000)
    {
        //
        // The image is too large to fit into the available FLASH, so return a
        // failure.
        //
        return(0);
    }

    //
    // Erase enough of the NOR FLASH to store the entire program image.
    //
    EraseFLASH(HwPlayerAddress, ulFileLength, bIs16Bit);

    //
    // Initialize the offset to the beginning of the player image.
    //
    ulOffset = HwPlayerAddress;

    //
    // Success.
    //
    return(1);
}

//****************************************************************************
//
// FSRead reads data from the opened file.
//
//****************************************************************************
unsigned long
FSRead(void *pFile, void *pvBuffer, unsigned long ulNumBytes)
{
    //
    // We do not support reading from the NOR FLASH.
    //
    return(0);
}

//****************************************************************************
//
// FSReadBS reads data from the opened file, performing a byte-swap on each
// 32-bit word in the read data.
//
//****************************************************************************
unsigned long
FSReadBS(void *pFile, void *pvBuffer, unsigned long ulNumBytes)
{
    //
    // We do not support reading from the NOR FLASH.
    //
    return(0);
}

//****************************************************************************
//
// FSWrite writes data to the opened file.
//
//****************************************************************************
unsigned long
FSWrite(void *pFile, void *pvBuffer, unsigned long ulNumBytes)
{
    //
    // Write the data into the NOR FLASH.
    //
    WriteFLASH((unsigned char *)pvBuffer, ulOffset, (ulNumBytes + 31) & ~31,
               bIs16Bit);

    //
    // Increment the offset by the number of bytes written.
    //
    ulOffset += ulNumBytes;

    //
    // Success.
    //
    return(ulNumBytes);
}

//****************************************************************************
//
// FSSeek moves the read/write pointer for the opened file to the specified
// position.
//
//****************************************************************************
unsigned long
FSSeek(void *pFile, unsigned long ulPos)
{
    //
    // We do not support seeking within the NOR FLASH.
    //
    return(0);
}

//****************************************************************************
//
// FSDriveNum returns the number of the drive on which the currently opened
// file resides.
//
//****************************************************************************
unsigned long
FSDriveNum(void *pFile)
{
    //
    // The current file is always on drive zero, since there is only one drive.
    //
    return(0);
}

//****************************************************************************
//
// FSTell returns the current read/write for the opened file.
//
//****************************************************************************
unsigned long
FSTell(void *pFile)
{
    //
    // We do not support reporting the current file pointer.
    //
    return(0);
}

//****************************************************************************
//
// FSLength returns the length of the currently opened file.
//
//****************************************************************************
unsigned long
FSLength(void *pFile)
{
    //
    // We do not support reporting the file length.
    //
    return(0);
}

//****************************************************************************
//
// FSGetDate returns the modification date/time of the currently opened file.
//
//****************************************************************************
unsigned long
FSGetDate(void *pFile)
{
    //
    // We do not support reporting the modification date/time.
    //
    return(0);
}

//****************************************************************************
//
// FSClose closes the currently opened file.
//
//****************************************************************************
unsigned long
FSClose(void *pFile)
{
    //
    // There is nothing to do on close, so return success.
    //
    return(1);
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
    // We do not support deleting files.
    //
    return(0);
}

//****************************************************************************
//
// FSOpenDir opens the specified directory on the specified drive.
//
//****************************************************************************
unsigned long
FSOpenDir(void *pDir, unsigned long ulDrive, const char *pcDirName)
{
    //
    // We do not support directory scans.
    //
    return(0);
}

//****************************************************************************
//
// FSReadDir reads the next directory entry from the currently opened
// directory.
//
//****************************************************************************
unsigned long
FSReadDir(void *pDir, char *pcFileName, unsigned long ulType)
{
    //
    // We do not support directory scans.
    //
    return(0);
}

//****************************************************************************
//
// FSCloseDir closes the currently opened directory.
//
//****************************************************************************
unsigned long
FSCloseDir(void *pDir)
{
    //
    // We do not support directory scans.
    //
    return(0);
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
    // We do not support creating directories.
    //
    return(0);
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
    // We do not support removing directories.
    //
    return(0);
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
    // We do not report the capacity of the NOR FLASH.
    //
    return(0);
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
    // We do not report the available capacity of the NOR FLASH.
    //
    return(0);
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
    // We do not support formatting the NOR FLASH.
    //
    return(0);
}

//****************************************************************************
//
// TimeSetSystemTime programs the given time (specified in seconds since
// January 1, 1970) into the system real time clock.
//
//****************************************************************************
void
TimeSetSystemTime(unsigned long ulSeconds)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;

    //
    // Program the given time into the EP7209's RTC.
    //
    pulPtr[HwRtcData >> 2] = ulSeconds;
}

//****************************************************************************
//
// TimeGetSystemTime returns the current value of the system real time clock
// (which is assumed to be the number of seconds since January 1, 1970).
//
//****************************************************************************
unsigned long
TimeGetSystemTime(void)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;

    //
    // Return the current value of the EP7209's RTC.
    //
    return(pulPtr[HwRtcData >> 2]);
}

//****************************************************************************
//
// Update handle the actual software update process.
//
//****************************************************************************
void
Update(unsigned long bConfigured)
{
    volatile unsigned long *pulPtr = (unsigned long *)HwBaseAddress;

    //
    // Determine if we have 16 or 32 bit wide program FLASH.
    //
    if((pulPtr[HwStatus >> 2] & 0x18000000) == 0x10000000)
    {
        //
        // The width of the program FLASH is 16 bits.
        //
        bIs16Bit = 1;
    }
    else
    {
        //
        // The width of the program FLASH is 32 bits.
        //
        bIs16Bit = 0;
    }

    //
    // Initialize the user interface.
    //
    UIEnable();

    //
    // If we are not configured (i.e. if we are not running as a result of the
    // main application calling us in response to an update request), then
    // handle configuring the USB controller and waiting for the update
    // request.
    //
    if(!bConfigured)
    {
        //
        // Enable the USB controller.
        //
        USBEnable();

        //
        // Indicate that an update needs to occur.
        //
        UIRequestUpdate();
    }
    else
    {
        //
        // Force the USB state to indicate that we are configured.
        //
        USBForceConfiguration(1);

        //
        // Enable the USB interrupt.
        //
        pulPtr[HwIntMask >> 2] |= HwIrqUSB;

        //
        // Indicate that an update is in progress.
        //
        UIUpdate();
    }

    //
    // Start the software update process.
    //
    USBDownload((unsigned char *)ulEndOfRAM);

    //
    // Disable the USB controller.
    //
    USBDisable();

    //
    // Disable the user interface.
    //
    UIDisable();
}

//****************************************************************************
//
// GLOBALS.H - Contains global definitions and function prototypes for the
//             Internet audio player application.
//
// Copyright (c) 1999-2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include "buffer/buffer.h"
#include "fat/fat.h"
#include "../config.h"

//****************************************************************************
//
// The following values define the flags contained in the system flags
// variable.
//
//****************************************************************************
#define SYSFLAG_KEYBOARD_COUNT_MASK             0x00000007
#define SYSFLAG_PROCESS_OUTPUT                  0x00000008
#define SYSFLAG_START_DOWNLOAD                  0x00000010
#define SYSFLAG_INPUT_ENABLED                   0x00000020
#define SYSFLAG_OUTPUT_ENABLED                  0x00000040
#define SYSFLAG_MEDIA_INSERT                    0x00000080
#define SYSFLAG_MEDIA_REMOVE                    0x00000100
#define SYSFLAG_REPEAT_MASK                     0x00000600
#define SYSFLAG_REPEAT_OFF                      0x00000000
#define SYSFLAG_REPEAT_ONE                      0x00000200
#define SYSFLAG_REPEAT_ALL                      0x00000400
#define SYSFLAG_REPEAT_SHIFT                    9

//****************************************************************************
//
// The following values are the IOCTLs which are sent to the individual codec
// drivers.
//
//****************************************************************************
enum
{
    IOCTL_CODEC_GETNAME,
    IOCTL_CODEC_GETARTIST,
    IOCTL_CODEC_GETTITLE,
    IOCTL_CODEC_GETBITRATE,
    IOCTL_CODEC_GETSAMPLERATE,
    IOCTL_CODEC_GETCHANNELS,
    IOCTL_CODEC_GETLENGTH,
    IOCTL_CODEC_GETTIME,
    IOCTL_CODEC_OPEN,
    IOCTL_CODEC_GETCAPTUREBUFFER,
    IOCTL_CODEC_SETBUFFER,
    IOCTL_CODEC_DECODE,
    IOCTL_CODEC_ENCODE,
    IOCTL_CODEC_SEEK,
    IOCTL_CODEC_CLOSE
};

//****************************************************************************
//
// The following are the flags passed to CodecOpen.
//
//****************************************************************************
#define CODEC_OPEN_ENCODE                       0x00000001
#define CODEC_OPEN_DECODE                       0x00000002

//****************************************************************************
//
// The following values are the IOCTLs which are sent to the individual
// file system drivers.
//
//****************************************************************************
enum
{
    IOCTL_FS_INIT,
    IOCTL_FS_GETMEDIAID,
    IOCTL_FS_OPEN,
    IOCTL_FS_CREATE,
    IOCTL_FS_READ,
    IOCTL_FS_READ_BS,
    IOCTL_FS_WRITE,
    IOCTL_FS_SEEK,
    IOCTL_FS_TELL,
    IOCTL_FS_LENGTH,
    IOCTL_FS_CLOSE,
    IOCTL_FS_DELETE,
    IOCTL_FS_OPENDIR,
    IOCTL_FS_READDIR,
    IOCTL_FS_CLOSEDIR,
    IOCTL_FS_MAKEDIR,
    IOCTL_FS_REMOVEDIR,
    IOCTL_FS_TOTALSPACE,
    IOCTL_FS_FREESPACE,
    IOCTL_FS_FORMAT
};

//****************************************************************************
//
// The following are the flags passed to FSOpen.
//
//****************************************************************************
#define FS_OPEN_READ                            0x00000001
#define FS_OPEN_WRITE                           0x00000002
#define FS_OPEN_CREATE                          0x00000004

//****************************************************************************
//
// The following are the types passed to FSReadDir.
//
//****************************************************************************
#define FS_TYPE_FILE                            0x00000001
#define FS_TYPE_SUBDIR                          0x00000002

//****************************************************************************
//
// The following structure contains the persistent state of an opened file.
// Only the file system specific state for the file system(s) actually used
// need to be defined in the union uInternal.
//
//****************************************************************************
typedef struct
{
    unsigned char ucDrive;
    unsigned char ucFlags;
    union
    {
        tFATFile sFATFile;
    } uInternal;
} tFile;

//****************************************************************************
//
// The following structure contains the persistent state of an opened
// directory.  Only the file system specific state for the file system(s)
// actually used need to be defined in union uInternal.
//
//****************************************************************************
typedef struct
{
    unsigned char ucDrive;
    union
    {
        tFATDir sFATDir;
    } uInternal;
} tDir;

//****************************************************************************
//
// The following structure contains a time broken down into it's component
// pieces (i.e. month, day, year, hour, minute, second, and day of week).
//
//****************************************************************************
typedef struct
{
    short sYear;
    char cMonth;
    char cDay;
    char cHour;
    char cMin;
    char cSec;
    char cWeekday;
} tTime;

//****************************************************************************
//
// The following are the various modes which can be passed to UISetMode.
//
//****************************************************************************
enum
{
    MODE_POWER_OFF,
    MODE_POWER_ON,
    MODE_DOWNLOAD,
    MODE_STOP,
    MODE_PLAY,
    MODE_PAUSE,
    MODE_RECORD
};

//****************************************************************************
//
// The following are the bitmasks for the logical buttons which can be
// returned by UIGetButtons.
//
//****************************************************************************
#define BUTTON_PLAY                             0x00000001
#define BUTTON_PAUSE                            0x00000002
#define BUTTON_STOP                             0x00000004
#define BUTTON_FFWD                             0x00000008
#define BUTTON_REW                              0x00000010
#define BUTTON_NEXT                             0x00000020
#define BUTTON_PREV                             0x00000040
#define BUTTON_POWER                            0x00000080
#define BUTTON_RECORD                           0x00000100

//****************************************************************************
//
// Definitions for the bit fields of each entry of the pusTracks array.
//
//****************************************************************************
#define TRACK_FILE_MASK                         0x03ff
#define TRACK_CODEC_MASK                        0x7c00
#define TRACK_DRIVE_MASK                        0x8000
#define TRACK_FILE_SHIFT                        0
#define TRACK_CODEC_SHIFT                       10
#define TRACK_DRIVE_SHIFT                       15

//****************************************************************************
//
// Function prototypes and global variables.
//
//****************************************************************************

//
// From aac.c
//
extern unsigned long AACIoctl(unsigned long ulIoctl, unsigned long ulParam1,
                              unsigned long ulParam2, unsigned long ulParam3,
                              unsigned long ulParam4);

//
// From acelpnet.c
//
extern unsigned long ACELPnetIoctl(unsigned long ulIoctl,
                                   unsigned long ulParam1,
                                   unsigned long ulParam2,
                                   unsigned long ulParam3,
                                   unsigned long ulParam4);
//
// From codec.c
//
extern unsigned long CodecOpen(unsigned long ulCodec, unsigned long ulFlags,
                               tFile *pFile);
extern unsigned long CodecGetIndex(unsigned long *pulIndex);
extern unsigned long CodecGetName(char **ppcName);
extern unsigned long CodecGetNameByIndex(unsigned long ulIndex,
                                         char **ppcName);
extern unsigned long CodecGetArtist(char **ppcName);
extern unsigned long CodecGetTitle(char **ppcName);
extern unsigned long CodecGetBitRate(unsigned long *pulBitRate);
extern unsigned long CodecGetSampleRate(unsigned long *pulSampleRate);
extern unsigned long CodecGetChannels(unsigned long *pulChannels);
extern unsigned long CodecGetLength(unsigned long *pulLength);
extern unsigned long CodecGetTime(unsigned long *pulTime);
extern unsigned long CodecGetCaptureBuffer(short **ppsBuffer, long *plLength);
extern unsigned long CodecSetBuffer(BufferState *psBuffer);
extern unsigned long CodecDecode(void);
extern unsigned long CodecEncode(void);
extern unsigned long CodecSeek(unsigned long ulTime);
extern unsigned long CodecClose(void);

//
// From dai.c
//
extern void DAIInit(void);
extern void DAIEnable(void);
extern void DAIDisable(void);
extern BufferState *DAIGetPlayBuffer(void);
extern BufferState *DAIGetRecordBuffer(void);

//
// From fs.c
//
extern void FSInit(void);
extern void FSSetWriteScratch(unsigned char *pucWriteScratch);
extern unsigned long FSGetMediaID(unsigned long ulDrive,
                                  unsigned char *pucMediaIDBuffer,
                                  unsigned long *pulLength);
extern unsigned long FSNumDrives(void);
extern unsigned long FSOpen(tFile *pFile, unsigned long ulDrive,
                            const char *pcFileName, unsigned long ulFlags);
extern unsigned long FSCreate(tFile *pFile, unsigned long ulDrive,
                              const char *pcFileName,
                              unsigned long ulFileLength);
extern unsigned long FSRead(tFile *pFile, void *pvBuffer,
                            unsigned long ulNumBytes);
extern unsigned long FSReadBS(tFile *pFile, void *pvBuffer,
                              unsigned long ulNumBytes);
extern unsigned long FSWrite(tFile *pFile, void *pvBuffer,
                             unsigned long ulNumBytes);
extern unsigned long FSSeek(tFile *pFile, unsigned long ulPos);
extern unsigned long FSDriveNum(tFile *pFile);
extern unsigned long FSTell(tFile *pFile);
extern unsigned long FSLength(tFile *pFile);
extern unsigned long FSClose(tFile *pFile);
extern unsigned long FSDelete(unsigned long ulDrive, const char *pcFileName);
extern unsigned long FSOpenDir(tDir *pDir, unsigned long ulDrive,
                               const char *pcDirName);
extern unsigned long FSReadDir(tDir *pDir, char *pcFileName,
                               unsigned long ulType);
extern unsigned long FSCloseDir(tDir *pDir);
extern unsigned long FSMakeDir(unsigned long ulDrive, const char *pcDirName);
extern unsigned long FSRemoveDir(unsigned long ulDrive, const char *pcDirName);
extern unsigned long FSTotalSpace(unsigned long ulDrive);
extern unsigned long FSFreeSpace(unsigned long ulDrive);
extern unsigned long FSFormat(unsigned long ulDrive);

//
// From g721.c
//
extern unsigned long G721Ioctl(unsigned long ulIoctl, unsigned long ulParam1,
                               unsigned long ulParam2, unsigned long ulParam3,
                               unsigned long ulParam4);

//
// From i2c.c
//
extern void I2CWrite(unsigned char ucAddr, unsigned char ucReg,
                     unsigned char ucValue);

//
// From id3.c
//
extern void ID3FindTags(tFile *pFile, unsigned long *pulStart,
                        unsigned long *pulLength, unsigned char *pucBuffer);
extern unsigned long ID3GetTagValue(tFile *pFile, unsigned long ulStart,
                                    unsigned long ulLength,
                                    unsigned char *pucBuffer, char *pcTag,
                                    unsigned char *pucValue,
                                    unsigned long ulValueLength);

//
// From input.c
//
extern unsigned long InputEnable(long lSampleRate, short *psBuffer,
                                 long lLength);
extern void InputDisable(void);
extern BufferState *InputGetOutputBuffer(void);

//
// From irq.s
//
extern void DisableFIQ(void);
extern void EnableFIQ(void);
extern void DisableIRQ(void);
extern void EnableIRQ(void);
extern void DisableTimer(void);
extern void EnableTimer(void);

//
// From main.c
//
extern unsigned short pusTracks[100];
extern unsigned long ulTrack;
extern void ScanAllTracks(unsigned long ulNumTracks, unsigned long ulDrive);
extern void MediaRemoved(void);
extern void MediaInserted(void);

//
// From mp3.c
//
extern unsigned long MP3Ioctl(unsigned long ulIoctl, unsigned long ulParam1,
                              unsigned long ulParam2, unsigned long ulParam3,
                              unsigned long ulParam4);

//
// From msadpcm.c
//
extern unsigned long MSADPCMIoctl(unsigned long ulIoctl,
                                  unsigned long ulParam1,
                                  unsigned long ulParam2,
                                  unsigned long ulParam3,
                                  unsigned long ulParam4);

//
// From nand.c
//
extern unsigned long NANDIoctl(unsigned char *pucScratch,
                               unsigned char *pucWriteBuffer,
                               unsigned long ulIoctl, unsigned long ulInstance,
                               unsigned long ulParam1, unsigned long ulParam2);

//
// From nandsupp.s
//
extern unsigned char NANDGetID(unsigned long ulNANDAddress);
extern void NANDRead_256(unsigned long ulNANDAddress, unsigned long ulPage,
                         unsigned char *pucData, unsigned char *pucRedundant);
extern void NANDRead_512_3(unsigned long ulNANDAddress, unsigned long ulPage,
                           unsigned char *pucData,
                           unsigned char *pucRedundant);
extern void NANDRead_512_4(unsigned long ulNANDAddress, unsigned long ulPage,
                           unsigned char *pucData,
                           unsigned char *pucRedundant);
extern void NANDWrite_256(unsigned long ulNANDAddress, unsigned long ulPage,
                          unsigned char *pucData, unsigned char *pucRedundant);
extern void NANDWrite_512_3(unsigned long ulNANDAddress, unsigned long ulPage,
                            unsigned char *pucData,
                            unsigned char *pucRedundant);
extern void NANDWrite_512_4(unsigned long ulNANDAddress, unsigned long ulPage,
                            unsigned char *pucData,
                            unsigned char *pucRedundant);
extern void NANDErase_16(unsigned long ulNANDAddress, unsigned long ulBlock);
extern void NANDErase_32_3(unsigned long ulNANDAddress, unsigned long ulBlock);
extern void NANDErase_32_4(unsigned long ulNANDAddress, unsigned long ulBlock);
extern void NANDReadRedt_256(unsigned long ulNANDAddress, unsigned long ulPage,
                             unsigned char *pucRedundant);
extern void NANDReadRedt_512_3(unsigned long ulNANDAddress,
                               unsigned long ulPage,
                               unsigned char *pucRedundant);
extern void NANDReadRedt_512_4(unsigned long ulNANDAddress,
                               unsigned long ulPage,
                               unsigned char *pucRedundant);
extern void NANDWriteRedt_256(unsigned long ulNANDAddress,
                              unsigned long ulPage,
                              unsigned char *pucRedundant);
extern void NANDWriteRedt_512_3(unsigned long ulNANDAddress,
                                unsigned long ulPage,
                                unsigned char *pucRedundant);
extern void NANDWriteRedt_512_4(unsigned long ulNANDAddress,
                                unsigned long ulPage,
                                unsigned char *pucRedundant);
extern void NANDWaitTilNotBusy(unsigned long ulNANDAddress);

//
// From output.c
//
extern void OutputInit(void);
extern void OutputEnable(void);
extern void OutputDisable(void);
extern BufferState *OutputGetInputBuffer(void);
extern unsigned long OutputSetFormat(long lSampleRate, long lChannels);
extern void OutputSetRate(long lInputSampleRate, long bIsMono);
extern void OutputSetTone(long lTreble, long lBass, long lGain);
extern void OutputSetTreble(long lTreble);
extern void OutputSetBass(long lBass);
extern void OutputSetGain(long lGain);
extern void OutputSetVolume(long lGain);

//
// From play.c
//
extern unsigned long Play(void);

//
// From power.s
//
extern void Standby(void);
extern void Halt(void);

//
// From record.c
//
extern unsigned long Record(void);

//
// From smartmed.c
//
extern unsigned long SMIoctl(unsigned char *pucScratch,
                             unsigned char *pucWriteBuffer,
                             unsigned long ulIoctl, unsigned long ulInstance,
                             unsigned long ulParam1, unsigned long ulParam2);
//
// From support.s
//
extern void memcpy(void *pvDest, const void *pvSrc, int iCount);
extern void memmove(void *pvDest, const void *pvSrc, int iCount);
extern int memcmp(const void *pvSrc1, const void *pvSrc2, int iCount);
extern int strcmp(const char *pcSrc1, const char *pcSrc2);
extern void memset(void *pvDest, char cChar, int iCount);
extern int strlen(const char *pvSrc);

//
// From time.c
//
extern void TimeSetSystemTime(unsigned long ulSeconds);
extern unsigned long TimeGetSystemTime(void);
extern void TimeFromSeconds(tTime *pTime, unsigned long ulSeconds);
extern void TimeToSeconds(tTime *pTime, unsigned long *pulSeconds);
extern void TimeIncrementMonth(tTime *pTime);
extern void TimeDecrementMonth(tTime *pTime);
extern void TimeIncrementDay(tTime *pTime);
extern void TimeDecrementDay(tTime *pTime);
extern void TimeIncrementYear(tTime *pTime);
extern void TimeDecrementYear(tTime *pTime);
extern void TimeIncrementHour(tTime *pTime);
extern void TimeDecrementHour(tTime *pTime);
extern void TimeIncrementMinute(tTime *pTime);
extern void TimeDecrementMinute(tTime *pTime);
extern void TimeIncrementSecond(tTime *pTime);
extern void TimeDecrementSecond(tTime *pTime);

//
// From ui.c
//
extern void UIInit(void);
extern void UISetMode(unsigned long ulMode);
extern void UIFileLoaded(char *pcFileName, unsigned long ulTrack);
extern void UIFileUnloaded(void);
extern void UISetCurrentTime(unsigned long ulMS);
extern unsigned long UIGetButtons(void);

//
// From usb.c
//
extern void USBEnable(void);
extern void USBDisable(void);
extern void USBDownload(unsigned char *pucData);

//
// From vectors.s
//
extern volatile unsigned long ulSystemFlags;
extern unsigned long ulEndOfRAM;
extern unsigned long SupportUpdate(void);
extern void StartUpdate(void);

//
// From wma.c
//
extern unsigned long WMAIoctl(unsigned long ulIoctl, unsigned long ulParam1,
                              unsigned long ulParam2, unsigned long ulParam3,
                              unsigned long ulParam4);

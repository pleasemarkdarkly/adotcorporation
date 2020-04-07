//****************************************************************************
//
// FTL.H - Public definitions for the FLASH translation layer code.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
//
//****************************************************************************

#ifndef _FTL_H
#define _FTL_H

//****************************************************************************
//
// The following values are the IOCTLs which are sent to the FLASH drivers by
// the FTL layer.
//
//****************************************************************************
enum
{
    IOCTL_FLASH_GETID,
    IOCTL_FLASH_READ,
    IOCTL_FLASH_WRITE,
    IOCTL_FLASH_ERASE,
    IOCTL_FLASH_READ_REDT,
    IOCTL_FLASH_WRITE_REDT
};

//****************************************************************************
//
// The following are the flags passed to the FLASH drivers by the FTL layer.
//
//****************************************************************************
#define FLASH_SIZE_MASK                         0x0f
#define FLASH_SIZE_1MB                          0x00
#define FLASH_SIZE_2MB                          0x01
#define FLASH_SIZE_4MB                          0x02
#define FLASH_SIZE_8MB                          0x03
#define FLASH_SIZE_16MB                         0x04
#define FLASH_SIZE_32MB                         0x05
#define FLASH_SIZE_64MB                         0x06
#define FLASH_SIZE_128MB                        0x07
#define FLASH_ADDR_4_CYCLE                      0x10
#define FLASH_BS16                              0x20
#define FLASH_PS256                             0x40

//****************************************************************************
//
// The following structure contains the persistent state of the FLASH
// translation layer.
//
//****************************************************************************
#ifndef _FTL_I_H
typedef struct
{
    //
    // A pointer to the driver for performing physical access to the FLASH
    // device.
    //
    unsigned long (*pfnFlashDriver)(unsigned long ulIoctl,
                                    unsigned long ulParam1,
                                    unsigned long ulParam2,
                                    unsigned long ulParam3,
                                    unsigned long ulParam4);

    //
    // The internal, persistent state of the FTL code.
    //
    unsigned char ucInternal[1048];
} tFTL;
#endif

//****************************************************************************
//
// Function prototypes and global variables.
//
//****************************************************************************
extern unsigned long FTLIoctl(void *pvState, unsigned char *pucScratch,
                              unsigned long ulIoctl, unsigned long ulParam1,
                              unsigned long ulParam2, unsigned long ulParam3);

#endif // _FTL_H

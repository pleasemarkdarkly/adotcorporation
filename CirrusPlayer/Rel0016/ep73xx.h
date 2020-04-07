//****************************************************************************
//
// EP73XX.H - Memory and register definitions for the EP73xx family.
//
// Copyright (c) 1998,1999,2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************

//****************************************************************************
//
// The EP73xx is based on the EP72xx, so include those definitions.
//
//****************************************************************************
#include "ep72xx.h"

//****************************************************************************
//
// The EP73xx internal registers.
//
//****************************************************************************

//
// The offsets to the registers added in the EP73xx.
//
#define HwSDRAMConfig                           0x00002300
#define HwSDRAMRefresh                          0x00002340
#define HwDAI64Fs                               0x00002600
#define HwPLLWrite                              0x00002610
#define HwPLLRead                               0x0000a5a8

//
// Definitions of the bit fields in the HwSDRAMConfig register.
//
#define HwSDRAMConfigCASLatencyMask             0x00000003
#define HwSDRAMConfigCASLatency2                0x00000002
#define HwSDRAMConfigCASLatency3                0x00000003
#define HwSDRAMConfigCapacityMask               0x00000060
#define HwSDRAMConfigCapacity16                 0x00000000
#define HwSDRAMConfigCapacity64                 0x00000020
#define HwSDRAMConfigCapacity128                0x00000040
#define HwSDRAMConfigCapacity256                0x00000060
#define HwSDRAMConfigWidthMask                  0x00000180
#define HwSDRAMConfigWidth4                     0x00000000
#define HwSDRAMConfigWidth8                     0x00000080
#define HwSDRAMConfigWidth16                    0x00000100
#define HwSDRAMConfigWidth32                    0x00000180
#define HwSDRAMConfigClockEn                    0x00000200
#define HwSDRAMConfigEn                         0x00000400

//
// Definitions of the bit fields in the HwSDRAMRefresh register.
//
#define HwSDRAMRefreshMask                      0x0000ffff
#define HwSDRAMRefreshShift                     0

//
// Definitions of the bit fields in the HwDAI64Fs register.
//
#define HwDAI64FsEn                             0x00000001
#define HwDAI64FsClockEn                        0x00000002
#define HwDAI64FsClkSrcMask                     0x00000004
#define HwDAI64FsClk74MHz                       0x00000000
#define HwDAI64FsClk11MHz                       0x00000004
#define HwDAI64FsMclkEn                         0x00000008
#define HwDAI64FsLoopback                       0x00000020
#define HwDAI64FsDivisorMask                    0x00007f00
#define HwDAI64FsDivisor32kHz                   0x00000900
#define HwDAI64FsDivisor44kHz                   0x00000200
#define HwDAI64FsDivisor48kHz                   0x00000600
#define HwDAI64FsDivisorShift                   8

//
// Definitions of the bit fields in the HwPLLWrite and HwPLLRead registers.
//
#define HwPLLMultMask                           0xff000000
#define HwPLLMultShift                          24

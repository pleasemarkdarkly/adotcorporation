//****************************************************************************
//
// EP72XX.H - Memory and register definitions for the EP72xx family.
//
// Copyright (c) 1998,1999,2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************

//****************************************************************************
//
// The base addresses for the various memories in the system.
//
//****************************************************************************
#define HwProgramAddress                        0x00000000
#define HwSRAMAddress                           0x00100000
#define HwBaseAddress                           0x00200000
#define HwNANDAddress                           0x00300000
#define HwUSBAddress                            0x00400000
#define HwLCDAddress                            0x00500000
#define HwMMCAddress                            0x00600000

//****************************************************************************
//
// The EP72xx internal registers.
//
//****************************************************************************

//
// The offset to each individual register in the EP72xx.
//
#define HwPortABCD                              0x00000000
#define HwPortA                                 0x00000000
#define HwPortB                                 0x00000001
#define HwPortD                                 0x00000003
#define HwDdrABCD                               0x00000040
#define HwDdrA                                  0x00000040
#define HwDdrB                                  0x00000041
#define HwDdrD                                  0x00000043
#define HwPortE                                 0x00000080
#define HwDdrE                                  0x000000c0
#define HwControl                               0x00000100
#define HwStatus                                0x00000140
#define HwMemConfig1                            0x00000180
#define HwMemConfig2                            0x000001c0
#define HwEDORefresh                            0x00000200
#define HwIntStatus                             0x00000240
#define HwIntMask                               0x00000280
#define HwLcdControl                            0x000002c0
#define HwTimer1Data                            0x00000300
#define HwTimer2Data                            0x00000340
#define HwRtcData                               0x00000380
#define HwRtcMatch                              0x000003c0
#define HwPumpControl                           0x00000400
#define HwCodecData                             0x00000440
#define HwUartData                              0x00000480
#define HwUartControl                           0x000004c0
#define HwSpiData                               0x00000500
#define HwPaletteLSW                            0x00000540
#define HwPaletteMSW                            0x00000580
#define HwStartFlagClear                        0x000005c0
#define HwBatteryLowEOI                         0x00000600
#define HwMediaChangedEOI                       0x00000640
#define HwTickEOI                               0x00000680
#define HwTimer1EOI                             0x000006c0
#define HwTimer2EOI                             0x00000700
#define HwRtcMatchEOI                           0x00000740
#define HwUartEOI                               0x00000780
#define HwCodecEOI                              0x000007c0
#define HwHalt                                  0x00000800
#define HwStandby                               0x00000840
#define HwLcdFrameBuffer                        0x00001000
#define HwControl2                              0x00001100
#define HwStatus2                               0x00001140
#define HwIntStatus2                            0x00001240
#define HwIntMask2                              0x00001280
#define HwUart2Data                             0x00001480
#define HwUart2Control                          0x000014c0
#define HwKeyboardEOI                           0x00001700
#define HwDAIControl                            0x00002000
#define HwDAIData0                              0x00002040
#define HwDAIData1                              0x00002080
#define HwDAIData2                              0x000020c0
#define HwDAIStatus                             0x00002100
#define HwControl3                              0x00002200
#define HwIntStatus3                            0x00002240
#define HwIntMask3                              0x00002280
#define HwLEDFlash                              0x000022c0

//
// Definitions of the bit fields in the HwControl register.
//
#define HwControlColumnDrive                    0x0000000f
#define HwControlColumnAllHigh                  0x00000000
#define HwControlColumnAllLow                   0x00000001
#define HwControlColumnAllTriState              0x00000002
#define HwControlColumn0High                    0x00000008
#define HwControlColumn1High                    0x00000009
#define HwControlColumn2High                    0x0000000a
#define HwControlColumn3High                    0x0000000b
#define HwControlColumn4High                    0x0000000c
#define HwControlColumn5High                    0x0000000d
#define HwControlColumn6High                    0x0000000e
#define HwControlColumn7High                    0x0000000f
#define HwControlTimer1PreOrFree                0x00000010
#define HwControlTimer1K512OrK2                 0x00000020
#define HwControlTimer2PreOrFree                0x00000040
#define HwControlTimer2K512OrK2                 0x00000080
#define HwControlUartEnable                     0x00000100
#define HwControlBuzzerToggle                   0x00000200
#define HwControlBuzzerTimer1OrToggle           0x00000400
#define HwControlDebugEnable                    0x00000800
#define HwControlLcdEnable                      0x00001000
#define HwControlCodecTxEnable                  0x00002000
#define HwControlCodecRxEnable                  0x00004000
#define HwControlUartSirEnable                  0x00008000
#define HwControlSpiClockSelect                 0x00030000
#define HwControlSpiClock4KHz                   0x00000000
#define HwControlSpiClock16KHz                  0x00010000
#define HwControlSpiClock64KHz                  0x00020000
#define HwControlSpiClock128KHz                 0x00030000
#define HwControlExpClockEnable                 0x00040000
#define HwControlWakeupDisable                  0x00080000
#define HwControlIrDAWidthEnable                0x00100000

//
// Definitions of the bit fields in the HwStatus register.
//
#define HwStatusMediaChangedState               0x00000001
#define HwStatusDcPresent                       0x00000002
#define HwStatusWakeUpState                     0x00000004
#define HwStatusWakeUpFlag                      0x00000008
#define HwStatusLcdType                         0x000000f0
#define HwStatusCts                             0x00000100
#define HwStatusDsr                             0x00000200
#define HwStatusDcd                             0x00000400
#define HwStatusUartTxBusy                      0x00000800
#define HwStatusBatteryChangedFlag              0x00001000
#define HwStatusResetFlag                       0x00002000
#define HwStatusPowerFailFlag                   0x00004000
#define HwStatusColdStartFlag                   0x00008000
#define HwStatusRtcLSB                          0x003f0000
#define HwStatusUartRxFifoEmpty                 0x00400000
#define HwStatusUartTxFifoFull                  0x00800000
#define HwStatusCodecRxFifoEmpty                0x01000000
#define HwStatusCodecTxFifoFull                 0x02000000
#define HwStatusSpiBusy                         0x04000000
#define HwStatusBootWidthMask                   0x18000000
#define HwStatusBootWidth32                     0x00000000
#define HwStatusBootWidth8                      0x08000000
#define HwStatusBootWidth16                     0x10000000
#define HwStatusID                              0x20000000
#define HwStatusVersionID                       0xc0000000

//
// Definitions of the bit fields in the HwMemConfig1 and HwMemConfig2
// registers.
//
#define HwMemConfigBusWidth                     0x00000003
#define HwMemConfigBusWidth32                   0x00000000
#define HwMemConfigBusWidth16                   0x00000001
#define HwMemConfigBusWidth8                    0x00000002
#define HwMemConfigBusWidthPcmcia               0x00000003
#define HwMemConfigRaWaitState                  0x0000000c
#define HwMemConfigRaWaitState4                 0x00000000
#define HwMemConfigRaWaitState3                 0x00000004
#define HwMemConfigRaWaitState2                 0x00000008
#define HwMemConfigRaWaitState1                 0x0000000c
#define HwMemConfigSaWaitState                  0x00000030
#define HwMemConfigSaWaitState3                 0x00000000
#define HwMemConfigSaWaitState2                 0x00000010
#define HwMemConfigSaWaitState1                 0x00000020
#define HwMemConfigSaWaitState0                 0x00000030
#define HwMemConfigWaitStateMask                0x0000003c
#define HwMemConfigWaitState8_3                 0x00000000
#define HwMemConfigWaitState7_3                 0x00000004
#define HwMemConfigWaitState6_3                 0x00000008
#define HwMemConfigWaitState5_3                 0x0000000c
#define HwMemConfigWaitState4_2                 0x00000010
#define HwMemConfigWaitState3_2                 0x00000014
#define HwMemConfigWaitState2_2                 0x00000018
#define HwMemConfigWaitState1_2                 0x0000001c
#define HwMemConfigWaitState8_1                 0x00000020
#define HwMemConfigWaitState7_1                 0x00000024
#define HwMemConfigWaitState6_1                 0x00000028
#define HwMemConfigWaitState5_1                 0x0000002c
#define HwMemConfigWaitState4_0                 0x00000030
#define HwMemConfigWaitState3_0                 0x00000034
#define HwMemConfigWaitState2_0                 0x00000038
#define HwMemConfigWaitState1_0                 0x0000003c
#define HwMemConfigSeqAccessEnable              0x00000040
#define HwMemConfigExpClkEnable                 0x00000080

//
// Definitions of the bit fields in the HwEDORefresh register.
//
#define HwEDORefreshCountMask                   0x0000007f
#define HwEDORefreshEnable                      0x00000080
#define HwEDORefreshCountShift                  0

//
// Definitions of the bit fields in the HwIntStatus and HwIntMask registers.
//
#define HwFiqExt                                0x00000001
#define HwFiqBatteryLow                         0x00000002
#define HwFiqWatchDog                           0x00000004
#define HwFiqMediaChange                        0x00000008
#define HwIrqCodec                              0x00000010
#define HwIrqExt1                               0x00000020
#define HwIrqExt2                               0x00000040
#define HwIrqExt3                               0x00000080
#define HwIrqTimer1                             0x00000100
#define HwIrqTimer2                             0x00000200
#define HwIrqRtcMatch                           0x00000400
#define HwIrqTick                               0x00000800
#define HwIrqUartTx                             0x00001000
#define HwIrqUartRx                             0x00002000
#define HwIrqUartModem                          0x00004000
#define HwIrqSpi                                0x00008000

//
// Definitions of the bit fields in the HwLcdControl register.
//
#define HwLcdControlBufferSize                  0x00001fff
#define HwLcdControlLineLength                  0x0007e000
#define HwLcdControlPixelPrescale               0x01f80000
#define HwLcdControlAcPrescale                  0x3e000000
#define HwLcdControlGreyEnable                  0x40000000
#define HwLcdControlGrey4Or2                    0x80000000
#define HwLcdControlBufferSizeShift             0
#define HwLcdControlLineLengthShift             13
#define HwLcdControlPixelPrescaleShift          19
#define HwLcdControlAcPrescaleShift             25

//
// Definitions of the bit fields in the HwPumpControl register.
//
#define HwPumpControlVhBat                      0x0000000f
#define HwPumpControlVhDc                       0x000000f0
#define HwPumpControlLcd                        0x00000f00

//
// Definitions of the bit fields in the HwUartData register.
//
#define HwUartDataFrameErr                      0x00000100
#define HwUartDataParityErr                     0x00000200
#define HwUartDataOverrunErr                    0x00000400

//
// Definitions of the bit fields in the HwUartControl register.
//
#define HwUartControlRate                       0x00000fff
#define HwUartControlRate115200                 0x00000001
#define HwUartControlRate76800                  0x00000002
#define HwUartControlRate57600                  0x00000003
#define HwUartControlRate38400                  0x00000005
#define HwUartControlRate28800                  0x00000007
#define HwUartControlRate19200                  0x0000000b
#define HwUartControlRate14400                  0x0000000f
#define HwUartControlRate9600                   0x00000017
#define HwUartControlRate4800                   0x0000002f
#define HwUartControlRate2400                   0x0000005f
#define HwUartControlRate1200                   0x000000bf
#define HwUartControlRate600                    0x0000017f
#define HwUartControlRate300                    0x000002ff
#define HwUartControlRate150                    0x000005ff
#define HwUartControlRate110                    0x0000082e
#define HwUartControlBreak                      0x00001000
#define HwUartControlParityEnable               0x00002000
#define HwUartControlParityEven                 0x00004000
#define HwUartControlTwoStopBits                0x00008000
#define HwUartControlFifoEnable                 0x00010000
#define HwUartControlDataLength                 0x00060000
#define HwUartControlDataLength5                0x00000000
#define HwUartControlDataLength6                0x00020000
#define HwUartControlDataLength7                0x00040000
#define HwUartControlDataLength8                0x00060000

//
// Definitions of the bit fields in the HwSpiData register.
//
#define HwSpiDataConfig                         0x000000ff
#define HwSpiDataFrameLength                    0x00001f00
#define HwSpiDataSampleClockFreerun             0x00002000
#define HwSpiDataTxFrame                        0x00004000

//
// Definitions of the bit fields in the HwControl2 register.
//
#define HwControl2CodecEnable                   0x00000001
#define HwControl2Kbd6                          0x00000002
#define HwControl2DRAM16                        0x00000004
#define HwControl2KeyboardWakeupEnable          0x00000008
#define HwControl2PCMCIA1Enable                 0x00000020
#define HwControl2PCMCIA2Enable                 0x00000040
#define HwControl2Uart2Enable                   0x00000100
#define HwControl2OSTimerEnable                 0x00001000
#define HwControl2ClockSelect                   0x00002000
#define HwControl2BuzzerSelect                  0x00004000

//
// Definitions of the bit fields in the HwStatus2 register.
//
#define HwStatus2ClockMode                      0x00000040
#define HwStatus2Uart2TxBusy                    0x00000800
#define HwStatus2Uart2RxFifoEmpty               0x00400000
#define HwStatus2Uart2TxFifoFull                0x00800000

//
// Definitions of the bit fields in the HwIntStatus2 and HwIntMask2 registers.
//
#define HwIrqKeyboard                           0x00000001
#define HwIrqUart2Tx                            0x00001000
#define HwIrqUart2Rx                            0x00002000

//
// Definitions of the bit fields in the HwDAIControl register.
//
#define HwDAIControlDAIEN                       0x00010000
#define HwDAIControlECS                         0x00020000
#define HwDAIControlLCTM                        0x00080000
#define HwDAIControlLCRM                        0x00100000
#define HwDAIControlRCTM                        0x00200000
#define HwDAIControlRCRM                        0x00400000
#define HwDAIControlLBM                         0x00800000

//
// Definitions of the bit fields in the HwDAIData0 register.
//
#define HwDAIData0DataMask                      0x0000ffff
#define HwDAIData0DataShift                     0

//
// Definitions of the bit fields in the HwDAIData1 register.
//
#define HwDAIData1DataMask                      0x0000ffff
#define HwDAIData1DataShift                     0

//
// Definitions of the FIFO command for the HwDAIData2 register.
//
#define HwDAIData2RightEnable                   0x000d8000
#define HwDAIData2RightDisable                  0x000d0000
#define HwDAIData2LeftEnable                    0x00118000
#define HwDAIData2LeftDisable                   0x00110000

//
// Definitions of the bit fields in the HwDAIStatus register.
//
#define HwDAIStatusRCTS                         0x00000001
#define HwDAIStatusRCRS                         0x00000002
#define HwDAIStatusLCTS                         0x00000004
#define HwDAIStatusLCRS                         0x00000008
#define HwDAIStatusRCTU                         0x00000010
#define HwDAIStatusRCRO                         0x00000020
#define HwDAIStatusLCTU                         0x00000040
#define HwDAIStatusLCRO                         0x00000080
#define HwDAIStatusRCNF                         0x00000100
#define HwDAIStatusRCNE                         0x00000200
#define HwDAIStatusLCNF                         0x00000400
#define HwDAIStatusLCNE                         0x00000800
#define HwDAIStatusFIFO                         0x00001000

//
// Definitions of the bit fields in the HwControl3 register.
//
#define HwControl3ADCExtended                   0x00000001
#define HwControl3ClkCtlMask                    0x00000006
#define HwControl3ClkCtl18MHz                   0x00000000
#define HwControl3ClkCtl36MHz                   0x00000002
#define HwControl3ClkCtl49MHz                   0x00000004
#define HwControl3ClkCtl74MHz                   0x00000006
#define HwControl3ClkCtlDiv8                    0x00000000
#define HwControl3ClkCtlDiv4                    0x00000002
#define HwControl3ClkCtlDiv3                    0x00000004
#define HwControl3ClkCtlDiv2                    0x00000006
#define HwControl3DAISelect                     0x00000008
#define HwControl3ADCClkEdge                    0x00000010
#define HwControl3VersionMask                   0x000000e0
#define HwControl3FastWakeup                    0x00000100
#define HwControl3Required                      0x00000200

//
// Definitions of the bit fields in the HwIntStatus3 and HwIntMask3 registers.
//
#define HwFiqDAI                                0x00000001

//
// Definitions of the bit fields in the HwLEDFlash register.
//
#define HwLEDFlashRateMask                      0x00000003
#define HwLEDFlashRate1Sec                      0x00000000
#define HwLEDFlashRate2Sec                      0x00000001
#define HwLEDFlashRate3Sec                      0x00000002
#define HwLEDFlashRate4Sec                      0x00000003
#define HwLEDFlashRatioMask                     0x0000003c
#define HwLEDFlashRatio1_15                     0x00000000
#define HwLEDFlashRatio2_14                     0x00000004
#define HwLEDFlashRatio3_13                     0x00000008
#define HwLEDFlashRatio4_12                     0x0000000c
#define HwLEDFlashRatio5_11                     0x00000010
#define HwLEDFlashRatio6_10                     0x00000014
#define HwLEDFlashRatio7_9                      0x00000018
#define HwLEDFlashRatio8_8                      0x0000001c
#define HwLEDFlashRatio9_7                      0x00000020
#define HwLEDFlashRatio10_6                     0x00000024
#define HwLEDFlashRatio11_5                     0x00000028
#define HwLEDFlashRatio12_4                     0x0000002c
#define HwLEDFlashRatio13_3                     0x00000030
#define HwLEDFlashRatio14_2                     0x00000034
#define HwLEDFlashRatio15_1                     0x00000038
#define HwLEDFlashRatio16_0                     0x0000003c
#define HwLEDFlashEnable                        0x00000040

//****************************************************************************
//
// The LCD controller registers.
//
//****************************************************************************

//
// Definitions of the bit fields in the LCD controller interface latch.
//
#define HwLCD_Data                              0x000000ff
#define HwLCD_nRES                              0x00000100
#define HwLCD_nCS                               0x00000200
#define HwLCD_RS                                0x00000400
#define HwLCD_nWR                               0x00000800
#define HwLCD_nRD                               0x00001000
#define HwLCD_BL_EN                             0x00002000
#define HwLCD_USB_nCS                           0x00008000

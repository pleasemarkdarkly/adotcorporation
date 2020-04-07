//****************************************************************************
//
// HWDEFS.H - Memory and register definitions for the hardware.
//
// Copyright (c) 1998-2000 Cirrus Logic, Inc.
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

//****************************************************************************
//
// The EP72xx/EP73xx internal registers.
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
#define HwDdrE                                  0x000000C0
#define HwControl                               0x00000100
#define HwStatus                                0x00000140
#define HwMemConfig1                            0x00000180
#define HwMemConfig2                            0x000001C0
#define HwEDORefresh                            0x00000200
#define HwIntStatus                             0x00000240
#define HwIntMask                               0x00000280
#define HwLcdControl                            0x000002C0
#define HwTimer1Data                            0x00000300
#define HwTimer2Data                            0x00000340
#define HwRtcData                               0x00000380
#define HwRtcMatch                              0x000003C0
#define HwPumpControl                           0x00000400
#define HwCodecData                             0x00000440
#define HwUartData                              0x00000480
#define HwUartControl                           0x000004C0
#define HwSpiData                               0x00000500
#define HwPaletteLSW                            0x00000540
#define HwPaletteMSW                            0x00000580
#define HwStartFlagClear                        0x000005C0
#define HwBatteryLowEOI                         0x00000600
#define HwMediaChangedEOI                       0x00000640
#define HwTickEOI                               0x00000680
#define HwTimer1EOI                             0x000006C0
#define HwTimer2EOI                             0x00000700
#define HwRtcMatchEOI                           0x00000740
#define HwUartEOI                               0x00000780
#define HwCodecEOI                              0x000007C0
#define HwHalt                                  0x00000800
#define HwStandby                               0x00000840
#define HwLcdFrameBuffer                        0x00001000
#define HwControl2                              0x00001100
#define HwStatus2                               0x00001140
#define HwIntStatus2                            0x00001240
#define HwIntMask2                              0x00001280
#define HwUart2Data                             0x00001480
#define HwUart2Control                          0x000014C0
#define HwKeyboardEOI                           0x00001700
#define HwDAIControl                            0x00002000
#define HwDAIData0                              0x00002040
#define HwDAIData1                              0x00002080
#define HwDAIData2                              0x000020C0
#define HwDAIStatus                             0x00002100
#define HwControl3                              0x00002200
#define HwIntStatus3                            0x00002240
#define HwIntMask3                              0x00002280
#define HwLEDFlash                              0x000022C0

//
// The offsets to the registers added in the EP73xx.
//
#define HwSDRAMConfig                           0x00002300
#define HwSDRAMRefresh                          0x00002340
#define HwDAI64Fs                               0x00002600

//
// Definitions of the bit fields in the HwControl register.
//
#define HwControlColumnDrive                    0x0000000F
#define HwControlColumnAllHigh                  0x00000000
#define HwControlColumnAllLow                   0x00000001
#define HwControlColumnAllTriState              0x00000002
#define HwControlColumn0High                    0x00000008
#define HwControlColumn1High                    0x00000009
#define HwControlColumn2High                    0x0000000A
#define HwControlColumn3High                    0x0000000B
#define HwControlColumn4High                    0x0000000C
#define HwControlColumn5High                    0x0000000D
#define HwControlColumn6High                    0x0000000E
#define HwControlColumn7High                    0x0000000F
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
#define HwStatusLcdType                         0x000000F0
#define HwStatusCts                             0x00000100
#define HwStatusDsr                             0x00000200
#define HwStatusDcd                             0x00000400
#define HwStatusUartTxBusy                      0x00000800
#define HwStatusBatteryChangedFlag              0x00001000
#define HwStatusResetFlag                       0x00002000
#define HwStatusPowerFailFlag                   0x00004000
#define HwStatusColdStartFlag                   0x00008000
#define HwStatusRtcLSB                          0x003F0000
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
#define HwStatusVersionID                       0xC0000000

//
// Definitions of the bit fields in the HwMemConfig1 and HwMemConfig2
// registers.
//
#define HwMemConfigBusWidth                     0x00000003
#define HwMemConfigBusWidth32                   0x00000000
#define HwMemConfigBusWidth16                   0x00000001
#define HwMemConfigBusWidth8                    0x00000002
#define HwMemConfigBusWidthPcmcia               0x00000003
#define HwMemConfigRaWaitState                  0x0000000C
#define HwMemConfigRaWaitState4                 0x00000000
#define HwMemConfigRaWaitState3                 0x00000004
#define HwMemConfigRaWaitState2                 0x00000008
#define HwMemConfigRaWaitState1                 0x0000000C
#define HwMemConfigSaWaitState                  0x00000030
#define HwMemConfigSaWaitState3                 0x00000000
#define HwMemConfigSaWaitState2                 0x00000010
#define HwMemConfigSaWaitState1                 0x00000020
#define HwMemConfigSaWaitState0                 0x00000030
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
#define HwLcdControlBufferSize                  0x00001FFF
#define HwLcdControlLineLength                  0x0007E000
#define HwLcdControlPixelPrescale               0x01F80000
#define HwLcdControlAcPrescale                  0x3E000000
#define HwLcdControlGreyEnable                  0x40000000
#define HwLcdControlGrey4Or2                    0x80000000
#define HwLcdControlBufferSizeShift             0
#define HwLcdControlLineLengthShift             13
#define HwLcdControlPixelPrescaleShift          19
#define HwLcdControlAcPrescaleShift             25

//
// Definitions of the bit fields in the HwPumpControl register.
//
#define HwPumpControlVhBat                      0x0000000F
#define HwPumpControlVhDc                       0x000000F0
#define HwPumpControlLcd                        0x00000F00

//
// Definitions of the bit fields in the HwUartData register.
//
#define HwUartDataFrameErr                      0x00000100
#define HwUartDataParityErr                     0x00000200
#define HwUartDataOverrunErr                    0x00000400

//
// Definitions of the bit fields in the HwUartControl register.
//
#define HwUartControlRate                       0x00000FFF
#define HwUartControlRate115200                 0x00000001
#define HwUartControlRate76800                  0x00000002
#define HwUartControlRate57600                  0x00000003
#define HwUartControlRate38400                  0x00000005
#define HwUartControlRate28800                  0x00000007
#define HwUartControlRate19200                  0x0000000B
#define HwUartControlRate14400                  0x0000000F
#define HwUartControlRate9600                   0x00000017
#define HwUartControlRate4800                   0x0000002F
#define HwUartControlRate2400                   0x0000005F
#define HwUartControlRate1200                   0x000000BF
#define HwUartControlRate600                    0x0000017F
#define HwUartControlRate300                    0x000002FF
#define HwUartControlRate150                    0x000005FF
#define HwUartControlRate110                    0x0000082E
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
#define HwSpiDataConfig                         0x000000FF
#define HwSpiDataFrameLength                    0x00001F00
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
#define HwDAIData0DataMask                      0x0000FFFF
#define HwDAIData0DataShift                     0

//
// Definitions of the bit fields in the HwDAIData1 register.
//
#define HwDAIData1DataMask                      0x0000FFFF
#define HwDAIData1DataShift                     0

//
// Definitions of the FIFO command for the HwDAIData2 register.
//
#define HwDAIData2RightEnable                   0x000D8000
#define HwDAIData2RightDisable                  0x000D0000
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
#define HwControl3ClkCtl37MHz                   0x00000002
#define HwControl3ClkCtl49MHz                   0x00000004
#define HwControl3ClkCtl74MHz                   0x00000006
#define HwControl3DAISelect                     0x00000008
#define HwControl3ADCClkEdge                    0x00000010
#define HwControl3VersionMask                   0x000000E0
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
#define HwLEDFlashRatioMask                     0x0000003C
#define HwLEDFlashRatio1_15                     0x00000000
#define HwLEDFlashRatio2_14                     0x00000004
#define HwLEDFlashRatio3_13                     0x00000008
#define HwLEDFlashRatio4_12                     0x0000000C
#define HwLEDFlashRatio5_11                     0x00000010
#define HwLEDFlashRatio6_10                     0x00000014
#define HwLEDFlashRatio7_9                      0x00000018
#define HwLEDFlashRatio8_8                      0x0000001C
#define HwLEDFlashRatio9_7                      0x00000020
#define HwLEDFlashRatio10_6                     0x00000024
#define HwLEDFlashRatio11_5                     0x00000028
#define HwLEDFlashRatio12_4                     0x0000002C
#define HwLEDFlashRatio13_3                     0x00000030
#define HwLEDFlashRatio14_2                     0x00000034
#define HwLEDFlashRatio15_1                     0x00000038
#define HwLEDFlashRatio16_0                     0x0000003C
#define HwLEDFlashEnable                        0x00000040

//
// Definitions of the bit fields in the HwSDRAMConfig register.
//
#define HwSDRAMConfigEn                         0x00000400
#define HwSDRAMConfigClockEn                    0x00000200
#define HwSDRAMConfigWidthMask                  0x00000180
#define HwSDRAMConfigWidth4                     0x00000000
#define HwSDRAMConfigWidth8                     0x00000080
#define HwSDRAMConfigWidth16                    0x00000100
#define HwSDRAMConfigWidth32                    0x00000180
#define HwSDRAMConfigCapacityMask               0x00000060
#define HwSDRAMConfigCapacity16                 0x00000000
#define HwSDRAMConfigCapacity64                 0x00000020
#define HwSDRAMConfigCapacity128                0x00000040
#define HwSDRAMConfigCapacity256                0x00000060
#define HwSDRAMConfigCASLatencyMask             0x00000003
#define HwSDRAMConfigCASLatency2                0x00000002
#define HwSDRAMConfigCASLatency3                0x00000003

//
// Definitions of the bit fields in the HwSDRAMRefresh register.
//
#define HwSDRAMRefreshMask                      0x0000ffff
#define HwSDRAMRefreshShift                     0

//
// Definitions of the bit fields in the HwDAI64Fs register.
//
#define HwDAI64FsDivisorMask                    0x00007f00
#define HwDAI64FsDivisor8kHz                    0x00004800
#define HwDAI64FsDivisor11kHz                   0x00001000
#define HwDAI64FsDivisor12kHz                   0x00003000
#define HwDAI64FsDivisor16kHz                   0x00002400
#define HwDAI64FsDivisor22kHz                   0x00000800
#define HwDAI64FsDivisor24kHz                   0x00001800
#define HwDAI64FsDivisor32kHz                   0x00001200
#define HwDAI64FsDivisor44kHz                   0x00000400
#define HwDAI64FsDivisor48kHz                   0x00000c00
#define HwDAI64FsLoopback                       0x00000020
#define HwDAI64FsMclkEn                         0x00000008
#define HwDAI64FsClkSrcMask                     0x00000004
#define HwDAI64FsClk74MHz                       0x00000000
#define HwDAI64FsClk11MHz                       0x00000004
#define HwDAI64FsClockEn                        0x00000002
#define HwDAI64FsEn                             0x00000001
#define HwDAI64FsDivisorShift                   8

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

//****************************************************************************
//
// The indices of the registers in the HD66410.
//
//****************************************************************************
#define LCD_REG_CONTROL1                        0
#define LCD_REG_CONTROL2                        1
#define LCD_REG_X                               2
#define LCD_REG_Y                               3
#define LCD_REG_DATA                            4
#define LCD_REG_START                           5
#define LCD_REG_BLINK1                          6
#define LCD_REG_BLINK2                          7
#define LCD_REG_BLINK_START                     8
#define LCD_REG_BLINK_END                       9
#define LCD_REG_ANNUNCIATOR1                    16
#define LCD_REG_ANNUNCIATOR2                    17
#define LCD_REG_ANNUNCIATOR3                    18
#define LCD_REG_ANNUNCIATOR4                    19
#define LCD_REG_ANNUNCIATOR5                    20
#define LCD_REG_ANNUNCIATOR6                    21
#define LCD_REG_ANNUNCIATOR7                    22
#define LCD_REG_ANNUNCIATOR8                    23
#define LCD_REG_ANNUNCIATOR9                    24
#define LCD_REG_ANNUNCIATOR_BLINK1              25
#define LCD_REG_ANNUNCIATOR_BLINK2              26
#define LCD_REG_ANNUNCIATOR_BLINK3              27

//****************************************************************************
//
// The various bits in the HD66410 control register one.
//
//****************************************************************************
#define LCD_CONTROL1_ADC                        0x01
#define LCD_CONTROL1_CNF                        0x02
#define LCD_CONTROL1_IDTY                       0x04
#define LCD_CONTROL1_OSC                        0x08
#define LCD_CONTROL1_PWR                        0x10
#define LCD_CONTROL1_STBY                       0x20
#define LCD_CONTROL1_DISP                       0x40

//****************************************************************************
//
// The various bits in the HD66410 control register two.
//
//****************************************************************************
#define LCD_CONTROL2_BLK                        0x01
#define LCD_CONTROL2_INC                        0x02
#define LCD_CONTROL2_DDTY                       0x04
#define LCD_CONTROL2_RMW                        0x08

//****************************************************************************
//
// The various icons across the top of the LCD panel.
//
//****************************************************************************
#define LCD_ANNUNC3_ARROWS                      0x80
#define LCD_ANNUNC3_LEFT_SEMICIRCLE             0x40
#define LCD_ANNUNC3_PROGRAM                     0x20
#define LCD_ANNUNC3_RANDOM                      0x10
#define LCD_ANNUNC3_SINGLE                      0x08
#define LCD_ANNUNC3_RIGHT_ARROWCIRCLE           0x04
#define LCD_ANNUNC3_LEFT_ARROWCIRCLE            0x02
#define LCD_ANNUNC3_PLAY                        0x01
#define LCD_ANNUNC4_TAPE                        0x02
#define LCD_ANNUNC5_TAPEBOX                     0x80
#define LCD_ANNUNC5_BATTERY_FILL3               0x40
#define LCD_ANNUNC5_BATTERY                     0x20
#define LCD_ANNUNC5_BATTERY_FILL2               0x10
#define LCD_ANNUNC5_BATTERY_FILL1               0x08
#define LCD_ANNUNC5_LOCK                        0x04
#define LCD_ANNUNC5_BOOK                        0x02
#define LCD_ANNUNC6_RIGHT_SEMICIRCLE            0x80

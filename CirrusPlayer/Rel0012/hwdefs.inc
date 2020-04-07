;*****************************************************************************
;
; HWDEFS.INC - Memory and register definitions for the hardware.
;
; Copyright (c) 1999-2000 Cirrus Logic, Inc.
;
;*****************************************************************************

;*****************************************************************************
;
; The base address for the various memories in the system.
;
;*****************************************************************************
HwProgramAddress                EQU     0x00000000
HwSRAMAddress                   EQU     0x00100000
HwBaseAddress                   EQU     0x00200000
HwNANDAddress                   EQU     0x00300000
HwUSBAddress                    EQU     0x00400000
HwLCDAddress                    EQU     0x00500000

;*****************************************************************************
;
; The EP72xx/EP73xx internal registers.
;
;*****************************************************************************

;
; The offset to each individual register in the EP72xx.
;
HwPortABCD                      EQU     0x00000000
HwPortA                         EQU     0x00000000
HwPortB                         EQU     0x00000001
HwPortD                         EQU     0x00000003
HwDdrABCD                       EQU     0x00000040
HwDdrA                          EQU     0x00000040
HwDdrB                          EQU     0x00000041
HwDdrD                          EQU     0x00000043
HwPortE                         EQU     0x00000080
HwDdrE                          EQU     0x000000C0
HwControl                       EQU     0x00000100
HwStatus                        EQU     0x00000140
HwMemConfig1                    EQU     0x00000180
HwMemConfig2                    EQU     0x000001C0
HwEDORefresh                    EQU     0x00000200
HwIntStatus                     EQU     0x00000240
HwIntMask                       EQU     0x00000280
HwLcdControl                    EQU     0x000002C0
HwTimer1Data                    EQU     0x00000300
HwTimer2Data                    EQU     0x00000340
HwRtcData                       EQU     0x00000380
HwRtcMatch                      EQU     0x000003C0
HwPumpControl                   EQU     0x00000400
HwCodecData                     EQU     0x00000440
HwUartData                      EQU     0x00000480
HwUartControl                   EQU     0x000004C0
HwSpiData                       EQU     0x00000500
HwPaletteLSW                    EQU     0x00000540
HwPaletteMSW                    EQU     0x00000580
HwStartFlagClear                EQU     0x000005C0
HwBatteryLowEOI                 EQU     0x00000600
HwMediaChangedEOI               EQU     0x00000640
HwTickEOI                       EQU     0x00000680
HwTimer1EOI                     EQU     0x000006C0
HwTimer2EOI                     EQU     0x00000700
HwRtcMatchEOI                   EQU     0x00000740
HwUartEOI                       EQU     0x00000780
HwCodecEOI                      EQU     0x000007C0
HwHalt                          EQU     0x00000800
HwStandby                       EQU     0x00000840
HwLcdFrameBuffer                EQU     0x00001000
HwControl2                      EQU     0x00001100
HwStatus2                       EQU     0x00001140
HwIntStatus2                    EQU     0x00001240
HwIntMask2                      EQU     0x00001280
HwUart2Data                     EQU     0x00001480
HwUart2Control                  EQU     0x000014C0
HwKeyboardEOI                   EQU     0x00001700
HwDAIControl                    EQU     0x00002000
HwDAIData0                      EQU     0x00002040
HwDAIData1                      EQU     0x00002080
HwDAIData2                      EQU     0x000020C0
HwDAIStatus                     EQU     0x00002100
HwControl3                      EQU     0x00002200
HwIntStatus3                    EQU     0x00002240
HwIntMask3                      EQU     0x00002280
HwLEDFlash                      EQU     0x000022C0

;
; The offsets to the registers added in the EP73xx.
;
HwSDRAMConfig                   EQU     0x00002300
HwSDRAMRefresh                  EQU     0x00002340
HwDAI64Fs                       EQU     0x00002600

;
; Definitions of the bit fields in the HwControl register.
;
HwControlColumnDrive            EQU     0x0000000F
HwControlColumnAllHigh          EQU     0x00000000
HwControlColumnAllLow           EQU     0x00000001
HwControlColumnAllTriState      EQU     0x00000002
HwControlColumn0High            EQU     0x00000008
HwControlColumn1High            EQU     0x00000009
HwControlColumn2High            EQU     0x0000000A
HwControlColumn3High            EQU     0x0000000B
HwControlColumn4High            EQU     0x0000000C
HwControlColumn5High            EQU     0x0000000D
HwControlColumn6High            EQU     0x0000000E
HwControlColumn7High            EQU     0x0000000F
HwControlTimer1PreOrFree        EQU     0x00000010
HwControlTimer1K512OrK2         EQU     0x00000020
HwControlTimer2PreOrFree        EQU     0x00000040
HwControlTimer2K512OrK2         EQU     0x00000080
HwControlUartEnable             EQU     0x00000100
HwControlBuzzerToggle           EQU     0x00000200
HwControlBuzzerTimer1OrToggle   EQU     0x00000400
HwControlDebugEnable            EQU     0x00000800
HwControlLcdEnable              EQU     0x00001000
HwControlCodecTxEnable          EQU     0x00002000
HwControlCodecRxEnable          EQU     0x00004000
HwControlUartSirEnable          EQU     0x00008000
HwControlSpiClockSelect         EQU     0x00030000
HwControlSpiClock4KHz           EQU     0x00000000
HwControlSpiClock16KHz          EQU     0x00010000
HwControlSpiClock64KHz          EQU     0x00020000
HwControlSpiClock128KHz         EQU     0x00030000
HwControlExpClockEnable         EQU     0x00040000
HwControlWakeupDisable          EQU     0x00080000
HwControlIrDAWidthEnable        EQU     0x00100000

;
; Definitions of the bit fields in the HwStatus register.
;
HwStatusMediaChangedState       EQU     0x00000001
HwStatusDcPresent               EQU     0x00000002
HwStatusWakeUpState             EQU     0x00000004
HwStatusWakeUpFlag              EQU     0x00000008
HwStatusLcdType                 EQU     0x000000F0
HwStatusCts                     EQU     0x00000100
HwStatusDsr                     EQU     0x00000200
HwStatusDcd                     EQU     0x00000400
HwStatusUartTxBusy              EQU     0x00000800
HwStatusBatteryChangedFlag      EQU     0x00001000
HwStatusResetFlag               EQU     0x00002000
HwStatusPowerFailFlag           EQU     0x00004000
HwStatusColdStartFlag           EQU     0x00008000
HwStatusRtcLSB                  EQU     0x003F0000
HwStatusUartRxFifoEmpty         EQU     0x00400000
HwStatusUartTxFifoFull          EQU     0x00800000
HwStatusCodecRxFifoEmpty        EQU     0x01000000
HwStatusCodecTxFifoFull         EQU     0x02000000
HwStatusSpiBusy                 EQU     0x04000000
HwStatusBootWidthMask           EQU     0x18000000
HwStatusBootWidth32             EQU     0x00000000
HwStatusBootWidth8              EQU     0x08000000
HwStatusBootWidth16             EQU     0x10000000
HwStatusID                      EQU     0x20000000
HwStatusVersionID               EQU     0xC0000000

;
; Definitions of the bit fields in the HwMemConfig1 and HwMemConfig2 registers.
;
HwMemConfigBusWidth             EQU     0x00000003
HwMemConfigBusWidth32           EQU     0x00000000
HwMemConfigBusWidth16           EQU     0x00000001
HwMemConfigBusWidth8            EQU     0x00000002
HwMemConfigBusWidthPcmcia       EQU     0x00000003
HwMemConfigRaWaitState          EQU     0x0000000C
HwMemConfigRaWaitState4         EQU     0x00000000
HwMemConfigRaWaitState3         EQU     0x00000004
HwMemConfigRaWaitState2         EQU     0x00000008
HwMemConfigRaWaitState1         EQU     0x0000000C
HwMemConfigSaWaitState          EQU     0x00000030
HwMemConfigSaWaitState3         EQU     0x00000000
HwMemConfigSaWaitState2         EQU     0x00000010
HwMemConfigSaWaitState1         EQU     0x00000020
HwMemConfigSaWaitState0         EQU     0x00000030
HwMemConfigSeqAccessEnable      EQU     0x00000040
HwMemConfigExpClkEnable         EQU     0x00000080

;
; Definitions of the bit fields in the HwEDORefresh register.
;
HwEDORefreshCountMask           EQU     0x0000007f
HwEDORefreshEnable              EQU     0x00000080
HwEDORefreshCountShift          EQU     0

;
; Definitions of the bit fields in the HwIntStatus and HwIntMask registers.
;
HwFiqExt                        EQU     0x00000001
HwFiqBatteryLow                 EQU     0x00000002
HwFiqWatchDog                   EQU     0x00000004
HwFiqMediaChange                EQU     0x00000008
HwIrqCodec                      EQU     0x00000010
HwIrqExt1                       EQU     0x00000020
HwIrqExt2                       EQU     0x00000040
HwIrqExt3                       EQU     0x00000080
HwIrqTimer1                     EQU     0x00000100
HwIrqTimer2                     EQU     0x00000200
HwIrqRtcMatch                   EQU     0x00000400
HwIrqTick                       EQU     0x00000800
HwIrqUartTx                     EQU     0x00001000
HwIrqUartRx                     EQU     0x00002000
HwIrqUartModem                  EQU     0x00004000
HwIrqSpi                        EQU     0x00008000

;
; Definitions of the bit fields in the HwLcdControl register.
;
HwLcdControlBufferSize          EQU     0x00001FFF
HwLcdControlLineLength          EQU     0x0007E000
HwLcdControlPixelPrescale       EQU     0x01F80000
HwLcdControlAcPrescale          EQU     0x3E000000
HwLcdControlGreyEnable          EQU     0x40000000
HwLcdControlGrey4Or2            EQU     0x80000000
HwLcdControlBufferSizeShift     EQU     0
HwLcdControlLineLengthShift     EQU     13
HwLcdControlPixelPrescaleShift  EQU     19
HwLcdControlAcPrescaleShift     EQU     25

;
; Definitions of the bit fields in the HwPumpControl register.
;
HwPumpControlVhBat              EQU     0x0000000F
HwPumpControlVhDc               EQU     0x000000F0
HwPumpControlLcd                EQU     0x00000F00

;
; Definitions of the bit fields in the HwUartData register.
;
HwUartDataFrameErr              EQU     0x00000100
HwUartDataParityErr             EQU     0x00000200
HwUartDataOverrunErr            EQU     0x00000400

;
; Definitions of the bit fields in the HwUartControl register.
;
HwUartControlRate               EQU     0x00000FFF
HwUartControlRate115200         EQU     0x00000001
HwUartControlRate76800          EQU     0x00000002
HwUartControlRate57600          EQU     0x00000003
HwUartControlRate38400          EQU     0x00000005
HwUartControlRate28800          EQU     0x00000007
HwUartControlRate19200          EQU     0x0000000B
HwUartControlRate14400          EQU     0x0000000F
HwUartControlRate9600           EQU     0x00000017
HwUartControlRate4800           EQU     0x0000002F
HwUartControlRate2400           EQU     0x0000005F
HwUartControlRate1200           EQU     0x000000BF
HwUartControlRate600            EQU     0x0000017F
HwUartControlRate300            EQU     0x000002FF
HwUartControlRate150            EQU     0x000005FF
HwUartControlRate110            EQU     0x0000082E
HwUartControlBreak              EQU     0x00001000
HwUartControlParityEnable       EQU     0x00002000
HwUartControlParityEven         EQU     0x00004000
HwUartControlTwoStopBits        EQU     0x00008000
HwUartControlFifoEnable         EQU     0x00010000
HwUartControlDataLength         EQU     0x00060000
HwUartControlDataLength5        EQU     0x00000000
HwUartControlDataLength6        EQU     0x00020000
HwUartControlDataLength7        EQU     0x00040000
HwUartControlDataLength8        EQU     0x00060000

;
; Definitions of the bit fields in the HwSpiData register.
;
HwSpiDataConfig                 EQU     0x000000FF
HwSpiDataFrameLength            EQU     0x00001F00
HwSpiDataSampleClockFreerun     EQU     0x00002000
HwSpiDataTxFrame                EQU     0x00004000

;
; Definitions of the bit fields in the HwControl2 register.
;
HwControl2CodecEnable           EQU     0x00000001
HwControl2Kbd6                  EQU     0x00000002
HwControl2DRAM16                EQU     0x00000004
HwControl2KeyboardWakeupEnable  EQU     0x00000008
HwControl2PCMCIA1Enable         EQU     0x00000020
HwControl2PCMCIA2Enable         EQU     0x00000040
HwControl2Uart2Enable           EQU     0x00000100
HwControl2OSTimerEnable         EQU     0x00001000
HwControl2ClockSelect           EQU     0x00002000
HwControl2BuzzerSelect          EQU     0x00004000

;
; Definitions of the bit fields in the HwStatus2 register.
;
HwStatus2ClockMode              EQU     0x00000040
HwStatus2Uart2TxBusy            EQU     0x00000800
HwStatus2Uart2RxFifoEmpty       EQU     0x00400000
HwStatus2Uart2TxFifoFull        EQU     0x00800000

;
; Definitions of the bit fields in the HwIntStatus2 and HwIntMask2 registers.
;
HwIrqKeyboard                   EQU     0x00000001
HwIrqUart2Tx                    EQU     0x00001000
HwIrqUart2Rx                    EQU     0x00002000

;
; Definitions of the bit fields in the HwDAIControl register.
;
HwDAIControlDAIEN               EQU     0x00010000
HwDAIControlECS                 EQU     0x00020000
HwDAIControlLCTM                EQU     0x00080000
HwDAIControlLCRM                EQU     0x00100000
HwDAIControlRCTM                EQU     0x00200000
HwDAIControlRCRM                EQU     0x00400000
HwDAIControlLBM                 EQU     0x00800000

;
; Definitions of the bit fields in the HwDAIData0 register.
;
HwDAIData0DataMask              EQU     0x0000FFFF
HwDAIData0DataShift             EQU     0

;
; Definitions of the bit fields in the HwDAIData1 register.
;
HwDAIData1DataMask              EQU     0x0000FFFF
HwDAIData1DataShift             EQU     0

;
; Definitions of the FIFO command for the HwDAIData2 register.
;
HwDAIData2RightEnable           EQU     0x000D8000
HwDAIData2RightDisable          EQU     0x000D0000
HwDAIData2LeftEnable            EQU     0x00118000
HwDAIData2LeftDisable           EQU     0x00110000

;
; Definitions of the bit fields in the HwDAIStatus register.
;
HwDAIStatusRCTS                 EQU     0x00000001
HwDAIStatusRCRS                 EQU     0x00000002
HwDAIStatusLCTS                 EQU     0x00000004
HwDAIStatusLCRS                 EQU     0x00000008
HwDAIStatusRCTU                 EQU     0x00000010
HwDAIStatusRCRO                 EQU     0x00000020
HwDAIStatusLCTU                 EQU     0x00000040
HwDAIStatusLCRO                 EQU     0x00000080
HwDAIStatusRCNF                 EQU     0x00000100
HwDAIStatusRCNE                 EQU     0x00000200
HwDAIStatusLCNF                 EQU     0x00000400
HwDAIStatusLCNE                 EQU     0x00000800
HwDAIStatusFIFO                 EQU     0x00001000

;
; Definitions of the bit fields in the HwControl3 register.
;
HwControl3ADCExtended           EQU     0x00000001
HwControl3ClkCtlMask            EQU     0x00000006
HwControl3ClkCtl18MHz           EQU     0x00000000
HwControl3ClkCtl37MHz           EQU     0x00000002
HwControl3ClkCtl49MHz           EQU     0x00000004
HwControl3ClkCtl74MHz           EQU     0x00000006
HwControl3DAISelect             EQU     0x00000008
HwControl3ADCClkEdge            EQU     0x00000010
HwControl3VersionMask           EQU     0x000000E0
HwControl3FastWakeup            EQU     0x00000100
HwControl3Required              EQU     0x00000200

;
; Definitions of the bit fields in the HwIntStatus3 and HwIntMask3 registers.
;
HwFiqDAI                        EQU     0x00000001

;
; Definitions of the bit fields in the HwLEDFlash register.
;
HwLEDFlashRateMask              EQU     0x00000003
HwLEDFlashRate1Sec              EQU     0x00000000
HwLEDFlashRate2Sec              EQU     0x00000001
HwLEDFlashRate3Sec              EQU     0x00000002
HwLEDFlashRate4Sec              EQU     0x00000003
HwLEDFlashRatioMask             EQU     0x0000003C
HwLEDFlashRatio1_15             EQU     0x00000000
HwLEDFlashRatio2_14             EQU     0x00000004
HwLEDFlashRatio3_13             EQU     0x00000008
HwLEDFlashRatio4_12             EQU     0x0000000C
HwLEDFlashRatio5_11             EQU     0x00000010
HwLEDFlashRatio6_10             EQU     0x00000014
HwLEDFlashRatio7_9              EQU     0x00000018
HwLEDFlashRatio8_8              EQU     0x0000001C
HwLEDFlashRatio9_7              EQU     0x00000020
HwLEDFlashRatio10_6             EQU     0x00000024
HwLEDFlashRatio11_5             EQU     0x00000028
HwLEDFlashRatio12_4             EQU     0x0000002C
HwLEDFlashRatio13_3             EQU     0x00000030
HwLEDFlashRatio14_2             EQU     0x00000034
HwLEDFlashRatio15_1             EQU     0x00000038
HwLEDFlashRatio16_0             EQU     0x0000003C
HwLEDFlashEnable                EQU     0x00000040

;
; Definitions of the bit fields in the HwSDRAMConfig register.
;
HwSDRAMConfigEn                 EQU     0x00000400
HwSDRAMConfigClockEn            EQU     0x00000200
HwSDRAMConfigWidthMask          EQU     0x00000180
HwSDRAMConfigWidth4             EQU     0x00000000
HwSDRAMConfigWidth8             EQU     0x00000080
HwSDRAMConfigWidth16            EQU     0x00000100
HwSDRAMConfigWidth32            EQU     0x00000180
HwSDRAMConfigCapacityMask       EQU     0x00000060
HwSDRAMConfigCapacity16         EQU     0x00000000
HwSDRAMConfigCapacity64         EQU     0x00000020
HwSDRAMConfigCapacity128        EQU     0x00000040
HwSDRAMConfigCapacity256        EQU     0x00000060
HwSDRAMConfigCASLatencyMask     EQU     0x00000003
HwSDRAMConfigCASLatency2        EQU     0x00000002
HwSDRAMConfigCASLatency3        EQU     0x00000003

;
; Definitions of the bit fields in the HwSDRAMRefresh register.
;
HwSDRAMRefreshMask              EQU     0x0000ffff
HwSDRAMRefreshShift             EQU     0

;
; Definitions of the bit fields in the HwDAI64Fs register.
;
HwDAI64FsDivisorMask            EQU     0x00007f00
HwDAI64FsDivisor8kHz            EQU     0x00004800
HwDAI64FsDivisor11kHz           EQU     0x00001000
HwDAI64FsDivisor12kHz           EQU     0x00003000
HwDAI64FsDivisor16kHz           EQU     0x00002400
HwDAI64FsDivisor22kHz           EQU     0x00000800
HwDAI64FsDivisor24kHz           EQU     0x00001800
HwDAI64FsDivisor32kHz           EQU     0x00001200
HwDAI64FsDivisor44kHz           EQU     0x00000400
HwDAI64FsDivisor48kHz           EQU     0x00000c00
HwDAI64FsLoopback               EQU     0x00000020
HwDAI64FsMclkEn                 EQU     0x00000008
HwDAI64FsClkSrcMask             EQU     0x00000004
HwDAI64FsClk74MHz               EQU     0x00000000
HwDAI64FsClk11MHz               EQU     0x00000004
HwDAI64FsClockEn                EQU     0x00000002
HwDAI64FsEn                     EQU     0x00000001
HwDAI64FsDivisorShift           EQU     8

;*****************************************************************************
;
; The LCD controller registers.
;
;*****************************************************************************

;
; Definitions of the bit fields in the LCD controller interface latch.
;
HwLCD_Data                      EQU     0x000000ff
HwLCD_nRES                      EQU     0x00000100
HwLCD_nCS                       EQU     0x00000200
HwLCD_RS                        EQU     0x00000400
HwLCD_nWR                       EQU     0x00000800
HwLCD_nRD                       EQU     0x00001000
HwLCD_BL_EN                     EQU     0x00002000
HwLCD_USB_nCS                   EQU     0x00008000

;
; The indices of the registers in the HD66410.
;
LCD_REG_CONTROL1                EQU     0
LCD_REG_CONTROL2                EQU     1
LCD_REG_X                       EQU     2
LCD_REG_Y                       EQU     3
LCD_REG_DATA                    EQU     4
LCD_REG_START                   EQU     5
LCD_REG_BLINK1                  EQU     6
LCD_REG_BLINK2                  EQU     7
LCD_REG_BLINK_START             EQU     8
LCD_REG_BLINK_END               EQU     9
LCD_REG_ANNUNCIATOR1            EQU     16
LCD_REG_ANNUNCIATOR2            EQU     17
LCD_REG_ANNUNCIATOR3            EQU     18
LCD_REG_ANNUNCIATOR4            EQU     19
LCD_REG_ANNUNCIATOR5            EQU     20
LCD_REG_ANNUNCIATOR6            EQU     21
LCD_REG_ANNUNCIATOR7            EQU     22
LCD_REG_ANNUNCIATOR8            EQU     23
LCD_REG_ANNUNCIATOR9            EQU     24
LCD_REG_ANNUNCIATOR_BLINK1      EQU     25
LCD_REG_ANNUNCIATOR_BLINK2      EQU     26
LCD_REG_ANNUNCIATOR_BLINK3      EQU     27

;
; The various bits in the HD66410 control register one.
;
LCD_CONTROL1_ADC                EQU     0x01
LCD_CONTROL1_CNF                EQU     0x02
LCD_CONTROL1_IDTY               EQU     0x04
LCD_CONTROL1_OSC                EQU     0x08
LCD_CONTROL1_PWR                EQU     0x10
LCD_CONTROL1_STBY               EQU     0x20
LCD_CONTROL1_DISP               EQU     0x40

;
; The various bits in the HD66410 control register two.
;
LCD_CONTROL2_BLK                EQU     0x01
LCD_CONTROL2_INC                EQU     0x02
LCD_CONTROL2_DDTY               EQU     0x04
LCD_CONTROL2_RMW                EQU     0x08

;
; The various icons across the top of the LCD panel.
;
LCD_ANNUNC3_ARROWS              EQU     0x80
LCD_ANNUNC3_LEFT_SEMICIRCLE     EQU     0x40
LCD_ANNUNC3_PROGRAM             EQU     0x20
LCD_ANNUNC3_RANDOM              EQU     0x10
LCD_ANNUNC3_SINGLE              EQU     0x08
LCD_ANNUNC3_RIGHT_ARROWCIRCLE   EQU     0x04
LCD_ANNUNC3_LEFT_ARROWCIRCLE    EQU     0x02
LCD_ANNUNC3_PLAY                EQU     0x01
LCD_ANNUNC4_TAPE                EQU     0x02
LCD_ANNUNC5_TAPEBOX             EQU     0x80
LCD_ANNUNC5_BATTERY_FILL3       EQU     0x40
LCD_ANNUNC5_BATTERY             EQU     0x20
LCD_ANNUNC5_BATTERY_FILL2       EQU     0x10
LCD_ANNUNC5_BATTERY_FILL1       EQU     0x08
LCD_ANNUNC5_LOCK                EQU     0x04
LCD_ANNUNC5_BOOK                EQU     0x02
LCD_ANNUNC6_RIGHT_SEMICIRCLE    EQU     0x80

    END

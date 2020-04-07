//****************************************************************************
//
// CONFIG.H - Configuration values for the Internet audio player.  The
//            definitions in this file must match those in config.inc.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
//
//****************************************************************************

//****************************************************************************
//
// Defines for the various codecs that we support.  By commenting out a
// particular codec, support for that codec will not be included in the player
// (saving ROM).
//
//****************************************************************************
//#define SUPPORT_AAC
//#define SUPPORT_ACELPNET
//#define SUPPORT_G721
#define SUPPORT_MP3
#define SUPPORT_MSADPCM
//#define SUPPORT_WMA

//****************************************************************************
//
// Defines for the various ACELP.net bitrates that we support.  By commenting
// out a particular rate, support for that ACELP.net rate will not be included
// in the player (saving ROM).
//
//****************************************************************************
#ifdef SUPPORT_ACELPNET
#define SUPPORT_ACELPNET_5K0
#define SUPPORT_ACELPNET_6K5
#define SUPPORT_ACELPNET_8K5
#define SUPPORT_ACELPNET_16K0
#endif

//****************************************************************************
//
// Defines for the various platforms to which the player has been ported.
// Only one of these can be defined at a time.
//
//****************************************************************************
#define EP7209_Eval_Board
//#define EP7209_Reference_Board_Rev_A
//#define EP7209_Reference_Board_Rev_B
//#define EP73XX_Eval_Board

//****************************************************************************
//
// Definitions for the EP7209 evaluation board.
//
//****************************************************************************

#ifdef EP7209_Eval_Board

//
// Indicate that we use the serial port for our "user interface".
//
#define UI_IS_SERIAL_PORT

//
// Definitions of the bit fields in the HwPortABCD register.
//
#define HwPortABCD_SmartMedia_Power             0x00000000
#define HwPortABCD_Button_Mask                  0x000000ff
#define HwPortABCD_NAND_CLE                     0x00001000
#define HwPortABCD_NAND_ALE                     0x00002000
#define HwPortABCD_NAND1_CS                     0x00004000
#define HwPortABCD_SmartMedia_CS                0x00008000
//#define HwPortABCD_I2C_SDA                      0x10000000
//#define HwPortABCD_I2C_SCL                      0x20000000

//
// Definitions of the bit fields in the HwPortE register.
//
#define HwPortE_Codec_Power                     0x00000001

//
// The I2C address of the CS43L43.
//
//#define I2CADDR_CS43L43                         0x00000020

//
// Definitions of the external interrupt sources.
//
#define HwIrqUSB                                HwIrqExt1

//
// The hardware ID for this board.
//
#define HardwareID                              0x01010100

#endif

//****************************************************************************
//
// Definitions for the EP7209 reference design board, Rev. A.
//
//****************************************************************************

#ifdef EP7209_Reference_Board_Rev_A

//
// Indicate that we use the serial port for our "user interface".
//
#define UI_IS_SERIAL_PORT

//
// Indicate that we need the voice record capability.
//
#define REQUIRE_RECORD

//
// Definitions of the bit fields in the HwPortABCD register.
//
#define HwPortABCD_Button_Mask                  0x000000ff
#define HwPortABCD_I2C_SCL                      0x00000100
#define HwPortABCD_I2C_SDA                      0x00000200
#define HwPortABCD_Codec_Power                  0x00000400
#define HwPortABCD_NAND_ALE                     0x01000000
#define HwPortABCD_NAND_CLE                     0x02000000
#define HwPortABCD_NAND_WP                      0x04000000
#define HwPortABCD_SmartMedia_CS                0x08000000
#define HwPortABCD_NAND1_CS                     0x10000000
#define HwPortABCD_NAND2_CS                     0x20000000
#define HwPortABCD_SmartMedia_Power             0x40000000
#define HwPortABCD_NAND_Ready                   0x80000000

//
// The I2C addresses of the CS43L43 and CS53L32.
//
#define I2CADDR_CS43L43                         0x00000020
#define I2CADDR_CS53L32                         0x00000022

//
// Definitions of the external interrupt sources.
//
#define HwIrqUSB                                HwIrqExt1

//
// The hardware ID for this board.
//
#define HardwareID                              0x01020100

#endif

//****************************************************************************
//
// Definitions for the EP7209 reference design board, Rev. B.
//
//****************************************************************************

#ifdef EP7209_Reference_Board_Rev_B

//
// Indicate that we use the LCD for our user interface.
//
#define UI_IS_LCD

//
// Indicate that we need the voice record capability.
//
#define REQUIRE_RECORD

//
// Definitions of the bit fields in the HwPortABCD register.
//
#define HwPortABCD_Button_Mask                  0x000000ff
#define HwPortABCD_NAND_ALE                     0x00000100
#define HwPortABCD_NAND_CLE                     0x00000200
#define HwPortABCD_NAND_WP                      0x00000400
#define HwPortABCD_SmartMedia_CS                0x00000800
#define HwPortABCD_NAND1_CS                     0x00001000
#define HwPortABCD_NAND2_CS                     0x00002000
#define HwPortABCD_SmartMedia_Power             0x00004000
#define HwPortABCD_NAND_Ready                   0x00008000
#define HwPortABCD_I2C_SCL                      0x01000000
#define HwPortABCD_I2C_SDA                      0x02000000
#define HwPortABCD_SmartMedia_WP                0x04000000
#define HwPortABCD_DAC_Enable                   0x08000000
#define HwPortABCD_ADC_Enable                   0x10000000
#define HwPortABCD_USB_Suspend                  0x20000000

//
// The I2C addresses of the CS43L43 and CS53L32.
//
#define I2CADDR_CS43L43                         0x00000020
#define I2CADDR_CS53L32                         0x00000022

//
// Definitions of the external interrupt sources.
//
#define HwIrqSmartMediaInsert                   HwIrqExt3
#define HwIrqSmartMediaRemove                   HwIrqExt2
#define HwIrqUSB                                HwIrqExt1

//
// Definition of the bit which indicates whether or not the USB cable is
// connected.
//
#define HwRegUSBCable                           HwIntStatus
#define HwBitUSBCable                           HwFiqExt
#define HwUSBCableConnected                     0

//
// The hardware ID for this board.
//
#define HardwareID                              0x01020200

#endif

//****************************************************************************
//
// Definitions for the EP73XX evaluation board.
//
//****************************************************************************

#ifdef EP73XX_Eval_Board

//
// Indicate that we use the serial port for our "user interface".
//
#define UI_IS_SERIAL_PORT

//
// Indicate that we need the voice record capability.
//
#define REQUIRE_RECORD

//
// Definitions of the bit fields in the HwPortABCD register.
//
#define HwPortABCD_SmartMedia_Power             0x00000000
#define HwPortABCD_Button_Mask                  0x000000ff
#define HwPortABCD_NAND_CLE                     0x00001000
#define HwPortABCD_NAND_ALE                     0x00002000
#define HwPortABCD_NAND1_CS                     0x0000c000
#define HwPortABCD_NAND2_CS                     0x00008000
#define HwPortABCD_SmartMedia_CS                0x00004000
#define HwPortABCD_I2C_SDA                      0x10000000
#define HwPortABCD_I2C_SCL                      0x20000000

//
// Definitions of the bit fields in the HwPortE register.
//
#define HwPortE_Codec_Power                     0x00000001

//
// The I2C addresses of the CS43L42 and CS53L32.
//
#define I2CADDR_CS43L43                         0x00000020
#define I2CADDR_CS53L32                         0x00000022

//
// Definitions of the external interrupt sources.
//
#define HwIrqUSB                                HwIrqExt1

//
// The hardware ID for this board.
//
#define HardwareID                              0x02010100

#endif

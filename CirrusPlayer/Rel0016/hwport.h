//****************************************************************************
//
// HWPORT.H - Definitions required to run the player on various hardware
//            platforms.  The definitions in this file must match those in
//            hwport.inc.
//
// Copyright (c) 2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************

//****************************************************************************
//
// Definitions for the EP7209 evaluation board.
//
//****************************************************************************

#ifdef EP7209_Eval_Board

//
// This board uses an EP72XX.
//
#define PROCESSOR_EP72XX

//
// Indicate that we use the serial port for our "user interface".
//
#define UI_IS_SERIAL_PORT

//
// Since we do not have a CS43L42 or CS43L43, use a software volume control.
//
#define SOFTWARE_VOLUME_CONTROL

//
// The EP7209 evaluation board is populated with Intel B3 FLASH devices.
//
#define FLASH_INTEL_B3

//
// The base address of the player image in the FLASH.
//
#define HwPlayerAddress                         0x00004000

//
// Definitions of the bit fields in the HwPortABCD register.
//
#define HwPortABCD_Button_Mask                  0x000000ff
#define HwPortABCD_NAND_CLE                     0x00001000
#define HwPortABCD_NAND_ALE                     0x00002000
#define HwPortABCD_NAND1_CS                     0x00004000
#define HwPortABCD_SmartMedia_CS                0x00008000
//#define HwPortABCD_Data                         0x10000000
//#define HwPortABCD_Clock                        0x20000000

//
// Definitions of the bit fields in the HwPortE register.
//
#define HwPortE_Codec_Power                     0x00000001

//
// The address of the CS43L43.
//
//#define ADDR_CS43L43                            0x00000020

//
// The default GPIO pin directions and values.
//
#define HwPortABCD_DefaultDir                   0x0000fa00
#define HwPortE_DefaultDir                      0x00000007
#define HwPortABCD_DefaultValue                 0x3100c000
#define HwPortE_DefaultValue                    0x00000000

//
// The values for the two memory configuration registers.
//
#define HwMemConfig1_Value                      0x00001814
#define HwMemConfig2_Value                      0x0000001a

//
// The values for the three system control registers.
//
#define HwControl1_Value                        0x00000010
#define HwControl2_Value                        0x00000000
#define HwControl3_Value                        0x0000020e

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
// Definitions for the EP7212 reference design board, Rev. A.
//
//****************************************************************************

#ifdef EP7212_Reference_Board_Rev_A

//
// This board uses an EP72XX.
//
#define PROCESSOR_EP72XX

//
// Indicate that we use the serial port for our "user interface".
//
#define UI_IS_SERIAL_PORT

//
// Indicate that we need the voice record capability.
//
#define REQUIRE_RECORD

//
// The EP7209 reference design board, rev. A, is populated with Intel B3 FLASH
// devices.
//
#define FLASH_INTEL_B3

//
// The base address of the player image in the FLASH.
//
#define HwPlayerAddress                         0x00004000

//
// Definitions of the bit fields in the HwPortABCD register.
//
#define HwPortABCD_Button_Mask                  0x000000ff
#define HwPortABCD_Clock                        0x00000100
#define HwPortABCD_Data                         0x00000200
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
// The address of the CS43L43 and CS53L32.
//
#define ADDR_CS43L43                            0x00000020
#define ADDR_CS53L32                            0x00000022

//
// The default GPIO pin directions and values.
//
#define HwPortABCD_DefaultDir                   0x8000ff00
#define HwPortE_DefaultDir                      0x00000000
#define HwPortABCD_DefaultValue                 0x3c000300
#define HwPortE_DefaultValue                    0x00000000

//
// The values for the two memory configuration registers.
//
#define HwMemConfig1_Value                      0x00001814
#define HwMemConfig2_Value                      0x0000001a

//
// The values for the three system control registers.
//
#define HwControl1_Value                        0x00000012
#define HwControl2_Value                        0x00000000
#define HwControl3_Value                        0x0000020e

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
// Definitions for the EP7212 reference design board, Rev. B.
//
//****************************************************************************

#ifdef EP7212_Reference_Board_Rev_B

//
// This board uses an EP72XX.
//
#define PROCESSOR_EP72XX

//
// Indicate that we use the LCD for our user interface.
//
#define UI_IS_LCD

//
// Indicate that we need the voice record capability.
//
#define REQUIRE_RECORD

//
// Loopback is a test mode for testing the record capability of the board.
//
#define REQUIRE_LOOPBACK

//
// The EP7209 reference design board, rev. B, is populated with an Intel J3
// FLASH device, and the data bus is reversed (inadvertently).
//
#define FLASH_INTEL_J3
#define DATA_BUS_REVERSED

//
// The base address of the player image in the FLASH.
//
#define HwPlayerAddress                         0x00020000

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
#define HwPortABCD_Clock                        0x01000000
#define HwPortABCD_Data                         0x02000000
#define HwPortABCD_SmartMedia_WP                0x04000000
#define HwPortABCD_DAC_Enable                   0x08000000
#define HwPortABCD_ADC_Enable                   0x10000000
#define HwPortABCD_USB_Suspend                  0x20000000

//
// The address of the CS43L43 and CS53L32.
//
#define ADDR_CS43L43                            0x00000020
#define ADDR_CS53L32                            0x00000022

//
// The default GPIO pin directions and values.
//
#define HwPortABCD_DefaultDir                   0x04007f00
#define HwPortE_DefaultDir                      0x00000000
#define HwPortABCD_DefaultValue                 0x03007c00
#define HwPortE_DefaultValue                    0x00000000

//
// The values for the two memory configuration registers.
//
#define HwMemConfig1_Value                      0x001c1874
#define HwMemConfig2_Value                      0x0000001a

//
// The values for the three system control registers.
//
#define HwControl1_Value                        0x00000012
#define HwControl2_Value                        0x00000004
#define HwControl3_Value                        0x0000020e

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
// Definitions for the EP7309 reference design board, Rev. B.
//
//****************************************************************************

#ifdef EP7309_Reference_Board_Rev_B

//
// This board uses an EP73XX.
//
#define PROCESSOR_EP73XX

//
// Indicate that we use the LCD for our user interface.
//
#define UI_IS_LCD

//
// Indicate that we need the voice record capability.
//
#define REQUIRE_RECORD

//
// Loopback is a test mode for testing the record capability of the board.
//
#define REQUIRE_LOOPBACK

//
// The EP7209 reference design board, rev. B, is populated with an Intel J3
// FLASH device, and the data bus is reversed (inadvertently).
//
#define FLASH_INTEL_J3
#define DATA_BUS_REVERSED

//
// Indicate that we use the clock generator in the digital audio interface.
//
#define USE_DAI_CLOCK_GENERATOR

//
// The base address of the player image in the FLASH.
//
#define HwPlayerAddress                         0x00020000

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
#define HwPortABCD_Clock                        0x01000000
#define HwPortABCD_Data                         0x02000000
#define HwPortABCD_SmartMedia_WP                0x04000000
#define HwPortABCD_DAC_Enable                   0x08000000
#define HwPortABCD_ADC_Enable                   0x10000000
#define HwPortABCD_USB_Suspend                  0x20000000

//
// The address of the CS43L43 and CS53L32.
//
#define ADDR_CS43L43                            0x00000020
#define ADDR_CS53L32                            0x00000022

//
// The default GPIO pin directions and values.
//
#define HwPortABCD_DefaultDir                   0x04007f00
#define HwPortE_DefaultDir                      0x00000000
#define HwPortABCD_DefaultValue                 0x03007c00
#define HwPortE_DefaultValue                    0x00000000

//
// The values for the two memory configuration registers.
//
#define HwMemConfig1_Value                      0x001c1874
#define HwMemConfig2_Value                      0x0000001a

//
// The values for the three system control registers.
//
#define HwControl1_Value                        0x00000012
#define HwControl2_Value                        0x00000004
#define HwControl3_Value                        0x0000000e

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
#define HardwareID                              0x01030200

#endif

//****************************************************************************
//
// Definitions for the EP7212 reference design board, Rev. C.
//
//****************************************************************************

#ifdef EP7212_Reference_Board_Rev_C

//
// This board uses an EP72XX.
//
#define PROCESSOR_EP72XX

//
// Indicate that we use the LCD for our user interface.
//
#define UI_IS_LCD

//
// Indicate that we need the voice record capability.
//
#define REQUIRE_RECORD

//
// Loopback is a test mode for testing the record capability of the board.
//
#define REQUIRE_LOOPBACK

//
// The EP7209 reference design board, rev. C, is populated with an Intel J3
// FLASH device.
//
#define FLASH_INTEL_J3

//
// The base address of the player image in the FLASH.
//
#define HwPlayerAddress                         0x00020000

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
#define HwPortABCD_Clock                        0x01000000
#define HwPortABCD_Data                         0x02000000
#define HwPortABCD_SmartMedia_WP                0x04000000
#define HwPortABCD_DAC_Enable                   0x08000000
#define HwPortABCD_ADC_Enable                   0x10000000
#define HwPortABCD_USB_CS                       0x20000000

//
// The address of the CS43L43 and CS53L32.
//
#define ADDR_CS43L43                            0x00000020
#define ADDR_CS53L32                            0x00000022

//
// The default GPIO pin directions and values.
//
#define HwPortABCD_DefaultDir                   0x04007f00
#define HwPortE_DefaultDir                      0x00000000
#define HwPortABCD_DefaultValue                 0x23007c00
#define HwPortE_DefaultValue                    0x00000000

//
// The values for the two memory configuration registers.
//
#define HwMemConfig1_Value                      0x001c1874
#define HwMemConfig2_Value                      0x0000001a

//
// The values for the three system control registers.
//
#define HwControl1_Value                        0x00000012
#define HwControl2_Value                        0x00000004
#define HwControl3_Value                        0x0000020e

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
#define HardwareID                              0x01020300

#endif

//****************************************************************************
//
// Definitions for the EP7309 reference design board, Rev. C.
//
//****************************************************************************

#ifdef EP7309_Reference_Board_Rev_C

//
// This board uses an EP73XX.
//
#define PROCESSOR_EP73XX

//
// Indicate that we use the LCD for our user interface.
//
#define UI_IS_LCD

//
// Indicate that we need the voice record capability.
//
#define REQUIRE_RECORD

//
// Loopback is a test mode for testing the record capability of the board.
//
#define REQUIRE_LOOPBACK

//
// The EP7209 reference design board, rev. C, is populated with an Intel J3
// FLASH device.
//
#define FLASH_INTEL_J3

//
// Indicate that we use the clock generator in the digital audio interface.
//
#define USE_DAI_CLOCK_GENERATOR

//
// The base address of the player image in the FLASH.
//
#define HwPlayerAddress                         0x00020000

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
#define HwPortABCD_Clock                        0x01000000
#define HwPortABCD_Data                         0x02000000
#define HwPortABCD_SmartMedia_WP                0x04000000
#define HwPortABCD_DAC_Enable                   0x08000000
#define HwPortABCD_ADC_Enable                   0x10000000
#define HwPortABCD_USB_CS                       0x20000000

//
// The address of the CS43L43 and CS53L32.
//
#define ADDR_CS43L43                            0x00000020
#define ADDR_CS53L32                            0x00000022

//
// The default GPIO pin directions and values.
//
#define HwPortABCD_DefaultDir                   0x04007f00
#define HwPortE_DefaultDir                      0x00000006
#define HwPortABCD_DefaultValue                 0x23007c00
#define HwPortE_DefaultValue                    0x00000006

//
// The values for the two memory configuration registers.
//
#define HwMemConfig1_Value                      0x001c1874
#define HwMemConfig2_Value                      0x0000001a

//
// The values for the three system control registers.
//
#define HwControl1_Value                        0x00000012
#define HwControl2_Value                        0x00000004
#define HwControl3_Value                        0x0000000e

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
#define HardwareID                              0x01030300

#endif

//****************************************************************************
//
// Definitions for the EP73XX evaluation board.
//
//****************************************************************************

#ifdef EP73XX_Eval_Board

//
// This board uses an EP73XX.
//
#define PROCESSOR_EP73XX

//
// Indicate that we use the serial port for our "user interface".
//
#define UI_IS_SERIAL_PORT

//
// Indicate that we need the voice record capability.
//
#define REQUIRE_RECORD

//
// The EP73XX evaluation board is populated with Intel B3 FLASH devices.
//
#define FLASH_INTEL_B3

//
// The base address of the player image in the FLASH.
//
#define HwPlayerAddress                         0x00004000

//
// Definitions of the bit fields in the HwPortABCD register.
//
#define HwPortABCD_Button_Mask                  0x000000ff
#define HwPortABCD_NAND_CLE                     0x00001000
#define HwPortABCD_NAND_ALE                     0x00002000
#define HwPortABCD_NAND1_CS                     0x0000c000
#define HwPortABCD_NAND2_CS                     0x00008000
#define HwPortABCD_SmartMedia_CS                0x00004000
#define HwPortABCD_Data                         0x10000000
#define HwPortABCD_Clock                        0x20000000

//
// Definitions of the bit fields in the HwPortE register.
//
#define HwPortE_Codec_Power                     0x00000001

//
// The address of the CS43L42 and CS53L32.
//
#define ADDR_CS43L43                            0x00000020
#define ADDR_CS53L32                            0x00000022

//
// The default GPIO pin directions and values.
//
#define HwPortABCD_DefaultDir                   0x0000fa00
#define HwPortE_DefaultDir                      0x00000007
#define HwPortABCD_DefaultValue                 0x3100c000
#define HwPortE_DefaultValue                    0x00000000

//
// The values for the two memory configuration registers.
//
#define HwMemConfig1_Value                      0x00001814
#define HwMemConfig2_Value                      0x0000001a

//
// The values for the three system control registers.
//
#define HwControl1_Value                        0x00000010
#define HwControl2_Value                        0x00000000
#define HwControl3_Value                        0x0000020e

//
// Definitions of the external interrupt sources.
//
#define HwIrqUSB                                HwIrqExt1

//
// The hardware ID for this board.
//
#define HardwareID                              0x02010100

#endif

//****************************************************************************
//
// Definitions for the EP7312 evaluation board.
//
//****************************************************************************

#ifdef EP7312_Eval_Board

//
// This board uses an EP73XX.
//
#define PROCESSOR_EP73XX

//
// Indicate that we use the serial port for our "user interface".
//
#define UI_IS_SERIAL_PORT

//
// Indicate that we need the voice record capability.
//
#define REQUIRE_RECORD

//
// The EP7312 evaluation board is populated with a Intel J3 FLASH device.
//
#define FLASH_INTEL_J3

//
// The base address of the player image in the FLASH.
//
#define HwPlayerAddress                         0x00020000

//
// Definitions of the bit fields in the HwPortABCD register.
//
#define HwPortABCD_Button_Mask                  0x000000ff
#define HwPortABCD_NAND_CLE                     0x00001000
#define HwPortABCD_NAND_ALE                     0x00002000
#define HwPortABCD_NAND1_CS                     0x00004000
#define HwPortABCD_SmartMedia_CS                0x00008000
#define HwPortABCD_Data                         0x10000000
#define HwPortABCD_Clock                        0x20000000

//
// Definitions of the bit fields in the HwPortE register.
//
#define HwPortE_Codec_Power                     0x00000001

//
// The address of the CS43L42 and CS53L32.
//
#define ADDR_CS43L43                            0x00000020
#define ADDR_CS53L32                            0x00000022

//
// The default GPIO pin directions and values.
//
#define HwPortABCD_DefaultDir                   0x0000fa00
#define HwPortE_DefaultDir                      0x00000007
#define HwPortABCD_DefaultValue                 0x3100c000
#define HwPortE_DefaultValue                    0x00000000

//
// The values for the two memory configuration registers.
//
#define HwMemConfig1_Value                      0x0000186c
#define HwMemConfig2_Value                      0x0000001a

//
// The values for the three system control registers.
//
#define HwControl1_Value                        0x00000010
#define HwControl2_Value                        0x00000000
#define HwControl3_Value                        0x0000020e

//
// Definitions of the external interrupt sources.
//
#define HwIrqUSB                                HwIrqExt1

//
// The hardware ID for this board.
//
#define HardwareID                              0x02020100

#endif

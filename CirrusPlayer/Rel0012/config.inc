;*****************************************************************************
;
; CONFIG.INC - Configuration values for the Internet audio player.  The
;              definitions in this file must match those in config.h.
;
; Copyright (c) 2000 Cirrus Logic, Inc.
;
;*****************************************************************************

;*****************************************************************************
;
; Defines for the various platforms to which the player has been ported.  Only
; one of these can be defined at a time.
;
;*****************************************************************************
EP7209_Eval_Board               EQU     1
;EP7209_Reference_Board_Rev_A    EQU     1
;EP7209_Reference_Board_Rev_B    EQU     1
;EP73XX_Eval_Board               EQU     1

;*****************************************************************************
;
; Definitions for the EP7209 evaluation board.
;
;*****************************************************************************

    IF (:DEF: EP7209_Eval_Board)

;
; Indicate that we use the serial port for our "user interface".
;
UI_IS_SERIAL_PORT               EQU     1

;
; Definitions of the bit fields in the HwPortABCD register.
;
HwPortABCD_SmartMedia_Power     EQU     0x00000000
HwPortABCD_Button_Mask          EQU     0x000000ff
HwPortABCD_NAND_CLE             EQU     0x00001000
HwPortABCD_NAND_ALE             EQU     0x00002000
HwPortABCD_NAND1_CS             EQU     0x00004000
HwPortABCD_SmartMedia_CS        EQU     0x00008000
;HwPortABCD_I2C_SDA              EQU     0x10000000
;HwPortABCD_I2C_SCL              EQU     0x20000000

;
; Definitions of the bit fields in the HwPortE register.
;
HwPortE_Codec_Power             EQU     0x00000001

;
; The I2C address of the CS43L43.
;
;I2CADDR_CS43L43                 EQU     0x00000020

;
; The default GPIO pin directions and values.
;
HwPortABCD_DefaultDir           EQU     0x0000fa00
HwPortE_DefaultDir              EQU     0x00000007
HwPortABCD_DefaultValue         EQU     0x3100c000
HwPortE_DefaultValue            EQU     0x00000000

;
; The values for the two memory configuration registers.
;
HwMemConfig1_Value              EQU     0x00001814
HwMemConfig2_Value              EQU     0x0000001a

;
; The values for the three system control registers.
;
HwControl1_Value                EQU     0x00000010
HwControl2_Value                EQU     0x00000000
HwControl3_Value                EQU     0x0000020e

;
; Definitions of the external interrupt sources.
;
HwIrqUSB                        EQU     HwIrqExt1

;
; The hardware ID for this board.
;
HardwareID                      EQU     0x01010100

    ENDIF

;*****************************************************************************
;
; Definitions for the EP7209 reference design board, Rev. A.
;
;*****************************************************************************

    IF (:DEF: EP7209_Reference_Board_Rev_A)

;
; Indicate that we use the serial port for our "user interface".
;
UI_IS_SERIAL_PORT               EQU     1

;
; Indicate that we need the voice record capability.
;
REQUIRE_RECORD                  EQU     1

;
; Definitions of the bit fields in the HwPortABCD register.
;
HwPortABCD_Button_Mask          EQU     0x000000ff
HwPortABCD_I2C_SCL              EQU     0x00000100
HwPortABCD_I2C_SDA              EQU     0x00000200
HwPortABCD_Codec_Power          EQU     0x00000400
HwPortABCD_NAND_ALE             EQU     0x01000000
HwPortABCD_NAND_CLE             EQU     0x02000000
HwPortABCD_NAND_WP              EQU     0x04000000
HwPortABCD_SmartMedia_CS        EQU     0x08000000
HwPortABCD_NAND1_CS             EQU     0x10000000
HwPortABCD_NAND2_CS             EQU     0x20000000
HwPortABCD_SmartMedia_Power     EQU     0x40000000
HwPortABCD_NAND_Ready           EQU     0x80000000

;
; The I2C addresses of the CS43L43 and CS53L32.
;
I2CADDR_CS43L43                 EQU     0x00000020
I2CADDR_CS53L32                 EQU     0x00000022

;
; The default GPIO pin directions and values.
;
HwPortABCD_DefaultDir           EQU     0x8000ff00
HwPortE_DefaultDir              EQU     0x00000000
HwPortABCD_DefaultValue         EQU     0x3c000300
HwPortE_DefaultValue            EQU     0x00000000

;
; The values for the two memory configuration registers.
;
HwMemConfig1_Value              EQU     0x00001814
HwMemConfig2_Value              EQU     0x0000001a

;
; The values for the three system control registers.
;
HwControl1_Value                EQU     0x00000012
HwControl2_Value                EQU     0x00000000
HwControl3_Value                EQU     0x0000020e

;
; Definitions of the external interrupt sources.
;
HwIrqUSB                        EQU     HwIrqExt1

;
; The hardware ID for this board.
;
HardwareID                      EQU     0x01020100

    ENDIF

;*****************************************************************************
;
; Definitions for the EP7209 reference design board, Rev. B.
;
;*****************************************************************************

    IF (:DEF: EP7209_Reference_Board_Rev_B)

;
; Indicate that we use the LCD for our user interface.
;
UI_IS_LCD                       EQU     1

;
; Indicate that we need the voice record capability.
;
REQUIRE_RECORD                  EQU     1

;
; Definitions of the bit fields in the HwPortABCD register.
;
HwPortABCD_Button_Mask          EQU     0x000000ff
HwPortABCD_NAND_ALE             EQU     0x00000100
HwPortABCD_NAND_CLE             EQU     0x00000200
HwPortABCD_NAND_WP              EQU     0x00000400
HwPortABCD_SmartMedia_CS        EQU     0x00000800
HwPortABCD_NAND1_CS             EQU     0x00001000
HwPortABCD_NAND2_CS             EQU     0x00002000
HwPortABCD_SmartMedia_Power     EQU     0x00004000
HwPortABCD_NAND_Ready           EQU     0x00008000
HwPortABCD_I2C_SCL              EQU     0x01000000
HwPortABCD_I2C_SDA              EQU     0x02000000
HwPortABCD_SmartMedia_WP        EQU     0x04000000
HwPortABCD_DAC_Enable           EQU     0x08000000
HwPortABCD_ADC_Enable           EQU     0x10000000
HwPortABCD_USB_Suspend          EQU     0x20000000

;
; The I2C addresses of the CS43L43 and CS53L32.
;
I2CADDR_CS43L43                 EQU     0x00000020
I2CADDR_CS53L32                 EQU     0x00000022

;
; The default GPIO pin directions and values.
;
HwPortABCD_DefaultDir           EQU     0x04007f00
HwPortE_DefaultDir              EQU     0x00000000
HwPortABCD_DefaultValue         EQU     0x03007c00
HwPortE_DefaultValue            EQU     0x00000000

;
; The values for the two memory configuration registers.
;
HwMemConfig1_Value              EQU     0x001c1874
HwMemConfig2_Value              EQU     0x0000001a

;
; The values for the three system control registers.
;
HwControl1_Value                EQU     0x00000012
HwControl2_Value                EQU     0x00000004
HwControl3_Value                EQU     0x0000020e

;
; Definitions of the external interrupt sources.
;
HwIrqSmartMediaInsert           EQU     HwIrqExt3
HwIrqSmartMediaRemove           EQU     HwIrqExt2
HwIrqUSB                        EQU     HwIrqExt1

;
; Definition of the bit which indicates whether or not the USB cable is
; connected.
;
HwRegUSBCable                   EQU     HwIntStatus
HwBitUSBCable                   EQU     HwFiqExt
HwUSBCableConnected             EQU     0

;
; The hardware ID for this board.
;
HardwareID                      EQU     0x01020200

    ENDIF

;*****************************************************************************
;
; Definitions for the EP73XX evaluation board.
;
;*****************************************************************************

    IF (:DEF: EP73XX_Eval_Board)

;
; Indicate that we use the serial port for our "user interface".
;
UI_IS_SERIAL_PORT               EQU     1

;
; Indicate that we need the voice record capability.
;
REQUIRE_RECORD                  EQU     1

;
; Definitions of the bit fields in the HwPortABCD register.
;
HwPortABCD_SmartMedia_Power     EQU     0x00000000
HwPortABCD_Button_Mask          EQU     0x000000ff
HwPortABCD_NAND_CLE             EQU     0x00001000
HwPortABCD_NAND_ALE             EQU     0x00002000
HwPortABCD_NAND1_CS             EQU     0x0000c000
HwPortABCD_NAND2_CS             EQU     0x00008000
HwPortABCD_SmartMedia_CS        EQU     0x00004000
HwPortABCD_I2C_SDA              EQU     0x10000000
HwPortABCD_I2C_SCL              EQU     0x20000000

;
; Definitions of the bit fields in the HwPortE register.
;
HwPortE_Codec_Power             EQU     0x00000001

;
; The I2C addresses of the CS43L42 and CS53L32.
;
I2CADDR_CS43L43                 EQU     0x00000020
I2CADDR_CS53L32                 EQU     0x00000022

;
; The default GPIO pin directions and values.
;
HwPortABCD_DefaultDir           EQU     0x0000fa00
HwPortE_DefaultDir              EQU     0x00000007
HwPortABCD_DefaultValue         EQU     0x3100c000
HwPortE_DefaultValue            EQU     0x00000000

;
; The values for the two memory configuration registers.
;
HwMemConfig1_Value              EQU     0x00001814
HwMemConfig2_Value              EQU     0x0000001a

;
; The values for the three system control registers.
;
HwControl1_Value                EQU     0x00000010
HwControl2_Value                EQU     0x00000000
HwControl3_Value                EQU     0x0000020e

;
; Definitions of the external interrupt sources.
;
HwIrqUSB                        EQU     HwIrqExt1

;
; The hardware ID for this board.
;
HardwareID                      EQU     0x02010100

    ENDIF

    END

//****************************************************************************
//
// MMC.H - Definitions of the MMC commands, states, and registers.
//
// Copyright (c) 2001 Cirrus Logic, Inc.
//
//****************************************************************************

//****************************************************************************
//
// The MMC commands in version 3.0 of the specification.
//
//****************************************************************************
#define GO_IDLE_STATE                           0
#define SEND_OP_COND                            1
#define ALL_SEND_CID                            2
#define SET_RELATIVE_ADDR                       3
#define SET_DSR                                 4
#define SELECT_CARD                             7
#define SEND_CSD                                9
#define SEND_CID                                10
#define READ_DAT_UNTIL_STOP                     11
#define STOP_TRANSMISSION                       12
#define SEND_STATUS                             13
#define GO_INACTIVE_STATE                       15
#define SET_BLOCKLEN                            16
#define READ_SINGLE_BLOCK                       17
#define READ_MULTIPLE_BLOCK                     18
#define WRITE_DAT_UNTIL_STOP                    20
#define SET_BLOCK_COUNT                         23
#define WRITE_BLOCK                             24
#define WRITE_MULTIPLE_BLOCK                    25
#define PROGRAM_CID                             26
#define PROGRAM_CSD                             27
#define SET_WRITE_PROT                          28
#define CLR_WRITE_PROT                          29
#define SEND_WRITE_PROT                         30
#define TAG_ERASE_GROUP_START                   35
#define TAG_ERASE_GROUP_END                     36
#define ERASE                                   38
#define FAST_IO                                 39
#define GO_IRQ_STATE                            40
#define LOCK_UNLOCK                             42
#define APP_CMD                                 55
#define GEN_CMD                                 56

//****************************************************************************
//
// The states of a MMC card.
//
//****************************************************************************
#define STATE_IDLE                              0
#define STATE_READY                             1
#define STATE_IDENT                             2
#define STATE_STBY                              3
#define STATE_TRAN                              4
#define STATE_DATA                              5
#define STATE_RCV                               6
#define STATE_PRG                               7
#define STATE_DIS                               8

//****************************************************************************
//
// Definitions of the bit fields in the card status response.
//
//****************************************************************************
#define STATUS_APP_CMD                          0x00000020
#define STATUS_READY_FOR_DATA                   0x00000100
#define STATUS_CURRENT_STATE_MASK               0x00001e00
#define STATUS_ERASE_RESET                      0x00002000
#define STATUS_CARD_ECC_DISABLED                0x00004000
#define STATUS_WP_ERASE_SKIP                    0x00008000
#define STATUS_CIS_CSD_OVERWRITE                0x00010000
#define STATUS_OVERRUN                          0x00020000
#define STATUS_UNDERRUN                         0x00040000
#define STATUS_ERROR                            0x00080000
#define STATUS_CC_ERROR                         0x00100000
#define STATUS_CARD_ECC_FAILED                  0x00200000
#define STATUS_ILLEGAL_COMMAND                  0x00400000
#define STATUS_COM_CRC_ERROR                    0x00800000
#define STATUS_LOCK_UNLOCK_FAILED               0x01000000
#define STATUS_CARD_IS_LOCKED                   0x02000000
#define STATUS_WP_VIOLATION                     0x04000000
#define STATUS_ERASE_PARAM                      0x08000000
#define STATUS_ERASE_SEQ_ERROR                  0x10000000
#define STATUS_BLOCK_LEN_ERROR                  0x20000000
#define STATUS_ADDRESS_ERROR                    0x40000000
#define STATUS_OUT_OF_RANGE                     0x80000000
#define STATUS_CURRENT_STATE_SHIFT              9

//****************************************************************************
//
// Definitions of the bit fields in the operating conditions register (OCR).
//
//****************************************************************************
#define OCR_20_21                               0x00000100
#define OCR_21_22                               0x00000200
#define OCR_22_23                               0x00000400
#define OCR_23_24                               0x00000800
#define OCR_24_25                               0x00001000
#define OCR_25_26                               0x00002000
#define OCR_26_27                               0x00004000
#define OCR_27_28                               0x00008000
#define OCR_28_29                               0x00010000
#define OCR_29_30                               0x00020000
#define OCR_30_31                               0x00040000
#define OCR_31_32                               0x00080000
#define OCR_32_33                               0x00100000
#define OCR_33_34                               0x00200000
#define OCR_34_35                               0x00400000
#define OCR_35_36                               0x00800000
#define OCR_READY                               0x80000000

//****************************************************************************
//
// Definition of the CSD_STRUCTURE field in the CSD register.
//
//****************************************************************************
#define CSD_STRUCTURE_1_0                       0
#define CSD_STRUCTURE_1_1                       1
#define CSD_STRUCTURE_1_2                       2
#define CSD_STRUCTURE_1_3                       3

//****************************************************************************
//
// Definition of the SPEC_VERS field in the CSD register.
//
//****************************************************************************
#define SPEC_VERS_1_2                           0
#define SPEC_VERS_1_4                           1
#define SPEC_VERS_2_11                          2
#define SPEC_VERS_2_21                          3
#define SPEC_VERS_3_0                           4

//****************************************************************************
//
// Definition of the TAAC field in the CSD register.
//
//****************************************************************************
#define TAAC_UNIT_MASK                          0x07
#define TAAC_UNIT_1NS                           0x00
#define TAAC_UNIT_10NS                          0x01
#define TAAC_UNIT_100NS                         0x02
#define TAAC_UNIT_1US                           0x03
#define TAAC_UNIT_10US                          0x04
#define TAAC_UNIT_100US                         0x05
#define TAAC_UNIT_1MS                           0x06
#define TAAC_UNIT_10MS                          0x07
#define TAAC_VALUE_MASK                         0x78
#define TAAC_VALUE_1_0                          0x08
#define TAAC_VALUE_1_2                          0x10
#define TAAC_VALUE_1_3                          0x18
#define TAAC_VALUE_1_5                          0x20
#define TAAC_VALUE_2_0                          0x28
#define TAAC_VALUE_2_5                          0x30
#define TAAC_VALUE_3_0                          0x38
#define TAAC_VALUE_3_5                          0x40
#define TAAC_VALUE_4_0                          0x48
#define TAAC_VALUE_4_5                          0x50
#define TAAC_VALUE_5_0                          0x58
#define TAAC_VALUE_5_5                          0x60
#define TAAC_VALUE_6_0                          0x68
#define TAAC_VALUE_7_0                          0x70
#define TAAC_VALUE_8_0                          0x78

//****************************************************************************
//
// Definition of the TRAN_SPEED field in the CSD register.
//
//****************************************************************************
#define TRAN_SPEED_UNIT_MASK                    0x07
#define TRAN_SPEED_UNIT_100KB_S                 0x00
#define TRAN_SPEED_UNIT_1MB_S                   0x01
#define TRAN_SPEED_UNIT_10MB_S                  0x02
#define TRAN_SPEED_UNIT_100MB_S                 0x03
#define TRAN_SPEED_VALUE_MASK                   0x78
#define TRAN_SPEED_VALUE_1_0                    0x08
#define TRAN_SPEED_VALUE_1_2                    0x10
#define TRAN_SPEED_VALUE_1_3                    0x18
#define TRAN_SPEED_VALUE_1_5                    0x20
#define TRAN_SPEED_VALUE_2_0                    0x28
#define TRAN_SPEED_VALUE_2_5                    0x30
#define TRAN_SPEED_VALUE_3_0                    0x38
#define TRAN_SPEED_VALUE_3_5                    0x40
#define TRAN_SPEED_VALUE_4_0                    0x48
#define TRAN_SPEED_VALUE_4_5                    0x50
#define TRAN_SPEED_VALUE_5_0                    0x58
#define TRAN_SPEED_VALUE_5_5                    0x60
#define TRAN_SPEED_VALUE_6_0                    0x68
#define TRAN_SPEED_VALUE_7_0                    0x70
#define TRAN_SPEED_VALUE_8_0                    0x78

//****************************************************************************
//
// Definition of the CCC field in the CSD register.
//
//****************************************************************************
#define CCC_CLASS_0                             0x001
#define CCC_CLASS_1                             0x002
#define CCC_CLASS_2                             0x004
#define CCC_CLASS_3                             0x008
#define CCC_CLASS_4                             0x010
#define CCC_CLASS_5                             0x020
#define CCC_CLASS_6                             0x040
#define CCC_CLASS_7                             0x080
#define CCC_CLASS_8                             0x100
#define CCC_CLASS_9                             0x200
#define CCC_CLASS_10                            0x400
#define CCC_CLASS_11                            0x800

//****************************************************************************
//
// Definition of the VDD_R_CURR_MIN and VDD_W_CURR_MIN fields in the CSD
// register.
//
//****************************************************************************
#define VDD_CURR_MIN_0_5MA                      0x0
#define VDD_CURR_MIN_1_0MA                      0x1
#define VDD_CURR_MIN_5_0MA                      0x2
#define VDD_CURR_MIN_10_0MA                     0x3
#define VDD_CURR_MIN_25_0MA                     0x4
#define VDD_CURR_MIN_35_0MA                     0x5
#define VDD_CURR_MIN_60_0MA                     0x6
#define VDD_CURR_MIN_100_0MA                    0x7

//****************************************************************************
//
// Definition of the VDD_R_CURR_MAX and VDD_W_CURR_MAX fields in the CSD
// register.
//
//****************************************************************************
#define VDD_CURR_MAX_1_0MA                      0x0
#define VDD_CURR_MAX_5_0MA                      0x1
#define VDD_CURR_MAX_10_0MA                     0x2
#define VDD_CURR_MAX_25_0MA                     0x3
#define VDD_CURR_MAX_35_0MA                     0x4
#define VDD_CURR_MAX_45_0MA                     0x5
#define VDD_CURR_MAX_80_0MA                     0x6
#define VDD_CURR_MAX_200_0MA                    0x7

//****************************************************************************
//
// Definition of the DEFAULT_ECC and ECC fields in the CSD register.
//
//****************************************************************************
#define ECC_NONE                                0x0
#define ECC_BCH                                 0x1

//****************************************************************************
//
// Definition of the R2W_FACTOR field in the CSD register.
//
//****************************************************************************
#define R2W_FACTOR_1X                           0x0
#define R2W_FACTOR_2X                           0x1
#define R2W_FACTOR_4X                           0x2
#define R2W_FACTOR_8X                           0x3
#define R2W_FACTOR_16X                          0x4
#define R2W_FACTOR_32X                          0x5
#define R2W_FACTOR_64X                          0x6
#define R2W_FACTOR_128X                         0x7

//****************************************************************************
//
// Definition of the FILE_FORMAT field in the CSD register (when
// FILE_FORMAT_GRP is 0).
//
//****************************************************************************
#define FILE_FORMAT_HARD_DRIVE                  0x0
#define FILE_FORMAT_FLOPPY                      0x1
#define FILE_FORMAT_UNIVERSAL                   0x2
#define FILE_FORMAT_UNKNOWN                     0x3

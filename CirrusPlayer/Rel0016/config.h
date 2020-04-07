//****************************************************************************
//
// CONFIG.H - Configuration values for the Internet audio player.
//
// Copyright (c) 2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************

//****************************************************************************
//
// The tool set being used to cross compile code to the ARM processor.  Only
// one of these can be uncommented at a time.
//
//****************************************************************************
//#define CROSSTOOLS gcc
//#define CROSSTOOLS sdt25
//#define CROSSTOOLS ads

//****************************************************************************
//
// The tool set being used to compile native code for the host computer.  Only
// one of these can be uncommented at a time.
//
//****************************************************************************
//#define NATIVETOOLS gcc
//#define NATIVETOOLS msvc

//****************************************************************************
//
// Defines for the hardware platform on which the player is to run.  Only one
// of these can be defined at a time.
//
//****************************************************************************
//#define BOARD EP7209_Eval_Board
//#define BOARD EP7212_Reference_Board_Rev_A
//#define BOARD EP7212_Reference_Board_Rev_B
//#define BOARD EP7309_Reference_Board_Rev_B
//#define BOARD EP7212_Reference_Board_Rev_C
//#define BOARD EP7309_Reference_Board_Rev_C
//#define BOARD EP73XX_Eval_Board
//#define BOARD EP7312_Eval_Board

//****************************************************************************
//
// Defines for the various codecs that are supported.  By commenting out a
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
// A define for whether or not we support the AudibleReady portable device
// requirements.
//
//****************************************************************************
#define SUPPORT_AUDIBLE

//****************************************************************************
//
// A define for whether or not we support SRS Labs' audio processing.
//
//****************************************************************************
//#define SUPPORT_SRS

//****************************************************************************
//
// If SUPPORT_SRS is defined, then the following defines determine the version
// of SRS's processing that is supported.  Only one of these can be defined
// or there will be compiler errors.  The defines are ordered from the most
// processor MIPS to the least...consult the SRS documentation for details of
// each of these algorithms.
//
//****************************************************************************
#ifdef SUPPORT_SRS
#define SUPPORT_WOW_FULL
//#define SUPPORT_WOW_MINIMAL
//#define SUPPORT_TRU_BASS
//#define SUPPORT_SRS_HEADPHONE
#endif

//****************************************************************************
//
// A define for whether or not we support QSound's audio processing.
//
//****************************************************************************
//#define SUPPORT_QSOUND

//****************************************************************************
//
// A define for whether or not we support Spatializer's audio processing.
//
//****************************************************************************
//#define SUPPORT_SPATIALIZER

//****************************************************************************
//
// A define for whether or not we support InterTrust's Rights/PD.
//
//****************************************************************************
//#define SUPPORT_RIGHTS_PD

//****************************************************************************
//
// Whether or not we include the spectrum analyzer.
//
//****************************************************************************
//#define SUPPORT_SPECTRUM_ANALYZER

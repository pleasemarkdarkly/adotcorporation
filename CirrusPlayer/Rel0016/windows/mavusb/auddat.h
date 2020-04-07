//****************************************************************************
//
// AUDDAT.H - Prototypes for the functions in auddat.cpp.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
//
//****************************************************************************

extern unsigned long GetAudibleMetaData(unsigned long ulDrive,
                                        unsigned char *pusFileName,
                                        char *pcData, unsigned long ulSize);
extern unsigned long SetAudibleMetaData(unsigned long ulDrive,
                                        unsigned short *pusFileName,
                                        char *pcData, unsigned long ulSize);
extern unsigned long RemoveAudibleMetaData(unsigned long ulDrive,
                                           unsigned short *pusFileName);

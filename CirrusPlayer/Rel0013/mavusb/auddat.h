//****************************************************************************
//
// AUDDAT.H - Prototypes for the functions in auddat.cpp.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
//
//****************************************************************************

extern unsigned long GetAudibleMetaData(char *pcFileName, char *pcData,
                                        unsigned long ulSize);
extern unsigned long SetAudibleMetaData(char *pcFileName, char *pcData,
                                        unsigned long ulSize);
extern unsigned long RemoveAudibleMetaData(char *pcFileName);

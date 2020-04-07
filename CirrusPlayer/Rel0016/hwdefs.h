//****************************************************************************
//
// HWDEFS.H - Memory and register definitions for the hardware.
//
// Copyright (c) 1998,1999,2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************

//****************************************************************************
//
// Determine the correct hardware definition to include based on the processor
// type.
//
//****************************************************************************
#ifdef PROCESSOR_EP72XX
#include "ep72xx.h"
#endif
#ifdef PROCESSOR_EP73XX
#include "ep73xx.h"
#endif

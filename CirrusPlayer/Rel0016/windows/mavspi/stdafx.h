//****************************************************************************
//
// STDAFX.H - Include file for precompiled headers.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
// Copyright (C) Microsoft Corporation, 1999 - 2001.  All rights reserved.
//
//****************************************************************************
#if !defined(AFX_STDAFX_H__d4685da0_ae3b_11d4_9a3c_00aa00b881c3__INCLUDED_)
#define AFX_STDAFX_H__d4685da0_ae3b_11d4_9a3c_00aa00b881c3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define STRICT

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0400
#endif

#define _ATL_APARTMENT_THREADED


#include <atlbase.h>

//
// You may derive a class from CComModule and use it if you want to override
// something, but do not change the name of _Module
//
extern CComModule _Module;

#include <atlcom.h>

#include "resource.h"
#include "mavspi.h"
#include "mavdefs.h"
#include "scserver.h"

#include "servprov.h"
#include "device.h"
#include "storage.h"
#include "storglob.h"
#include "enumdev.h"
#include "enumstor.h"

#include "mavcmd.h"
#include "mavinfo.h"
#include "auddat.h"
#include "../../player/usbven.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__d4685da0_ae3b_11d4_9a3c_00aa00b881c3__INCLUDED_)

/* this ALWAYS GENERATED file contains the proxy stub code */


/* File created by MIDL compiler version 5.01.0164 */
/* at Thu Mar 22 08:34:35 2001
 */
/* Compiler settings for D:\ep7209\player.new\windows\mavspi\mavspi.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 440
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "mavspi.h"

#define TYPE_FORMAT_STRING_SIZE   757                               
#define PROC_FORMAT_STRING_SIZE   2607                              

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;


/* Standard interface: __MIDL_itf_mavspi_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IWMDeviceManager, ver. 0.0,
   GUID={0x1DCB3A00,0x33ED,0x11d3,{0x84,0x70,0x00,0xC0,0x4F,0x79,0xDB,0xC0}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IWMDeviceManager_ServerInfo;

#pragma code_seg(".orpc")
static const unsigned short IWMDeviceManager_FormatStringOffsetTable[] = 
    {
    0,
    28,
    56
    };

static const MIDL_SERVER_INFO IWMDeviceManager_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IWMDeviceManager_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IWMDeviceManager_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IWMDeviceManager_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(6) _IWMDeviceManagerProxyVtbl = 
{
    &IWMDeviceManager_ProxyInfo,
    &IID_IWMDeviceManager,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *)-1 /* IWMDeviceManager::GetRevision */ ,
    (void *)-1 /* IWMDeviceManager::GetDeviceCount */ ,
    (void *)-1 /* IWMDeviceManager::EnumDevices */
};

const CInterfaceStubVtbl _IWMDeviceManagerStubVtbl =
{
    &IID_IWMDeviceManager,
    &IWMDeviceManager_ServerInfo,
    6,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IWMDMStorageGlobals, ver. 0.0,
   GUID={0x1DCB3A07,0x33ED,0x11d3,{0x84,0x70,0x00,0xC0,0x4F,0x79,0xDB,0xC0}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IWMDMStorageGlobals_ServerInfo;

#pragma code_seg(".orpc")
static const unsigned short IWMDMStorageGlobals_FormatStringOffsetTable[] = 
    {
    0,
    84,
    118,
    152,
    186,
    220,
    248
    };

static const MIDL_SERVER_INFO IWMDMStorageGlobals_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IWMDMStorageGlobals_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IWMDMStorageGlobals_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IWMDMStorageGlobals_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(10) _IWMDMStorageGlobalsProxyVtbl = 
{
    &IWMDMStorageGlobals_ProxyInfo,
    &IID_IWMDMStorageGlobals,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *)-1 /* IWMDMStorageGlobals::GetCapabilities */ ,
    (void *)-1 /* IWMDMStorageGlobals::GetSerialNumber */ ,
    (void *)-1 /* IWMDMStorageGlobals::GetTotalSize */ ,
    (void *)-1 /* IWMDMStorageGlobals::GetTotalFree */ ,
    (void *)-1 /* IWMDMStorageGlobals::GetTotalBad */ ,
    (void *)-1 /* IWMDMStorageGlobals::GetStatus */ ,
    (void *)-1 /* IWMDMStorageGlobals::Initialize */
};

const CInterfaceStubVtbl _IWMDMStorageGlobalsStubVtbl =
{
    &IID_IWMDMStorageGlobals,
    &IWMDMStorageGlobals_ServerInfo,
    10,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IWMDMStorage, ver. 0.0,
   GUID={0x1DCB3A06,0x33ED,0x11d3,{0x84,0x70,0x00,0xC0,0x4F,0x79,0xDB,0xC0}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IWMDMStorage_ServerInfo;

#pragma code_seg(".orpc")
static const unsigned short IWMDMStorage_FormatStringOffsetTable[] = 
    {
    282,
    316,
    344,
    378,
    412,
    440,
    474,
    514,
    542
    };

static const MIDL_SERVER_INFO IWMDMStorage_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IWMDMStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IWMDMStorage_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IWMDMStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(12) _IWMDMStorageProxyVtbl = 
{
    &IWMDMStorage_ProxyInfo,
    &IID_IWMDMStorage,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *)-1 /* IWMDMStorage::SetAttributes */ ,
    (void *)-1 /* IWMDMStorage::GetStorageGlobals */ ,
    (void *)-1 /* IWMDMStorage::GetAttributes */ ,
    (void *)-1 /* IWMDMStorage::GetName */ ,
    (void *)-1 /* IWMDMStorage::GetDate */ ,
    (void *)-1 /* IWMDMStorage::GetSize */ ,
    (void *)-1 /* IWMDMStorage::GetRights */ ,
    (void *)-1 /* IWMDMStorage::EnumStorage */ ,
    (void *)-1 /* IWMDMStorage::SendOpaqueCommand */
};

const CInterfaceStubVtbl _IWMDMStorageStubVtbl =
{
    &IID_IWMDMStorage,
    &IWMDMStorage_ServerInfo,
    12,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IWMDMOperation, ver. 0.0,
   GUID={0x1DCB3A0B,0x33ED,0x11d3,{0x84,0x70,0x00,0xC0,0x4F,0x79,0xDB,0xC0}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IWMDMOperation_ServerInfo;

#pragma code_seg(".orpc")
static const unsigned short IWMDMOperation_FormatStringOffsetTable[] = 
    {
    570,
    592,
    614,
    648,
    682,
    716,
    750,
    784,
    818,
    858
    };

static const MIDL_SERVER_INFO IWMDMOperation_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IWMDMOperation_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IWMDMOperation_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IWMDMOperation_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(13) _IWMDMOperationProxyVtbl = 
{
    &IWMDMOperation_ProxyInfo,
    &IID_IWMDMOperation,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *)-1 /* IWMDMOperation::BeginRead */ ,
    (void *)-1 /* IWMDMOperation::BeginWrite */ ,
    (void *)-1 /* IWMDMOperation::GetObjectName */ ,
    (void *)-1 /* IWMDMOperation::SetObjectName */ ,
    (void *)-1 /* IWMDMOperation::GetObjectAttributes */ ,
    (void *)-1 /* IWMDMOperation::SetObjectAttributes */ ,
    (void *)-1 /* IWMDMOperation::GetObjectTotalSize */ ,
    (void *)-1 /* IWMDMOperation::SetObjectTotalSize */ ,
    (void *)-1 /* IWMDMOperation::TransferObjectData */ ,
    (void *)-1 /* IWMDMOperation::End */
};

const CInterfaceStubVtbl _IWMDMOperationStubVtbl =
{
    &IID_IWMDMOperation,
    &IWMDMOperation_ServerInfo,
    13,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IWMDMProgress, ver. 0.0,
   GUID={0x1DCB3A0C,0x33ED,0x11d3,{0x84,0x70,0x00,0xC0,0x4F,0x79,0xDB,0xC0}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IWMDMProgress_ServerInfo;

#pragma code_seg(".orpc")
static const unsigned short IWMDMProgress_FormatStringOffsetTable[] = 
    {
    892,
    920,
    948
    };

static const MIDL_SERVER_INFO IWMDMProgress_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IWMDMProgress_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IWMDMProgress_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IWMDMProgress_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(6) _IWMDMProgressProxyVtbl = 
{
    &IWMDMProgress_ProxyInfo,
    &IID_IWMDMProgress,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *)-1 /* IWMDMProgress::Begin */ ,
    (void *)-1 /* IWMDMProgress::Progress */ ,
    (void *)-1 /* IWMDMProgress::End */
};

const CInterfaceStubVtbl _IWMDMProgressStubVtbl =
{
    &IID_IWMDMProgress,
    &IWMDMProgress_ServerInfo,
    6,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IWMDMDevice, ver. 0.0,
   GUID={0x1DCB3A02,0x33ED,0x11d3,{0x84,0x70,0x00,0xC0,0x4F,0x79,0xDB,0xC0}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IWMDMDevice_ServerInfo;

#pragma code_seg(".orpc")
static const unsigned short IWMDMDevice_FormatStringOffsetTable[] = 
    {
    970,
    1004,
    1038,
    1066,
    1094,
    440,
    1128,
    1156,
    1184,
    1212,
    1258
    };

static const MIDL_SERVER_INFO IWMDMDevice_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IWMDMDevice_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IWMDMDevice_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IWMDMDevice_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(14) _IWMDMDeviceProxyVtbl = 
{
    &IWMDMDevice_ProxyInfo,
    &IID_IWMDMDevice,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *)-1 /* IWMDMDevice::GetName */ ,
    (void *)-1 /* IWMDMDevice::GetManufacturer */ ,
    (void *)-1 /* IWMDMDevice::GetVersion */ ,
    (void *)-1 /* IWMDMDevice::GetType */ ,
    (void *)-1 /* IWMDMDevice::GetSerialNumber */ ,
    (void *)-1 /* IWMDMDevice::GetPowerSource */ ,
    (void *)-1 /* IWMDMDevice::GetStatus */ ,
    (void *)-1 /* IWMDMDevice::GetDeviceIcon */ ,
    (void *)-1 /* IWMDMDevice::EnumStorage */ ,
    (void *)-1 /* IWMDMDevice::GetFormatSupport */ ,
    (void *)-1 /* IWMDMDevice::SendOpaqueCommand */
};

const CInterfaceStubVtbl _IWMDMDeviceStubVtbl =
{
    &IID_IWMDMDevice,
    &IWMDMDevice_ServerInfo,
    14,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IWMDMEnumDevice, ver. 0.0,
   GUID={0x1DCB3A01,0x33ED,0x11d3,{0x84,0x70,0x00,0xC0,0x4F,0x79,0xDB,0xC0}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IWMDMEnumDevice_ServerInfo;

#pragma code_seg(".orpc")
static const unsigned short IWMDMEnumDevice_FormatStringOffsetTable[] = 
    {
    1286,
    1326,
    948,
    1360
    };

static const MIDL_SERVER_INFO IWMDMEnumDevice_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IWMDMEnumDevice_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IWMDMEnumDevice_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IWMDMEnumDevice_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(7) _IWMDMEnumDeviceProxyVtbl = 
{
    &IWMDMEnumDevice_ProxyInfo,
    &IID_IWMDMEnumDevice,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *)-1 /* IWMDMEnumDevice::Next */ ,
    (void *)-1 /* IWMDMEnumDevice::Skip */ ,
    (void *)-1 /* IWMDMEnumDevice::Reset */ ,
    (void *)-1 /* IWMDMEnumDevice::Clone */
};

const CInterfaceStubVtbl _IWMDMEnumDeviceStubVtbl =
{
    &IID_IWMDMEnumDevice,
    &IWMDMEnumDevice_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IWMDMDeviceControl, ver. 0.0,
   GUID={0x1DCB3A04,0x33ED,0x11d3,{0x84,0x70,0x00,0xC0,0x4F,0x79,0xDB,0xC0}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IWMDMDeviceControl_ServerInfo;

#pragma code_seg(".orpc")
static const unsigned short IWMDMDeviceControl_FormatStringOffsetTable[] = 
    {
    0,
    28,
    948,
    1388,
    1416,
    1438,
    1460,
    784
    };

static const MIDL_SERVER_INFO IWMDMDeviceControl_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IWMDMDeviceControl_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IWMDMDeviceControl_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IWMDMDeviceControl_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(11) _IWMDMDeviceControlProxyVtbl = 
{
    &IWMDMDeviceControl_ProxyInfo,
    &IID_IWMDMDeviceControl,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *)-1 /* IWMDMDeviceControl::GetStatus */ ,
    (void *)-1 /* IWMDMDeviceControl::GetCapabilities */ ,
    (void *)-1 /* IWMDMDeviceControl::Play */ ,
    (void *)-1 /* IWMDMDeviceControl::Record */ ,
    (void *)-1 /* IWMDMDeviceControl::Pause */ ,
    (void *)-1 /* IWMDMDeviceControl::Resume */ ,
    (void *)-1 /* IWMDMDeviceControl::Stop */ ,
    (void *)-1 /* IWMDMDeviceControl::Seek */
};

const CInterfaceStubVtbl _IWMDMDeviceControlStubVtbl =
{
    &IID_IWMDMDeviceControl,
    &IWMDMDeviceControl_ServerInfo,
    11,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IWMDMEnumStorage, ver. 0.0,
   GUID={0x1DCB3A05,0x33ED,0x11d3,{0x84,0x70,0x00,0xC0,0x4F,0x79,0xDB,0xC0}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IWMDMEnumStorage_ServerInfo;

#pragma code_seg(".orpc")
static const unsigned short IWMDMEnumStorage_FormatStringOffsetTable[] = 
    {
    1482,
    1326,
    948,
    1522
    };

static const MIDL_SERVER_INFO IWMDMEnumStorage_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IWMDMEnumStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IWMDMEnumStorage_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IWMDMEnumStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(7) _IWMDMEnumStorageProxyVtbl = 
{
    &IWMDMEnumStorage_ProxyInfo,
    &IID_IWMDMEnumStorage,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *)-1 /* IWMDMEnumStorage::Next */ ,
    (void *)-1 /* IWMDMEnumStorage::Skip */ ,
    (void *)-1 /* IWMDMEnumStorage::Reset */ ,
    (void *)-1 /* IWMDMEnumStorage::Clone */
};

const CInterfaceStubVtbl _IWMDMEnumStorageStubVtbl =
{
    &IID_IWMDMEnumStorage,
    &IWMDMEnumStorage_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IWMDMStorageControl, ver. 0.0,
   GUID={0x1DCB3A08,0x33ED,0x11d3,{0x84,0x70,0x00,0xC0,0x4F,0x79,0xDB,0xC0}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IWMDMStorageControl_ServerInfo;

#pragma code_seg(".orpc")
static const unsigned short IWMDMStorageControl_FormatStringOffsetTable[] = 
    {
    1550,
    1602,
    1636,
    1676,
    1722
    };

static const MIDL_SERVER_INFO IWMDMStorageControl_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IWMDMStorageControl_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IWMDMStorageControl_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IWMDMStorageControl_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(8) _IWMDMStorageControlProxyVtbl = 
{
    &IWMDMStorageControl_ProxyInfo,
    &IID_IWMDMStorageControl,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *)-1 /* IWMDMStorageControl::Insert */ ,
    (void *)-1 /* IWMDMStorageControl::Delete */ ,
    (void *)-1 /* IWMDMStorageControl::Rename */ ,
    (void *)-1 /* IWMDMStorageControl::Read */ ,
    (void *)-1 /* IWMDMStorageControl::Move */
};

const CInterfaceStubVtbl _IWMDMStorageControlStubVtbl =
{
    &IID_IWMDMStorageControl,
    &IWMDMStorageControl_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IWMDMObjectInfo, ver. 0.0,
   GUID={0x1DCB3A09,0x33ED,0x11d3,{0x84,0x70,0x00,0xC0,0x4F,0x79,0xDB,0xC0}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IWMDMObjectInfo_ServerInfo;

#pragma code_seg(".orpc")
static const unsigned short IWMDMObjectInfo_FormatStringOffsetTable[] = 
    {
    0,
    920,
    1038,
    1762,
    1790,
    220,
    1128
    };

static const MIDL_SERVER_INFO IWMDMObjectInfo_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IWMDMObjectInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IWMDMObjectInfo_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IWMDMObjectInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(10) _IWMDMObjectInfoProxyVtbl = 
{
    &IWMDMObjectInfo_ProxyInfo,
    &IID_IWMDMObjectInfo,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *)-1 /* IWMDMObjectInfo::GetPlayLength */ ,
    (void *)-1 /* IWMDMObjectInfo::SetPlayLength */ ,
    (void *)-1 /* IWMDMObjectInfo::GetPlayOffset */ ,
    (void *)-1 /* IWMDMObjectInfo::SetPlayOffset */ ,
    (void *)-1 /* IWMDMObjectInfo::GetTotalLength */ ,
    (void *)-1 /* IWMDMObjectInfo::GetLastPlayPosition */ ,
    (void *)-1 /* IWMDMObjectInfo::GetLongestPlayPosition */
};

const CInterfaceStubVtbl _IWMDMObjectInfoStubVtbl =
{
    &IID_IWMDMObjectInfo,
    &IWMDMObjectInfo_ServerInfo,
    10,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_mavspi_0222, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IMDServiceProvider, ver. 0.0,
   GUID={0x1DCB3A10,0x33ED,0x11d3,{0x84,0x70,0x00,0xC0,0x4F,0x79,0xDB,0xC0}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IMDServiceProvider_ServerInfo;

#pragma code_seg(".orpc")
static const unsigned short IMDServiceProvider_FormatStringOffsetTable[] = 
    {
    0,
    1818
    };

static const MIDL_SERVER_INFO IMDServiceProvider_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IMDServiceProvider_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IMDServiceProvider_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IMDServiceProvider_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(5) _IMDServiceProviderProxyVtbl = 
{
    &IMDServiceProvider_ProxyInfo,
    &IID_IMDServiceProvider,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *)-1 /* IMDServiceProvider::GetDeviceCount */ ,
    (void *)-1 /* IMDServiceProvider::EnumDevices */
};

const CInterfaceStubVtbl _IMDServiceProviderStubVtbl =
{
    &IID_IMDServiceProvider,
    &IMDServiceProvider_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IMDSPEnumDevice, ver. 0.0,
   GUID={0x1DCB3A11,0x33ED,0x11d3,{0x84,0x70,0x00,0xC0,0x4F,0x79,0xDB,0xC0}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IMDSPEnumDevice_ServerInfo;

#pragma code_seg(".orpc")
static const unsigned short IMDSPEnumDevice_FormatStringOffsetTable[] = 
    {
    1846,
    1326,
    948,
    1886
    };

static const MIDL_SERVER_INFO IMDSPEnumDevice_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IMDSPEnumDevice_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IMDSPEnumDevice_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IMDSPEnumDevice_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(7) _IMDSPEnumDeviceProxyVtbl = 
{
    &IMDSPEnumDevice_ProxyInfo,
    &IID_IMDSPEnumDevice,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *)-1 /* IMDSPEnumDevice::Next */ ,
    (void *)-1 /* IMDSPEnumDevice::Skip */ ,
    (void *)-1 /* IMDSPEnumDevice::Reset */ ,
    (void *)-1 /* IMDSPEnumDevice::Clone */
};

const CInterfaceStubVtbl _IMDSPEnumDeviceStubVtbl =
{
    &IID_IMDSPEnumDevice,
    &IMDSPEnumDevice_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IMDSPDevice, ver. 0.0,
   GUID={0x1DCB3A12,0x33ED,0x11d3,{0x84,0x70,0x00,0xC0,0x4F,0x79,0xDB,0xC0}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IMDSPDevice_ServerInfo;

#pragma code_seg(".orpc")
static const unsigned short IMDSPDevice_FormatStringOffsetTable[] = 
    {
    1914,
    1948,
    1038,
    1066,
    1094,
    440,
    1128,
    1156,
    1982,
    1212,
    1258
    };

static const MIDL_SERVER_INFO IMDSPDevice_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IMDSPDevice_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IMDSPDevice_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IMDSPDevice_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(14) _IMDSPDeviceProxyVtbl = 
{
    &IMDSPDevice_ProxyInfo,
    &IID_IMDSPDevice,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *)-1 /* IMDSPDevice::GetName */ ,
    (void *)-1 /* IMDSPDevice::GetManufacturer */ ,
    (void *)-1 /* IMDSPDevice::GetVersion */ ,
    (void *)-1 /* IMDSPDevice::GetType */ ,
    (void *)-1 /* IMDSPDevice::GetSerialNumber */ ,
    (void *)-1 /* IMDSPDevice::GetPowerSource */ ,
    (void *)-1 /* IMDSPDevice::GetStatus */ ,
    (void *)-1 /* IMDSPDevice::GetDeviceIcon */ ,
    (void *)-1 /* IMDSPDevice::EnumStorage */ ,
    (void *)-1 /* IMDSPDevice::GetFormatSupport */ ,
    (void *)-1 /* IMDSPDevice::SendOpaqueCommand */
};

const CInterfaceStubVtbl _IMDSPDeviceStubVtbl =
{
    &IID_IMDSPDevice,
    &IMDSPDevice_ServerInfo,
    14,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IMDSPDeviceControl, ver. 0.0,
   GUID={0x1DCB3A14,0x33ED,0x11d3,{0x84,0x70,0x00,0xC0,0x4F,0x79,0xDB,0xC0}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IMDSPDeviceControl_ServerInfo;

#pragma code_seg(".orpc")
static const unsigned short IMDSPDeviceControl_FormatStringOffsetTable[] = 
    {
    0,
    28,
    948,
    1388,
    1416,
    1438,
    1460,
    784
    };

static const MIDL_SERVER_INFO IMDSPDeviceControl_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IMDSPDeviceControl_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IMDSPDeviceControl_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IMDSPDeviceControl_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(11) _IMDSPDeviceControlProxyVtbl = 
{
    &IMDSPDeviceControl_ProxyInfo,
    &IID_IMDSPDeviceControl,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *)-1 /* IMDSPDeviceControl::GetDCStatus */ ,
    (void *)-1 /* IMDSPDeviceControl::GetCapabilities */ ,
    (void *)-1 /* IMDSPDeviceControl::Play */ ,
    (void *)-1 /* IMDSPDeviceControl::Record */ ,
    (void *)-1 /* IMDSPDeviceControl::Pause */ ,
    (void *)-1 /* IMDSPDeviceControl::Resume */ ,
    (void *)-1 /* IMDSPDeviceControl::Stop */ ,
    (void *)-1 /* IMDSPDeviceControl::Seek */
};

const CInterfaceStubVtbl _IMDSPDeviceControlStubVtbl =
{
    &IID_IMDSPDeviceControl,
    &IMDSPDeviceControl_ServerInfo,
    11,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IMDSPEnumStorage, ver. 0.0,
   GUID={0x1DCB3A15,0x33ED,0x11d3,{0x84,0x70,0x00,0xC0,0x4F,0x79,0xDB,0xC0}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IMDSPEnumStorage_ServerInfo;

#pragma code_seg(".orpc")
static const unsigned short IMDSPEnumStorage_FormatStringOffsetTable[] = 
    {
    2010,
    1326,
    948,
    2050
    };

static const MIDL_SERVER_INFO IMDSPEnumStorage_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IMDSPEnumStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IMDSPEnumStorage_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IMDSPEnumStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(7) _IMDSPEnumStorageProxyVtbl = 
{
    &IMDSPEnumStorage_ProxyInfo,
    &IID_IMDSPEnumStorage,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *)-1 /* IMDSPEnumStorage::Next */ ,
    (void *)-1 /* IMDSPEnumStorage::Skip */ ,
    (void *)-1 /* IMDSPEnumStorage::Reset */ ,
    (void *)-1 /* IMDSPEnumStorage::Clone */
};

const CInterfaceStubVtbl _IMDSPEnumStorageStubVtbl =
{
    &IID_IMDSPEnumStorage,
    &IMDSPEnumStorage_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IMDSPStorage, ver. 0.0,
   GUID={0x1DCB3A16,0x33ED,0x11d3,{0x84,0x70,0x00,0xC0,0x4F,0x79,0xDB,0xC0}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IMDSPStorage_ServerInfo;

#pragma code_seg(".orpc")
static const unsigned short IMDSPStorage_FormatStringOffsetTable[] = 
    {
    282,
    2078,
    344,
    2106,
    412,
    440,
    474,
    2140,
    1982,
    2186
    };

static const MIDL_SERVER_INFO IMDSPStorage_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IMDSPStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IMDSPStorage_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IMDSPStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(13) _IMDSPStorageProxyVtbl = 
{
    &IMDSPStorage_ProxyInfo,
    &IID_IMDSPStorage,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *)-1 /* IMDSPStorage::SetAttributes */ ,
    (void *)-1 /* IMDSPStorage::GetStorageGlobals */ ,
    (void *)-1 /* IMDSPStorage::GetAttributes */ ,
    (void *)-1 /* IMDSPStorage::GetName */ ,
    (void *)-1 /* IMDSPStorage::GetDate */ ,
    (void *)-1 /* IMDSPStorage::GetSize */ ,
    (void *)-1 /* IMDSPStorage::GetRights */ ,
    (void *)-1 /* IMDSPStorage::CreateStorage */ ,
    (void *)-1 /* IMDSPStorage::EnumStorage */ ,
    (void *)-1 /* IMDSPStorage::SendOpaqueCommand */
};

const CInterfaceStubVtbl _IMDSPStorageStubVtbl =
{
    &IID_IMDSPStorage,
    &IMDSPStorage_ServerInfo,
    13,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IMDSPStorageGlobals, ver. 0.0,
   GUID={0x1DCB3A17,0x33ED,0x11d3,{0x84,0x70,0x00,0xC0,0x4F,0x79,0xDB,0xC0}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IMDSPStorageGlobals_ServerInfo;

#pragma code_seg(".orpc")
static const unsigned short IMDSPStorageGlobals_FormatStringOffsetTable[] = 
    {
    0,
    84,
    118,
    152,
    186,
    220,
    248,
    2214,
    2242
    };

static const MIDL_SERVER_INFO IMDSPStorageGlobals_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IMDSPStorageGlobals_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IMDSPStorageGlobals_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IMDSPStorageGlobals_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(12) _IMDSPStorageGlobalsProxyVtbl = 
{
    &IMDSPStorageGlobals_ProxyInfo,
    &IID_IMDSPStorageGlobals,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *)-1 /* IMDSPStorageGlobals::GetCapabilities */ ,
    (void *)-1 /* IMDSPStorageGlobals::GetSerialNumber */ ,
    (void *)-1 /* IMDSPStorageGlobals::GetTotalSize */ ,
    (void *)-1 /* IMDSPStorageGlobals::GetTotalFree */ ,
    (void *)-1 /* IMDSPStorageGlobals::GetTotalBad */ ,
    (void *)-1 /* IMDSPStorageGlobals::GetStatus */ ,
    (void *)-1 /* IMDSPStorageGlobals::Initialize */ ,
    (void *)-1 /* IMDSPStorageGlobals::GetDevice */ ,
    (void *)-1 /* IMDSPStorageGlobals::GetRootStorage */
};

const CInterfaceStubVtbl _IMDSPStorageGlobalsStubVtbl =
{
    &IID_IMDSPStorageGlobals,
    &IMDSPStorageGlobals_ServerInfo,
    12,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IMDSPObjectInfo, ver. 0.0,
   GUID={0x1DCB3A19,0x33ED,0x11d3,{0x84,0x70,0x00,0xC0,0x4F,0x79,0xDB,0xC0}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IMDSPObjectInfo_ServerInfo;

#pragma code_seg(".orpc")
static const unsigned short IMDSPObjectInfo_FormatStringOffsetTable[] = 
    {
    0,
    920,
    1038,
    1762,
    1790,
    220,
    1128
    };

static const MIDL_SERVER_INFO IMDSPObjectInfo_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IMDSPObjectInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IMDSPObjectInfo_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IMDSPObjectInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(10) _IMDSPObjectInfoProxyVtbl = 
{
    &IMDSPObjectInfo_ProxyInfo,
    &IID_IMDSPObjectInfo,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *)-1 /* IMDSPObjectInfo::GetPlayLength */ ,
    (void *)-1 /* IMDSPObjectInfo::SetPlayLength */ ,
    (void *)-1 /* IMDSPObjectInfo::GetPlayOffset */ ,
    (void *)-1 /* IMDSPObjectInfo::SetPlayOffset */ ,
    (void *)-1 /* IMDSPObjectInfo::GetTotalLength */ ,
    (void *)-1 /* IMDSPObjectInfo::GetLastPlayPosition */ ,
    (void *)-1 /* IMDSPObjectInfo::GetLongestPlayPosition */
};

const CInterfaceStubVtbl _IMDSPObjectInfoStubVtbl =
{
    &IID_IMDSPObjectInfo,
    &IMDSPObjectInfo_ServerInfo,
    10,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IMDSPObject, ver. 0.0,
   GUID={0x1DCB3A18,0x33ED,0x11d3,{0x84,0x70,0x00,0xC0,0x4F,0x79,0xDB,0xC0}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IMDSPObject_ServerInfo;

#pragma code_seg(".orpc")
static const unsigned short IMDSPObject_FormatStringOffsetTable[] = 
    {
    892,
    2270,
    2310,
    2350,
    2384,
    2418,
    2452,
    2492
    };

static const MIDL_SERVER_INFO IMDSPObject_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IMDSPObject_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IMDSPObject_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IMDSPObject_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(11) _IMDSPObjectProxyVtbl = 
{
    &IMDSPObject_ProxyInfo,
    &IID_IMDSPObject,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *)-1 /* IMDSPObject::Open */ ,
    (void *)-1 /* IMDSPObject::Read */ ,
    (void *)-1 /* IMDSPObject::Write */ ,
    (void *)-1 /* IMDSPObject::Delete */ ,
    (void *)-1 /* IMDSPObject::Seek */ ,
    (void *)-1 /* IMDSPObject::Rename */ ,
    (void *)-1 /* IMDSPObject::Move */ ,
    (void *)-1 /* IMDSPObject::Close */
};

const CInterfaceStubVtbl _IMDSPObjectStubVtbl =
{
    &IID_IMDSPObject,
    &IMDSPObject_ServerInfo,
    11,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_mavspi_0231, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IComponentAuthenticate, ver. 0.0,
   GUID={0xA9889C00,0x6D2B,0x11d3,{0x84,0x96,0x00,0xC0,0x4F,0x79,0xDB,0xC0}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IComponentAuthenticate_ServerInfo;

#pragma code_seg(".orpc")

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x20000, /* Ndr library version */
    0,
    0x50100a4, /* MIDL Version 5.1.164 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    1,  /* Flags */
    0,  /* Reserved3 */
    0,  /* Reserved4 */
    0   /* Reserved5 */
    };

static const unsigned short IComponentAuthenticate_FormatStringOffsetTable[] = 
    {
    2514,
    2572
    };

static const MIDL_SERVER_INFO IComponentAuthenticate_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IComponentAuthenticate_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IComponentAuthenticate_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IComponentAuthenticate_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(5) _IComponentAuthenticateProxyVtbl = 
{
    &IComponentAuthenticate_ProxyInfo,
    &IID_IComponentAuthenticate,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *)-1 /* IComponentAuthenticate::SACAuth */ ,
    (void *)-1 /* IComponentAuthenticate::SACGetProtocols */
};

const CInterfaceStubVtbl _IComponentAuthenticateStubVtbl =
{
    &IID_IComponentAuthenticate,
    &IComponentAuthenticate_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};

#pragma data_seg(".rdata")

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT40_OR_LATER)
#error You need a Windows NT 4.0 or later to run this stub because it uses these features:
#error   -Oif or -Oicf, more than 32 methods in the interface.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will die there with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure GetPlayLength */


	/* Procedure GetCapabilities */


	/* Procedure GetDCStatus */


	/* Procedure GetDeviceCount */


	/* Procedure GetPlayLength */


	/* Procedure GetStatus */


	/* Procedure GetCapabilities */


	/* Procedure GetRevision */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
#ifndef _ALPHA_
/*  8 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x10 ),	/* 16 */
/* 14 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pdwLength */


	/* Parameter pdwCapabilities */


	/* Parameter pdwStatus */


	/* Parameter pdwCount */


	/* Parameter pdwLength */


	/* Parameter pdwStatus */


	/* Parameter pdwCapabilities */


	/* Parameter pdwRevision */

/* 16 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 18 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 20 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 22 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 24 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 26 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCapabilities */


	/* Procedure GetCapabilities */


	/* Procedure GetDeviceCount */

/* 28 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 30 */	NdrFcLong( 0x0 ),	/* 0 */
/* 34 */	NdrFcShort( 0x4 ),	/* 4 */
#ifndef _ALPHA_
/* 36 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 38 */	NdrFcShort( 0x0 ),	/* 0 */
/* 40 */	NdrFcShort( 0x10 ),	/* 16 */
/* 42 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pdwCapabilitiesMask */


	/* Parameter pdwCapabilitiesMask */


	/* Parameter pdwCount */

/* 44 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 46 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 48 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 50 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 52 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 54 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumDevices */

/* 56 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 58 */	NdrFcLong( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x5 ),	/* 5 */
#ifndef _ALPHA_
/* 64 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 66 */	NdrFcShort( 0x0 ),	/* 0 */
/* 68 */	NdrFcShort( 0x8 ),	/* 8 */
/* 70 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnumDevice */

/* 72 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
#ifndef _ALPHA_
/* 74 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 76 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 78 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 80 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 82 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSerialNumber */


	/* Procedure GetSerialNumber */

/* 84 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 86 */	NdrFcLong( 0x0 ),	/* 0 */
/* 90 */	NdrFcShort( 0x4 ),	/* 4 */
#ifndef _ALPHA_
/* 92 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 94 */	NdrFcShort( 0x10 ),	/* 16 */
/* 96 */	NdrFcShort( 0xb4 ),	/* 180 */
/* 98 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter pSerialNum */


	/* Parameter pSerialNum */

/* 100 */	NdrFcShort( 0x112 ),	/* Flags:  must free, out, simple ref, */
#ifndef _ALPHA_
/* 102 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 104 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Parameter abMac */


	/* Parameter abMac */

/* 106 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
#ifndef _ALPHA_
/* 108 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 110 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Return value */


	/* Return value */

/* 112 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 114 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 116 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTotalSize */


	/* Procedure GetTotalSize */

/* 118 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 120 */	NdrFcLong( 0x0 ),	/* 0 */
/* 124 */	NdrFcShort( 0x5 ),	/* 5 */
#ifndef _ALPHA_
/* 126 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 128 */	NdrFcShort( 0x0 ),	/* 0 */
/* 130 */	NdrFcShort( 0x18 ),	/* 24 */
/* 132 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter pdwTotalSizeLow */


	/* Parameter pdwTotalSizeLow */

/* 134 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 136 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 138 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pdwTotalSizeHigh */


	/* Parameter pdwTotalSizeHigh */

/* 140 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 142 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 144 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 146 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 148 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 150 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTotalFree */


	/* Procedure GetTotalFree */

/* 152 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 154 */	NdrFcLong( 0x0 ),	/* 0 */
/* 158 */	NdrFcShort( 0x6 ),	/* 6 */
#ifndef _ALPHA_
/* 160 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 162 */	NdrFcShort( 0x0 ),	/* 0 */
/* 164 */	NdrFcShort( 0x18 ),	/* 24 */
/* 166 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter pdwFreeLow */


	/* Parameter pdwFreeLow */

/* 168 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 170 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 172 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pdwFreeHigh */


	/* Parameter pdwFreeHigh */

/* 174 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 176 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 178 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 180 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 182 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 184 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTotalBad */


	/* Procedure GetTotalBad */

/* 186 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 188 */	NdrFcLong( 0x0 ),	/* 0 */
/* 192 */	NdrFcShort( 0x7 ),	/* 7 */
#ifndef _ALPHA_
/* 194 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 196 */	NdrFcShort( 0x0 ),	/* 0 */
/* 198 */	NdrFcShort( 0x18 ),	/* 24 */
/* 200 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter pdwBadLow */


	/* Parameter pdwBadLow */

/* 202 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 204 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 206 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pdwBadHigh */


	/* Parameter pdwBadHigh */

/* 208 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 210 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 212 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 214 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 216 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 218 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetLastPlayPosition */


	/* Procedure GetStatus */


	/* Procedure GetLastPlayPosition */


	/* Procedure GetStatus */

/* 220 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 222 */	NdrFcLong( 0x0 ),	/* 0 */
/* 226 */	NdrFcShort( 0x8 ),	/* 8 */
#ifndef _ALPHA_
/* 228 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 230 */	NdrFcShort( 0x0 ),	/* 0 */
/* 232 */	NdrFcShort( 0x10 ),	/* 16 */
/* 234 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pdwLastPos */


	/* Parameter pdwStatus */


	/* Parameter pdwLastPos */


	/* Parameter pdwStatus */

/* 236 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 238 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 240 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 242 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 244 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 246 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Initialize */


	/* Procedure Initialize */

/* 248 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 250 */	NdrFcLong( 0x0 ),	/* 0 */
/* 254 */	NdrFcShort( 0x9 ),	/* 9 */
#ifndef _ALPHA_
/* 256 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 258 */	NdrFcShort( 0x8 ),	/* 8 */
/* 260 */	NdrFcShort( 0x8 ),	/* 8 */
/* 262 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter fuMode */


	/* Parameter fuMode */

/* 264 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 266 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 268 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pProgress */


	/* Parameter pProgress */

/* 270 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
#ifndef _ALPHA_
/* 272 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 274 */	NdrFcShort( 0x38 ),	/* Type Offset=56 */

	/* Return value */


	/* Return value */

/* 276 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 278 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 280 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetAttributes */


	/* Procedure SetAttributes */

/* 282 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 284 */	NdrFcLong( 0x0 ),	/* 0 */
/* 288 */	NdrFcShort( 0x3 ),	/* 3 */
#ifndef _ALPHA_
/* 290 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 292 */	NdrFcShort( 0x8 ),	/* 8 */
/* 294 */	NdrFcShort( 0x8 ),	/* 8 */
/* 296 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter dwAttributes */


	/* Parameter dwAttributes */

/* 298 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 300 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 302 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pFormat */


	/* Parameter pFormat */

/* 304 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
#ifndef _ALPHA_
/* 306 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 308 */	NdrFcShort( 0x4e ),	/* Type Offset=78 */

	/* Return value */


	/* Return value */

/* 310 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 312 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 314 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetStorageGlobals */

/* 316 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 318 */	NdrFcLong( 0x0 ),	/* 0 */
/* 322 */	NdrFcShort( 0x4 ),	/* 4 */
#ifndef _ALPHA_
/* 324 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 326 */	NdrFcShort( 0x0 ),	/* 0 */
/* 328 */	NdrFcShort( 0x8 ),	/* 8 */
/* 330 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppStorageGlobals */

/* 332 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
#ifndef _ALPHA_
/* 334 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 336 */	NdrFcShort( 0x60 ),	/* Type Offset=96 */

	/* Return value */

/* 338 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 340 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 342 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAttributes */


	/* Procedure GetAttributes */

/* 344 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 346 */	NdrFcLong( 0x0 ),	/* 0 */
/* 350 */	NdrFcShort( 0x5 ),	/* 5 */
#ifndef _ALPHA_
/* 352 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 354 */	NdrFcShort( 0x0 ),	/* 0 */
/* 356 */	NdrFcShort( 0x10 ),	/* 16 */
/* 358 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter pdwAttributes */


	/* Parameter pdwAttributes */

/* 360 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 362 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 364 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pFormat */


	/* Parameter pFormat */

/* 366 */	NdrFcShort( 0x6113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=24 */
#ifndef _ALPHA_
/* 368 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 370 */	NdrFcShort( 0x4e ),	/* Type Offset=78 */

	/* Return value */


	/* Return value */

/* 372 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 374 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 376 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetName */

/* 378 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 380 */	NdrFcLong( 0x0 ),	/* 0 */
/* 384 */	NdrFcShort( 0x6 ),	/* 6 */
#ifndef _ALPHA_
/* 386 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 388 */	NdrFcShort( 0x8 ),	/* 8 */
/* 390 */	NdrFcShort( 0x8 ),	/* 8 */
/* 392 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter pwszName */

/* 394 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
#ifndef _ALPHA_
/* 396 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 398 */	NdrFcShort( 0x7e ),	/* Type Offset=126 */

	/* Parameter nMaxChars */

/* 400 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 402 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 404 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 406 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 408 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 410 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDate */


	/* Procedure GetDate */

/* 412 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 414 */	NdrFcLong( 0x0 ),	/* 0 */
/* 418 */	NdrFcShort( 0x7 ),	/* 7 */
#ifndef _ALPHA_
/* 420 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 422 */	NdrFcShort( 0x0 ),	/* 0 */
/* 424 */	NdrFcShort( 0x1c ),	/* 28 */
/* 426 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pDateTimeUTC */


	/* Parameter pDateTimeUTC */

/* 428 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
#ifndef _ALPHA_
/* 430 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 432 */	NdrFcShort( 0x88 ),	/* Type Offset=136 */

	/* Return value */


	/* Return value */

/* 434 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 436 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 438 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSize */


	/* Procedure GetPowerSource */


	/* Procedure GetPowerSource */


	/* Procedure GetSize */

/* 440 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 442 */	NdrFcLong( 0x0 ),	/* 0 */
/* 446 */	NdrFcShort( 0x8 ),	/* 8 */
#ifndef _ALPHA_
/* 448 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 450 */	NdrFcShort( 0x0 ),	/* 0 */
/* 452 */	NdrFcShort( 0x18 ),	/* 24 */
/* 454 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter pdwSizeLow */


	/* Parameter pdwPowerSource */


	/* Parameter pdwPowerSource */


	/* Parameter pdwSizeLow */

/* 456 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 458 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 460 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pdwSizeHigh */


	/* Parameter pdwPercentRemaining */


	/* Parameter pdwPercentRemaining */


	/* Parameter pdwSizeHigh */

/* 462 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 464 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 466 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 468 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 470 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 472 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetRights */


	/* Procedure GetRights */

/* 474 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 476 */	NdrFcLong( 0x0 ),	/* 0 */
/* 480 */	NdrFcShort( 0x9 ),	/* 9 */
#ifndef _ALPHA_
/* 482 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 484 */	NdrFcShort( 0x10 ),	/* 16 */
/* 486 */	NdrFcShort( 0x20 ),	/* 32 */
/* 488 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter ppRights */


	/* Parameter ppRights */

/* 490 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
#ifndef _ALPHA_
/* 492 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 494 */	NdrFcShort( 0x94 ),	/* Type Offset=148 */

	/* Parameter pnRightsCount */


	/* Parameter pnRightsCount */

/* 496 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 498 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 500 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter abMac */


	/* Parameter abMac */

/* 502 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
#ifndef _ALPHA_
/* 504 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 506 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Return value */


	/* Return value */

/* 508 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 510 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 512 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumStorage */

/* 514 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 516 */	NdrFcLong( 0x0 ),	/* 0 */
/* 520 */	NdrFcShort( 0xa ),	/* 10 */
#ifndef _ALPHA_
/* 522 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 524 */	NdrFcShort( 0x0 ),	/* 0 */
/* 526 */	NdrFcShort( 0x8 ),	/* 8 */
/* 528 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pEnumStorage */

/* 530 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
#ifndef _ALPHA_
/* 532 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 534 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Return value */

/* 536 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 538 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 540 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SendOpaqueCommand */

/* 542 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 544 */	NdrFcLong( 0x0 ),	/* 0 */
/* 548 */	NdrFcShort( 0xb ),	/* 11 */
#ifndef _ALPHA_
/* 550 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 552 */	NdrFcShort( 0x0 ),	/* 0 */
/* 554 */	NdrFcShort( 0x8 ),	/* 8 */
/* 556 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pCommand */

/* 558 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
#ifndef _ALPHA_
/* 560 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 562 */	NdrFcShort( 0xf0 ),	/* Type Offset=240 */

	/* Return value */

/* 564 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 566 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 568 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure BeginRead */

/* 570 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 572 */	NdrFcLong( 0x0 ),	/* 0 */
/* 576 */	NdrFcShort( 0x3 ),	/* 3 */
#ifndef _ALPHA_
/* 578 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 580 */	NdrFcShort( 0x0 ),	/* 0 */
/* 582 */	NdrFcShort( 0x8 ),	/* 8 */
/* 584 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 586 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 588 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 590 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure BeginWrite */

/* 592 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 594 */	NdrFcLong( 0x0 ),	/* 0 */
/* 598 */	NdrFcShort( 0x4 ),	/* 4 */
#ifndef _ALPHA_
/* 600 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 602 */	NdrFcShort( 0x0 ),	/* 0 */
/* 604 */	NdrFcShort( 0x8 ),	/* 8 */
/* 606 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 608 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 610 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 612 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetObjectName */

/* 614 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 616 */	NdrFcLong( 0x0 ),	/* 0 */
/* 620 */	NdrFcShort( 0x5 ),	/* 5 */
#ifndef _ALPHA_
/* 622 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 624 */	NdrFcShort( 0x8 ),	/* 8 */
/* 626 */	NdrFcShort( 0x8 ),	/* 8 */
/* 628 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter pwszName */

/* 630 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
#ifndef _ALPHA_
/* 632 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 634 */	NdrFcShort( 0x110 ),	/* Type Offset=272 */

	/* Parameter nMaxChars */

/* 636 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 638 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 640 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 642 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 644 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 646 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetObjectName */

/* 648 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 650 */	NdrFcLong( 0x0 ),	/* 0 */
/* 654 */	NdrFcShort( 0x6 ),	/* 6 */
#ifndef _ALPHA_
/* 656 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 658 */	NdrFcShort( 0x8 ),	/* 8 */
/* 660 */	NdrFcShort( 0x8 ),	/* 8 */
/* 662 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pwszName */

/* 664 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
#ifndef _ALPHA_
/* 666 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 668 */	NdrFcShort( 0x11a ),	/* Type Offset=282 */

	/* Parameter nMaxChars */

/* 670 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 672 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 674 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 676 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 678 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 680 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetObjectAttributes */

/* 682 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 684 */	NdrFcLong( 0x0 ),	/* 0 */
/* 688 */	NdrFcShort( 0x7 ),	/* 7 */
#ifndef _ALPHA_
/* 690 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 692 */	NdrFcShort( 0x0 ),	/* 0 */
/* 694 */	NdrFcShort( 0x10 ),	/* 16 */
/* 696 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter pdwAttributes */

/* 698 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 700 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 702 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pFormat */

/* 704 */	NdrFcShort( 0x6113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=24 */
#ifndef _ALPHA_
/* 706 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 708 */	NdrFcShort( 0x4e ),	/* Type Offset=78 */

	/* Return value */

/* 710 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 712 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 714 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetObjectAttributes */

/* 716 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 718 */	NdrFcLong( 0x0 ),	/* 0 */
/* 722 */	NdrFcShort( 0x8 ),	/* 8 */
#ifndef _ALPHA_
/* 724 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 726 */	NdrFcShort( 0x8 ),	/* 8 */
/* 728 */	NdrFcShort( 0x8 ),	/* 8 */
/* 730 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter dwAttributes */

/* 732 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 734 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 736 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pFormat */

/* 738 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
#ifndef _ALPHA_
/* 740 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 742 */	NdrFcShort( 0x4e ),	/* Type Offset=78 */

	/* Return value */

/* 744 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 746 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 748 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetObjectTotalSize */

/* 750 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 752 */	NdrFcLong( 0x0 ),	/* 0 */
/* 756 */	NdrFcShort( 0x9 ),	/* 9 */
#ifndef _ALPHA_
/* 758 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 760 */	NdrFcShort( 0x0 ),	/* 0 */
/* 762 */	NdrFcShort( 0x18 ),	/* 24 */
/* 764 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter pdwSize */

/* 766 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 768 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 770 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pdwSizeHigh */

/* 772 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 774 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 776 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 778 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 780 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 782 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Seek */


	/* Procedure Seek */


	/* Procedure SetObjectTotalSize */

/* 784 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 786 */	NdrFcLong( 0x0 ),	/* 0 */
/* 790 */	NdrFcShort( 0xa ),	/* 10 */
#ifndef _ALPHA_
/* 792 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 794 */	NdrFcShort( 0x10 ),	/* 16 */
/* 796 */	NdrFcShort( 0x8 ),	/* 8 */
/* 798 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter fuMode */


	/* Parameter fuMode */


	/* Parameter dwSize */

/* 800 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 802 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 804 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nOffset */


	/* Parameter nOffset */


	/* Parameter dwSizeHigh */

/* 806 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 808 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 810 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 812 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 814 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 816 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure TransferObjectData */

/* 818 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 820 */	NdrFcLong( 0x0 ),	/* 0 */
/* 824 */	NdrFcShort( 0xb ),	/* 11 */
#ifndef _ALPHA_
/* 826 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 828 */	NdrFcShort( 0x18 ),	/* 24 */
/* 830 */	NdrFcShort( 0x20 ),	/* 32 */
/* 832 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter pData */

/* 834 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
#ifndef _ALPHA_
/* 836 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 838 */	NdrFcShort( 0x124 ),	/* Type Offset=292 */

	/* Parameter pdwSize */

/* 840 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
#ifndef _ALPHA_
/* 842 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 844 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter abMac */

/* 846 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
#ifndef _ALPHA_
/* 848 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 850 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Return value */

/* 852 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 854 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 856 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure End */

/* 858 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 860 */	NdrFcLong( 0x0 ),	/* 0 */
/* 864 */	NdrFcShort( 0xc ),	/* 12 */
#ifndef _ALPHA_
/* 866 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 868 */	NdrFcShort( 0x8 ),	/* 8 */
/* 870 */	NdrFcShort( 0x8 ),	/* 8 */
/* 872 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter phCompletionCode */

/* 874 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
#ifndef _ALPHA_
/* 876 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 878 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pNewObject */

/* 880 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
#ifndef _ALPHA_
/* 882 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 884 */	NdrFcShort( 0x132 ),	/* Type Offset=306 */

	/* Return value */

/* 886 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 888 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 890 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Open */


	/* Procedure Begin */

/* 892 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 894 */	NdrFcLong( 0x0 ),	/* 0 */
/* 898 */	NdrFcShort( 0x3 ),	/* 3 */
#ifndef _ALPHA_
/* 900 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 902 */	NdrFcShort( 0x8 ),	/* 8 */
/* 904 */	NdrFcShort( 0x8 ),	/* 8 */
/* 906 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter fuMode */


	/* Parameter dwEstimatedTicks */

/* 908 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 910 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 912 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 914 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 916 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 918 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetPlayLength */


	/* Procedure SetPlayLength */


	/* Procedure Progress */

/* 920 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 922 */	NdrFcLong( 0x0 ),	/* 0 */
/* 926 */	NdrFcShort( 0x4 ),	/* 4 */
#ifndef _ALPHA_
/* 928 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 930 */	NdrFcShort( 0x8 ),	/* 8 */
/* 932 */	NdrFcShort( 0x8 ),	/* 8 */
/* 934 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter dwLength */


	/* Parameter dwLength */


	/* Parameter dwTranspiredTicks */

/* 936 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 938 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 940 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 942 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 944 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 946 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Reset */


	/* Procedure Play */


	/* Procedure Reset */


	/* Procedure Reset */


	/* Procedure Play */


	/* Procedure Reset */


	/* Procedure End */

/* 948 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 950 */	NdrFcLong( 0x0 ),	/* 0 */
/* 954 */	NdrFcShort( 0x5 ),	/* 5 */
#ifndef _ALPHA_
/* 956 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 958 */	NdrFcShort( 0x0 ),	/* 0 */
/* 960 */	NdrFcShort( 0x8 ),	/* 8 */
/* 962 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 964 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 966 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 968 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetName */

/* 970 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 972 */	NdrFcLong( 0x0 ),	/* 0 */
/* 976 */	NdrFcShort( 0x3 ),	/* 3 */
#ifndef _ALPHA_
/* 978 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 980 */	NdrFcShort( 0x8 ),	/* 8 */
/* 982 */	NdrFcShort( 0x8 ),	/* 8 */
/* 984 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter pwszName */

/* 986 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
#ifndef _ALPHA_
/* 988 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 990 */	NdrFcShort( 0x148 ),	/* Type Offset=328 */

	/* Parameter nMaxChars */

/* 992 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 994 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 996 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 998 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1000 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1002 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetManufacturer */

/* 1004 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1006 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1010 */	NdrFcShort( 0x4 ),	/* 4 */
#ifndef _ALPHA_
/* 1012 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 1014 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1016 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1018 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter pwszName */

/* 1020 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
#ifndef _ALPHA_
/* 1022 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1024 */	NdrFcShort( 0x152 ),	/* Type Offset=338 */

	/* Parameter nMaxChars */

/* 1026 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 1028 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1030 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1032 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1034 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1036 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPlayOffset */


	/* Procedure GetVersion */


	/* Procedure GetPlayOffset */


	/* Procedure GetVersion */

/* 1038 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1040 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1044 */	NdrFcShort( 0x5 ),	/* 5 */
#ifndef _ALPHA_
/* 1046 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1048 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1050 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1052 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pdwOffset */


	/* Parameter pdwVersion */


	/* Parameter pdwOffset */


	/* Parameter pdwVersion */

/* 1054 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 1056 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1058 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 1060 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1062 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1064 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetType */


	/* Procedure GetType */

/* 1066 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1068 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1072 */	NdrFcShort( 0x6 ),	/* 6 */
#ifndef _ALPHA_
/* 1074 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1076 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1078 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1080 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pdwType */


	/* Parameter pdwType */

/* 1082 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 1084 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1086 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 1088 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1090 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1092 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSerialNumber */


	/* Procedure GetSerialNumber */

/* 1094 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1096 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1100 */	NdrFcShort( 0x7 ),	/* 7 */
#ifndef _ALPHA_
/* 1102 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 1104 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1106 */	NdrFcShort( 0xb4 ),	/* 180 */
/* 1108 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter pSerialNumber */


	/* Parameter pSerialNumber */

/* 1110 */	NdrFcShort( 0x112 ),	/* Flags:  must free, out, simple ref, */
#ifndef _ALPHA_
/* 1112 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1114 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Parameter abMac */


	/* Parameter abMac */

/* 1116 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
#ifndef _ALPHA_
/* 1118 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1120 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Return value */


	/* Return value */

/* 1122 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1124 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1126 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetLongestPlayPosition */


	/* Procedure GetStatus */


	/* Procedure GetLongestPlayPosition */


	/* Procedure GetStatus */

/* 1128 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1130 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1134 */	NdrFcShort( 0x9 ),	/* 9 */
#ifndef _ALPHA_
/* 1136 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1138 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1140 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1142 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pdwLongestPos */


	/* Parameter pdwStatus */


	/* Parameter pdwLongestPos */


	/* Parameter pdwStatus */

/* 1144 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 1146 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1148 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 1150 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1152 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1154 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDeviceIcon */


	/* Procedure GetDeviceIcon */

/* 1156 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1158 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1162 */	NdrFcShort( 0xa ),	/* 10 */
#ifndef _ALPHA_
/* 1164 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1166 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1168 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1170 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter hIcon */


	/* Parameter hIcon */

/* 1172 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 1174 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1176 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 1178 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1180 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1182 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumStorage */

/* 1184 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1186 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1190 */	NdrFcShort( 0xb ),	/* 11 */
#ifndef _ALPHA_
/* 1192 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1194 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1196 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1198 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnumStorage */

/* 1200 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
#ifndef _ALPHA_
/* 1202 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1204 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Return value */

/* 1206 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1208 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1210 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFormatSupport */


	/* Procedure GetFormatSupport */

/* 1212 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1214 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1218 */	NdrFcShort( 0xc ),	/* 12 */
#ifndef _ALPHA_
/* 1220 */	NdrFcShort( 0x18 ),	/* x86, MIPS, PPC Stack size/offset = 24 */
#else
			NdrFcShort( 0x30 ),	/* Alpha Stack size/offset = 48 */
#endif
/* 1222 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1224 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1226 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x5,		/* 5 */

	/* Parameter pFormatEx */


	/* Parameter ppFormatEx */

/* 1228 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
#ifndef _ALPHA_
/* 1230 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1232 */	NdrFcShort( 0x158 ),	/* Type Offset=344 */

	/* Parameter pnFormatCount */


	/* Parameter pnFormatCount */

/* 1234 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 1236 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1238 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pppwszMimeType */


	/* Parameter pppwszMimeType */

/* 1240 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
#ifndef _ALPHA_
/* 1242 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1244 */	NdrFcShort( 0x172 ),	/* Type Offset=370 */

	/* Parameter pnMimeTypeCount */


	/* Parameter pnMimeTypeCount */

/* 1246 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 1248 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 1250 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 1252 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1254 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 1256 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SendOpaqueCommand */


	/* Procedure SendOpaqueCommand */

/* 1258 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1260 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1264 */	NdrFcShort( 0xd ),	/* 13 */
#ifndef _ALPHA_
/* 1266 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1268 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1270 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1272 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pCommand */


	/* Parameter pCommand */

/* 1274 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
#ifndef _ALPHA_
/* 1276 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1278 */	NdrFcShort( 0xf0 ),	/* Type Offset=240 */

	/* Return value */


	/* Return value */

/* 1280 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1282 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1284 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 1286 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1288 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1292 */	NdrFcShort( 0x3 ),	/* 3 */
#ifndef _ALPHA_
/* 1294 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 1296 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1298 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1300 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter celt */

/* 1302 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 1304 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1306 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppDevice */

/* 1308 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
#ifndef _ALPHA_
/* 1310 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1312 */	NdrFcShort( 0x1ae ),	/* Type Offset=430 */

	/* Parameter pceltFetched */

/* 1314 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 1316 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1318 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1320 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1322 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 1324 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Skip */

/* 1326 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1328 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1332 */	NdrFcShort( 0x4 ),	/* 4 */
#ifndef _ALPHA_
/* 1334 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 1336 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1338 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1340 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter celt */


	/* Parameter celt */


	/* Parameter celt */


	/* Parameter celt */

/* 1342 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 1344 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1346 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pceltFetched */


	/* Parameter pceltFetched */


	/* Parameter pceltFetched */


	/* Parameter pceltFetched */

/* 1348 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 1350 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1352 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 1354 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1356 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1358 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 1360 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1362 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1366 */	NdrFcShort( 0x6 ),	/* 6 */
#ifndef _ALPHA_
/* 1368 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1370 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1372 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1374 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnumDevice */

/* 1376 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
#ifndef _ALPHA_
/* 1378 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1380 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 1382 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1384 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1386 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Record */


	/* Procedure Record */

/* 1388 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1390 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1394 */	NdrFcShort( 0x6 ),	/* 6 */
#ifndef _ALPHA_
/* 1396 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1398 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1400 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1402 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pFormat */


	/* Parameter pFormat */

/* 1404 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
#ifndef _ALPHA_
/* 1406 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1408 */	NdrFcShort( 0x4e ),	/* Type Offset=78 */

	/* Return value */


	/* Return value */

/* 1410 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1412 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1414 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Pause */


	/* Procedure Pause */

/* 1416 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1418 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1422 */	NdrFcShort( 0x7 ),	/* 7 */
#ifndef _ALPHA_
/* 1424 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1426 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1428 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1430 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */


	/* Return value */

/* 1432 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1434 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1436 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Resume */


	/* Procedure Resume */

/* 1438 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1440 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1444 */	NdrFcShort( 0x8 ),	/* 8 */
#ifndef _ALPHA_
/* 1446 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1448 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1450 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1452 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */


	/* Return value */

/* 1454 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1456 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1458 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Stop */


	/* Procedure Stop */

/* 1460 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1462 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1466 */	NdrFcShort( 0x9 ),	/* 9 */
#ifndef _ALPHA_
/* 1468 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1470 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1472 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1474 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */


	/* Return value */

/* 1476 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1478 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1480 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 1482 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1484 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1488 */	NdrFcShort( 0x3 ),	/* 3 */
#ifndef _ALPHA_
/* 1490 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 1492 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1494 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1496 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter celt */

/* 1498 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 1500 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1502 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppStorage */

/* 1504 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
#ifndef _ALPHA_
/* 1506 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1508 */	NdrFcShort( 0x1d6 ),	/* Type Offset=470 */

	/* Parameter pceltFetched */

/* 1510 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 1512 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1514 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1516 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1518 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 1520 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 1522 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1524 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1528 */	NdrFcShort( 0x6 ),	/* 6 */
#ifndef _ALPHA_
/* 1530 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1532 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1534 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1536 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnumStorage */

/* 1538 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
#ifndef _ALPHA_
/* 1540 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1542 */	NdrFcShort( 0xba ),	/* Type Offset=186 */

	/* Return value */

/* 1544 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1546 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1548 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Insert */

/* 1550 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1552 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1556 */	NdrFcShort( 0x3 ),	/* 3 */
#ifndef _ALPHA_
/* 1558 */	NdrFcShort( 0x1c ),	/* x86, MIPS, PPC Stack size/offset = 28 */
#else
			NdrFcShort( 0x38 ),	/* Alpha Stack size/offset = 56 */
#endif
/* 1560 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1562 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1564 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x6,		/* 6 */

	/* Parameter fuMode */

/* 1566 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 1568 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1570 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pwszFile */

/* 1572 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
#ifndef _ALPHA_
/* 1574 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1576 */	NdrFcShort( 0x1e8 ),	/* Type Offset=488 */

	/* Parameter pOperation */

/* 1578 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
#ifndef _ALPHA_
/* 1580 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1582 */	NdrFcShort( 0x1ec ),	/* Type Offset=492 */

	/* Parameter pProgress */

/* 1584 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
#ifndef _ALPHA_
/* 1586 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 1588 */	NdrFcShort( 0x38 ),	/* Type Offset=56 */

	/* Parameter ppNewObject */

/* 1590 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
#ifndef _ALPHA_
/* 1592 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 1594 */	NdrFcShort( 0x1fe ),	/* Type Offset=510 */

	/* Return value */

/* 1596 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1598 */	NdrFcShort( 0x18 ),	/* x86, MIPS, PPC Stack size/offset = 24 */
#else
			NdrFcShort( 0x30 ),	/* Alpha Stack size/offset = 48 */
#endif
/* 1600 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Delete */

/* 1602 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1604 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1608 */	NdrFcShort( 0x4 ),	/* 4 */
#ifndef _ALPHA_
/* 1610 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 1612 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1614 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1616 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter fuMode */

/* 1618 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 1620 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1622 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pProgress */

/* 1624 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
#ifndef _ALPHA_
/* 1626 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1628 */	NdrFcShort( 0x38 ),	/* Type Offset=56 */

	/* Return value */

/* 1630 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1632 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1634 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Rename */

/* 1636 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1638 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1642 */	NdrFcShort( 0x5 ),	/* 5 */
#ifndef _ALPHA_
/* 1644 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 1646 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1648 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1650 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter fuMode */

/* 1652 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 1654 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1656 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pwszNewName */

/* 1658 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
#ifndef _ALPHA_
/* 1660 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1662 */	NdrFcShort( 0x204 ),	/* Type Offset=516 */

	/* Parameter pProgress */

/* 1664 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
#ifndef _ALPHA_
/* 1666 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1668 */	NdrFcShort( 0x38 ),	/* Type Offset=56 */

	/* Return value */

/* 1670 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1672 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 1674 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Read */

/* 1676 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1678 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1682 */	NdrFcShort( 0x6 ),	/* 6 */
#ifndef _ALPHA_
/* 1684 */	NdrFcShort( 0x18 ),	/* x86, MIPS, PPC Stack size/offset = 24 */
#else
			NdrFcShort( 0x30 ),	/* Alpha Stack size/offset = 48 */
#endif
/* 1686 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1688 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1690 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter fuMode */

/* 1692 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 1694 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1696 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pwszFile */

/* 1698 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
#ifndef _ALPHA_
/* 1700 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1702 */	NdrFcShort( 0x1e8 ),	/* Type Offset=488 */

	/* Parameter pProgress */

/* 1704 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
#ifndef _ALPHA_
/* 1706 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1708 */	NdrFcShort( 0x38 ),	/* Type Offset=56 */

	/* Parameter pOperation */

/* 1710 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
#ifndef _ALPHA_
/* 1712 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 1714 */	NdrFcShort( 0x1ec ),	/* Type Offset=492 */

	/* Return value */

/* 1716 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1718 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 1720 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Move */

/* 1722 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1724 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1728 */	NdrFcShort( 0x7 ),	/* 7 */
#ifndef _ALPHA_
/* 1730 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 1732 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1734 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1736 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter fuMode */

/* 1738 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 1740 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1742 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pTargetObject */

/* 1744 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
#ifndef _ALPHA_
/* 1746 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1748 */	NdrFcShort( 0x1c4 ),	/* Type Offset=452 */

	/* Parameter pProgress */

/* 1750 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
#ifndef _ALPHA_
/* 1752 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1754 */	NdrFcShort( 0x38 ),	/* Type Offset=56 */

	/* Return value */

/* 1756 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1758 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 1760 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetPlayOffset */


	/* Procedure SetPlayOffset */

/* 1762 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1764 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1768 */	NdrFcShort( 0x6 ),	/* 6 */
#ifndef _ALPHA_
/* 1770 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1772 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1774 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1776 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter dwOffset */


	/* Parameter dwOffset */

/* 1778 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 1780 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1782 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 1784 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1786 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1788 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTotalLength */


	/* Procedure GetTotalLength */

/* 1790 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1792 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1796 */	NdrFcShort( 0x7 ),	/* 7 */
#ifndef _ALPHA_
/* 1798 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1800 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1802 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1804 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pdwLength */


	/* Parameter pdwLength */

/* 1806 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 1808 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1810 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 1812 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1814 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1816 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumDevices */

/* 1818 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1820 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1824 */	NdrFcShort( 0x4 ),	/* 4 */
#ifndef _ALPHA_
/* 1826 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1828 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1830 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1832 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnumDevice */

/* 1834 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
#ifndef _ALPHA_
/* 1836 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1838 */	NdrFcShort( 0x206 ),	/* Type Offset=518 */

	/* Return value */

/* 1840 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1842 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1844 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 1846 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1848 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1852 */	NdrFcShort( 0x3 ),	/* 3 */
#ifndef _ALPHA_
/* 1854 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 1856 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1858 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1860 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter celt */

/* 1862 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 1864 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1866 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppDevice */

/* 1868 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
#ifndef _ALPHA_
/* 1870 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1872 */	NdrFcShort( 0x232 ),	/* Type Offset=562 */

	/* Parameter pceltFetched */

/* 1874 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 1876 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1878 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1880 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1882 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 1884 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 1886 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1888 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1892 */	NdrFcShort( 0x6 ),	/* 6 */
#ifndef _ALPHA_
/* 1894 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1896 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1898 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1900 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnumDevice */

/* 1902 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
#ifndef _ALPHA_
/* 1904 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1906 */	NdrFcShort( 0x206 ),	/* Type Offset=518 */

	/* Return value */

/* 1908 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1910 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1912 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetName */

/* 1914 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1916 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1920 */	NdrFcShort( 0x3 ),	/* 3 */
#ifndef _ALPHA_
/* 1922 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 1924 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1926 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1928 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter pwszName */

/* 1930 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
#ifndef _ALPHA_
/* 1932 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1934 */	NdrFcShort( 0x248 ),	/* Type Offset=584 */

	/* Parameter nMaxChars */

/* 1936 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 1938 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1940 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1942 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1944 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1946 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetManufacturer */

/* 1948 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1950 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1954 */	NdrFcShort( 0x4 ),	/* 4 */
#ifndef _ALPHA_
/* 1956 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 1958 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1960 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1962 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter pwszName */

/* 1964 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
#ifndef _ALPHA_
/* 1966 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1968 */	NdrFcShort( 0x252 ),	/* Type Offset=594 */

	/* Parameter nMaxChars */

/* 1970 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 1972 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1974 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1976 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1978 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1980 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumStorage */


	/* Procedure EnumStorage */

/* 1982 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1984 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1988 */	NdrFcShort( 0xb ),	/* 11 */
#ifndef _ALPHA_
/* 1990 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1992 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1994 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1996 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnumStorage */


	/* Parameter ppEnumStorage */

/* 1998 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
#ifndef _ALPHA_
/* 2000 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 2002 */	NdrFcShort( 0x258 ),	/* Type Offset=600 */

	/* Return value */


	/* Return value */

/* 2004 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 2006 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 2008 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 2010 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2012 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2016 */	NdrFcShort( 0x3 ),	/* 3 */
#ifndef _ALPHA_
/* 2018 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 2020 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2022 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2024 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter celt */

/* 2026 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 2028 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 2030 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppStorage */

/* 2032 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
#ifndef _ALPHA_
/* 2034 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 2036 */	NdrFcShort( 0x284 ),	/* Type Offset=644 */

	/* Parameter pceltFetched */

/* 2038 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 2040 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 2042 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2044 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 2046 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 2048 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 2050 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2052 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2056 */	NdrFcShort( 0x6 ),	/* 6 */
#ifndef _ALPHA_
/* 2058 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 2060 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2062 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2064 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnumStorage */

/* 2066 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
#ifndef _ALPHA_
/* 2068 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 2070 */	NdrFcShort( 0x258 ),	/* Type Offset=600 */

	/* Return value */

/* 2072 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 2074 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 2076 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetStorageGlobals */

/* 2078 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2080 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2084 */	NdrFcShort( 0x4 ),	/* 4 */
#ifndef _ALPHA_
/* 2086 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 2088 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2090 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2092 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppStorageGlobals */

/* 2094 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
#ifndef _ALPHA_
/* 2096 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 2098 */	NdrFcShort( 0x296 ),	/* Type Offset=662 */

	/* Return value */

/* 2100 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 2102 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 2104 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetName */

/* 2106 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2108 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2112 */	NdrFcShort( 0x6 ),	/* 6 */
#ifndef _ALPHA_
/* 2114 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 2116 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2118 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2120 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter pwszName */

/* 2122 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
#ifndef _ALPHA_
/* 2124 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 2126 */	NdrFcShort( 0x2b0 ),	/* Type Offset=688 */

	/* Parameter nMaxChars */

/* 2128 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 2130 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 2132 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2134 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 2136 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 2138 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateStorage */

/* 2140 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2142 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2146 */	NdrFcShort( 0xa ),	/* 10 */
#ifndef _ALPHA_
/* 2148 */	NdrFcShort( 0x18 ),	/* x86, MIPS, PPC Stack size/offset = 24 */
#else
			NdrFcShort( 0x30 ),	/* Alpha Stack size/offset = 48 */
#endif
/* 2150 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2152 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2154 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter dwAttributes */

/* 2156 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 2158 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 2160 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pFormat */

/* 2162 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
#ifndef _ALPHA_
/* 2164 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 2166 */	NdrFcShort( 0x2b6 ),	/* Type Offset=694 */

	/* Parameter pwszName */

/* 2168 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
#ifndef _ALPHA_
/* 2170 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 2172 */	NdrFcShort( 0x204 ),	/* Type Offset=516 */

	/* Parameter ppNewStorage */

/* 2174 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
#ifndef _ALPHA_
/* 2176 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 2178 */	NdrFcShort( 0x2ba ),	/* Type Offset=698 */

	/* Return value */

/* 2180 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 2182 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 2184 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SendOpaqueCommand */

/* 2186 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2188 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2192 */	NdrFcShort( 0xc ),	/* 12 */
#ifndef _ALPHA_
/* 2194 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 2196 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2198 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2200 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pCommand */

/* 2202 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
#ifndef _ALPHA_
/* 2204 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 2206 */	NdrFcShort( 0xf0 ),	/* Type Offset=240 */

	/* Return value */

/* 2208 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 2210 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 2212 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDevice */

/* 2214 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2216 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2220 */	NdrFcShort( 0xa ),	/* 10 */
#ifndef _ALPHA_
/* 2222 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 2224 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2226 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2228 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppDevice */

/* 2230 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
#ifndef _ALPHA_
/* 2232 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 2234 */	NdrFcShort( 0x2be ),	/* Type Offset=702 */

	/* Return value */

/* 2236 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 2238 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 2240 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetRootStorage */

/* 2242 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2244 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2248 */	NdrFcShort( 0xb ),	/* 11 */
#ifndef _ALPHA_
/* 2250 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 2252 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2254 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2256 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppRoot */

/* 2258 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
#ifndef _ALPHA_
/* 2260 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 2262 */	NdrFcShort( 0x2ba ),	/* Type Offset=698 */

	/* Return value */

/* 2264 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 2266 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 2268 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Read */

/* 2270 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2272 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2276 */	NdrFcShort( 0x4 ),	/* 4 */
#ifndef _ALPHA_
/* 2278 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 2280 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2282 */	NdrFcShort( 0x20 ),	/* 32 */
/* 2284 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter pData */

/* 2286 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
#ifndef _ALPHA_
/* 2288 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 2290 */	NdrFcShort( 0x124 ),	/* Type Offset=292 */

	/* Parameter pdwSize */

/* 2292 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
#ifndef _ALPHA_
/* 2294 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 2296 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter abMac */

/* 2298 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
#ifndef _ALPHA_
/* 2300 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 2302 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Return value */

/* 2304 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 2306 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 2308 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Write */

/* 2310 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2312 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2316 */	NdrFcShort( 0x5 ),	/* 5 */
#ifndef _ALPHA_
/* 2318 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 2320 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2322 */	NdrFcShort( 0x20 ),	/* 32 */
/* 2324 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter pData */

/* 2326 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
#ifndef _ALPHA_
/* 2328 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 2330 */	NdrFcShort( 0x124 ),	/* Type Offset=292 */

	/* Parameter pdwSize */

/* 2332 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
#ifndef _ALPHA_
/* 2334 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 2336 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter abMac */

/* 2338 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
#ifndef _ALPHA_
/* 2340 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 2342 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Return value */

/* 2344 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 2346 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 2348 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Delete */

/* 2350 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2352 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2356 */	NdrFcShort( 0x6 ),	/* 6 */
#ifndef _ALPHA_
/* 2358 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 2360 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2362 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2364 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter fuMode */

/* 2366 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 2368 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 2370 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pProgress */

/* 2372 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
#ifndef _ALPHA_
/* 2374 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 2376 */	NdrFcShort( 0x38 ),	/* Type Offset=56 */

	/* Return value */

/* 2378 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 2380 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 2382 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Seek */

/* 2384 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2386 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2390 */	NdrFcShort( 0x7 ),	/* 7 */
#ifndef _ALPHA_
/* 2392 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 2394 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2396 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2398 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter fuFlags */

/* 2400 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 2402 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 2404 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwOffset */

/* 2406 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 2408 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 2410 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2412 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 2414 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 2416 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Rename */

/* 2418 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2420 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2424 */	NdrFcShort( 0x8 ),	/* 8 */
#ifndef _ALPHA_
/* 2426 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 2428 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2430 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2432 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pwszNewName */

/* 2434 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
#ifndef _ALPHA_
/* 2436 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 2438 */	NdrFcShort( 0x204 ),	/* Type Offset=516 */

	/* Parameter pProgress */

/* 2440 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
#ifndef _ALPHA_
/* 2442 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 2444 */	NdrFcShort( 0x38 ),	/* Type Offset=56 */

	/* Return value */

/* 2446 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 2448 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 2450 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Move */

/* 2452 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2454 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2458 */	NdrFcShort( 0x9 ),	/* 9 */
#ifndef _ALPHA_
/* 2460 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 2462 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2464 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2466 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter fuMode */

/* 2468 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 2470 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 2472 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pProgress */

/* 2474 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
#ifndef _ALPHA_
/* 2476 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 2478 */	NdrFcShort( 0x38 ),	/* Type Offset=56 */

	/* Parameter pTarget */

/* 2480 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
#ifndef _ALPHA_
/* 2482 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 2484 */	NdrFcShort( 0x272 ),	/* Type Offset=626 */

	/* Return value */

/* 2486 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 2488 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 2490 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Close */

/* 2492 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2494 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2498 */	NdrFcShort( 0xa ),	/* 10 */
#ifndef _ALPHA_
/* 2500 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 2502 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2504 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2506 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 2508 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 2510 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 2512 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SACAuth */

/* 2514 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2516 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2520 */	NdrFcShort( 0x3 ),	/* 3 */
#ifndef _ALPHA_
/* 2522 */	NdrFcShort( 0x20 ),	/* x86, MIPS, PPC Stack size/offset = 32 */
#else
			NdrFcShort( 0x40 ),	/* Alpha Stack size/offset = 64 */
#endif
/* 2524 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2526 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2528 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x7,		/* 7 */

	/* Parameter dwProtocolID */

/* 2530 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 2532 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 2534 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwPass */

/* 2536 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 2538 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 2540 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pbDataIn */

/* 2542 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
#ifndef _ALPHA_
/* 2544 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 2546 */	NdrFcShort( 0x2c6 ),	/* Type Offset=710 */

	/* Parameter dwDataInLen */

/* 2548 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 2550 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 2552 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppbDataOut */

/* 2554 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
#ifndef _ALPHA_
/* 2556 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 2558 */	NdrFcShort( 0x2d0 ),	/* Type Offset=720 */

	/* Parameter pdwDataOutLen */

/* 2560 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 2562 */	NdrFcShort( 0x18 ),	/* x86, MIPS, PPC Stack size/offset = 24 */
#else
			NdrFcShort( 0x30 ),	/* Alpha Stack size/offset = 48 */
#endif
/* 2564 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2566 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 2568 */	NdrFcShort( 0x1c ),	/* x86, MIPS, PPC Stack size/offset = 28 */
#else
			NdrFcShort( 0x38 ),	/* Alpha Stack size/offset = 56 */
#endif
/* 2570 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SACGetProtocols */

/* 2572 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2574 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2578 */	NdrFcShort( 0x4 ),	/* 4 */
#ifndef _ALPHA_
/* 2580 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 2582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2584 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2586 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter ppdwProtocols */

/* 2588 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
#ifndef _ALPHA_
/* 2590 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 2592 */	NdrFcShort( 0x2e2 ),	/* Type Offset=738 */

	/* Parameter pdwProtocolCount */

/* 2594 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 2596 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 2598 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2600 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 2602 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 2604 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/*  4 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x11, 0x10,	/* FC_RP */
/*  8 */	NdrFcShort( 0x2 ),	/* Offset= 2 (10) */
/* 10 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 12 */	NdrFcLong( 0x1dcb3a01 ),	/* 499857921 */
/* 16 */	NdrFcShort( 0x33ed ),	/* 13293 */
/* 18 */	NdrFcShort( 0x11d3 ),	/* 4563 */
/* 20 */	0x84,		/* 132 */
			0x70,		/* 112 */
/* 22 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 24 */	0x4f,		/* 79 */
			0x79,		/* 121 */
/* 26 */	0xdb,		/* 219 */
			0xc0,		/* 192 */
/* 28 */	
			0x11, 0x0,	/* FC_RP */
/* 30 */	NdrFcShort( 0x8 ),	/* Offset= 8 (38) */
/* 32 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 34 */	NdrFcShort( 0x80 ),	/* 128 */
/* 36 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 38 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 40 */	NdrFcShort( 0x8c ),	/* 140 */
/* 42 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 44 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 46 */	NdrFcShort( 0xfffffff2 ),	/* Offset= -14 (32) */
/* 48 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 50 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 52 */	NdrFcShort( 0x8 ),	/* 8 */
/* 54 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 56 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 58 */	NdrFcLong( 0x1dcb3a0c ),	/* 499857932 */
/* 62 */	NdrFcShort( 0x33ed ),	/* 13293 */
/* 64 */	NdrFcShort( 0x11d3 ),	/* 4563 */
/* 66 */	0x84,		/* 132 */
			0x70,		/* 112 */
/* 68 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 70 */	0x4f,		/* 79 */
			0x79,		/* 121 */
/* 72 */	0xdb,		/* 219 */
			0xc0,		/* 192 */
/* 74 */	
			0x11, 0x0,	/* FC_RP */
/* 76 */	NdrFcShort( 0x2 ),	/* Offset= 2 (78) */
/* 78 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 80 */	NdrFcShort( 0x14 ),	/* 20 */
/* 82 */	NdrFcShort( 0x0 ),	/* 0 */
/* 84 */	NdrFcShort( 0x0 ),	/* Offset= 0 (84) */
/* 86 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 88 */	0x38,		/* FC_ALIGNM4 */
			0x8,		/* FC_LONG */
/* 90 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 92 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 94 */	0x3e,		/* FC_STRUCTPAD2 */
			0x5b,		/* FC_END */
/* 96 */	
			0x11, 0x10,	/* FC_RP */
/* 98 */	NdrFcShort( 0x2 ),	/* Offset= 2 (100) */
/* 100 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 102 */	NdrFcLong( 0x1dcb3a07 ),	/* 499857927 */
/* 106 */	NdrFcShort( 0x33ed ),	/* 13293 */
/* 108 */	NdrFcShort( 0x11d3 ),	/* 4563 */
/* 110 */	0x84,		/* 132 */
			0x70,		/* 112 */
/* 112 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 114 */	0x4f,		/* 79 */
			0x79,		/* 121 */
/* 116 */	0xdb,		/* 219 */
			0xc0,		/* 192 */
/* 118 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 120 */	NdrFcShort( 0xffffffd6 ),	/* Offset= -42 (78) */
/* 122 */	
			0x11, 0x0,	/* FC_RP */
/* 124 */	NdrFcShort( 0x2 ),	/* Offset= 2 (126) */
/* 126 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 128 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
#ifndef _ALPHA_
/* 130 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 132 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 134 */	NdrFcShort( 0x2 ),	/* Offset= 2 (136) */
/* 136 */	
			0x15,		/* FC_STRUCT */
			0x1,		/* 1 */
/* 138 */	NdrFcShort( 0xc ),	/* 12 */
/* 140 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 142 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 144 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 146 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 148 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] */
/* 150 */	NdrFcShort( 0x2 ),	/* Offset= 2 (152) */
/* 152 */	
			0x13, 0x0,	/* FC_OP */
/* 154 */	NdrFcShort( 0x12 ),	/* Offset= 18 (172) */
/* 156 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 158 */	NdrFcShort( 0x24 ),	/* 36 */
/* 160 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 162 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 164 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 166 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 168 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (136) */
/* 170 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 172 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 174 */	NdrFcShort( 0x24 ),	/* 36 */
/* 176 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
#ifndef _ALPHA_
/* 178 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 180 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 182 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (156) */
/* 184 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 186 */	
			0x11, 0x10,	/* FC_RP */
/* 188 */	NdrFcShort( 0x2 ),	/* Offset= 2 (190) */
/* 190 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 192 */	NdrFcLong( 0x1dcb3a05 ),	/* 499857925 */
/* 196 */	NdrFcShort( 0x33ed ),	/* 13293 */
/* 198 */	NdrFcShort( 0x11d3 ),	/* 4563 */
/* 200 */	0x84,		/* 132 */
			0x70,		/* 112 */
/* 202 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 204 */	0x4f,		/* 79 */
			0x79,		/* 121 */
/* 206 */	0xdb,		/* 219 */
			0xc0,		/* 192 */
/* 208 */	
			0x11, 0x0,	/* FC_RP */
/* 210 */	NdrFcShort( 0x1e ),	/* Offset= 30 (240) */
/* 212 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 214 */	NdrFcShort( 0x10 ),	/* 16 */
/* 216 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 218 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 220 */	0x0,		/* 0 */
			NdrFcShort( 0xffffff55 ),	/* Offset= -171 (50) */
			0x5b,		/* FC_END */
/* 224 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 226 */	NdrFcShort( 0x14 ),	/* 20 */
/* 228 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 230 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 232 */	NdrFcShort( 0x1 ),	/* 1 */
/* 234 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 236 */	NdrFcShort( 0x10 ),	/* 16 */
/* 238 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 240 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 242 */	NdrFcShort( 0x2c ),	/* 44 */
/* 244 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 246 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 248 */	NdrFcShort( 0x14 ),	/* 20 */
/* 250 */	NdrFcShort( 0x14 ),	/* 20 */
/* 252 */	0x13, 0x0,	/* FC_OP */
/* 254 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (230) */
/* 256 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 258 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffd1 ),	/* Offset= -47 (212) */
			0x8,		/* FC_LONG */
/* 262 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 264 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffd7 ),	/* Offset= -41 (224) */
			0x5b,		/* FC_END */
/* 268 */	
			0x11, 0x0,	/* FC_RP */
/* 270 */	NdrFcShort( 0x2 ),	/* Offset= 2 (272) */
/* 272 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 274 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
#ifndef _ALPHA_
/* 276 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 278 */	
			0x11, 0x0,	/* FC_RP */
/* 280 */	NdrFcShort( 0x2 ),	/* Offset= 2 (282) */
/* 282 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 284 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
#ifndef _ALPHA_
/* 286 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 288 */	
			0x11, 0x0,	/* FC_RP */
/* 290 */	NdrFcShort( 0x2 ),	/* Offset= 2 (292) */
/* 292 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 294 */	NdrFcShort( 0x1 ),	/* 1 */
/* 296 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
#ifndef _ALPHA_
/* 298 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 300 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 302 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 304 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 306 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 308 */	NdrFcLong( 0x0 ),	/* 0 */
/* 312 */	NdrFcShort( 0x0 ),	/* 0 */
/* 314 */	NdrFcShort( 0x0 ),	/* 0 */
/* 316 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 318 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 320 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 322 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 324 */	
			0x11, 0x0,	/* FC_RP */
/* 326 */	NdrFcShort( 0x2 ),	/* Offset= 2 (328) */
/* 328 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 330 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
#ifndef _ALPHA_
/* 332 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 334 */	
			0x11, 0x0,	/* FC_RP */
/* 336 */	NdrFcShort( 0x2 ),	/* Offset= 2 (338) */
/* 338 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 340 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
#ifndef _ALPHA_
/* 342 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 344 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] */
/* 346 */	NdrFcShort( 0x2 ),	/* Offset= 2 (348) */
/* 348 */	
			0x13, 0x0,	/* FC_OP */
/* 350 */	NdrFcShort( 0x2 ),	/* Offset= 2 (352) */
/* 352 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 354 */	NdrFcShort( 0x0 ),	/* 0 */
/* 356 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
#ifndef _ALPHA_
/* 358 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 360 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 364 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 366 */	NdrFcShort( 0xfffffee0 ),	/* Offset= -288 (78) */
/* 368 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 370 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] */
/* 372 */	NdrFcShort( 0x2 ),	/* Offset= 2 (374) */
/* 374 */	
			0x13, 0x0,	/* FC_OP */
/* 376 */	NdrFcShort( 0x2 ),	/* Offset= 2 (378) */
/* 378 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 380 */	NdrFcShort( 0x4 ),	/* 4 */
/* 382 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
#ifndef _ALPHA_
/* 384 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 386 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 388 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 390 */	NdrFcShort( 0x4 ),	/* 4 */
/* 392 */	NdrFcShort( 0x0 ),	/* 0 */
/* 394 */	NdrFcShort( 0x1 ),	/* 1 */
/* 396 */	NdrFcShort( 0x0 ),	/* 0 */
/* 398 */	NdrFcShort( 0x0 ),	/* 0 */
/* 400 */	0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 402 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 404 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 406 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 408 */	
			0x11, 0x0,	/* FC_RP */
/* 410 */	NdrFcShort( 0x14 ),	/* Offset= 20 (430) */
/* 412 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 414 */	NdrFcLong( 0x1dcb3a02 ),	/* 499857922 */
/* 418 */	NdrFcShort( 0x33ed ),	/* 13293 */
/* 420 */	NdrFcShort( 0x11d3 ),	/* 4563 */
/* 422 */	0x84,		/* 132 */
			0x70,		/* 112 */
/* 424 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 426 */	0x4f,		/* 79 */
			0x79,		/* 121 */
/* 428 */	0xdb,		/* 219 */
			0xc0,		/* 192 */
/* 430 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 432 */	NdrFcShort( 0x0 ),	/* 0 */
/* 434 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
#ifndef _ALPHA_
/* 436 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 438 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
#ifndef _ALPHA_
/* 440 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 442 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 444 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (412) */
/* 446 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 448 */	
			0x11, 0x0,	/* FC_RP */
/* 450 */	NdrFcShort( 0x14 ),	/* Offset= 20 (470) */
/* 452 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 454 */	NdrFcLong( 0x1dcb3a06 ),	/* 499857926 */
/* 458 */	NdrFcShort( 0x33ed ),	/* 13293 */
/* 460 */	NdrFcShort( 0x11d3 ),	/* 4563 */
/* 462 */	0x84,		/* 132 */
			0x70,		/* 112 */
/* 464 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 466 */	0x4f,		/* 79 */
			0x79,		/* 121 */
/* 468 */	0xdb,		/* 219 */
			0xc0,		/* 192 */
/* 470 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 472 */	NdrFcShort( 0x0 ),	/* 0 */
/* 474 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
#ifndef _ALPHA_
/* 476 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 478 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
#ifndef _ALPHA_
/* 480 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 482 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 484 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (452) */
/* 486 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 488 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 490 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 492 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 494 */	NdrFcLong( 0x1dcb3a0b ),	/* 499857931 */
/* 498 */	NdrFcShort( 0x33ed ),	/* 13293 */
/* 500 */	NdrFcShort( 0x11d3 ),	/* 4563 */
/* 502 */	0x84,		/* 132 */
			0x70,		/* 112 */
/* 504 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 506 */	0x4f,		/* 79 */
			0x79,		/* 121 */
/* 508 */	0xdb,		/* 219 */
			0xc0,		/* 192 */
/* 510 */	
			0x11, 0x10,	/* FC_RP */
/* 512 */	NdrFcShort( 0xffffffc4 ),	/* Offset= -60 (452) */
/* 514 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 516 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 518 */	
			0x11, 0x10,	/* FC_RP */
/* 520 */	NdrFcShort( 0x2 ),	/* Offset= 2 (522) */
/* 522 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 524 */	NdrFcLong( 0x1dcb3a11 ),	/* 499857937 */
/* 528 */	NdrFcShort( 0x33ed ),	/* 13293 */
/* 530 */	NdrFcShort( 0x11d3 ),	/* 4563 */
/* 532 */	0x84,		/* 132 */
			0x70,		/* 112 */
/* 534 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 536 */	0x4f,		/* 79 */
			0x79,		/* 121 */
/* 538 */	0xdb,		/* 219 */
			0xc0,		/* 192 */
/* 540 */	
			0x11, 0x0,	/* FC_RP */
/* 542 */	NdrFcShort( 0x14 ),	/* Offset= 20 (562) */
/* 544 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 546 */	NdrFcLong( 0x1dcb3a12 ),	/* 499857938 */
/* 550 */	NdrFcShort( 0x33ed ),	/* 13293 */
/* 552 */	NdrFcShort( 0x11d3 ),	/* 4563 */
/* 554 */	0x84,		/* 132 */
			0x70,		/* 112 */
/* 556 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 558 */	0x4f,		/* 79 */
			0x79,		/* 121 */
/* 560 */	0xdb,		/* 219 */
			0xc0,		/* 192 */
/* 562 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 564 */	NdrFcShort( 0x0 ),	/* 0 */
/* 566 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
#ifndef _ALPHA_
/* 568 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 570 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
#ifndef _ALPHA_
/* 572 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 574 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 576 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (544) */
/* 578 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 580 */	
			0x11, 0x0,	/* FC_RP */
/* 582 */	NdrFcShort( 0x2 ),	/* Offset= 2 (584) */
/* 584 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 586 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
#ifndef _ALPHA_
/* 588 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 590 */	
			0x11, 0x0,	/* FC_RP */
/* 592 */	NdrFcShort( 0x2 ),	/* Offset= 2 (594) */
/* 594 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 596 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
#ifndef _ALPHA_
/* 598 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 600 */	
			0x11, 0x10,	/* FC_RP */
/* 602 */	NdrFcShort( 0x2 ),	/* Offset= 2 (604) */
/* 604 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 606 */	NdrFcLong( 0x1dcb3a15 ),	/* 499857941 */
/* 610 */	NdrFcShort( 0x33ed ),	/* 13293 */
/* 612 */	NdrFcShort( 0x11d3 ),	/* 4563 */
/* 614 */	0x84,		/* 132 */
			0x70,		/* 112 */
/* 616 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 618 */	0x4f,		/* 79 */
			0x79,		/* 121 */
/* 620 */	0xdb,		/* 219 */
			0xc0,		/* 192 */
/* 622 */	
			0x11, 0x0,	/* FC_RP */
/* 624 */	NdrFcShort( 0x14 ),	/* Offset= 20 (644) */
/* 626 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 628 */	NdrFcLong( 0x1dcb3a16 ),	/* 499857942 */
/* 632 */	NdrFcShort( 0x33ed ),	/* 13293 */
/* 634 */	NdrFcShort( 0x11d3 ),	/* 4563 */
/* 636 */	0x84,		/* 132 */
			0x70,		/* 112 */
/* 638 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 640 */	0x4f,		/* 79 */
			0x79,		/* 121 */
/* 642 */	0xdb,		/* 219 */
			0xc0,		/* 192 */
/* 644 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 646 */	NdrFcShort( 0x0 ),	/* 0 */
/* 648 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
#ifndef _ALPHA_
/* 650 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 652 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
#ifndef _ALPHA_
/* 654 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 656 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 658 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (626) */
/* 660 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 662 */	
			0x11, 0x10,	/* FC_RP */
/* 664 */	NdrFcShort( 0x2 ),	/* Offset= 2 (666) */
/* 666 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 668 */	NdrFcLong( 0x1dcb3a17 ),	/* 499857943 */
/* 672 */	NdrFcShort( 0x33ed ),	/* 13293 */
/* 674 */	NdrFcShort( 0x11d3 ),	/* 4563 */
/* 676 */	0x84,		/* 132 */
			0x70,		/* 112 */
/* 678 */	0x0,		/* 0 */
			0xc0,		/* 192 */
/* 680 */	0x4f,		/* 79 */
			0x79,		/* 121 */
/* 682 */	0xdb,		/* 219 */
			0xc0,		/* 192 */
/* 684 */	
			0x11, 0x0,	/* FC_RP */
/* 686 */	NdrFcShort( 0x2 ),	/* Offset= 2 (688) */
/* 688 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 690 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
#ifndef _ALPHA_
/* 692 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 694 */	
			0x12, 0x0,	/* FC_UP */
/* 696 */	NdrFcShort( 0xfffffd96 ),	/* Offset= -618 (78) */
/* 698 */	
			0x11, 0x10,	/* FC_RP */
/* 700 */	NdrFcShort( 0xffffffb6 ),	/* Offset= -74 (626) */
/* 702 */	
			0x11, 0x10,	/* FC_RP */
/* 704 */	NdrFcShort( 0xffffff60 ),	/* Offset= -160 (544) */
/* 706 */	
			0x11, 0x0,	/* FC_RP */
/* 708 */	NdrFcShort( 0x2 ),	/* Offset= 2 (710) */
/* 710 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 712 */	NdrFcShort( 0x1 ),	/* 1 */
/* 714 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
#ifndef _ALPHA_
/* 716 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 718 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 720 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] */
/* 722 */	NdrFcShort( 0x2 ),	/* Offset= 2 (724) */
/* 724 */	
			0x13, 0x0,	/* FC_OP */
/* 726 */	NdrFcShort( 0x2 ),	/* Offset= 2 (728) */
/* 728 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 730 */	NdrFcShort( 0x1 ),	/* 1 */
/* 732 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
#ifndef _ALPHA_
/* 734 */	NdrFcShort( 0x18 ),	/* x86, MIPS, PPC Stack size/offset = 24 */
#else
			NdrFcShort( 0x30 ),	/* Alpha Stack size/offset = 48 */
#endif
/* 736 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 738 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] */
/* 740 */	NdrFcShort( 0x2 ),	/* Offset= 2 (742) */
/* 742 */	
			0x13, 0x0,	/* FC_OP */
/* 744 */	NdrFcShort( 0x2 ),	/* Offset= 2 (746) */
/* 746 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 748 */	NdrFcShort( 0x4 ),	/* 4 */
/* 750 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
#ifndef _ALPHA_
/* 752 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 754 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */

			0x0
        }
    };

const CInterfaceProxyVtbl * _mavspi_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IWMDeviceManagerProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IComponentAuthenticateProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IWMDMEnumDeviceProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IWMDMDeviceProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IWMDMDeviceControlProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IWMDMEnumStorageProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IWMDMStorageProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IWMDMStorageGlobalsProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IWMDMStorageControlProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IWMDMObjectInfoProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IWMDMOperationProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IWMDMProgressProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IMDServiceProviderProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IMDSPEnumDeviceProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IMDSPDeviceProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IMDSPDeviceControlProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IMDSPEnumStorageProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IMDSPStorageProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IMDSPStorageGlobalsProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IMDSPObjectProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IMDSPObjectInfoProxyVtbl,
    0
};

const CInterfaceStubVtbl * _mavspi_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IWMDeviceManagerStubVtbl,
    ( CInterfaceStubVtbl *) &_IComponentAuthenticateStubVtbl,
    ( CInterfaceStubVtbl *) &_IWMDMEnumDeviceStubVtbl,
    ( CInterfaceStubVtbl *) &_IWMDMDeviceStubVtbl,
    ( CInterfaceStubVtbl *) &_IWMDMDeviceControlStubVtbl,
    ( CInterfaceStubVtbl *) &_IWMDMEnumStorageStubVtbl,
    ( CInterfaceStubVtbl *) &_IWMDMStorageStubVtbl,
    ( CInterfaceStubVtbl *) &_IWMDMStorageGlobalsStubVtbl,
    ( CInterfaceStubVtbl *) &_IWMDMStorageControlStubVtbl,
    ( CInterfaceStubVtbl *) &_IWMDMObjectInfoStubVtbl,
    ( CInterfaceStubVtbl *) &_IWMDMOperationStubVtbl,
    ( CInterfaceStubVtbl *) &_IWMDMProgressStubVtbl,
    ( CInterfaceStubVtbl *) &_IMDServiceProviderStubVtbl,
    ( CInterfaceStubVtbl *) &_IMDSPEnumDeviceStubVtbl,
    ( CInterfaceStubVtbl *) &_IMDSPDeviceStubVtbl,
    ( CInterfaceStubVtbl *) &_IMDSPDeviceControlStubVtbl,
    ( CInterfaceStubVtbl *) &_IMDSPEnumStorageStubVtbl,
    ( CInterfaceStubVtbl *) &_IMDSPStorageStubVtbl,
    ( CInterfaceStubVtbl *) &_IMDSPStorageGlobalsStubVtbl,
    ( CInterfaceStubVtbl *) &_IMDSPObjectStubVtbl,
    ( CInterfaceStubVtbl *) &_IMDSPObjectInfoStubVtbl,
    0
};

PCInterfaceName const _mavspi_InterfaceNamesList[] = 
{
    "IWMDeviceManager",
    "IComponentAuthenticate",
    "IWMDMEnumDevice",
    "IWMDMDevice",
    "IWMDMDeviceControl",
    "IWMDMEnumStorage",
    "IWMDMStorage",
    "IWMDMStorageGlobals",
    "IWMDMStorageControl",
    "IWMDMObjectInfo",
    "IWMDMOperation",
    "IWMDMProgress",
    "IMDServiceProvider",
    "IMDSPEnumDevice",
    "IMDSPDevice",
    "IMDSPDeviceControl",
    "IMDSPEnumStorage",
    "IMDSPStorage",
    "IMDSPStorageGlobals",
    "IMDSPObject",
    "IMDSPObjectInfo",
    0
};


#define _mavspi_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _mavspi, pIID, n)

int __stdcall _mavspi_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _mavspi, 21, 16 )
    IID_BS_LOOKUP_NEXT_TEST( _mavspi, 8 )
    IID_BS_LOOKUP_NEXT_TEST( _mavspi, 4 )
    IID_BS_LOOKUP_NEXT_TEST( _mavspi, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _mavspi, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _mavspi, 21, *pIndex )
    
}

const ExtendedProxyFileInfo mavspi_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _mavspi_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _mavspi_StubVtblList,
    (const PCInterfaceName * ) & _mavspi_InterfaceNamesList,
    0, // no delegation
    & _mavspi_IID_Lookup, 
    21,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};

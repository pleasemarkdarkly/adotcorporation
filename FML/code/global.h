//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1997-2000.
//
//  File:       G L O B A L. H
//
//  Contents:   Definitions, structures, and prototypes that are 
//              global for the device kit.
//
//----------------------------------------------------------------------------


// Modified by Rob

#ifndef _GLOBAL_H
#define _GLOBAL_H

#include "stdafx.h"
#include "global.h"


#include "network.h"
#include "httpserver.h"

//#include <stdlib.h>
//#include <stdio.h>
#define debugMessage(x) //afxDump << x

//#define _VERBOSE_DEBUG

#ifdef _VERBOSE_DEBUG

void LogOutput(const char *fmt, ... ); 
void OpenDebugFile(const char *name);	
void CloseDebugFile();

#endif //_VERBOSE_DEBUG


// Rob: We do not use the isapi so we don't include it
/*
// Include the data that is specific for the Web service interface.
#include "..\inc\isapictl.h"
*/

// Some arbitrary maximum values. 
static const DWORD MAX_ACTIONS          = 32;
static const DWORD MAX_SVC              = 32;
static const DWORD DEFAULT_ILIFETIME    = 600;
static const DWORD MAX_DEV_STACK        = 50;
static const DWORD MAX_NAME_SIZE        = 256;
static const DWORD MAX_VALUE_SIZE       = 256;
static const DWORD MAX_DATE_SIZE        = 32;
static const DWORD MAX_PROP_CHANGES     = 32;
static const DWORD MAX_URI_SIZE         = 1024;
static const DWORD MAX_HEADER_SIZE		= 512;
static const DWORD MAX_BUFFER_SIZE		= 1024;		//buffers for incoming HTTP requests


//  HTTP error codes.
#define FAULT_INVALID_ACTION            401
#define FAULT_INVALID_ARG               402
#define FAULT_INVALID_SEQUENCE_NUMBER   403
#define FAULT_INVALID_VARIABLE          404
#define FAULT_DEVICE_INTERNAL_ERROR     501
#define FAULT_ACTION_SPECIFIC_BASE      600
#define FAULT_ACTION_SPECIFIC_MAX       899


bool ValidateMACAddress(SOCKADDR_IN *sockAddr);

// Command line parsing.
static const DWORD MAX_ARGS             = 8;      

// Unique string used as marker for inserting the IP address.
const CHAR c_szReplaceGuid[] = "5479f6b6-71ac-44fc-9164-7e901a557f81";

// Definitions used for handling actions.
// Input arguments for the action; arguments are parsed out of the SOAP message.
struct ARG
{
    CHAR        szValue [MAX_VALUE_SIZE];
    CHAR        szName  [MAX_NAME_SIZE];
};

// Output arguments for the action. This is not used for the X10Light sample device. 
// This structure can be used by other devices that return arguments back to the
// control point.  ISAPIctl will generate a SOAP response containing these arguments.
struct ARG_OUT 
{
    CHAR        szValue [MAX_VALUE_SIZE];
    CHAR        szName  [MAX_NAME_SIZE];
};

//  Structure containing SOAP request-response data after it has been parsed.
//  The method name and argument values are placed in this structure.
struct SOAP_DATA
{
    LONG       lReturn;                        
    CHAR       szEventSource  [MAX_URI_SIZE];
    CHAR       szAction       [MAX_NAME_SIZE];
    DWORD      cArgs;
    ARG        rgArgs         [MAX_PROP_CHANGES];
    DWORD      cArgsOut;
    ARG_OUT    rgArgsOut      [MAX_PROP_CHANGES];
    CHAR       szNameSpaceUri [MAX_URI_SIZE]; 
};


// The following function is used for mapping a SOAP Action request to an actual
// implementation function in the device.

// pfnActionSync: This device-specific entry point is called before 
// ISAPI returns the SOAP response.  This allows the device to process 
// output arguments if needed. Since InvokeAction has a 30 second timeout, 
// the device must return from this function in less then 30 seconds.

typedef DWORD (*PFNAS) (
                        CHAR*           szEventUrl,
                        DWORD           cArgs,                  
                        ARG*            rgArgs,                
                        PDWORD          pArgsOut,               
                        ARG_OUT*        rgArgsOut             
                        );

// Entry point for device initialization.
typedef DWORD (*PFNAI) (CHAR* szEventUrl);  

// Entry point for device cleanup.
typedef DWORD (*PFNAC) (CHAR* szEventUrl);


struct ACTION
{
    LPCSTR   szAction;             // Action identifier (such as level). 
    PFNAS    pfnActionSync;        // Pointer to the device-specific function that performs the action.
};

// The following structure associates a service identifier with a set of 
// actions.  Each action has its associated device-specific entry points.  
// These entry points are implemented in X10Light.cpp.   
struct SERVICE_CTL
{
    LPCSTR          szServiceId;               // Service ID.
    PFNAI           pfnActionInit;             // Pointer to the device-specific initialization function.
    PFNAC           pfnActionCleanup;          // Pointer to the device-specific cleanup function.
    DWORD           cActions;                  // Number of actions in rgActions.
    ACTION          rgActions[MAX_ACTIONS];    // Array of actions for the service.
};

// The UPNPSVC structure is filled in with data from the device description
// file. One is filled for each service in the service list.
// It represents attributes needed to publish and control the device.
struct UPNPSVC 
{
    CHAR    szServiceId   [MAX_URI_SIZE];       // Key used to determine which service to invoke (UPDIAG-specific, such as pwrdim).
    CHAR    szServiceType [MAX_URI_SIZE];       // Prefix to the service type (such as urn:schemas-upnp-org:service:pwrdim:1).                
    CHAR    szControlUrl  [MAX_URI_SIZE];       // Canonical control URL (such as /upnp/services/pwrdim).
    CHAR    szControlQuery[MAX_URI_SIZE];       // Query part of control URL
    CHAR    szEvtUrl      [MAX_URI_SIZE];       // Canonical event URL (such as /upnp/services/pwrdim).
    CHAR    szEvtQuery    [MAX_URI_SIZE];       // Query part of event URL
    const   SERVICE_CTL* pSvcControl;           // Pointer to the device-specific function calls (UPDIAG-specific).
};

// The UPNPDEV structure is filled in with data from the device description 
// file. One is filled for the root device and for each device in the 
// device list.  It represents attributes needed to publish 
// and control devices.
struct UPNPDEV 
{
    BOOL    fRoot;                              // If TRUE, this is a root device (UPDIAG-specific).
    UPNPSVC rgSvc             [MAX_SVC];        // Array of UPNPSVC services related to this device (UPDIAG-specific). 
    DWORD   cSvcs;                              // Number of services in rgSvc (UPDIAG-specific).
    UPNPDEV* pNextDev;                          // Pointer to a nested device (UPDIAG-specific).
    CHAR   szUdn              [MAX_URI_SIZE];   // Unique device name (such as uuid:{Your UDN}).
    CHAR   szDeviceType       [MAX_URI_SIZE];   // Prefix to the device type (such as urn:schemas-upnp-org:device.lighting.1).
    CHAR   szDescUrl          [MAX_URI_SIZE];   // URL that points to the location of this device's description document.
};

/*
// Function prototypes, by module.
// Updiag.cpp -----------------------------------------------------------------
UPNPSVC* PSvcFromId(LPCSTR szId);

phase1.cpp
*/

DWORD GetFileContents(LPTSTR szFile, PCHAR* ppszaContents);

/*


// Device.cpp -----------------------------------------------------------------
*/
BOOL FCreateRootDeviceStruct(
                             IN CHAR* szDevDescription,
                             OUT UPNPDEV** pRoot
                             );

BOOL FDeleteRootDeviceStruct(
                             IN UPNPDEV* pRoot
                             );
/*
// Control.cpp ----------------------------------------------------------------
SharedMessageData* WebInterfaceInit();
BOOL WebInterfaceClose();

BOOL ActionHandlerInit(SharedMessageData* pWebData);
BOOL ActionHandlerClose();


UPNPSVC* PSvcFromIdDev(UPNPDEV* pDev, LPCSTR szId);


// DoCmds.cpp -----------------------------------------------------------------
typedef BOOL (* PFNCMD)(DWORD iCmd, DWORD cArgs, LPTSTR* rgArgs);

enum ECMD_CONTEXT 
{
    CTX_ROOT    = 0x00000001,       // Root context (start of app)
    CTX_CD      = 0x00000002,       // Looking at specific device
    CTX_AUTO    = 0x80000000,
    CTX_ANY     = 0xFFFFFFFF,       // Any context
};

struct COMMAND 
{
    LPCTSTR     szCommand;
    LPCTSTR     szCmdDesc;
    ECMD_CONTEXT dwCtx;
    PFNCMD      pfnCommand;
    LPCTSTR     szUsage;
};

struct UPDIAG_CONTEXT 
{
    ECMD_CONTEXT   ectx;
    UPNPDEV*       pRootDev[MAX_DEV_STACK];
    DWORD          dwDevCount;
    DWORD          dwCurDev;
};

BOOL     DoNewCd(
                 DWORD iCmd, 
                 DWORD cArgs, 
                 LPTSTR* rgArgs
                 );

BOOL     DoCleanupCd(
                     UPNPDEV* pRoot
                     );

BOOL     DoListDevices (
                        DWORD iCmd, 
                        DWORD cArgs, 
                        LPTSTR* rgArgs
                        );

BOOL     DoListServices (
                         DWORD iCmd, 
                         DWORD cArgs, 
                         LPTSTR* rgArgs
                         );

BOOL     DoHelp (
                 DWORD iCmd, 
                 DWORD cArgs, 
                 LPTSTR* rgArgs
                 );

BOOL     DoPrompt (
                   DWORD iCmd, 
                   DWORD cArgs, 
                   LPTSTR* rgArgs
                   );

UPNPDEV* PDevCur ();

// Publish.cpp ----------------------------------------------------------------

*/
  
BOOL     FPublishRootDevice ( 
                             UPNPDEV* pRoot 
                             );

BOOL     FUnPublishRootDevice ( 
                               UPNPDEV* pRoot 
                               );

/*
// Updiag.cpp -----------------------------------------------------------------

#define debugMessage(x) printf(x)

VOID  Usage (DWORD iCmd);

BOOL  DoExit(DWORD iCmd, DWORD cArgs, LPTSTR* rgArgs);

BOOL  DoRoot(DWORD iCmd, DWORD cArgs, LPTSTR* rgArgs);

BOOL  DoDevice(DWORD iCmd, DWORD cArgs, LPTSTR* rgArgs);

DWORD WINAPI RequestHandlerThreadStart(LPVOID pvParam);

// soap.cpp -------------------------------------------------------------------
DWORD ProcessSoapRequest(LPSTR szUri, DWORD cbData, LPBYTE pbData);
DWORD ComposeSoapResponse(LONG lValue, LPSTR pszOut, LPSTR pszStatusOut, DWORD* pdwHttpStatus);

// util.h ---------------------------------------------------------------------
VOID* MemAlloc(size_t cb);
VOID MemFree(VOID* pv);

*/

VOID GetDateTime(CHAR* szDateTime);
BOOL GetUniqueId(CHAR* szUniqueId, unsigned int size);
VOID GetServerString(CHAR* szServer, unsigned int size);


/*
// External Data --------------------------------------------------------------------
extern UPDIAG_CONTEXT               g_ctx;
extern MessageInfo*                 g_soapdata;
extern HANDLE                       g_hSoapEvent;
extern HANDLE                       g_hSoapEventRet;
extern MessageInfo*                 g_genadata;
extern HANDLE                       g_hGenaEvent;
extern HANDLE                       g_hGenaEventRet;

*/  
extern HANDLE                       g_hEventCleanup;

/*
extern SOAP_DATA*                   g_pdata;
*/

extern bool g_ServerActive;
BOOL DoCleanupCd(UPNPDEV* pRoot);

int startUPnPDiscovery(char *descriptionFile);
DWORD GetFileContents(LPTSTR szFile, PCHAR* ppszaContents);
BOOL BuildDescURL(char **descURL);

BOOL BuildBaseURL(char **baseURL);

// end testing purposes


// the global event that signals threads to die
extern HANDLE                  g_hEventCleanup;

// the root device that we are dealing with
extern UPNPDEV*				pDeviceRoot;

// where to get the device description from
extern char                    *pDeviceDescription;

#define descFile "c:\\desc.xml"
#define descName "/desc.xml"



#endif //_GLOBAL_H

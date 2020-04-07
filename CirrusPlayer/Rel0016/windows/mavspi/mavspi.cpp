//****************************************************************************
//
// MAVSPI.CPP - DLL entry points for the Maverick(tm) service provider.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
// Copyright (C) Microsoft Corporation, 1999 - 2001.  All rights reserved.
//
//****************************************************************************
#include "stdafx.h"
#include "initguid.h"
#include "mavspi_i.c"

//****************************************************************************
//
// The COM server object used by the service provider.
//
//****************************************************************************
CComModule _Module;

//****************************************************************************
//
// The instance handle of our DLL.
//
//****************************************************************************
HINSTANCE g_hinstance;

//****************************************************************************
//
// The secure channel server object used by the service provider.
//
//****************************************************************************
CSecureChannelServer *g_pAppSCServer = NULL;

//****************************************************************************
//
// The object which contains a local copy of the file list on the device.
//
//****************************************************************************
CMaverickInfo g_MaverickInfo;

//****************************************************************************
//
// The name of the registry key into which we register outselves with WMDM.
//
//****************************************************************************
static const char *pcRegKey =
    "Software\\Microsoft\\Windows Media Device Manager\\Plugins\\SP\\MavSPI";

//****************************************************************************
//
// The object map describing our top-level COM objects.
//
//****************************************************************************
BEGIN_OBJECT_MAP(ObjectMap)
    OBJECT_ENTRY(CLSID_MDServiceProvider, CMDServiceProvider)
END_OBJECT_MAP()

//****************************************************************************
//
// The main entry point for the DLL.
//
//****************************************************************************
extern "C" BOOL WINAPI
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    //
    // Save our instance handle for later use.
    //
    g_hinstance = hInstance;

    //
    // See if we were just loaded.
    //
    if(dwReason == DLL_PROCESS_ATTACH)
    {
        //
        // We were just loaded, so initialize our COM interface module.
        //
        _Module.Init(ObjectMap, hInstance);

        //
        // Prevent DLL_THREAD_ATTACH and DLL_THREAD_DETACH messages from being
        // sent to us, since we don't need them.
        //
        DisableThreadLibraryCalls(hInstance);
    }

    //
    // See if we are about to be unloaded.
    //
    else if(dwReason == DLL_PROCESS_DETACH)
    {
        //
        // We are about to be unloaded, so terminate our COM interface module.
        //
        _Module.Term();
    }

    //
    // Success.
    //
    return(TRUE);
}

//****************************************************************************
//
// Determines whether the DLL can be unloaded by OLE.
//
//****************************************************************************
STDAPI
DllCanUnloadNow(void)
{
    //
    // We can be unloaded when the COM interface module's lock count is zero.
    //
    return((_Module.GetLockCount() == 0) ? S_OK : S_FALSE);
}

//****************************************************************************
//
// Returns a class factory to create an object of the requested type
//
//****************************************************************************
STDAPI
DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv)
{
    //
    // Return the class factory from the COM interface module.
    //
    return(_Module.GetClassObject(rclsid, riid, ppv));
}

//****************************************************************************
//
// Registers the DLL by adding entries to the system registry.
//
//****************************************************************************
STDAPI
DllRegisterServer(void)
{
    char *pcValue = "MDServiceProviderMavSPI.MDServiceProviderMavSPI";
    HKEY hKey;
    LONG lRet;

    //
    // Create a key in the registry so we can register ourself with WMDM.
    //
    lRet = RegCreateKeyEx(HKEY_LOCAL_MACHINE, pcRegKey, 0, NULL,
                          REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL,
                          &hKey, NULL);

    //
    // See if we could create the registry key.
    //
    if(lRet == ERROR_SUCCESS)
    {
        //
        // Register our ProgID with WMDM.
        //
        RegSetValueEx(hKey, "ProgID", 0, REG_SZ, (LPBYTE)pcValue,
                      lstrlen(pcValue) + 1);

        //
        // Close the registry key.
        //
        RegCloseKey(hKey);

        //
        // Register the type library and all interfaces in the type library.
        //
        return(_Module.RegisterServer(TRUE));
    }

    //
    // We could not create the registry key, so return an error.
    //
    else
    {
        return(REGDB_E_WRITEREGDB);
    }
}

//****************************************************************************
//
// Unregisters the DLL by removing entries from the system registry.
//
//****************************************************************************
STDAPI
DllUnregisterServer(void)
{
    //
    // Unregister type type library and all interfaces in the type library.
    //
    _Module.UnregisterServer();

    //
    // Remove our WMDM registry key.
    //
    RegDeleteKey(HKEY_LOCAL_MACHINE, pcRegKey);

    //
    // Success.
    //
    return(S_OK);
}

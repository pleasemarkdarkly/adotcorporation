//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1997-2001.
//
//  File:       P U B L I S H . C P P
//
//  Contents:   Publishes a device with SSDP.
//
//--------------------------------------------------------------------------

#include "stdafx.h"
//#include <windows.h>
//#include <stdlib.h>
//#include <stdio.h>

#include "util.h"
#include "global.h"
#include "ssdp.h"
//#include "event.h"

// Forward declarations for private functions.
BOOL FRegisterDevice(UPNPDEV* pDev, LPSTR szDescDocUrl);
BOOL FRegisterService(UPNPDEV* pDev, UPNPSVC* pSvc, LPSTR szDescDocUrl);
BOOL FRegisterEvent(UPNPSVC* pSvc);
BOOL FUnregisterEvent(UPNPSVC* pSvc);


//////////
// FPublishRootDevice
//      Publishes a root device with SSDP.
//
// PARAMETERS
//      pRoot:  Pointer to the root device to publish.
//
// RETURNS:  
//      BOOL    TRUE if the device was published.
//              FALSE if the function failed to publish the device.
//
BOOL FPublishRootDevice(UPNPDEV* pRoot)
{
    UPNPDEV*    pDev = pRoot;
    UPNPSVC*    pSvc = NULL;
    BOOL        fPassed = TRUE;
    CHAR        szDescUrl[MAX_URI_SIZE];
    
    // Verify that pRoot points to a root device.
    if (pRoot->fRoot == FALSE)
    {
        puts("Can't register a non root device as a root device.");
        fPassed = FALSE;
        return fPassed;
    }

    strcpy(szDescUrl, pRoot->szDescUrl);
    
    // Walk through the root and all nested devices and register each
    // device and its services with SSDP.
    while (pDev)
    {
        // Register the device.
        if (!FRegisterDevice(pDev, szDescUrl))
        {
            puts("Failed to register device.");
            return FALSE;
        }
        
        // Register the devices's services.
        for (unsigned int i = 0; i < pDev->cSvcs; i++)
        {
            pSvc = &pDev->rgSvc[i];
            if (!FRegisterService (pDev, pSvc, szDescUrl))
            {
                puts("Failed to register a service.");
                return FALSE;
            }
        }
        
        // Get the next device in the list.
        pDev = pDev->pNextDev;
        
    } 
    
    // Publish it on the network (beacon)
    SsdpStartDiscovery(DEFAULT_ILIFETIME);
    
    return fPassed;
} // end FPublishRootDevice


/////////
// FUnPublishRootDevice
//      Unpublishes a root device with SSDP.
//
// PARAMETERS
//      pRoot:  Pointer to the root device to unpublish.
//
// RETURNS:  
//      BOOL    TRUE if the device was unpublished.
//              FALSE if the function failed to unpublish the device.
//
BOOL FUnPublishRootDevice(UPNPDEV* pRoot)
{
    BOOL fPassed = TRUE;
    
    // Verify that pRoot points to a root device.
    if (!pRoot)
    {
        return FALSE;
    }
    
    fPassed = SsdpStopDiscovery();
    
    // Cleanup all of this root devices event sources
    for (unsigned int dwI = 0; dwI < pRoot->cSvcs; dwI++)
    {
        if (!FUnregisterEvent(&pRoot->rgSvc[dwI]))
            fPassed = FALSE;
    }
    
    return fPassed;
} // end FUnPublishRootDevice


/////////
// FRegisterDevice
//      Register a single device with SSDP. This function is called by 
//      FPublishRootDevice.  If the root device flag is set in 
//      pDev, the device is registered as a root device.
//
// PARAMETERS
//      pDev:          Pointer to the device to publish.
//      szDescDocUrl:  The URL to this device's description document.
//
// RETURNS:  
//      BOOL    TRUE if the device was published.
//              FALSE if the function failed to publish the device.
//
BOOL FRegisterDevice(UPNPDEV* pDev, LPSTR szDescDocUrl)
{
    CHAR*  szUSN;
    int    sizeUSN = strlen(pDev->szUdn) + strlen(pDev->szDeviceType) + 
                        strlen("::upnp:rootdevice") + 1;

    szUSN = (CHAR*)malloc(sizeUSN);
    if (szUSN)
    {
        // If this is a root device, publish it as a root device.
        if (pDev->fRoot)
        {
            // Use new SSDP, add to SSDP message list.
            sprintf(szUSN, "%s::upnp:rootdevice", pDev->szUdn);
            SsdpAddMessage(szDescDocUrl, "upnp:rootdevice", szUSN);
        }
    
        strcpy(szUSN, pDev->szUdn);
        SsdpAddMessage(szDescDocUrl, pDev->szUdn, szUSN);
    
        sprintf(szUSN, "%s::%s", pDev->szUdn, pDev->szDeviceType);
        SsdpAddMessage(szDescDocUrl, pDev->szDeviceType, szUSN);

        free(szUSN);

        return TRUE;
    }
    else
        return FALSE;
} // end FRegisterDevice


//////////
// FRegisterService
//      Register the service with SSDP, and register the EventSource.
//
// PARAMETERS
//      pDev:          Pointer to the device to publish.
//      pSvc           Pointer to service control block.
//      szDescDocUrl:  The URL to this device's description document.
//
// RETURNS
//      BOOL    TRUE if the service was published.
//              FALSE if the function failed to publish the service.
//
BOOL FRegisterService(UPNPDEV* pDev, UPNPSVC* pSvc, LPSTR szDescDocUrl)
{
    CHAR   szUSN[512];
    
    sprintf(szUSN, "%s::%s", pDev->szUdn, pSvc->szServiceType);
    
    SsdpAddMessage(szDescDocUrl, pSvc->szServiceType, szUSN);
    
    // Register this service's event source with SSDP.
    if (!FRegisterEvent(pSvc))
    {
        return FALSE;
    }    
    
    return TRUE;
} // end FRegisterService


//////////
// FRegisterEvent
//      Calls the device initialization code to have it register its
//      state table with SSDP.
//
// PARAMETERS
//      pSvc        Pointer to service control block.
//
// RETURNS
//      BOOL        TRUE if the state table was registered.
//                  FALSE if the function failed to register its state table.
//
BOOL FRegisterEvent(UPNPSVC* pSvc)
{
    DWORD dwResult = pSvc->pSvcControl->pfnActionInit(pSvc->szEvtUrl);
    
    if (!dwResult)
    {
        puts("Failed to initialize the device.");
        return FALSE;
    }
    
    return TRUE;
} // end FRegisterEvent


//////////
// FUnregisterEvent
//      Calls the device cleanup code to have it unregister its
//      state table with SSDP, and do any other cleanup it needs to do.
//
// PARAMETERS
//      pSvc    Pointer to service control block.
//
// RETURNS
//      BOOL    TRUE if the cleanup was successful.
//              FALSE if the cleanup function returned an error.
//
BOOL FUnregisterEvent(UPNPSVC* pSvc)
{
    DWORD dwResult = pSvc->pSvcControl->pfnActionCleanup(pSvc->szEvtUrl);
    
    if (!dwResult)
    {
        puts("Failed to cleanup the device.");
        return FALSE;
    }
    
    return TRUE;
} // end FUnregisterEvent


//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1997-2001.
//
//  File:       U D E V I C E . C P P
//
//  Contents:   Creates a UPnP Root Device Data Sturcture from the device
//              description document.
//
//----------------------------------------------------------------------------

#include "stdafx.h"
//#include <windows.h>
//#include <stdlib.h>
//#include <stdio.h>
#include "global.h"
#include "util.h"
#include "udevice.h"


#include "xmlif.h"

#define celems(_x) (sizeof(_x) / sizeof(_x[0])) 

// Private support functions.
static BOOL ParseServiceElement (HELEMENT hSvc, UPNPSVC*);
static UPNPDEV* ParseDeviceElement(HELEMENT hDevice);
static VOID FixUpURLs(UPNPDEV* pDev);
static BOOL AttachServiceControl(UPNPSVC* pSvc);

// Service ID prefix.
const CHAR c_szSIDprefix[] = "urn:upnp-org:serviceId:";


//////////
// FCreateRootDeviceStruct
//      Create a UPnP root device structure from the description document
//
// PARAMETERS
//      szDevDescription:   Description document to parse.
//      pRoot:              On output, returns a pointer to the new root device structure.
// 
// RETURNS: 
//      BOOL                TRUE if the structure was created.
//                          FALSE if the creation failed.
//
BOOL FCreateRootDeviceStruct (CHAR* szDevDescription, UPNPDEV** pRoot ) 
{
    DWORD   rc;
    UINT    majorVer = 0;
    UINT    minorVer = 0;
    UPNPDEV* pDev = NULL;
    HELEMENT hRoot = NULL;
    HELEMENT hChild;
    HELEMENT hElem;
    LPCSTR  pszTemp;
    LPCSTR  pszURLBase = NULL;
    
    // Parse the description document.
    rc = XMLParse(szDevDescription, &hRoot);
    
    if (rc == 0)
    {
        // Parse command succeeded.

        // Get the first child element.
        hElem = XMLGetFirstChild(hRoot);
        while (hElem)
        {
            // If the element name is the correct version, get the first child.
            if (strcmp(XMLGetElementName(hElem), "specVersion") == 0)
            {
                hChild = XMLGetFirstChild(hElem);
                // While we have a child, loop.
                while (hChild) 
                {
                    PUINT puVer = NULL;
                    if (strcmp(XMLGetElementName(hChild), "major") == 0)
                    {
                        puVer = &majorVer;
                    }
                    else if (strcmp(XMLGetElementName(hChild), "minor") == 0)
                    {
                        puVer = &minorVer;
                    }
                    // Get the value of the element.
                    pszTemp = XMLGetElementValue(hChild);
                    if (pszTemp && puVer)
                        *puVer = atoi(pszTemp);
                
                    // Get the next sibling to continue loop.
                    hChild = XMLGetNextSibling(hChild);
                }
            
            }
            else if (strcmp(XMLGetElementName(hElem), "URLBase") == 0)
            {
                // If the element name is the correct URL base, get the element value.
                pszURLBase = XMLGetElementValue(hElem);
            }
            else if (strcmp(XMLGetElementName(hElem), "device") == 0)
            {
                // If the element name is the device name, parse an device element.
                pDev = ParseDeviceElement(hElem);
                break;
            }
        
            // Process next top level element.
            hElem = XMLGetNextSibling(hElem);
        }
    }
    
    // Point to the root node.
    if (majorVer == 1 && pDev )
    {
        
        pDev->fRoot = TRUE;
        
        if (pszURLBase)
        {
            FixUpURLs(pDev);
        }
        *pRoot = pDev;
        return TRUE;
    }
    
    // MemFree the root node.
    if (hRoot)
        XMLFree(hRoot);
    
    *pRoot = NULL;
    if (pDev)
        FDeleteRootDeviceStruct(pDev);
    
    return FALSE;
    
} // end FCreateRootDeviceStruct


//////////
// ParseDeviceElement
//      Obtain a device element from a previously parsed XML document. 
//
// PARAMETERS
//      hDevice:    Handle to the XML element that describes
//                  the device.
// 
// RETURNS: 
//      UPNPDEV*    Pointer to the device, structure if successful.
//                  NULL if not successful.
//
UPNPDEV* ParseDeviceElement(HELEMENT hDevice)
{
    UPNPDEV* pDev = new UPNPDEV;
    UPNPDEV* pDevChild = pDev;
    HELEMENT hElem, hChild;
    LPCSTR pszPresentationURL;
    DWORD i;
    
    // If it's not a device, return NULL.
    if (!pDev)
        return NULL;
    ZeroMemory(pDev, sizeof(UPNPDEV));
    
    // Get the first child.
    hElem = XMLGetFirstChild(hDevice);
    
    // While still looking at a child element, loop.
    while (hElem)
    {
        // If the element is a name, write the element value to shared memory.
        if (strcmp(XMLGetElementName(hElem), "deviceType") == 0)
        {
            strcpy(pDev->szDeviceType, XMLGetElementValue(hElem));
        }
        else if(strcmp(XMLGetElementName(hElem), "UDN") == 0)
        {
            // If the element name is an UDN, write the UDN to shared memory.
            strcpy(pDev->szUdn, XMLGetElementValue(hElem));
        }
        else if(strcmp(XMLGetElementName(hElem), "serviceList") == 0)
        {
            // If the element name is serviceList, loop through each service 
            // and write the list to shared memory.
            hChild = XMLGetFirstChild(hElem);
            for (i = 0; i < MAX_SVC && hChild; i++)
            {
                ParseServiceElement(hChild, &pDev->rgSvc[i]);
                hChild = XMLGetNextSibling(hChild);
            }
            pDev->cSvcs = i;
        }
        else if(strcmp(XMLGetElementName(hElem), "deviceList") == 0)
        {
            // Else if it's a device list, walk through the device list.
            hChild = XMLGetFirstChild(hElem);
            
            while(hChild && strcmp(XMLGetElementName(hChild), "device") == 0)
            {
                // Nested devices stored as a linklist.
                pDevChild->pNextDev = ParseDeviceElement(hChild);
                // WARNING: Does not handle more than one level of nested devices,
                // so if the child device has children, we exit.
                if (!pDevChild->pNextDev || pDevChild->pNextDev->pNextDev)
                {
                    goto cleanup;
                }
                pDevChild = pDevChild->pNextDev;
                
                hChild = XMLGetNextSibling(hChild);
            }
        }
        else if(strcmp(XMLGetElementName(hElem), "presentationURL") == 0)
        {
            // If the element is the presentation URL, save it.
            pszPresentationURL = XMLGetElementValue(hElem);
        }
        // Obtain the next sibling to walk through.
        hElem = XMLGetNextSibling(hElem);
    }
    
    // If we obtained the device, return it.
    if (pDev && pDev->cSvcs && pDev->szUdn && pDev->szDeviceType )
    {
        return pDev;
    }
    
cleanup:
    
    // Otherwise, remove the partial structure and return NULL.
    FDeleteRootDeviceStruct(pDev);
    
    return NULL;
} // end ParseDeviceElement


//////////
// FDeleteRootDeviceStruct
//      Deletes a UPnP root device structure from memory.
//      Also deletes the child devices.  This function should 
//      only be called after the structure has been unpublished 
//      using FUnPublishRootDevice.
//
// PARAMETERS
//      pRoot:      Pointer to root device structure.
// 
// RETURNS: 
//      BOOL        TRUE.
//
BOOL FDeleteRootDeviceStruct (UPNPDEV* pRoot) 
{
    
    UPNPDEV* pDev = pRoot;
    UPNPDEV* pLast = NULL;
    
    // Walk through the linked list and remove each element.
    while (pDev) 
    {
        pLast = pDev;
        pDev = pDev->pNextDev;
        delete pLast;
    }
    
    return TRUE;
    
} // end FDeleteRootDeviceStruct


//////////
// ParseServiceElement
//      Parse a service element from the device description. 
//
// PARAMETERS
//      hSvc:   Handle to the (parsed) service XML element.
//      pSvc:   Pointer to the service structure to be filled.
// 
// RETURNS: 
//      BOOL    TRUE if the action succeeded.
//              FALSE if the action did not succeed.
//
static BOOL ParseServiceElement (HELEMENT hSvc, UPNPSVC* pSvc) 
{
    HELEMENT hChild;
    LPCSTR pszServiceType = NULL; 
    LPCSTR pszServiceId = NULL; 
    LPCSTR pszSCPD = NULL; 
    LPCSTR pszControl = NULL;
    LPCSTR pszEventSub = NULL;
    BOOL    bReturn = TRUE;
    
    // Get the first child.
    hChild = XMLGetFirstChild(hSvc);
    
    // While the current element is still a child, loop.
    while(hChild)
    {
        // If the element is a service type, get the value and store it.
        if (strcmp(XMLGetElementName(hChild), "serviceType") == 0)
        {
            pszServiceType = XMLGetElementValue(hChild);    
        }
        else if (strcmp(XMLGetElementName(hChild), "serviceId") == 0)
        {
            // If it's a service ID, get the value and store it.
            pszServiceId = XMLGetElementValue(hChild);    
        }
        else if (strcmp(XMLGetElementName(hChild), "SCPDURL") == 0)
        {
            // If it's an SCPDURL, get the value and store it.
            pszSCPD = XMLGetElementValue(hChild);
        }
        else if (strcmp(XMLGetElementName(hChild), "controlURL") == 0)
        {
            // If it's a control URL, get the value and store it.
            pszControl = XMLGetElementValue(hChild);
        }
        else if (strcmp(XMLGetElementName(hChild), "eventSubURL") == 0)
        {
            // If it's an event SubURL, get the value and store it.
            pszEventSub = XMLGetElementValue(hChild);
        }
        
        // Get the next sibling to process.
        hChild = XMLGetNextSibling(hChild);
    }
    
    // If we got all the necessary information, write it to service control structure.
    if (pszServiceType && pszServiceId && pszSCPD && pszControl && pszEventSub)
    {
        if (pSvc)
        {
            ZeroMemory(pSvc, sizeof(UPNPSVC));
            strcpy(pSvc->szServiceType, pszServiceType);
            
            // Skip the "urn:upnp-org:serviceId:" prefix if present.
            if (memcmp(pszServiceId, c_szSIDprefix, 
                        sizeof(c_szSIDprefix)-sizeof(CHAR)) == 0)
            {
                pszServiceId += celems(c_szSIDprefix) - 1;
            }
            strcpy(pSvc->szServiceId, pszServiceId);
            
            // Get the control URL and query.
            strcpy(pSvc->szControlUrl, pszControl);
            LPSTR pszQuery = strchr(pszControl, '?');
            if (pszQuery != NULL)
            {
                strcpy(pSvc->szControlQuery, pszQuery + 1);
            }
            
            // Get the event URL and query.
            strcpy(pSvc->szEvtUrl, pszEventSub);
            pszQuery = strchr(pszEventSub, '?');
            if (pszQuery != NULL)
            {
                strcpy(pSvc->szEvtQuery, pszQuery + 1);
            }
            // Use this service's contol ID to attach a set of 
            // device-specific actions for this service.



            if (!AttachServiceControl(pSvc)) 
            {
                puts("Error, failed to attach service control.");
                bReturn = FALSE;
            }

        }
    }

    return bReturn;
} // end ParseServiceElement


//////////
// MakeRelativeURL
//      Converts szUrl (an absolute URL) to a relative URL by
//      removing 'http://hostname:port'.
//
// PARAMETERS
//      pszUrl:  On input the absolute URL to convert. 
//               On output, points to the relative URL.
// 
// RETURNS: 
//      Nothing.
//
static VOID MakeRelativeURL(PCHAR pszUrl)
{
    CHAR*  pc = pszUrl;
    
    while (*pc)
    {
        if (*pc == '/' && *++pc == '/')
        {
            // Found two slashes.
            ++pc;
            break;
        }
        pc++;
    }
    
    while (*pc && *pc != '/')
    {
        // Advance to the next slash.
        pc++;
    }
    
    // Move the relative URL to pszUrl.
    if (*pc == '/')
    {
        memmove(pszUrl, pc, (strlen(pc) + 1)*sizeof(CHAR));
    }
} // end MakeRelativeURL


//////////
// FixUpURLs
//      Ensures that URLs are properly formatted.  Converts 
//      the absolute URL for each service of specified device
//      into a relative URL.
//
// PARAMETERS
//      pDev:       Pointer to the device.
// 
// RETURNS: 
//      Nothing.
//
static VOID FixUpURLs(UPNPDEV* pDev)
{
    DWORD i;
    UPNPSVC* pSvc;
    
    // Turn all absolute URLs to relative URLs.
    while (pDev)
    {
        for (i = 0; i < pDev->cSvcs && (pSvc = &pDev->rgSvc[i]); i++)
        {
            MakeRelativeURL(pSvc->szEvtUrl);    
        }
        pDev = pDev->pNextDev;
    }
} // end FixUpURLs




//////////
// AttachServiceControl
//      Attaches a service control pointer (pSvc->pSvcContol) to 
//      the correct service control table entry (c_crSvc[]).
//      The service IDs must match.
//
// PARAMETERS
//      pSvc:   Service control structure to attach.
// 
// RETURNS: 
//      BOOL    TRUE if successful.
//              FALSE if an error was detected.
//
BOOL AttachServiceControl(UPNPSVC* pSvc) 
{
    
    // Make sure that there is a control ID in the service node.
    if (!pSvc->szServiceId) 
    {
        puts("Error, no service control ID found.");
        return FALSE;
    }
    
    // Look for the service control ID in the table.
    for (DWORD isvc = 0; isvc < c_cDemoSvc; isvc++) 
    {
        
        // Look for a match.
        if (!_stricmp(c_rgSvc[isvc].szServiceId, pSvc->szServiceId)) 
        {
            
            // A match was found, so set the pointer and return TRUE.
            pSvc->pSvcControl = &c_rgSvc[isvc];      
            return TRUE;
        }
    }
    
    // Failed to find a match in the table.
    printf("Error, could not find %s in the service control table\n",
                   pSvc->szServiceId);
    return FALSE;
} // end AttachServiceControl


DWORD Do_PwrDim_Init    (CHAR* StrEventUrl)
{
	return 1;
}

DWORD Do_PwrDim_Cleanup (CHAR* StrEventUrl)
{
	return 1;
}

DWORD Do_SetPowerOn (
                         CHAR*           StrEventUrl,
                         DWORD           cArgs,       
                         ARG*            rgArgs,      
                         PDWORD          pArgsOut,      
                         ARG_OUT*        rgArgsOut   
                     )
{
	return 0;
}

DWORD Do_SetPowerOff ( 
                         CHAR*           StrEventUrl,
                         DWORD           cArgs,       
                         ARG*            rgArgs,      
                         PDWORD          pArgsOut,      
                         ARG_OUT*        rgArgsOut   
                      )
{
	return 0;
}

DWORD Do_IncreaseLevel (
                         CHAR*           StrEventUrl,
                         DWORD           cArgs,       
                         ARG*            rgArgs,      
                         PDWORD          pArgsOut,      
                         ARG_OUT*        rgArgsOut   
                        )
{
	return 0;
}

DWORD Do_DecreaseLevel (  
                         CHAR*           StrEventUrl,
                         DWORD           cArgs,       
                         ARG*            rgArgs,      
                         PDWORD          pArgsOut,      
                         ARG_OUT*        rgArgsOut   
                        )
{
	return 1;
}



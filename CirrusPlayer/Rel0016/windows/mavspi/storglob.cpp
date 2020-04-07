//****************************************************************************
//
// STORGLOB.CPP - The CMDSPStorageGlobals class, which implements the
//                IMDSPStorageGlobals interface.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
// Copyright (C) Microsoft Corporation, 1999 - 2001.  All rights reserved.
//
//****************************************************************************
#include "stdafx.h"

//****************************************************************************
//
// Specifies the drive which this object represents.
//
//****************************************************************************
void
CMDSPStorageGlobals::SetDrive(unsigned long ulDrive)
{
    //
    // Remember the drive.
    //
    m_ulDrive = ulDrive;
}

//****************************************************************************
//
// The IMDSPStorageGlobals::GetCapabilities method, which returns the
// capabilities of the storage medium.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorageGlobals::GetCapabilities(DWORD *pdwCapabilities)
{
    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(S_FALSE);
    }

    //
    // If we are not authenticated, then return a failure.
    //
    if(!(g_pAppSCServer->fIsAuthenticated()))
    {
        return(WMDM_E_NOTCERTIFIED);
    }

    //
    // If we were passed a NULL pointer, then return a failure.
    //
    if(!pdwCapabilities)
    {
        return(E_INVALIDARG);
    }

    //
    // Return the capabilites of this storage medium.
    //
    *pdwCapabilities = WMDM_STORAGECAP_FILESINROOT |
                       WMDM_STORAGECAP_FILELIMITEXISTS;

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// The IMDSPStorageGlobals::GetDevice method, which returns the device object
// with which this storage medium is associated.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorageGlobals::GetDevice(IMDSPDevice **ppDevice)
{
    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(S_FALSE);
    }

    //
    // If we are not authenticated, then return a failure.
    //
    if(!(g_pAppSCServer->fIsAuthenticated()))
    {
        return(WMDM_E_NOTCERTIFIED);
    }

    //
    // If we were passed a NULL pointer, then return a failure.
    //
    if(!ppDevice)
    {
        return(E_INVALIDARG);
    }

    //
    // Get the device object for the device.
    //
    *ppDevice = g_MaverickInfo.GetDevice(m_ulDrive);
    if(!*ppDevice)
    {
        return(E_FAIL);
    }

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// The IMDSPStorageGlobals::GetRootStorage method, which returns the root
// storage object for this storage medium.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorageGlobals::GetRootStorage(IMDSPStorage **ppRoot)
{
    CComObject<CMDSPStorage> *pObj;
    WCHAR pwcName[256];
    HRESULT hr;

    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(S_FALSE);
    }

    //
    // If we are not authenticated, then return a failure.
    //
    if(!(g_pAppSCServer->fIsAuthenticated()))
    {
        return(WMDM_E_NOTCERTIFIED);
    }

    //
    // If we were passed a NULL pointer, then return a failure.
    //
    if(!ppRoot)
    {
        return(E_INVALIDARG);
    }

    //
    // Create a new CMDSPStorage object.
    //
    hr = CComObject<CMDSPStorage>::CreateInstance(&pObj);
    if(FAILED(hr))
    {
        return(hr);
    }

    //
    // Get a pointer to the IMDSPStorage interface for the object.
    //
    hr = pObj->QueryInterface(IID_IMDSPStorage,
                              reinterpret_cast<void **>(ppRoot));
    if(FAILED(hr))
    {
        delete pObj;
        return(hr);
    }

    //
    // Get the name of this storage object.
    //
    g_MaverickInfo.GetDriveName(m_ulDrive, pwcName, 256);

    //
    // Initialize the storage object.
    //
    pObj->SetStorageInfo(m_ulDrive, pwcName, TRUE, 0, 0);

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// The IMDSPStorageGlobals::GetSerialNumber method, which returns the serial
// number that uniquely identifies this storage medium.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorageGlobals::GetSerialNumber(PWMDMID pSerialNum,
                                     BYTE abMac[WMDM_MAC_LENGTH])
{
    HRESULT hr;
    HMAC hMAC;

    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(S_FALSE);
    }

    //
    // If we are not authenticated, then return a failure.
    //
    if(!(g_pAppSCServer->fIsAuthenticated()))
    {
        return(WMDM_E_NOTCERTIFIED);
    }

    //
    // If we were passed a NULL pointer, then return a failure.
    //
    if(!pSerialNum)
    {
        return(E_INVALIDARG);
    }

    //
    // Get the device serial number.
    //
    g_MaverickInfo.GetSerialNumber(m_ulDrive, pSerialNum);
    if(pSerialNum->cbSize == 0)
    {
        return(WMDM_E_NOTSUPPORTED);
    }

    //
    // Acquire a MAC channel.
    //
    hr = g_pAppSCServer->MACInit(&hMAC);
    if(FAILED(hr))
    {
        return(hr);
    }

    //
    // Update the MAC value with the data buffer contents.
    //
    hr = g_pAppSCServer->MACUpdate(hMAC, (BYTE *)pSerialNum, sizeof(WMDMID));
    if(FAILED(hr))
    {
        return(hr);
    }

    //
    // Release the MAC channel and retrieve the final MAC value.
    //
    hr = g_pAppSCServer->MACFinal(hMAC, abMac);
    if(FAILED(hr))
    {
        return(hr);
    }

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// The IMDSPStorageGlobals::GetStatus method, which returns the current status
// of the storage medium.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorageGlobals::GetStatus(DWORD *pdwStatus)
{
    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(S_FALSE);
    }

    //
    // If we are not authenticated, then return a failure.
    //
    if(!(g_pAppSCServer->fIsAuthenticated()))
    {
        return(WMDM_E_NOTCERTIFIED);
    }

    //
    // If we were passed a NULL pointer, then return a failure.
    //
    if(!pdwStatus)
    {
        return(E_INVALIDARG);
    }

    //
    // Get the current device status.
    //
    *pdwStatus = WMDM_STATUS_READY;

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// The IMDSPStorageGlobals::GetTotalBad method, which returns the amount of
// unusable space on the storage medium.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorageGlobals::GetTotalBad(DWORD *pdwBadLow, DWORD *pdwBadHigh)
{
    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(S_FALSE);
    }

    //
    // If we are not authenticated, then return a failure.
    //
    if(!(g_pAppSCServer->fIsAuthenticated()))
    {
        return(WMDM_E_NOTCERTIFIED);
    }

    //
    // If we were passed a NULL pointer, then return a failure.
    //
    if(!pdwBadLow)
    {
        return(E_INVALIDARG);
    }

    //
    // We do not support reporting the number of bad blocks.
    //
    return(WMDM_E_NOTSUPPORTED);
}

//****************************************************************************
//
// The IMDSPStorageGlobals::GetTotalFree method, which returns the amount of
// available space on the storage medium.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorageGlobals::GetTotalFree(DWORD *pdwFreeLow, DWORD *pdwFreeHigh)
{
    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(S_FALSE);
    }

    //
    // If we are not authenticated, then return a failure.
    //
    if(!(g_pAppSCServer->fIsAuthenticated()))
    {
        return(WMDM_E_NOTCERTIFIED);
    }

    //
    // If we were passed a NULL pointer, then return a failure.
    //
    if(!pdwFreeLow)
    {
        return(E_INVALIDARG);
    }

    //
    // Get the amount of free space on this storage medium.
    //
    *pdwFreeLow = Maverick_FreeSpace(m_ulDrive);
    if(pdwFreeHigh)
    {
        *pdwFreeHigh = 0;
    }

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// The IMDSPStorageGlobals::GetTotalSize method, which returns the total
// capacity of the storage medium.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorageGlobals::GetTotalSize(DWORD *pdwTotalSizeLow,
                                  DWORD *pdwTotalSizeHigh)
{
    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(S_FALSE);
    }

    //
    // If we are not authenticated, then return a failure.
    //
    if(!(g_pAppSCServer->fIsAuthenticated()))
    {
        return(WMDM_E_NOTCERTIFIED);
    }

    //
    // If we were passed a NULL pointer, then return a failure.
    //
    if(!pdwTotalSizeLow)
    {
        return(E_INVALIDARG);
    }

    //
    // Get the amount of free space on this storage medium.
    //
    *pdwTotalSizeLow = Maverick_TotalSpace(m_ulDrive);
    if(pdwTotalSizeHigh)
    {
        *pdwTotalSizeHigh = 0;
    }

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// The IMDSPStorageGlobals::Initialize method, which formats the storage
// medium.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorageGlobals::Initialize(UINT fuMode, IWMDMProgress *pProgress)
{
    //
    // If there is a progress object, then initialize it now.
    //
    if(pProgress)
    {
        //
        // Indicate that the format is beginning.
        //
        pProgress->Begin(100);
    }

    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        if(pProgress)
        {
            pProgress->Progress(100);
            pProgress->End();
        }
        return(S_FALSE);
    }

    //
    // If we are not authenticated, then return a failure.
    //
    if(!(g_pAppSCServer->fIsAuthenticated()))
    {
        if(pProgress)
        {
            pProgress->Progress(100);
            pProgress->End();
        }
        return(WMDM_E_NOTCERTIFIED);
    }

    //
    // Format the storage medium.
    //
    Maverick_Format(m_ulDrive);

    //
    // Refresh the Maverick info database.
    //
    g_MaverickInfo.Refresh(m_ulDrive);

    //
    // If there is a progress object, then indicate that the delete has
    // completed.
    //
    if(pProgress)
    {
        //
        // Set the progress to the end.
        //
        pProgress->Progress(100);

        //
        // Indicate that the delete is complete.
        //
        pProgress->End();
    }

    //
    // Success.
    //
    return(S_OK);
}

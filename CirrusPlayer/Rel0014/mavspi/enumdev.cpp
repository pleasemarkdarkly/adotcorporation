//****************************************************************************
//
// ENUMDEV.CPP - The CMDSPEnumDevice class, which implements the
//               IMDSPEnumDevice interface.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
// Copyright (C) Microsoft Corporation, 1999 - 2001.  All rights reserved.
//
//****************************************************************************
#include "stdafx.h"

//****************************************************************************
//
// The constructor for the CMDSPEnumDevice class.
//
//****************************************************************************
CMDSPEnumDevice::CMDSPEnumDevice(void)
{
    //
    // Indicate that we are at the beginning of the enumeration.
    //
    m_ulIndex = 0;
}

//****************************************************************************
//
// IMDSPEnumDevice::Clone creates another enumerator with the same enumeration
// state as this object.
//
//****************************************************************************
STDMETHODIMP
CMDSPEnumDevice::Clone(IMDSPEnumDevice **ppEnumDevice)
{
    CComObject<CMDSPEnumDevice> *pEnumObj;
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
    if(!ppEnumDevice)
    {
        return(S_FALSE);
    }

    //
    // Create a new device enumerator object.
    //
    hr = CComObject<CMDSPEnumDevice>::CreateInstance(&pEnumObj);
    if(FAILED(hr))
    {
        return(hr);
    }

    //
    // Get a pointer to the IMDSPEnumDevice interface for the object.
    //
    hr = pEnumObj->QueryInterface(IID_IMDSPEnumDevice,
                                  reinterpret_cast<void **>(ppEnumDevice));
    if(FAILED(hr))
    {
        delete pEnumObj;
        return(hr);
    }

    //
    // Set the state of the new enumerator to match our state.
    //
    pEnumObj->m_ulIndex = m_ulIndex;

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// IMDSPEnumDevice::Next returns the specified number of devices in the
// enumeration sequence.
//
//****************************************************************************
STDMETHODIMP
CMDSPEnumDevice::Next(ULONG celt, IMDSPDevice **ppDevice, ULONG *pceltFetched)
{
    unsigned long ulIndex, ulNumItems;

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
    if(!ppDevice || !pceltFetched)
    {
        return(S_FALSE);
    }

    //
    // Set the number of returned devices to zero and the first device
    // interface pointer to NULL.
    //
    *pceltFetched = 0;
    *ppDevice = NULL;

    //
    // Determine the number of items in the enumeration.
    //
    ulNumItems = g_MaverickInfo.GetNumDrives();

    //
    // If we are at the end of the enumeration, then return a failure.
    //
    if(m_ulIndex == ulNumItems)
    {
        return(S_FALSE);
    }

    //
    // Loop through the number of requested items.
    //
    for(ulIndex = 0; (ulIndex < celt) && (m_ulIndex != ulNumItems); )
    {
        //
        // Get the device object for this device.
        //
        ppDevice[*pceltFetched] = g_MaverickInfo.GetDevice(m_ulIndex);
        if(!ppDevice[*pceltFetched])
        {
            return(S_FALSE);
        }

        //
        // Increment the count of objects fetched.
        //
        (*pceltFetched)++;
        ulIndex++;

        //
        // Increment the index into the enumeration.
        //
        m_ulIndex++;
    }

    //
    // Success.
    //
    return((*pceltFetched == celt) ? S_OK : S_FALSE);
}

//****************************************************************************
//
// IMDSPEnumDevice::Reset resets the enumeration sequence to the beginning.
//
//****************************************************************************
STDMETHODIMP
CMDSPEnumDevice::Reset()
{
    //
    // if we do not have a secure channel server object, then return a failure.
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
    // Reset our enumeration to the beginning.
    //
    m_ulIndex = 0;

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// IMDSPEnumDevice::Skip skips the specified number of devices in the
// enumeration sequence.
//
//****************************************************************************
STDMETHODIMP
CMDSPEnumDevice::Skip(ULONG celt, ULONG *pceltFetched)
{
    unsigned long ulNumItems;

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
    if(!pceltFetched)
    {
        return(S_FALSE);
    }

    //
    // Determine the number of items in the enumeration.
    //
    ulNumItems = g_MaverickInfo.GetNumDrives();

    //
    // See if there are more items remaining in the enumeration that were
    // requested.
    //
    if((ulNumItems - m_ulIndex) >= celt)
    {
        //
        // Set the number of items skipped to the number of requested items.
        //
        *pceltFetched = celt;

        //
        // Increment the enumeration index by the number of requested items.
        //
        m_ulIndex += celt;
    }
    else
    {
        //
        // Set the number of items skipped to the number of remaining items in
        // the enumeration.
        //
        *pceltFetched = ulNumItems - m_ulIndex;

        //
        // Set the enumeration index to the end of the list of items.
        //
        m_ulIndex = ulNumItems;
    }

    //
    // Success.
    //
    return((*pceltFetched == celt) ? S_OK : S_FALSE);
}

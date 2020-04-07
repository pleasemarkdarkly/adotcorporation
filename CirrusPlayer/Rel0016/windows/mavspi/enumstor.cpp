//****************************************************************************
//
// ENUMSTOR.CPP - The CMDSPEnumStorage class, which implements the
//                IMDSPEnumStorage interface.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
// Copyright (C) Microsoft Corporation, 1999 - 2001.  All rights reserved.
//
//****************************************************************************
#include "stdafx.h"

//****************************************************************************
//
// The constructor for the CMDSPEnumStorage class.
//
//****************************************************************************
CMDSPEnumStorage::CMDSPEnumStorage(void)
{
    //
    // Indicate that we are at the beginning of the enumeration.
    //
    m_ulDrive = -1;
    m_bIsRoot = TRUE;
    m_ulIndex = 0;
}

//****************************************************************************
//
// Specifies the drive to be enumerated by this object.
//
//****************************************************************************
void
CMDSPEnumStorage::SetDrive(unsigned long ulDrive, unsigned long bIsRoot)
{
    //
    // Save the drive being enumerated.
    //
    m_ulDrive = ulDrive;
    m_bIsRoot = bIsRoot;
}

//****************************************************************************
//
// IMDSPEnumStorage::Clone creates another enumerator with the same
// enumeration state as this object.
//
//****************************************************************************
STDMETHODIMP
CMDSPEnumStorage::Clone(IMDSPEnumStorage **ppEnumStorage)
{
    CComObject<CMDSPEnumStorage> *pEnumObj;
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
    if(!ppEnumStorage)
    {
        return(S_FALSE);
    }

    //
    // Create a new storage enumerator object.
    //
    hr = CComObject<CMDSPEnumStorage>::CreateInstance(&pEnumObj);
    if(FAILED(hr))
    {
        return(hr);
    }

    //
    // Get a pointer to the IMDSPEnumStorage interface for the object.
    //
    hr = pEnumObj->QueryInterface(IID_IMDSPEnumStorage,
                                  reinterpret_cast<void **>(ppEnumStorage));
    if(FAILED(hr))
    {
        delete pEnumObj;
        return(hr);
    }

    //
    // Set the state of the new enumerator to match our state.
    //
    pEnumObj->m_ulDrive = m_ulDrive;
    pEnumObj->m_bIsRoot = m_bIsRoot;
    pEnumObj->m_ulIndex = m_ulIndex;

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// IMDSPEnumStorage::Next returns the specified number of storage objects in
// the enumeration sequence.
//
//****************************************************************************
STDMETHODIMP
CMDSPEnumStorage::Next(ULONG celt, IMDSPStorage **ppStorage,
                       ULONG *pceltFetched)
{
    CComObject<CMDSPStorage> *pStg;
    unsigned long ulIndex, ulNumItems;
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
    if(!ppStorage || !pceltFetched)
    {
        return(S_FALSE);
    }

    //
    // Set the count of returned objects to zero and the first storage
    // interface pointer to NULL.
    //
    *pceltFetched = 0;
    *ppStorage = NULL;

    //
    // Determine the number of items in the enumeration.
    //
    if(m_bIsRoot)
    {
        ulNumItems = 1;
    }
    else
    {
        ulNumItems = g_MaverickInfo.GetNumFiles(m_ulDrive);
    }

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
        // Create a new CMDSPStorage object.
        //
        hr = CComObject<CMDSPStorage>::CreateInstance(&pStg);
        if(FAILED(hr))
        {
            break;
        }

        //
        // Get a pointer to the IMDSPStorage interface for the object.
        //
        hr = pStg->QueryInterface(IID_IMDSPStorage,
            reinterpret_cast<void **>(&(ppStorage[*pceltFetched])));
        if(FAILED(hr))
        {
            delete pStg;
            break;
        }

        //
        // Get the name of this storage object.
        //
        if(m_bIsRoot)
        {
            g_MaverickInfo.GetDriveName(m_ulIndex, pwcName, 256);
        }
        else
        {
            g_MaverickInfo.GetFileName(m_ulDrive, m_ulIndex, pwcName, 256);
        }

        //
        // Initialize the storage object.
        //
        if(m_bIsRoot)
        {
            pStg->SetStorageInfo(m_ulDrive, pwcName, TRUE, 0, 0);
        }
        else
        {
            pStg->SetStorageInfo(m_ulDrive, pwcName, FALSE,
                                 g_MaverickInfo.GetFileLength(m_ulDrive,
                                                              m_ulIndex),
                                 g_MaverickInfo.GetFileDate(m_ulDrive,
                                                            m_ulIndex));
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
    // If the final result is success, but the number of fetched items is less
    // than the number of requested items, then set the return code to a
    // failure.
    //
    if(SUCCEEDED(hr) && (*pceltFetched < celt))
    {
        hr = S_FALSE;
    }

    //
    // Return the result.
    //
    return(hr);
}

//****************************************************************************
//
// IMDSPEnumStorage::Reset resets the enumeration sequence to the beginning.
//
//****************************************************************************
STDMETHODIMP
CMDSPEnumStorage::Reset(void)
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
    // Reset the index to zero.
    //
    m_ulIndex = 0;

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// IMDSPEnumStorage::Skip skips the specified number of storage objects in the
// enumeration sequence.
//
//****************************************************************************
STDMETHODIMP
CMDSPEnumStorage::Skip(ULONG celt, ULONG *pceltFetched)
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
    if(m_bIsRoot)
    {
        ulNumItems = 1;
    }
    else
    {
        ulNumItems = g_MaverickInfo.GetNumFiles(m_ulDrive);
    }

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

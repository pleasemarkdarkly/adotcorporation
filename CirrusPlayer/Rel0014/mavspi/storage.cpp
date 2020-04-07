//****************************************************************************
//
// STORAGE.CPP - The CMDSPStorage class, which implements the IMDSPStorage,
//               IMDSPObjectInfo, and IMDSPObject interfaces.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
// Copyright (C) Microsoft Corporation, 1999 - 2001.  All rights reserved.
//
//****************************************************************************
#include "stdafx.h"

//****************************************************************************
//
// The constructor for the CMDSPStorage class.
//
//****************************************************************************
CMDSPStorage::CMDSPStorage(void)
{
    //
    // Make sure that the name pointer is NULL.
    //
    m_pwcName = NULL;
}

//****************************************************************************
//
// The destructor for the CMDSPStorage class.
//
//****************************************************************************
CMDSPStorage::~CMDSPStorage(void)
{
    //
    // If this storage object is opened, then close it now.
    //
    if(m_bIsOpen)
    {
        Maverick_Close();
    }

    //
    // If we allocated memory for the name, then free it now.
    //
    if(m_pwcName)
    {
        delete [] m_pwcName;
    }
}

//****************************************************************************
//
// Initializes the information about this storage object.
//
//****************************************************************************
void
CMDSPStorage::SetStorageInfo(unsigned long ulDrive, WCHAR *pcName,
                             unsigned long bIsRoot, unsigned long ulSize,
                             unsigned long ulDate)
{
    //
    // Save the drive.
    //
    m_ulDrive = ulDrive;

    //
    // Allocate memory to hold the name.
    //
    m_pwcName = new WCHAR[wcslen(pcName) + 1];
    if(!m_pwcName)
    {
        return;
    }

    //
    // Copy the name.
    //
    wcscpy(m_pwcName, pcName);

    //
    // Save the root storage object indicator.
    //
    m_bIsRoot = bIsRoot;

    //
    // Save the size.
    //
    m_ulSize = ulSize;

    //
    // Save the date.
    //
    m_ulDate = ulDate;

    //
    // All storage objects are closed by default.
    //
    m_bIsOpen = FALSE;
}

//****************************************************************************
//
// The IMDSPStorage::CreateStorage method, which creates a new storage at the
// level of this storage object.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorage::CreateStorage(DWORD dwAttributes, _WAVEFORMATEX *pFormat,
                            LPWSTR pwszName, IMDSPStorage **ppNewStorage)
{
    CComObject<CMDSPStorage> *pObj;
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
    if(!pwszName || !ppNewStorage)
    {
        return(E_INVALIDARG);
    }

    //
    // If this storage object has been deleted, then return a failure.
    //
    if(!m_pwcName[0])
    {
        return(WMDM_E_INTERFACEDEAD);
    }

    //
    // We only support:
    //   1) Inserting a new storage object into the root storage object, or
    //   2) Inserting a new storage object before or after a non-root storage
    //      object,
    // so if either of these two cases are not requested, then return a
    // failure.
    //
    if((m_bIsRoot && (dwAttributes & (WMDM_STORAGECONTROL_INSERTBEFORE |
                                      WMDM_STORAGECONTROL_INSERTAFTER))) ||
       (!m_bIsRoot && (dwAttributes & WMDM_STORAGECONTROL_INSERTINTO)))
    {
        return(WMDM_E_NOTSUPPORTED);
    }

    //
    // Create a new storage object.
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
                              reinterpret_cast<void **>(ppNewStorage));
    if(FAILED(hr))
    {
        delete pObj;
        return(hr);
    }

    //
    // Initialize the storage object.
    //
    pObj->SetStorageInfo(m_ulDrive, pwszName, FALSE, 0, 0);

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// The IMDSPStorage::EnumStorage method, which returns an enumerator for the
// storage objects contained in this storage object.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorage::EnumStorage(IMDSPEnumStorage **ppEnumStorage)
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
        return(E_INVALIDARG);
    }

    //
    // If this storage object is a file, then there are no storage objects to
    // be enumerated.
    //
    if(!m_bIsRoot)
    {
        return(WMDM_E_NOTSUPPORTED);
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
    // Specify the drive to be enumerated by the new object.
    //
    pEnumObj->SetDrive(m_ulDrive, FALSE);

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// The IMDSPStorage::GetAttributes method, which returns the attributes of
// this storage object.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorage::GetAttributes(DWORD *pdwAttributes, _WAVEFORMATEX *pFormat)
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
    if(!pdwAttributes || !pFormat)
    {
        return(E_INVALIDARG);
    }

    //
    // If this storage object has been deleted, then return a failure.
    //
    if(!m_pwcName[0])
    {
        return(WMDM_E_INTERFACEDEAD);
    }

    //
    // See if we are a drive or a file.
    //
    if(m_bIsRoot)
    {
        //
        // We are a drive, so return the base set of attributes.
        //
        *pdwAttributes = WMDM_STORAGE_ATTR_FILESYSTEM |
                         WMDM_FILE_ATTR_FOLDER | WMDM_FILE_ATTR_CANREAD;

        //
        // See if there are any files on this drive.
        //
        if(g_MaverickInfo.GetNumFiles(m_ulDrive) != 0)
        {
            //
            // Indicate that there are files on this drive.
            //
            *pdwAttributes |= WMDM_STORAGE_ATTR_HAS_FILES;
        }
    }
    else
    {
        //
        // We are a file, so return the file attributes.
        //
        *pdwAttributes = WMDM_FILE_ATTR_FILE | WMDM_FILE_ATTR_CANREAD |
                         WMDM_FILE_ATTR_AUDIO;
    }

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// The IMDSPStorage::GetDate method, which returns the date on which this
// storage object was most recently modified.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorage::GetDate(PWMDMDATETIME pDateTimeUTC)
{
    unsigned char ucMonthLength[12] = { 31, 29, 31, 30, 31, 30,
                                        31, 31, 30, 31, 30, 31 };
    TIME_ZONE_INFORMATION tzInfo;
    unsigned long ulSeconds;

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
    if(!pDateTimeUTC)
    {
        return(E_INVALIDARG);
    }

    //
    // If this storage object has been deleted, then return a failure.
    //
    if(!m_pwcName[0])
    {
        return(WMDM_E_INTERFACEDEAD);
    }

    //
    // Get the modification date/time of this storage object.
    //
    ulSeconds = m_ulDate;

    //
    // Get the current time zone information.
    //
    if(GetTimeZoneInformation(&tzInfo) != TIME_ZONE_ID_INVALID)
    {
        //
        // Add the time zone bias to the modification date/time.
        //
        ulSeconds += tzInfo.Bias * 60;
    }

    //
    // Extract the number of seconds from the time.
    //
    pDateTimeUTC->wSecond = (WORD)(ulSeconds % 60);
    ulSeconds /= 60;

    //
    // Extract the number of minutes from the time.
    //
    pDateTimeUTC->wMinute = (WORD)(ulSeconds % 60);
    ulSeconds /= 60;

    //
    // Extract the number of hours from the time.
    //
    pDateTimeUTC->wHour = (WORD)(ulSeconds % 24);
    ulSeconds /= 24;

    //
    // We now have days, so add the number of days between January 1, 1900 and
    // January 1, 1970.
    //
    ulSeconds += (70 * 365) + 18;

    //
    // Compute the number of years in terms of group of years from leap year to
    // leap year.
    //
    pDateTimeUTC->wYear = (WORD)(4 * (ulSeconds / ((365 * 4) + 1)));
    ulSeconds %= (365 * 4) + 1;

    //
    // If there are more than 365 days left in the current count of days, then
    // subtract the days from the remaining non-leap years.
    //
    if(ulSeconds >= 366)
    {
        pDateTimeUTC->wYear += (WORD)((ulSeconds - 1) / 365);
        ulSeconds = (ulSeconds - 1) % 365;
    }

    //
    // The year is now the number of years since 1900, so add 1900 so it is the
    // actual year.
    //
    pDateTimeUTC->wYear += 1900;

    //
    // If this is a non-leap year and the day is past February 28th, then
    // increment the count of days by one (i.e. act as if each year is a leap
    // year).
    //
    if(((pDateTimeUTC->wYear & 3) != 0) && (ulSeconds >= (31 + 28)))
    {
        ulSeconds++;
    }

    //
    // Subtract days for each month till we find the current month.
    //
    pDateTimeUTC->wMonth = 0;
    while(ulSeconds >= ucMonthLength[pDateTimeUTC->wMonth])
    {
        ulSeconds -= ucMonthLength[pDateTimeUTC->wMonth++];
    }

    //
    // The month is actually 1 indexed, so add one now.
    //
    pDateTimeUTC->wMonth++;

    //
    // Save the computed day.
    //
    pDateTimeUTC->wDay = (WORD)(ulSeconds + 1);
 
    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// The IMDSPStorage::GetName method, which returns the name of this storage
// object.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorage::GetName(LPWSTR pwszName, UINT nMaxChars)
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
    // If we were passed a NULL pointer, then return an error.
    //
    if(!pwszName)
    {
        return(E_INVALIDARG);
    }

    //
    // If this storage object has been deleted, then return a failure.
    //
    if(!m_pwcName[0])
    {
        return(WMDM_E_INTERFACEDEAD);
    }

    //
    // Make sure that the output buffer is large enough to hold the the name
    // of this storage object.
    //
    if(wcslen(m_pwcName) >= nMaxChars)
    {
        return(WMDM_E_BUFFERTOOSMALL);
    }

    //
    // Copy the name of this storage object into the output buffer.
    //
    wcscpy(pwszName, m_pwcName);

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// The IMDSPStorage::GetRights method, which returns the rights information
// for this storage object.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorage::GetRights(PWMDMRIGHTS *ppRights, UINT *pnRightsCount,
                        BYTE abMac[WMDM_MAC_LENGTH])
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
    // If this storage object has been deleted, then return a failure.
    //
    if(!m_pwcName[0])
    {
        return(WMDM_E_INTERFACEDEAD);
    }

    //
    // We do not support rights management (at this time).
    //
    return(WMDM_E_NOTSUPPORTED);
}

//****************************************************************************
//
// The IMDSPStorage::GetSize method, which returns the size of this storage
// object.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorage::GetSize(DWORD *pdwSizeLow, DWORD *pdwSizeHigh)
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
    if(!pdwSizeLow)
    {
        return(E_INVALIDARG);
    }

    //
    // If this storage object has been deleted, then return a failure.
    //
    if(!m_pwcName[0])
    {
        return(WMDM_E_INTERFACEDEAD);
    }

    //
    // Return the size of this storage object.
    //
    *pdwSizeLow = m_ulSize;
    if(pdwSizeHigh)
    {
        *pdwSizeHigh = 0;
    }

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// The IMDSPStorage::GetStorageGlobals method, which returns the
// IMDSPStorageGlobals interface which provides access to global information
// about the storage medium.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorage::GetStorageGlobals(IMDSPStorageGlobals **ppStorageGlobals)
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
    if(!ppStorageGlobals)
    {
        return(E_INVALIDARG);
    }

    //
    // If this storage object has been deleted, then return a failure.
    //
    if(!m_pwcName[0])
    {
        return(WMDM_E_INTERFACEDEAD);
    }

    //
    // Get the storage globals object for this drive/file.
    //
    *ppStorageGlobals = g_MaverickInfo.GetStorageGlobals(m_ulDrive);

    //
    // Success.
    //
    return(*ppStorageGlobals ? S_OK : E_FAIL);
}

//****************************************************************************
//
// The IMDSPStorage::SendOpaqueCommand method, which sends an unmodified
// command to this storage object.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorage::SendOpaqueCommand(OPAQUECOMMAND *pCommand)
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
    // If this storage object has been deleted, then return a failure.
    //
    if(!m_pwcName[0])
    {
        return(WMDM_E_INTERFACEDEAD);
    }

    //
    // We do not support any opaque commands at this time.
    //
    return(WMDM_E_NOTSUPPORTED);
}

//****************************************************************************
//
// The IMDSPStorage::SetAttribute method, which sets the attributes of this
// storage object.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorage::SetAttributes(DWORD dwAttributes, _WAVEFORMATEX *pFormat)
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
    if(pFormat)
    {
        return(E_INVALIDARG);
    }

    //
    // If this storage object has been deleted, then return a failure.
    //
    if(!m_pwcName[0])
    {
        return(WMDM_E_INTERFACEDEAD);
    }

    //
    // We do not allow the attributes of the files to be modified, so simply
    // return success.
    //
    return(S_OK);
}

//****************************************************************************
//
// The IMDSPObjectInfo::GetLastPlayPosition method, which returns the last
// play position of this storage object.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorage::GetLastPlayPosition(DWORD *pdwLastPos)
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
    if(!pdwLastPos)
    {
        return(E_INVALIDARG);
    }

    //
    // If this storage object has been deleted, then return a failure.
    //
    if(!m_pwcName[0])
    {
        return(WMDM_E_INTERFACEDEAD);
    }

    //
    // We do not support returning the last play position.
    //
    return(WMDM_E_NOTSUPPORTED);
}

//****************************************************************************
//
// The IMDSPObjectInfo::GetLongestPlayPosition method, which returns the
// longest play position of this storage object.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorage::GetLongestPlayPosition(DWORD *pdwLongestPos)
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
    if(!pdwLongestPos)
    {
        return(E_INVALIDARG);
    }

    //
    // If this storage object has been deleted, then return a failure.
    //
    if(!m_pwcName[0])
    {
        return(WMDM_E_INTERFACEDEAD);
    }

    //
    // We do not support returning the longest play position.
    //
    return(WMDM_E_NOTSUPPORTED);
}

//****************************************************************************
//
// The IMDSPObjectInfo::GetPlayLength method, which returns the play length of
// this storage object.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorage::GetPlayLength(DWORD *pdwLength)
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
    if(!pdwLength)
    {
        return(E_INVALIDARG);
    }

    //
    // If this storage object has been deleted, then return a failure.
    //
    if(!m_pwcName[0])
    {
        return(WMDM_E_INTERFACEDEAD);
    }

    //
    // We do not support returning the play length.
    //
    return(WMDM_E_NOTSUPPORTED);
}

//****************************************************************************
//
// The IMDSPObjectInfo::GetPlayOffset method, which returns the current play
// offset of this storage object.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorage::GetPlayOffset(DWORD *pdwOffset)
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
    if(!pdwOffset)
    {
        return(E_INVALIDARG);
    }

    //
    // If this storage object has been deleted, then return a failure.
    //
    if(!m_pwcName[0])
    {
        return(WMDM_E_INTERFACEDEAD);
    }

    //
    // We do not support returning the play offset.
    //
    return(WMDM_E_NOTSUPPORTED);
}

//****************************************************************************
//
// The IMDSPObjectInfo::GetTotalLength method, which returns the total play
// length of this storage object.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorage::GetTotalLength(DWORD *pdwLength)
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
    if(!pdwLength)
    {
        return(E_INVALIDARG);
    }

    //
    // If this storage object has been deleted, then return a failure.
    //
    if(!m_pwcName[0])
    {
        return(WMDM_E_INTERFACEDEAD);
    }

    //
    // We do not support returning the total length.
    //
    return(WMDM_E_NOTSUPPORTED);
}

//****************************************************************************
//
// The IMDSPObjectInfo::SetPlayLength method, which sets the play length of
// this storage object.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorage::SetPlayLength(DWORD dwLength)
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
    // If this storage object has been deleted, then return a failure.
    //
    if(!m_pwcName[0])
    {
        return(WMDM_E_INTERFACEDEAD);
    }

    //
    // We do not support setting the play length.
    //
    return(WMDM_E_NOTSUPPORTED);
}

//****************************************************************************
//
// The IMDSPObjectInfo::SetPlayOffset method, which sets the current play
// offset of this storage object.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorage::SetPlayOffset(DWORD dwOffset)
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
    // If this storage object has been deleted, then return a failure.
    //
    if(!m_pwcName[0])
    {
        return(WMDM_E_INTERFACEDEAD);
    }

    //
    // We do not support setting the play offset.
    //
    return(WMDM_E_NOTSUPPORTED);
}

//****************************************************************************
//
// The IMDSPObject::Close method, which closes this storage object.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorage::Close()
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
    // If this storage object has been deleted, then return a failure.
    //
    if(!m_pwcName[0])
    {
        return(WMDM_E_INTERFACEDEAD);
    }

    //
    // If this storage object is not opened, then return a failure.
    //
    if(!m_bIsOpen)
    {
        return(E_FAIL);
    }

    //
    // Close the file.
    //
    Maverick_Close();

    //
    // Indicate that this storage object is no longer opened.
    //
    m_bIsOpen = FALSE;

    //
    // Refresh the Maverick info database.
    //
    g_MaverickInfo.Refresh(m_ulDrive);

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// The IMDSPObject::Delete method, which deletes this storage object from the
// storage medium.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorage::Delete(UINT fuMode, IWMDMProgress *pProgress)
{
    HRESULT hr;

    //
    // If there is a progress object, then initialize it now.
    //
    if(pProgress)
    {
        //
        // Indicate that the delete is beginning.
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
        return(E_FAIL);
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
    // If this storage object has been deleted, then return a failure.
    //
    if(!m_pwcName[0])
    {
        if(pProgress)
        {
            pProgress->Progress(100);
            pProgress->End();
        }
        return(WMDM_E_INTERFACEDEAD);
    }

    //
    // We can not delete the root storage.
    //
    if(m_bIsRoot)
    {
        if(pProgress)
        {
            pProgress->Progress(100);
            pProgress->End();
        }
        return(E_FAIL);
    }

    //
    // If this file is currently opened, then we need to close it now.
    //
    if(m_bIsOpen)
    {
        //
        // Close this file.
        //
        Maverick_Close();

        //
        // Inidicate that this file is closed.
        //
        m_bIsOpen = FALSE;
    }

    //
    // Delete this file.
    //
    if(Maverick_Delete(m_ulDrive, m_pwcName) == 1)
    {
        //
        // Remove any Audible meta-data associated with this file.
        //
        RemoveAudibleMetaData(m_ulDrive, m_pwcName);

        //
        // Indicate that this file has been deleted.
        //
        m_pwcName[0] = '\0';

        //
        // Refresh the Maverick info database.
        //
        g_MaverickInfo.Refresh(m_ulDrive);

        //
        // The delete was successful.
        //
        hr = S_OK;
    }
    else
    {
        //
        // The delete failed.
        //
        hr = E_FAIL;
    }

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
    // Return the result.
    //
    return(hr);
}

//****************************************************************************
//
// The IMDSPObject::Move method, which moves this storage object to another
// storage object.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorage::Move(UINT fuMode, IWMDMProgress *pProgress,
                   IMDSPStorage *pTarget)
{
    //
    // If there is a progress object, then handle it now.
    //
    if(pProgress)
    {
        //
        // Indicate that the move is beginning.
        //
        pProgress->Begin(100);

        //
        // Set the progress to the end.
        //
        pProgress->Progress(100);

        //
        // Indicate that the move is complete.
        //
        pProgress->End();
    }

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
    // If this storage object has been deleted, then return a failure.
    //
    if(!m_pwcName[0])
    {
        return(WMDM_E_INTERFACEDEAD);
    }

    //
    // We do not support moving storage objects.
    //
    return(WMDM_E_NOTSUPPORTED);
}

//****************************************************************************
//
// The IMDSPObject::Open method, which opens this storage object and prepares
// it for other operations.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorage::Open(UINT fuMode)
{
    unsigned long ulFlags;

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
    // If this storage object has been deleted, then return a failure.
    //
    if(!m_pwcName[0])
    {
        return(WMDM_E_INTERFACEDEAD);
    }

    //
    // If this storage object is the root directory, then we can not open it.
    //
    if(m_bIsRoot)
    {
        return(WMDM_E_NOTSUPPORTED);
    }

    //
    // If this storage object is already open, then return a failure.
    //
    if(m_bIsOpen)
    {
        return(WMDM_E_BUSY);
    }

    //
    // Determine the flags for opening this file.
    //
    ulFlags = 0;
    if(fuMode & MDSP_READ)
    {
        ulFlags |= 1;
    }
    if(fuMode & MDSP_WRITE)
    {
        ulFlags |= 2;
    }

    //
    // Attempt to open this file.
    //
    if(Maverick_Open(m_ulDrive, m_pwcName, ulFlags) == 0)
    {
        //
        // Attempt to create this file.
        //
        if(Maverick_Open(m_ulDrive, m_pwcName, ulFlags | 4) == 0)
        {
            //
            // We could not open or create this file, so return a failure.
            //
            return(E_FAIL);
        }
    }

    //
    // Indicate that this file is opened.
    //
    m_bIsOpen = TRUE;

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// The IMDSPObject::Read method, which reads data from this storage object.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorage::Read(BYTE *pData, DWORD *pdwSize, BYTE abMac[WMDM_MAC_LENGTH])
{
    void *pTmpData;
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
    if(!pData || !pdwSize)
    {
        return(E_INVALIDARG);
    }

    //
    // If this storage object has been deleted, then return a failure.
    //
    if(!m_pwcName[0])
    {
        return(WMDM_E_INTERFACEDEAD);
    }

    //
    // If this storage object is not opened, then return a failure.
    //
    if(!m_bIsOpen)
    {
        return(E_FAIL);
    }

    //
    // Allocate memory to contain the data read from the device.
    //
    pTmpData = new BYTE [*pdwSize];
    if(!pTmpData)
    {
        return(E_FAIL);
    }

    //
    // Read data from the file.
    //
    *pdwSize = Maverick_Read(pTmpData, *pdwSize);

    //
    // See if we actually read any data.
    //
    if(*pdwSize)
    {
        //
        // Acquire a MAC channel.
        //
        hr = g_pAppSCServer->MACInit(&hMAC);
        if(FAILED(hr))
        {
            delete [] pTmpData;
            return(hr);
        }

        //
        // Update the MAC value with the data buffer contents.
        //
        hr = g_pAppSCServer->MACUpdate(hMAC, (BYTE *)pTmpData, *pdwSize);
        if(FAILED(hr))
        {
            delete [] pTmpData;
            return(hr);
        }

        //
        // Update the MAC value with the length of the data buffer.
        //
        hr = g_pAppSCServer->MACUpdate(hMAC, (BYTE *)pdwSize, sizeof(DWORD));
        if(FAILED(hr))
        {
            delete [] pTmpData;
            return(hr);
        }

        //
        // Release the MAC channel and retrieve the final MAC value.
        //
        hr = g_pAppSCServer->MACFinal(hMAC, abMac);
        if(FAILED(hr))
        {
            delete [] pTmpData;
            return(hr);
        }

        //
        // Encrypt the data buffer with the current session key.
        //
        hr = g_pAppSCServer->EncryptParam((BYTE *)pTmpData, *pdwSize);
        if(FAILED(hr))
        {
            delete [] pTmpData;
            return(hr);
        }

        //
        // Copy the encrypted data from the temporary buffer into the user
        // supplied buffer.
        //
        memcpy(pData, pTmpData, *pdwSize);
    }

    //
    // Delete the temporary memory buffer.
    //
    delete [] pTmpData;

    //
    // Success.
    //
    return((*pdwSize == 0) ? E_FAIL : S_OK);
}

//****************************************************************************
//
// The IMDSPObject::Rename method, which renames this storage object.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorage::Rename(LPWSTR pwszNewName, IWMDMProgress *pProgress)
{
    //
    // If there is a progress object, then handle it now.
    //
    if(pProgress)
    {
        //
        // Indicate that the rename is beginning.
        //
        pProgress->Begin(100);

        //
        // Set the progress to the end.
        //
        pProgress->Progress(100);

        //
        // Indicate that the rename is complete.
        //
        pProgress->End();
    }

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
    // If this storage object has been deleted, then return a failure.
    //
    if(!m_pwcName[0])
    {
        return(WMDM_E_INTERFACEDEAD);
    }

    //
    // We do not support renaming storage objects.
    //
    return(WMDM_E_NOTSUPPORTED);
}

//****************************************************************************
//
// The IMDSPObject::Seek method, which sets the current position within this
// storage object.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorage::Seek(UINT fuFlags, DWORD dwOffset)
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
    // If this storage object has been deleted, then return a failure.
    //
    if(!m_pwcName[0])
    {
        return(WMDM_E_INTERFACEDEAD);
    }

    //
    // If this storage object is not opened, then return a failure.
    //
    if(!m_bIsOpen)
    {
        return(E_FAIL);
    }

    //
    // Determine the seek method requested.
    //
    switch(fuFlags)
    {
        //
        // We should seek to the given offset from the beginning of the file.
        //
        case MDSP_SEEK_BOF:
        {
            //
            // The seek offset is simply the supplied offset, so there is
            // nothing to be done.
            //
            break;
        }

        //
        // We should seek to the given offset from the current position.
        //
        case MDSP_SEEK_CUR:
        {
            //
            // Add the current file position to the seek position.
            //
            dwOffset += Maverick_Tell();

            //
            // We now have a seek position relative to the beginning of the
            // file.
            //
            break;
        }

        //
        // We should seek to the given offset from the end of the file.
        //
        case MDSP_SEEK_EOF:
        {
            //
            // Add the file length to the seek position.
            //
            dwOffset += Maverick_Length();

            //
            // We now have a seek position relative to the beginning of the
            // file.
            //
            break;
        }

        //
        // An un-recognized seek method was specified, so return an error.
        //
        default:
        {
            return(E_INVALIDARG);
        }
    }

    //
    // Perform the seek.
    //
    if(Maverick_Seek(dwOffset) == 0)
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
// The IMDSPObject::Write method, which writes data to this storage object.
//
//****************************************************************************
STDMETHODIMP
CMDSPStorage::Write(BYTE *pData, DWORD *pdwSize, BYTE abMac[WMDM_MAC_LENGTH])
{
    BYTE pSelfMac[WMDM_MAC_LENGTH];
    void *pTmpData;
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
    if(!pData || !pdwSize)
    {
        return(E_INVALIDARG);
    }

    //
    // If this storage object has been deleted, then return a failure.
    //
    if(!m_pwcName[0])
    {
        return(WMDM_E_INTERFACEDEAD);
    }

    //
    // If this storage object is not opened, then return a failure.
    //
    if(!m_bIsOpen)
    {
        return(E_FAIL);
    }

    //
    // Allocate memory to contain the data to be written to the device.
    //
    pTmpData = new BYTE [*pdwSize];
    if(!pTmpData)
    {
        return(E_FAIL);
    }

    //
    // Copy the data from the user supplied buffer to our temporary buffer.
    //
    memcpy(pTmpData, pData, *pdwSize);

    //
    // Decrypt the data buffer with the current session key.
    //
    hr = g_pAppSCServer->DecryptParam((BYTE *)pTmpData, *pdwSize);
    if(FAILED(hr))
    {
        delete [] pTmpData;
        return(hr);
    }

    //
    // Acquire a MAC channel.
    //
    hr = g_pAppSCServer->MACInit(&hMAC);
    if(FAILED(hr))
    {
        delete [] pTmpData;
        return(hr);
    }

    //
    // Update the MAC value with the data buffer contents.
    //
    hr = g_pAppSCServer->MACUpdate(hMAC, (BYTE *)pTmpData, *pdwSize);
    if(FAILED(hr))
    {
        delete [] pTmpData;
        return(hr);
    }

    //
    // Update the MAC value with the length of the data buffer.
    //
    hr = g_pAppSCServer->MACUpdate(hMAC, (BYTE *)pdwSize, sizeof(DWORD));
    if(FAILED(hr))
    {
        delete [] pTmpData;
        return(hr);
    }

    //
    // Release the MAC channel and retrieve the final MAC value.
    //
    hr = g_pAppSCServer->MACFinal(hMAC, pSelfMac);

    //
    // Compare the computed MAC value with the supplied MAC value.
    //
    if(memcmp(abMac, pSelfMac, WMDM_MAC_LENGTH) != 0)
    {
        //
        // The MAC did not match, so do not perform the write (are we being
        // hacked?).
        //
        delete [] pTmpData;
        return(WMDM_E_MAC_CHECK_FAILED);
    }

    //
    // Write the data to the file.
    //
    *pdwSize = Maverick_Write(pTmpData, *pdwSize);

    //
    // Delete the temporary memory buffer.
    //
    delete [] pTmpData;

    //
    // Update our copy of the length of this file since it might change as a
    // result of a write.
    //
    m_ulSize = Maverick_Length();

    //
    // Success.
    //
    return((*pdwSize == 0) ? E_FAIL : S_OK);
}

//****************************************************************************
//
// DEVICE.CPP - The CMDSPDevice class, which implements the IMDSPDevice and
//              IMDSPDeviceControl interfaces.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
// Copyright (C) Microsoft Corporation, 1999 - 2001.  All rights reserved.
//
//****************************************************************************
#include "stdafx.h"

//****************************************************************************
//
// Sets the drive with which this device is associated.
//
//****************************************************************************
void
CMDSPDevice::SetDrive(unsigned long ulDrive)
{
    //
    // Remember the drive number.
    //
    m_ulDrive = ulDrive;
}

//****************************************************************************
//
// IMDSPDevice::GetDeviceIcon returns a HICON to be used to represent the
// device.
//
//****************************************************************************
STDMETHODIMP
CMDSPDevice::GetDeviceIcon(ULONG *hIcon)
{
    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(E_FAIL);
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
    if(!hIcon)
    {
        return(E_INVALIDARG);
    }

    //
    // Load the icon used to represnet the device.
    //
    *hIcon = (ULONG)LoadIconA(g_hinstance, MAKEINTRESOURCEA(IDI_ICON_PM));
    if(!(*hIcon))
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
// IMDSPDevice::GetFormatSupport returns a list of the formats supported by
// the device.
//
//****************************************************************************
STDMETHODIMP
CMDSPDevice::GetFormatSupport(_WAVEFORMATEX **pFormatEx, UINT *pnFormatCount,
                              LPWSTR **pppwszMimeType, UINT *pnMimeTypeCount)
{
    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(E_FAIL);
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
    if(!pFormatEx || !pnFormatCount || !pppwszMimeType || !pnMimeTypeCount)
    {
        return(E_INVALIDARG);
    }

    //
    // We will return a single WAVEFORMATEX structure.
    //
    *pnFormatCount = 1;

    //
    // Allocate memory for the WAVEFORMATEX structure which we will return.
    //
    *pFormatEx = (_WAVEFORMATEX *)CoTaskMemAlloc(sizeof(_WAVEFORMATEX));
    if(!(*pFormatEx))
    {
        return(E_FAIL);
    }

    //
    // Fill in the WAVEFORMATEX structure to indicate that we will support any
    // format with two channels.
    //
    (*pFormatEx)->wFormatTag = WMDM_WAVE_FORMAT_ALL;
    (*pFormatEx)->nChannels = 2;
    (*pFormatEx)->cbSize = 0;
    (*pFormatEx)->nSamplesPerSec = 0;
    (*pFormatEx)->nAvgBytesPerSec = 0;
    (*pFormatEx)->nBlockAlign = 0;
    (*pFormatEx)->wBitsPerSample = 0;

    //
    // We will return a single MIME type string.
    //
    *pnMimeTypeCount = 1;

    //
    // Allocate memory for the MIME type string array which we will return.
    //
    *pppwszMimeType = (LPWSTR *)CoTaskMemAlloc(sizeof(LPWSTR) * 1);
    if(!(*pppwszMimeType))
    {
        CoTaskMemFree(*pFormatEx);
        *pFormatEx = NULL;
        return(E_FAIL);
    }

    //
    // Allocate memory for the MIME type string which we will return.
    //
    **pppwszMimeType = (LPWSTR)CoTaskMemAlloc(sizeof(WCHAR) *
                                              (wcslen(WCS_MIME_TYPE_ALL) + 1));
    if(!(**pppwszMimeType))
    {
        CoTaskMemFree(*pFormatEx);
        *pFormatEx = NULL;
        CoTaskMemFree(*pppwszMimeType);
        *pppwszMimeType = NULL;
        return(E_FAIL);
    }

    //
    // Fill in the MIME type string to indicate that we will support any MIME
    // type.
    //
    wcscpy(**pppwszMimeType, WCS_MIME_TYPE_ALL);

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// IMDSPDevice::EnumStorage returns a storage enumerator.
//
//****************************************************************************
STDMETHODIMP
CMDSPDevice::EnumStorage(IMDSPEnumStorage **ppEnumStorage)
{
    CComObject<CMDSPEnumStorage> *pEnumObj;
    HRESULT hr;

    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(E_FAIL);
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
    // Create a new storage enumerator.
    //
    hr = CComObject<CMDSPEnumStorage>::CreateInstance(&pEnumObj);
    if(FAILED(hr))
    {
        return(hr);
    }

    //
    // Set the drive to be enumerated by the storage enumerator.
    //
    pEnumObj->SetDrive(m_ulDrive, TRUE);

    //
    // Get a pointer to the IMDSPEnumStorage interface for the object.
    //
    hr = pEnumObj->QueryInterface(IID_IMDSPEnumStorage,
                                  reinterpret_cast<void**>(ppEnumStorage));
    if(FAILED(hr))
    {
        delete pEnumObj;
        return(hr);
    }

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// IMDSPDevice::GetManufacturer returns the name of the device manufacturer.
//
//****************************************************************************
STDMETHODIMP
CMDSPDevice::GetManufacturer(LPWSTR pwszName, UINT nMaxChars)
{
    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(E_FAIL);
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
    if(!pwszName)
    {
        return(E_INVALIDARG);
    }

    //
    // Get the name of the device manufacturer.
    //
    g_MaverickInfo.GetManufacturerName(pwszName, nMaxChars);

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// IMDSPDevice::GetName returns the name of the device.
//
//****************************************************************************
STDMETHODIMP
CMDSPDevice::GetName(LPWSTR pwszName, UINT nMaxChars)
{
    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(E_FAIL);
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
    if(!pwszName)
    {
        return(E_INVALIDARG);
    }

    //
    // Get the name of the device.
    //
    g_MaverickInfo.GetDriveName(m_ulDrive, pwszName, nMaxChars);

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// IMDSPDevice::GetPowerSource returns the current power source of the device.
//
//****************************************************************************
STDMETHODIMP
CMDSPDevice::GetPowerSource(DWORD *pdwPowerSource, DWORD *pdwPercentRemaining)
{
    unsigned long ulLength;

    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(E_FAIL);
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
    if(!pdwPowerSource || !pdwPercentRemaining)
    {
        return(E_INVALIDARG);
    }

    //
    // The device runs on batteries only, and is therefore currently running
    // on the batteries.
    //
    *pdwPowerSource = WMDM_POWER_CAP_BATTERY | WMDM_POWER_IS_BATTERY |
                      WMDM_POWER_PERCENT_AVAILABLE;

    //
    // Get the remaining battery life.
    //
    ulLength = sizeof(unsigned long);
    if(Maverick_GetDescriptor(USB_Vendor_Descriptor_BatteryLife, 0,
                              (char *)pdwPercentRemaining, &ulLength) == 0)
    {
        *pdwPercentRemaining = 0;
    }

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// IMDSPDevice::GetSerialNumber returns the unique serial number of the
// device.
//
//****************************************************************************
STDMETHODIMP
CMDSPDevice::GetSerialNumber(PWMDMID pSerialNumber,
                             BYTE abMac[WMDM_MAC_LENGTH])
{
    HRESULT hr;
    HMAC hMAC;

    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(E_FAIL);
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
    if(!pSerialNumber)
    {
        return(E_INVALIDARG);
    }

    //
    // Get the device serial number.
    //
    g_MaverickInfo.GetSerialNumber(m_ulDrive, pSerialNumber);
    if(pSerialNumber->cbSize == 0)
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
    hr = g_pAppSCServer->MACUpdate(hMAC, (BYTE *)pSerialNumber,
                                   sizeof(WMDMID));
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
// IMDSPDevice::GetStatus returns the current device status.
//
//****************************************************************************
STDMETHODIMP
CMDSPDevice::GetStatus(DWORD *pdwStatus)
{
    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(E_FAIL);
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
    // The device is always ready.
    //
    *pdwStatus = WMDM_STATUS_READY;

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// IMDSPDevice::GetType returns the device type information.
//
//****************************************************************************
STDMETHODIMP
CMDSPDevice::GetType(DWORD *pdwType)
{
    WMDMID sSerialNum;

    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(E_FAIL);
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
    if(!pdwType)
    {
        return(E_INVALIDARG);
    }

    //
    // The device is simply a storage device, which accepts non-SDMI compliant
    // content.
    //
    *pdwType = WMDM_DEVICE_TYPE_STORAGE | WMDM_DEVICE_TYPE_NONSDMI;

    //
    // Get the device serial number.
    //
    g_MaverickInfo.GetSerialNumber(m_ulDrive, &sSerialNum);

    //
    // If we have a serial number, then indicate that we accept SDMI compliant
    // content.
    //
    if(sSerialNum.cbSize != 0)
    {
        *pdwType |= WMDM_DEVICE_TYPE_SDMI;
    }

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// IMDSPDevice::GetVersion returns the version number of the device.
//
//****************************************************************************
STDMETHODIMP
CMDSPDevice::GetVersion(DWORD *pdwVersion)
{
    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(E_FAIL);
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
    if(!pdwVersion)
    {
        return(E_INVALIDARG);
    }

    //
    // Return the version number of the device.
    //
    *pdwVersion = g_MaverickInfo.GetVersion();

    //
    // Success.
    //
    return(S_OK);
}

//
// Opaque Command to get extended certification information
//
// GUID = {C39BF696-B776-459c-A13A-4B7116AB9F09}
//
#if 0
static const GUID guidCertInfoEx =
{ 0xc39bf696, 0xb776, 0x459c, { 0xa1, 0x3a, 0x4b, 0x71, 0x16, 0xab, 0x9f, 0x9 } };

typedef struct
{
    HRESULT hr;
    DWORD   cbCert;
    BYTE    pbCert[1];

} CERTINFOEX;

static const BYTE bCertInfoEx_App[] =
{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 };

static const BYTE bCertInfoEx_SP[] =
{ 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00,
  0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00 };
#endif

//****************************************************************************
//
// IMDSPDevice::SendOpaqueCommand sends an opaque command to the device.
//
//****************************************************************************
STDMETHODIMP
CMDSPDevice::SendOpaqueCommand(OPAQUECOMMAND *pCommand)
{
    //HRESULT hr;
    //HMAC hMAC;
    //BYTE abMACVerify[WMDM_MAC_LENGTH];

    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(E_FAIL);
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
    if(!pCommand)
    {
        return(E_INVALIDARG);
    }

    //
    // We do not support any opaque device commands.
    //
    return(WMDM_E_NOTSUPPORTED);

    // XYZZY Do we need this?
#if 0
    //
    // Compute and verify MAC
    //
    CORg(g_pAppSCServer->MACInit(&hMAC));
    CORg(g_pAppSCServer->MACUpdate(hMAC, (BYTE *)(&(pCommand->guidCommand)),
                                   sizeof(GUID)));
    CORg(g_pAppSCServer->MACUpdate(hMAC, (BYTE *)(&(pCommand->dwDataLen)),
                                   sizeof(pCommand->dwDataLen)));
    if(pCommand->pData)
    {
        CORg(g_pAppSCServer->MACUpdate(hMAC, (BYTE *)(pCommand->pData),
                                       pCommand->dwDataLen));
    }
    CORg(g_pAppSCServer->MACFinal(hMAC, abMACVerify));

    if(memcmp(abMACVerify, pCommand->abMAC, WMDM_MAC_LENGTH) != 0)
    {
        CORg(WMDM_E_MAC_CHECK_FAILED);
    }

    //
    // Take action based on the command GUID
    //
    if(memcmp(&(pCommand->guidCommand), &guidCertInfoEx, sizeof(GUID)) == 0)
    {
        //
        // Command to exchange extended authentication information
        //
        CERTINFOEX *pCertInfoEx;

        DWORD cbData_App = sizeof(bCertInfoEx_App) / sizeof(BYTE);
        DWORD cbData_SP = sizeof(bCertInfoEx_SP) / sizeof(BYTE);
        DWORD cbData_Return = sizeof(CERTINFOEX) + cbData_SP;

        //
        // The caller must include their extended cert info
        //
        if(!pCommand->pData)
        {
            CORg(E_INVALIDARG);
        }

        //
        // Map the data in the opaque command to a CERTINFOEX structure
        //
        pCertInfoEx = (CERTINFOEX *)pCommand->pData;

        //
        // In this simple extended authentication scheme, the caller must
        // provide the exact cert info
        //
        if((pCertInfoEx->cbCert != cbData_App) ||
           (memcmp(pCertInfoEx->pbCert, bCertInfoEx_App, cbData_App) != 0))
        {
            CORg(WMDM_E_NOTCERTIFIED);
        }

        //
        // Free the caller data and allocate enough data for our return data
        //
        CoTaskMemFree(pCommand->pData);

        CFRg((pCommand->pData = (BYTE *)CoTaskMemAlloc(cbData_Return)));
        pCommand->dwDataLen = cbData_Return;

        //
        // Copy the extended cert info into return data structure
        //
        pCertInfoEx = (CERTINFOEX *)pCommand->pData;

        pCertInfoEx->hr = S_OK;
        pCertInfoEx->cbCert = cbData_SP;
        memcpy(pCertInfoEx->pbCert, bCertInfoEx_SP, cbData_SP);

        //
        // Compute MAC on return data
        //
        CORg(g_pAppSCServer->MACInit(&hMAC));
        CORg(g_pAppSCServer->MACUpdate(hMAC,
                                       (BYTE *)(&(pCommand->guidCommand)),
                                       sizeof(GUID)));
        CORg(g_pAppSCServer->MACUpdate(hMAC, (BYTE *)(&(pCommand->dwDataLen)),
                                       sizeof(pCommand->dwDataLen)));
        if(pCommand->pData)
        {
            CORg(g_pAppSCServer->MACUpdate(hMAC, (BYTE *)(pCommand->pData),
                                           pCommand->dwDataLen));
        }
        CORg(g_pAppSCServer->MACFinal(hMAC, pCommand->abMAC));

        hr = S_OK;
    }
    else
    {
        CORg(WMDM_E_NOTSUPPORTED);
    }

Error:

    return(hr);
#endif
}

//****************************************************************************
//
// IMDSPDeviceControl::GetCapabilities returns the control capabilities of the
// device.
//
//****************************************************************************
STDMETHODIMP
CMDSPDevice::GetCapabilities(DWORD *pdwCapabilitiesMask)
{
    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(E_FAIL);
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
    if(!pdwCapabilitiesMask)
    {
        return(E_INVALIDARG);
    }

    //
    // We do not support any device control.
    //
    *pdwCapabilitiesMask = 0;

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// IMDSPDeviceControl::GetDCStatus returns the control status of the device.
//
//****************************************************************************
STDMETHODIMP
CMDSPDevice::GetDCStatus(DWORD *pdwStatus)
{
    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(E_FAIL);
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
    // Our status is always ready.
    //
    *pdwStatus = WMDM_STATUS_READY;

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// IMDSPDeviceControl::Pause pauses the playback or record on the device.
//
//****************************************************************************
STDMETHODIMP
CMDSPDevice::Pause(void)
{
    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(E_FAIL);
    }

    //
    // If we are not authenticated, then return a failure.
    //
    if(!(g_pAppSCServer->fIsAuthenticated()))
    {
        return(WMDM_E_NOTCERTIFIED);
    }

    //
    // We do not support the ::Pause method.
    //
    return(WMDM_E_NOTSUPPORTED);
}

//****************************************************************************
//
// IMDSPDeviceControl::Play starts playback on the device.
//
//****************************************************************************
STDMETHODIMP
CMDSPDevice::Play(void)
{
    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(E_FAIL);
    }

    //
    // If we are not authenticated, then return a failure.
    //
    if(!(g_pAppSCServer->fIsAuthenticated()))
    {
        return(WMDM_E_NOTCERTIFIED);
    }

    //
    // We do not support the ::Play method.
    //
    return(WMDM_E_NOTSUPPORTED);
}

//****************************************************************************
//
// IMDSPDeviceControl::Record starts recording on the device.
//
//****************************************************************************
STDMETHODIMP
CMDSPDevice::Record(_WAVEFORMATEX *pFormat)
{
    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(E_FAIL);
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
    if(!pFormat)
    {
        return(E_INVALIDARG);
    }

    //
    // We do not support the ::Record method.
    //
    return(WMDM_E_NOTSUPPORTED);
}

//****************************************************************************
//
// IMDSPDeviceControl::Resume resumes the paused playback or record on the
// device.
//
//****************************************************************************
STDMETHODIMP
CMDSPDevice::Resume(void)
{
    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(E_FAIL);
    }

    //
    // If we are not authenticated, then return a failure.
    //
    if(!(g_pAppSCServer->fIsAuthenticated()))
    {
        return(WMDM_E_NOTCERTIFIED);
    }

    //
    // We do not support the ::Resume method.
    //
    return(WMDM_E_NOTSUPPORTED);
}

//****************************************************************************
//
// IMDSPDeviceControl::Seek seeks to the specified position on the device.
//
//****************************************************************************
STDMETHODIMP
CMDSPDevice::Seek(UINT fuMode, int nOffset)
{
    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(E_FAIL);
    }

    //
    // If we are not authenticated, then return a failure.
    //
    if(!(g_pAppSCServer->fIsAuthenticated()))
    {
        return(WMDM_E_NOTCERTIFIED);
    }

    //
    // We do not support the ::Seek method.
    //
    return(WMDM_E_NOTSUPPORTED);
}

//****************************************************************************
//
// IMDSPDeviceControl::Stop stops the playback or record on the device.
//
//****************************************************************************
STDMETHODIMP
CMDSPDevice::Stop(void)
{
    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(E_FAIL);
    }

    //
    // If we are not authenticated, then return a failure.
    //
    if(!(g_pAppSCServer->fIsAuthenticated()))
    {
        return(WMDM_E_NOTCERTIFIED);
    }

    //
    // We do not support the ::Stop method.
    //
    return(WMDM_E_NOTSUPPORTED);
}

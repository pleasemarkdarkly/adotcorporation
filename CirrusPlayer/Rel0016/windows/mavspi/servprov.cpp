//****************************************************************************
//
// SERVPROV.CPP - The CMDServiceProvider class, which implements the
//                IMDServiceProvider and IComponentAuthenticate interfaces.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
// Copyright (C) Microsoft Corporation, 1999 - 2001.  All rights reserved.
//
//****************************************************************************
#include "stdafx.h"
#include "key.c"

//****************************************************************************
//
// The constructor for the CMDServiceProvider class.
//
//****************************************************************************
CMDServiceProvider::CMDServiceProvider(void)
{
    //
    // Allocate a new secure channel server object.
    //
    g_pAppSCServer = new CSecureChannelServer();

    //
    // See if we were able to allocate the object.
    //
    if(g_pAppSCServer)
    {
        //
        // Supply the secure channel server object with our certificate.
        //
        g_pAppSCServer->SetCertificate(SAC_CERT_V1, (BYTE *)abCert,
                                       sizeof(abCert), (BYTE *)abPVK,
                                       sizeof(abPVK));
    }

    //
    // Initialize the CMaverickInfo class.
    //
    g_MaverickInfo.Initialize();
}

//****************************************************************************
//
// The destructor for the CMDServiceProvider class.
//
//****************************************************************************
CMDServiceProvider::~CMDServiceProvider(void)
{
    //
    // Reset the Maverick database.
    //
    g_MaverickInfo.Reset();

    //
    // See if we have a secure channel server object.
    //
    if(g_pAppSCServer)
    {
        //
        // Delete our secure channel server object.
        //
        delete g_pAppSCServer;
        g_pAppSCServer = NULL;
    }
}

//****************************************************************************
//
// IMDServiceProvider::EnumDevices returns a device enumerator.
//
//****************************************************************************
STDMETHODIMP
CMDServiceProvider::EnumDevices(IMDSPEnumDevice **ppEnumDevice)
{
    HRESULT hr;
    CComObject<CMDSPEnumDevice> *pEnumObj;

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
    // Create a new CMDSPEnumDevice object for enumerating the available
    // devices.
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
                                  reinterpret_cast<void**>(ppEnumDevice));
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
// IMDServiceProvider::GetDeviceCount returns the number of media device which
// are installed.
//
//****************************************************************************
STDMETHODIMP
CMDServiceProvider::GetDeviceCount(DWORD *pdwCount)
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
    if(!pdwCount)
    {
        return(E_FAIL);
    }

    //
    // We support a single device per drive.
    //
    *pdwCount = g_MaverickInfo.GetNumDrives();

    //
    // Success.
    //
    return(S_OK);
}

//****************************************************************************
//
// IComponentAuthenticate::SACAuth establishes a secure authenticated channel
// between software components.
//
//****************************************************************************
STDMETHODIMP
CMDServiceProvider::SACAuth(DWORD dwProtocolID, DWORD dwPass, BYTE *pbDataIn,
                            DWORD dwDataInLen, BYTE **ppbDataOut,
                            DWORD *pdwDataOutLen)
{
    HRESULT hr = E_FAIL;

    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(E_FAIL);
    }

    //
    // Call the SACAuth method of the secure channel server object.
    //
    hr = g_pAppSCServer->SACAuth(dwProtocolID, dwPass, pbDataIn, dwDataInLen,
                                 ppbDataOut, pdwDataOutLen);

    //
    // Return the result.
    //
    return(hr);
}

//****************************************************************************
//
// IComponentAuthenticate::SACGetProtocols returns the authentication
// prototcols supported by our module.
//
//****************************************************************************
STDMETHODIMP
CMDServiceProvider::SACGetProtocols(DWORD **ppdwProtocols,
                                    DWORD *pdwProtocolCount)
{
    HRESULT hr = E_FAIL;

    //
    // If we do not have a secure channel server object, then return a failure.
    //
    if(!g_pAppSCServer)
    {
        return(E_FAIL);
    }

    //
    // Call the SACGetProtocols method of the secure channel server object.
    //
    hr = g_pAppSCServer->SACGetProtocols(ppdwProtocols, pdwProtocolCount);

    //
    // Return the result.
    //
    return(hr);
}

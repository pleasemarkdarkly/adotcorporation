//****************************************************************************
//
// SERVPROV.H - Definition of the CMDServiceProvider class, which implements
//              the IMDServiceProvider and IComponentAuthenticate interfaces.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
// Copyright (C) Microsoft Corporation, 1999 - 2001.  All rights reserved.
//
//****************************************************************************
#ifndef __MDSERVICEPROVIDER_H_
#define __MDSERVICEPROVIDER_H_

//****************************************************************************
//
// The CMDServiceProvider class, which implements the IMDServiceProvider and
// IComponentAuthenticate interfaces.
//
//****************************************************************************
class ATL_NO_VTABLE CMDServiceProvider :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CMDServiceProvider, &CLSID_MDServiceProvider>,
    public IMDServiceProvider,
    public IComponentAuthenticate
{
    //
    // The public methods of this object.
    //
    public:
        //
        // The constructor.
        //
        CMDServiceProvider(void);

        //
        // The destructor.
        //
        ~CMDServiceProvider(void);

        //
        // Define the class factory for this object.
        //
        DECLARE_CLASSFACTORY_SINGLETON(CMDServiceProvider)

        //
        // Define the resource identifier for the registry information
        // associated with this object.
        //
        DECLARE_REGISTRY_RESOURCEID(IDR_MDSERVICEPROVIDER)

        //
        // Define the list of interfaces supported through COM by this object.
        //
        BEGIN_COM_MAP(CMDServiceProvider)
            COM_INTERFACE_ENTRY(IMDServiceProvider)
            COM_INTERFACE_ENTRY(IComponentAuthenticate)
        END_COM_MAP()

        //
        // The IMDServiceProvider::EnumDevices method, which returns a device
        // enumerator.
        //
        STDMETHOD(EnumDevices)(IMDSPEnumDevice **ppEnumDevice);

        //
        // The IMDServiceProvider::GetDeviceCount method, which returns the
        // number of devices.
        //
        STDMETHOD(GetDeviceCount)(DWORD *pdwCount);

        //
        // The IComponentAuthenticate::SACAuth method, which establishes a
        // secure authenticated channel between software components.
        //
        STDMETHOD(SACAuth)(DWORD dwProtocolID,
                           DWORD dwPass,
                           BYTE *pbDataIn,
                           DWORD dwDataInLen,
                           BYTE **ppbDataOut,
                           DWORD *pdwDataOutLen);

        //
        // The IComponentAuthenticate::SACGetProtocols method, which returns
        // the authentication protocols supported by our module.
        //
        STDMETHOD(SACGetProtocols)(DWORD **ppdwProtocols,
                                   DWORD *pdwProtocolCount);
};

#endif //__MDSERVICEPROVIDER_H_

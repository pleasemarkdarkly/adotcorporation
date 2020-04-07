//****************************************************************************
//
// STORGLOB.H - Definition of the CMDSPStorageGlobals class, which implements
//              the IMDSPStorageGlobals interface.
//
// Coypright (c) 2000 Cirrus Logic, Inc.
// Copyright (C) Microsoft Corporation, 1999 - 2001.  All rights reserved.
//
//****************************************************************************
#ifndef __MDSPSTORAGEGLOBALS_H_
#define __MDSPSTORAGEGLOBALS_H_

//****************************************************************************
//
// The CMDSPStorageGlobals class, which implements the IMDSPStorageGlobals
// interface.
//
//****************************************************************************
class ATL_NO_VTABLE CMDSPStorageGlobals :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CMDSPStorageGlobals, &CLSID_MDSPStorageGlobals>,
    public IMDSPStorageGlobals
{
    //
    // The public methods of this object.
    //
    public:
        //
        // Specifies the drive which this object represents.
        //
        void SetDrive(unsigned long ulDrive);

        //
        // Define the resource identifier for the registry information
        // associated with this object.
        //
        DECLARE_REGISTRY_RESOURCEID(IDR_MDSPSTORAGEGLOBALS)

        //
        // Define the list of interfaces supported through COM by this object.
        //
        BEGIN_COM_MAP(CMDSPStorageGlobals)
            COM_INTERFACE_ENTRY(IMDSPStorageGlobals)
        END_COM_MAP()

        //
        // The IMDSPStorageGlobals::GetCapabilities method, which returns the
        // capabilities of the storage medium.
        //
        STDMETHOD(GetCapabilities)(DWORD *pdwCapabilities);

        //
        // The IMDSPStorageGlobals::GetDevice method, which returns the device
        // object with which this storage medium is associated.
        //
        STDMETHOD(GetDevice)(IMDSPDevice **ppDevice);

        //
        // The IMDSPStorageGlobals::GetRootStorage method, which returns the
        // root storage object for this storage medium.
        //
        STDMETHOD(GetRootStorage)(IMDSPStorage **ppRoot);

        //
        // The IMDSPStorageGlobals::GetSerialNumber method, which returns the
        // serial number that uniquely identifies this storage medium.
        //
        STDMETHOD(GetSerialNumber)(PWMDMID pSerialNum,
                                   BYTE abMac[WMDM_MAC_LENGTH]);

        //
        // The IMDSPStorageGlobals::GetStatus method, which returns the current
        // status of the storage medium.
        //
        STDMETHOD(GetStatus)(DWORD *pdwStatus);

        //
        // The IMDSPStorageGlobals::GetTotalBad method, which returns the
        // amount of unusable space on the storage medium.
        //
        STDMETHOD(GetTotalBad)(DWORD *pdwBadLow, DWORD *pdwBadHigh);

        //
        // The IMDSPStorageGlobals::GetTotalFree method, which returns the
        // amount of available space on the storage medium.
        //
        STDMETHOD(GetTotalFree)(DWORD *pdwFreeLow, DWORD *pdwFreeHigh);

        //
        // The IMDSPStorageGlobals::GetTotalSize method, which returns the
        // total capacity of the storage medium.
        //
        STDMETHOD(GetTotalSize)(DWORD *pdwTotalSizeLow,
                                DWORD *pdwTotalSizeHigh);

        //
        // The IMDSPStorageGlobals::Initialize method, which formats the
        // storage medium.
        //
        STDMETHOD(Initialize)(UINT fuMode, IWMDMProgress *pProgress);

    //
    // The private members of this object.
    //
    private:
        //
        // The drive which this storage global object represents.
        //
        unsigned long m_ulDrive;
};

#endif //__MDSPSTORAGEGLOBALS_H_

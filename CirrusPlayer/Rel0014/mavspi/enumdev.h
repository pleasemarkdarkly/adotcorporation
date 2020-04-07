//****************************************************************************
//
// ENUMDEV.H - Definition of the CMDSPEnumDevice class, which implements the
//             IMDSPEnumDevice interface.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
// Copyright (C) Microsoft Corporation, 1999 - 2001.  All rights reserved.
//
//****************************************************************************
#ifndef __MDSPENUMDEVICE_H_
#define __MDSPENUMDEVICE_H_

//****************************************************************************
//
// The CMDSPEnumDevice class, which implements the IMDSPEnumDevice interface.
//
//****************************************************************************
class ATL_NO_VTABLE CMDSPEnumDevice :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CMDSPEnumDevice, &CLSID_MDSPEnumDevice>,
    public IMDSPEnumDevice
{
    //
    // The public methods of this object.
    //
    public:
        //
        // The constructor.
        //
        CMDSPEnumDevice(void);

        //
        // Define the resource identifier for the registry information
        // associated with this object.
        //
        DECLARE_REGISTRY_RESOURCEID(IDR_MDSPENUMDEVICE)

        //
        // Define the list of interfaces supported through COM by this object.
        //
        BEGIN_COM_MAP(CMDSPEnumDevice)
            COM_INTERFACE_ENTRY(IMDSPEnumDevice)
        END_COM_MAP()

        //
        // The IMDSPEnumDevice::Clone method, which creates another enumerator
        // with the same enumeration state as this object.
        //
        STDMETHOD(Clone)(IMDSPEnumDevice **ppEnumDevice);

        //
        // The IMDSPEnumDevice::Next method, which returns the specified number
        // of devices in the enumeration sequence.
        //
        STDMETHOD(Next)(ULONG celt, IMDSPDevice **ppDevice,
                        ULONG *pceltFetched);

        //
        // The IMDSPEnumDevice::Reset method, which resets the enumeration
        // sequence to the beginning.
        //
        STDMETHOD(Reset)(void);

        //
        // The IMDSPEnumDevice::Skip method, which skips the specified number
        // of devices in the enumeration sequence.
        //
        STDMETHOD(Skip)(ULONG celt, ULONG *pceltFetched);

    //
    // The protected members of this object.
    //
    protected:
        //
        // The current index into the enumeration.
        //
        unsigned long m_ulIndex;
};

#endif //__MDSPENUMDEVICE_H_

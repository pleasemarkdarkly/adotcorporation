//****************************************************************************
//
// ENUMSTOR.H - Definition of the CMDSPEnumStorage class, which implements the
//              IMDSPEnumStorage interface.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
// Copyright (C) Microsoft Corporation, 1999 - 2001.  All rights reserved.
//
//****************************************************************************
#ifndef __MDSPENUMSTORAGE_H_
#define __MDSPENUMSTORAGE_H_

//****************************************************************************
//
// The CMDSPEnumStorage class, which implements the IMDSPEnumStorage
// interface.
//
//****************************************************************************
class ATL_NO_VTABLE CMDSPEnumStorage :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CMDSPEnumStorage, &CLSID_MDSPEnumStorage>,
    public IMDSPEnumStorage
{
    //
    // The public methods of this object.
    //
    public:
        //
        // The constructor.
        //
        CMDSPEnumStorage(void);

        //
        // Specifies the drive to be enumerated by this object.
        //
        void SetDrive(unsigned long ulDrive, unsigned long bIsRoot);

        //
        // Define the resource identifier for the registry information
        // associated with this object.
        //
        DECLARE_REGISTRY_RESOURCEID(IDR_MDSPENUMSTORAGE)

        //
        // Define the list of interfaces supported through COM by this object.
        //
        BEGIN_COM_MAP(CMDSPEnumStorage)
            COM_INTERFACE_ENTRY(IMDSPEnumStorage)
        END_COM_MAP()

        //
        // The IMDSPEnumStorage::Clone method, which creates another enumerator
        // with the same enumeration state as this object.
        //
        STDMETHOD(Clone)(IMDSPEnumStorage **ppEnumStorage);

        //
        // The IMDSPEnumStorage::Next method, which returns the specified
        // number of storage objects in the enumeration sequence.
        //
        STDMETHOD(Next)(ULONG celt, IMDSPStorage **ppStorage,
                        ULONG *pceltFetched);

        //
        // The IMDSPEnumStorage::Reset method, which resets the enumeration
        // sequence to the beginning.
        //
        STDMETHOD(Reset)(void);

        //
        // The IMDSPEnumStorage::Skip method, which skips the specified number
        // of storage objects in the enumeration sequence.
        //
        STDMETHOD(Skip)(ULONG celt, ULONG *pceltFetched);

    //
    // The protected members of this object.
    //
    protected:
        //
        // The drive which is being enumerated.
        //
        unsigned long m_ulDrive;

        //
        // A boolean which is TRUE if we are "enumerating" the root storage
        // object, and FALSE if we are enumerating the files on the device.
        //
        unsigned long m_bIsRoot;

        //
        // The current index into the enumeration.
        //
        unsigned long m_ulIndex;
};

#endif //__MDSPENUMSTORAGE_H_

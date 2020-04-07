//****************************************************************************
//
// STORAGE.H - Definition of the CMDSPStorage class, which implements the
//             IMDSPStorage, IMDSPObjectInfo, and IMDSPObject interfaces.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
// Copyright (C) Microsoft Corporation, 1999 - 2001.  All rights reserved.
//
//****************************************************************************
#ifndef __MDSPSTORAGE_H_
#define __MDSPSTORAGE_H_

//****************************************************************************
//
// The CMDSPStorage class, which implements the IMDSPStorage, IMDSPObjectInfo,
// and IMDSPObject interfaces.
//
//****************************************************************************
class ATL_NO_VTABLE CMDSPStorage :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CMDSPStorage, &CLSID_MDSPStorage>,
    public IMDSPStorage, IMDSPObjectInfo, IMDSPObject
{
    //
    // The public methods of this object.
    //
    public:
        //
        // The constructor.
        //
        CMDSPStorage(void);

        //
        // The destructor.
        //
        ~CMDSPStorage(void);

        //
        // Initializes the information about this storage object.
        //
        void SetStorageInfo(unsigned long ulDrive, WCHAR *pcName,
                            unsigned long bIsRoot, unsigned long ulSize,
                            unsigned long ulDate);

        //
        // Define the resource identifier for the registry information
        // associated with this object.
        //
        DECLARE_REGISTRY_RESOURCEID(IDR_MDSPSTORAGE)

        //
        // Define the list of interfaces supported through COM by this object.
        //
        BEGIN_COM_MAP(CMDSPStorage)
            COM_INTERFACE_ENTRY(IMDSPStorage)
            COM_INTERFACE_ENTRY(IMDSPObjectInfo)
            COM_INTERFACE_ENTRY(IMDSPObject)
        END_COM_MAP()

        //
        // The IMDSPStorage::CreateStorage method, which creates a new storage
        // at the level of this storage object.
        //
        STDMETHOD(CreateStorage)(DWORD dwAttributes, _WAVEFORMATEX *pFormat,
                                 LPWSTR pwszName, IMDSPStorage **ppNewStorage);

        //
        // The IMDSPStorage::EnumStorage method, which returns an enumerator
        // for the storage objects contained in this storage object.
        //
        STDMETHOD(EnumStorage)(IMDSPEnumStorage **ppEnumStorage);

        //
        // The IMDSPStorage::GetAttributes method, which returns the attributes
        // of this storage object.
        //
        STDMETHOD(GetAttributes)(DWORD *pdwAttributes, _WAVEFORMATEX *pFormat);

        //
        // The IMDSPStorage::GetDate method, which returns the date on which
        // this storage object was most recently modified.
        //
        STDMETHOD(GetDate)(PWMDMDATETIME pDateTimeUTC);

        //
        // The IMDSPStorage::GetName method, which returns the name of this
        // storage object.
        //
        STDMETHOD(GetName)(LPWSTR pwszName, UINT nMaxChars);

        //
        // The IMDSPStorage::GetRights method, which returns the rights
        // information for this storage object.
        //
        STDMETHOD(GetRights)(PWMDMRIGHTS *ppRights, UINT *pnRightsCount,
                             BYTE abMac[WMDM_MAC_LENGTH]);

        //
        // The IMDSPStorage::GetSize method, which returns the size of this
        // storage object.
        //
        STDMETHOD(GetSize)(DWORD *pdwSizeLow, DWORD *pdwSizeHigh);

        //
        // The IMDSPStorage::GetStorageGlobals method, which returns the
        // IMDSPStorageGlobals interface which provides access to global
        // information about the storage medium.
        //
        STDMETHOD(GetStorageGlobals)(IMDSPStorageGlobals **ppStorageGlobals);

        //
        // The IMDSPStorage::SendOpaqueCommand method, which sends an
        // unmodified command to this storage object.
        //
        STDMETHOD(SendOpaqueCommand)(OPAQUECOMMAND *pCommand);

        //
        // The IMDSPStorage::SetAttribute method, which sets the attributes of
        // this storage object.
        //
        STDMETHOD(SetAttributes)(DWORD dwAttributes, _WAVEFORMATEX *pFormat);

        //
        // The IMDSPObjectInfo::GetLastPlayPosition method, which returns the
        // last play position of this storage object.
        //
        STDMETHOD(GetLastPlayPosition)(DWORD *pdwLastPos);

        //
        // The IMDSPObjectInfo::GetLongestPlayPosition method, which returns
        // the longest play position of this storage object.
        //
        STDMETHOD(GetLongestPlayPosition)(DWORD *pdwLongestPos);

        //
        // The IMDSPObjectInfo::GetPlayLength method, which returns the play
        // length of this storage object.
        //
        STDMETHOD(GetPlayLength)(DWORD *pdwLength);

        //
        // The IMDSPObjectInfo::GetPlayOffset method, which returns the current
        // play offset of this storage object.
        //
        STDMETHOD(GetPlayOffset)(DWORD *pdwOffset);

        //
        // The IMDSPObjectInfo::GetTotalLength method, which returns the total
        // play length of this storage object.
        //
        STDMETHOD(GetTotalLength)(DWORD *pdwLength);

        //
        // The IMDSPObjectInfo::SetPlayLength method, which sets the play
        // length of this storage object.
        //
        STDMETHOD(SetPlayLength)(DWORD dwLength);

        //
        // The IMDSPObjectInfo::SetPlayOffset method, which sets the current
        // play offset of this storage object.
        //
        STDMETHOD(SetPlayOffset)(DWORD dwOffset);

        //
        // The IMDSPObject::Close method, which closes this storage object.
        //
        STDMETHOD(Close)(void);

        //
        // The IMDSPObject::Delete method, which deletes this storage object
        // from the storage medium.
        //
        STDMETHOD(Delete)(UINT fuMode, IWMDMProgress *pProgress);

        //
        // The IMDSPObject::Move method, which moves this storage object to
        // another storage object.
        //
        STDMETHOD(Move)(UINT fuMode, IWMDMProgress *pProgress,
                        IMDSPStorage *pTarget);

        //
        // The IMDSPObject::Open method, which opens this storage object and
        // prepares it for other operations.
        //
        STDMETHOD(Open)(UINT fuMode);

        //
        // The IMDSPObject::Read method, which reads data from this storage
        // object.
        //
        STDMETHOD(Read)(BYTE *pData, DWORD *pdwSize,
                        BYTE abMac[WMDM_MAC_LENGTH]);

        //
        // The IMDSPObject::Rename method, which renames this storage object.
        //
        STDMETHOD(Rename)(LPWSTR pwszNewName, IWMDMProgress *pProgress);

        //
        // The IMDSPObject::Seek method, which sets the current position within
        // this storage object.
        //
        STDMETHOD(Seek)(UINT fuFlags, DWORD dwOffset);

        //
        // The IMDSPObject::Write method, which writes data to this storage
        // object.
        //
        STDMETHOD(Write)(BYTE *pData, DWORD *pdwSize,
                         BYTE abMac[WMDM_MAC_LENGTH]);

    //
    // The private members of this object.
    //
    private:
        //
        // The drive on which this storage object resides (or which it
        // represents).
        //
        unsigned long m_ulDrive;

        //
        // The name of this storage object.
        //
        WCHAR *m_pwcName;

        //
        // An indicator which is TRUE if this storage object represents a
        // drive and FALSE if this storage object represents a file.
        //
        unsigned long m_bIsRoot;

        //
        // The size of the file represented by this storage object.
        //
        unsigned long m_ulSize;

        //
        // The date/time of the most recent modification of the file
        // represented by this storage object.
        //
        unsigned long m_ulDate;

        //
        // An indicator which is TRUE if this storage object is currently
        // openeded.
        //
        unsigned long m_bIsOpen;
};

#endif //__MDSPSTORAGE_H_

//****************************************************************************
//
// DEVICE.H - Definition of the CMDSPDevice class, which implements the
//            IMDSPDevice interface.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
// Copyright (C) Microsoft Corporation, 1999 - 2001.  All rights reserved.
//
//****************************************************************************
#ifndef __MDSPDEVICE_H_
#define __MDSPDEVICE_H_

//****************************************************************************
//
// The CMDSPDevice class, which implements the IMDSPDevice and
// IMDSPDeviceControl interfaces.
//
//****************************************************************************
class ATL_NO_VTABLE CMDSPDevice :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CMDSPDevice, &CLSID_MDSPDevice>,
    public IMDSPDevice, IMDSPDeviceControl
{
    //
    // The public methods of this object.
    //
    public:
        //
        // Sets the drive with which this device is associated.
        //
        void SetDrive(unsigned long ulDrive);

        //
        // Define the resource identifier for the registry information
        // associated with this object.
        //
        DECLARE_REGISTRY_RESOURCEID(IDR_MDSPDEVICE)

        //
        // Define the list of interfaces supported through COM by this object.
        //
        BEGIN_COM_MAP(CMDSPDevice)
            COM_INTERFACE_ENTRY(IMDSPDevice)
            COM_INTERFACE_ENTRY(IMDSPDeviceControl)
        END_COM_MAP()

        //
        // The IMDSPDevice::GetDeviceIcon method, which returns a HICON to be
        // used to represent the device.
        //
        STDMETHOD(GetDeviceIcon)(ULONG *hIcon);

        //
        // The IMDSPDevice::GetFormatSupport method, which returns a list of
        // the formats supported by the device.
        //
        STDMETHOD(GetFormatSupport)(_WAVEFORMATEX **pFormatEx,
                                    UINT *pnFormatCount,
                                    LPWSTR **pppwszMimeType,
                                    UINT *pnMimeTypeCount);

        //
        // The IMDSPDevice::EnumStorage method, which returns a storage
        // enumerator.
        //
        STDMETHOD(EnumStorage)(IMDSPEnumStorage **ppEnumStorage);

        //
        // The IMDSPDevice::GetManufacturer method, which returns the name of
        // the device manufacturer.
        //
        STDMETHOD(GetManufacturer)(LPWSTR pwszName, UINT nMaxChars);

        //
        // The IMDSPDevice::GetName method, which returns the name of the
        // device.
        //
        STDMETHOD(GetName)(LPWSTR pwszName, UINT nMaxChars);

        //
        // The IMDSPDevice::GetPowerSource method, which returns the current
        // power source of the device.
        //
        STDMETHOD(GetPowerSource)(DWORD *pdwPowerSource,
                                  DWORD *pdwPercentRemaining);

        //
        // The IMDSPDevice::GetSerialNumber method, which returns the unique
        // serial number of the device.
        //
        STDMETHOD(GetSerialNumber)(PWMDMID pSerialNumber,
                                   BYTE abMac[WMDM_MAC_LENGTH]);

        //
        // The IMDSPDevice::GetStatus method, which returns the current device
        // status.
        //
        STDMETHOD(GetStatus)(DWORD *pdwStatus);

        //
        // The IMDSPDevice::GetType method, which returns the device type
        // information.
        //
        STDMETHOD(GetType)(DWORD *pdwType);

        //
        // The IMDSPDevice::GetVersion method, which returns the version number
        // of the device.
        //
        STDMETHOD(GetVersion)(DWORD *pdwVersion);

        //
        // The IMDSPDevice::SendOpaqueCommand method, which sends an opaque
        // command to the device.
        //
        STDMETHOD(SendOpaqueCommand)(OPAQUECOMMAND *pCommand);

        //
        // The IMDSPDeviceControl::GetCapabilities method, which returns the
        // control capabilities of the device.
        //
        STDMETHOD(GetCapabilities)(DWORD *pdwCapabilitiesMask);

        //
        // The IMDSPDeviceControl::GetDCStatus method, which returns the
        // control status of the device.
        //
        STDMETHOD(GetDCStatus)(DWORD *pdwStatus);

        //
        // The IMDSPDeviceControl::Pause method, which pauses the playback or
        // record on the device.
        //
        STDMETHOD(Pause)(void);

        //
        // The IMDSPDeviceControl::Play method, which starts playback on the
        // device.
        //
        STDMETHOD(Play)(void);

        //
        // The IMDSPDeviceControl::Record method, which starts recording on the
        // device.
        //
        STDMETHOD(Record)(_WAVEFORMATEX *pFormat);

        //
        // The IMDSPDeviceControl::Resume method, which resumes the paused
        // playback or record on the device.
        //
        STDMETHOD(Resume)(void);

        //
        // The IMDSPDeviceControl::Seek method, which seeks to the specified
        // position on the device.
        //
        STDMETHOD(Seek)(UINT fuMode, int nOffset);

        //
        // The IMDSPDeviceControl::Stop method, which stops the playback or
        // record on the device.
        //
        STDMETHOD(Stop)(void);

    //
    // The private members of this object.
    //
    private:
        //
        // The drive number of this device.
        //
        unsigned long m_ulDrive;
};

#endif //__MDSPDEVICE_H_

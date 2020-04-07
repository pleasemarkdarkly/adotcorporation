//****************************************************************************
//
// MAVINFO.H - Definition of the CMaverickInfo class.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
//
//****************************************************************************
#ifndef __MAVERICKINFO_H
#define __MAVERICKINFO_H

//****************************************************************************
//
// The CMaverickInfo class.
//
//****************************************************************************
class CMaverickInfo
{
    //
    // The public methods of this object.
    //
    public:
        //
        // The constructor.
        //
        CMaverickInfo(void);

        //
        // The destructor.
        //
        ~CMaverickInfo(void);

        //
        // Performs initialization of this object.
        //
        void Initialize(void);

        //
        // Frees the databases and resets this object.
        //
        void Reset(void);

        //
        // Refreshes the local copy of the list of files on the device.
        //
        void Refresh(unsigned long ulDrive);

        //
        // Return the serial number of the device.
        //
        void GetSerialNumber(unsigned long ulDrive, WMDMID *pID);

        //
        // Returns the name of the device manufacturer.
        //
        void GetManufacturerName(WCHAR *pcName, unsigned long ulLength);

        //
        // Returns the version number of the device.
        //
        unsigned long GetVersion(void);

        //
        // Returns the number of drives on the device.
        //
        unsigned long GetNumDrives(void);

        //
        // Returns the name of the given drive.
        //
        void GetDriveName(unsigned long ulDrive, WCHAR *pcName,
                          unsigned long ulLength);

        //
        // Returns the number of files on the given drive.
        //
        unsigned long GetNumFiles(unsigned long ulDrive);

        //
        // Returns the name of the given file on the given drive.
        //
        void GetFileName(unsigned long ulDrive, unsigned long ulFile,
                         WCHAR *pcName, unsigned long ulLength);

        //
        // Returns the length of the given file on the given drive.
        //
        unsigned long GetFileLength(unsigned long ulDrive,
                                    unsigned long ulFile);

        //
        // Returns the modification date/time of the given file on the given
        // drive.
        //
        unsigned long GetFileDate(unsigned long ulDrive, unsigned long ulFile);

        //
        // Returns the storage globals object for the given drive.
        //
        IMDSPStorageGlobals *GetStorageGlobals(unsigned long ulDrive);

        //
        // Returns the device object for the device.
        //
        IMDSPDevice *GetDevice(unsigned long ulDrive);

    //
    // The private members of this object.
    //
    private:
        //
        // A critical section to protect access to the state of this object.
        //
        CComMultiThreadModel::AutoCriticalSection m_CriticalSection;

        //
        // The name of the device manufacturer.
        //
        WCHAR *m_pcManufacturerName;

        //
        // The version number of the device.
        //
        unsigned long m_ulVersion;

        //
        // The number of drives on the device.
        //
        unsigned long m_ulNumDrives;

        //
        // An array of serial numbers for each of the drives.
        //
        WMDMID *m_pID;

        //
        // An array of strings containing the names of the drives.
        //
        WCHAR **m_ppcDriveNames;

        //
        // The number of files on each drive of the device.
        //
        unsigned long *m_pulNumFiles;

        //
        // Strings of strings containing the names of the files on each drive.
        //
        WCHAR **m_ppcFileNames;

        //
        // The length of each file on each drive.
        //
        unsigned long **m_ppulFileSizes;

        //
        // The modification date of each file on each drive.
        //
        unsigned long **m_ppulFileDates;

        //
        // An array of device objects for each of the drives.
        //
        IMDSPDevice **m_ppDevice;

        //
        // An array of storage globals objects for each of the drives.
        //
        IMDSPStorageGlobals **m_ppStorageGlobals;
};
#endif

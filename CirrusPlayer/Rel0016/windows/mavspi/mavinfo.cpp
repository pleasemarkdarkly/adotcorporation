//****************************************************************************
//
// MAVINFO.CPP - The CMaverickInfo class.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include "stdafx.h"

//****************************************************************************
//
// TimeToSeconds converts the specified time as month, day, year, hour,
// minute, and second into the number of seconds since January 1, 1970.
//
//****************************************************************************
static unsigned long
TimeToSeconds(unsigned long ulYear, unsigned long ulMonth, unsigned long ulDay,
              unsigned long ulHour, unsigned long ulMinute,
              unsigned long ulSecond)
{
    unsigned char ucMonthLength[12] = { 31, 29, 31, 30, 31, 30,
                                        31, 31, 30, 31, 30, 31 };
    unsigned long ulRet, ulIdx;

    //
    // Compute the number of days that have past in this year.
    //
    ulRet = ulDay - 1;
    ulIdx = ulMonth - 1;
    while(ulIdx--)
    {
        ulRet += ucMonthLength[ulIdx];
    }

    //
    // If this is a non-leap year and the day is past February 28th, then
    // subtract the count of days by one (since we treat each year as if it
    // were a leap year).
    //
    if(((ulYear & 3) != 0) && (ulRet >= (31 + 28)))
    {
        ulRet--;
    }

    //
    // Add the days for the years that have past.
    //
    ulIdx = ulYear - 1970;
    ulRet += (ulIdx * 365) + ((ulIdx + 1) / 4);

    //
    // Convert the days to hours and add the current hour.
    //
    ulRet = (ulRet * 24) + ulHour;

    //
    // Convert the hours to minutes and add the current minute.
    //
    ulRet = (ulRet * 60) + ulMinute;

    //
    // Convert the minutes to seconds and add the current second.
    //
    ulRet = (ulRet * 60) + ulSecond;

    //
    // Return the converted time.
    //
    return(ulRet);
}

//****************************************************************************
//
// Checks the clock on the player and updates it if it is "too far" out of
// sync with the host clock.
//
//****************************************************************************
static void
CheckTime(void)
{
    SYSTEMTIME sLocalTime;
    unsigned long ulLength, ulTime, ulPlayerTime;

    //
    // Get the current time from the player.
    //
    if(Maverick_GetDescriptor(USB_Vendor_Descriptor_CurrentTime, 0,
                              (char *)&ulPlayerTime, &ulLength) == 0)
    {
        //
        // We could not get the current time from the player, so do nothing
        // further.
        //
        return;
    }

    //
    // Get the current host time.
    //
    GetLocalTime(&sLocalTime);

    //
    // Convert the current host time into seconds.
    //
    ulTime = TimeToSeconds(sLocalTime.wYear, sLocalTime.wMonth,
                           sLocalTime.wDay, sLocalTime.wHour,
                           sLocalTime.wMinute, sLocalTime.wSecond);

    //
    // See if the current player time is within 10 seconds on either side of
    // the current host time.
    //
    if(((ulTime < ulPlayerTime) && ((ulPlayerTime - ulTime) > 10)) ||
       ((ulTime > ulPlayerTime) && ((ulTime - ulPlayerTime) > 10)))
    {
        //
        // The current player time is more than 10 seconds different than the
        // current host time, so change the player time to the current host
        // time.
        //
        Maverick_SetDescriptor(USB_Vendor_Descriptor_CurrentTime, 0,
                               (char *)&ulTime, 4);
    }
}

//****************************************************************************
//
// The constructor for the CMaverickInfo class.
//
//****************************************************************************
CMaverickInfo::CMaverickInfo(void)
{
    //
    // Initialize all the pointers to NULL, and the number of drives to zero.
    //
    m_pID = NULL;
    m_pcManufacturerName = NULL;
    m_ulNumDrives = 0;
    m_ppcDriveNames = NULL;
    m_pulNumFiles = NULL;
    m_ppcFileNames = NULL;
    m_ppulFileSizes = NULL;
    m_ppulFileDates = NULL;
    m_ppDevice = NULL;
    m_ppStorageGlobals = NULL;
}

//****************************************************************************
//
// The destructor for the CMaverickInfo class.
//
//****************************************************************************
CMaverickInfo::~CMaverickInfo(void)
{
    //
    // Reset before we go away (which frees up any memory which has been
    // allocated).
    //
    Reset();
}

//****************************************************************************
//
// Performs initialization of this object.
//
//****************************************************************************
void
CMaverickInfo::Initialize(void)
{
    unsigned long ulDrive, ulLength;

    //
    // Obtain the critical section.
    //
    m_CriticalSection.Lock();

    //
    // Open the Maverick(tm) driver.
    //
    if(Maverick_OpenDevice() == 0)
    {
        //
        // There is no device attached now, so set the number of drives to
        // zero and return.
        //
        m_ulNumDrives = 0;
        m_CriticalSection.Unlock();
        return;
    }

    //
    // Check and possibly update the time on the player.
    //
    CheckTime();

    //
    // Get the number of drives on the device.
    //
    m_ulNumDrives = Maverick_NumDrives();

    //
    // Allocate memory for the manufacturer name.
    //
    m_pcManufacturerName = new WCHAR[128];
    if(!m_pcManufacturerName)
    {
        m_CriticalSection.Unlock();
        return;
    }

    //
    // Get the name of the device manufacturer.
    //
    if(Maverick_GetManufacturerName(m_pcManufacturerName, 256) == 0)
    {
        wcscpy(m_pcManufacturerName, L"Unknown");
    }

    //
    // Get the version of the hardware.
    //
    ulLength = sizeof(unsigned long);
    if(Maverick_GetDescriptor(USB_Vendor_Descriptor_FirmwareVersion, 0,
                              &m_ulVersion, &ulLength) == 0)
    {
        m_ulVersion = 0;
    }

    //
    // Allocate memory for the serial number array.
    //
    m_pID = new WMDMID[m_ulNumDrives];
    if(!m_pID)
    {
        m_CriticalSection.Unlock();
        return;
    }

    //
    // Allocate memory for the array of drive names.
    //
    m_ppcDriveNames = new WCHAR *[m_ulNumDrives];
    if(!m_ppcDriveNames)
    {
        m_CriticalSection.Unlock();
        return;
    }

    //
    // Loop through the drives.
    //
    for(ulDrive = 0; ulDrive < m_ulNumDrives; ulDrive++)
    {
        //
        // Allocate memory for the drive name.
        //
        m_ppcDriveNames[ulDrive] = new WCHAR[128];

        //
        // Get the name of this drive.
        //
        ulLength = 128;
        if(Maverick_GetDescriptor(USB_Vendor_Descriptor_MediaName, ulDrive,
                                  m_ppcDriveNames[ulDrive], &ulLength) == 0)
        {
            wcscpy(m_ppcDriveNames[ulDrive], L"Unknown");
        }

        //
        // Get the media ID for this drive.
        //
        ulLength = WMDMID_LENGTH;
        if(Maverick_GetDescriptor(USB_Vendor_Descriptor_MediaID, ulDrive,
                                  m_pID[ulDrive].pID, &ulLength) != 0)
        {
            m_pID[ulDrive].cbSize = sizeof(WMDMID);
            m_pID[ulDrive].SerialNumberLength = ulLength;
        }
    }

    //
    // Allocate memory for the number of files array.
    //
    m_pulNumFiles = new unsigned long[m_ulNumDrives];
    if(!m_pulNumFiles)
    {
        m_CriticalSection.Unlock();
        return;
    }

    //
    // Allocate memory for the file names array.
    //
    m_ppcFileNames = new WCHAR *[m_ulNumDrives];
    if(!m_ppcFileNames)
    {
        m_CriticalSection.Unlock();
        return;
    }

    //
    // Allocate memory for the file sizes array.
    //
    m_ppulFileSizes = new unsigned long *[m_ulNumDrives];
    if(!m_ppulFileSizes)
    {
        m_CriticalSection.Unlock();
        return;
    }

    //
    // Allocate memory for the file modification date array.
    //
    m_ppulFileDates = new unsigned long *[m_ulNumDrives];
    if(!m_ppulFileSizes)
    {
        m_CriticalSection.Unlock();
        return;
    }

    //
    // Allocate memory for the device object array.
    //
    m_ppDevice = new IMDSPDevice *[m_ulNumDrives];
    if(!m_ppDevice)
    {
        m_CriticalSection.Unlock();
        return;
    }

    //
    // Allocate memory for the storage globals object array.
    //
    m_ppStorageGlobals = new IMDSPStorageGlobals *[m_ulNumDrives];
    if(!m_ppStorageGlobals)
    {
        m_CriticalSection.Unlock();
        return;
    }

    //
    // Loop through the drives.
    //
    for(ulDrive = 0; ulDrive < m_ulNumDrives; ulDrive++)
    {
        //
        // Initialize the information for this drive.
        //
        m_pulNumFiles[ulDrive] = 0;
        m_ppcFileNames[ulDrive] = NULL;
        m_ppulFileSizes[ulDrive] = NULL;
        m_ppulFileDates[ulDrive] = NULL;
        m_ppDevice[ulDrive] = NULL;
        m_ppStorageGlobals[ulDrive] = NULL;

        //
        // Refresh this drive.
        //
        Refresh(ulDrive);
    }

    //
    // Release the critical section.
    //
    m_CriticalSection.Unlock();
}

//****************************************************************************
//
// Frees the databases and resets this object.
//
//****************************************************************************
void
CMaverickInfo::Reset(void)
{
    unsigned long ulDrive;

    //
    // Obtain the critical section.
    //
    m_CriticalSection.Lock();

    //
    // If there is a device manufacturer name string allocated, then delete it
    // now.
    //
    if(m_pcManufacturerName)
    {
        delete [] m_pcManufacturerName;
        m_pcManufacturerName = NULL;
    }

    //
    // If there is a serial number array allocated, then delete it now.
    //
    if(m_pID)
    {
        delete [] m_pID;
        m_pID = NULL;
    }

    //
    // If there is a number of files array allocated, then delete it now.
    //
    if(m_pulNumFiles)
    {
        delete [] m_pulNumFiles;
        m_pulNumFiles = NULL;
    }

    //
    // Loop through all the drives.
    //
    for(ulDrive = 0; ulDrive < m_ulNumDrives; ulDrive++)
    {
        //
        // If there is a drive name string allocated for this drive, then
        // delete it now.
        //
        if(m_ppcDriveNames && m_ppcDriveNames[ulDrive])
        {
            delete [] m_ppcDriveNames[ulDrive];
        }

        //
        // If there is a file name string allocated for this drive, then delete
        // it now.
        //
        if(m_ppcFileNames && m_ppcFileNames[ulDrive])
        {
            delete [] m_ppcFileNames[ulDrive];
        }

        //
        // If there is a file size array allocated for this drive, then delete
        // it now.
        //
        if(m_ppulFileSizes && m_ppulFileSizes[ulDrive])
        {
            delete [] m_ppulFileSizes[ulDrive];
        }

        //
        // If there is a file date array allocated for this drive, then delete
        // it now.
        //
        if(m_ppulFileDates && m_ppulFileDates[ulDrive])
        {
            delete [] m_ppulFileDates[ulDrive];
        }

        //
        // If there is a device object for this drive, then release it now.
        //
        if(m_ppDevice && m_ppDevice[ulDrive])
        {
            m_ppDevice[ulDrive]->Release();
        }

        //
        // If there is a storage globals object for this drive, then release it
        // now.
        //
        if(m_ppStorageGlobals && m_ppStorageGlobals[ulDrive])
        {
            m_ppStorageGlobals[ulDrive]->Release();
        }
    }

    //
    // If there is a drive names array allocated, then delete it now.
    //
    if(m_ppcDriveNames)
    {
        delete [] m_ppcDriveNames;
        m_ppcDriveNames = NULL;
    }

    //
    // If there is a file name array allocated, then delete it now.
    //
    if(m_ppcFileNames)
    {
        delete [] m_ppcFileNames;
        m_ppcFileNames = NULL;
    }

    //
    // If there is a file size array allocated, then delete it now.
    //
    if(m_ppulFileSizes)
    {
        delete [] m_ppulFileSizes;
        m_ppulFileSizes = NULL;
    }

    //
    // If there is a file date array allocated, then delete it now.
    //
    if(m_ppulFileDates)
    {
        delete [] m_ppulFileDates;
        m_ppulFileDates = NULL;
    }

    //
    // If there is a device array allocated, then delete it now.
    //
    if(m_ppDevice)
    {
        delete [] m_ppDevice;
        m_ppDevice = NULL;
    }

    //
    // If there is a storage globals array allocated, then delete it now.
    //
    if(m_ppStorageGlobals)
    {
        delete [] m_ppStorageGlobals;
        m_ppStorageGlobals = NULL;
    }

    //
    // Reset the number of drives to zero.
    //
    m_ulNumDrives = 0;

    //
    // Release the critical section.
    //
    m_CriticalSection.Unlock();

    //
    // Close the Maverick(tm) driver.
    //
    Maverick_CloseDevice();
}

//****************************************************************************
//
// Refreshes the local copy of the list of files on the device.
//
//****************************************************************************
void
CMaverickInfo::Refresh(unsigned long ulDrive)
{
    WCHAR *pwcString = NULL, *pwcTemp, pwcName[256];
    unsigned long *pulLength = NULL, *pulDate = NULL, *pulTemp;
    unsigned long ulNumFiles = 0, ulFiles = 0, ulStrLen = 0;

    //
    // Open the root directory on this drive.
    //
    if(Maverick_OpenDir(ulDrive, L"\\") == 1)
    {
        //
        // Loop through all the files on this drive.
        //
        while(Maverick_ReadDir(pwcName, 1) == 1)
        {
            //
            // Attempt to open this file.
            //
            if(Maverick_Open(ulDrive, pwcName, 1) == 0)
            {
                //
                // We could not open this file, so we'll leave the database as
                // is for now.
                //
                return;
            }

            //
            // See if the current file name string length is zero.
            //
            if(ulStrLen == 0)
            {
                //
                // Allocate memory to hold the name of this file.
                //
                pwcString = new WCHAR[wcslen(pwcName) + 1];
            }
            else
            {
                //
                // Allocate memory to hold the names of all the previous files,
                // along with the name of this file.
                //
                pwcTemp = new WCHAR[ulStrLen + wcslen(pwcName) + 1];

                //
                // Copy the names of all the previous files to the newly
                // allocated string.
                //
                memcpy(pwcTemp, pwcString, ulStrLen * sizeof(WCHAR));

                //
                // Delete the previous string.
                //
                delete [] pwcString;

                //
                // Use the newly allocated string.
                //
                pwcString = pwcTemp;
            }

            //
            // Copy the name of this file to the end of the string of file
            // names.
            //
            wcscpy(pwcString + ulStrLen, pwcName);

            //
            // Increment the length of the file names string by the length of
            // the name of this file.
            //
            ulStrLen += wcslen(pwcName) + 1;

            //
            // See if there is any more space in the length and date arrays.
            //
            if(ulNumFiles == ulFiles)
            {
                //
                // See if we have allocated the length and date arrays.
                //
                if(!ulFiles)
                {
                    //
                    // Set the initial size of the length and date arrays at
                    // 16.
                    //
                    ulFiles = 16;

                    //
                    // Allocate the array to hold the file lengths.
                    //
                    pulLength = new unsigned long[ulFiles];

                    //
                    // Allocate the array to hold the file dates.
                    //
                    pulDate = new unsigned long[ulFiles];
                }
                else
                {
                    //
                    // Double the size of the length and date arrays.
                    //
                    ulFiles *= 2;

                    //
                    // Allocate a new array to hold the file lengths.
                    //
                    pulTemp = new unsigned long[ulFiles];

                    //
                    // Copy the existing file lengths to the new array.
                    //
                    memcpy(pulTemp, pulLength,
                           ulNumFiles * sizeof(unsigned long));

                    //
                    // Delete the existing array for the file lengths.
                    //
                    delete [] pulLength;

                    //
                    // Use the newly allocated array for the file lengths.
                    //
                    pulLength = pulTemp;

                    //
                    // Allocate a new array to hold the file dates.
                    //
                    pulTemp = new unsigned long[ulFiles];

                    //
                    // Copy the existing file dates to the new array.
                    //
                    memcpy(pulTemp, pulDate,
                           ulNumFiles * sizeof(unsigned long));

                    //
                    // Delete the existing array for the file dates.
                    //
                    delete [] pulDate;

                    //
                    // Use the newly allocated array for the file dates.
                    //
                    pulDate = pulTemp;
                }
            }

            //
            // Get the length of this file.
            //
            pulLength[ulNumFiles] = Maverick_Length();

            //
            // Get the modification date/time of this file.
            //
            pulDate[ulNumFiles] = Maverick_GetDate();

            //
            // Close this file.
            //
            Maverick_Close();

            //
            // Increment the number of files on this drive.
            //
            ulNumFiles++;
        }

        //
        // Close the root directory.
        //
        Maverick_CloseDir();
    }

    //
    // Save the number of files on this drive.
    //
    m_pulNumFiles[ulDrive] = ulNumFiles;

    //
    // Save the string of names of the files on this drive.
    //
    if(m_ppcFileNames[ulDrive])
    {
        delete [] m_ppcFileNames[ulDrive];
    }
    m_ppcFileNames[ulDrive] = pwcString;

    //
    // Save the array of file lengths.
    //
    if(m_ppulFileSizes[ulDrive])
    {
        delete [] m_ppulFileSizes[ulDrive];
    }
    m_ppulFileSizes[ulDrive] = pulLength;

    //
    // Save the array of file dates.
    //
    if(m_ppulFileDates[ulDrive])
    {
        delete [] m_ppulFileDates[ulDrive];
    }
    m_ppulFileDates[ulDrive] = pulDate;
}

//****************************************************************************
//
// Returns the serial number of the device.
//
//****************************************************************************
void
CMaverickInfo::GetSerialNumber(unsigned long ulDrive, WMDMID *pID)
{
    //
    // Obtain the critical section.
    //
    m_CriticalSection.Lock();

    //
    // Make sure that:
    //   1) the specified drive is valid, and
    //   2) we have an array of serial numbers.
    //
    if((ulDrive >= m_ulNumDrives) || !m_pID)
    {
        m_CriticalSection.Unlock();
        pID->cbSize = 0;
        return;
    }

    //
    // Return our serial number in the WMDMID structure.
    //
    *pID = m_pID[ulDrive];

    //
    // Release the critical section.
    //
    m_CriticalSection.Unlock();
}

//****************************************************************************
//
// Returns the name of the device manufacturer.
//
//****************************************************************************
void
CMaverickInfo::GetManufacturerName(WCHAR *pcName, unsigned long ulLength)
{
    //
    // Obtain the critical section.
    //
    m_CriticalSection.Lock();

    //
    // Return the device manufacturer name.
    //
    if(!m_pcManufacturerName)
    {
        pcName[0] = L'\0';
    }
    else
    {
        wcsncpy(pcName, m_pcManufacturerName, ulLength);
    }

    //
    // Release the critical section.
    //
    m_CriticalSection.Unlock();
}

//****************************************************************************
//
// Returns the version number of the device.
//
//****************************************************************************
unsigned long
CMaverickInfo::GetVersion(void)
{
    //
    // Return the version number.
    //
    return(m_ulVersion);
}

//****************************************************************************
//
// Returns the number of drives on the device.
//
//****************************************************************************
unsigned long
CMaverickInfo::GetNumDrives(void)
{
    //
    // Return the number of drives.
    //
    return(m_ulNumDrives);
}

//****************************************************************************
//
// Returns the name of the given drive.
//
//****************************************************************************
void
CMaverickInfo::GetDriveName(unsigned long ulDrive, WCHAR *pcName,
                            unsigned long ulLength)
{
    //
    // Obtain the critical section.
    //
    m_CriticalSection.Lock();

    //
    // Make sure that:
    //   1) the specified drive is valid, and
    //   2) we have a string of drive names.
    //
    if((ulDrive >= m_ulNumDrives) || !m_ppcDriveNames)
    {
        m_CriticalSection.Unlock();
        pcName[0] = L'\0';
        return;
    }

    //
    // Return the name of this drive.
    //
    wcsncpy(pcName, m_ppcDriveNames[ulDrive], ulLength);

    //
    // Release the critical section.
    //
    m_CriticalSection.Unlock();
}

//****************************************************************************
//
// Returns the number of files on the given drive.
//
//****************************************************************************
unsigned long
CMaverickInfo::GetNumFiles(unsigned long ulDrive)
{
    unsigned long ulNumFiles;

    //
    // Obtain the critical section.
    //
    m_CriticalSection.Lock();

    //
    // Make sure that:
    //   1) the specified drive is valid, and
    //   2) we have an array of the number of files per drive.
    //
    if((ulDrive >= m_ulNumDrives) || !m_pulNumFiles)
    {
        m_CriticalSection.Unlock();
        return(0);
    }

    //
    // Get the number of files on this drive.
    //
    ulNumFiles = m_pulNumFiles[ulDrive];

    //
    // Release the critical section.
    //
    m_CriticalSection.Unlock();

    //
    // Return the number of files on this drive.
    //
    return(ulNumFiles);
}

//****************************************************************************
//
// Returns the name of the given file on the given drive.
//
//****************************************************************************
void
CMaverickInfo::GetFileName(unsigned long ulDrive, unsigned long ulFile,
                           WCHAR *pcName, unsigned long ulLength)
{
    WCHAR *pcFileName;

    //
    // Obtain the critical section.
    //
    m_CriticalSection.Lock();

    //
    // Make sure that:
    //   1) the specified drive is valid,
    //   2) we have an array of the number of files per drive,
    //   3) the specified file is valid,
    //   4) we have an array of the names of the files on each drive, and
    //   5) we have a string of the file names on this drive.
    //
    if((ulDrive >= m_ulNumDrives) || !m_pulNumFiles ||
       (ulFile >= m_pulNumFiles[ulDrive]) || !m_ppcFileNames ||
       !m_ppcFileNames[ulDrive])
    {
        m_CriticalSection.Unlock();
        pcName[0] = L'\0';
        return;
    }

    //
    // Get a local copy of the file name string.
    //
    pcFileName = m_ppcFileNames[ulDrive];

    //
    // Find the specified file.
    //
    while(ulFile--)
    {
        //
        // Skip to the next file name.
        //
        pcFileName += wcslen(pcFileName) + 1;
    }

    //
    // Return the name of this file.
    //
    wcsncpy(pcName, pcFileName, ulLength);

    //
    // Release the critical section.
    //
    m_CriticalSection.Unlock();
}

//****************************************************************************
//
// Returns the length of the given file on the given drive.
//
//****************************************************************************
unsigned long
CMaverickInfo::GetFileLength(unsigned long ulDrive, unsigned long ulFile)
{
    unsigned long ulFileSize;

    //
    // Obtain the critical section.
    //
    m_CriticalSection.Lock();

    //
    // Make sure that:
    //   1) the specified drive is valid,
    //   2) we have an array of the number of files per drive,
    //   3) the specified file is valid,
    //   4) we have an array of file sizes per drive, and
    //   5) we have an array of file sizes for this drive.
    //
    if((ulDrive >= m_ulNumDrives) || !m_pulNumFiles ||
       (ulFile >= m_pulNumFiles[ulDrive]) || !m_ppulFileSizes ||
       !m_ppulFileSizes[ulDrive])
    {
        ulFileSize = 0;
    }
    else
    {
        //
        // Get the length of this file.
        //
        ulFileSize = m_ppulFileSizes[ulDrive][ulFile];
    }

    //
    // Release the critical section.
    //
    m_CriticalSection.Unlock();

    //
    // Return the length of this file.
    //
    return(ulFileSize);
}

//****************************************************************************
//
// Returns the modification date/time of the given file on the given drive.
//
//****************************************************************************
unsigned long
CMaverickInfo::GetFileDate(unsigned long ulDrive, unsigned long ulFile)
{
    unsigned long ulFileSize;

    //
    // Obtain the critical section.
    //
    m_CriticalSection.Lock();

    //
    // Make sure that:
    //   1) the specified drive is valid,
    //   2) we have an array of the number of files per drive,
    //   3) the specified file is valid,
    //   4) we have an array of file sizes per drive, and
    //   5) we have an array of file sizes for this drive.
    //
    if((ulDrive >= m_ulNumDrives) || !m_pulNumFiles ||
       (ulFile >= m_pulNumFiles[ulDrive]) || !m_ppulFileDates ||
       !m_ppulFileDates[ulDrive])
    {
        ulFileSize = 0;
    }
    else
    {
        //
        // Get the modification date/time of this file.
        //
        ulFileSize = m_ppulFileDates[ulDrive][ulFile];
    }

    //
    // Release the critical section.
    //
    m_CriticalSection.Unlock();

    //
    // Return the length of this file.
    //
    return(ulFileSize);
}

//****************************************************************************
//
// Returns the storage globals object for the given drive.
//
//****************************************************************************
IMDSPStorageGlobals *
CMaverickInfo::GetStorageGlobals(unsigned long ulDrive)
{
    CComObject<CMDSPStorageGlobals> *pObj;
    IMDSPStorageGlobals *pGlobals;
    HRESULT hr;

    //
    // Obtain the critical section.
    //
    m_CriticalSection.Lock();

    //
    // Make sure that:
    //   1) the specified drive is valid, and
    //   2) we have an array of storage globals objects.
    //
    if((ulDrive >= m_ulNumDrives) || !m_ppStorageGlobals)
    {
        m_CriticalSection.Unlock();
        return(NULL);
    }

    //
    // See if there is already a storage globals object for this drive.
    //
    if(m_ppStorageGlobals[ulDrive])
    {
        //
        // There is already a storage globals object for this drive, so add a
        // reference to that object.
        //
        m_ppStorageGlobals[ulDrive]->AddRef();

        //
        // Release the critical section.
        //
        m_CriticalSection.Unlock();

        //
        // Return the storage globals object for this drive.
        //
        return(m_ppStorageGlobals[ulDrive]);
    }

    //
    // Create a new storage globals object.
    //
    hr = CComObject<CMDSPStorageGlobals>::CreateInstance(&pObj);
    if(FAILED(hr))
    {
        m_CriticalSection.Unlock();
        return(NULL);
    }

    //
    // Get a pointer to the IMDSPStorageGlobals interface of the object.
    //
    hr = pObj->QueryInterface(IID_IMDSPStorageGlobals,
                              reinterpret_cast<void **>(&pGlobals));
    if(FAILED(hr))
    {
        delete pObj;
        m_CriticalSection.Unlock();
        return(NULL);
    }

    //
    // Since we'll be saving a pointer to this object and use it for subsequent
    // calls, add a reference so it doesn't get deleted when the caller
    // releases the object.
    //
    pGlobals->AddRef();

    //
    // Initialize the storage globals object.
    //
    pObj->SetDrive(ulDrive);

    //
    // Save this object in our array.
    //
    m_ppStorageGlobals[ulDrive] = pGlobals;

    //
    // Release the critical section.
    //
    m_CriticalSection.Unlock();

    //
    // Return the storage globals object.
    //
    return(pGlobals);
}

//****************************************************************************
//
// Returns the device object for the device.
//
//****************************************************************************
IMDSPDevice *
CMaverickInfo::GetDevice(unsigned long ulDrive)
{
    CComObject<CMDSPDevice> *pObj;
    IMDSPDevice *pDevice;
    HRESULT hr;

    //
    // Obtain the critical section.
    //
    m_CriticalSection.Lock();

    //
    // Make sure that:
    //   1) the specified drive is valid, and
    //   2) we have an array of device objects.
    //
    if((ulDrive >= m_ulNumDrives) || !m_ppDevice)
    {
        m_CriticalSection.Unlock();
        return(NULL);
    }

    //
    // See if there is already a device object for this drive.
    //
    if(m_ppDevice[ulDrive])
    {
        //
        // There is already a device object for this drive, so add a reference
        // to that object.
        //
        m_ppDevice[ulDrive]->AddRef();

        //
        // Release the critical section.
        //
        m_CriticalSection.Unlock();

        //
        // Return the device object for this drive.
        //
        return(m_ppDevice[ulDrive]);
    }

    //
    // Create a new device object.
    //
    hr = CComObject<CMDSPDevice>::CreateInstance(&pObj);
    if(FAILED(hr))
    {
        m_CriticalSection.Unlock();
        return(NULL);
    }

    //
    // Get a pointer to the IMDSPDevice interface of the object.
    //
    hr = pObj->QueryInterface(IID_IMDSPDevice,
                              reinterpret_cast<void **>(&pDevice));
    if(FAILED(hr))
    {
        delete pObj;
        m_CriticalSection.Unlock();
        return(NULL);
    }

    //
    // Since we'll be saving a pointer to this object and use it for subsequent
    // calls, add a reference so it doesn't get deleted when the caller
    // releases the object.
    //
    pDevice->AddRef();

    //
    // Initialize the device object.
    //
    pObj->SetDrive(ulDrive);

    //
    // Save this object.
    //
    m_ppDevice[ulDrive] = pDevice;

    //
    // Release the critical section.
    //
    m_CriticalSection.Unlock();

    //
    // Return the device object.
    //
    return(pDevice);
}

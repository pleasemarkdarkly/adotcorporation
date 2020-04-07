//
// FatDataSource.h
//
// Copyright (c) 1998 - 2001 Fullplay Media Systems (TM). All rights reserved
//
// This file was distributed as part of the Dadio (TM) Software Development Kit
//     under the Dadio (TM) Software Development Kit license. Please see the file
//     "Dadio Software Development Kit license agreement.pdf" contained in the
//     "Legal" folder on the Dadio Distribution CD.
//

#ifndef FATDATASOURCE_H_
#define FATDATASOURCE_H_

#include <datasource/common/DataSource.h>
#include <cyg/io/io.h>

class IContentManager;
class IInputStream;
class IOutputStream;
template <class DataType> class SortList;
class IFileNameRef;
typedef class SortList<IFileNameRef*> FileRefList;
typedef struct media_record_info_s media_record_info_t;
typedef struct playlist_record_s playlist_record_t;

#define FAT_DATA_SOURCE_CLASS_ID  2

//! Implements the IDataSource class for storage devices that use the FAT file system.
class CFatDataSource : public IDataSource
{
public:

    //! Attempts to open the drive with the given drive number.
    //! Returns a pointer to a new CFatDataSource object if successful, 0 otherwise.
    static CFatDataSource* Open(int iDriveNumber);

    ~CFatDataSource();

    int GetClassID() const;
    int GetInstanceID() const;

    //! Sets the default refresh mode of the data source.
    //! If DSR_DEFAULT is passed in, then DSR_ONE_PASS_WITH_METADATA is used.
    void SetDefaultRefreshMode(RefreshMode mode);
    //! Returns the current default refresh mode of the data source.
    RefreshMode GetDefaultRefreshMode() const;

    //! Sets the default update chunk size of the data source
    //! This value is the maximum number of records that should be sent for each EVENT_CONTENT_UPDATE
    //! message.
    //! 0 indicates that all records should be sent in one single message.
    //! If DS_DEFAULT_CHUNK_SIZE is passed in, then 0 is used.
    void SetDefaultUpdateChunkSize(int iUpdateChunkSize);
    //! Returns the default update chunk size of the data source
    //! This value is the maximum number of records that will be sent for each EVENT_CONTENT_UPDATE
    //! message.
    //! 0 indicates that all records will be sent in one single message.
    int GetDefaultUpdateChunkSize() const;

    //! Copies the string the data source uses to prefix its URLs into the given string provided.
    bool GetRootURLPrefix(char* szRootURLPrefix, int iMaxLength) const;

    //! Asks the source to pass content updates lists through the event system that
    //! contain all of the content it can access.
    //! If bGetMetadata is true, then each entry is opened and passed to the matching codec for
    //! metadata retrieval.
    //! iUpdateChunkSize specifies how many media records to send back at a time.
    //! If iUpdateChunkSize is zero, then all records are sent back at once.
    ERESULT ListAllEntries(RefreshMode mode, int iUpdateChunkSize);

    //! Retrieves metadata for each media content record in the passed-in list.
    void GetContentMetadata(content_record_update_t* pContentUpdate);

    void CreateMediaRecordsForNewFiles(FileRefList* lstNewFiles);

    //! Asks the source to open this URL for reading.
    //! Returns 0 if the URL was unable to be opened, otherwise
    //! it returns the proper subclass of IInputStream for this file type.
    IInputStream* OpenInputStream(const char* szURL);

    IInputStream* OpenInputStreamByPath(const char* szPath);

    //! Asks the source to open this URL for writing.
    //! Returns 0 if the URL was unable to be opened, otherwise
    //! it returns the proper subclass of IOutputStream for this file type.
    IOutputStream* OpenOutputStream(const char* szURL);

    //! Asks the source the length of the media serial number, if available.
    //! Returns 0 if no serial number is available, or the length in bytes.
    int GetSerialNumberLength() const;

    //! Get the serial number from the media and copy it into the buffer.
    //! Returns the number of bytes copied, or -1 if an error was occurred.
    int GetSerialNumber( char* pBuffer, int iBufferLen ) const;

    //! Indicate whether or not the datsource supports prebuffering.
    bool QueryCanPrebuffer() const
    { return false; }


private:

    CFatDataSource(int iDriveNumber, cyg_io_handle_t hDrive, bool bMediaInitialized);

    //! Called by the data source manager to assign an ID to this data source.
    void SetInstanceID(int iDataSourceID);

friend class CFatDataSourceImp;
    CFatDataSourceImp*  m_pImp;
};

#endif	// FATDATASOURCE_H_

//
// PlaylistFormatManager.h
//
// Copyright (c) 1998 - 2001 Interactive Objects (TM). All rights reserved
//
// This file was distributed as part of the Dadio (TM) Software Development Kit
//     under the Dadio (TM) Software Development Kit license. Please see the file
//     "Dadio Software Development Kit license agreement.pdf" contained in the
//     "Legal" folder on the Dadio Distribution CD.
//

/** \addtogroup PlaylistFormat Playlist Format */
//@{

#ifndef PLAYLISTFORMATMANAGER_H_
#define PLAYLISTFORMATMANAGER_H_

#include <content/common/ContentManager.h>
#include <util/eresult/eresult.h>

//
// fdecl
//
class CPlaylistFormatManagerImp;
class IPlaylist;

//! The playlist format manager keeps track of the playlist file formats available in the
//! system and the functions for loading and saving them.
class CPlaylistFormatManager
{
public:

    //! Get a pointer to the single instance of the playlist format manager.
    static CPlaylistFormatManager* GetInstance();
    
    //! Given a file extension, this function searches the list of registered playlist formats
    //! for a matching extension.
    //! If found, then the ID of the playlist format is returned.
    //! If no matching playlist format is found, then 0 is returned.
    unsigned int FindPlaylistFormat( const char* szExtension );

    //! Load the playlist at the given URL into the playlist provided.
    //! \param iPlaylistFormatID The ID in the registry that matches the playlist format.
    //! Usually attained by calling FindPlaylistFormat.
    //! \param szURL The URL of the playlist file to open.
    //! \param records A vector to which the media content records loaded from the playlist will be added.
    //! \param iMaxTracks The maximum number of tracks to load from the playlist.
    //! If 0 then all tracks from the playlist will be loaded.
    ERESULT LoadPlaylist( int iPlaylistFormatID, const char* szURL, IMediaRecordInfoVector& records, int iMaxTracks = 0 ) ;

    //! Save the given playlist to a stream to be opened by the given URL.
    //! \param iPlaylistFormatID The ID in the registry that matches the playlist format.
    //! Usually attained by calling FindPlaylistFormat.
    //! \param szURL The URL of the playlist file to open.
    //! \param pPlaylist Pointer to a playlist whose entries are to be save out.
    ERESULT SavePlaylist( int iPlaylistFormatID, const char* szURL, IPlaylist* pPlaylist ) ;
    
private:

    CPlaylistFormatManager();
    ~CPlaylistFormatManager();

    CPlaylistFormatManagerImp* m_pPlaylistFormatManagerImp;
};

//@}

#endif  // PLAYLISTFORMATMANAGER_H_
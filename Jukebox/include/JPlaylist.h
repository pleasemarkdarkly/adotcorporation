//........................................................................................
//........................................................................................
//.. File Name: JPlaylist.h																..
//.. Date: 12/05/2000																	..
//.. Author(s): Donni Reitz-Pesek														..
//.. Description of content: Implementation of the JPlaylist class		 				..
//.. Usage: Used to implement standard jukebox functions on the metakit database		..
//.. Last Modified By: Donni Reitz-Pesek@iobjects.com									..	
//.. Modification date: 7/19/2001														..
//........................................................................................
//.. Copyright:(c) 1995-2000 Interactive Objects Inc.  									..
//..	 All rights reserved. This code may not be redistributed in source or linkable  ..
//.. 	 object form without the express written consent of Interactive Objects.        ..
//.. Contact Information: www.iobjects.com												..
//........................................................................................
//........................................................................................
#ifndef JPLAYLIST_H
#define JPLAYLIST_H

//#include <cyg/kernel/kapi.h>

#include "MetakitManager.h"

// MMT - Moved structMetadata and defines to type of Metakit Manager to simplify include hierarchy.

// MMT - Added JSong abstration for a row in a playlist
class JSong 
{
public:
	JSong(c4_RowRef ref) : m_RowRef(ref) {};
	JSong(const JSong& ref) : m_RowRef(ref.m_RowRef) {};
	~JSong() {};
	const char* UUID() const { return MetakitManager::GetInstance()->m_spUUID(m_RowRef); };
	const char* URL() const { return MetakitManager::GetInstance()->m_spURL(m_RowRef); };
	const char* Title() const { return MetakitManager::GetInstance()->m_spTitle(m_RowRef); };
	const char* Artist() const { return MetakitManager::GetInstance()->m_spArtist(m_RowRef); };
	const char* Album() const { return MetakitManager::GetInstance()->m_spAlbum(m_RowRef); };
	const char* Genre() const { return MetakitManager::GetInstance()->m_spGenre(m_RowRef); };
	//const char* FilePath() const { return MetakitManager::GetInstance()->m_spFpath(m_RowRef); };
	const char* FileName() const { return MetakitManager::GetInstance()->m_spFname(m_RowRef); };
	const char* SongLength() const { return MetakitManager::GetInstance()->m_spSonglength(m_RowRef); };
	const char* FileCreationDate() const { return MetakitManager::GetInstance()->m_spCreationDate(m_RowRef); };
	int SongId() const { return MetakitManager::GetInstance()->m_ipId(m_RowRef); };
	int VoteValue() const { return MetakitManager::GetInstance()->m_ipVoteValue(m_RowRef); };
	int Year() const { return MetakitManager::GetInstance()->m_ipYear(m_RowRef); };
	int PlayCount() const { return MetakitManager::GetInstance()->m_ipPlaycount(m_RowRef); };
	int SampleFreq() const { return MetakitManager::GetInstance()->m_ipSamplefreq(m_RowRef); };
	int BitRate() const { return MetakitManager::GetInstance()->m_ipBitrate(m_RowRef); };
	int NumChannels() const { return MetakitManager::GetInstance()->m_ipNumchans(m_RowRef); };
	int GetFlags() const { return MetakitManager::GetInstance()->m_ipFlags.Get(m_RowRef); };
	//void SetFlags(int iFlag) { MetakitManager::GetInstance()->m_ipFlags.Set(m_RowRef, t4_i32(iFlag)); }
	int Date() const { return MetakitManager::GetInstance()->m_ipDate(m_RowRef); };
	int Time() const { return MetakitManager::GetInstance()->m_ipTime(m_RowRef); };
	int TimeTenths() const { return MetakitManager::GetInstance()->m_ipTimeTenths(m_RowRef); };

	// Writes the song's title and artist into the string provided.
	void GetFormattedTitle(char* szTitleString, int iTitleStringLength);

	c4_RowRef m_RowRef;
};

class JPlaylist {
public:

	JPlaylist(char* name);
	~JPlaylist();
	//add a song or playlist to this playlist
	void AddItem(int indexInMaster);
	// Adds a song to the jukebox and then adds it to the playlist.
	// Returns the index of the song in the jukebox.
	int AddSong(const structMetadata& md);
	//find a song in this playlist
	int FindSong(char* title, char* artist);
	//find a playlist in this playlist
	int FindPlaylist(char* name);
	//delete a song from this playlist
	void DeleteItem(int indexInPlaylist);
	// Deletes all songs from the playlist and the jukebox.
	void DeleteAllItems();
	//return the name of this playlist
	char* GetName();
	//return number of elements in playlist 
	int GetSize();
	//sort this playlist on specified eColumn
	void SortOn(structColumns &cols);
	//print songs
	void PrintSongs(bool viewsOnly=false);
	//print songs out to a file
	void PrintSongsToFile(char* name);
	//	Print the formatted titles of all songs in the playlist.
	void PrintFormattedTitles();
		
	JPlaylist& operator= (const JPlaylist &jp);

	//Character Completion stuff
	// returns -1 if no match is available, 0 if set up correctly
	// col should indicate which column view is indexed on
	int StartCharComplete(char firstChar, eColumn col);
	// returns the index value of the exact match, or -1 for no match, 
	//-2 if entire section matches, -9 is an error
	// use in character completion after mStartCharComplete for each sucessive character
	int NextChar(char ch, int position, eColumn col);
	//prints the section that matches characters thus far (will only include those that fit 
	//in the strings[] array)
	void PrintSelection();

	JSong GetSong(int index);
	void SetFlags(int index, int iFlags);

	// Returns the zero-based index of the current track.
	int GetCurrentTrackIndex() const
		{ return m_iCurrentIndex; }

	// Sets the entry at the given index as the current entry in the playlist.
	// Returns true if the track was set as current, false otherwise.
	bool SetCurrentTrackIndex(int index);

	// Sets and returns the zero-based index of the next/previous track.
	// Returns 0 if there is no next/previous track.
	int GetNextTrackIndex();
	int GetPreviousTrackIndex();

	// Locks/unlocks the playlist mutex.
	void Lock();
	void Unlock();

private:
	char m_sName[100];
	c4_View m_vView;
	friend class MetakitManager;

//	cyg_mutex_t m_mutexPlaylist;	// A general-purpose mutex other parts of the app can use for locking and unlocking.

	int		m_iCurrentIndex;	// The zero-based index of the current track in the playlist.

};

#endif
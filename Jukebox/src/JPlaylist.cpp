//........................................................................................
//........................................................................................
//.. File Name: JPlaylist.cpp															..
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
//11/20/2000
#include "JPlaylist.h"
#include "Jukebox.h"
//#include "DataSourceManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "debug.h"
#define diag_printf printf

// Writes the song's title and artist into the string provided.
void
JSong::GetFormattedTitle(char* szTitleString, int iTitleStringLength)
{
	int iCurChar = 0;

	// Start with either the title, filename, or URL.
	int iLen = strlen(Title());
	if (iLen)
	{
		if (iLen > iTitleStringLength)
		{
			strncpy(szTitleString, Title(), iTitleStringLength - 1);
			szTitleString[iTitleStringLength - 1] = '\0';
			return;
		}
		
		strcpy(szTitleString, Title());
		iCurChar += iLen;
	}
	else
	{
		if (iLen = strlen(FileName()))
		{
			if (iLen > iTitleStringLength)
			{
				strncpy(szTitleString, FileName(), iTitleStringLength - 1);
				szTitleString[iTitleStringLength - 1] = '\0';
				return;
			}
			
			strcpy(szTitleString, FileName());
			iCurChar += iLen;
		}
		else
		{
			if (iLen = strlen(URL()))
			{
				if (iLen > iTitleStringLength)
				{
					strncpy(szTitleString, URL(), iTitleStringLength - 1);
					szTitleString[iTitleStringLength - 1] = '\0';
					return;
				}
				
				strcpy(szTitleString, URL());
				iCurChar += iLen;
			}
		}
	}

	// Add the artist.
	if (iLen = strlen(Artist()))
	{
		if (iLen + 4 + iCurChar < iTitleStringLength)
		{
			strcat(szTitleString, " - ");
			strcat(szTitleString, Artist());
		}
	}
}


JPlaylist::JPlaylist(char* name)
	: m_iCurrentIndex(-1)
{
//	cyg_mutex_init(&m_mutexPlaylist);
	strcpy(m_sName, name);
	MetakitManager::GetInstance()->InitializeAPlaylist(*this);
}

JPlaylist::~JPlaylist()
{
//	cyg_mutex_destroy(&m_mutexPlaylist);
}
	//add an item to this playlist
void JPlaylist::AddItem(int indexInMaster){
	MetakitManager::GetInstance()->AddItemToPlaylist(m_vView, indexInMaster);
}

// Adds a song to the jukebox and then adds it to the playlist.
// Returns the index of the song in the jukebox.
int
JPlaylist::AddSong(const structMetadata& md)
{
	int index = Jukebox::GetInstance()->AddSong(md);
	AddItem(index);

	// If this is the first song added to the playlist then set the current index.
	if ((m_iCurrentIndex == -1) && GetSize())
		m_iCurrentIndex = 0;

	return index;
}

//find a song in this playlist
int JPlaylist::FindSong(char* title, char* artist){
	return MetakitManager::GetInstance()->FindSongInPlaylist(m_vView, title, artist);
}

//delete a song from this playlist
void JPlaylist::DeleteItem(int indexInPlaylist)
{
	MetakitManager::GetInstance()->DeleteItemFromPlaylist(m_vView, indexInPlaylist);
}

// Deletes all songs from the playlist and the jukebox.
void
JPlaylist::DeleteAllItems()
{
	while (GetSize())
	{
		int index = Jukebox::GetInstance()->GetRealIndexOfSong(*this, 0);
		Jukebox::GetInstance()->DeleteSong(index);
		DeleteItem(0);
	}
}

	//return the name of this playlist
char* JPlaylist::GetName(){
	return m_sName;
}
	//find the embedded playlist who's name is
int JPlaylist::FindPlaylist(char *name) {
	return MetakitManager::GetInstance()->FindPlaylistInPlaylist(*this, name);
}

	//sort this playlist on specified eColumn
void JPlaylist::SortOn(structColumns &cols){
	m_vView = MetakitManager::GetInstance()->SortOn(m_vView, cols);
}

//print songs
void JPlaylist::PrintSongs(bool viewsOnly)
{
	MetakitManager::GetInstance()->PrintPlaylist(m_vView, viewsOnly);
}
	
void JPlaylist::PrintSongsToFile(char* name) {
	MetakitManager::GetInstance()->PrintPlaylistToFile(m_vView, name);
}

//	Print the formatted titles of all songs in the playlist.
void
JPlaylist::PrintFormattedTitles()
{
	char szFormattedTitle[200];
	const int cSongs = GetSize();
	for (int i = 0; i < cSongs; ++i)
	{
		GetSong(i).GetFormattedTitle(szFormattedTitle, 200);
		diag_printf("Track %d (%d): %s\n", i, GetSong(i).VoteValue(), szFormattedTitle);
	}
}

JPlaylist& JPlaylist::operator= (const JPlaylist &jp){
	m_vView = jp.m_vView;
	return *this;
}

int JPlaylist::GetSize() {
	return MetakitManager::GetInstance()->GetPlaylistSize(*this);
}

int JPlaylist::StartCharComplete(char firstChar, eColumn col) {
	return MetakitManager::GetInstance()->StartCharComplete(firstChar, col, *this);
}

int JPlaylist::NextChar(char ch, int position, eColumn col){
	return MetakitManager::GetInstance()->NextChar(ch, position, col, *this);
}
	//prints the section that matches characters thus far (will only include those that fit 
	//in the strings[] array)
void JPlaylist::PrintSelection(){
	MetakitManager::GetInstance()->PrintSelection(*this);
}

JSong JPlaylist::GetSong(int index)
{
	return MetakitManager::GetInstance()->GetPlaylistSong(*this,index);
}

void JPlaylist::SetFlags(int index, int iFlags)
{
	MetakitManager::GetInstance()->SetPlaylistFlags(*this,index,iFlags);
}

// Sets the entry at the given index as the current entry in the playlist.
// Returns true if the track was set as current, false otherwise.
bool
JPlaylist::SetCurrentTrackIndex(int index)
{
	if ((index < 0) || (index > GetSize()))
		return false;

	m_iCurrentIndex = index;

	return true;
}

// Gets the zero-based index of the next track.
// Returns 0 if there is no next track.
int
JPlaylist::GetNextTrackIndex()
{
/*	switch (CDataSourceManager::GetDataSourceManager()->GetPlaylistMode())
	{
		case CDataSourceManager::NORMAL:
			if (m_iCurrentIndex + 1 > GetSize())
				return -1;
			else
				++m_iCurrentIndex;
			break;

		case CDataSourceManager::RANDOM:
			break;

		case CDataSourceManager::REPEAT_ALL:
			m_iCurrentIndex = (m_iCurrentIndex + 1) % GetSize();
			break;

		case CDataSourceManager::REPEAT_RANDOM:
			break;

		case CDataSourceManager::REPEAT_TRACK:
			return m_iCurrentIndex;
			break;

		case CDataSourceManager::INTRO_SCAN:
			break;

		default:
			diag_printf("*** Should never be here: " __FILE__ ": %d ***\n", __LINE__);
			break;
	}
*/
	return m_iCurrentIndex;
}

// Gets the zero-based index of the previous track.
// Returns 0 if there is no previous track.
int
JPlaylist::GetPreviousTrackIndex()
{
/*	switch (CDataSourceManager::GetDataSourceManager()->GetPlaylistMode())
	{
		case CDataSourceManager::NORMAL:
			if (m_iCurrentIndex - 1 < 0)
				return -1;
			else
				--m_iCurrentIndex;
			break;

		case CDataSourceManager::RANDOM:
			break;

		case CDataSourceManager::REPEAT_ALL:
			if (m_iCurrentIndex - 1 < 0)
				m_iCurrentIndex = GetSize() - 1;
			else
				--m_iCurrentIndex;
			break;

		case CDataSourceManager::REPEAT_RANDOM:
			break;

		case CDataSourceManager::REPEAT_TRACK:
			return m_iCurrentIndex;
			break;

		case CDataSourceManager::INTRO_SCAN:
			break;

		default:
			diag_printf("*** Should never be here: " __FILE__ ": %d ***\n", __LINE__);
			break;
	}

*/	return m_iCurrentIndex;
}

// Locks/unlocks the playlist mutex.
void
JPlaylist::Lock()
{
//	cyg_mutex_lock(&m_mutexPlaylist);
}

void
JPlaylist::Unlock()
{
//	cyg_mutex_unlock(&m_mutexPlaylist);
}

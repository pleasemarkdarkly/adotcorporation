#ifndef _JUKEBOX_H__
#define _JUKEBOX_H__

#include "JPlaylist.h"
#include "MetakitManager.h"

class Jukebox {

public:
	static Jukebox* GetInstance();
	//returns all the songs in the database sorted on specified eColumns
	void GetAllSongs(JPlaylist &pl, structColumns &cols);
	//find a song by title and artist in entire database
	int FindSong(char* title, char* artist);
	
	void AddSong(structMetadata md);
	
	void PlayerAddFile(const char* szFilename, const char* szFullFilename, unsigned short usDate, unsigned short usTime, unsigned short ubTimeTenths);
	void DeleteSong(int index);
	//returns number of items in the jukebox
	int GetSize();

	//initialize a new, empty playlist, add it to root
	JPlaylist* CreatePlaylist(char* name);
	//destroy playlist, keep it in root
	void ReleaseStoredPlaylist(JPlaylist* pl);
	//destroy the playlist and delete it from root
	void DestroyPlaylist(JPlaylist *pl);

	//find a playlist by name
	int FindPlaylistInMaster(char* name);
	void RetrievePlaylistFromMaster(int index, JPlaylist &jp);
	int FindPlaylistInPlaylist(JPlaylist &parent_pl, char* name);
	void AddPlaylistToPlaylist(JPlaylist &parent_pl, int indexInMaster);
	void DeletePlaylistFromPlaylist(JPlaylist &parent_pl, int indexInPlaylist);
	void ViewPlaylist(JPlaylist pl);
	void PlayPlaylist(JPlaylist pl);
	int GetRealIndexOfSong(JPlaylist &pl, int indexInPlaylist);

	void SortAllPlaylists();
	void ViewAllPLaylists();
	void ViewDatabase();
	void StoreDatabase();
	void TestPrint();

	//character matching functions
	int StartCharComplete(char firstChar, eColumn col, JPlaylist &pl, JPlaylist& results);
	int NextChar(char ch, int position, eColumn col, JPlaylist &pl , JPlaylist& results);
	//void PrintSelection(JPlaylist &pl);

	JPlaylist GetStandardPlaylist(eColumn col);	
	structColumns cols;

private:
	Jukebox();
	~Jukebox();
	void InitJukebox();

	static Jukebox* m_spJukebox;
	JPlaylist m_plSortOnGenre;
	JPlaylist m_plSortOnTitle;
	JPlaylist m_plSortOnArtist;
	JPlaylist m_plSortOnAlbum;
	JPlaylist m_plEmbeddedPlaylists;
};

#endif
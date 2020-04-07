//........................................................................................
//........................................................................................
//.. File Name: MetakitManager.h														..
//.. Date: 12/05/2000																	..
//.. Author(s): Donni Reitz-Pesek														..
//.. Description of content: Definition of the MetakitManager class		 				..
//.. Usage: Used to implement jukebox functions on the metakit database					..
//.. Last Modified By: Donni Reitz-Pesek@iobjects.com									..	
//.. Modification date: 7/19/2001														..
//........................................................................................
//.. Copyright:(c) 1995-2000 Interactive Objects Inc.  									..
//..	 All rights reserved. This code may not be redistributed in source or linkable  ..
//.. 	 object form without the express written consent of Interactive Objects.        ..
//.. Contact Information: www.iobjects.com												..
//........................................................................................
//........................................................................................
#ifndef METAKITMANAGER_H
#define METAKITMANAGER_H

#define MAX_STRINGS 1000

#include "mk4.h"
#include "mk4str.h"

#define MAX_STRING_SIZE 256

typedef struct structMetadata {
	char szUUID[MAX_STRING_SIZE];
	char szURL[MAX_STRING_SIZE];
	char Title[MAX_STRING_SIZE];
	char Artist[MAX_STRING_SIZE];
	char Album[MAX_STRING_SIZE];
	char Genre[MAX_STRING_SIZE];
	char FilePath[MAX_STRING_SIZE];
	char FileName[MAX_STRING_SIZE];
	char SongLength[MAX_STRING_SIZE];
	char FileCreationDate[MAX_STRING_SIZE];
	int	iVoteValue;
	int SongId;
	int Year;
	int PlayCount;
	int SampleFreq;
	int BitRate;
	int NumChannels;
	int Flags;
	int Date;
	int Time;
	int TimeTenths;
}structMetadata;

typedef enum eColumn { ID, UUID, URL, TITLE, ARTIST, ALBUM, VOTEVALUE, YEAR, GENRE, FILEPATH, FILENAME, SONGLENGTH, PLAYCOUNT,
					  SAMPLEFREQ, BITRATE, NUMCHANNELS, FLAGS, EMBVIEW, VIEWS, CREATIONDATE, NOCOL };

typedef struct structColumns {
	int NumCols;
	eColumn columns[10];
}structColumns;

//MMT - forward declaration of these structures, include to JPlaylist now in MetakitManager.cpp
//		to avoid circular reference
class JPlaylist;
class JSong;

class MetakitManager {
	friend class JSong;
public:
	//initialize the metakit structure
	static MetakitManager* GetInstance();
	
	//Functions that affect entire database
	int AddSongToJukebox(const structMetadata& md);
	void AddPlaylistToJukebox(JPlaylist &pl, char* name);
	void DeleteItemFromJukebox(int index);
	int FindSongInJukebox(char* title, char* artist);
	int FindPlaylistInJukebox(char* name); 
	void FillMetadataFromMaster(structMetadata &md, int index);
	void SortMasterOn(JPlaylist &pl, structColumns &cols);
	void PrintDatabase();
	int GetMasterSize();

	//playlist specific functions
	void AddItemToPlaylist(c4_View view, int indexInMaster);
	void DeleteItemFromPlaylist(c4_View view, int indexInPlaylist);
	int FindSongInPlaylist(c4_View view, char* title, char* artist);
	int FindPlaylistInPlaylist(JPlaylist &pl, char* name); 
	void InitializeAPlaylist(JPlaylist &jp);
	void GetEmbeddedPlaylist(int index, JPlaylist &jp);
	int GetIndexInMasterForCopy(int indexInThis, JPlaylist &jp);
	int GetPlaylistSize(JPlaylist &jp);
	void PrintPlaylistToFile(c4_View view, char* name="out.txt");
	void SelectOn(JPlaylist &jp, eColumn col, char* value);
	// MMT - Added JSong abstration for a row in a playlist
	JSong GetPlaylistSong(JPlaylist &jp, int index);
	void SetPlaylistFlags(JPlaylist& jp,int index,int iFlags);
	
	//generic
	c4_View SortOn(c4_View &view, structColumns &cols);
	void PrintPlaylist(c4_View view, bool viewsOnly=false);
	void MakePlaylistForPlaying(c4_View view);
	void PrepToClose();
	void SaveDatabase();

	//dar
	int AddRowToArtistTable(int id, char* artist);
	const char* GetArtistFromArtistTable(int id);
	int AddRowToGenreTable(int id, char* artist);
	const char* GetGenreFromGenreTable(int id);


	//**Character Completion stuff**
	//Wraper funtions for use in the Jukebox
//	int StartCharComplete(char firstChar, eColumn col);
//	int NextChar(char ch, int position, eColumn col);
//	void PrintSelection();
	//generic playlist version
	// returns -1 if no match is available, 0 if set up correctly
	// col should indicate which column view is indexed on
	int StartCharComplete(char firstChar, eColumn col, JPlaylist pl);
	// returns the index value of the exact match, or -1 for no match, 
	//-2 if entire section matches, -9 is an error
	// use in character completion after mStartCharComplete for each sucessive character
	int NextChar(char ch, int position, eColumn col, JPlaylist pl);
	//prints the section that matches characters thus far (will only include those that fit 
	//in the strings[] array)
	void PrintSelection(JPlaylist pl);
	

	c4_Storage m_sStorage;

private:

	static MetakitManager* m_spMetakitmanager;
	MetakitManager(char* filename);
	~MetakitManager();
	void SetCharCompleteMarkers(c4_View &view, eColumn col);
	int LoadStrings(eColumn col, c4_View &view); //returns number of strings left

/*	
	char SongLength[80];
	char FileCreationDate[80];
*/

	c4_IntProp m_ipId;
	c4_StringProp m_spUUID;
	c4_StringProp m_spURL;
	c4_StringProp m_spTitle;
	c4_StringProp m_spArtist;
	c4_StringProp m_spAlbum;
	c4_StringProp m_spGenre;
	c4_StringProp m_spFname;
	c4_StringProp m_spFpath;
	c4_StringProp m_spEmbname;
	c4_StringProp m_spSonglength;
	c4_StringProp m_spCreationDate;
	c4_IntProp m_ipVoteValue;
	c4_IntProp m_ipYear;
	c4_IntProp m_ipPlaycount;
	c4_IntProp m_ipSamplefreq;
	c4_IntProp m_ipBitrate;
	c4_IntProp m_ipNumchans;
	c4_IntProp m_ipFlags;
	c4_IntProp m_ipDate;
	c4_IntProp m_ipTime;
	c4_IntProp m_ipTimeTenths;
	c4_IntProp m_ipArtistID;
	c4_IntProp m_ipGenreID;
	c4_ViewProp m_vpEmbplaylist;
	c4_Row m_rAddSongRow;
	c4_Row m_rAddViewRow;
	c4_Row m_rFindSongRow;
	c4_Row m_rFindViewRow;
	c4_Row m_rCopyRow;
	c4_Row m_rDARRow;
	c4_Row m_rDARFindRow;
	c4_View m_vMaster;
	c4_View m_vArtist;
	c4_View m_vGenre;

		//first marker is for all non-alphanumerics,
	//1-10 are for the digits,
	//11-37 is for letters, upper and lower together.
	int m_iCCMarkers[37];
	char* m_sStrings[MAX_STRINGS];
	int m_iSection;		//section for character matching
	int m_iSize;			//size of section
	bool m_bMore;
	int m_iCurStr; 
	int m_iCurIndex;

};

#endif
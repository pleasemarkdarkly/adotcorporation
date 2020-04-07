#ifndef METAKITMANAGER_H
#define METAKITMANAGER_H

#define MAX_STRINGS 1000
#define MAX_STRING_SIZE 200

#include "mk4.h"
#include "mk4str.h"


typedef struct structMetadata {
	char Title[MAX_STRING_SIZE];
	char Artist[MAX_STRING_SIZE];
	char Album[MAX_STRING_SIZE];
	char Genre[MAX_STRING_SIZE];
	char FilePath[MAX_STRING_SIZE];
	char FileName[MAX_STRING_SIZE];
	char SongLength[MAX_STRING_SIZE];
	char FileCreationDate[MAX_STRING_SIZE];
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

typedef enum eColumn { ID, TITLE, ARTIST, ALBUM, YEAR, GENRE, FILEPATH, FILENAME, SONGLENGTH, PLAYCOUNT,
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
	void AddSongToJukebox(structMetadata md);
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

	//Song specific functions


	//generic
	c4_View SortOn(c4_View &view, structColumns &cols);
	void PrintPlaylist(c4_View view, bool viewsOnly=false);
	void MakePlaylistForPlaying(c4_View view);
	void PrepToClose();
	void SaveDatabase();

	//**Character Completion stuff**
	//Wraper funtions for use in the Jukebox
//	int StartCharComplete(char firstChar, eColumn col);
//	int NextChar(char ch, int position, eColumn col);
//	void PrintSelection();
	//generic playlist version
	// returns -1 if no match is available, 0 if set up correctly
	// col should indicate which column view is indexed on
	int StartCharComplete(char firstChar, eColumn col, JPlaylist& pl,JPlaylist& plResults);
	// returns the index value of the exact match, or -1 for no match, 
	//-2 if entire section matches, -9 is an error
	// use in character completion after mStartCharComplete for each sucessive character
	void NextChar(char ch, int position, eColumn col, JPlaylist& pl);
	//prints the section that matches characters thus far (will only include those that fit 
	//in the strings[] array)
	//void PrintSelection(JPlaylist pl);
	

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
	c4_StringProp m_spTitle;
	c4_StringProp m_spArtist;
	c4_StringProp m_spAlbum;
	c4_StringProp m_spGenre;
	c4_StringProp m_spFname;
	c4_StringProp m_spFpath;
	c4_StringProp m_spEmbname;
	c4_StringProp m_spSonglength;
	c4_StringProp m_spCreationDate;
	c4_IntProp m_ipYear;
	c4_IntProp m_ipPlaycount;
	c4_IntProp m_ipSamplefreq;
	c4_IntProp m_ipBitrate;
	c4_IntProp m_ipNumchans;
	c4_IntProp m_ipFlags;
	c4_IntProp m_ipDate;
	c4_IntProp m_ipTime;
	c4_IntProp m_ipTimeTenths;
	c4_ViewProp m_vpEmbplaylist;
	c4_Row m_rAddSongRow;
	c4_Row m_rAddViewRow;
	c4_Row m_rFindSongRow;
	c4_Row m_rFindViewRow;
	c4_Row m_rCopyRow;
	c4_View m_vMaster;

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
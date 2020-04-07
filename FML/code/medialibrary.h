#ifndef _MEDIA_LIBRARY_H
#define _MEDIA_LIBRARY_H

#include "stdafx.h"
#include "ScanDialog.h"
#include "metadata.h"
#include "responses.h"
#include <afxtempl.h>


// these get used for the tree controls in the item data to identify the type of data

static DWORD TET_MASK     = 0xF0000000;

static DWORD TET_MEDIA    = 0x10000000;
static DWORD TET_ALBUM    = 0x20000000;
static DWORD TET_ARTIST   = 0x30000000;
static DWORD TET_GENRE    = 0x40000000;
static DWORD TET_PLAYLIST = 0x50000000;

// class to provide unique string arrays
// behaves much like the CStringArray class except for add

extern HANDLE g_DisplayReady;

extern HANDLE g_WaitDisplayReady, g_WaitThreadReady;


class CUniqueStringArray : public CStringArray
{
public:
	int Add(CString newElement);
	void Serialize(CArchive &ar);
};

class CProvidedRadioStation : public CObject
{
public:
	CProvidedRadioStation(CString genre, CString name, CString url)
	{m_genre = genre; m_name = name; m_url = url;}
	CString m_genre, m_name, m_url;
};


class CRadioStation : public CObject
{
public:
	CRadioStation(CString name, CString url, CString mime);
	CRadioStation();
	DECLARE_SERIAL(CRadioStation);

	CString GetStationTitle();
	CString GetStationURL();
	CString	GetStationMimeType();

	void SetStationTitle(CString name);
	void SetStationURL(CString url);

	DWORD	myIndex;
	DWORD   myMediaKey;

	virtual void Serialize(CArchive &ar);

private:
	CString _name;
	CString _url;
	CString _mime;
};


// types of media elements
#define LOCAL_FILE		0
#define INTERNET_RADIO	1

class CMediaElement : public CObject
{
friend class CMediaManager;
public:
	DECLARE_SERIAL(CMediaElement);

	virtual void Serialize(CArchive &ar);

	CMediaElement(DWORD artistIndex, DWORD genreIndex, DWORD albumIndex, CString elementTitle, CString mimeType, CString filePath);
	CMediaElement(CRadioStation *station); // construct from an internet radio station object
	CMediaElement(CMediaElement *element);  // copy constructor
	CMediaElement();						// default constructor

	DWORD   GetArtistIndex();
	DWORD   GetGenreIndex();
	DWORD   GetAlbumIndex();
	CString GetElementTitle();
	CString GetMimeType();
	CString GetFilePath();
	DWORD   GetMediaType();
	void	SetMediaType(DWORD mediaType);

	void	SetLinkKey(DWORD linkKey);
	DWORD   GetLinkKey();
	void	SetPathValidity(int Validate);
	int		GetPathValidity();

	void	SetAlbumPosition(DWORD albumPos);
	DWORD	GetAlbumPosition();

	void UpdateMetadata(CMediaElement *pElement);

	



	operator =  (CMediaElement *rhs);
	int operator == (CMediaElement *rhs);
	DWORD   myIndex;

private:
	int	_pathValidated;
	DWORD	_artistIndex;
	DWORD	_genreIndex;
	DWORD	_albumIndex;
	CString _elementTitle;

	CString _mimeType;
	CString _filePath;

	DWORD   _mediaType;
	DWORD   _linkKey;
	DWORD   _albumPos;
};
	


// database manager classes

class CArtistManager : public CObject
{
friend class CArtistView;
public:
    DECLARE_SERIAL(CArtistManager);

	CArtistManager();
	~CArtistManager();

	DWORD	AddArtistToDatabase(CString artistName);
	CString GetArtistFromKey(DWORD artistKey);

	void	ClearAll();

	DWORD GetCount();

	DWORD GetMaxIndex();

	virtual void Serialize(CArchive &ar);

private:
	CUniqueStringArray  _artistNames;

};


class CAlbumManager : public CObject
{
friend class CAlbumView;
public:
    DECLARE_SERIAL(CAlbumManager);

	CAlbumManager();
	~CAlbumManager();

	DWORD      AddAlbumToDatabase(CString albumName);
	CString	   GetAlbumFromKey(DWORD albumKey);
	void	ClearAll();

	DWORD GetCount();
	DWORD GetMaxIndex();

	virtual void Serialize(CArchive &ar);
private:
	CUniqueStringArray _albumNames;

};

class CGenreManager : public CObject
{
friend class CGenreView;
public:
	DECLARE_SERIAL(CGenreManager);

	CGenreManager();
	~CGenreManager();

	DWORD		AddGenreToDatabase(CString genreName);
	CString		GetGenreFromKey(DWORD genreKey);
	
	void	ClearAll();

	DWORD GetCount();
	DWORD GetMaxIndex();

	virtual void Serialize(CArchive &ar);

private:

	CUniqueStringArray _genreNames;

};



class CRadioManager : public CObject
{
friend class CRadioView;
public:
	DECLARE_SERIAL(CRadioManager);

	CRadioManager();
	~CRadioManager();

	DWORD AddStationToDatabase(CRadioStation *station);
	CRadioStation *GetStationFromKey(DWORD stationKey);
	void UpdateStationDetails(DWORD stationKey, CRadioStation *radioStation);
	void RemoveEntry(DWORD index);
	void ReIndexRadioStations();
	DWORD GetMaxIndex();

	DWORD GetSize();

	void	ClearAll();


	virtual void Serialize(CArchive &ar);

private:
	CObArray stationList;
};


class CPlayList : public CObject
{
public:
	CString title;;
	CDWordArray playListEntries;
	DECLARE_SERIAL(CPlayList);
	CPlayList();  // default constructor
	CPlayList(CString playListTitle, CDWordArray & entries);
	~CPlayList();
	virtual void Serialize(CArchive &ar);
};

class CPlayListManager : public CObject
{
public:
	DECLARE_SERIAL(CPlayListManager);
	CPlayListManager();
	~CPlayListManager();

	// returns index of the new playlist
	DWORD AddPlayList(CString playListTitle, CDWordArray  &playListEntries);
	void UpdatePlayList(DWORD playListIndex, CDWordArray * newPlayListEntries);
	void DeletePlayList(DWORD playListIndex);

	DWORD GetNumPlaylists();

	CString GetPlayListTitle(DWORD playListIndex);
	void SetPlayListTitle(DWORD playListIndex, CString playListTitle);

	DWORD AddPlayListEntry(DWORD playListIndex, DWORD entry);

	// action: [- Move up]  [0 Delete] [+ Move down]
	DWORD MovePlayListEntry(DWORD playListIndex, DWORD entry, int action);

	DWORD GetPlayListEntry(DWORD playListIndex, DWORD entry);

	CPlayList *GetPlayList(DWORD playListIndex);

	void RevalidateAllPlaylistEntries();

	void ClearAll();
	DWORD GetMaxIndex();

	virtual void Serialize(CArchive &ar);
private:
	CObArray _playLists;
};



class CMediaManager : public CObject
{
public:
	DECLARE_SERIAL(CMediaManager);

	CMediaManager();
	~CMediaManager();

	DWORD				AddMediaToDatabase(CMediaElement *mediaElement);
	CMediaElement*		GetMediaFromKey(DWORD mediaKey);
	void	ClearAll();

	DWORD GetCount();

	DWORD GetMaxIndex();

	virtual void Serialize(CArchive &ar);

private:
	CObArray	_mediaElements;
};



extern int     g_AnimSeries;
extern CString g_WaitText;
extern HANDLE g_AnimationStoppedEvent;


// there will be a global instance of this class so that the other objects can call
// its methods to do stuff.

class CMediaDatabase : public CObject
{
public:
	void Shutdown();
	CMediaDatabase();
	~CMediaDatabase();


	// directory management (where to search for content)
	void ClearDirectories();
	void AddDirectory(CString name);
	CStringArray* GetDirectories();

	//database file management

	BOOL LoadDatabase(LPCSTR filepath);
	BOOL SaveDatabase(LPCSTR filepath);


	//void BuildViews(CWaitDialog *pwaitDialog);

	// serialization to dump database to a file
    DECLARE_SERIAL(CMediaDatabase);
	virtual void Serialize(CArchive &ar);

	// these 'managers' manage how IDs are allocate to the elements of the database
	CMediaManager		mediaManager;
	CArtistManager		artistManager;
	CAlbumManager		albumManager;
	CGenreManager		genreManager;
	CRadioManager		radioManager;
	CPlayListManager    playListManager;

	// manages the view IDs for database queries
	CViewManager		viewManager;

	// local storage for the directory strings
	CUniqueStringArray		contentDirectories;
	CUniqueStringArray        oldContentDirectories;

	// private functions
	void DumpArtistViewToScreen();

	// some statistics
	DWORD numArtists;
	DWORD numAlbums;
	DWORD numGenres;
	DWORD numTracks;

	CString libraryName;
	CString libraryGUID;

	// mutex to make this threadsafe
	CRITICAL_SECTION	databaseMutex;
	// status for reads and updates
	DWORD databaseAccess;


	// status for GUI actions
	int databaseStatus;  

	// we need to know about this dialog so our threads can refresh the status
	// that is displayed
	CScanDialog refreshDlg;


	// actions called from the GUI
	void BuildDatabase();

	// called by BuildDatabase
	void AddMediaFromFolder(LPCSTR directory);
	void CreateAndAddTitle(LPCSTR filepath, LPCSTR filename,ULONGLONG length);
	void UpdateRefreshStatus(IML_METADATA *pMetadata,LPCSTR filepath);

	void ReAdvertiseUPnP();
	void ByeByeUpnp();
	void AnnounceUpnp();

	//called when the media tree is updated

	void LogIPAddress(DWORD address);

	CArray<DWORD,DWORD> DJAddresses;

	DWORD totalQueries;

	void UpdateStats(DWORD dwArtistCount,DWORD dwAlbumCount,DWORD dwGenreCount,DWORD dwTrackCount)
	{ numArtists = dwArtistCount; numAlbums = dwAlbumCount; numGenres = dwGenreCount; numTracks = dwTrackCount;};

};


#define IML_ABORT_REFRESH 0x01

extern CMediaDatabase theMediaDatabase;


#endif
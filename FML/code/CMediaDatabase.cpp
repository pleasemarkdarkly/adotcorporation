 // there will be a global instance of this class so that the other objects can call
// its methods to do stuff.

#include "medialibrary.h"
#include "metadata.h"
#include "stdafx.h"
#include "ssdp.h"
#include "WaitingDialog.h"
#include "ScanDialog.h"
#include <direct.h>

#ifdef _WIN32_IE
#undef _WIN32_IE
#define _WIN32_IE 0x0501
#endif

#ifdef _WINRESRC_
#undef _WINRESRC_
#endif

#define _SHELL32_

#include "shfolder.h"
#include "shlobj.h"
#include "shlwapi.h"

#include "FMLDlg.h"


HANDLE g_DisplayReady;
HANDLE g_AnimationStoppedEvent, g_RefreshComplete,g_WaitThreadReady;
HANDLE g_WaitDisplayReady;

DWORD WINAPI RefreshThread(void *param);
void DoRefresh();

#define MIN_WAV_SIZE 500 * 1024

#define MAX_FILE_SIZE 2000 * 1000 * 1024


int     g_AnimSeries;
CString g_WaitText;


IMPLEMENT_SERIAL(CMediaDatabase, CObject, 0);

CMediaDatabase::CMediaDatabase()
{
	CObject::CObject();
	contentDirectories.SetSize(5,5);
	contentDirectories.RemoveAll();

	totalQueries = 0;

	databaseStatus = 0;
	databaseAccess = 0;
	g_ServerActive = false;
	InitializeCriticalSection(&databaseMutex);
	g_DisplayReady = CreateEvent(NULL,FALSE,FALSE,"ScreenSync");

	g_WaitDisplayReady = CreateEvent(NULL,FALSE,FALSE,"waitReady");
	
	g_AnimationStoppedEvent = CreateEvent(NULL,FALSE,FALSE,"AnimationStopped");
	g_RefreshComplete		= CreateEvent(NULL,FALSE,FALSE,"refreshcomplete");
	g_WaitThreadReady		= CreateEvent(NULL,FALSE,FALSE,"waitthreadready");
}

CMediaDatabase::~CMediaDatabase()
{
	DeleteCriticalSection(&databaseMutex);
	CObject::~CObject();
}


void CMediaDatabase::ClearDirectories()
{
	contentDirectories.RemoveAll();
}

void CMediaDatabase::AddDirectory(CString name)	
{
	contentDirectories.Add(name);
}
	

CStringArray* CMediaDatabase::GetDirectories()
{
	CStringArray* temp = new CStringArray();
	temp->Copy(contentDirectories);
	return temp;
}

/*
void CMediaDatabase::DisplayWaitingDialog(int AnimSeries, CString text)
{

	g_AnimSeries = AnimSeries;
	g_WaitText = text;

	AfxBeginThread(RUNTIME_CLASS(CWaitThread));

	WaitForSingleObject(g_WaitThreadReady,INFINITE);

}*/



HANDLE g_RefreshThread;
DWORD  g_RefreshThreadID;

void CMediaDatabase::BuildDatabase()
{

	SaveDatabase("fmlbackup.fml");
		
	mediaManager.ClearAll();

	DoRefresh();
		
	databaseStatus &= ~IML_ABORT_REFRESH;

	int nRet = refreshDlg.DoModal();

	WaitForSingleObject(g_RefreshComplete,INFINITE);

	if (databaseStatus  & IML_ABORT_REFRESH)
	{
		mediaManager.ClearAll();

		LoadDatabase("fmlbackup.fml");
	}

}

// this has to be a glocal level function to create the thread
void DoRefresh()
{
	g_RefreshThread = CreateThread(0, 0, 
									(LPTHREAD_START_ROUTINE) RefreshThread,
                                    (LPVOID)0,
                                    0,
									&g_RefreshThreadID);
}


DWORD WINAPI RefreshThread(void *param)
{
	// wait until the dialog is ready to be drawn on
	DWORD wautCause = WaitForSingleObject(g_DisplayReady,INFINITE);

	int	numberOfDirectories = theMediaDatabase.contentDirectories.GetSize();

	for (int i = 0; i<numberOfDirectories; i++)
	{
		CString directory = theMediaDatabase.contentDirectories.GetAt(i);

		theMediaDatabase.AddMediaFromFolder(directory);
	}

	// show status that the refreshing is done
	theMediaDatabase.refreshDlg.Done();

	SetEvent(g_RefreshComplete);

	return 0;
}



void CMediaDatabase::AddMediaFromFolder(LPCSTR directory)
{

	CFileFind finder;

	// build a string with wildcards
	CString strWildcard(directory);
	strWildcard += _T("\\*.*");  // cheat for now

	// start working for files
	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking)
	{
		
		if (databaseStatus & IML_ABORT_REFRESH)
			break;

	
		bWorking = finder.FindNextFile();

		// skip . and .. files; otherwise, we'd
		// recur infinitely!

		if (finder.IsDots())
			continue;

		// if it's a directory, recursively search it

		CString str, fname;
		ULONGLONG length;

		str = finder.GetFilePath();
		fname = finder.GetFileName();
		length = finder.GetLength();

		if (finder.IsDirectory())
		{
			AddMediaFromFolder(str);
		}
		else
		{
			CreateAndAddTitle(str,fname,length);
		}
   }
   finder.Close();
}

void RemoveCrap(char *str)
{
	CString tempString(str);

	//tempString.OemToAnsi();
	tempString.TrimLeft();
	tempString.TrimRight();

	// these mess up the DJ xml parser
	tempString.Replace('<','[');
	tempString.Replace('>',']');

	strcpy(str,tempString);
}

void CleanUpMetadata(IML_METADATA *pMetadata)
{
	RemoveCrap((char*)&pMetadata->album);
	RemoveCrap((char*)&pMetadata->artist);
	RemoveCrap((char*)&pMetadata->genre);
	RemoveCrap((char*)&pMetadata->title);

	if (!strlen(pMetadata->album))
		strcpy(pMetadata->album,"Unknown Album");
	if (!strlen(pMetadata->artist))
		strcpy(pMetadata->artist,"Unknown Artist");
	if (!strlen(pMetadata->genre))
		strcpy(pMetadata->genre,"Unknown Genre");
}



// updates the dialog text with the current metadata

void CMediaDatabase::UpdateRefreshStatus(IML_METADATA *pMetadata,LPCSTR filepath)
{


	CString currentlyAdding;
	DWORD currentCount = mediaManager.GetCount();

	CString genre = (char*)pMetadata->genre;
	CString artist = (char*)pMetadata->artist;
	CString title = (char*)pMetadata->title;
	CString album = (char*)pMetadata->album;

	currentlyAdding.Format("%d. Found: %s : %s - %s ( %s )",currentCount,genre,artist,title,album);

	refreshDlg.ShowUpdate(currentlyAdding);

}


void CMediaDatabase::CreateAndAddTitle(LPCSTR filepath, LPCSTR filename,ULONGLONG length)
{

	IML_METADATA  metadata;
	memset(&metadata,0,sizeof(metadata));

	BOOL validFile = FALSE;

	DWORD artistID, albumID, genreID, mediaID;

	CString csFilePath(filepath);

	CString filetype = csFilePath.Right(3);
	CString mimetype;

	filetype.MakeUpper();

	if (filetype == "MP3")
	{
		GetMP3Metadata(filepath,&metadata);
		mimetype = "x-ms-mp3";
		validFile = TRUE;
	}
	if (filetype == "WMA")
	{
		if (GetWMAMetadata(filepath, &metadata))
		{
			mimetype = "x-ms-wma";
			validFile = TRUE;
		}
	}

	if (filetype == "WAV")
	{
		if ((length > MIN_WAV_SIZE) && (length < MAX_FILE_SIZE))
		{
			mimetype = "wav";
			validFile = true;
		}
	}

	if (validFile)
	{
		CleanUpMetadata(&metadata);

		if (!strlen(metadata.title))
		{
			CString cTitle = filename;
			cTitle = cTitle.Left(cTitle.ReverseFind('.'));
			strncpy(metadata.title,cTitle,METADATA_STRING_LENGTH-1);
			metadata.title[METADATA_STRING_LENGTH-1] = '\0';
		}
		
		UpdateRefreshStatus(&metadata,filepath);

		artistID = artistManager.AddArtistToDatabase(metadata.artist);
		albumID =  albumManager.AddAlbumToDatabase(metadata.album);
		genreID = genreManager.AddGenreToDatabase(metadata.genre);

		CMediaElement* media = new CMediaElement(artistID,genreID,albumID,metadata.title,mimetype,filepath);

		int track;
		if (sscanf(metadata.albumPosition,"%d",&track)==1)
		{
			media->SetAlbumPosition(track);
		}
		else
		{
			media->SetAlbumPosition(0);
		}

		mediaID = mediaManager.AddMediaToDatabase(media);

	}
}

void CMediaDatabase::Serialize(CArchive &ar)
{
	// the user interface data stuff
	contentDirectories.Serialize(ar);

	// the meat of the database
	mediaManager.Serialize(ar);
	artistManager.Serialize(ar);
	albumManager.Serialize(ar);
	genreManager.Serialize(ar);
	radioManager.Serialize(ar);
	playListManager.Serialize(ar);

	if( ar.IsStoring() )
	{
		ar << libraryName;
		ar << libraryGUID;
	}
	else
	{
		ar >> libraryName;
		ar >> libraryGUID;
	}
}

BOOL CMediaDatabase::LoadDatabase(LPCSTR filepath)
{
	CFile theFile;

	TCHAR szPath[MAX_PATH];

	if(SUCCEEDED(SHGetFolderPath(NULL, 
                             CSIDL_APPDATA, 
                             NULL, 
                             0, 
                             szPath))) 
	{
	    PathAppend(szPath,"Fullplay");
		PathAppend(szPath,filepath);
	}
	else
	{
		strcpy(szPath,filepath);

	}

	mediaManager.ClearAll();
	artistManager.ClearAll();
	albumManager.ClearAll();
	genreManager.ClearAll();
	radioManager.ClearAll();
	playListManager.ClearAll();

	if (!theFile.Open(szPath,CFile::modeRead))
		return FALSE;
		
	CArchive archive(&theFile, CArchive::load);
	Serialize(archive);
	theFile.Close();

	return TRUE;
}


BOOL CMediaDatabase::SaveDatabase(LPCSTR filepath)
{
	CFile theFile;

	TCHAR szPath[MAX_PATH];

	if(SUCCEEDED(SHGetFolderPath(NULL, 
                             CSIDL_APPDATA | CSIDL_FLAG_CREATE, 
                             NULL, 
                             0, 
                             szPath))) 
	{
		_chdir(szPath);

		if (_chdir("Fullplay"))
		{
			_mkdir("Fullplay");
		}

	    PathAppend(szPath,"Fullplay");
		PathAppend(szPath,filepath);
	}
	else
	{
		strcpy(szPath,filepath);
	}

	if (!theFile.Open(szPath,CFile::modeCreate | CFile::modeWrite ))
		return FALSE;

	CArchive archive(&theFile, CArchive::store);
	// go save
	Serialize(archive);
	archive.Close();
	theFile.Close();

	return TRUE;
}

void CMediaDatabase::ReAdvertiseUPnP()
{

	DJAddresses.RemoveAll();
	pMainDlg->m_configDlg.UpdateInfoText();
	ByeByeAlive();   

}

void CMediaDatabase::ByeByeUpnp()
{
	DJAddresses.RemoveAll();
	pMainDlg->m_configDlg.UpdateInfoText();
	
	ByeBye();
}

void CMediaDatabase::AnnounceUpnp()
{

	AliveAnnounce();
}

void CMediaDatabase::Shutdown()
{
	DJAddresses.RemoveAll();
	pMainDlg->m_configDlg.UpdateInfoText();
	mediaManager.ClearAll();
	artistManager.ClearAll();
	albumManager.ClearAll();
	genreManager.ClearAll();
	radioManager.ClearAll();
	playListManager.ClearAll();
}

void CMediaDatabase::LogIPAddress(DWORD address)
{
	bool found = false;
	int i = 0;


	//totalQueries++;

	while ((i < DJAddresses.GetSize()) && (!found))
	{
		found = (address == DJAddresses[i]);
		i++;
	}

	if (!found)	
	{
		DJAddresses.Add(address);
		pMainDlg->m_configDlg.UpdateInfoText();
	}
}


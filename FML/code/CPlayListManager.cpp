#include "stdafx.h"
#include "medialibrary.h"


// implementation of the playlist manager

/*
class CPlayList : public CObject
{
public:
	CString playListTitle;
	CDWordArray playListEntries;
	DECLARE_SERIAL(CPlayList);
	CPlayList();  // default constructor
	CPlayList(CString title;  
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
	DWORD AddPlayList(CString playListTitle = "", CDWordArray * playListEntries = NULL);
	void UpdatePlayList(DWORD playListIndex, CDWordArray * newPlayListEntries);
	void DeletePlayList(DWORD playListIndex);

	DWORD GetNumPlaylists();

	CString GetPlayListTitle(DWORD playListIndex);
	void SetPlayListTitle(DWORD playListIndex, CString playListTitle);

	CDWordArray *GetPlayList(DWORD playlistIndex);
private:
	CObArray _playLists;
};
  */

// CPlayList is the isntance of a playlist that is managed by CPlayListManager

IMPLEMENT_SERIAL(CPlayList, CObject, 0);

CPlayList::CPlayList()
{
	CObject::CObject();
	title = "";
	playListEntries.SetSize(5,5);
}

CPlayList::CPlayList(CString playListTitle, CDWordArray & entries)
{
	CObject::CObject();
	title = playListTitle;
		int j = 0, num = entries.GetSize();
		while (j < num)
		{
			playListEntries.Add(entries.GetAt(j));
			j++;
		}

}

CPlayList::~CPlayList()
{
	playListEntries.RemoveAll();
	CObject::~CObject();

}


void CPlayList::Serialize(CArchive &ar)
{
	CObject::Serialize( ar );

	playListEntries.Serialize (ar);

	if( ar.IsStoring() )
	{
		ar << title;
	}
    else
	{
		ar >> title;
	}
}


// CPlayListManager
// ****************


IMPLEMENT_SERIAL(CPlayListManager, CObject, 0);

CPlayListManager::CPlayListManager()
{
	CObject::CObject();
	_playLists.SetSize(5,5);

}

CPlayListManager::~CPlayListManager()
{
	ClearAll();
	CObject::~CObject();
}


DWORD CPlayListManager::AddPlayList(CString playListTitle , CDWordArray & playListEntries)
{
	CPlayList *newPlaylist = new CPlayList(playListTitle,playListEntries);

	DWORD index = _playLists.Add(newPlaylist);

	return index;

}

void CPlayListManager::UpdatePlayList(DWORD playListIndex, CDWordArray * newPlayListEntries)
{
	CPlayList * pPlayList = (CPlayList*)_playLists.GetAt(playListIndex);

	if (pPlayList)
	{

		pPlayList->playListEntries.RemoveAll();
		int j = 0, num = newPlayListEntries->GetSize();
		while (j < num)	
		{
			pPlayList->playListEntries.Add(newPlayListEntries->GetAt(j));
			j++;
		}
		
	}
}

void CPlayListManager::DeletePlayList(DWORD playListIndex)
{
	if (playListIndex < (DWORD)_playLists.GetSize())
	{
		delete (CPlayList*)_playLists.GetAt(playListIndex);
		_playLists.RemoveAt(playListIndex);
	}

	// now we need to reindex the rest of them
}

DWORD CPlayListManager::GetNumPlaylists()
{
	return (DWORD)_playLists.GetSize();
}

CString CPlayListManager::GetPlayListTitle(DWORD playListIndex)
{
	CString ret = "";
	if (playListIndex < (DWORD)_playLists.GetSize())
	{
		CPlayList * pPlayList = (CPlayList*)_playLists.GetAt(playListIndex);
		ret = pPlayList->title;
	}
	return ret;
}


void CPlayListManager::SetPlayListTitle(DWORD playListIndex, CString playListTitle)
{
	if (playListIndex < (DWORD)_playLists.GetSize())
	{
		CPlayList * pPlayList = (CPlayList*)_playLists.GetAt(playListIndex);
		pPlayList->title = playListTitle;
	}
}

CPlayList * CPlayListManager::GetPlayList(DWORD playListIndex)
{
	CPlayList * ret = NULL;
	if (playListIndex < (DWORD)_playLists.GetSize())
	{
		CPlayList * pPlayList = (CPlayList*)_playLists.GetAt(playListIndex);
		ret = pPlayList;
	}
	return ret;
}

void CPlayListManager::ClearAll()
{
	int i = 0;
	
	while (i < _playLists.GetSize())
	{
		delete (CPlayList*)_playLists.GetAt(i);
		_playLists.RemoveAt(i);
	}
}

DWORD CPlayListManager::AddPlayListEntry(DWORD playListIndex, DWORD entry)
{
	int ret;

	if (playListIndex < (DWORD)_playLists.GetSize())
	{
		CPlayList * pPlayList = (CPlayList*)_playLists.GetAt(playListIndex);
		ret = pPlayList->playListEntries.Add(entry);
	}

	return (DWORD)ret;
}
	
	
DWORD CPlayListManager::GetPlayListEntry(DWORD playListIndex, DWORD entry)
{
	DWORD ret = 0;
	if (playListIndex < (DWORD)_playLists.GetSize())
	{
		CPlayList * pPlayList = (CPlayList*)_playLists.GetAt(playListIndex);
		if (entry < (DWORD)pPlayList->playListEntries.GetSize())
		{
			ret = pPlayList->playListEntries.GetAt(entry);
		}
	}
	return ret;
}

// action: [- Move up]  [0 Delete] [+ Move down]
DWORD CPlayListManager::MovePlayListEntry(DWORD playListIndex, DWORD entry, int action)
{
	DWORD ret = entry;
	if (playListIndex < (DWORD)_playLists.GetSize())
	{
		CPlayList * pPlayList = (CPlayList*)_playLists.GetAt(playListIndex);
		if (entry < (DWORD)pPlayList->playListEntries.GetSize())
		{
			if (action == 0)
			{
				pPlayList->playListEntries.RemoveAt(entry);

				if (entry >= (DWORD)pPlayList->playListEntries.GetUpperBound())
				{
					ret = (DWORD)pPlayList->playListEntries.GetUpperBound();
				}
			}
			else if (action < 0)
			{
				if (entry >= 1)
				{
					// swap towards top of array
					DWORD temp = pPlayList->playListEntries[entry-1];
					pPlayList->playListEntries[entry-1] = pPlayList->playListEntries[entry];
					pPlayList->playListEntries[entry] = temp;
					ret--;
				}
			}
			else if (action > 0)
			{
				if (entry <= ((DWORD)pPlayList->playListEntries.GetUpperBound()-1))
				{
					DWORD temp = pPlayList->playListEntries[entry+1];
					pPlayList->playListEntries[entry+1] = pPlayList->playListEntries[entry];
					pPlayList->playListEntries[entry] = temp;
					ret++;
				}
			}
		}
	}
	return ret;
}


DWORD CPlayListManager::GetMaxIndex()
{
	return _playLists.GetSize();
}



void CPlayListManager::Serialize(CArchive &ar)
{
	CObject::Serialize( ar );
	_playLists.Serialize(ar);
}

void CPlayListManager::RevalidateAllPlaylistEntries()
{
	int numPlaylists = _playLists.GetSize();
	int numEntries = 0;
	CMediaElement *pMedia;
	DWORD index;
	int i,j;

	CPlayList *pPlaylist;


	for (i=0;i< numPlaylists;i++)
	{
		pPlaylist = (CPlayList*)_playLists.GetAt(i);
		numEntries = pPlaylist->playListEntries.GetSize();

		j = 0;
		while (j < numEntries)
		{
			index = pPlaylist->playListEntries.GetAt(j);
			pMedia = theMediaDatabase.mediaManager.GetMediaFromKey(index);
			if (!pMedia->GetPathValidity())
			{
				pPlaylist->playListEntries.RemoveAt(j);
				numEntries = pPlaylist->playListEntries.GetSize();
				j--;
			}
			j++;
		}
	}
}






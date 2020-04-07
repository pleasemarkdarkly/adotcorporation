#include "stdafx.h"
#include "medialibrary.h"
#include "mediahelper.h"
#include "responses.h"

/*

class CViewManager : public CObject
{
public:
	~CViewManager();
	DWORD GetViewID(DWORD viewType,DWORD viewKey);
	void  ReleaseViewID(DWORD viewID);			  

private:
	CObList databaseViews;
	CDWordArray usedViewIDs;
	DWORD lastusedID

};  

class CViewDetails : public CObject
{
public:
	CViewDetails(DWORD viewID, char * xpathString, char * szSortOrder);
	~CViewDetails();

	context_t     GetViewContext()	{ age = 0; return viewContext;};
	DWORD		  GetViewID()		{ age = 0; return viewID; };
	const CDWordArray * GetItems()	{ age = 0; return this->viewItems; };
	DWORD		  GetAge()			{ return age;};


private:
	context_t	viewContext;
	DWORD		viewID;
	CDWordArray viewItems;
	DWORD		age;
};  */

/*
	CONTEXT_UNKNOWN,
	CONTEXT_MEDIA,
	CONTEXT_ARTIST,
	CONTEXT_ALBUM,
	CONTEXT_GENRE,
	CONTEXT_PLAYLIST,
	CONTEXT_RADIOSTATION
*/

char * contextComparators[NUM_CONTEXTS] = {"NONE","ME","AR","AL","GR","PL","RS"};


char * FindStartContext(char *pchar)
{
	char * ret;
	ret = pchar;

	// find the 3rd brace

	while (*ret++!='[');
	while (*ret++!='[');

	return ret;
}

xpath_context_t  ParseContext(char * context)
{
	
	xpath_context_t ret = CONTEXT_UNKNOWN;
	int found = 0;

	int i = 0;
	while ((i < NUM_CONTEXTS) && (!found))
	{
		if (strstr(context,contextComparators[i])==context)
		{
			ret = (xpath_context_t)(i);
			found = 1;
		}
		i++;
	}

	return ret;
}


void AddUnique(CDWordArray &pArray,xpath_context_t viewContext, xpath_context_t queryContext, DWORD queryContextKey)
{
	int keyToAdd = -1;


		switch (queryContext)
		{
			case CONTEXT_UNKNOWN:
				break;
			case CONTEXT_MEDIA:

				switch (viewContext)
				{
					case CONTEXT_UNKNOWN:
						break;
					case CONTEXT_ARTIST:
						keyToAdd = (int)theMediaDatabase.mediaManager.GetMediaFromKey(queryContextKey)->GetArtistIndex();
						break;
					case CONTEXT_ALBUM:
						keyToAdd = (int)theMediaDatabase.mediaManager.GetMediaFromKey(queryContextKey)->GetAlbumIndex();
						break;
					case CONTEXT_GENRE:
						keyToAdd = (int)theMediaDatabase.mediaManager.GetMediaFromKey(queryContextKey)->GetGenreIndex();
						break;
					case CONTEXT_MEDIA:
						keyToAdd = (int)queryContextKey;
						break;
					default:
						keyToAdd = 0;
						break;
				}
				break;
			case CONTEXT_RADIOSTATION:
				// add the media key of radio stations, not the radio station index
				keyToAdd = (int)queryContextKey;
				break;
			default:
				keyToAdd = (int)queryContextKey;
				break;
		}

	
	if (keyToAdd != -1)
	{
		DWORD numElements = pArray.GetSize();
		bool found = false;
		DWORD i = 0;

		if ((DWORD)keyToAdd != queryContextKey)
		{
			while ((i<numElements)&&(!found))
			{
				found = (pArray.GetAt(i)== (DWORD)keyToAdd);
				i++;
			}
			if (!found)
				pArray.Add((DWORD)keyToAdd);
		}
		else
		{
			pArray.Add((DWORD)keyToAdd);
		}
	}
}


CViewDetails::CViewDetails(DWORD viewID, char * queryExpression, char* queryContext, char *returnContext, char * szSortOrder)
{
	// build the filter 

	CMediaFilter *pFilter = new CMediaFilter(queryExpression);

	viewContext = ParseContext(returnContext);
	_viewID = viewID;

	mac = 0;
	
	age = 0;

	DWORD totalItems = theMediaDatabase.mediaManager.GetCount();

	DWORD mediaKey = 0;

	DWORD contextMaxKey, queryContextKey, i = 1;

	xpath_context_t xqueryContext = ParseContext(queryContext);

	// filter the media elements that we want to use

	switch (xqueryContext)
	{
	case CONTEXT_UNKNOWN:
		contextMaxKey = 0;
		break;
	case CONTEXT_MEDIA:
		contextMaxKey = theMediaDatabase.mediaManager.GetMaxIndex();
		break;
	case CONTEXT_ARTIST:
		contextMaxKey = theMediaDatabase.artistManager.GetMaxIndex();
		break;
	case CONTEXT_ALBUM:
		contextMaxKey = theMediaDatabase.albumManager.GetMaxIndex();
		break;
	case CONTEXT_GENRE:
		contextMaxKey = theMediaDatabase.genreManager.GetMaxIndex();
		break;
	case CONTEXT_PLAYLIST:
		// quick fix since playlists are indexed from 0
		contextMaxKey = theMediaDatabase.playListManager.GetMaxIndex();
		break;
	case CONTEXT_RADIOSTATION:
		// quick fix since stations are indexed from 0
		contextMaxKey = theMediaDatabase.radioManager.GetMaxIndex();
		break;
	}

	// build a list of items in the queryContext context

	debugMessage("Evaluating items\n");

	for (queryContextKey = 1; queryContextKey <= contextMaxKey; queryContextKey++)
	{
		if (pFilter->Evaluate(xqueryContext,queryContextKey))
		{
			 AddUnique(ViewItems,viewContext,xqueryContext,(xqueryContext==CONTEXT_RADIOSTATION)?queryContextKey-1:queryContextKey);
		}
	}


	// now sort them

	debugMessage("Sorting items\n");

	CMediaSorter *pSorter = new CMediaSorter(szSortOrder);

	pSorter->SortList(viewContext,ViewItems);

	debugMessage("Finished sorting\n");

	// done with the filter and the sorter
	delete pFilter;
	delete pSorter;
} 

CViewDetails::CViewDetails(DWORD viewID, char * queryExpression, char* queryContext, char *returnContext, char * szSortOrder, DWORD qMAC)
{
	mac = qMAC;

	CMediaFilter *pFilter = new CMediaFilter(queryExpression);

	viewContext = ParseContext(returnContext);
	_viewID = viewID;
	
	age = 0;

	DWORD totalItems = theMediaDatabase.mediaManager.GetCount();

	DWORD mediaKey = 0;

	DWORD contextMaxKey, queryContextKey, i = 1;

	xpath_context_t xqueryContext = ParseContext(queryContext);

	// filter the media elements that we want to use

	switch (xqueryContext)
	{
	case CONTEXT_UNKNOWN:
		contextMaxKey = 0;
		break;
	case CONTEXT_MEDIA:
		contextMaxKey = theMediaDatabase.mediaManager.GetMaxIndex();
		break;
	case CONTEXT_ARTIST:
		contextMaxKey = theMediaDatabase.artistManager.GetMaxIndex();
		break;
	case CONTEXT_ALBUM:
		contextMaxKey = theMediaDatabase.albumManager.GetMaxIndex();
		break;
	case CONTEXT_GENRE:
		contextMaxKey = theMediaDatabase.genreManager.GetMaxIndex();
		break;
	case CONTEXT_PLAYLIST:
		// quick fix since playlists are indexed from 0
		contextMaxKey = theMediaDatabase.playListManager.GetMaxIndex();
		break;
	case CONTEXT_RADIOSTATION:
		// quick fix since stations are indexed from 0
		contextMaxKey = theMediaDatabase.radioManager.GetMaxIndex();
		break;
	}

	// build a list of items in the queryContext context

	debugMessage("Evaluating items\n");

	for (queryContextKey = 1; queryContextKey <= contextMaxKey; queryContextKey++)
	{
		if (pFilter->Evaluate(xqueryContext,queryContextKey))
		{
			 AddUnique(ViewItems,viewContext,xqueryContext,(xqueryContext==CONTEXT_RADIOSTATION)?queryContextKey-1:queryContextKey);
		}
	}
/*
	afxDump << "Context: " << queryContext << "\n";
	afxDump << "Sort order: " << szSortOrder << "\n";
	afxDump << "Query: " << queryExpression << "\n\n";

	// now sort them

	debugMessage("Sorting items\n");

	afxDump << "Before sort:" << "\n";

		for (int j = 0; j < ViewItems.GetSize(); j ++)
			afxDump << ViewItems.GetAt(j) << "  ";

		afxDump << "\n";
*/
	CMediaSorter *pSorter = new CMediaSorter(szSortOrder);

	pSorter->SortList(viewContext,ViewItems);
/*
	afxDump << "After sort: " << "\n";

		for (j = 0; j < ViewItems.GetSize(); j ++)
			afxDump << ViewItems.GetAt(j) << "  ";

		afxDump << "\n";

	debugMessage("Finished sorting\n");
*/
	// done with the filter and the sorter
	delete pFilter;
	delete pSorter;
}



CViewDetails::~CViewDetails()
{
	
	CObject::~CObject();
}




#define CLEANUP_INTERVAL 1000*30  // 30 seconds
//#define VIEW_MAX_AGE     4        // 4 * 30 seconds = 2 minutes
#define VIEW_MAX_AGE     2880       // 2880 * 30 seconds = 1 day

HANDLE g_CleanupThreadSignal;

DWORD WINAPI ViewManagerCleanupThread(void *param)
{
	DWORD  WaitCause = WAIT_TIMEOUT;
	debugMessage("\nViewManagerCleanupThread is running \n\r");

	// thread loop starts here
	while (1)   
	{
		WaitCause = WaitForSingleObject(g_CleanupThreadSignal,CLEANUP_INTERVAL);
        if (WaitCause == WAIT_OBJECT_0)
           break;
		theMediaDatabase.viewManager.AgeViews();
	}
	debugMessage("\nViewManagerCleanupThread is exiting \n\r");
	return 0;
}


CViewManager::CViewManager()
{
	CObject::CObject();
	InitializeCriticalSection(&viewManagerLock);
	g_CleanupThreadSignal = CreateEvent(NULL, TRUE, FALSE, NULL);
	lastUsedID = 0;
	m_hCleanupthread = CreateThread(0, 0, 
                                    (LPTHREAD_START_ROUTINE) ViewManagerCleanupThread,
                                    (LPVOID)0,
                                    0,
                                    &m_dwCleanupThreadID);
}

CViewManager::~CViewManager()
{
	databaseViews.RemoveAll();
	usedViewIDs.RemoveAll();
	SetEvent(g_CleanupThreadSignal);
	DeleteCriticalSection(&viewManagerLock);
	CObject::~CObject();
}

DWORD CViewManager::GetViewID(char * queryExpression, char* queryContext, char *returnContext, char * szSortOrder)
{
	DWORD newID;

	EnterCriticalSection(&viewManagerLock);

	newID = lastUsedID++;

	databaseViews.AddTail(new CViewDetails(newID,queryExpression,queryContext,returnContext,szSortOrder));

	LeaveCriticalSection(&viewManagerLock);

	return newID;
}

DWORD CViewManager::GetViewID(char * queryExpression, char* queryContext, char *returnContext, char * szSortOrder, DWORD qMAC)
{
	DWORD newID;

	EnterCriticalSection(&viewManagerLock);

	newID = lastUsedID++;

	databaseViews.AddTail(new CViewDetails(newID,queryExpression,queryContext,returnContext,szSortOrder,qMAC));

	LeaveCriticalSection(&viewManagerLock);

	return newID;
}



void CViewManager::ReleaseViewID(DWORD viewID)
{
	EnterCriticalSection(&viewManagerLock);

	POSITION pos = databaseViews.GetHeadPosition();
	POSITION prepos;
	BOOL found = FALSE;
	CViewDetails *pdetails;

	while (pos && (!found))
	{
		prepos = pos;
		pdetails = (CViewDetails*)databaseViews.GetNext(pos);
		found = (pdetails->GetViewID() == viewID);
	}

	if (found)
	{
		databaseViews.RemoveAt(prepos);
		delete pdetails;
	}


	LeaveCriticalSection(&viewManagerLock);
}

void CViewManager::AgeViews()
{
	EnterCriticalSection(&viewManagerLock);

	POSITION pos = databaseViews.GetHeadPosition();
	POSITION prepos;

	CViewDetails *pdetails;

	while (pos)
	{
		prepos = pos;
		pdetails = (CViewDetails*)databaseViews.GetNext(pos);
		pdetails->age++;
		if (pdetails->age >= VIEW_MAX_AGE)
		{
			debugMessage("\nCleaning up viewID: ");
			debugMessage(pdetails->GetViewID());

			databaseViews.RemoveAt(prepos);
			delete pdetails;
		}
	}

	LeaveCriticalSection(&viewManagerLock);
}



CViewDetails * CViewManager::GetViewDetails(DWORD viewID)
{
	EnterCriticalSection(&viewManagerLock);

	BOOL found = FALSE;

	CViewDetails* pdetails;

	POSITION pos = databaseViews.GetHeadPosition();

	while (pos && (!found))
	{
		pdetails = (CViewDetails*)databaseViews.GetNext(pos);
		found = (pdetails->GetViewID() == viewID);
	}

	if (!found) pdetails = NULL;

	LeaveCriticalSection(&viewManagerLock);

	return pdetails;
}

		






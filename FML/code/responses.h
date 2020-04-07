#ifndef _responses_h_
#define _responses_h_

#include "stdafx.h"
#include "mediahelper.h"


CString GetItemsFromView(DWORD viewID, DWORD startIndex, DWORD count,char * jumpLetter,int jumpOffset,DWORD *totalCount, DWORD *numReturned);


class CViewDetails : public CObject
{
public:
	CViewDetails(DWORD viewID, char * queryExpression, char* queryContext, char *returnContext, char * szSortOrder);
	CViewDetails(DWORD viewID, char * queryExpression, char* queryContext, char *returnContext, char * szSortOrder, DWORD qMAC);


	~CViewDetails();
	xpath_context_t     GetViewContext()	{ age = 0; return viewContext;};
	DWORD		  GetViewID()		{ age = 0; return _viewID; };
	CDWordArray  *GetItems()	{ age = 0; return &this->ViewItems; };

	DWORD		GetMAC() { return mac; };

	DWORD		age;

	DWORD       mac;

private:
	xpath_context_t	viewContext;
	DWORD		_viewID;

	CDWordArray ViewItems;

};

class CViewManager : public CObject
{
public:
	CViewManager();
	~CViewManager();
	void  ReleaseViewID(DWORD viewID);
	void  AgeViews();

	DWORD GetViewID(char * queryExpression, char* queryContext, char *returnContext, char * szSortOrder);
	DWORD GetViewID(char * queryExpression, char* queryContext, char *returnContext, char * szSortOrder, DWORD qMAC);

	CViewDetails * GetViewDetails(DWORD viewID);

private:
	CObList databaseViews;
	CDWordArray usedViewIDs;
	DWORD lastUsedID;

	HANDLE m_hCleanupthread;
    DWORD  m_dwCleanupThreadID;

	CRITICAL_SECTION viewManagerLock;
};


#endif

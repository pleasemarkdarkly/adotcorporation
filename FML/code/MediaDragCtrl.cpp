// MediaDragCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "FMLDlg.h"

#include "MediaToPlayListDropTarget.h"
#include "MediaDragCtrl.h"
#include "medialibrary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMediaDragCtrl

CMediaDragCtrl::CMediaDragCtrl()
{
}

CMediaDragCtrl::~CMediaDragCtrl()
{
}


BEGIN_MESSAGE_MAP(CMediaDragCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CMediaDragCtrl)
	ON_WM_CREATE()
	
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMediaDragCtrl message handlers

int CMediaDragCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{

	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

void CMediaDragCtrl::Populate()
{
		
	DeleteAllItems();
	DWORD viewID = theMediaDatabase.viewManager.GetViewID("MEN=CMK_ALL","ME","ME","ARN/ALN/ALO/MEN");
	CViewDetails * pViewDetails = theMediaDatabase.viewManager.GetViewDetails(viewID);
	CDWordArray * items = pViewDetails->GetItems();
	DWORD totalItems = (DWORD)items->GetSize();

	DWORD dwArtistCount = 0;
	DWORD dwAlbumCount  = 0;
	DWORD dwGenreCount  = 0;
	DWORD dwTrackCount  = 0;


	HTREEITEM hArtists = InsertItem("Browse by artists",1, 1);
	bool newArtist = true;
	bool newAlbum  = true;
	bool newGenre  = true;
	int previousArtistIndex = -1;
	int previousAlbumIndex = -1;
	CMediaElement * pCurrElement;
	CString itemText;
	HTREEITEM hArtist;
	HTREEITEM hAlbum;
	HTREEITEM hTrack;
	HTREEITEM hGenre;

	for (DWORD artistViewIndex=0; artistViewIndex < totalItems; artistViewIndex++)
	{
		DWORD artistIndex;
		DWORD albumIndex;
		Sleep(0);
		DWORD mediaKey = items->GetAt(artistViewIndex);
		pCurrElement = theMediaDatabase.mediaManager.GetMediaFromKey(mediaKey);
		artistIndex = pCurrElement->GetArtistIndex();
		albumIndex = pCurrElement->GetAlbumIndex();
		newArtist = ((int)artistIndex != previousArtistIndex);
		if (newArtist)
		{
			dwArtistCount++;
			hArtist = InsertItem(theMediaDatabase.artistManager.GetArtistFromKey(artistIndex),1,1,hArtists,NULL);
			SetItemData(hArtist,TET_ARTIST+artistIndex);
			previousArtistIndex = artistIndex;
			hAlbum = InsertItem(theMediaDatabase.albumManager.GetAlbumFromKey(albumIndex),3,3,hArtist,NULL);
			SetItemData(hAlbum,TET_ALBUM+albumIndex);
			previousAlbumIndex = albumIndex;
		}

		newAlbum = ((int)albumIndex != previousAlbumIndex);
		if (newAlbum)
		{
			hAlbum = InsertItem(theMediaDatabase.albumManager.GetAlbumFromKey(albumIndex),3,3,hArtist,NULL);
			SetItemData(hAlbum,TET_ALBUM+albumIndex);
			previousAlbumIndex = albumIndex;
		}
		
		hTrack = InsertItem(pCurrElement->GetElementTitle(),4,4,hAlbum,NULL);
		SetItemData(hTrack,TET_MEDIA+mediaKey);
	}

	theMediaDatabase.viewManager.ReleaseViewID(viewID);
	viewID = theMediaDatabase.viewManager.GetViewID("MEN=CMK_ALL","ME","ME","ALN/ALO/MEN");
	pViewDetails = theMediaDatabase.viewManager.GetViewDetails(viewID);
	items = pViewDetails->GetItems();
	totalItems = (DWORD)items->GetSize();

	HTREEITEM hAlbums = InsertItem("Browse by albums", 3, 3);
	previousAlbumIndex = -1;
	for (DWORD albumViewIndex=0; albumViewIndex < totalItems; albumViewIndex++)
	{
		Sleep(0);
		DWORD albumIndex;
		DWORD mediaKey = items->GetAt(albumViewIndex);
		pCurrElement = theMediaDatabase.mediaManager.GetMediaFromKey(mediaKey);
		albumIndex = pCurrElement->GetAlbumIndex();
		newAlbum = ((int)albumIndex != previousAlbumIndex);
		if (newAlbum)
		{
			dwAlbumCount++;
			hAlbum = InsertItem(theMediaDatabase.albumManager.GetAlbumFromKey(albumIndex),3,3,hAlbums,NULL);
			SetItemData(hAlbum,TET_ALBUM+albumIndex);
			previousAlbumIndex = albumIndex;
		}
		
		hTrack = InsertItem(pCurrElement->GetElementTitle(),4,4,hAlbum,NULL);
		SetItemData(hTrack,TET_MEDIA+mediaKey);
	}
	theMediaDatabase.viewManager.ReleaseViewID(viewID);
	viewID = theMediaDatabase.viewManager.GetViewID("MEN=CMK_ALL","ME","ME","GRN/MEN");
	pViewDetails = theMediaDatabase.viewManager.GetViewDetails(viewID);
	items = pViewDetails->GetItems();
	totalItems = (DWORD)items->GetSize();
	HTREEITEM hGenres = InsertItem("Browse by genres", 2, 2);
	int previousGenreIndex = -1;
	for (DWORD genreViewIndex=0; genreViewIndex < totalItems; genreViewIndex++)
	{
		Sleep(0);
		DWORD genreIndex;
		DWORD mediaKey = items->GetAt(genreViewIndex);
		pCurrElement = theMediaDatabase.mediaManager.GetMediaFromKey(mediaKey);
		genreIndex = pCurrElement->GetGenreIndex();
		newGenre = ((int)genreIndex != previousGenreIndex);
		if (newGenre)
		{
			dwGenreCount++;
			hGenre = InsertItem(theMediaDatabase.genreManager.GetGenreFromKey(genreIndex),3,3,hGenres,NULL);
			SetItemData(hGenre,TET_ALBUM+genreIndex);
			previousGenreIndex = genreIndex;
		}

		itemText.Format("%s (%s - %s)",pCurrElement->GetElementTitle(),theMediaDatabase.artistManager.GetArtistFromKey(pCurrElement->GetArtistIndex()),
						theMediaDatabase.albumManager.GetAlbumFromKey(pCurrElement->GetAlbumIndex()));
		hTrack = InsertItem(itemText,4,4,hGenre,NULL);
		SetItemData(hTrack,TET_MEDIA+mediaKey);
	}
	theMediaDatabase.viewManager.ReleaseViewID(viewID);
	viewID = theMediaDatabase.viewManager.GetViewID("MEN=CMK_ALL","ME","ME","MEN");
	pViewDetails = theMediaDatabase.viewManager.GetViewDetails(viewID);
	HTREEITEM hTitles = InsertItem("Browse all titles", 0, 0);
	items = pViewDetails->GetItems();
	totalItems = (DWORD)items->GetSize();

	for (DWORD titleViewIndex=0; titleViewIndex < totalItems; titleViewIndex++)
	{
		Sleep(0);
		dwTrackCount++;
		DWORD mediaKey = items->GetAt(titleViewIndex);
		pCurrElement = theMediaDatabase.mediaManager.GetMediaFromKey(mediaKey);
		itemText.Format("%s (%s - %s)",pCurrElement->GetElementTitle(),theMediaDatabase.artistManager.GetArtistFromKey(pCurrElement->GetArtistIndex()),
						theMediaDatabase.albumManager.GetAlbumFromKey(pCurrElement->GetAlbumIndex()));
		hTrack = InsertItem(itemText,4,4,hTitles,NULL);
		SetItemData(hTrack,TET_MEDIA+mediaKey);

	}

	theMediaDatabase.viewManager.ReleaseViewID(viewID);

	theMediaDatabase.UpdateStats(dwArtistCount,dwAlbumCount,dwGenreCount,dwTrackCount);


}


BOOL CMediaDragCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style = WS_CHILD |WS_TABSTOP | WS_VISIBLE |TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS;
	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	
	return CTreeCtrl::PreCreateWindow(cs);
}

void CMediaDragCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (HitTest(point) == GetSelectedItem())
	{
		DWORD itemData = GetItemData(GetSelectedItem());
		if ((itemData & TET_MASK) == TET_MEDIA)
		{
			DWORD mediaKey = (itemData & (~TET_MASK));
			CMediaElement * theElement = theMediaDatabase.mediaManager.GetMediaFromKey(mediaKey);
			ShellExecute(HWND(this),"open",theElement->GetFilePath(),NULL,NULL,SW_SHOWNOACTIVATE);
		}	
	}

	CTreeCtrl::OnLButtonDblClk(nFlags, point);
}

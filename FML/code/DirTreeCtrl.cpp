// DirTreeCtrl.cpp: 
// 
// wrapped CTreeCtrl to select and or display folders and files (optional )
// 

#include "stdafx.h"

#include "DirTreeCtrl.h"
#include "SortStringArray.h"
#include "medialibrary.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// index into the state image list
//#define STATE_SELECTED			1
//#define STATE_CHILD_SELECTED	2
//#define STATE_PARENT_SELECTED	3
//#define STATE_UNSELECTED		4

/////////////////////////////////////////////////////////////////////////////
// CDirTreeCtrl

CDirTreeCtrl::CDirTreeCtrl()
{
	m_strRoot = "";      // Auf Leer setzen
	m_StateImageList.Create( IDB_TREESTATES, 16, 1, RGB(255,255,255) );
	m_hasChanged = false;
}

CDirTreeCtrl::~CDirTreeCtrl()
{
	m_imgList.Detach();
	m_StateImageList.Detach();
}


BEGIN_MESSAGE_MAP(CDirTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CDirTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CDirTreeCtrl 

BOOL CDirTreeCtrl::DisplayTree(LPCTSTR strRoot, BOOL bFiles)
{
	DWORD dwStyle = GetStyle();   // read the windowstyle
	if ( dwStyle & TVS_EDITLABELS ) 
	{
		// Don't allow the user to edit ItemLabels
		ModifyStyle( TVS_EDITLABELS , 0 );
	}

	
	// Display the DirTree with the Rootname e.g. C:\
	// if Rootname == NULL then Display all Drives on this PC
    // First, we need the system-ImageList
	
	DeleteAllItems();

	if ( !GetSysImgList() )
		return FALSE;
    m_bFiles = bFiles;  // if TRUE, Display Path- and Filenames 
	if ( strRoot == NULL || strRoot[0] == '\0' )
	{
		if ( !DisplayDrives() )
			return FALSE;
		m_strRoot = "";
	}
    else
	{
		m_strRoot = strRoot;
		if ( m_strRoot.Right(1) != '\\' )
			m_strRoot += "\\";
		HTREEITEM hParent = AddItem( TVI_ROOT, m_strRoot );
		DisplayPath( hParent, strRoot );
	}
	return TRUE;	
}
/////////////////////////////////////////////////
BOOL CDirTreeCtrl::GetSysImgList()
/////////////////////////////////////////////////
{
	SHFILEINFO shFinfo;
	HIMAGELIST hImgList = NULL;

	if ( GetImageList( TVSIL_NORMAL ) )
		m_imgList.Detach();


	hImgList = (HIMAGELIST)SHGetFileInfo( "C:\\",
							  0,
							  &shFinfo,
							  sizeof( shFinfo ),
							  SHGFI_SYSICONINDEX | 
							  SHGFI_SMALLICON );
	if ( !hImgList )
	{
		m_strError = "Cannot retrieve the Handle of SystemImageList!";
		return FALSE;
	}

	m_imgList.m_hImageList = hImgList;    
    
	SetImageList( &m_imgList, TVSIL_NORMAL );

	SetImageList( &(m_StateImageList), TVSIL_STATE );

	return TRUE;   // OK
}


int GetDirectoryStatus(CString path)
{
	int i=0; bool found = false; bool parent = false; 

	while ((i < theMediaDatabase.contentDirectories.GetSize()) && (!found) && (!parent))
	{
		found = theMediaDatabase.contentDirectories.GetAt(i) == path;
		if (!found)
		{
			parent = (theMediaDatabase.contentDirectories.GetAt(i).Find(path,0)==0);
		}

		i++;
	}


	int ret = 0;

	if (found) ret = 1;
	else if (parent) ret = 2;

	return ret;
}


void CDirTreeCtrl::GetCheckedDirectories( HTREEITEM hItem)
{
	unsigned int status;

	if ((hItem != NULL) && (GetItemText(hItem) != ""))
	{
	
		//afxDump << "Passing1: " << GetFullPath(hItem) << "\n";
		if (ItemHasChildren(hItem))
		{
			GetCheckedDirectories(GetChildItem(hItem));
		}

		if (GetNextSiblingItem(hItem))
		{
			GetCheckedDirectories(GetNextSiblingItem(hItem));
		}

		status = GetItemState(hItem,TVIS_STATEIMAGEMASK) >> 12;
		if (status == CHECKED)
		{
			theMediaDatabase.contentDirectories.Add(GetFullPath(hItem));
			//afxDump << "Adding: " << GetFullPath(hItem) << "\n";
		}
#if 0
		else if (status == CHILD_CHECKED)
		{

			int numItems = theMediaDatabase.oldContentDirectories.GetSize();
			for (int i = 0; i < numItems; i++)
			{
				if (theMediaDatabase.oldContentDirectories.GetAt(i).Find(GetFullPath(hItem))==0)
				{

					afxDump << "Passing2: " << theMediaDatabase.oldContentDirectories.GetAt(i) << "\n";
					theMediaDatabase.contentDirectories.Add(theMediaDatabase.oldContentDirectories.GetAt(i));
				}
			}
		}
#endif
	}

}

BOOL CDirTreeCtrl::DisplayDrives()
{
	//
	// Displaying the Availible Drives on this PC
	// This are the First Items in the TreeCtrl
	//
	DeleteAllItems();
	char  szDrives[128];
	char* pDrive;

//	char buf[MAX_PATH];
//	LPITEMIDLIST itemlist;

//	SHGetSpecialFolderLocation (this->m_hWnd, CSIDL_DESKTOP, &itemlist);
//	SHGetPathFromIDList(itemlist, buf ); 


	if ( !GetLogicalDriveStrings( sizeof(szDrives), szDrives ) )
	{
		m_strError = "Error Getting Logical DriveStrings!";
		return FALSE;
	}

	pDrive = szDrives;
	while( *pDrive )
	{
		HTREEITEM hParent = AddItem( TVI_ROOT, pDrive );
		SetItemData(hParent,0);
		int status = GetDirectoryStatus(GetFullPath(hParent));

		if (status == 1)
		{
			SetItemState( hParent, INDEXTOSTATEIMAGEMASK(CHECKED),TVIS_STATEIMAGEMASK );
		}
		else if (status == 2)
		{
			SetItemState( hParent, INDEXTOSTATEIMAGEMASK(CHILD_CHECKED),TVIS_STATEIMAGEMASK );
		} 
		else 
		{
			SetItemState( hParent, INDEXTOSTATEIMAGEMASK(UNCHECKED),TVIS_STATEIMAGEMASK );
		} 

		int newState = GetItemState(hParent,TVIS_STATEIMAGEMASK) >> 12;

		SetCheck(hParent,REFRESH);

		newState = GetItemState(hParent,TVIS_STATEIMAGEMASK) >> 12;

		if ( FindSubDir( pDrive ) )
			InsertItem( "", 0, 0, hParent );

		pDrive += strlen( pDrive ) + 1;
	}


	return TRUE;

}




void CDirTreeCtrl::DisplayPath(HTREEITEM hParent, LPCTSTR strPath)
{
	//
	// Displaying the Path in the TreeCtrl
	//
	CFileFind find;
	CString   strPathFiles = strPath;
	BOOL      bFind;
	CSortStringArray strDirArray;
	CSortStringArray strFileArray;
	
	if ( strPathFiles.Right(1) != "\\" )
		strPathFiles += "\\";
	strPathFiles += "*.*";

	bFind = find.FindFile( strPathFiles );

	while ( bFind )
	{
		bFind = find.FindNextFile();
		if ( find.IsDirectory() && !find.IsDots() )
		{		
			strDirArray.Add( find.GetFilePath() );
		}
		if ( !find.IsDirectory() && m_bFiles )
			strFileArray.Add( find.GetFilePath() );

	}
    
	strDirArray.Sort();
	SetRedraw( FALSE );
	CWaitCursor wait;
    
	for ( int i = 0; i < strDirArray.GetSize(); i++ )
	{
			HTREEITEM hItem = AddItem( hParent, strDirArray.GetAt(i) );

			int status = GetDirectoryStatus(GetFullPath(hItem));

			if (IsParentChecked(hParent))
			{
				SetItemState( hItem, INDEXTOSTATEIMAGEMASK(PARENT_CHECKED),TVIS_STATEIMAGEMASK );
			}
			else
			{
				if (status == 1)
				{
					SetItemState( hItem, INDEXTOSTATEIMAGEMASK(CHECKED),TVIS_STATEIMAGEMASK );
				}
				else if (status == 2)
				{
					SetItemState( hItem, INDEXTOSTATEIMAGEMASK(CHILD_CHECKED),TVIS_STATEIMAGEMASK );
				} 
				else
				{
					SetItemState( hItem, INDEXTOSTATEIMAGEMASK(UNCHECKED),TVIS_STATEIMAGEMASK );
				}

			}

			SetCheck(hItem,REFRESH);

			SetItemData(hItem,0);
			if ( FindSubDir( strDirArray.GetAt(i) ) )
				InsertItem( "", 0, 0, hItem );
	}

	SetItemData(hParent,1);

	if ( m_bFiles )
	{
		strFileArray.Sort();
		for ( i = 0; i < strFileArray.GetSize(); i++ )
		{
			HTREEITEM hItem = AddItem( hParent, strFileArray.GetAt(i) );
			
		}
	}
    
	SetRedraw( TRUE );

	
}

HTREEITEM CDirTreeCtrl::AddItem(HTREEITEM hParent, LPCTSTR strPath)
{
	// Adding the Item to the TreeCtrl with the current Icons
	SHFILEINFO shFinfo;
	int iIcon, iIconSel;
    CString    strTemp = strPath;
    
	if ( strTemp.Right(1) != '\\' )
		 strTemp += "\\";
	if ( !SHGetFileInfo( strTemp,
						0,
						&shFinfo,
						sizeof( shFinfo ),
						SHGFI_ICON | 
					    SHGFI_SMALLICON ) )
	{
		m_strError = "Error Gettting SystemFileInfo!";
		return NULL;
	}

	iIcon = shFinfo.iIcon;

	// we only need the index from the system image list

	DestroyIcon( shFinfo.hIcon );

	if ( !SHGetFileInfo( strTemp,
						0,
						&shFinfo,
						sizeof( shFinfo ),
						SHGFI_ICON | SHGFI_OPENICON |
					    SHGFI_SMALLICON ) )
	{
		m_strError = "Error Gettting SystemFileInfo!";
		return NULL;
	}

	iIconSel = shFinfo.iIcon;

	// we only need the index of the system image list

	DestroyIcon( shFinfo.hIcon );

	if ( strTemp.Right(1) == "\\" )
		strTemp.SetAt( strTemp.GetLength() - 1, '\0' );
	
	if ( hParent == TVI_ROOT )
		return InsertItem( strTemp, iIcon, iIconSel, hParent );
	
	return InsertItem( GetSubPath( strTemp ), iIcon, iIconSel, hParent );
}

LPCTSTR CDirTreeCtrl::GetSubPath(LPCTSTR strPath)
{
	//
	// getting the last SubPath from a PathString
	// e.g. C:\temp\readme.txt
	// the result = readme.txt
	static CString strTemp;
	int     iPos;

	strTemp = strPath;
	if ( strTemp.Right(1) == '\\' )
		 strTemp.SetAt( strTemp.GetLength() - 1, '\0' );
	iPos = strTemp.ReverseFind( '\\' );
	if ( iPos != -1 )
	    strTemp = strTemp.Mid( iPos + 1);

	return (LPCTSTR)strTemp;
}

BOOL CDirTreeCtrl::FindSubDir( LPCTSTR strPath)
{
	//
	// Are there subDirs ?
	//
	CFileFind find;
	CString   strTemp = strPath;
	BOOL      bFind;

	if ( strTemp[strTemp.GetLength()-1] == '\\' )
		strTemp += "*.*";
	else
		strTemp += "\\*.*";
		
	bFind = find.FindFile( strTemp );
	
	
	while ( bFind )
	{
		bFind = find.FindNextFile();

		if ( find.IsDirectory() && !find.IsDots() )
		{
			return TRUE;
		}
		if ( !find.IsDirectory() && m_bFiles && !find.IsHidden() )
			return TRUE;
		
	}

	return FALSE;

}

void CDirTreeCtrl::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	CString strPath;
	 
	if ( pNMTreeView->itemNew.state & TVIS_EXPANDED )
	{
		//UINT uTest = TVIS_EXPANDEDONCE;
		ExpandItem( pNMTreeView->itemNew.hItem, TVE_EXPAND );
	    /*
		//
		// Delete All items
		// And display the subpath
		//
		HTREEITEM hChild = GetChildItem( pNMTreeView->itemNew.hItem );
		while ( hChild )
		{
			DeleteItem( hChild );
			hChild = GetChildItem( pNMTreeView->itemNew.hItem );
		}
        
		strPath = GetFullPath( pNMTreeView->itemNew.hItem );
		DisplayPath( pNMTreeView->itemNew.hItem, strPath );
		*/
	}
	else
	{
		//
		// Delete the Items, but leave one there, for 
		// expanding the item next time
		//
		
		/*
		HTREEITEM hChild = GetChildItem( pNMTreeView->itemNew.hItem );
				
		while ( hChild ) 
		{
			DeleteItem( hChild );
			hChild = GetChildItem( pNMTreeView->itemNew.hItem );
		}
		InsertItem( "", pNMTreeView->itemNew.hItem );

  */
	}

	*pResult = 0;
}

CString CDirTreeCtrl::GetFullPath(HTREEITEM hItem)
{
	// get the Full Path of the item
	CString strReturn;
	CString strTemp;
	HTREEITEM hParent = hItem;

	strReturn = "";

	while ( hParent )
	{
		
		strTemp  = GetItemText( hParent );
		strTemp += "\\";
		strReturn = strTemp + strReturn;
		hParent = GetParentItem( hParent );
	}
    
	strReturn.TrimRight( '\\' );

    return strReturn;

}

BOOL CDirTreeCtrl::SetSelPath(LPCTSTR strPath)
{
	ASSERT(0);
	// Setting the Selection in the Tree
	HTREEITEM hParent  = TVI_ROOT;
	int       iLen    = strlen(strPath) + 2;
	char*     pszPath = new char[iLen];
	char*     pPath   = pszPath;
	BOOL      bRet    = FALSE;
    
	if ( !IsValidPath( strPath ) )
	{
		delete [] pszPath; // this must be added 29.03.99
		return FALSE;
	}
		
	strcpy( pszPath, strPath );
	strupr( pszPath );
	
	if ( pszPath[strlen(pszPath)-1] != '\\' )
		strcat( pszPath, "\\" );
    
	int iLen2 = strlen( pszPath );
	
	for (WORD i = 0; i < iLen2; i++ )
	{
		if ( pszPath[i] == '\\' )
		{
			SetRedraw( FALSE );
			pszPath[i] = '\0';
			hParent = SearchSiblingItem( hParent, pPath );
			if ( !hParent )  // Not found!
				break;
			else
			{				
				// Info:
				// the notification OnItemExpanded 
				// will not called every time 
				// after the call Expand. 
				// You must call Expand with TVE_COLLAPSE | TVE_COLLAPSERESET
				// to Reset the TVIS_EXPANDEDONCE Flag
				
				UINT uState;
				uState = GetItemState( hParent, TVIS_EXPANDEDONCE );
				if ( uState )
				{
					Expand( hParent, TVE_EXPAND );
					Expand( hParent, TVE_COLLAPSE | TVE_COLLAPSERESET );
					InsertItem("", hParent ); // insert a blank child-item
					Expand( hParent, TVE_EXPAND ); // now, expand send a notification
				}
				else
					Expand( hParent, TVE_EXPAND );
			}
			pPath += strlen(pPath) + 1;
		}
	}

	delete [] pszPath;
	
	if ( hParent ) // Ok the last subpath was found
	{		
		SelectItem( hParent ); // select the last expanded item
		bRet = TRUE;
	}
	else
	{
		bRet = FALSE;
	}
	
	SetRedraw( TRUE );

    return bRet;
}

HTREEITEM CDirTreeCtrl::SearchSiblingItem( HTREEITEM hItem, LPCTSTR strText)
{
	HTREEITEM hFound = GetChildItem( hItem );
	CString   strTemp;
	while ( hFound )
	{
		strTemp = GetItemText( hFound );
        strTemp.MakeUpper();
		if ( strTemp == strText )
			return hFound;
		hFound = GetNextItem( hFound, TVGN_NEXT );
	}

	return NULL;
}


void CDirTreeCtrl::ExpandItem(HTREEITEM hItem, UINT nCode)
{	
	CString strPath;
	
	if ( nCode == TVE_EXPAND )
	{
		if (!GetItemData(hItem))
		{
			// if we don't already have the children, then get them
			HTREEITEM hChild = GetChildItem( hItem );

			// delete the dummy inserted one
			while ( hChild )
			{
				DeleteItem( hChild );
				hChild = GetChildItem( hItem );
			}
	        
			strPath = GetFullPath( hItem );
			DisplayPath( hItem, strPath );
		}
	}
}

BOOL CDirTreeCtrl::IsValidPath(LPCTSTR strPath)
{
	// This function check the Pathname
	
	HTREEITEM hChild;
	CString   strItem;
	CString   strTempPath = strPath;
	BOOL      bFound = FALSE;
	CFileFind find;

	hChild = GetChildItem( TVI_ROOT );
	strTempPath.MakeUpper();
	strTempPath.TrimRight('\\');

	while ( hChild )
	{
		strItem = GetItemText( hChild );
		strItem.MakeUpper();
		if ( strItem == strTempPath.Mid( 0, strItem.GetLength() ) )
		{
			bFound = TRUE;
			break;
		}
		hChild = GetNextItem( hChild, TVGN_NEXT );
	}
    
	if ( !bFound )
		return FALSE;

	strTempPath += "\\nul";
	if ( find.FindFile( strTempPath ) )
		return TRUE;
     
	return FALSE;
}




int CDirTreeCtrl::SetCheckState(HTREEITEM hItem, int stateIndex)
{
	TVITEM tvItem;

    tvItem.mask = TVIF_HANDLE | TVIF_STATE;
    tvItem.hItem = hItem;
    tvItem.stateMask = TVIS_STATEIMAGEMASK;


    tvItem.state = (stateIndex << 12);

    return SetItem(&tvItem);

}

BOOL CDirTreeCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style = WS_CHILD |WS_TABSTOP | WS_VISIBLE |TVS_CHECKBOXES|TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS;
	//cs.dwExStyle |= WS_EX_CLIENTEDGE;
	
	return CTreeCtrl::PreCreateWindow(cs);
}

void CDirTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	UINT uFlags=0;
	HTREEITEM hItem = HitTest(point,&uFlags);

	if( uFlags & TVHT_ONITEMSTATEICON )
	{
		SetCheck( hItem, TOGGLE );
		m_hasChanged = true;
		return;
	}

	CTreeCtrl::OnLButtonDown(nFlags, point) ;
}

bool CDirTreeCtrl::SetCheck(HTREEITEM hItem, CheckType nCheck)
{
	if( hItem == NULL ) 
	return FALSE;

	UINT nState = GetItemState( hItem, TVIS_STATEIMAGEMASK ) >> 12;

	//CHILD_CHECKED
	// Determine the new check state
	if ( nCheck == CHECK )
	{
		if( nState == CHECKED || nState == PARENT_CHECKED )
			return TRUE;
		switch( nState )
		{
		case UNCHECKED:		nState = CHECKED; break;
		case CHILD_CHECKED:		nState = CHECKED; break;
		}

		
	}
	else if( nCheck == UNCHECK )
	{
		if( nState == UNCHECKED || nState == CHILD_CHECKED )
			return TRUE;
		switch( nState )
		{
		case CHECKED:			nState = UNCHECKED; break;

		}

		
	}
	else if( nCheck == TOGGLE )
	{

		switch( nState )
		{
		case UNCHECKED:	
			
			nState = CHECKED;

			SetChildrenState(hItem,PARENT_CHECKED);

			break;
			
		case CHECKED:
			
			nState = UNCHECKED; 

			SetChildrenState(hItem,UNCHECKED);
			
			break;
		case CHILD_CHECKED:		
			nState = CHECKED; 
			
			SetChildrenState(hItem,PARENT_CHECKED);
			
			break;
		}
	}
	else if( nCheck == REFRESH )
	{
		// Scan children to determine new state
		BOOL bChildSelected = FALSE;
		HTREEITEM htiChild = GetChildItem( hItem );

		// Scan children
		while( htiChild )
		{
			UINT nChildState = GetItemState( htiChild, 
						TVIS_STATEIMAGEMASK ) >> 12;
			if( nChildState != UNCHECKED && nChildState != NOSTATE )
			{
				bChildSelected = TRUE;
				break;
			}
			htiChild = GetNextItem( htiChild, TVGN_NEXT );
		}
		if( bChildSelected )
		{
			if( nState == CHECKED ) nState = CHECKED;
			else if( nState == UNCHECKED ) nState = CHILD_CHECKED;
		}
		//else
		//{
		//	if( nState == SELF_AND_CHILD_CHECKED ) nState = CHECKED;
		//	else if( nState == CHILD_CHECKED ) nState = UNCHECKED;
		//}
	}

	SetItemState( hItem, INDEXTOSTATEIMAGEMASK(nState), 
					TVIS_STATEIMAGEMASK );

	if( nState == CHECKED 
		|| ( REFRESH 
			&& (nState == CHILD_CHECKED)
			)
		)
	{
		// Mark the parents to indicate that a child item is selected.
		// Use checkbox with red border.
		while( (hItem = GetParentItem( hItem )) != NULL )
		{
			nState = GetItemState( hItem, TVIS_STATEIMAGEMASK ) >> 12;
			if( nState == UNCHECKED )
				SetItemState( hItem, INDEXTOSTATEIMAGEMASK(CHILD_CHECKED), 
						TVIS_STATEIMAGEMASK );
			//else if ( nState == CHECKED )
			//	SetItemState( hItem,
			//		INDEXTOSTATEIMAGEMASK(SELF_AND_CHILD_CHECKED), 
			//		TVIS_STATEIMAGEMASK );
		}




	}
	else if( nState == UNCHECKED )
	{
		// Maybe the parent ( ancestors ) state needs to be adjusted if
		// no more children selected.
		while( (hItem = GetParentItem( hItem )) != NULL )
		{
			BOOL bChildSelected = FALSE;
			HTREEITEM htiChild = GetChildItem( hItem );

			// Scan children
			while( htiChild )
			{
				UINT nChildState = GetItemState( htiChild, 
							TVIS_STATEIMAGEMASK ) >> 12;
				if( nChildState != UNCHECKED && nChildState != NOSTATE )
				{
					bChildSelected = TRUE;
					break;
				}
				htiChild = GetNextItem( htiChild, TVGN_NEXT );
			}
			if( bChildSelected )
			{
				// The parent does not need to be updated
				// since there are other children still selected
				break;
			}
			else
			{
				// The parent item has no more children selected. 
				// Mark accordingly
				UINT nParentState = GetItemState( hItem, 
							TVIS_STATEIMAGEMASK ) >> 12;
				if( nParentState == CHILD_CHECKED )
					SetItemState( hItem, 
							INDEXTOSTATEIMAGEMASK(UNCHECKED), 
							TVIS_STATEIMAGEMASK );
				//else if ( nParentState == SELF_AND_CHILD_CHECKED )
				//	SetItemState( hItem, INDEXTOSTATEIMAGEMASK(CHECKED), 
				//			TVIS_STATEIMAGEMASK );
			}
		}

 	}                                                                                                              
	return TRUE;

}

bool CDirTreeCtrl::IsItemChecked(HTREEITEM hItem)
{
	int iImage = GetItemState( hItem, TVIS_STATEIMAGEMASK )>>12;
	return iImage == CHECKED ;//|| iImage == SELF_AND_CHILD_CHECKED;
}



HTREEITEM CDirTreeCtrl::GetFirstCheckedItem()
{
	for ( HTREEITEM hItem = GetRootItem(); hItem!=NULL;  )
	{
		int iImage = GetItemState( hItem, TVIS_STATEIMAGEMASK )>>12;
		if ( iImage == CHECKED )//|| iImage == SELF_AND_CHILD_CHECKED )
			return hItem;
		
		if( iImage != CHILD_CHECKED )
		{
			HTREEITEM hti = GetNextItem( hItem, TVGN_NEXT );
			if( hti == NULL )
				hItem = GetNextItem( hItem );
			else 
				hItem = hti;
		}
		else
			hItem = GetNextItem( hItem );
	}

	return NULL;

}

HTREEITEM CDirTreeCtrl::GetNextCheckedItem(HTREEITEM hItem)
{
	hItem = GetNextItem( hItem );
	while( hItem!=NULL )
	{
		int iImage = GetItemState( hItem, TVIS_STATEIMAGEMASK )>>12;
		if ( iImage == CHECKED )//|| iImage == SELF_AND_CHILD_CHECKED )
			return hItem;
		
		if( iImage != CHILD_CHECKED )
		{
			HTREEITEM hti = GetNextItem( hItem, TVGN_NEXT );
			if( hti == NULL )
				hItem = GetNextItem( hItem );
			else 
				hItem = hti;
		}
		else
			hItem = GetNextItem( hItem );
	}

	return NULL;


}

HTREEITEM CDirTreeCtrl::GetPrevCheckedItem(HTREEITEM hItem)
{
	for ( hItem = GetPrevItem( hItem ); hItem!=NULL; hItem = GetPrevItem( hItem ) )
		if ( IsItemChecked(hItem) )
			return hItem;
	return NULL;

}

// GetNextItem  - Get next item as if outline was completely expanded
// Returns      - The item immediately below the reference item
// hItem        - The reference item
HTREEITEM CDirTreeCtrl::GetNextItem( HTREEITEM hItem )
{
        HTREEITEM       hti;

        if( ItemHasChildren( hItem ) )
                return GetChildItem( hItem );           // return first child
        else{
                // return next sibling item
                // Go up the tree to find a parent's sibling if needed.
                while( (hti = GetNextSiblingItem( hItem )) == NULL ){
                        if( (hItem = GetParentItem( hItem ) ) == NULL )
                                return NULL;
                }
        }
        return hti;
}

// GetNextItem  - Get previous item as if outline was completely expanded
// Returns              - The item immediately above the reference item
// hItem                - The reference item
HTREEITEM CDirTreeCtrl::GetPrevItem( HTREEITEM hItem )
{
        HTREEITEM       hti;

        hti = GetPrevSiblingItem(hItem);
        if( hti == NULL )
                hti = GetParentItem(hItem);
        else
                hti = GetLastItem(hti);
        return hti;
}

// GetLastItem  - Gets last item in the branch
// Returns      - Last item
// hItem        - Node identifying the branch. NULL will 
//                return the last item in outine
HTREEITEM CDirTreeCtrl::GetLastItem( HTREEITEM hItem )
{
        // Last child of the last child of the last child ...
        HTREEITEM htiNext;

        if( hItem == NULL ){
                // Get the last item at the top level
                htiNext = GetRootItem();
                while( htiNext ){
                        hItem = htiNext;
                        htiNext = GetNextSiblingItem( htiNext );
                }
        }

        while( ItemHasChildren( hItem ) ){
                htiNext = GetChildItem( hItem );
                while( htiNext ){
                        hItem = htiNext;
                        htiNext = GetNextSiblingItem( htiNext );
                }
        }

        return hItem;
}







void CDirTreeCtrl::SetChildrenState(HTREEITEM hItem, int State)
{
	if (State == PARENT_CHECKED)
	{
		SetChildrenParented(hItem);
	}
	else if (State == UNCHECKED)
	{
		SetChildrenUnchecked(hItem);
	}
}

bool CDirTreeCtrl::IsParentChecked(HTREEITEM hParent)
{
	int iImage =  GetItemState( hParent, TVIS_STATEIMAGEMASK ) >> 12;
	return iImage == CHECKED || iImage == PARENT_CHECKED;
}

void CDirTreeCtrl::SetChildrenUnchecked(HTREEITEM hItem)
{
	HTREEITEM hNext;

	if (ItemHasChildren(hItem))
	{
		hNext = GetChildItem(hItem);

		SetItemState( hNext, INDEXTOSTATEIMAGEMASK(UNCHECKED),TVIS_STATEIMAGEMASK );
		SetChildrenUnchecked(hNext);

		while (hNext = GetNextSiblingItem(hNext))
		{
			SetItemState(hNext,INDEXTOSTATEIMAGEMASK(UNCHECKED),TVIS_STATEIMAGEMASK );
			SetChildrenUnchecked(hNext);
		}
	}

}

void CDirTreeCtrl::SetChildrenParented(HTREEITEM hItem)
{
	HTREEITEM hNext;

	if (ItemHasChildren(hItem))
	{
		hNext = GetChildItem(hItem);

		SetItemState( hNext, INDEXTOSTATEIMAGEMASK(PARENT_CHECKED),TVIS_STATEIMAGEMASK );
		SetChildrenParented(hNext);

		while (hNext = GetNextSiblingItem(hNext))
		{
			SetItemState(hNext,INDEXTOSTATEIMAGEMASK(PARENT_CHECKED),TVIS_STATEIMAGEMASK );
			SetChildrenParented(hNext);
		}
	}

}

#include <stdafx.h>
#include "medialibrary.h"


// CArtistManager implementation
// *****************************************************************************

IMPLEMENT_SERIAL(CArtistManager, CObject, 0);

CArtistManager::CArtistManager()
{
	CObject::CObject();
	// start at 50 elements, grow by 50 at a time
	_artistNames.SetSize(50,50);
	_artistNames.Add("INVALID");
}

CArtistManager::~CArtistManager()
{
	_artistNames.RemoveAll();
	CObject::~CObject();
}


DWORD CArtistManager::AddArtistToDatabase(const CString artistName)
{
	return _artistNames.Add(artistName);
}

CString CArtistManager::GetArtistFromKey(DWORD artistKey)
{
	if (_artistNames.GetUpperBound() == -1) return "";
	if (artistKey <= (DWORD)_artistNames.GetUpperBound())
		return _artistNames.GetAt(artistKey);
	else
		return "";
}

void CArtistManager::Serialize(CArchive &ar)
{
	CObject::Serialize( ar );
	_artistNames.Serialize (ar);
}


void CArtistManager::ClearAll()
{

	_artistNames.RemoveAll();
	_artistNames.Add("INVALID");
}

DWORD CArtistManager::GetCount()
{
	return _artistNames.GetSize();
}

DWORD CArtistManager::GetMaxIndex()
{
	return GetCount();
}


// CAlbumManager implementation
// *****************************************************************************

IMPLEMENT_SERIAL(CAlbumManager, CObject, 0);

CAlbumManager::CAlbumManager()
{
	CObject::CObject();
	_albumNames.SetSize(50,50);
	_albumNames.Add("INVALID");
}

CAlbumManager::~CAlbumManager()
{
	_albumNames.RemoveAll();
	CObject::~CObject();
}

DWORD CAlbumManager::AddAlbumToDatabase(CString albumName)
{
	return _albumNames.Add(albumName);
}

CString CAlbumManager::GetAlbumFromKey(DWORD albumKey)
{
	if (_albumNames.GetUpperBound() == -1) return "";
	if (albumKey <= (DWORD)_albumNames.GetUpperBound())
		return _albumNames.GetAt(albumKey);
	else
		return "";
}

void CAlbumManager::Serialize(CArchive &ar)
{
	CObject::Serialize( ar );
	_albumNames.Serialize(ar);
}


void CAlbumManager::ClearAll()
{
	_albumNames.RemoveAll();
	_albumNames.Add("INVALID");
}

DWORD CAlbumManager::GetCount()
{
	return _albumNames.GetSize();
}

DWORD CAlbumManager::GetMaxIndex()
{
	return GetCount();
}


// CGenreManager implementation
// *****************************************************************************

IMPLEMENT_SERIAL(CGenreManager, CObject, 0);

CGenreManager::CGenreManager()
{
	CObject::CObject();
	_genreNames.SetSize(50,50);
	_genreNames.Add("INVALID");

}

CGenreManager::~CGenreManager()
{
	_genreNames.RemoveAll();
	CObject::~CObject();
}

DWORD CGenreManager::AddGenreToDatabase(CString genreName)
{
	return _genreNames.Add(genreName);
}

CString CGenreManager::GetGenreFromKey(DWORD genreKey)
{
	if (genreKey <= (DWORD)_genreNames.GetUpperBound())
		return _genreNames.GetAt(genreKey);
	else
		return "";
}

void CGenreManager::Serialize(CArchive &ar)
{
	CObject::Serialize( ar );
	_genreNames.Serialize(ar);
}


void CGenreManager::ClearAll()
{
	_genreNames.RemoveAll();
	_genreNames.Add("INVALID");
}

DWORD CGenreManager::GetCount()
{
	return _genreNames.GetSize();
}

DWORD CGenreManager::GetMaxIndex()
{
	return GetCount();
}


 
// CMediaElement implementation
// *****************************************************************************

IMPLEMENT_SERIAL(CMediaElement, CObject, 0);

CMediaElement::CMediaElement(DWORD artistIndex, DWORD genreIndex, DWORD albumIndex, CString elementTitle, CString mimeType, CString filePath)
{
	CObject::CObject();
	_artistIndex  = artistIndex;
	_genreIndex   = genreIndex;
	_albumIndex   = albumIndex;
	_elementTitle = elementTitle;
	_mimeType     = mimeType;
	_filePath     = filePath;
	myIndex       = 0;
	_mediaType    = LOCAL_FILE;  // assume this
	_linkKey      = 0;
	_pathValidated = 0;
}

CMediaElement::CMediaElement(CMediaElement *element)
{
	CObject::CObject();
	_artistIndex  = element->_artistIndex;
	_genreIndex   = element->_genreIndex;
	_albumIndex   = element->_albumIndex;
	_elementTitle = element->_elementTitle;
	_mimeType     = element->_mimeType;
	_filePath     = element->_filePath;
	myIndex		  = 0;
	_mediaType    = LOCAL_FILE;
	_linkKey      = 0;
	_pathValidated = 0;
}


CMediaElement::CMediaElement(CRadioStation *station)
{
	CObject::CObject();

	_artistIndex  = 0;
	_genreIndex   = 0;
	_albumIndex   = 0;
	_elementTitle = "";
	_mimeType     = "x-ms-mp3";
	_filePath     = "";
	myIndex       = 0;
	_mediaType    = INTERNET_RADIO;
	_linkKey      = station->myIndex;
}
	

CMediaElement::CMediaElement()
{
	CObject::CObject();
	myIndex       = 0;
}



DWORD CMediaElement::GetAlbumIndex()
{
	return _albumIndex;
}

DWORD CMediaElement::GetArtistIndex()
{
	return _artistIndex;
}

DWORD CMediaElement::GetGenreIndex()
{
	return _genreIndex;
}

CString CMediaElement::GetElementTitle()
{
	if (_mediaType == INTERNET_RADIO)
	{
		CRadioStation * pRS = theMediaDatabase.radioManager.GetStationFromKey(_linkKey);
		if (pRS)
			return theMediaDatabase.radioManager.GetStationFromKey(_linkKey)->GetStationTitle();
		else
			return "";
	}
	else
	{
		return _elementTitle;
	}
}

CString CMediaElement::GetMimeType()
{
	return _mimeType;
}

CString CMediaElement::GetFilePath()
{
	return _filePath;
}

DWORD   CMediaElement::GetMediaType()
{
	return _mediaType;
}

void CMediaElement::SetMediaType(DWORD mediaType)
{
	_mediaType = mediaType;
}

void CMediaElement::SetLinkKey(DWORD linkKey)
{
	_linkKey = linkKey;
}

DWORD CMediaElement::GetLinkKey()
{
	return _linkKey;
}

void CMediaElement::SetPathValidity(int Validate)
{
	_pathValidated = Validate;
}

int CMediaElement::GetPathValidity()
{
	return _pathValidated;
}

void CMediaElement::SetAlbumPosition(DWORD albumPos)
{
	_albumPos = albumPos;
}

DWORD CMediaElement::GetAlbumPosition()
{
	return _albumPos;
}


void CMediaElement::UpdateMetadata(CMediaElement *pElement)
{
	_artistIndex  = pElement->GetArtistIndex();
	_genreIndex   = pElement->GetGenreIndex();
	_albumIndex   = pElement->GetAlbumIndex();
	_albumPos     = pElement->GetAlbumPosition();
	_elementTitle = pElement->GetElementTitle();
}

void CMediaElement::Serialize(CArchive &ar)
{
	CObject::Serialize( ar );
    if( ar.IsStoring() )
	{
		ar << _artistIndex;
		ar << _genreIndex;
		ar << _albumIndex;
		ar << _elementTitle;
		ar << _mimeType;
		ar << _filePath;
		ar << myIndex;
		ar << _mediaType;
		ar << _linkKey;
		ar << _pathValidated;
		ar << _albumPos;
	}
    else
	{
		ar >> _artistIndex;
		ar >> _genreIndex;
		ar >> _albumIndex;
		ar >> _elementTitle;
		ar >> _mimeType;
		ar >> _filePath;
		ar >> myIndex;
		ar >> _mediaType;
		ar >> _linkKey;
		ar >> _pathValidated;
		ar >> _albumPos;
	}
}


CMediaElement::operator = (CMediaElement *rhs)
{
		_artistIndex = rhs->_artistIndex;
		_genreIndex = rhs->_genreIndex;
		_albumIndex = rhs->_albumIndex;
		_elementTitle = rhs->_elementTitle;
		_mimeType = rhs->_mimeType;
}


int CMediaElement::operator == (CMediaElement *rhs)
{
	return (	(_artistIndex == rhs->_artistIndex)		&&
				(_genreIndex == rhs->_genreIndex)		&&
				(_albumIndex == rhs->_albumIndex)		&&
				(_elementTitle == rhs->_elementTitle)	&&
				(_mimeType == rhs->_mimeType)			
			);
}


// CMediaManager implementation
// *************************************************************************************************************

IMPLEMENT_SERIAL(CMediaManager, CObject, 0);

CMediaManager::CMediaManager()
{
	CObject::CObject();
	_mediaElements.SetSize(50,50);
	
	_mediaElements.RemoveAll();
	_mediaElements.Add(NULL);


}

CMediaManager::~CMediaManager()
{
	_mediaElements.RemoveAll();
	CObject::~CObject();
}

DWORD CMediaManager::AddMediaToDatabase(CMediaElement *mediaElement)
{
	// first look

	DWORD totalEntries = _mediaElements.GetSize();
	DWORD i=1;
	bool found = false;
	while ((i < totalEntries) && (!found))
	{
		found = (((CMediaElement*)_mediaElements.GetAt(i))->GetFilePath().CompareNoCase(mediaElement->GetFilePath())==0);
		i++;
	}
	DWORD index;
	if (found)
	{
		index = i-1;
		((CMediaElement*)_mediaElements.GetAt(index))->SetPathValidity(true);
		((CMediaElement*)_mediaElements.GetAt(index))->UpdateMetadata(mediaElement);
		delete mediaElement;
		return index;
	}
	else
	{
		mediaElement->SetPathValidity(true);
		index = _mediaElements.Add(mediaElement);
		mediaElement->myIndex = index;
		return index;
	}
}

CMediaElement* CMediaManager::GetMediaFromKey(DWORD mediaKey)
{
	DWORD key = mediaKey;
	// get the media element index my mediakey, or the next validated one
	// return null if at the end of the list

	if (_mediaElements.GetUpperBound() == -1) return NULL;
	DWORD upperBound =(DWORD)_mediaElements.GetUpperBound();
	if (mediaKey > upperBound) return NULL;
	CMediaElement * pElement = (CMediaElement * )_mediaElements.GetAt(key);
	return pElement;

}

void CMediaManager::ClearAll()
{
	DWORD totalEntries = _mediaElements.GetSize();

	for (int i = 1; i < (int)totalEntries; i++)
	{
		((CMediaElement*)_mediaElements.GetAt(i))->SetPathValidity(false);
	}
		
	//_mediaElements.RemoveAll();
	//_mediaElements.Add(NULL);
}

DWORD CMediaManager::GetCount()
{
	DWORD count = 0;

	DWORD totalEntries = _mediaElements.GetSize();

	for (int i = 1; i < (int)totalEntries; i++)
	{
		count +=(((CMediaElement*)_mediaElements.GetAt(i))->GetPathValidity());
	}

	return count;
}

DWORD CMediaManager::GetMaxIndex()
{
	return _mediaElements.GetUpperBound();
}


void CMediaManager::Serialize(CArchive &ar)
{
	_mediaElements.Serialize(ar);
}

// ************************************************************************************************************
// CRadioManager and CRadioStation implementation


IMPLEMENT_SERIAL(CRadioStation, CObject, 0);

CRadioStation::CRadioStation(CString name, CString url, CString mime)
{
	_name = name;
	_url  = url;
	_mime = mime;
	
}


CRadioStation::CRadioStation()
{
	CObject::CObject();

	_name = "";
	_url = "";
	_mime = "";
}


CString CRadioStation::GetStationTitle()
{
	return _name;
}

CString CRadioStation::GetStationURL()
{
	return _url;
}

CString	CRadioStation::GetStationMimeType()
{
	return _mime;
}

void CRadioStation::SetStationTitle(CString name)
{
	_name = name;
}


void CRadioStation::SetStationURL(CString url)
{
	_url = url;
}


void CRadioStation::Serialize(CArchive &ar)
{
	CObject::Serialize( ar );
    if( ar.IsStoring() )
	{
		ar << _name;
		ar << _url;
		ar << _mime;
		ar << myIndex;
	}
    else
	{
		ar >> _name;
		ar >> _url;
		ar >> _mime;
		ar >> myIndex;
	}
}


// **************************************************8
// CRadioManager implementation



IMPLEMENT_SERIAL(CRadioManager, CObject, 0);

CRadioManager::CRadioManager()
{
	CObject::CObject();
	stationList.SetSize(10,10);
	//stationList.Add(new CRadioStation("INVALID","INVALID","INVALID"));
}

CRadioManager::~CRadioManager()
{
	stationList.RemoveAll();
	CObject::~CObject();
}

DWORD CRadioManager::AddStationToDatabase(CRadioStation *radioStation)
{
	DWORD index = stationList.Add(radioStation);
	radioStation->myIndex = index;

	return index;
}

void CRadioManager::UpdateStationDetails(DWORD stationKey, CRadioStation *radioStation)
{
	if (stationKey <= (DWORD)stationList.GetUpperBound())
	{
		stationList.SetAt(stationKey,radioStation);
	}
}

CRadioStation* CRadioManager::GetStationFromKey(DWORD stationKey)
{
	if (stationList.GetUpperBound() == -1) return NULL;

	if (stationKey <= (DWORD)stationList.GetUpperBound())
		return (CRadioStation*)stationList.GetAt(stationKey);
	else
		return NULL;
}

void CRadioManager::Serialize(CArchive &ar)
{
	stationList.Serialize(ar);
}

void CRadioManager::ClearAll()
{
	stationList.RemoveAll();
	// index 0 is always reserved
//	stationList.Add(new CRadioStation("INVALID","INVALID"));
}

void CRadioManager::RemoveEntry(DWORD index)
{
	CRadioStation *temp;
	int ub = stationList.GetUpperBound();

	if (index <= (DWORD)ub)
	{
		temp = (CRadioStation *)stationList.GetAt(index);
		stationList.RemoveAt(index);
		delete temp;
	}

	// now we need to re-index the remainder of the stations, a list would probably be better!

	ub = stationList.GetUpperBound();

	if (ub != -1)
		while (index <= (DWORD)stationList.GetUpperBound())
		{
			temp = (CRadioStation *)stationList.GetAt(index);
			temp->myIndex = index;
			index++;
		}
		
}


// if the media database is refreshed, then the radio stations must be given ne mediaKeys
void CRadioManager::ReIndexRadioStations()
{
/*	CRadioStation *station;
	DWORD stationKey = 0;

	while (station = theMediaDatabase.radioManager.GetStationFromKey(stationKey++))
	{
		DWORD mediaIndex = theMediaDatabase.mediaManager.AddMediaToDatabase(new CMediaElement(station));
		station->myMediaKey = mediaIndex;
	}
*/
}

DWORD CRadioManager::GetMaxIndex()
{
	return stationList.GetSize();
}

DWORD CRadioManager::GetSize()
{
	return stationList.GetSize();
}








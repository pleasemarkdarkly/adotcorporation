#include "stdafx.h"
#include "medialibrary.h"
#include "responses.h"
#include "mediahelper.h"


CString TagItem(CString tag, CString item)
{
	CString response;
	response = "&lt;";
	response += tag;
	response += "&gt;";
	response += item;
	response += "&lt;";
	response += "/";
	response += tag;
	response += "&gt;";
	return response;
}

CString TagItem(CString tag, DWORD value)
{
	CString response;
	CString val;
	val.Format("%d",value);
	response = "&lt;";
	response += tag;
	response += "&gt;";
	response += val;
	response += "&lt;";
	response += "/";
	response += tag;
	response += "&gt;";	
	return response;
}


CString Append(CString str1, CString str2)
{
	CString response = str1;
	response += str2;
	return response;
}

CString GetItemContextME(DWORD key)
{
	CString response;
	char *ipAddress;
	CString titleName;
	CString artist;
	CString mimeType;


	CString mediaURL;
	CMediaElement * theMediaElement = theMediaDatabase.mediaManager.GetMediaFromKey(key);

	DWORD   mediaType = theMediaElement->GetMediaType();

	if (mediaType == LOCAL_FILE)
	{
		titleName = theMediaElement->GetElementTitle();
		artist    = theMediaDatabase.artistManager.GetArtistFromKey(theMediaElement->GetArtistIndex());
		mimeType  = theMediaElement->GetMimeType();

		GetIPAddress(&ipAddress);

		mediaURL.Format("%s%s%s%s%d",
					"http://",
					ipAddress,
					HTTP_CONTENT_PORT_TEXT,
					"/?MediaKey=",
					key);
	}
	response = TagItem("ME",Append(TagItem("MEK",key),
									Append(TagItem("MEN",titleName),
									Append(TagItem("MEU",mediaURL),
									Append(TagItem("MET",mimeType),
									TagItem("AR",TagItem("ARN",artist)))))));
	
	return response;
}



CString GetItemContextAR(DWORD key)
{
	CString response;

	CString artistName = theMediaDatabase.artistManager.GetArtistFromKey(key);

	response = TagItem("AR",Append(TagItem("ARK",key),TagItem("ARN",artistName)));

	return response;
}

CString GetItemContextAL(DWORD key)
{
	CString response;

	CString albumName = theMediaDatabase.albumManager.GetAlbumFromKey(key);

	response = TagItem("AL",Append(TagItem("ALK",key),TagItem("ALN",albumName)));

	return response;
}

CString GetItemContextGR(DWORD key)
{
	CString response;

	CString albumName = theMediaDatabase.genreManager.GetGenreFromKey(key);

	response = TagItem("GR",Append(TagItem("GRK",key),TagItem("GRN",albumName)));

	return response;
}

CString GetItemContextPL(DWORD key)
{
	CString response;

	CString playlistName = theMediaDatabase.playListManager.GetPlayListTitle(key-1);

	response = TagItem("PL",Append(TagItem("PLK",key),TagItem("PLN",playlistName)));

	return response;
}

CString GetItemContextRS(DWORD key)
{
	CString response = "";

	CRadioStation * pStation = theMediaDatabase.radioManager.GetStationFromKey(key) ;

	CString stationName; 
	CString stationURL;
	CString stationMime;

	if (pStation)
	{
		stationName = pStation->GetStationTitle();
		stationURL  = pStation->GetStationURL();
		stationMime = pStation->GetStationMimeType();

		response = TagItem("RS",Append(TagItem("RSU",stationURL),
		                    Append(TagItem("RSN",stationName),
							TagItem("MET",stationMime))));
	}

	return response;
}

DWORD FindItem(char jumpLetter,int jumpOffset,CDWordArray *items,xpath_context_t context)
{
	DWORD totalItems = (DWORD)items->GetSize();
	bool found = false;
	int i=1;

	CString compareString = "";

	while ((!found) && (i++ < (int)totalItems))
	{

		switch (context)
		{
		case CONTEXT_MEDIA: 
			compareString = theMediaDatabase.mediaManager.GetMediaFromKey(items->GetAt(i))->GetElementTitle();
			break;
		case CONTEXT_ARTIST:
			compareString = theMediaDatabase.artistManager.GetArtistFromKey(items->GetAt(i));
			break;
		case CONTEXT_ALBUM:
			compareString = theMediaDatabase.albumManager.GetAlbumFromKey(items->GetAt(i));
			break;
		case CONTEXT_GENRE:
			compareString = theMediaDatabase.genreManager.GetGenreFromKey(items->GetAt(i));
			break;
		case CONTEXT_PLAYLIST:
			compareString = theMediaDatabase.playListManager.GetPlayListTitle(items->GetAt(i)-1);
			break;
		case CONTEXT_RADIOSTATION:
			compareString = theMediaDatabase.radioManager.GetStationFromKey(items->GetAt(i))->GetStationTitle();
			break;
		default:
			break;
		}

		found = (compareString[0] == jumpLetter);
		i += jumpOffset;
	}

	if (found)
	{
		return i;
	}
	else
	{
		return 0;
	}


}

CString GetItemsFromView(DWORD viewID, DWORD startIndex, DWORD count,char * jumpLetter,int jumpOffset,DWORD *totalCount, DWORD *numReturned)
{
	CViewDetails * pViewDetails = theMediaDatabase.viewManager.GetViewDetails(viewID);
	if (!pViewDetails)
	{
		// we don't know about this particular viewID - oops
		// don't response at all - this will generate an HTTP error at the HTTP thread layer
		return "";
	}

	CString response;

	CDWordArray * items = pViewDetails->GetItems();
	DWORD totalItems = (DWORD)items->GetSize();
	*totalCount = totalItems;
	*numReturned = 0;
	xpath_context_t context = pViewDetails->GetViewContext();


	
	DWORD i;

	if (jumpLetter != NULL)
	{
		i = FindItem(jumpLetter[0],jumpOffset,items,context);
	}
	else
	{
		i = startIndex;
		if (startIndex > totalItems)
		return "";
	}

	DWORD itemsLeft = count;


	while ((i < totalItems) && itemsLeft)
	{
		switch (context)
		{
		case CONTEXT_MEDIA: 
			response += GetItemContextME(items->GetAt(i));
			break;
		case CONTEXT_ARTIST:
			response += GetItemContextAR(items->GetAt(i));
			break;
		case CONTEXT_ALBUM:
			response += GetItemContextAL(items->GetAt(i));
			break;

		case CONTEXT_GENRE:
			response += GetItemContextGR(items->GetAt(i));
			break;
		case CONTEXT_PLAYLIST:
			// quick fix since playlist are indexed from 0
			response += GetItemContextPL(items->GetAt(i));
			break;
		case CONTEXT_RADIOSTATION:
			response += GetItemContextRS(items->GetAt(i));
			break;
		case CONTEXT_UNKNOWN:
		default:
			break;
		}

		//response += "\r\n";
		
		itemsLeft--;
		i++;
	}

	*numReturned = i - startIndex;

	return response;
}
















	



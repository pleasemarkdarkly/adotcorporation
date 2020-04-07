#include <stdafx.h>

#include <TCHAR.h>



//#include <stdlib.h>
//#include <stdio.h>
#include <string.h>

#include "wmsdk.h"
#include "metadata.h"

#include "id3_tag.h"

//#include "id3_dll.h"


#define SAFE_RELEASE( x )           \
               if( x )              \
               {                    \
                   x->Release();    \
                   x = NULL;        \
               }

#include "medialibrary.h"
//upper bounds x to value y
#define maxlimit(x,y) x > y ? y : x




#define ID3V2_CHUNKSIZE 512  	// number of bytes to read from beginning of file
#define ID3V1_CHUNKSIZE 128		// number of bytes to read from end of file

char *genreText[];				//forward reference to this array
#define MAX_GENRE_VALUE 124		//the number of entries in the genre text array


HRESULT ConvertWCtoMB ( LPWSTR pwszInString, LPTSTR *pptszOutString)
{
	if( pwszInString == NULL || pptszOutString == NULL )
	{
		return E_INVALIDARG ;
	}

    HRESULT hr = S_OK ;

	int nSizeCount = 0 ;
    do
    {
        //
	    // Get the memory reqd for this string
	    //

		

        nSizeCount = WideCharToMultiByte(CP_ACP, 0, pwszInString, -1, NULL, 0, NULL, NULL);
        if( 0 ==  nSizeCount )
        {
            hr = HRESULT_FROM_WIN32( GetLastError() ) ;
            break ;
        }

        *pptszOutString = new CHAR[nSizeCount] ;
        if( NULL == *pptszOutString )
        {
            hr = HRESULT_FROM_WIN32( GetLastError() ) ;
            break ;
        }

		if ( 0 == WideCharToMultiByte(CP_ACP, 0, pwszInString, -1, *pptszOutString, nSizeCount,NULL, NULL))
        {
            hr = HRESULT_FROM_WIN32( GetLastError() ) ;
            break ;
        }
    }
    while( FALSE ) ;
    
    if( FAILED( hr ) )
    {
        delete[] (*pptszOutString) ;
        *pptszOutString = NULL ;
        _tprintf( _T( "Internal error (hr=0x%08x)\n" ), hr );
    }

	return hr;
}



HRESULT ConvertMBtoWC( LPCTSTR ptszInString, LPWSTR *ppwszOutString )
{
	if( ptszInString == NULL || ppwszOutString == NULL )
	{
		return E_INVALIDARG ;
	}

    HRESULT hr = S_OK ;

	int nSizeCount = 0 ;
    do
    {
        //
	    // Get the memory reqd for this string
	    //
        nSizeCount = MultiByteToWideChar( CP_ACP, 0, ptszInString, -1, NULL, 0 ) ;
        if( 0 ==  nSizeCount )
        {
            hr = HRESULT_FROM_WIN32( GetLastError() ) ;
            break ;
        }

        *ppwszOutString = new WCHAR[nSizeCount] ;
        if( NULL == *ppwszOutString )
        {
            hr = HRESULT_FROM_WIN32( GetLastError() ) ;
            break ;
        }

        if( 0 == MultiByteToWideChar( CP_ACP, 0, ptszInString, -1, *ppwszOutString, nSizeCount ) )
        {
            hr = HRESULT_FROM_WIN32( GetLastError() ) ;
            break ;
        }
    }
    while( FALSE ) ;
    
    if( FAILED( hr ) )
    {
        delete[] (*ppwszOutString) ;
        *ppwszOutString = NULL ;
        _tprintf( _T( "Internal error (hr=0x%08x)\n" ), hr );
    }

	return hr;
}


void
AddID3v2String(char * buf, ID3_Tag& tag, ID3_FrameID frameID)
{
    ID3_Frame *myFrame;

    if (myFrame = tag.Find(frameID))
    {
	    myFrame->Field(ID3FN_TEXT).Get(buf, 79);
    }
}


BOOL GetMP3Metadata(CString filepath,IML_METADATA *metadata)
{
	char * pFileName = (char*)malloc(filepath.GetLength()+1);
	strcpy(pFileName,filepath);

	ID3_Tag fileTag(pFileName);

	
    AddID3v2String(metadata->title, fileTag, ID3FID_TITLE);
    AddID3v2String(metadata->artist, fileTag, ID3FID_LEADARTIST);
    AddID3v2String(metadata->album, fileTag, ID3FID_ALBUM);
    AddID3v2String(metadata->genre, fileTag, ID3FID_CONTENTTYPE);
	AddID3v2String(metadata->albumPosition, fileTag, ID3FID_TRACKNUM);

	char genreNum[4];
	int iGenreNum;
	char *startNum, *endNum;
	if (startNum=strstr(metadata->genre,"("))
	{
		startNum++;
		endNum = strstr(startNum,")");
		if (endNum)
		{
			strncpy(genreNum,startNum,endNum-startNum);
			genreNum[endNum-startNum] = '\0';
			iGenreNum = atoi(genreNum);
			if ((iGenreNum < MAX_GENRE_VALUE) && (iGenreNum > 0))
			{
				int len = strlen(genreText[iGenreNum]);
				memcpy(metadata->genre,genreText[iGenreNum],len);
				metadata->genre[len] = '\0';
			}
			else
			{
				strcpy(metadata->genre,"Unknown Genre");
			}
		}
	}

	free(pFileName);

	return true;

	
}

// text for genres in ID3
// see:  http:///www.id3.org

char *genreText[] =
{
	"Blues",
	"Classic Rock",
	"Country",
	"Dance",
	"Disco",
	"Funk",
	"Grunge",
	"Hip-Hop",
	"Jazz",
	"Metal",
	"New Age",
	"Oldies",
	"Other",
	"Pop",
	"R&B",
	"Rap",
	"Reggae", 
	"Rock",
	"Techno",
	"Industrial",
	"Alternative",
	"Ska",
	"Death Metal",
	"Pranks",
	"Soundtrack",
	"Euro-Techno",
	"Ambient",
	"Trip-Hop",
	"Vocal",
	"Jazz+Funk",
	"Fusion",
	"Trance",
	"Classical", 
	"Instrumental", 
	"Acid",
	"House",
	"Game",
	"Sound Clip",
	"Gospel",
	"Noise",
	"AlternRock",
	"Bass",
	"Soul",
	"Punk",
	"Space",
	"Meditative",
	"Instrumental Pop",
	"Instrumental Rock",
	"Ethnic",
	"Gothic",
	"Darkwave",
	"Techno-Industrial",
	"Electronic",
	"Pop-Folk",
	"Eurodance",
	"Dream",
	"Southern Rock",
	"Comedy",
	"Cult",
	"Gangsta",
	"Top 40",
	"Christian Rap",
	"Pop/Funk",
	"Jungle",
	"Native American",
	"Cabaret",
	"New Wave",
	"Psychadelic",
	"Rave",
	"Showtunes",
	"Trailer",
	"Lo-Fi",
	"Tribal",
	"Acid Punk",
	"Acid Jazz",
	"Polka",
	"Retro",
	"Musical",
	"Rock & Roll",
	"Hard Rock",
	"Folk",
	"Folk/Rock",
	"National Folk",
	"Swing",
	"Bebob",
	"Latin",
	"Revival",
	"Celtic",
	"Bluegrass",
	"Avantgarde",
	"Gothic Rock",
	"Progressive Rock",
	"Psychedelic Rock",
	"Symphonic Rock",
	"Slow Rock",
	"Big Band",
	"Chorus",
	"Easy Listening",
	"Acoustic",
	"Humour",
	"Speech",
	"Chanson",
	"Opera",
	"Chamber Music",
	"Sonata",
	"Symphony",
	"Booty Bass",
	"Primus",
	"Porn Groove",
	"Satire",
	"Slow Jam",
	"Club",
	"Tango",
	"Samba",
	"Folklore",
	"Ballad",
	"Power Ballad",
	"Rythmic Soul",
	"Freestyle",
	"Duet",
	"Punk Rock",
	"Drum Solo",
	"A capella",
	"Euro-House",
	"Dance Hall",
};

BOOL GetWMAMetadata(CString filename,IML_METADATA *metadata)
{
	// Quite a bit of this function is borrowed from the Windows Media Format SDK - ID3 example code
	HRESULT hr      = S_OK ;

    IWMMetadataEditor* pEditor      = NULL;
    IWMHeaderInfo*     pInfo  = NULL;

	if (!metadata)
		return FALSE;

	// initialize the metadata structure
	memset(metadata,0,sizeof(IML_METADATA));

	// "do while" loop with breaks on errors
	do
	{
        //
        // Lets create a Metadata Editor.
        //
        hr = WMCreateEditor( &pEditor );
        if( FAILED( hr ) )
        {
			break;
        }

		LPWSTR pwszFilename = NULL;
		hr = ConvertMBtoWC( filename, &pwszFilename ) ;
		if( FAILED( hr ) )
        {
			break;
        }

        hr = pEditor->Open(pwszFilename);
        if( FAILED ( hr ) )
        {
	        //_tprintf( _T( "Could not open outfile %ws (hr=0x%08x).\n" ), pwszInFile, hr );
	        break ;
        }

        hr = pEditor->QueryInterface( IID_IWMHeaderInfo, ( void ** ) &pInfo );
        if( FAILED( hr ) )
        {
	        //_tprintf( _T( "Could not QI for IWMHeaderInfo (hr=0x%08x).\n" ), hr );
	        break ;
        }


		WORD wStreamNum = 0;
		WMT_ATTR_DATATYPE Type;
		WORD cbLength;


		// First check if this wma file had any DRM flags set, if so we'll just skip it for now
		
		hr = pInfo->GetAttributeByName( &wStreamNum, L"Is_Protected", &Type, NULL, &cbLength );

		if (hr != ASF_E_NOTFOUND)
		{

			// check we get a BOOL as expected
	
			if (Type = WMT_TYPE_BOOL)
			{
				BOOL IsProtected;
				hr = pInfo->GetAttributeByName( &wStreamNum, L"Is_Protected", &Type, (BYTE *)&IsProtected, &cbLength );
				if( FAILED( hr ) )
				{
					break;
				}	
		
				// if the wma is drm protected, we want to return FALSE
				if (IsProtected)
				{
					hr = ASF_E_NOTFOUND;  //anything != S_OK will do - we just want to return false at the end
					break;
				}
			}
		}

		// Get the title metadata
		//
		// First retrieve the length of the string, and allocate memory.
		//
		hr = pInfo->GetAttributeByName( &wStreamNum, L"Title", &Type, NULL, &cbLength );

		// if we didn't get a title we'll do the default title later from the filename

		if( FAILED( hr ) && hr != ASF_E_NOTFOUND)
		{
			break;
		}

		if (hr != ASF_E_NOTFOUND)
		{

			WCHAR *pwszTitle = (WCHAR *) new BYTE[ cbLength ];
			if( !pwszTitle )
			{
				break;
			}
			//
			// Now, retrieve the string itself.
			//
			hr = pInfo->GetAttributeByName( &wStreamNum, L"Title", &Type, (BYTE *) pwszTitle, &cbLength );
			if( FAILED( hr ) )
			{
				break;
			}

			if (cbLength != 0)
			{
				char * title = NULL;
				hr = ConvertWCtoMB( pwszTitle, &title );
	
				if (FAILED (hr) )
				{
					break;
				}	

				memcpy(metadata->title,title,maxlimit(strlen(title),METADATA_STRING_LENGTH));
				metadata->title[maxlimit(strlen(title),METADATA_STRING_LENGTH-1)] = '\0';
				delete title;
			}

			delete pwszTitle;
			
		}

		// Get the artist name metadata

		hr = pInfo->GetAttributeByName( &wStreamNum, L"Author", &Type, NULL, &cbLength );

		if( FAILED( hr ) && hr != ASF_E_NOTFOUND)
		{
			break;
		}

		if (hr != ASF_E_NOTFOUND)
		{
			WCHAR *pwszArtist = (WCHAR *) new BYTE[ cbLength ];
			if( !pwszArtist )
			{
				break;
			}
			hr = pInfo->GetAttributeByName( &wStreamNum, L"Author", &Type, (BYTE *) pwszArtist, &cbLength );
			if( FAILED( hr ) )
			{
				break;
			}

			if (cbLength != 0)
			{
	
				char * artist = NULL;
				hr = ConvertWCtoMB( pwszArtist, &artist);
	
				if (FAILED (hr) )
				{
					break;
				}

				memcpy(metadata->artist,artist,maxlimit(strlen(artist),METADATA_STRING_LENGTH));
				metadata->artist[maxlimit(strlen(artist),METADATA_STRING_LENGTH-1)] = '\0';

				delete artist;
			}

			delete pwszArtist;
		}

		// Get the album name metadata

		hr = pInfo->GetAttributeByName( &wStreamNum, L"WM/AlbumTitle", &Type, NULL, &cbLength );
		if( FAILED( hr ) && hr != ASF_E_NOTFOUND)
		{
			break;
		}

		if (hr != ASF_E_NOTFOUND)
		{

			WCHAR *pwszAlbum = (WCHAR *) new BYTE[ cbLength ];
			if( !pwszAlbum)
			{
				break;
			}
			//
			// Now, retrieve the string itself.
			//
			hr = pInfo->GetAttributeByName( &wStreamNum, L"WM/AlbumTitle", &Type, (BYTE *) pwszAlbum, &cbLength );
			if( FAILED( hr ) )
			{
				break;
			}

			if (cbLength != 0)
	
			{
				char * album = NULL;
				hr = ConvertWCtoMB( pwszAlbum, &album);
	
				if (FAILED (hr) )
				{
					break;
				}
	
				memcpy(metadata->album,album,maxlimit(strlen(album),METADATA_STRING_LENGTH));
				metadata->album[maxlimit(strlen(album),METADATA_STRING_LENGTH-1)] = '\0';

				delete album;
			}

			delete pwszAlbum;
		}

		// Get the genre information

		hr = pInfo->GetAttributeByName( &wStreamNum, L"WM/Genre", &Type, NULL, &cbLength );
		if( FAILED( hr ) && hr != ASF_E_NOTFOUND)
		{
			break;

		}

		if (hr != ASF_E_NOTFOUND)
		{

			WCHAR *pwszGenre = (WCHAR *) new BYTE[ cbLength ];
			if( !pwszGenre )
			{
				break;
			}
			//
			// Now, retrieve the string itself.
			//
			hr = pInfo->GetAttributeByName( &wStreamNum, L"WM/Genre", &Type, (BYTE *) pwszGenre, &cbLength );
			if( FAILED( hr ) )
			{
				break;
			}

			if (cbLength != 0)
	
			{
				char * genre = NULL;
				hr = ConvertWCtoMB( pwszGenre, &genre);
	
				if (FAILED (hr) )
				{
					break;
				}
		
				memcpy(metadata->genre,genre,maxlimit(strlen(genre),METADATA_STRING_LENGTH));
				metadata->genre[maxlimit(strlen(genre),METADATA_STRING_LENGTH-1)] = '\0';

				delete genre;
			}

			delete pwszGenre;
		}


		// Get the track information

		hr = pInfo->GetAttributeByName( &wStreamNum, L"WM/Track", &Type, NULL, &cbLength );
		if( FAILED( hr ) && hr != ASF_E_NOTFOUND)
		{
			break;
		}

		if (hr != ASF_E_NOTFOUND)
		{

			WCHAR *pwszTrack = (WCHAR *) new BYTE[ cbLength ];
			if( !pwszTrack )
			{
				break;
			}
			//
			// Now, retrieve the string itself.
			//
			hr = pInfo->GetAttributeByName( &wStreamNum, L"WM/Track", &Type, (BYTE *) pwszTrack, &cbLength );
			if( FAILED( hr ) )
			{
				break;
			}
	
			char * track = NULL;
			hr = ConvertWCtoMB( pwszTrack, &track);
	
			if (FAILED (hr) )
			{
				break;
			}

			sprintf(metadata->albumPosition,"%d",(unsigned char)track[0]);
	
			delete pwszTrack;
			delete track;
		}

		hr = S_OK;



    }
    while( FALSE ) ;

	hr = pEditor->Close();
    
    SAFE_RELEASE( pInfo ) ;
    SAFE_RELEASE( pEditor ) ;

	char genreNum[4];
	int iGenreNum;
	char *startNum, *endNum;
	if (startNum=strstr(metadata->genre,"("))
	{
		startNum++;
		endNum = strstr(startNum,")");
		if (endNum)
		{
			strncpy(genreNum,startNum,endNum-startNum);
			genreNum[endNum-startNum] = '\0';
			iGenreNum = atoi(genreNum);
			if ((iGenreNum < MAX_GENRE_VALUE) && (iGenreNum > 0))
			{
				int len = strlen(genreText[iGenreNum]);
				memcpy(metadata->genre,genreText[iGenreNum],len);
				metadata->genre[len] = '\0';
			}
			else
			{
				strcpy(metadata->genre,"Unknown Genre");
			}
		}
	}


	return (! FAILED(hr));

}



			
				






	








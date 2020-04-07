#ifndef _IML_UPNP_H_
#define _IML_UPNP_H_

// This is the header file for the interfaces between the upnp and htpp server and the media
// library database


// query types:

#define QUERY_ARTIST_VIEW			1	
#define QUERY_ALBUM_VIEW			2		
#define QUERY_ARTIST_ALBUMS_VIEW	3	
#define QUERY_ALBUM_DETAILS_VIEW	4   
#define QUERY_GENRE_VIEW			5	
#define QUERY_GENRE_DETAILS_VIEW	6	
#define QUERY_TITLE_VIEW			7
#define QUERY_MEDIA_DETAILS			8	
#define QUERY_INTERNET_RADIO		9
#define QUERY_ARTIST_TITLES			10
#define QUERY_PLAYLISTS             11
#define QUERY_PLAYLIST_DETAILS      12


char * iml_Query(char * request,DWORD qMAC);

char * iml_GetReleaseViewResponse(char * buffer,DWORD qMAC);

char * iml_ViewItems(char *request,DWORD qMAC);



#endif
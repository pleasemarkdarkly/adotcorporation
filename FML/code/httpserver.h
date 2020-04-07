#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

// Header file for the http server


// cheat and put the colon in too

// these must correspond
#define HTTP_LISTEN_PORT_TEXT ":2900"
#define HTTP_LISTEN_PORT 2900

#define HTTP_CONTENT_PORT_TEXT ":3030"
#define HTTP_CONTENT_PORT 3030

#define HTTP_HTML_PORT 4040
#define HTTP_HTML_PORT_TEXT ":4040"

#define MAX_PENDING_CONNECTS 10

//chunk size for serving media
#define CHUNK_SIZE 5982
//#define SPEW_CHUNKS


#define descName "/desc.xml"


void StopHTTPServer();

BOOL StartHTTPServer();



#endif

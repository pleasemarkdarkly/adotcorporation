//........................................................................................
//........................................................................................
//.. File Name: httpserver.cpp											
//.. Date: 6/18/2000																		
//.. Author(s): Rob Laubscher															
//.. Description of content: contains the definition of the CMSMediaStoreDataSource	
//..		class																		..
//.. Usage: Call only StartHTTPServer()  and StopHTTPServer() from other modules.  Everything else
//..        is internal.
//.. Last Modified By: Rob Laubscher  robl@iobjects.com
//.. Modification date: 6/18/2001														..
//........................................................................................
//.. Copyright:(c) 1995-2001 Interactive Objects Inc.  									..
//..	 All rights reserved. This code may not be redistributed in source or linkable  ..
//.. 	 object form without the express written consent of Interactive Objects.        ..
//.. Contact Information: www.iobjects.com												..
//........................................................................................
//........................................................................................

/*

  The basic flow in this HTTP server is as follows:

  StartHTTPServer
	- starts a thread that listens on our upnp port (HTTPListenThread)
	- starts a thread that listens on the content server port (HTTPContentThread)

  HTTPListenThread
	- accepts a connections 
	- creates a worker thread to handle it (HTTPConnectThread)

  HTTPConnectThread
	- actually processes the incoming data and sends the responses
	- this determines the kind of command that has come in (SOAP, GENA, GET, WebDAV(later)  etc)

  HTTPContentListThread
	- processing the incoming requests for media
	- creates a thread to deal with the media request

  HTTPContentServerThread
    - chunks up a media files and 'streams' it.

*/



#include "stdafx.h"

#include "global.h"
#include "ssdp.h"
#include "network.h"
#include "parser.h"
#include "util.h"
#include "httpserver.h"
#include "iml_upnp.h"
#include "encode.h"
#include "medialibrary.h"
#include "iml_xml.h"

#include "iphlpapi.h"
#include <afxtempl.h>



// This handle is to an event that is signalled when the threads must die
HANDLE g_HTTPThreadSignal;

// Scratch area for handles and IDs of worker threads that are created
HANDLE g_hListenThread;
DWORD  g_dwListenThreadID;
HANDLE g_hContentThread;
DWORD  g_dwContentThreadID;


HANDLE g_hQueryThread;
DWORD  g_dwQueryThreadID;

// thread entry points
DWORD WINAPI HTTPListenThread(void *param);
DWORD WINAPI HTTPContentServerThread(void *param);
DWORD WINAPI HTTPContentListenThread(void *param);
DWORD WINAPI ContentPoolThread(void *param);
DWORD WINAPI HTTPQueryThread(void *param);

// called in HTTPContentServerThread to stream the requested file
BOOL HTTPSpewFile(char *buffer, SOCKET outSocket);

// various forward references
void ConstructHTTPResponsePacket(char* pBytes,u_long ContentLength, char *ContentType);
BOOL HandleHTTPRequest(char *buffer,SOCKET ourSocket,DWORD qMAC);
BOOL HandleHTTP_POST(char *buffer, SOCKET ourSocket, DWORD qMAC);
BOOL HandleHTTP_GET(char *buffer, SOCKET ourSocket,DWORD qMAC);
void sendErrorReply(SOCKET ourSocket);
char * getLocalFile(char * filename);
BOOL isXML;

typedef struct
{
	HANDLE hThreadHandle;
	DWORD  dwThreadID;
	SOCKET sSocket;
	HANDLE flSignal;
	DWORD  dwStatus;
} t_ThreadInfo;

#define MAX_CONTENT_THREADS   8

t_ThreadInfo      ContentThreadPool[MAX_CONTENT_THREADS];
CRITICAL_SECTION  csContentThreadPool;

HANDLE   hQueryThreadSemaphore;

CList<unsigned int,unsigned int> lQueryQueue;
CList<unsigned int,unsigned int> lQueryMACs;

CRITICAL_SECTION queryQueueAccess;

#define TPOOL_CREATED 0
#define TPOOL_READY   1
#define TPOOL_RUNNING 2

void InitializeThreadPool()
{
	InitializeCriticalSection(&csContentThreadPool);

	EnterCriticalSection(&csContentThreadPool);
	for (int i=0; i < MAX_CONTENT_THREADS; i++)
	{
		ContentThreadPool[i].dwStatus = TPOOL_CREATED;;
		ContentThreadPool[i].flSignal = CreateEvent(NULL, TRUE, FALSE, NULL);
		ContentThreadPool[i].hThreadHandle = CreateThread(0,0, 
                                    (LPTHREAD_START_ROUTINE) ContentPoolThread,
                                    (LPVOID)i,
                                    CREATE_SUSPENDED ,
                                    &ContentThreadPool[i].dwThreadID);
		ContentThreadPool[i].sSocket = 0;
	}

	for (i=0; i < MAX_CONTENT_THREADS; i++)
	{
		ResumeThread(ContentThreadPool[i].hThreadHandle);
	}

	LeaveCriticalSection(&csContentThreadPool);
}

void CleanupThreadPools()
{
	for (int i = 0; i < MAX_CONTENT_THREADS; i++)
	{
		CloseHandle(ContentThreadPool[i].flSignal);
		CloseHandle(ContentThreadPool[i].hThreadHandle);
	}

}


bool RunContentThread(SOCKET socket)
{
	EnterCriticalSection(&csContentThreadPool);

	int i = 0;
	bool found = false;

#ifdef _VERBOSE_DEBUG
	LogOutput("Content: Status of thread pool before :\r\n");

	for (int j = 0; j < MAX_CONTENT_THREADS;j++)
	{
		LogOutput("%d ",ContentThreadPool[j].dwStatus);
	}
	LogOutput("\r\n");
#endif

	while ((i < MAX_CONTENT_THREADS) && (!found))
	{
		found = (ContentThreadPool[i].dwStatus == TPOOL_READY);
		i++;
	}

	if (found)
	{
		i--;

#ifdef _VERBOSE_DEBUG
		LogOutput("Content: Using thread pool %d :\r\n",i);
#endif

		ContentThreadPool[i].dwStatus = TPOOL_RUNNING;

		ContentThreadPool[i].sSocket = socket;
		SetEvent(ContentThreadPool[i].flSignal);
	}

#ifdef _VERBOSE_DEBUG
	LogOutput("Content: Status of thread pool after :\r\n");

	for (j = 0; j < MAX_CONTENT_THREADS;j++)
	{
		LogOutput("%d ",ContentThreadPool[j].dwStatus);
	}
	LogOutput("\r\n");
#endif

	LeaveCriticalSection(&csContentThreadPool);

	return found;
}




bool ValidateMACAddress(SOCKADDR_IN *sockAddr, DWORD *retMACAddr)
{
	// Get the ARP table

	PMIB_IPNETTABLE pIpNetTable=NULL;
	ULONG           dwSize;

	bool validMAC = false;

	dwSize = 0;

	// first see how big the table is
	GetIpNetTable(pIpNetTable,&dwSize,false);

	// allocate the buffer for the net table
	pIpNetTable = (PMIB_IPNETTABLE)malloc(dwSize);

	// now get the real table
	GetIpNetTable(pIpNetTable,&dwSize,false);

	DWORD numEntries = pIpNetTable->dwNumEntries;
	bool found = false;

	DWORD i = 0;

	*retMACAddr = 0;

	// now find the IP address
	while ((!found) && (i<numEntries))
	{
		found = (pIpNetTable->table[i].dwAddr == sockAddr->sin_addr.S_un.S_addr);
		i++;
	}

	if (found)
	{
		i--;
        BYTE macAddr[6];
		memcpy(macAddr,pIpNetTable->table[i].bPhysAddr,6);

		validMAC =  ((macAddr[0] == 0x00) &&
					 (macAddr[1] == 0x06) &&
					 (macAddr[2] == 0xD4));

		DWORD theMAC;
		theMAC  = macAddr[5] + (macAddr[4] << 8) + (macAddr[3]<<16);
        theMAC += (macAddr[2] << 24) + (macAddr[1] << 32) + (macAddr[0] << 40);
		*retMACAddr = theMAC;
	}

	free(pIpNetTable);

	return validMAC;
}





void StopHTTPServer()
{
	// signal the HTTP threads to die
	SetEvent(g_HTTPThreadSignal);

	Sleep(1000);

	CloseHandle(hQueryThreadSemaphore);

	CloseHandle(g_hListenThread);
	CloseHandle(g_hContentThread);
	CloseHandle(g_hQueryThread);

	CleanupThreadPools();

	CloseHandle(g_HTTPThreadSignal);

#ifdef _VERBOSE_DEBUG
	CloseDebugFile();
#endif

	// this should do the trick
}


BOOL StartHTTPServer()
{
	SOCKET		listenSocket = INVALID_SOCKET;
	SOCKADDR_IN listen_sin;

	SOCKET		contentSocket = INVALID_SOCKET;
	SOCKADDR_IN	content_sin;

	
	// create the event to stop the threads later (manual reset)
	g_HTTPThreadSignal = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!g_HTTPThreadSignal)
	{
		debugMessage("Could not create thread signal event\n\r");
		return FALSE;
	}

#ifdef _VERBOSE_DEBUG
	OpenDebugFile("debug.log");
#endif



	InitializeThreadPool();

	hQueryThreadSemaphore = CreateSemaphore(NULL,0,50,NULL);

	InitializeCriticalSection(&queryQueueAccess);

	g_hQueryThread = CreateThread(0, 0, 
                                    (LPTHREAD_START_ROUTINE) HTTPQueryThread,
                                    (LPVOID)0,
                                    0,
                                    &g_dwQueryThreadID);





	// we're just going to start the HTTP listen thread on the preset port

	
	// Create a TCP/IP socket to listen for incoming connect requests

	if ((listenSocket = socket (AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) 
	{
		debugMessage(" Failed to create socket \n\r");
		return FALSE;
	}

	// Fill out the local socket's address information.
	listen_sin.sin_family = AF_INET;
	listen_sin.sin_port = htons (HTTP_LISTEN_PORT);  
	listen_sin.sin_addr.s_addr = htonl (INADDR_ANY);

	// Associate the local address with listenSocket.

	if (bind (listenSocket,(struct sockaddr *) &listen_sin, sizeof (listen_sin)) == SOCKET_ERROR) 
	{
		debugMessage(" Failed to bind listen socket \n\r");
		closesocket (listenSocket);
		return FALSE;
	}

	// now create the thread that is going to accept connections on the listen socket

	debugMessage(" Creating listen thread to listen for HTTP requests \n\r");
	g_hListenThread = CreateThread(0, 0, 
                                    (LPTHREAD_START_ROUTINE) HTTPListenThread,
                                    (LPVOID)listenSocket,
                                    0,
                                    &g_dwListenThreadID);

	// now create the thread that is going to listen for content requests

	if ((contentSocket = socket (AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) 
	{
		debugMessage(" Failed to create socket \n\r");
		return FALSE;
	}

	// Fill out the local socket's address information.
	content_sin.sin_family = AF_INET;
	content_sin.sin_port = htons (HTTP_CONTENT_PORT);  
	content_sin.sin_addr.s_addr = htonl (INADDR_ANY);

	// Associate the local address with listenSocket.

	if (bind (contentSocket,(struct sockaddr *) &content_sin, sizeof (content_sin)) == SOCKET_ERROR) 
	{
		debugMessage(" Failed to bind content socket \n\r");
		closesocket (contentSocket);
		return FALSE;
	}

	debugMessage(" Creating content listen thread to listen for content requests \n\r");

	g_hContentThread = CreateThread(0, 0, 
                                    (LPTHREAD_START_ROUTINE) HTTPContentListenThread,
                                    (LPVOID)contentSocket,
                                    0,
                                    &g_dwListenThreadID);

	
    return TRUE;
}


DWORD WINAPI HTTPQueryThread(void *param)
{
	// this thread services all the HTTP GET requests on port 2900 - queries etc
	HANDLE waitObjects[2];

	waitObjects[0] = g_HTTPThreadSignal;
	waitObjects[1] = hQueryThreadSemaphore;

	while (1)
	{
		DWORD WaitCause = WaitForMultipleObjects(2,waitObjects,FALSE,INFINITE);
		// either being told to die, or service a query
		if (WaitCause == WAIT_OBJECT_0)  // exit this thread
			break;

		//something for us to service

		SOCKET qSocket;
		DWORD  qMAC;

		EnterCriticalSection(&queryQueueAccess);

		if (lQueryQueue.GetCount())
		{
			qSocket = (SOCKET)lQueryQueue.RemoveHead();
			qMAC    = lQueryMACs.RemoveHead();

			LeaveCriticalSection(&queryQueueAccess);

		}
		else
		{
			LeaveCriticalSection(&queryQueueAccess);
			continue;
		}

		int iStatus;
		char buffer[MAX_BUFFER_SIZE];
		memset(buffer,0,MAX_BUFFER_SIZE);

		iStatus = recv(qSocket,buffer,MAX_BUFFER_SIZE,0);
		// get the data from the socket
		if (iStatus == SOCKET_ERROR)
		{
#ifdef _VERBOSE_DEBUG
			LogOutput("!! Query: Error receiving from MAC %x using socket %d\r\n.  Shutting down socket.\r\n");
#endif
			debugMessage("ConnectThread: Socket Error. Exiting\n\r");
			shutdown (qSocket, 0x02);
			closesocket(qSocket);
		}
		else
		{
#ifdef _VERBOSE_DEBUG
			LogOutput("Query: HTTP request from MAC %x Socket %d:\r\n",qMAC,qSocket);
			LogOutput(buffer);
			LogOutput("\r\n");
#endif
			// ok, we now have a buffer that should contain the whole HTTP request
			// this is the place to handle the commands
			if (!HandleHTTPRequest(buffer,qSocket,qMAC))
			{
#ifdef _VERBOSE_DEBUG
				LogOutput("!! Query: Problem handling request on socket %d.  Sending error response.\r\n\r\n",qSocket);
#endif

				debugMessage("Problem handling the HTTP request!  Sending error response\n\r");
				sendErrorReply(qSocket);
			}
#ifdef _VERBOSE_DEBUG
			LogOutput("\r\nQuery: HTTP request from MAC %x finished.  Socket %d is being shutdown.\r\n\r\n", qMAC, qSocket);
#endif
			shutdown (qSocket, 0x02);
			closesocket(qSocket);
		}
	}

	return 1;
}



DWORD WINAPI HTTPListenThread(void *param)
{
	DWORD  WaitCause = WAIT_TIMEOUT;

	SOCKET ourSocket = (SOCKET)(param);

	SOCKET acceptSocket;

	SOCKADDR_IN accept_sin;
	DWORD accept_sin_len;

	DWORD qMAC = 0;

	int retCode;
    FD_SET readSet;
    struct timeval timeout;

	//HANDLE childThread;

	if (!param)
	{
		debugMessage(" Invalid socket passed to HTTP listen thread \n\r");
		return 0;
	}

	debugMessage(" HTTPListenThread is running \n\r");

	// thread loop starts here
	while (1)   
	{
		WaitCause = WaitForSingleObject(g_HTTPThreadSignal,0);
        if (WaitCause == WAIT_OBJECT_0)
           break;

		// listen for a connection

		if (listen (ourSocket, MAX_PENDING_CONNECTS) == SOCKET_ERROR) 
		{
			debugMessage(" Listening to client failed");
			closesocket (ourSocket);
			return FALSE;
		}

		// accept the connection

		accept_sin_len = sizeof (accept_sin);

		// Accept an incoming connection attempt on WinSocket.
		acceptSocket = accept (ourSocket, 
                     (struct sockaddr *) &accept_sin, 
                     (int *) &accept_sin_len);

		if (acceptSocket > 0)
		{
			FD_ZERO( &readSet );
			FD_SET( acceptSocket, &readSet );
			timeout.tv_sec = 5;
	
			timeout.tv_usec = 0;
			retCode = select(acceptSocket+1, &readSet, NULL, NULL, &timeout );
			if (retCode==-1)
			{
#ifdef _VERBOSE_DEBUG
				LogOutput("\r\n!! Query: Select from %s:%d failed - error reading.  Shutting down socket.\r\n", inet_ntoa (accept_sin.sin_addr),accept_sin.sin_port);
#endif
				debugMessage("Error reading from socket\n");
				shutdown(acceptSocket,2);
				closesocket(acceptSocket);
			}
			else if (retCode ==0)
			{
#ifdef _VERBOSE_DEBUG
				LogOutput("\r\n!! Query: Select from %s:%d failed - timeout.  Shutting down socket.\r\n", inet_ntoa (accept_sin.sin_addr),accept_sin.sin_port);
#endif
				debugMessage("Time out on socket.  Shutting down socket\n");
				shutdown(acceptSocket,2);
				debugMessage("Time out on socket.  Closing socket\n");
				closesocket(acceptSocket);
			}
	 if (ValidateMACAddress(&accept_sin,&qMAC))
		{

#ifdef _VERBOSE_DEBUG
			LogOutput("\r\nQuery: Queuing HTTP request from %s:%d MAC %x \r\n", inet_ntoa(accept_sin.sin_addr),accept_sin.sin_port,qMAC);
#endif

			// now that we are connected, queue the socket for service later by another thread

			EnterCriticalSection(&queryQueueAccess);

			theMediaDatabase.LogIPAddress(accept_sin.sin_addr.S_un.S_addr);

			lQueryQueue.AddTail((unsigned int)acceptSocket);
			lQueryMACs.AddTail((unsigned int)qMAC);

			LeaveCriticalSection(&queryQueueAccess);
			ReleaseSemaphore(hQueryThreadSemaphore,1,NULL);

		}
		else
		{
			shutdown (acceptSocket, 0x02);
			closesocket(acceptSocket);
		}

		// look and see if we are being told to die 
		WaitCause = WaitForSingleObject(g_HTTPThreadSignal,0);
        if (WaitCause == WAIT_OBJECT_0)
           break;
		}
	}

	debugMessage("HTTPListenThread is exiting \n\r");

	// Stop listening for connections from clients.
	closesocket (ourSocket);

	if (ourSocket == INVALID_SOCKET) 
	{
		debugMessage(" Could not close socket ");
		return FALSE;
	}
	return TRUE;
}



typedef enum
{
	HTTP_GET,
	HTTP_POST,
	HTTP_UNKNOWN
} HTTP_COMMAND_TYPE;

#define MAX_HTTP_COMMAND_TYPE_LENGTH 5


HTTP_COMMAND_TYPE HTTPCommandType(char *buffer)
{ 
	char *curr = buffer;
	int i=0, j=0;
	char commandType[MAX_HTTP_COMMAND_TYPE_LENGTH];

	// skip leading spaces
	while (curr[i] == ' ') i++;

	// copy the word until the next space
	while ((curr[i] != ' ') && (j < MAX_HTTP_COMMAND_TYPE_LENGTH))
		commandType[j++]=curr[i++];
	commandType[j]='\0';

	// see what it is and return the HTTP type

	if (strcmp("GET",commandType) == 0)
		return HTTP_GET;

	if (strcmp("POST",commandType)== 0)
		return HTTP_POST;

	return HTTP_UNKNOWN;
}


BOOL HandleHTTPRequest(char *buffer,SOCKET ourSocket,DWORD qMAC)
{
//	debugMessage("HandleHTTPRequest: Got a HTTP request that looks like this:\n\r");
//	debugMessage(buffer);

	// only do HTTP GET and POST now
	switch (HTTPCommandType(buffer))
	{
	case HTTP_GET:
		return HandleHTTP_GET(buffer,ourSocket,qMAC);
	case HTTP_POST:
		return HandleHTTP_POST(buffer,ourSocket,qMAC);
	default:
		return FALSE;  //buffer gets free'd in calling function
	}
}


char * GetQueryRequest(char * buffer)
{
	char *start,*end;

	// find the request tag
	start = strstr(buffer,"<Request>");
	if (!start) return NULL;

	// find the end of the tag and the start of the request part
	// we will definitely get the end tag if the strstr above succeeded
	while (*start++ != '>');

	// start now points to the start of the request string

	// now find the end of the request
	end = start;

	end = strstr(end,"</Request>");


	char * query = (char*)malloc(end-start+1);
	if (!query)
		return NULL;

	int i = 0;

	// copy the characters

	while (start<end)
		query[i++]=*start++;

	query[i] = '\0';

	return query;
}


BOOL HandleHTTP_POST(char *buffer, SOCKET ourSocket,DWORD qMAC)
{
 	char * curr;   //current pointer in the buffer
	unsigned int i=0, j=0;
	char *returnBuffer;
	char *responseBuffer;
	char responseHeader[MAX_HEADER_SIZE];

	debugMessage("HandleHTTP_POST: The request looks like this:\n\r");
	debugMessage(buffer);


	// skip straight to the SOAPACTION header and look what action is required
	curr = strstr(buffer,"SOAPACTION:");
	if (curr == NULL)
	{
		// didn't find it, bug out now
		return FALSE;
	}

	// this is crap but does the trick for now
	// this could be made nicer with a look up table and callback function...

	if (strstr(curr,"Query"))
		returnBuffer = (char*)iml_Query(GetQueryRequest(buffer),qMAC);
	else if (strstr(curr,"ReleaseView"))
		returnBuffer = iml_GetReleaseViewResponse(buffer,qMAC);
	else if (strstr(curr,"ViewItems"))
		returnBuffer = iml_ViewItems(buffer,qMAC);
	else
		return FALSE;

	if (!returnBuffer)
	{
		// couldn't get the data for some reason
		return FALSE;
	}

	// now we are ready to send our response

	// construct the header first
	ConstructHTTPResponsePacket(&responseHeader[0],strlen(returnBuffer),"text/xml");

	// next allocate the buffer for the whole response
	responseBuffer = (char *)malloc(strlen(returnBuffer)+strlen(responseHeader)+1);

	// put the header in first
	strcpy(responseBuffer,responseHeader);

	// then the file that we are serving 
	strcat(responseBuffer,returnBuffer);

	// we don't need the stuff returned by the file anymore, because it has been copied
	free(returnBuffer);

	debugMessage("About to send this:\n\r");
	debugMessage(responseBuffer);

	// send it back on the socket that the request came on
	if (send(ourSocket,responseBuffer,strlen(responseBuffer),0)==SOCKET_ERROR)
	{	
		debugMessage("socket error responding");
		if (responseBuffer)
				free(responseBuffer);
		return FALSE;
	}

	// free up the response buffer
	if (responseBuffer)
		free(responseBuffer);

	return TRUE;
}



BOOL HandleHTTP_GET(char *buffer, SOCKET ourSocket,DWORD qMAC)
{
	char * curr;   //current pointer in the buffer
	char fileName[MAX_NAME_SIZE];
	int i=0, j=0;
	char *returnBuffer;
	char *responseBuffer;
	char responseHeader[MAX_HEADER_SIZE];

	debugMessage("HandleHTTP_GET: The request looks like this:\n\r");
	debugMessage(buffer);

	// we know we have a GET
	// all we want to do here is extract the file name and spew the file back

	// start at the beginning of the buffer
	curr = buffer;

	// skip any leading spaces
	while (curr[i++]==' ');

	// now skip until there is a space
	while (curr[i++]!=' ');

    // now we just take the filename until there is a space

	j = 0;
	while (curr[i] != ' ')
		fileName[j++] = curr[i++];

	fileName[j]='\0';

	// check there was a filename
	if (strcmp("HTTP/1.1",fileName)==0)
		return FALSE;

	// now get the file that was asked for
	// returnBuffer gets allocated for us, we must free it
	returnBuffer = getLocalFile(fileName);
	if (!returnBuffer)
	{
		// file probably does not exist
		return FALSE;
	}

	// now we are ready to send our response

	// construct the header first
	if (isXML)
		ConstructHTTPResponsePacket(&responseHeader[0],strlen(returnBuffer),"text/xml");
	else
		ConstructHTTPResponsePacket(&responseHeader[0],strlen(returnBuffer),"text/html");



	// next allocate the buffer for the whole response
	responseBuffer = (char *)malloc(strlen(returnBuffer)+strlen(responseHeader)+1);

	// put the header in first
	strcpy(responseBuffer,responseHeader);

	// then the file that we are serving 
	strcat(responseBuffer,returnBuffer);

	// we don't need the stuff returned by the file anymore, because it has been copied
	free(returnBuffer);

	debugMessage("About to send this:\n\r");
	debugMessage(responseBuffer);

	// send it back on the socket that the request came on
	if (send(ourSocket,responseBuffer,strlen(responseBuffer),0)==SOCKET_ERROR)
	{	
		debugMessage("socket error responding");
		if (responseBuffer)
				free(responseBuffer);
		return FALSE;
	}

	// free up the response buffer
	if (responseBuffer)
		free(responseBuffer);

	return TRUE;

}


void sendErrorReply(SOCKET ourSocket)
{
	char Response[300];
	
	char Date[MAX_DATE_SIZE];
    char Server[MAX_URI_SIZE];
    
    GetDateTime(Date);
    GetServerString(Server, sizeof(Server));

    sprintf(&Response[0],
        "511 Not Implemented\r\n"
		"DATE: %s\r\n"
		"Connection: close\r\n"
        "SERVER: %s\r\n"
		"Content-Length: 0\r\n"
        "\r\n",
		Date,
		Server);

	if (send(ourSocket,Response,strlen(Response),0) ==SOCKET_ERROR)
	{
		debugMessage("SocketError \n\r");
	}
}


char * getLocalFile(char * filename)
{
	char *tempchar = NULL;
	// this is very lame at the moment

	if (strcmp(filename,"/desc.xml")==0)
	{

			char *baseURL;
			BuildBaseURL(&baseURL);

			CString temp;

			temp.Format(deviceDescriptionBase,theMediaDatabase.libraryGUID,theMediaDatabase.libraryName,baseURL);

			tempchar = (char*)malloc(temp.GetLength()+1);

			strcpy(tempchar,temp);
			isXML = TRUE;
			free(baseURL);
	}

	return tempchar;
}


/*
Example response packet header

HTTP/1.1 200 OK
Date: Mon, 14 May 2001 23:26:56 GMT
Connection: close
Server: Microsoft Windows NT/5.1 UPnP/1.0 UPnP Device Host/1.0
Content-Length: 755
Content-Type: text/xml
*/

void ConstructHTTPResponsePacket(char* pBytes,u_long ContentLength, char *ContentType) 
{
    CHAR Date[MAX_DATE_SIZE];
    CHAR Server[MAX_URI_SIZE];
    
    GetDateTime(Date);
    GetServerString(Server, sizeof(Server));

    sprintf(pBytes,
        "HTTP/1.1 200 OK\r\n"
		"DATE: %s\r\n"
		"Connection: close\r\n"
        "SERVER: %s\r\n"
		"Content-Length: %d\r\n"
		"Content-Type: %s\r\n"
        "\r\n",
		Date,
		Server,
		ContentLength,
		ContentType);
}

void ConstructHTTPContentResponsePacket(char* pBytes,u_long ContentLength, char *ContentType) 
{
    CHAR Date[MAX_DATE_SIZE];
    CHAR Server[MAX_URI_SIZE];
    
    GetDateTime(Date);
    GetServerString(Server, sizeof(Server));

    sprintf(pBytes,
        "HTTP/1.1 200 OK\r\n"
		"Content-Type: audio/%s\r\n"
		"Server: iML/1.0\r\n"
#ifdef SPEW_CHUNKS
		"Transfer-Encoding: chunked\r\n"
#endif
		"Content-Length: %d\r\n"
		"Content-Location: local\r\n"
        "\r\n",
//		Date,
//		Server,
		ContentType,
		ContentLength);
//		ContentType);
}




DWORD WINAPI HTTPContentListenThread(void *param)
{
	DWORD  WaitCause = WAIT_TIMEOUT;

	SOCKET ourSocket = (SOCKET)(param);

	SOCKET acceptSocket;

	SOCKADDR_IN accept_sin;
	DWORD accept_sin_len;

	int retCode;
    FD_SET readSet;
    struct timeval timeout;


	if (!param)
	{
		debugMessage(" Invalid socket passed to HTTP content listen thread \n\r");
		return 0;
	}

	debugMessage(" HTTP Content Listen Thread is running \n\r");

	// thread loop starts here
	while (1)   
	{
		WaitCause = WaitForSingleObject(g_HTTPThreadSignal,0);
        if (WaitCause == WAIT_OBJECT_0)
           break;

		// listen for a connection

		if (listen (ourSocket, MAX_PENDING_CONNECTS) == SOCKET_ERROR) 
		{
			debugMessage(" Listening to client failed");
			closesocket (ourSocket);
			return FALSE;
		}

		// accept the connection

		accept_sin_len = sizeof (accept_sin);

		// Accept an incoming connection attempt on our socket
		acceptSocket = accept (ourSocket, 
                     (struct sockaddr *) &accept_sin, 
                     (int *) &accept_sin_len);


		if (acceptSocket > 0)
		{
			FD_ZERO( &readSet );
			FD_SET( acceptSocket, &readSet );
			timeout.tv_sec = 5;
	
			timeout.tv_usec = 0;
			retCode = select(acceptSocket+1, &readSet, NULL, NULL, &timeout );
			if (retCode==-1)
			{
#ifdef _VERBOSE_DEBUG
				LogOutput("\r\n!! Content: Select from %s:%d failed - read error.  Shutting down socket.\r\n", inet_ntoa (accept_sin.sin_addr),accept_sin.sin_port);
#endif
				debugMessage("Error reading from socket\n");
				shutdown(acceptSocket,2);
				closesocket(acceptSocket);
			}
			else if (retCode ==0)
			{
#ifdef _VERBOSE_DEBUG
				LogOutput("\r\n!! Content: Select from %s:%d failed - time out.  Shutting down socket.\r\n", inet_ntoa (accept_sin.sin_addr),accept_sin.sin_port);
#endif

				debugMessage("Time out on socket.  Shutting down socket\n");
				shutdown(acceptSocket,2);
				debugMessage("Time out on socket.  Closing socket\n");
				closesocket(acceptSocket);
			}

			DWORD qMAC;

			if (ValidateMACAddress(&accept_sin,&qMAC))
			{

#ifdef _VERBOSE_DEBUG
				LogOutput("\r\nContent: Finding pool thread for %s:%d \r\n", inet_ntoa (accept_sin.sin_addr),accept_sin.sin_port);
#endif
				if (!RunContentThread(acceptSocket))
				{
#ifdef _VERBOSE_DEBUG
				LogOutput("\r\n!! Content: Failed to get pool thread for for %s:%d Shutting down socket\r\n", inet_ntoa (accept_sin.sin_addr),accept_sin.sin_port);
#endif

					debugMessage("RunContentThread failed!!\n\r");
					shutdown(acceptSocket,2);
					closesocket(acceptSocket);
				}
			}
			else
			{
				shutdown(acceptSocket,2);
				closesocket(acceptSocket);
			}

		}

		// look and see if we are being told to die 
		WaitCause = WaitForSingleObject(g_HTTPThreadSignal,0);
        if (WaitCause == WAIT_OBJECT_0)
           break;
	}

	debugMessage("HTTPListenThread is exiting \n\r");

	// Stop listening for connections from clients.
	closesocket (ourSocket);

	if (ourSocket == INVALID_SOCKET) 
	{
		debugMessage(" Could not close socket ");
		return FALSE;
	}
	return TRUE;
}



/*
DWORD WINAPI HTTPContentServerThread(void *param)
{
	DWORD  WaitCause = WAIT_TIMEOUT;
	SOCKET ourSocket = (SOCKET)(param);
	int iStatus;

	char *buffer = (char*)malloc(MAX_BUFFER_SIZE);

	debugMessage("Running HTTP Content Server Thread \n\r");
	
	iStatus = recv(ourSocket,buffer,MAX_BUFFER_SIZE,0);
	// get the data from the socket

	if (iStatus == SOCKET_ERROR)
	{
		debugMessage("Content Server Thread: SOCKET ERROR, exiting.\n\r");
		free(buffer);
		return 0;
	}

	// ok, we now have a buffer that should contain the whole HTTP request
	// don't forget to free the buffer later!
	// this is the place to handle the commands

	if (HTTPCommandType(buffer)==HTTP_GET)
	{
		
		HTTPSpewFile(buffer,ourSocket);
	}

	free(buffer);

	shutdown (ourSocket, 0x02);
	closesocket(ourSocket);

	debugMessage("Exiting HTTP Content Server Thread \n\r");
	return 1;
}
*/


BOOL HTTPSpewFile(char *buffer, SOCKET ourSocket)
{
	DWORD WaitCause;
	char * curr;   //current pointer in the buffer
	char mediaIdentifier[MAX_NAME_SIZE];
	int i=0, j=0;
	char *responseBuffer;
	DWORD bytesToRead;

	char *socketbuffer;
	char *filebuffer;

	char responseHeader[MAX_HEADER_SIZE];

	//debugMessage("HTTP_Spew File: The request looks like this:\n\r");
	//debugMessage(buffer);

	// we know we have a GET
	// all we want to do here is extract the file name and spew the file back

	// start at the beginning of the buffer
	curr = buffer;

	// skip any leading spaces
	while (curr[i++]==' ');

	// now skip until there is a space
	while (curr[i++]!=' ');

	while (curr[i++]!='=');

	// now we can get the identifier of the media we want to serve up
	    

	j = 0;
	while (curr[i] != ' ')
		mediaIdentifier[j++] = curr[i++];

	mediaIdentifier[j]='\0';

	DWORD mediaKey = atoi(mediaIdentifier);

	// check there was a media identifier
	if (strcmp("HTTP/1.1",mediaIdentifier)==0)
		return FALSE;

	// find out the file we need to open for this piece of media

//	fileIdentifier = getMedia(mediaIdentifier);


	//char fileIdentifier[50];
	char mimeType[50];
	CMediaElement *mediaElement = theMediaDatabase.mediaManager.GetMediaFromKey(mediaKey);

	if (!mediaElement || (!mediaElement->GetPathValidity()))
		return FALSE;

	CString fileIdentifier = mediaElement->GetFilePath();

	//strcpy(fileIdentifier,mediaElement->GetFilePath());
	strcpy(mimeType,mediaElement->GetMimeType());

	// now we need to open up the file and chunk encode it and send it over the socket

	DWORD        fileSize, fileSent;
    BOOL        fRet = FALSE;
  
   
    // Obtain a handle to the file.
    HANDLE hFile = CreateFile(fileIdentifier,
                                GENERIC_READ,
                                FILE_SHARE_READ,
                                NULL,
                                OPEN_EXISTING,
                                0,
                                NULL);

    // Failed to obtain a handle; log an error and exit the function.
    if (hFile == INVALID_HANDLE_VALUE)
    {
        debugMessage("HTTP Spew: CreateFile failed to open file");
        return FALSE;
    }
    
    // Obtain the size of the file; 
    fileSize = GetFileSize(hFile, NULL);

	// now we are ready to send our response

	// construct the header first
	
	ConstructHTTPContentResponsePacket(&responseHeader[0],fileSize,mimeType);


	// next allocate the buffer for the first response
	responseBuffer = (char *)malloc(strlen(responseHeader)+1);

	// put the header in first
	strcpy(responseBuffer,responseHeader);

	debugMessage("HTTP Spew file: response header is:\n\r");
	debugMessage(responseBuffer);

	// send it back on the socket that the request came on
	if (send(ourSocket,responseBuffer,strlen(responseBuffer),0)==SOCKET_ERROR)
	{	
		debugMessage("socket error responding");
		CloseHandle(hFile);
		if (responseBuffer)
				free(responseBuffer);
		return FALSE;
	}
	if (responseBuffer)	free(responseBuffer);

	fileSent=0;

	bytesToRead=CHUNK_SIZE;

	socketbuffer =(char*)malloc(CHUNK_SIZE+100);
	filebuffer = (char *)malloc(CHUNK_SIZE+4); 

	while (fileSent<fileSize)
	{
		// do this a chunk at a time
		//debugMessage("Getting chunk\n\r");
		bytesToRead=CHUNK_SIZE;
		ReadFile(hFile,filebuffer,bytesToRead,&bytesToRead,NULL);

		fileSent += bytesToRead;

		//debugMessage("appending CRLF\n\r");
		//append the CRLF on the chunk
		int bit = 0;

#ifdef SPEW_CHUNKS
		filebuffer[bytesToRead]='\r';
		filebuffer[bytesToRead+1]='\n';


		sprintf(socketbuffer,"%x\r\n",bytesToRead);
		bit = strlen(socketbuffer);

		bytesToRead+=2;
#endif

		memcpy((char*)(socketbuffer+bit),filebuffer,bytesToRead);

		// check we are not being told to die
		WaitCause = WaitForSingleObject(g_HTTPThreadSignal,0);
        if (WaitCause == WAIT_OBJECT_0)
           break;
		
		// send it on the socket now

		if (send(ourSocket,socketbuffer,bit+bytesToRead,0)==SOCKET_ERROR)
		{	
#ifdef _VERBOSE_DEBUG
			LogOutput("\r\n!! Socket error writing file.  File: %s  Position: %d \r\n",fileIdentifier,fileSent);
#endif
			
			CloseHandle(hFile);

			if (filebuffer) free(filebuffer);
			if (socketbuffer) free(socketbuffer);

			return FALSE;
		}

		// check we are not being told to die
		WaitCause = WaitForSingleObject(g_HTTPThreadSignal,0);
        if (WaitCause == WAIT_OBJECT_0)
			break;

	}

	// end off the chunk now - we are at the end of the file

#ifdef SPEW_CHUNKS
	char *endchunk = "0\r\n\r\n";

	if (send(ourSocket,endchunk,strlen(endchunk),0)==SOCKET_ERROR)
	{	
		debugMessage("socket error responding");
		CloseHandle(hFile);
		return FALSE;
	}
#endif
	// now do clean up
    CloseHandle(hFile);
	debugMessage("Exiting HTTP Spew file \n\r");
	if (filebuffer) free(filebuffer);
	if (socketbuffer) free(socketbuffer);
	return TRUE;
} 




DWORD WINAPI ContentPoolThread(void *param)
{
	int myIndex = (int)param;

	HANDLE waitObjects[2];

	SOCKET ourSocket;

	EnterCriticalSection(&csContentThreadPool);

	waitObjects[0] = g_HTTPThreadSignal;
	waitObjects[1] = ContentThreadPool[myIndex].flSignal;

	LeaveCriticalSection(&csContentThreadPool);

	while (1)
	{
		ContentThreadPool[myIndex].dwStatus = TPOOL_READY;

		DWORD WaitCause = WaitForMultipleObjects(2,waitObjects,FALSE,INFINITE);

		// either being told to die, or start streaming file
		if (WaitCause == WAIT_OBJECT_0)
			break;
		EnterCriticalSection(&csContentThreadPool);
		ResetEvent(ContentThreadPool[myIndex].flSignal);
		ourSocket = ContentThreadPool[myIndex].sSocket;
		LeaveCriticalSection(&csContentThreadPool);
		int iStatus;
		char buffer[MAX_BUFFER_SIZE];
		memset(buffer,0,MAX_BUFFER_SIZE);
		iStatus = recv(ourSocket,buffer,MAX_BUFFER_SIZE,0);
		// get the data from the socket


		if (iStatus != SOCKET_ERROR)
		{
#ifdef _VERBOSE_DEBUG
		LogOutput("\r\nThreadPool [%d]: Request looks like this: \r\n", myIndex);
		LogOutput(buffer);
		LogOutput("\r\n");
#endif
			if (HTTPCommandType(buffer)==HTTP_GET)
			{
				HTTPSpewFile(buffer,ourSocket);
				
			}
		}
		else
		{
#ifdef _VERBOSE_DEBUG
		LogOutput("\r\n!! ThreadPool [%d]: Socket Error on socket: \r\n", myIndex,ourSocket);
		LogOutput(buffer);
		LogOutput("\r\n");
#endif
		}

#ifdef _VERBOSE_DEBUG
		LogOutput("\r\nThreadPool [%d]: Shutting down socket %d \r\n", myIndex, ourSocket);
#endif
		shutdown(ourSocket,0x02);
		closesocket(ourSocket);
	}
	return 1;
}





















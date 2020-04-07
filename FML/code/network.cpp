//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1997-2001.
//
//  File:       N E T W O R K. C P P
//
//  Contents:   Provides the socket interface.
//
//----------------------------------------------------------------------------

#include "stdafx.h"
//#include <stdio.h>
#include "network.h"
#include "global.h"

extern BOOL g_bDebug;

//////////
// WinsockInit
//      Initialize Winsock.  Negotiates the version number used.
//      Version must be between 1.1 and 2.0.
//
// PARAMETERS
//      None.  
// 
// RETURNS: 
//      BOOL    TRUE if Winsock initialized.
//              FALSE if the initialization failed.
//
BOOL WinsockInit() 
{
    
    WSADATA wsadata;
    WORD wVersionRequested = MAKEWORD(WSA_MAX_MAJOR, WSA_MIN_MINOR);
    int iRet;
    
    // WinSock version negotiation. Use 2.0 if available. 1.1 is the minimum.
    iRet = WSAStartup(wVersionRequested,&wsadata);
    
    // If negotiation did not succeed, process the error.
    if (iRet != 0)
    {
        if (iRet == WSAVERNOTSUPPORTED)
        {
            printf("SocketInit, WSAStartup failed with error %d. DLL "             
                    "supports version higher than 2.0, but not also 2.0.\n",
                    GetLastError());
            return FALSE;
        }
        else
        {
            printf("SocketInit, WSAStartup failed with error %d.\n",
                    GetLastError());      
            return FALSE;
        }
    }
    
    // Version negotiation succeeded; examine the results.
    if (wVersionRequested == wsadata.wVersion)
    {
        puts("WinsockInit, succeeded.");
    }
    else if ((LOWORD(wsadata.wHighVersion)) >= WSA_MIN_MAJOR)
    {
        // Version number incorrect.  Log why it is incorrect.
        // Supported version must be at least 1.*, and must be < 2.0
        if ((HIWORD(wsadata.wHighVersion)) < WSA_MIN_MINOR)
        {
            puts("SocketInit: Minor version supported is below our min. requirement.");        
            
            int result = WSACleanup();
            if (0 == result)
            {
                SetLastError(WSAVERNOTSUPPORTED);
            }
            return FALSE;
        }
    }
    else
    {
        puts("SocketInit: Major version supported is below our min. requirement.\n");        
        
        int result = WSACleanup();
        if (0 == result)
        {
            SetLastError(WSAVERNOTSUPPORTED);
        }
        return FALSE;
    }
    return TRUE;
} // end WinsockInit


//////////
// GetNetwork
//      Bind to a socket. 
//
// PARAMETERS
//      pSocketAddressFrom:  Pointer to memory where the socket should be returned.
// 
// RETURNS: 
//      INT                  0 if the function could bind to the socket.
//                           -1 if the function could not bind to the socket.
//
INT GetNetwork (SOCKADDR_IN* pSocketAddressFrom) 
{
    char hostname[MAX_HOST_NAME];
    struct hostent* phe;
    
    // Get the host name, perform various checks to ensure it 
    // is valid.
    if (0 != gethostname(hostname, sizeof(hostname)))
    {
        return -1;
    }
    phe = gethostbyname(hostname);
    
    if (phe == NULL)
    {
        return -1;
    }
    if (phe->h_addrtype != AF_INET || phe->h_length != 4)
    {
        return -1;
    }
    
    // Bind the socket.
    int i = 0;
    SOCKADDR_IN sockadr;
    
    sockadr.sin_family = AF_INET;
    sockadr.sin_port = htons(SSDP_PORT);
    memcpy(&sockadr.sin_addr, phe->h_addr_list[i], sizeof(sockadr.sin_addr));
    if(g_bDebug) 
    {
        printf("GetNetwork, will bind socket to %s.\n", 
                INET_NTOA(sockadr.sin_addr.s_addr));
    }
    
    *pSocketAddressFrom = sockadr;
    return 0;
} // end GetNetwork



BOOL GetIPAddress(char **address)
{
    char hostname[MAX_HOST_NAME];
    struct hostent* phe;
    
    // Get the host name, perform various checks to ensure it 
    // is valid.
    if (0 != gethostname(hostname, sizeof(hostname)))
    {
		int lastError = WSAGetLastError();
        return FALSE;
    }
    phe = gethostbyname(hostname);
	       
    if (phe == NULL)
    {
        return FALSE;
    }
    if (phe->h_addrtype != AF_INET || phe->h_length != 4)
    {
        return FALSE;
    }

	SOCKADDR_IN sockadr;
    
	// get out local IP address
    memcpy(&sockadr.sin_addr, phe->h_addr_list[0], sizeof(sockadr.sin_addr));

	*address = inet_ntoa(sockadr.sin_addr);

	return TRUE;
}




//////////
// SocketClose
//      Closes the specified socket
//
// PARAMETERS
//      socketToClose:  The socket to close.
// 
// RETURNS: 
//      BOOL            TRUE if successful.
//                      FALSE if not successful.
//
BOOL SocketClose(SOCKET socketToClose)
{
    closesocket(socketToClose);
    return TRUE;
} // end SocketClose


//////////
// SocketFinish
//      Terminates the socket interface.
//
// PARAMETERS
//      None.
// 
// RETURNS: 
//      Nothing.
//
VOID SocketFinish()
{
    WSACleanup();
} // end SocketFinish


//////////
// OpenMulticastSocket
//      Creates and opens a socket on the specified multicast
//      channel.  Once this function has been called, a socket can
//      then be created on a given interface.
//
//      Preconditions:  A call to SocketInit must have been 
//      made successfully.
//
// PARAMETERS
//      pSocketToOpen:  Pointer to the socket to open.
//      IpAddress:      The multicast IP address to listen on.
// 
// RETURNS: 
//      BOOL            TRUE if successful.
//                      FALSE if not successful.
//
BOOL OpenMulticastSocket( 
                         SOCKET* pSocketToOpen, 
                         PSOCKADDR_IN IpAddress)
{
    struct ip_mreq mreq;
    INT iRet;
    INT iTTL = SSDP_TTL;
    SOCKET socketSSDP;
    
    // Create a socket to listen on the multicast channel.
    socketSSDP = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketSSDP == INVALID_SOCKET)
    {      
        puts("OpenMulticastSocket, failed to create socket.");
        return FALSE;
    }

	int one = 1;
	setsockopt(   
		socketSSDP, 
		SOL_SOCKET, 
		SO_REUSEADDR,	
		(char *)&one,
		sizeof(one));

	
    
    // Bind to the socket.
    iRet = bind(socketSSDP, (struct sockaddr*)IpAddress, sizeof(*IpAddress));
    
    // Cleanup if an error occurred.
    if (iRet == SOCKET_ERROR)
    {
		debugMessage("OpenMulticastSocket: Failed to bind to socket");

        printf("OpenMulticastSocket, bind using %s failed with error (%d)\n.",
                INET_NTOA(IpAddress->sin_addr), GetLastError());

        SocketClose(socketSSDP);
        *pSocketToOpen = INVALID_SOCKET;
        return FALSE;
    }
    
    // Join the multicast group.
    mreq.imr_multiaddr.s_addr = inet_addr(SSDP_ADDR);
    mreq.imr_interface.s_addr = IpAddress->sin_addr.s_addr; 
    iRet = setsockopt(socketSSDP, IPPROTO_IP, IP_ADD_MEMBERSHIP, (CHAR*)&mreq, sizeof(mreq));
    
    // Cleanup if an error occurred.
    if (iRet == SOCKET_ERROR)
    {
        printf("OpenMulticastSocket, join multicast group failed with error %d.\n",
                    GetLastError());     
        SocketClose(socketSSDP);
        *pSocketToOpen = INVALID_SOCKET;
        return FALSE;
    }
    
    // Set time-to-live (TTL) time for the multicast packets.
    iRet = setsockopt(
                    socketSSDP, 
                    IPPROTO_IP, 
                    IP_MULTICAST_IF, 
                    (CHAR*)&IpAddress->sin_addr.s_addr, 
                    sizeof(IpAddress->sin_addr.s_addr)
                    );
#ifndef UNDER_CE 
    if (iRet == SOCKET_ERROR)
    {
        puts("OpenMulticastSocket, failed to set IP_MULTICAST_IF option.");     
        SocketClose(socketSSDP);
        *pSocketToOpen = INVALID_SOCKET;
        return FALSE;
    }
#endif
    iRet = setsockopt(socketSSDP, IPPROTO_IP, IP_MULTICAST_TTL, (CHAR*)&iTTL, sizeof(iTTL));
    
    // Cleanup if error occurred.
    if (iRet == SOCKET_ERROR)
    {  
        puts("OpenMulticastSocket, failed to set IP_MULTICAST_TTL option.");        
        SocketClose(socketSSDP);
        *pSocketToOpen = INVALID_SOCKET;
        return FALSE;
    }
    *pSocketToOpen = socketSSDP;
    return TRUE;
} // end OpenMulticastSocket


//////////
// SocketSend
//      Sends data over the specified socket, to the specified 
//      remove location. 
//
// PARAMETERS
//      szBytes:         The number of bytes to be sent.
//      socket:          The socket to send the data over. 
//      pRemoteAddress:  The remote location to send the data to.
// 
// RETURNS: 
//      BOOL            TRUE if the message was sent.
//                      FALSE if the message was not sent.
//
BOOL SocketSend (
                 const CHAR* szBytes, 
                 SOCKET socket, 
                 SOCKADDR_IN* pRemoteAddress)
{
    INT iBytesToSend, iBytesSent;
    iBytesToSend = strlen(szBytes);
    
    // Send the data.
    iBytesSent = sendto ( 
        socket, 
        szBytes, 
        iBytesToSend, 0,
        (struct sockaddr*) pRemoteAddress, 
        sizeof(SOCKADDR_IN));
    
    // Cleanup if an error occurred.
    if (iBytesSent == SOCKET_ERROR)
    {
        puts("SocketSend, failed to send data to remote address.");
        return FALSE;
    }
    else if (iBytesSent != iBytesToSend)
    {
        printf("SocketSend, only sent %d bytes instead of %d bytes.\n",
                iBytesSent, iBytesToSend);
        return FALSE;
    }
    return TRUE;
} // end SocketSend


//////////
// SocketReceive
//      Read data from the specified socket. 
//
// PARAMETERS
//      socket:     Socket to read from.
//      pszData:    Out output, pointer where receive buffer pointer is written to
//                  This memory must be released by the caller using free.
//      fromSocket: On output, pointer that receives the source address.
// 
// RETURNS: 
//      BOOL        TRUE if successful.
//                  FALSE if not successful.
//
BOOL SocketReceive(
                   SOCKET socket, 
                   CHAR** pszData, 
                   SOCKADDR_IN* fromSocket)
{
    u_long BufferSize;
    u_long BytesReceived;
    CHAR* ReceiveBuffer;
    SOCKADDR_IN RemoteSocket;
    INT SocketSize = sizeof(RemoteSocket);
    
    // Ensure socket can be read from.
    if (ioctlsocket(socket, FIONREAD, &BufferSize) != 0)
    {
        printf("SocketReceive, ioctlsocket(%x, FIONREAD) failed.\n", socket);
        return FALSE;
    }


    
    // Create the buffer to receive the data.
    ReceiveBuffer = (CHAR*) malloc(sizeof(CHAR) * (BufferSize + 1));
    if (ReceiveBuffer == NULL)
    {
        printf("SocketReceive, failed to allocate Buffer of (%d) bytes.\n",
                 BufferSize + 1);
        return FALSE;
    }
    
    // Receive the data.
    BytesReceived = recvfrom(
                        socket, 
                        ReceiveBuffer, 
                        BufferSize, 0,
                        (struct sockaddr*) &RemoteSocket, 
                        &SocketSize);
    
    // If an error occurred, return FALSE.
    if (BytesReceived == SOCKET_ERROR)
    {
        free(ReceiveBuffer);
        return FALSE;
    }
    else
    {
        // Otherwise set the output pointers.
        ReceiveBuffer[BytesReceived] = '\0';
        *pszData = ReceiveBuffer;
        *fromSocket = RemoteSocket;
        return TRUE;
    }
} // end SocketReceive


//////////
// ReadSelect
//      Determine the status of the read socket.  This function 
///     is used to discover if there is data available to be read.
//      Time is specified for the timeout on the socket select call
//
// PARAMETERS
//      sock:           Socket to check status for.
//      dwMillisecs:    Timeout value.
// 
// RETURNS: 
//      int             A "select" error code.  
//                      See MSDN documentation for select.
//
int ReadSelect(SOCKET sock, DWORD dwMillisecs)
{
    fd_set set;
    struct timeval t;
    struct timeval* pt;
    
    // Set up the timeout.
    if (dwMillisecs == INFINITE)
    {
        pt = NULL;
    }
    else
    {
        t.tv_sec = (dwMillisecs / 1000);
        t.tv_usec = (dwMillisecs % 1000)*1000;
        pt = &t;
    }
    
    // Check the socket.
    FD_ZERO(&set);
    FD_SET(sock, &set);
    
    // Check the socket and return the value.
    return select(0, &set, NULL, NULL, pt);
} // end ReadSelect


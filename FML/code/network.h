//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1997-2001.
//
//  File:       N E T W O R K. H
//
//  Contents:   Provides the socket interface.
//
//----------------------------------------------------------------------------

// Include all the necessary header files.
#ifndef _NETWORK_H
#define _NETWORK_H

#include "stdafx.h"
//#include <windows.h>
#include <winsock.h>

#define WSA_MIN_MINOR 0x01
#define WSA_MIN_MAJOR 0x01


#define WSA_MAX_MINOR 0x00
#ifdef UNDER_CE
#define WSA_MAX_MAJOR 0x01
#else
#define WSA_MAX_MAJOR 0x02
#endif

#define RECEIVE_BUFFER_SIZE 256                // Maxiumum size of data that can be received.
#define SSDP_ADDR "239.255.255.250"
#define SSDP_PORT 1900
#define SSDP_TTL 4

#define MAX_HOST_NAME 32                      // Maximum host name of local SSDP socket address
#define INET_NTOA(a) inet_ntoa(*(struct in_addr*)&(a))

BOOL WinsockInit();

BOOL OpenMulticastSocket( 
                         SOCKET* pSocketToOpen, 
                         PSOCKADDR_IN IpAddress);

BOOL SocketClose(SOCKET socketToClose);

INT GetNetwork( 
               SOCKADDR_IN* pSocketAddressFrom);

BOOL SocketSend( 
                const CHAR* szBytes, 
                SOCKET socket, 
                SOCKADDR_IN* pRemoteAddress);

BOOL SocketReceive(
                   SOCKET socket, 
                   CHAR** pszData, 
                   SOCKADDR_IN* fromSocket);

int ReadSelect(
               SOCKET sock, 
               DWORD dwMillisecs);

VOID SocketFinish();

int GetIPAddress(char **address);


// Multi-network support information.

struct SSDPNetwork 
{
   SSDPNetwork* FLink;
   SOCKADDR_IN IpAddress;
   SOCKET socket;
};

INT GetNetworks();
BOOL InitializeListNetwork();
VOID CleanupListNetwork();
VOID SendOnAllNetworks(CHAR *szBytes, SOCKADDR_IN *RemoteAddress, BOOL bDbgPrint);
int SelectAny(SOCKET* psock, DWORD dwMillisecs);
int ReplyOnNetwork(const CHAR* szBytes, 
               SOCKET socket, 
               SOCKADDR_IN* pRemoteAddress,
               BOOL bDbgPrint);


#endif // _NETWORK_H




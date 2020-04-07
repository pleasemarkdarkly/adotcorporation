//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1997-2001.
//
//  File:       M N E T W O R K. C P P
//
//  Contents:   Provides support for multiple network adapters.
//
//----------------------------------------------------------------------------

#include "stdafx.h"
//#include <stdio.h>
#include "network.h"
#include "global.h"


// A list of distinct networks hosted on this machine.
static SSDPNetwork* listNetwork = NULL;
static CRITICAL_SECTION CSListNetwork;
static BOOL g_bListNetwork = FALSE;


// Function prototypes.
SSDPNetwork* AddToListNetwork(PSOCKADDR_IN IpAddress);
VOID FreeSSDPNetwork(SSDPNetwork* pSSDPNetwork);
VOID RemoveFromListNetwork(SSDPNetwork* pssdpNetwork);
BOOL ReplaceTokenInLocation(LPCSTR szIn, LPSTR szReplace, LPSTR* pszOut);




//////////
// GetNetworks
//      Finds all the network interfaces available, and adds them 
//      to ListNetwork.  Tests if the interface can be used for 
//      multicast--rejected if the interface cannot.
//
// PARAMETERS
//      None.
// 
// RETURNS: 
//      INT     0 if the function found some interfaces.
//              -1 if the function could not find an interface.
//
INT GetNetworks()
{
    char hostname[MAX_HOST_NAME];
    struct hostent* phe;
    int i;
    
    // Get the host name, perform various checks to ensure it 
    // is valid.
    if (0 != gethostname(hostname, sizeof(hostname)))
    {
		int lastError = WSAGetLastError();
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
    
    // phe now contains a list of all the addresses/networks 
    // that are owned by a device.  Test them.
    for (i = 0; phe->h_addr_list[i]; i++)
    {
        SSDPNetwork* pSsdpNetwork;
        SOCKADDR_IN sockadr;
    
        sockadr.sin_family = AF_INET;
        sockadr.sin_port = htons(SSDP_PORT);
        memcpy(&sockadr.sin_addr, phe->h_addr_list[i], sizeof(sockadr.sin_addr));
        
        pSsdpNetwork = AddToListNetwork(&sockadr);
        printf("Adding Network %i with address %s\n", i, INET_NTOA(sockadr.sin_addr));
        if (pSsdpNetwork != NULL)
        {
            if (OpenMulticastSocket(&pSsdpNetwork->socket,
                           &sockadr) == FALSE)
            {
                // Open fails, remove from the list.
                printf("Removing Network %d\n", i);
                RemoveFromListNetwork(pSsdpNetwork);
                FreeSSDPNetwork(pSsdpNetwork);
            }
        }
    }

    if (listNetwork == NULL)
    {
        return -1;
    }

    return 0;
} // end GetNetworks


//////////
// SendOnAllNetworks
//      For each network interface in listNetworks, send the data to the 
//      remote address. Optionally display information about the sent data.
//      If the data includes a sending IP address (Location header),
//      the caller should include the identifier c_szReplaceGuid where
//      the IP address would normally go. This function will replace the 
//      identifier with the IP address associated with each interface 
//      before sending.
//
// PARAMETERS
//      szBytes         Pointer to data to be sent.
//      RemoteAddress   IP address of remote host.
//      bDbgPrint       Flag indicating whether to display contents of data.
// 
// RETURNS: 
//      Nothing.
//
VOID SendOnAllNetworks(CHAR *szBytes, SOCKADDR_IN *RemoteAddress, BOOL bDbgPrint)
{
    SSDPNetwork* pEntry;
    SOCKET          socketName;

    socketName = socket(AF_INET, SOCK_STREAM, 0);

    if (socketName == INVALID_SOCKET)
    {
        printf("Failed to create socket. Error %d.", GetLastError());
        return;
    }

    EnterCriticalSection(&CSListNetwork);

    for (pEntry = listNetwork; pEntry != NULL; pEntry = pEntry->FLink)
    {
        ULONG           nAddr;
        ULONG           nAddrRoute;
        CHAR *          szBytesNew;

        nAddr = pEntry->IpAddress.sin_addr.s_addr;
        nAddrRoute = pEntry->IpAddress.sin_addr.S_un.S_addr;

        // Replace c_szReplaceGuid with the source IP address.
        if (ReplaceTokenInLocation(szBytes, INET_NTOA(nAddrRoute),
                                    &szBytesNew))
        {
            if (szBytesNew)
            {
                if(bDbgPrint)
                {
                    puts(szBytesNew);
                }
                // Send the data.
                SocketSend(szBytesNew, pEntry->socket, RemoteAddress);
                free(szBytesNew);
            }
            else
            {
                if(bDbgPrint)
                {
                    puts(szBytes);
                }
                SocketSend(szBytes, pEntry->socket, RemoteAddress);
            }
        }
    }

    LeaveCriticalSection(&CSListNetwork);

    closesocket(socketName);
} // end SendOnAllNetworks


//////////
// ReplyOnNetwork
//      Send reply data using same interface as the request. 
//      If the data includes a sending IP address (Location header),
//      the caller should include the identifier c_szReplaceGuid where
//      the IP address would go. This function will replace the identifier
//      with the IP address associated with each interface before sending.
//
// PARAMETERS
//      szBytes         Pointer to data to be sent.
//      socket          Socket to use for sending.
//      RemoteAddress   IP address of remote host.
//      bDbgPrint       Flag indicating whether to display contents of data.
// 
// RETURNS: 
//      INT             0 if successful
//                      Otherwise, returns a socket error code.
//
int ReplyOnNetwork(const CHAR* szBytes, 
               SOCKET socket, 
               SOCKADDR_IN* pRemoteAddress,
               BOOL bDbgPrint)
{
    SOCKADDR_IN locaddr;
    int cbName = sizeof(locaddr);
    ULONG           nAddr;
    ULONG           nAddrRoute;
    CHAR *          szBytesNew;

    int rc = getsockname(socket, (SOCKADDR*)&locaddr, &cbName);

    nAddr = locaddr.sin_addr.s_addr;
    nAddrRoute = locaddr.sin_addr.S_un.S_addr;

    // Replace c_szReplaceGuid with the source IP address.
    if (ReplaceTokenInLocation(szBytes, INET_NTOA(nAddrRoute),
                                &szBytesNew))
    {
        if (szBytesNew)
        {
            if(bDbgPrint)
            {
                puts(szBytesNew);
            }
            SocketSend(szBytesNew, socket, pRemoteAddress);
            free(szBytesNew);
        }
        else
        {
            if(bDbgPrint)
            {
                puts(szBytes);
            }
            SocketSend(szBytes, socket, pRemoteAddress);
        }
    }

    return rc;

} // end ReplyOnNetwork


//////////
// SelectAny
//      Determine the status of all the networks--are they read-ready.
//      This is used to discover if there is data available to be read.
//      Time is specified for timeout on the socket select call.
//
// PARAMETERS
//      psock:        Returns the socket that has something to read.
//      dwMillisecs:  Timeout value.
// 
// RETURNS: 
//      int           A "select" error code.  See MSDN documentation for select.
//
int SelectAny(SOCKET* psock, DWORD dwMillisecs)
{
    fd_set set;
    struct timeval t;
    struct timeval* pt;
    int rcSelect;
    SSDPNetwork* pEntry;
    
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

    EnterCriticalSection(&CSListNetwork);

    for (pEntry = listNetwork; pEntry != NULL; pEntry = pEntry->FLink)
    {
        FD_SET(pEntry->socket, &set);
    }

    LeaveCriticalSection(&CSListNetwork);

    
    // Check the socket and return the value.
    rcSelect = select(0, &set, NULL, NULL, pt);

    if (rcSelect > 0 && rcSelect != SOCKET_ERROR)
    {
        // Something is ready.  Find a socket.
        EnterCriticalSection(&CSListNetwork);

        rcSelect = 0;
        for (pEntry = listNetwork; pEntry != NULL; pEntry = pEntry->FLink)
        {
            if (FD_ISSET(pEntry->socket, &set))
            {
                *psock = pEntry->socket;
                rcSelect = 1;
                break;
            }
        }

        LeaveCriticalSection(&CSListNetwork);

    }

    return rcSelect;
} // end SelectAny



//////////
// InitializeListNetwork
//      Initialize the list structure used for holding network interfaces.
//
// PARAMETERS
//      None.
// 
// RETURNS: 
//      BOOL    TRUE if successful.
//              FALSE if fails
//
BOOL InitializeListNetwork()
{
    __try
    {
    InitializeCriticalSection(&CSListNetwork);
    EnterCriticalSection(&CSListNetwork);
    listNetwork = NULL;
    LeaveCriticalSection(&CSListNetwork);
    }
    __except(1)
    {
        return FALSE;
    }
    g_bListNetwork = TRUE;
    return TRUE;
} // end InitializeListNetwork


//////////
// CleanupListNetwork
//      For each network interface, close the interface and remove the interface
//      from the list.
//
// PARAMETERS
//      None.
// 
// RETURNS: 
//      Nothing.
//
VOID CleanupListNetwork()
{
    SSDPNetwork* pEntry;
    SSDPNetwork* pNext;

    if (g_bListNetwork == FALSE)
        return;

    EnterCriticalSection(&CSListNetwork);
    for (pEntry = listNetwork; pEntry != NULL;)
    {
        pNext = pEntry->FLink;

        SocketClose(pEntry->socket);

        FreeSSDPNetwork(pEntry);

        pEntry = pNext;
    }

    listNetwork = NULL;

    LeaveCriticalSection(&CSListNetwork);

    DeleteCriticalSection(&CSListNetwork);
    g_bListNetwork = FALSE;
} // end CleanupListNetwork


//////////
// AddToListNetwork
//      Add a network interface item SSDPNetwork to the listNetworks.
//
// PARAMETERS
//      IpAddress       Local IP Address for the network interface
// 
// RETURNS: 
//      SSDPNetwork*    Pointer to allocated memory for the interface.
//
SSDPNetwork* AddToListNetwork(PSOCKADDR_IN IpAddress)
{
    SSDPNetwork* pssdpNetwork;

    // Create SSDPNetwork from SSDPMessage.

    pssdpNetwork = (SSDPNetwork*) malloc (sizeof(SSDPNetwork));

    if (pssdpNetwork == NULL)
    {
        return NULL;
    }

    pssdpNetwork->IpAddress = *IpAddress;

    // Add to head of list.
    EnterCriticalSection(&CSListNetwork);

    pssdpNetwork->FLink = listNetwork;
    listNetwork = pssdpNetwork;

    LeaveCriticalSection(&CSListNetwork);

    return pssdpNetwork;
} // end AddToListNetwork


//////////
// FreeSSDPNetwork
//      Free the SSDPNetwork memory allocated by AddToListNetwork.
//
// PARAMETERS
//      pSSDPNetwork    Pointer to a SSDPNetwork item
// 
// RETURNS: 
//      Nothing.
//
VOID FreeSSDPNetwork(SSDPNetwork* pSSDPNetwork)
{
    free(pSSDPNetwork);
} // end FreeSSDPNetwork


//////////
// RemoveFromListNetwork
//      Remove the specified SSDPNetwork item from listNetwork.
//
// PARAMETERS
//      pSSDPNetwork    Pointer to a SSDPNetwork item.
// 
// RETURNS: 
//      Nothing.
//
VOID RemoveFromListNetwork(SSDPNetwork* pssdpNetwork)
{
    SSDPNetwork* pEntry;
    SSDPNetwork* pPrev;

    EnterCriticalSection(&CSListNetwork);

    pEntry = listNetwork;
    pPrev = NULL;
    while (pEntry != NULL && pEntry != pssdpNetwork)
    {
        pPrev = pEntry;
        pEntry = pPrev->FLink;
    }
    if (pEntry != NULL)
    {
        if (pPrev == NULL)
        {
            listNetwork = pEntry->FLink;
        }
        else
        {
            pPrev->FLink = pEntry->FLink;
        }
    }

    LeaveCriticalSection(&CSListNetwork);
} // end RemoveFromListNetwork


//////////
// ReplaceTokenInLocation
//      Search the character buffer for the special identifier c_szReplaceGuid.
//      If it is found, then allocate memory for a copy of the buffer, and replace
//      c_szReplaceGuid with the string szReplace.
//
//      Note that this only replaces the first instance of c_szReplaceGuid.
//
// PARAMETERS
//      szIn        Pointer to input buffer.
//      szReplace   String to replace the c_szReplaceGuid with (usually an IP address).
//      pszOut      New buffer that has the new string.
// 
// RETURNS: 
//      BOOL        TRUE if a replacement was done, or if none was needed.
//                  If pszOut is NULL, a new buffer was returned.
//                  FALSE if replacement needed, but memory allocation failed.
//
BOOL ReplaceTokenInLocation(LPCSTR szIn, LPSTR szReplace, LPSTR *pszOut)
{
    CHAR *  pch;
    int nToken = strlen(c_szReplaceGuid);

    *pszOut = NULL;

    // Check for special identifier.
    if (pch = strstr(szIn, c_szReplaceGuid))
    {
        LPSTR   szOut;
        DWORD   cbOut;

        // Allocate space for the modified string.
        cbOut = (strlen(szIn) - nToken + strlen(szReplace) + 1) * sizeof(CHAR);

        szOut = (LPSTR)malloc(cbOut);
        if (szOut)
        {
            int len1 = pch - szIn;

            // Copy the first part of URL.
            strncpy(szOut, szIn, len1);
            // Copy the actual IP address to replace c_szReplaceGuid.
            strcpy(szOut + len1, szReplace);

            // Copy the remaining part of the URL (after c_szReplaceGuid).
            pch += nToken;
            strcpy(szOut + len1 + strlen(szReplace), pch);

            *pszOut = szOut;
        }
        else
        {
            return FALSE;
        }

    }

    return TRUE;
} // end ReplaceTokenInLocation


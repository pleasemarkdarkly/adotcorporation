//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1997-2001.
//
//  File:       S S D P. C P P
//
//  Contents:   Responsible for keeping a device alive, and listening and
//              responding to M-Searches.
//
//----------------------------------------------------------------------------

#include "stdafx.h"
//#include <windows.h>
//#include <stdlib.h>
//#include <stdio.h>

#include "global.h"
#include "ssdp.h"
#include "network.h"
#include "parser.h"
#include "util.h"


extern BOOL g_bDebug;

// All the GLOBALS defined below are shared only by
// the StartDiscovery function and StopDiscovery Function.
static BOOL g_bStartup = FALSE;

// Global address to where messages are sent; set by StartDiscovery.
SOCKADDR_IN g_SocketAddressTo;

// Global socket; set by StartDiscovery.
SOCKET g_Socket;

// Handles to the beacon and read threads.
HANDLE g_hBeaconThread = NULL;
HANDLE g_hSSDPReadThread  = NULL;

// Thread IDs for the beacon and read thread.
DWORD g_dwBeaconThreadID = 0;
DWORD g_dwSSDPReadThreadID = 0;

// Thread parameters.
BEACON_THREAD_PARAM g_bThreadParam;
READ_THREAD_PARAM  g_rThreadParam;

// The message list containg the SSDP messages to listen and search for.
SSDP_MESSAGE* g_pMessageList = NULL;
CRITICAL_SECTION g_csMessageList;
BOOL g_InitMessageList = FALSE;

// The lifetime of the device; set by StartDiscovery.
DWORD g_dwLifeTime = 0;

// Prototypes for private helper functions.
BOOL ConstructNotifyPacket(CHAR* pBytes, SSDP_MESSAGE* pMessage);
BOOL ConstructByeByePacket(CHAR* pBytes, SSDP_MESSAGE* pMessage);
BOOL ConstructResponsePacket(CHAR* pBytes, SSDP_MESSAGE* pMessage, CHAR* pTarget);
VOID ProcessSsdpRequest(SSDP_REQUEST* pSsdpRequest, RECEIVE_DATA* pData,
                        SSDP_MESSAGE* pMessageList);
VOID ProcessReceiveBuffer(RECEIVE_DATA* pData, SSDP_MESSAGE* pMessageList);
DWORD WINAPI BeaconThread(LPARAM lparam);
DWORD WINAPI SSDPReadThread(LPARAM lparam);

HANDLE g_hSsdpThreadCleanup = NULL;


//////////
// SsdpAddMessage
//      Add an SSDP message to the list. This list is used for
//      sending announcements (alive messages).  Each device and 
//      service should add a message to the list.
//
// PARAMETERS
//      szLocHeader: URL of the description document.
//      szType:      UPnP deviceType.
//      szUSN:       Unique service name.
//
// RETURNS: 
//      BOOL         TRUE if the message was added.
//                   FALSE if it was not added.

BOOL SsdpAddMessage(LPSTR szLocHeader, LPSTR szType, LPSTR szUSN) 
{
    // Checking to see if the SSDP message threads have already been started.
    if (g_bStartup)
    {
        puts("SsdpAddMessage: Can not add SSDP message while threads are running.");
        return NULL;
    }
    
    // Create a new pMessage node.
    SSDP_MESSAGE* pMessage;
    
    // Allocate memory for the message.
    pMessage = (SSDP_MESSAGE*)malloc(sizeof(SSDP_MESSAGE));
    if (!pMessage)
    {
        puts("SsdpAddMessage, failed to malloc new message node.");
        return FALSE;
    }
    
    // Copy the attributes to the new node.
    strcpy(pMessage->szLocHeader, szLocHeader);
    strcpy(pMessage->szType, szType);
    strcpy(pMessage->szUSN, szUSN);
    
    // Insert at the head of the list.
    if (!g_InitMessageList)
    {
        __try
        {
        InitializeCriticalSection(&g_csMessageList);
        }
        __except(1)
        {
			puts(" About to free pMessage");
            free(pMessage);
            return FALSE;
        }
        g_InitMessageList = TRUE;
    }
    EnterCriticalSection(&g_csMessageList);
    pMessage->pNext = g_pMessageList;
    g_pMessageList = pMessage;
    LeaveCriticalSection(&g_csMessageList);
    
    
    return TRUE;
} // end SsdpAddMessage


//////////
// SsdpStartDiscovery
//      Start beaconing announcements (alive notifications) and listening for 
//      search messages.  A thread is started for both activities.  This function 
//      must be called after the message list is created.
//
// PARAMETERS
//      dwLifeTime:  Number of milliseconds between broadcast of announcements.
// 
// RETURNS: 
//      BOOL    TRUE if start process succeeded.
//              FALSE if the start process failed.
//
BOOL SsdpStartDiscovery(DWORD dwLifeTime) 
{
    // Check to see if the discovery process has already been started.
    if (g_bStartup)
    {
        puts("SsdpStartDiscovery, discovery has already been initialized.");
        return FALSE;
    }
    
    // Assign to the global variable for the beaconing thread.
    g_dwLifeTime = dwLifeTime;
    
    // Set socket remote address attributes.

    g_SocketAddressTo.sin_family = AF_INET;
    g_SocketAddressTo.sin_addr.s_addr = inet_addr(SSDP_ADDR);
    g_SocketAddressTo.sin_port = htons(SSDP_PORT);
    
    // Print a message indicating the SSDP address and port.
    printf("SsdpStartDiscovery, port = %u address = %s\n", 
                SSDP_PORT, SSDP_ADDR);  
    
    if (!InitializeListNetwork())
    {
        puts("SsdpStartDiscovery, failed to initialize network adapter addresses.");
        return FALSE;
    }
    if (GetNetworks() != 0)
    {
        puts("SsdpStartDiscovery, failed to get local network adapter addresses.");
        return FALSE;
    }

    // Create a manual reset event to signal termination.
    g_hSsdpThreadCleanup = CreateEvent(NULL, TRUE, FALSE, NULL);
    // Verify that the control event was created.
    if (!g_hSsdpThreadCleanup)
    {
        puts("Could not create SSDP cleanup event!");
        return FALSE;
    }
    
    // Set beacon thread attributes.
    g_bThreadParam.ppMessageList = &g_pMessageList;
    g_bThreadParam.pSocketAddressTo = &g_SocketAddressTo;
    g_bThreadParam.Socket = g_Socket;
    g_bThreadParam.dwLifeTime = dwLifeTime;
    
    // Create the beacon thread. 
    g_hBeaconThread = CreateThread(0, 0, 
                                    (LPTHREAD_START_ROUTINE) BeaconThread,
                                    (LPVOID)&g_bThreadParam,
                                    0,
                                    &g_dwBeaconThreadID);
    
    // Set the listening thread attributes.
    g_rThreadParam.ppMessageList = &g_pMessageList;
    g_rThreadParam.Socket = g_Socket;
    
    // Create the listen thread. Listen and respond to M-SEARCHes.
    g_hSSDPReadThread = CreateThread(0, 0, 
                                    (LPTHREAD_START_ROUTINE) SSDPReadThread,
                                    (LPVOID)&g_rThreadParam,
                                    0,
                                    &g_dwSSDPReadThreadID);

  g_bStartup = TRUE;
    
    return TRUE;
} // end SsdpStartDiscovery


//////////
// SsdpStopDiscovery
//      Stop beaconing alive messages and listening for Search messages.
//      Sends a series of Byebye messages to announce device not available.
//
// PARAMETERS
//      None.  
// 
// RETURNS: 
//      BOOL    TRUE if the process was successfully stopped.
//              FALSE if unable to stop the process.
//
BOOL SsdpStopDiscovery()
{
    BOOL bResult;
    
    // Check to see if the discovery process is already running.
    if (!g_bStartup)
    {
        puts("SsdpStopDiscovery: Discovery has not been started.");

        return FALSE;
    }
    
    // Signal threads to terminate.
    if (g_hSsdpThreadCleanup)
        SetEvent(g_hSsdpThreadCleanup);
    
    // Wait for beacon thread to stop.
    WaitForSingleObject(g_hBeaconThread, 5000);
    
    // Release the handle to the beacon thread.
    bResult = CloseHandle(g_hBeaconThread);
    if (!bResult) 
    {
        puts("SsdpStopDiscovery failed to CloseHandle (beacon thread).");
    }
    g_hBeaconThread = NULL;
    
    // Deal with the message structure.
    SSDP_MESSAGE* pTemp = NULL;
    SSDP_MESSAGE* pLast = NULL;
    
    CHAR* szBuffer = (CHAR*)malloc(MAX_RESPOND);
    if (szBuffer)
    {
        if (g_bDebug)
        {
            puts("Sending ByeByes.");
        }
        // Walk through each node in the SSDP message list, and construct and send 
        // three byebye messages for each message in the list.
        EnterCriticalSection(&g_csMessageList);
        for (int i = 0; i < 3; i++)
        {
            BOOL bDbgPrint = FALSE;

            pTemp = g_pMessageList;
            while (pTemp) 
            {
                ConstructByeByePacket(szBuffer, pTemp);
                if (g_bDebug && i == 0)
                {
                    bDbgPrint = TRUE;
                }
                SendOnAllNetworks(szBuffer, &g_SocketAddressTo, bDbgPrint);
                pTemp = pTemp->pNext;
            }
        
            Sleep(XMIT_DELAY);
        }
        LeaveCriticalSection(&g_csMessageList);

        free(szBuffer);
    }
    
    CleanupListNetwork();
    SocketFinish();
    
    
    // Wait for SSDP read thread to stop.
    WaitForSingleObject(g_hSSDPReadThread, 5000);
    
    // Release the handle to the listen thread.
    bResult = CloseHandle(g_hSSDPReadThread);
    if (!bResult) 
    {
        puts("SsdpStopDiscovery failed to CloseHandle (read thread).");
    }
    g_hSSDPReadThread = NULL;
    
    // Delete the SSDP message list.
    EnterCriticalSection(&g_csMessageList);
    pTemp = g_pMessageList;
    g_pMessageList = NULL;

    while (pTemp)
    {
        pLast = pTemp;
        pTemp = pTemp->pNext;
        free (pLast);
    }
    LeaveCriticalSection(&g_csMessageList);
    
    // Put globals back into their inital state.
    CloseHandle(g_hSsdpThreadCleanup);
    g_hSsdpThreadCleanup = NULL;
    g_bStartup = FALSE;
    g_dwBeaconThreadID = 0;
    g_dwSSDPReadThreadID = 0;
    
    memset(&g_bThreadParam, NULL, sizeof(g_bThreadParam));
    memset(&g_rThreadParam, NULL, sizeof(g_rThreadParam));
    
    g_pMessageList = NULL;
    DeleteCriticalSection(&g_csMessageList);
    g_InitMessageList = FALSE;
    
    g_dwLifeTime = 0;
    
    return TRUE;
} // end SsdpStopDiscovery


/////////
//   
//      This is a separate thread.  For each message in the 
//      SSDP list, this thread does a multicast send.  Each
//      message is send 3 times, in case of dropped packets.
//      The thread continues to do this periodically (based on 
//      the Lifetime) until the thread is asked to terminate.
//      The thread terminates itself if g_hSsdpThreadCleanup 
//      is signalled
//
// PARAMETERS
//      lparam:  The BEACON_THREAD_PARAM structure passed to the thread.
// 
// RETURNS: 
//      DWORD   0 when the thread terminates.
//
DWORD WINAPI BeaconThread(LPARAM lparam) 
{
    BEACON_THREAD_PARAM*  pParam;
    SSDP_MESSAGE**         ppMessageList;
    SOCKADDR_IN*          pSocketAddressTo; 
    DWORD                 dwSleep;
    DWORD                   WaitCause = WAIT_TIMEOUT;
    
    
    CHAR* szBuffer = (CHAR*)malloc(MAX_RESPOND);
    if (szBuffer)
    {
        // Set up necessary variables to begin beaconing.
        pParam = (BEACON_THREAD_PARAM*)lparam;
        ppMessageList = pParam->ppMessageList;
        dwSleep = pParam->dwLifeTime;
        pSocketAddressTo = pParam->pSocketAddressTo;
    
        // If there is nothing in the message list, log it.
        if (!(*ppMessageList)) 
        {
            puts("BeaconThread, no messages found in list.");
        }
        else 
        {
            // Otherwise, there are messages, log that.
            printf("BeaconThread, all services will beacon every %u seconds.\n",
                        dwSleep);
        }
    
        dwSleep *= 1000;  // Convert to milliseconds.
    
        // Begin the beaconing process.
        while (TRUE) 
        {
            //afxDump << "BeaconThread: Sending SSDP notifications..\n";
        
            // Send three notifications for each message in the global list.
            for (int i = 0; i < 3; i++) 
            {
                BOOL  bDbgPrint = FALSE;
                EnterCriticalSection(&g_csMessageList);
                SSDP_MESSAGE* pTemp = *ppMessageList;
            
                // Construct the message.
                while (pTemp) 
                {
                    ConstructNotifyPacket(szBuffer, pTemp);
                    // Send discovery announcement only for the first iteration.
                    if(g_bDebug && i == 0)
                    {
                        bDbgPrint = TRUE;
                    }
                    SendOnAllNetworks(szBuffer, pSocketAddressTo, bDbgPrint);
                
                    // Select next message.
                    pTemp = pTemp->pNext;

					Sleep(XMIT_DELAY);
                }

                LeaveCriticalSection(&g_csMessageList);


            
                WaitCause = WaitForSingleObject(g_hSsdpThreadCleanup, XMIT_DELAY);
                if (WaitCause == WAIT_OBJECT_0)
                    break;
            } // end for
        
			// beacon every ten minutes
			//afxDump << "Beacon thread iswaiting for 10 minutes.\n";
            WaitCause = WaitForSingleObject(g_hSsdpThreadCleanup, 600000);
            if (WaitCause == WAIT_OBJECT_0)
                break;
        
        } // end while (TRUE)
    

        free(szBuffer);
    }
	//afxDump << "Beacon thread is exiting.\n";
    return 0;
} // end BeaconThread

void ByeBye()
{
	CHAR* szBuffer = (CHAR*)malloc(MAX_RESPOND);
	SSDP_MESSAGE* pTemp;

	EnterCriticalSection(&g_csMessageList);
        for (int i = 0; i < 3; i++)
        {
            BOOL bDbgPrint = FALSE;

            pTemp = g_pMessageList;
            while (pTemp) 
            {
                ConstructByeByePacket(szBuffer, pTemp);
                if (g_bDebug && i == 0)
                {
                    bDbgPrint = TRUE;
                }
                SendOnAllNetworks(szBuffer, &g_SocketAddressTo, bDbgPrint);
                pTemp = pTemp->pNext;
            }

        
            Sleep(XMIT_DELAY);
        }
        LeaveCriticalSection(&g_csMessageList);
}

void AliveAnnounce()
{
	CHAR* szBuffer = (CHAR*)malloc(MAX_RESPOND);

	EnterCriticalSection(&g_csMessageList);

	      for (int i = 0; i < 3; i++) 
            {
                BOOL  bDbgPrint = FALSE;
                
                SSDP_MESSAGE* pTemp = g_pMessageList;
            
                // Construct the message.
                while (pTemp) 
                {
                    ConstructNotifyPacket(szBuffer, pTemp);
                    // Send discovery announcement only for the first iteration.
                    if(g_bDebug && i == 0)
                    {
                        bDbgPrint = TRUE;
                    }
                    SendOnAllNetworks(szBuffer, &g_SocketAddressTo, bDbgPrint);
                
                    // Select next message.
                    pTemp = pTemp->pNext;
                }

				Sleep(XMIT_DELAY);
			}

	LeaveCriticalSection(&g_csMessageList);
}

void ByeByeAlive()
{
	
	ByeBye();

	AliveAnnounce();
}


/////////
//  SSDPReadThread
//      This function starts a separate thread that listens for 
//      messages on the SSDP port, then processes the SSDP messages 
//      (M-SEARCH).  The thread continues to do this until asked 
//      to terminate.  The thread terminates itself if 
//      g_hSsdpThreadCleanup is signalled.
//
// PARAMETERS
//      lparam:  The READ_THREAD_PARAM structure passed to the thread.
// 
// RETURNS: 
//      DWORD   0 when the thread terminates.
//
DWORD WINAPI SSDPReadThread(LPARAM lparam)
{
    SSDP_MESSAGE**      ppMessageList;
    READ_THREAD_PARAM*  pParam;
    SOCKET              Socket;
    INT                 ReturnValue;
    CHAR*               pszData;
    SOCKADDR_IN         RemoteSocket;
    RECEIVE_DATA        rData;
    SSDP_REQUEST        SsdpMessage;
    
    // Set up to begin read thread.
    pParam =  (READ_THREAD_PARAM*)lparam;
    ppMessageList = pParam->ppMessageList;
    
    while (TRUE)
    {
        ReturnValue = SelectAny(&Socket, INFINITE);
        if (!ReturnValue || ReturnValue == SOCKET_ERROR)
        {
            puts("SSDPReadThread, ReadSelect failed.");
            break;
        }
        
        if (SocketReceive(Socket, &pszData, &RemoteSocket) == TRUE)
        {
            if (pszData) 
            {
                // Collect information about message and message source.
                rData.socket = Socket;
                rData.szBuffer = pszData;
                rData.RemoteSocket = RemoteSocket;
                InitializeSsdpRequest(&SsdpMessage);
                
                // Parse the SSDP request.
                ReturnValue = ParseSsdpRequest(rData.szBuffer, &SsdpMessage);
                
                // Process the SSDP request.
                if (ReturnValue != FALSE) 
                {
                    ProcessSsdpRequest(&SsdpMessage, &rData, *ppMessageList); 
                    FreeSsdpRequest(&SsdpMessage);
                }
                
                free (pszData);
            }
        }

        if (WaitForSingleObject(g_hSsdpThreadCleanup, 0) == WAIT_OBJECT_0)
            break;
        
    } // end while (TRUE)
    
    return 0;
} // end SSDPReadThread
 

//////////
// ConstructResponsePacket
//      Constructs an SSDP response packet to a SSDP message.
//
// PARAMETERS
//      pBytes:     Pointer to buffer where response data is written.
//      pMessage:   Pointer to the request that we are responding to.
//      pTarget:    The target of this message.
// 
// RETURNS: 
//      BOOL        TRUE.
//
BOOL ConstructResponsePacket(CHAR* pBytes, SSDP_MESSAGE* pMessage, CHAR* pTarget) 
{
    CHAR Date[MAX_DATE_SIZE];
    CHAR Server[MAX_URI_SIZE];
    
    GetDateTime(Date);
    GetServerString(Server, sizeof(Server));

    sprintf(pBytes,
        "HTTP/1.1 200 OK\r\n"
        "ST:%s\r\n"                         // Search target.
        "USN:%s\r\n"                        // Unique server name.
        "Location:%s\r\n"                   // Location URL.
        "SERVER: %s\r\n"
        "EXT:\r\n"
        "Cache-Control:max-age=%u\r\n"      // Cache control.
        "DATE: %s\r\n"
        "\r\n",
        pTarget,
        pMessage->szUSN,
        pMessage->szLocHeader,
        Server,
        g_dwLifeTime,
        Date);
    
    return TRUE;
} // end ConstructResponsePacket


//////////
// ConstructByeByePacket
//      Constructs an SSDP ByeBye packet.
//
// PARAMETERS
//      pBytes:     Pointer to buffer where response data is written.
//      pMessage:   Request information that has Type, Location, and USN.
// 
// RETURNS: 
//      BOOL        TRUE.
//
BOOL ConstructByeByePacket(CHAR* pBytes, SSDP_MESSAGE* pMessage) 
{
    sprintf(pBytes,
        "NOTIFY * HTTP/1.1\r\n"
        "Host:239.255.255.250:1900\r\n"
        "NT:%s\r\n"
        "NTS:%s\r\n"
        "Location:%s\r\n"                      // Location URL.
        "USN:%s\r\n"                           // Unique server name.
        "Cache-Control:max-age=%u\r\n\r\n",    // Cache control.
        pMessage->szType,
        "ssdp:byebye",
        pMessage->szLocHeader,
        pMessage->szUSN,
        g_dwLifeTime);
    
    return TRUE;
} // end ConstructByeByePacket


//////////
// ConstructNotifyPacket
//      Constructs an SSDP Alive packet.
//
// PARAMETERS
//      pBytes:     Pointer to the buffer where response data is written.
//      pMessage:   Request information that has Type, Location, and USN.
// 
// RETURNS: 
//      BOOL        TRUE.
//
BOOL ConstructNotifyPacket(CHAR* pBytes, SSDP_MESSAGE* pMessage) 
{
    CHAR Server[MAX_URI_SIZE];

    GetServerString(Server, sizeof(Server));

    sprintf(pBytes,
        "NOTIFY * HTTP/1.1\r\n"
        "Host:239.255.255.250:1900\r\n"
        "NT:%s\r\n"
        "NTS:%s\r\n"
        "Location:%s\r\n"                     // Location URL.
        "USN:%s\r\n"                          // Unique server name.
        "SERVER: %s\r\n"
        "Cache-Control:max-age=%u\r\n\r\n",   // Cache control.
        pMessage->szType,
        "ssdp:alive",
        pMessage->szLocHeader,
        pMessage->szUSN,
        Server,
        g_dwLifeTime);
    
    return TRUE;
} // end ConstructNotifyPacket


//////////
// ProcessSsdpRequest
//      Processes the SSDP M_SEARCH message.  Ignores other messages.
//
// PARAMETERS
//      pSsdpRequest:   SSDP request to process.
//      pData:          Information about sockets to use.
//      pMessageList:   Message list that of alive messages.
// 
// RETURNS: 
//      Nothing.
//
VOID ProcessSsdpRequest(SSDP_REQUEST* pSsdpRequest, RECEIVE_DATA* pData,
                        SSDP_MESSAGE* pMessageList)
{
    
    if (pSsdpRequest->Method == SSDP_M_SEARCH)
    {
        if(g_bDebug)
        {
            printf("ProcessSsdpRequest, searching for %s\n",
                    pSsdpRequest->Headers[SSDP_ST]);
        }
        
        CHAR* pBuffer = (CHAR*)malloc(MAX_RESPOND);
        if (pBuffer)
        {
            EnterCriticalSection(&g_csMessageList);
        
            SSDP_MESSAGE* pTemp = pMessageList;
            while (pTemp) 
            {
                BOOL bDbgPrint = FALSE;
                // Perform the search and return the result to the control point.
                if ((strcmp(pSsdpRequest->Headers[SSDP_ST], "ssdp:all") == 0) || 
                    (strcmp(pSsdpRequest->Headers[SSDP_ST], pTemp->szType) == 0))
                {
                    ConstructResponsePacket(pBuffer, pTemp, pTemp->szType);
                    if(g_bDebug)
                    {
                        puts("ProcessSsdpRequest, match found, sending response.");
                        bDbgPrint = TRUE;
                    }

					for (int i = 0; i < 3; i++)
					{
						if (g_ServerActive)
						{
							ReplyOnNetwork(pBuffer, pData->socket, &pData->RemoteSocket, bDbgPrint);
							Sleep(XMIT_DELAY);
						}
					}
                }
            
                pTemp = pTemp->pNext;
            }
            LeaveCriticalSection(&g_csMessageList);

            free(pBuffer);
        }
    }
    else 
    {
        FreeSsdpRequest(pSsdpRequest);
    }
    
} // end ProcessSsdpRequest


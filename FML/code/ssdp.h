//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1997-2001.
//
//  File:       S S D P. H
//
//  Contents:   Responsible for keeping a device alive, and listening and
//              responding to Msearches.
//
//----------------------------------------------------------------------------

// Include all the necessary header files.
#ifndef _SSDP_H
#define _SSDP_H

#include <winsock.h>

#define XMIT_DELAY 1000                  // Time between each of the three notifications.


#define MAX_TYPE      256
#define MAX_LOCATION 1024
#define MAX_USN       256
#define MAX_RESPOND   512

// Fields necessary to publish a device.
typedef struct _SSDP_MESSAGE 
{
    CHAR szType      [MAX_TYPE];         // UPnP deviceType. 
    CHAR szLocHeader [MAX_LOCATION];     // URL of the description document.
    CHAR szUSN       [MAX_USN];          // Unique service name.
    _SSDP_MESSAGE*   pNext;              // Pointer to the next SSDP message.
} SSDP_MESSAGE;

// Parameters for beacon-thread creation.
typedef struct _BEACON_THREAD_PARAM 
{
    SSDP_MESSAGE** ppMessageList;        // Points to the message list head(_SSDP_MESSAGE).
    SOCKET Socket;                       // Multicast socket to send announcments on.
    SOCKADDR_IN* pSocketAddressTo;       // Multicast address of Socket.
    DWORD dwLifeTime;                    // Frequency, in milliseconds to send beacon messages.
} BEACON_THREAD_PARAM;                 

// Parameters for listen-thread creation.
typedef struct _READ_THREAD_PARAM 
{
    SSDP_MESSAGE** ppMessageList;        // Points to the message list head(_SSDP_MESSAGE).
    SOCKET Socket;                       // Multicast socket to listen for announcments on.
} READ_THREAD_PARAM;

typedef enum _NOTIFY_TYPE 
{ 
    NOTIFY_ALIVE  = 0,                         
        NOTIFY_PROP_CHANGE = NOTIFY_ALIVE + 1   
}   NOTIFY_TYPE;

// Parameters for receiving data from the remote socket.
typedef struct _RECEIVE_DATA 
{
    SOCKET socket;                       // Multicast socket to receive data on.
    CHAR* szBuffer;                      // Buffer to put messages in.
    SOCKADDR_IN RemoteSocket;            // Address of the remote socket.
} RECEIVE_DATA;

// Public calls



// Builds the SSDP message list.  Call before SsdpStartDiscovery.
BOOL SsdpAddMessage(
                    LPSTR szLocHeader,
                    LPSTR szType,
                    LPSTR szUSN);

// Start the beacon thread and the listen thread.
BOOL SsdpStartDiscovery ( 
                         DWORD dwLifeTime );

// Stop the beacon thread and the listen thread.
BOOL SsdpStopDiscovery();


void ByeByeAlive();


void ByeBye();

void AliveAnnounce();



#endif //_SSDP_H




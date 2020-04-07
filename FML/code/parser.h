//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1997-2001.
//
//  File:       P A R S E R. H
//
//  Contents:   Parses packets into SSDP message structures.
//
//----------------------------------------------------------------------------

// Include all the necessary header files.
#ifndef _PARSER_H
#define _PARSER_H

#include "stdafx.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <windows.h>

#define NUM_OF_HEADERS  16
#define NUM_OF_METHODS   5
#define END_HEADERS_SIZE 3


typedef enum _SSDP_METHOD 
{ 
    SSDP_NOTIFY         = 0,
        SSDP_M_SEARCH       = 1,
        GENA_SUBSCRIBE      = 2,
        GENA_UNSUBSCRIBE    = 3,
        SSDP_INVALID        = 4
}   SSDP_METHOD;


// SSDP message structure.
typedef struct _SSDP_REQUEST 
{
    SSDP_METHOD Method;              // SSDP method to invoke.
    LPSTR RequestUri;                // The unique ID of this request.
    LPSTR Headers[ 16 ];             // Array of message headers.
    LPSTR ContentType;               
    LPSTR Content;
}   SSDP_REQUEST;


typedef enum _SSDP_HEADER 
{ 
    SSDP_HOST            = 0,
    SSDP_NT           = 1,
    SSDP_NTS          = 2,
    SSDP_ST           = 3,
    SSDP_MAN          = 4,
    SSDP_MX           = 5,
    SSDP_LOCATION     = 6,
    SSDP_AL           = 7,
    SSDP_USN          = 8,
    SSDP_CACHECONTROL = 9,
    GENA_CALLBACK     = 10,
    GENA_TIMEOUT      = 11,
    GENA_SCOPE        = 12,
    GENA_SID          = 13,
    GENA_SEQ          = 14,
    CONTENT_LENGTH    = 15
}   SSDP_HEADER;


// Function prototypes.
BOOL InitializeSsdpRequest(
                           SSDP_REQUEST* pRequest);

VOID FreeSsdpRequest(
                     SSDP_REQUEST* pSsdpRequest);

BOOL ParseSsdpRequest(
                      CHAR* szMessage, 
                      SSDP_REQUEST* Result);

BOOL ParseSsdpRequestFormatted(
                               CHAR* szQueryString, 
                               CHAR* szMethod, 
                               CHAR* szPathInfo, 
                               CHAR* szHeader, 
                               CHAR* szData, 
                               SSDP_REQUEST* Result);

BOOL VerifySsdpHeaders(
                       SSDP_REQUEST* Result);

PCHAR ParseHeaders(
                   CHAR* szMessage, 
                   SSDP_REQUEST* Result);

PCHAR ParseRequestLine(
                       CHAR* szMessage, 
                       SSDP_REQUEST* Result);

BOOL ParseContent(
                  const char* pContent, 
                  SSDP_REQUEST* Result);

BOOL IsStrDigits(
                 LPSTR pszStr);

VOID strtrim(
             CHAR** pszStr);


#endif // _PARSER_H
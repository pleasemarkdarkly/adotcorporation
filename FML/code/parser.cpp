//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1997-2001.
//
//  File:       P A R S E R. C P P
//
//  Contents:   Parses packets into SSDP message structures.
//
//----------------------------------------------------------------------------

// Include the necessary headers files.

#include "stdafx.h"
//#include <windows.h>
//#include <stdlib.h>
//#include <stdio.h>
#include "parser.h"
#include "global.h"

const char* szEndOfHeaders = "\n\r\n";

// Valid SSDP methods.
CHAR* SsdpMethodStr[] = 
{
    "NOTIFY",
    "M-SEARCH",
    "SUBSCRIBE",
    "UNSUBSCRIBE",
    "INVALID",
};

// Valid SSDP headers.
CHAR* SsdpHeaderStr[] = 
{
    "Host",
    "NT",
    "NTS",
    "ST",
    "Man",
    "MX",
    "Location",
    "AL",
    "USN",
    "Cache-Control",
    "Callback",
    "Timeout",
    "Scope",
    "SID",
    "SEQ",
    "Content-Length"
};


//////////
// InitializeSsdpRequest
//      Set up a new SSDP request message. 
//
// PARAMETERS
//      pRequest:  The SSDP request to process.
// 
// RETURNS: 
//      BOOL       TRUE.
//
BOOL InitializeSsdpRequest(
                           SSDP_REQUEST* pRequest)
{
    INT i;
    
    // Set everything to NULL.
    pRequest->Method = SSDP_INVALID;
    pRequest->RequestUri = NULL;
    
    for (i = 0; i < NUM_OF_HEADERS; i++)
    {
        pRequest->Headers[i] = NULL;
    }
    
    pRequest->ContentType = NULL;
    pRequest->Content = NULL;
    return TRUE;
} // end InitializeSsdpRequest


//////////
// FreeSsdpRequest
//      Release an SSDP request message.
//
// PARAMETERS
//      pSsdpRequest:  The request to free.
// 
// RETURNS: 
//      Nothing.
//
VOID FreeSsdpRequest(
                     SSDP_REQUEST* pSsdpRequest)
{    
    
    // Set everything to NULL.
    INT i = 0;
    if (pSsdpRequest->Content != NULL)
    {
        free(pSsdpRequest->Content);
        pSsdpRequest->Content = NULL;
    }
    
    if (pSsdpRequest->ContentType != NULL)
    {
        free(pSsdpRequest->ContentType);
        pSsdpRequest->ContentType = NULL;
    }
    
    if (pSsdpRequest->RequestUri != NULL)
    {
        free(pSsdpRequest->RequestUri);
        pSsdpRequest->RequestUri = NULL;
    }
    
    for (i = 0; i < NUM_OF_HEADERS; i++)
    {
        if (pSsdpRequest->Headers[i] != NULL)
        {
            free(pSsdpRequest->Headers[i]);
            pSsdpRequest->Headers[i] = NULL;
        }
    }
} // end FreeSsdpRequest


//////////
// ParseSsdpRequest
//      Parses an SSDP request and return the content. 
//
// PARAMETERS
//      szMessage:  The SSDP request to parse.
//      Result:     On output, contains the request as a SSDP_REQUEST.
// 
// RETURNS: 
//      BOOL    TRUE if the message was successfully parsed.
//              FALSE if the message could not be parsed.
//
BOOL ParseSsdpRequest(
                      CHAR* szMessage, 
                      SSDP_REQUEST* Result)
{
    
    CHAR* szHeaders;    
    
    // Parse the line.  If there are no headers, return FALSE.
    szHeaders = ParseRequestLine(szMessage, Result); 
    if (szHeaders == NULL)
    {
        return FALSE; 
    }
    
    // Parse the headers.  If there is no content, return FALSE.
    char* pContent = ParseHeaders(szHeaders, Result);
    if (pContent == NULL)
    {
        return FALSE;
    }
    else
    {
        // Otherwise, verify the headers.
        if (VerifySsdpHeaders(Result) == FALSE)
        {
            return FALSE; 
        }
        
        // If the headers are correct, parse the content.  
        if (Result->Headers[CONTENT_LENGTH] != NULL)
        {
            ParseContent(pContent, Result);
        }
        return TRUE;
    }
} // end ParseSsdpRequest


//////////
// ParseSsdpRequestFormatted
//      Parses an SSDP request and returns the content. 
//      The HTTP request is already partially processed,
//      so the headers and location info are passed as 
//      separate parameters
//
// PARAMETERS
//      szQueryString:  The query part of request URL.
//      szMethod:       The method name from the request.
//      szPathInfo:     The path info of the request URL.
//      szHeader:       The HTTP header part of request.
//      szData:         The body part of request.
//      Result:         On output, contains the request as a SSDP_REQUEST.
// 
// RETURNS: 
//      BOOL            TRUE if the message was successfully parsed.
//                      FALSE if the message could not be parsed.
//
BOOL ParseSsdpRequestFormatted(
                               CHAR* szQueryString, 
                               CHAR* szMethod, 
                               CHAR* szPathInfo, 
                               CHAR* szHeader, 
                               CHAR* szData, 
                               SSDP_REQUEST* Result)
{
    INT i;
    
    // Look for the method.
    for (i = 0; i < NUM_OF_METHODS; i++)
    {
        if (_stricmp(SsdpMethodStr[i], szMethod) == 0)
        {
            Result->Method = (SSDP_METHOD)i;
            break;
        }
    }
    
    // If no method found, return FALSE.
    if (i == NUM_OF_METHODS)
    {     
        return FALSE;
    }
    
    // Parse the headers.  If there is no content, return FALSE.
    char* pContent = ParseHeaders(szHeader, Result);
    if (pContent == NULL)
    {
        return FALSE;
    }
    else
    {
        // Otherwise, verify the headers.
        if (VerifySsdpHeaders(Result) == FALSE)
        {
            return FALSE; 
        }
        
        // If the headers are correct, parse the content.  
        if (Result->Headers[CONTENT_LENGTH] != NULL)
        {
            ParseContent(szData, Result);
        }
        return TRUE;
    }
} // end ParseSsdpRequestFormatted


//////////
// VerifySsdpHeaders
//      Verify that the headers of an SSDP request are correct.
//
// PARAMETERS
//      Result:  The header to parse formatted as a SSDP_REQUEST.
// 
// RETURNS: 
//      BOOL     TRUE if the header is correct.
//               FALSE if the header could not be verified.
//
BOOL VerifySsdpHeaders(
                       SSDP_REQUEST* Result)
{
    // Perform the various verification tests.
    
    // Is the information for an SSDP_M_SEARCH present?
    if ((Result->Method == SSDP_M_SEARCH && 
        Result->Headers[SSDP_ST] == NULL) ||
        (Result->Method == SSDP_M_SEARCH && 
        Result->Headers[SSDP_MX] == NULL))
    {  
        return FALSE;
    } 
    
    // Is the correct type of information present?
    if (Result->Headers[SSDP_MX] != NULL && 
        IsStrDigits(Result->Headers[SSDP_MX]) == FALSE)
    {
        return FALSE;
    }
    
    // If the message is SSDP_NOTIFY, is the required information present?
    if (Result->Method == SSDP_NOTIFY)
    {
        if (Result->Headers[SSDP_NT] == NULL || 
            Result->Headers[SSDP_NTS] == NULL)
        {
            return FALSE;
        }
        
        // If the message is a PropChange method, 
        // is the required information present?
        if (_stricmp(Result->Headers[SSDP_NTS], "upnp:propchange") &&
            (Result->Headers[SSDP_AL] == NULL) &&
            (Result->Headers[SSDP_LOCATION] == NULL))
        {           
            return FALSE;
        }
    }
    
    // If there is not digit information present, return FALSE.
    if (Result->Headers[CONTENT_LENGTH] != NULL && 
        IsStrDigits(Result->Headers[CONTENT_LENGTH]) == FALSE )
    {       
        return FALSE;
    }
    return TRUE; 
} // end VerifySsdpHeaders


//////////
// ParseHeaders
//      Parse the headers out of an SSDP request. 
//
// PARAMETERS
//      szMessage:  The message to parse.
//      Result:     A SSDP_REQUEST, to be filled with results.
// 
// RETURNS: 
//      PCHAR       Pointer to message after the end of the headers.
//                  NULL if the header could not be parsed.
//
PCHAR ParseHeaders(
                   CHAR* szMessage, 
                   SSDP_REQUEST* Result)
{
    CHAR* token;
    INT i;
    
    // Get the next header.
    token = strtok(szMessage, "\r\n");
    
    while (token != NULL)
    {
        CHAR* pHeaderSep; 
        CHAR* pBeyondTokenEnd;
        pBeyondTokenEnd = token + strlen(token) + 1;
        pHeaderSep = strchr( token, ':' );
        
        // Check for proper format.
        if (pHeaderSep == NULL)
        {
            printf("ParseHeaders, token %s does not have a ':', ignored.\n",
                    token);
        }
        else
        {
            *pHeaderSep = '\0';
            strtrim(&token);
            
            // Walk through each of the headers.
            for (i = 0; i < NUM_OF_HEADERS; i++)
            {
                if (_stricmp(SsdpHeaderStr[i], token) == 0)
                {
                    // Record the header in the Result.
                    CHAR* value;
                    value = pHeaderSep + 1;
                    strtrim(&value);
                    // Added 9 byte pad to the malloc.
                    Result->Headers[i] = (CHAR*) malloc((sizeof(CHAR)) * 
                                                        (strlen(value) + 10));
                    
                    
                    // If empty, free the structure.
                    if (Result->Headers[i] == NULL)
                    {
                        FreeSsdpRequest(Result);
                        return NULL;
                    }
                    
                    // Copy the value into the Result structure.
                    strcpy(Result->Headers[i], value);
                    break;
                }
            }
            
            if (i == NUM_OF_HEADERS)
            {
                // Ignore any header not recognized.
            }
        }
        
        // Get the next header
        if (!strncmp(pBeyondTokenEnd, szEndOfHeaders, END_HEADERS_SIZE))
        {
            return (pBeyondTokenEnd + END_HEADERS_SIZE);
        }
        else
        {
            token = strtok(NULL, "\r\n");
            if (token == NULL)
                return pBeyondTokenEnd;
        }
    }
    
    return NULL;
} // end ParseHeaders


//////////
// ParseRequestLine
//      Parse the request line of an SSDP request. 
//
// PARAMETERS
//      szMessage:  The message to parse.
//      Result:     A SSDP_REQUEST to be filled with results.
// 
// RETURNS: 
//      PCHAR       Pointer to message after the end of the request line.
//                  NULL if the request line could not be parsed.
//
PCHAR ParseRequestLine(
                       CHAR* szMessage, 
                       SSDP_REQUEST* Result)
{
    CHAR* token;
    INT i;
    
    //  Obtain the HTTP method.
    token = strtok(szMessage, " \t\n");
    if (token == NULL)
    {
        return NULL;
    }
    
    // Look for the method.
    for (i = 0; i < NUM_OF_METHODS; i++)
    {
        if (_stricmp(SsdpMethodStr[i], token) == 0)
        {
            Result->Method = (SSDP_METHOD)i;
            break;
        }
    }
    
    // If no method found, return NULL.
    if (i == NUM_OF_METHODS)
    {     
        return NULL;
    }
    
    // Get the request URI.
    token = strtok(NULL, " ");
    if (token == NULL)
    {
        return NULL;
    }
    
    // Obtain the string after the last '/'.
    Result->RequestUri = (CHAR*)malloc(strlen(token) + 1);
    
    // If not found, return NULL.
    if (Result->RequestUri == NULL)
    {    
        return NULL;
    }
    
    // Record the service.
    strcpy(Result->RequestUri, token);
    
    // Get the version number.
    token = strtok(NULL, "  \t\r");
    
    // If version number not found, return NULL.
    if (token == NULL)
    {
        FreeSsdpRequest(Result);
        return NULL;
    }
    
    // If not HTTP/1.1, return NULL.
    if (_stricmp(token, "HTTP/1.1") != 0)
    {
        FreeSsdpRequest(Result);
        return NULL;
    }
    
    // Return the size of the header.
    return (token + strlen(token) + 1);
} // end ParseRequestLine


//////////
// ParseContent
//      Parses the content of an SSDP request. 
//
//      Pre-Conditions: Result->Headers[CONTENT_LENGTH] must contain only digits.
//      pContent must point to the first char of the body.
//
// PARAMETERS
//      pContent:   The content to parse.
//      Result:     A SSDP_REQUEST to be filled with results.
// 
// RETURNS: 
//      BOOL        TRUE if the content was parsed.
//                  FALSE if there not enough content, or other error.
//
BOOL ParseContent(const char* pContent, SSDP_REQUEST* Result)
{
    
    long lContentLength = atol(Result->Headers[CONTENT_LENGTH]);
    
    if (lContentLength == 0)
    {
        // There were no headers in the message.
        return TRUE;
    }
    else
    {
        if (((long) strlen(pContent) + 1) < lContentLength)
        { 
            // Bogus message detected.  
            return FALSE;
        }
        
        Result->Content = (CHAR*) malloc(lContentLength + 1);
        if (Result->Content == NULL)
        {
            // malloc failed.
            return FALSE;
        }
        else
        {
            // Copy message into local storage.
            strncpy(Result->Content, pContent, lContentLength);
            return TRUE;
        }
    }
} // end ParseContent


// Private helper functions

//////////
// IsStrDigits
//      Checks to see if a string has only digits in it.
//
// PARAMETERS
//      pszStr:  The string to check.
// 
// RETURNS: 
//      BOOL     TRUE if the string is all digits.
//               FALSE if the string contains non-digit characters.
//
BOOL IsStrDigits(LPSTR pszStr)
{
    int i = 0; 
    while (pszStr[i] != '\0')
    {
        
        if (isdigit(pszStr[i++]) == 0)
        {
            return FALSE; 
        }
    }
    return TRUE; 
} // end IsStrDigits


//////////
// strtrim
//      Removes spaces and tabs from a string. 
//
// PARAMETERS
//      pszStr:  The string to process. On output, contains the trimmed string.
// 
// RETURNS: 
//      Nothing.
//
VOID strtrim(CHAR** pszStr)
{
    CHAR* end;
    CHAR* begin;
    begin = *pszStr;
    end = begin + strlen(*pszStr) - 1;
    while (*begin == ' ' || *begin == '\t')
    {
        begin++;
    }
    *pszStr = begin;
    while (*end == ' ' || *end == '\t')
    {
        end--;
    }
    *(end + 1) = '\0';
} // end strtrim



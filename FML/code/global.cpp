
#include "stdafx.h"
#include "global.h"
#include "network.h"
#include "httpserver.h"
#include "metadata.h"
#include "medialibrary.h"
#include "iml_xml.h"



CMediaDatabase theMediaDatabase;

bool g_ServerActive;


#ifdef _VERBOSE_DEBUG

char buf[2048];
CRITICAL_SECTION access;
CFile			debugFile;
CFileException fileException;

void LogOutput(const char *fmt, ... )
{
	EnterCriticalSection(&access);	
    va_list ap;
	va_start( ap, fmt );
	vsprintf(buf,fmt,ap);
	va_end( ap );
	debugFile.Write(buf,strlen(buf));		
	LeaveCriticalSection(&access);	
}

void OpenDebugFile(const char *name)	
{
	InitializeCriticalSection(&access); 
	debugFile.Open( name, CFile::modeCreate | CFile::modeReadWrite , &fileException );
}

void CloseDebugFile()
{
	debugFile.Close(); 
	DeleteCriticalSection(&access);
}


#endif //_VERBOSE_DEBUG




// the global event that signals threads to die
HANDLE                  g_hEventCleanup = NULL;

// the root device that we are dealing with
UPNPDEV*				pDeviceRoot;

// where to get the device description from
char                    *pDeviceDescription;


int  g_bDebug = 1;

// build something along the lines of http://192.168.0.65:2867/description.xml 
BOOL BuildDescURL(char **descURL)
{
	char temp[MAX_NAME_SIZE];
	char *ipAddress;

	if (!GetIPAddress(&ipAddress))
		return FALSE;

	strcpy(temp,"http://");
	strcat(temp,ipAddress);
	strcat(temp,HTTP_LISTEN_PORT_TEXT);  //defined in httpserver.h
	strcat(temp,descName);

	*descURL = (char *)malloc(strlen(temp)+1);
	strcpy(*descURL,temp);

	return TRUE;
}

BOOL BuildBaseURL(char **baseURL)
{
	char temp[MAX_NAME_SIZE];
	char *ipAddress;

	if (!GetIPAddress(&ipAddress))
		return FALSE;

	strcpy(temp,"http://");
	strcat(temp,ipAddress);
	strcat(temp,HTTP_LISTEN_PORT_TEXT);  //defined in httpserver.h

	*baseURL = (char *)malloc(strlen(temp)+1);
	strcpy(*baseURL,temp);

	return TRUE;
}




DWORD GetFileContents(LPTSTR szFile, PCHAR* ppszaContents)
{
    DWORD       cch = 0;
    BOOL        fRet = FALSE;
    PCHAR       pszaContents;
    
   
    // Obtain a handle to the file.
    HANDLE hFile = CreateFile(szFile,
                                GENERIC_READ,
                                0,
                                NULL,
                                OPEN_EXISTING,
                                0,
                                NULL);

    // Failed to obtain a handle; log an error and exit the function.
    if (hFile == INVALID_HANDLE_VALUE)
    {
        debugMessage("CreateFile failed to open file");
        return 0;
    }
    
    // Obtain the size of the file; allocate the necessary memory.
    cch = GetFileSize(hFile, NULL);
    if (cch != (DWORD)-1)
    {
        pszaContents = (PCHAR)malloc(cch + 1);
        if (pszaContents)
        {
            // Read the file into the buffer.
            if (ReadFile(hFile, pszaContents, cch,&cch, NULL))
            {
                pszaContents[cch] = 0;  // Null-terminate.

                fRet = TRUE;
                *ppszaContents = pszaContents;
            }
            else
            {
                free(pszaContents);
            }
        }
    }
   
    // Release the handle.
    CloseHandle(hFile);
    return fRet ? cch : 0;
} // end GetFileContents

BOOL DoCleanupCd(UPNPDEV* pRoot)
{
    BOOL      fPassed = TRUE;   
    BOOL      bResult = TRUE;  
    
    // Unpublish the device with SSDP.
    bResult = FUnPublishRootDevice(pRoot);
    if (!bResult)
    {
        debugMessage(TEXT("Error, failed to unpublish a root device.")); 
        fPassed = FALSE;
    }
    
    // Deallocate the root device structure.
    bResult = FDeleteRootDeviceStruct (pRoot);
    if (!bResult)
    {
        debugMessage(TEXT("Error, failed to delete a root device structure."));
        fPassed = FALSE;
    }
    
    return fPassed;
} // end DoCleanupCd



int startUPnPDiscovery(char *descriptionFile)
{
	
	PCHAR		 pFileContents;
	char * baseURL;

	BuildBaseURL(&baseURL);

	
	char hostname[50];

	gethostname(&hostname[0],50);

	CString temp;

	CString friendlyName=theMediaDatabase.libraryName;
	CString deviceGUID = theMediaDatabase.libraryGUID;
	
	temp.Format(deviceDescriptionBase,deviceGUID,friendlyName,baseURL);

	pFileContents = (char*)malloc(temp.GetLength()+1);
	strcpy(pFileContents,temp);

	free(baseURL);
	
	
	debugMessage("Doing discovery startup\n\r");

	// Create the root device structure.

    BOOL bResult = FCreateRootDeviceStruct (pFileContents, &pDeviceRoot);


	// Save the device description URL in the root device.
    strcpy(pDeviceRoot->szDescUrl,pDeviceDescription);

    
    free (pFileContents); 
    
    if (!bResult)
    {
        debugMessage("Failed to parse Device description file "); 
        return 0;
    }


	// this starts the threads and so on
	bResult = FPublishRootDevice(pDeviceRoot);

    if (!bResult)
    {
        debugMessage("Failed to publish root device ");
    }    
    
	return 1;

}


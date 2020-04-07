//#include <windows.h>
#include "stdafx.h"

//#include <stdlib.h>

//#include <stdio.h>

#include "global.h"
#include "ssdp.h"
#include "network.h"
#include "parser.h"
#include "util.h"
#include "httpserver.h"
#include "iml_upnp.h"
#include "encode.h"
#include "responses.h"
#include "medialibrary.h"
#include "iml_xml.h"


 char RootMenuSoapHeader[]	= "<?xml version=\"1.0\"?>\n<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><SOAP-ENV:Body><m:GetDeviceRootMenuResponse xmlns:m=\"urn:schemas-upnp-org:service:mls:1\"><RootMenu xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"String\">";
 char RootMenuSoapFooter[]	= "</RootMenu></m:GetDeviceRootMenuResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>";  

 char c_releaseViewCannedResponse[] = "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><SOAP-ENV:Body><m:ReleaseViewResponse xmlns:m=\"urn:schemas-upnp-org:service:mls:1\"/></SOAP-ENV:Body></SOAP-ENV:Envelope>";


// get the numeric part of an xpath query string
int  getNumericKey(char * xpathquery)
{
	char *start;
	char number[20];
	int i = 0;
	
	start = strstr(xpathquery,"=");
	if (!start) return 0;
	start++;

	// get to after the "="

	while (*start != ']')
		number[i++] = *start++;

	number[i] = '\0';

	return atoi(number);
}

int GetTagValue(char * buffer, char *tag)
{
	
	char *start, *end;
	int val;
	start = strstr(buffer,tag);
	if (!start) return NULL;

	// find the end of the tag and the start of the request part
	// we will definitely get the end tag if the strstr above succeeded
	while (*start++ != '>');

	// start now points to the start of the request string

	// now find the end of the request
	end = start;

	end = strstr(end,"<");
	if (!end) return NULL;


	char * count = (char*)malloc(end-start+1);
	if (!count)
		return NULL;

	int i = 0;

	// copy the characters

	while (start<end)
		count[i++]=*start++;

	count[i] = '\0';

	// convert to an integer;
	val = atoi(count);
	free(count);

	return val;
}

char * GetTagString(char *buffer, char *tag)
{
	
	char *start, *end;

	start = strstr(buffer,tag);
	if (!start) return NULL;

	// find the end of the tag and the start of the request part
	// we will definitely get the end tag if the strstr above succeeded
	while (*start++ != '>');

	// start now points to the start of the request string

	// now find the end of the request
	end = start;

	end = strstr(end,"<");
	if (!end) return NULL;


	char * tagstring = (char*)malloc(end-start+1);
	if (!tagstring)
		return NULL;

	int i = 0;

	// copy the characters

	while (start<end)
		tagstring[i++]=*start++;

	tagstring[i] = '\0';

	return tagstring;
}




char  *GetXPathQuery(char *buffer)
{
	char *start, *end;

	start = strstr(buffer,"<xpLocation>");
	if (!start) return NULL;

	// find the end of the tag and the start of the request part
	// we will definitely get the end tag if the strstr above succeeded
	while (*start++ != '>');

	// start now points to the start of the request string

	// now find the end of the request
	end = start;

	end = strstr(end,"</xpLocation>");
	if (!end) return NULL;


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

char  *GetSortOrder(char *buffer)
{
	char *start, *end;

	start = strstr(buffer,"<sortOrder>");
	if (!start) return NULL;

	// find the end of the tag and the start of the request part
	// we will definitely get the end tag if the strstr above succeeded
	while (*start++ != '>');

	// start now points to the start of the request string

	// now find the end of the request
	end = start;

	end = strstr(end,"</sortOrder>");
	if (!end) return NULL;


	char * sort = (char*)malloc(end-start+1);


	int i = 0;

	// copy the characters

	while (start<end)
		sort[i++]=*start++;

	sort[i] = '\0';

	return sort;
}





char soapHeaderStart[] = "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><SOAP-ENV:Body><m:";
char soapHeaderPart2[] = " xmlns:m=\"urn:schemas-upnp-org:service:mls:1\"><Response xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"String\">";
char soapQueryFooter[] = "&lt;/responseBody&gt;</Response></m:QueryResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>";
char soapViewItemsFooter[] = "&lt;/responseBody&gt;</Response></m:ViewItemsResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>";

CString GenerateSoapHeader(DWORD viewID, DWORD totalItems, DWORD count, DWORD startIndex, char * actionResponseType)
{
	CString response;

	// yuk
	response.Format("%s%s%s%s%d%s%d%s%d%s%d%s%s",
					(LPCSTR)soapHeaderStart,
					(LPCSTR)actionResponseType,
					(LPCSTR)soapHeaderPart2,
					"&lt;responseBody&gt;&lt;responseView&gt;&lt;viewID&gt;",
					viewID,
					"&lt;/viewID&gt;&lt;timeout&gt;900000&lt;/timeout&gt;&lt;totalItems&gt;",
					totalItems,
					"&lt;/totalItems&gt;&lt;count&gt;",
					count,
					"&lt;/count&gt;&lt;startIndex&gt;",
					startIndex,
					"&lt;/startIndex&gt;",
					"&lt;/responseView&gt;");
	return response;
}



		
char * iml_Query(char * request,DWORD qMAC)
{
	// decode the request
	unsigned char *decodeBuffer;
	u_long decodeLength;

	Hr64SzToBlob(request,&decodeBuffer,&decodeLength);
	if (decodeLength == 0)
	{
		// not encoded, so just use the request that came in
		decodeBuffer = (unsigned char*)request;
	}

	debugMessage("imlQuery: decoded request:\n");
	debugMessage((char*)decodeBuffer);

	//<queryRequest><queryContext>ME</queryContext><queryExpression>ARK=%d and ALK=%d</queryExpression><queryReturnContext>GR</queryReturnContext>

	char *szQueryContext = GetTagString((char*)decodeBuffer,"<queryContext>");
	char *szReturnContext = GetTagString((char*)decodeBuffer,"<queryReturnContext>");
	char *szQueryExpression = GetTagString((char*)decodeBuffer,"<queryExpression>");
	char *szSortOrder = GetSortOrder((char*)decodeBuffer);

	if ((!szSortOrder) || (!szQueryExpression) || (!szReturnContext) || (!szQueryContext)) 
		return NULL;

	// get some of the tag data that we need
	int countRequested = GetTagValue((char*)decodeBuffer,"<count>");
	
	int startIndex = GetTagValue((char*)decodeBuffer,"<startIndex>");
	char * szRetView = GetTagString((char*)decodeBuffer,"<returnView>");

	int iReturnView = szRetView ? (strcmp(szRetView,"true")==0) : 0;

	int viewID = -1;

	// done with the decodeBuffer now, so free it up
	free(decodeBuffer);

	// this is what we return
	char *response=NULL;

	// the data that comes back from the media library
	CString responseData;
	DWORD totalItems = 0;

	DWORD count = 0;


	viewID = theMediaDatabase.viewManager.GetViewID(szQueryExpression,szQueryContext,szReturnContext,szSortOrder,qMAC);

	responseData = GetItemsFromView(viewID,startIndex,countRequested,NULL,0,&totalItems,&count);

	// release the view if a view was not requested.

	if (!iReturnView) theMediaDatabase.viewManager.ReleaseViewID(viewID);

	CString responsePacket =  GenerateSoapHeader(viewID,totalItems,count,startIndex,"QueryResponse");
	responsePacket += responseData;
	responsePacket += soapQueryFooter;

	// get rid of this CString and make/copy a buffer to return
	response = (char *)malloc(responsePacket.GetLength()+1);
	strcpy(response,responsePacket);


	free(szQueryContext);
	free(szReturnContext);
	free(szQueryExpression);
	free(szSortOrder);
	free(szRetView);

	return response;
}




char * iml_ViewItems(char *request,DWORD qMAC)
{
	// decode the request if necessary
	
	unsigned char *decodeBuffer;
	u_long decodeLength;

	Hr64SzToBlob(request,&decodeBuffer,&decodeLength);

	if (decodeLength == 0)
	{
		// not encoded, so just use the request that came in
		decodeBuffer = (unsigned char*)request;
	}

	int countRequested = GetTagValue((char*)decodeBuffer,"<count>");
	int startIndex = GetTagValue((char*)decodeBuffer,"<startIndex>");
	int viewID = GetTagValue((char*)decodeBuffer,"<viewID>");

	char * jumpLetter = GetTagString((char*)decodeBuffer,"<jumpLetter>");
	int    jumpOffset = GetTagValue((char*)decodeBuffer,"<jumpOffset>");

	if (decodeLength) free(decodeBuffer);
	CString responseData = "";
	char *response = NULL;

	DWORD totalItems=0;
	DWORD count=0;

	CViewDetails * pViewDetails;

	pViewDetails = theMediaDatabase.viewManager.GetViewDetails(viewID);

	if (pViewDetails)
	{
		if (pViewDetails->GetMAC() == qMAC)
		{
			responseData = GetItemsFromView(viewID,startIndex,countRequested,jumpLetter,jumpOffset,&totalItems, &count);
		}
	}


	if (responseData.IsEmpty())
	{
		if (jumpLetter)
		{
			free(jumpLetter);
		}
		return NULL;
	}

	CString responsePacket =  GenerateSoapHeader(viewID,totalItems,count,startIndex,"ViewItemsResponse");
	responsePacket += responseData;
	responsePacket += soapViewItemsFooter;

	// get rid of this CString and make/copy a buffer to return
	response = (char *)malloc(responsePacket.GetLength()+1);
	strcpy(response,responsePacket);

	if (jumpLetter)
	{
		free(jumpLetter);
	}

	return response;
}

char * escape(char *input, int length, int *outputlength)
{
	char * output = (char *)malloc(length * 4);  // worst case
	
	if (!output)
		return NULL;

	int i = 0,j = 0;

	while (i<=length)
	{
		if (input[i]=='<')
		{
			output[j++] = '&';
			output[j++] = 'l';
			output[j++] = 't';
			output[j++] = ';';
		}
		else if (input[i]=='>')
		{
			output[j++] = '&';
			output[j++] = 'g';
			output[j++] = 't';
			output[j++] = ';';
		}
		else
		{
			output[j++] = input[i];
		}
		i++;
	}
	*outputlength=j;
	return output;
}
		


char * iml_GetReleaseViewResponse(char * buffer,DWORD qMAC)
{
	// just dish up the preset response

	// find out the ViewID that is being released
	DWORD viewID = (DWORD)GetTagValue(buffer,"<ViewID>");

	// tell the database to release this view ID
	theMediaDatabase.viewManager.ReleaseViewID(viewID);

	// all responses are the same, so we just return this canned one
	char *response;
	response = (char*)malloc(sizeof(c_releaseViewCannedResponse)+1);
	strcpy(response,c_releaseViewCannedResponse);

	return response;
}



#ifndef _META_DATA_H_
#define _META_DATA_H_	

// metadata utility functions

#include "global.h"
#include "stdafx.h"


#define METADATA_STRING_LENGTH 80

// for now, this is all the metadata that we care about:
typedef struct
{
	char title[METADATA_STRING_LENGTH];
	char artist[METADATA_STRING_LENGTH];
	char album[METADATA_STRING_LENGTH];
	char genre[METADATA_STRING_LENGTH];
	char albumPosition[METADATA_STRING_LENGTH];
} IML_METADATA;


BOOL GetMP3Metadata(CString filepath,IML_METADATA *metadata);

BOOL GetWMAMetadata(CString filename,IML_METADATA *metadata);

HRESULT ConvertMBtoWC( LPCTSTR ptszInString, LPWSTR *ppwszOutString );




#endif


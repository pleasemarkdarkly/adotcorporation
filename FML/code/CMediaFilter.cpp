// helper file for filtering and sorting media items

// these clases understand the xpath syntax that is used to select media elements

#include "stdafx.h"
#include "medialibrary.h"
#include "mediahelper.h"


/* tokens:
MEN - media name
MEK - media key
MET - media type (mime type)

GRN - genre name
GRK - genre key

ALN - album name
ALK - album key

ARN - artist name
ARK - artist key

PLN - playlist name
PLK - playlist key

RSN - radio station name
RSK - radio station key
*/

#define CMK_ALL 0


typedef enum
{
	TYPE_INT,		// integer
	TYPE_STRING,	// string
	TYPE_NONE,
} xpath_token_type_t;


char * xpath_tokens[NUM_TOKENS] =				{"NONE",    "MEN",		"MEK",		"MET",		"MEU",		"GRN",		"GRK",		"ALN",		"ALK",	  "ARN",		"ARK",	  "PLN",	   "PLK",	"RSN",		"RSK",		"PLO",  "ALO"};

xpath_token_type_t xpath_types[NUM_TOKENS] =	{TYPE_NONE, TYPE_STRING,TYPE_INT,	TYPE_STRING, TYPE_STRING,TYPE_STRING,TYPE_INT,	TYPE_STRING, TYPE_INT, TYPE_STRING, TYPE_INT, TYPE_STRING, TYPE_INT, TYPE_STRING,TYPE_INT, TYPE_INT, TYPE_INT};

#define NUM_COMPARATORS 6
typedef enum
{
	COMP_EQUALS,
	COMP_LESSTHANEQUALS,
	COMP_LESSTHAN,
	COMP_GREATERTHANEQUALS,
	COMP_GREATERTHAN,
	COMP_STARTSWITH
} xpath_comparator_t;

char * xpath_comparators[NUM_COMPARATORS] = {"=","<=","<",">=",">","^"};

#define NUM_OPERATORS 2

typedef enum
{
	OP_NONE,   // for the first expresion typically
	OP_AND
} xpath_operator_t;

char * xpath_operators[NUM_OPERATORS] = {"none","and"};




class CBooleanExpression
{
public:
	CBooleanExpression(xpath_token_t token, xpath_comparator_t comparator, CString value);
	bool Evaluate(xpath_context_t context, DWORD key);
private:
	xpath_token_t		m_token;
	xpath_comparator_t	m_comparator;
	xpath_token_type_t	m_tokenType;
	CString				m_csCompareValue;
	DWORD				m_dwCompareValue;

	// two helper functions
	bool DoStringEvaluate(CString csDatabase);
	bool DoWordEvaluate(DWORD dwDatabase);

	bool DoMediaContextEvaluate(DWORD key);
	bool DoArtistContextEvaluate(DWORD key);
	bool DoAlbumContextEvaluate(DWORD key);
	bool DoGenreContextEvaluate(DWORD key);
	bool DoPlayListContextEvaluate(DWORD key);
	bool DoRadioStationEvaluate(DWORD key);

	bool DoFindKeyInPlayList(DWORD key);
};

class CFilterElement
{
public:
	CFilterElement(CBooleanExpression *pExpression, xpath_operator_t xoperator)
	{ m_pExpression = pExpression; m_operator = xoperator; };

	~CFilterElement()
	{ delete m_pExpression; }

	CBooleanExpression *m_pExpression;
	xpath_operator_t    m_operator;
};


// ###################################### CMediaFilter ###########################################

CMediaFilter::CMediaFilter(char * xPath)
{
	pExpression = new CFilterExpression(xPath);
}

CMediaFilter::~CMediaFilter()
{
	delete pExpression;
}

	//bool Evaluate(xpath_context_t context, DWORD mediaKey);

bool CMediaFilter::Evaluate(xpath_context_t context, DWORD key)
{
	bool ret = 0;
	if (pExpression)
		ret = pExpression->Evaluate(context,key);
	return ret;
}


// ######################################  xpath parsing  ###############################################

// expressions looks like this <!CDATA[yy[xxxccval op xxxccval op ....]]> 

char * FindStartFilter(char *pchar)
{
	char * ret;
	ret = pchar;

	// find the 3rd brace

	while (*ret++!='[');
	while (*ret++!='[');
	while (*ret++!='[');

	return ret;
}


bool ParseToken(xpath_token_t *token,char ** ppchar)
{
	int i = 0;
	while (i < NUM_TOKENS)
	{
		if (strstr(*ppchar,xpath_tokens[i])==*ppchar)
		{
			*token = (xpath_token_t)(i);
			*ppchar += strlen(xpath_tokens[i]);
			return 1;
		}
		i++;

	}
	return 0;
}

bool ParseComparator(xpath_comparator_t *comparator, char **ppchar)
{
	int i = 0;
	while (i < NUM_COMPARATORS)
	{
		if (strstr(*ppchar,xpath_comparators[i])==*ppchar)
		{
			*comparator = (xpath_comparator_t)(i);
			*ppchar += strlen(xpath_comparators[i]);
			return 1;
		}
		i++;
	}
	return 0;
}

bool ParseOperator(xpath_operator_t *xoperator, char **ppchar)
{
	int i = 0;
	while (i < NUM_OPERATORS)
	{
		if (strstr(*ppchar,xpath_operators[i])==*ppchar)
		{
			*xoperator = (xpath_operator_t)(i);
			*ppchar += strlen(xpath_operators[i]);
			return 1;
		}
		i++;
	}
	return 0;
}

bool GetValue(char **value, char **ppchar)
{
	char *end = *ppchar;
	char *start = *ppchar;
	char *endstring = start+strlen(*ppchar);

	if (*start == '\'')
	{
		start++;
		end = start+1;
		while (*end!='\'')end++;
	}
	else
	{
	
		while ((end<endstring) && (*end!= ' ') && (*end!='/'))
			end++;
	}

	char * newValue = (char *)malloc(end-start+1);

	int i = 0;

	while (start<end) 
		newValue[i++] = *start++;


	newValue[i] = '\0';

	*ppchar = end;

	*value = newValue;

	return 1;
}

void SkipSpaces(char ** ppchar)
{
	char *p = *ppchar;
	while (*p==' ')p++;
	*ppchar = p;

}


// ######################################  CFilterExpression ############################################

CFilterExpression::CFilterExpression(char *xpath)
{
	xpath_token_t token;
	xpath_comparator_t comparator;
	char * value;
	xpath_operator_t xoperator = OP_NONE;

	char * pCurr = xpath;

	if (ParseToken(&token,&pCurr))
	{	
		if (ParseComparator(&comparator,&pCurr))
		{
			if (GetValue(&value,&pCurr))
			{
				filterElements.Add((CObject*)new CFilterElement( new CBooleanExpression(token,comparator,value), xoperator));

				SkipSpaces(&pCurr);

				while (ParseOperator(&xoperator,&pCurr))
				{
					SkipSpaces(&pCurr);

					if (ParseToken(&token,&pCurr))
					{	
						SkipSpaces(&pCurr);
						if (ParseComparator(&comparator,&pCurr))
						{
							SkipSpaces(&pCurr);

							if (GetValue(&value,&pCurr))
							{
								filterElements.Add((CObject*)new CFilterElement( new CBooleanExpression(token,comparator,value), xoperator));
								SkipSpaces(&pCurr);
							}

							SkipSpaces(&pCurr);
						}
					}
				}
			}
		}
	}
}

CFilterExpression::~CFilterExpression()
{
	filterElements.RemoveAll();
}

bool CFilterExpression::Evaluate(xpath_context_t context, DWORD key)
{
	CFilterElement* pCurrElement;

	int totalCount = filterElements.GetSize();

	int currPosition = 0;
	bool result = 1;

	while ((result) && (currPosition < totalCount))
	{
		pCurrElement = (CFilterElement*)filterElements.GetAt(currPosition);
		switch (pCurrElement->m_operator)
		{
		case OP_NONE:
		
			result = pCurrElement->m_pExpression->Evaluate(context,key);
			break;
		case OP_AND:
			result &= pCurrElement->m_pExpression->Evaluate(context,key);
			break;
		default:
			break;
		}
		currPosition++;
	}

	return result;

}

// ######################################  CBooleanExpression ############################################

CBooleanExpression::CBooleanExpression(xpath_token_t token, xpath_comparator_t comparator, CString value)
{
	m_token = token;
	m_comparator = comparator;
	m_csCompareValue = value;
	if (xpath_types[token] == TYPE_INT)
	{
		m_dwCompareValue = atoi(value);
		m_tokenType = TYPE_INT;
	}
	else
	{
		m_tokenType = TYPE_STRING;
	}
}

bool CBooleanExpression::DoStringEvaluate(CString csMediaValue)
{
	bool ret = 0;

	// quick check for "all"
	if (m_csCompareValue.CompareNoCase("CMK_ALL")==0) return true;

	switch (m_comparator)
	{
	case COMP_EQUALS:
		ret = (csMediaValue.CompareNoCase(m_csCompareValue) == 0);
		break;
	case COMP_LESSTHAN:
		ret = (csMediaValue.CompareNoCase(m_csCompareValue) < 0);
		break;
	case COMP_LESSTHANEQUALS:
		ret = (csMediaValue.CompareNoCase(m_csCompareValue) <= 0);
		break;
	case COMP_GREATERTHAN:
		ret = (csMediaValue.CompareNoCase(m_csCompareValue) > 0);
		break;
	case COMP_GREATERTHANEQUALS:
		ret = (csMediaValue.CompareNoCase(m_csCompareValue) >= 0);
		break;
	case COMP_STARTSWITH:
		ret = (csMediaValue.CompareNoCase(m_csCompareValue.Left(csMediaValue.GetLength())) == 0);
		break;
	}
	return ret;
}

bool CBooleanExpression::DoWordEvaluate(DWORD dwMediaWord)
{
	bool ret = 0;

	if (m_dwCompareValue == CMK_ALL)
	{
		return 1;
	}

	switch (m_comparator)
	{
	case COMP_EQUALS:
		ret = (dwMediaWord == m_dwCompareValue);
		break;
	case COMP_LESSTHAN:
		ret = (dwMediaWord < m_dwCompareValue);
		break;
	case COMP_LESSTHANEQUALS:
		ret = (dwMediaWord <= m_dwCompareValue);
		break;
	case COMP_GREATERTHAN:
		ret = (dwMediaWord > m_dwCompareValue);
		break;
	case COMP_GREATERTHANEQUALS:
		ret = (dwMediaWord >= m_dwCompareValue);
		break;
	case COMP_STARTSWITH:
		// can't do "startswith" on an integer
		ret = 0;
		break;
	}
	return ret;

}

bool CBooleanExpression::DoFindKeyInPlayList(DWORD key)
{
	if (m_dwCompareValue == CMK_ALL) return true;

	CPlayList * pPlayList = theMediaDatabase.playListManager.GetPlayList(m_dwCompareValue-1);

	if (!pPlayList) return false;

	DWORD numEntries = pPlayList->playListEntries.GetSize();
	DWORD i = 0;
	bool found = 0;

	while ((!found) && (i < numEntries))
	{
		found = (pPlayList->playListEntries.GetAt(i) == key);
		i++;
	}
	return found;
}


bool CBooleanExpression::DoMediaContextEvaluate(DWORD key)
{
	bool ret;
	CMediaElement *pMedia = theMediaDatabase.mediaManager.GetMediaFromKey(key);

	if (!pMedia->GetPathValidity()) return false;

	switch (m_token)
	{
	case TOK_MEN:
		ret = DoStringEvaluate(pMedia->GetElementTitle());
		break;
	case TOK_MEK:
		ret = DoWordEvaluate(key);
		break;
	case TOK_MET:
		ret = DoStringEvaluate(pMedia->GetMimeType());
		break;
	case TOK_GRN:
		ret = DoStringEvaluate(theMediaDatabase.genreManager.GetGenreFromKey(pMedia->GetGenreIndex()));
		break;
	case TOK_GRK:
		ret = DoWordEvaluate(pMedia->GetGenreIndex());
		break;	
	case TOK_ALN:
		ret = DoStringEvaluate(theMediaDatabase.albumManager.GetAlbumFromKey(pMedia->GetAlbumIndex()));
		break;
	case TOK_ALK:
		ret = DoWordEvaluate(pMedia->GetAlbumIndex());
		break;	
	case TOK_ARN:
		ret = DoStringEvaluate(theMediaDatabase.artistManager.GetArtistFromKey(pMedia->GetArtistIndex()));
		break;
	case TOK_ARK:
		ret = DoWordEvaluate(pMedia->GetArtistIndex());
		break;
	case TOK_PLK:  // this means we are looking for the current mediakey in the specified playlist
		ret = DoFindKeyInPlayList(key);
		break;
	default:
		// the other tokens are not supported in this context
		ret = 0;
		break;
	}
	return ret;
}


bool CBooleanExpression::DoArtistContextEvaluate(DWORD key)
{
	bool ret = 0;
 	switch (m_token)
	{

	case TOK_ARN:
		ret = DoStringEvaluate(theMediaDatabase.artistManager.GetArtistFromKey(key));
		break;
	case TOK_ARK:
		ret = DoWordEvaluate(key);
		break;	
	default:
		ret = 0;
		break;
	}
	return ret;

}


bool CBooleanExpression::DoAlbumContextEvaluate(DWORD key)
{
	bool ret = 0;
	switch (m_token)
	{
	case TOK_ALN:
		ret = DoStringEvaluate(theMediaDatabase.albumManager.GetAlbumFromKey(key));
		break;
	case TOK_ALK:
		ret = DoWordEvaluate(key);
		break;	
	default:
		ret = 0;
		break;
	}

	return ret;
}


bool CBooleanExpression::DoGenreContextEvaluate(DWORD key)
{
	bool ret = 0;
	switch (m_token)
	{
	case TOK_GRN:
		ret = DoStringEvaluate(theMediaDatabase.genreManager.GetGenreFromKey(key));
		break;
	case TOK_GRK:
		ret = DoWordEvaluate(key);
		break;	
	default:
		ret = 0;
		break;
	}

	return ret;
}



bool CBooleanExpression::DoPlayListContextEvaluate(DWORD key)
{
	bool ret = 0;
	switch (m_token)
	{
	case TOK_PLN:
		ret = DoStringEvaluate(theMediaDatabase.playListManager.GetPlayListTitle(key-1));
		break;
	case TOK_PLK:
		ret = DoWordEvaluate(key-1);
		break;	
	default:
		ret = 0;
		break;
	}

	return ret;
}


bool CBooleanExpression::DoRadioStationEvaluate(DWORD key)
{
	bool ret = 0;
	switch (m_token)
	{
	case TOK_RSN:
		ret = DoStringEvaluate(theMediaDatabase.radioManager.GetStationFromKey(key-1)->GetStationTitle());
		break;
	case TOK_RSK:
		ret = DoWordEvaluate(key-1);
		break;	
	default:
		ret = 0;
		break;
	}

	return ret;
}



bool CBooleanExpression::Evaluate(xpath_context_t context, DWORD key)
{
	bool ret = 0;
	switch (context)
	{
	case CONTEXT_MEDIA:
		ret = DoMediaContextEvaluate(key);
		break;
	case CONTEXT_ARTIST:
		ret = DoArtistContextEvaluate(key);
		break;
	case CONTEXT_ALBUM:
		ret = DoAlbumContextEvaluate(key);
		break;
	case CONTEXT_GENRE:
		ret = DoGenreContextEvaluate(key);
		break;
	case CONTEXT_PLAYLIST:
		ret = DoPlayListContextEvaluate(key);
		break;
	case CONTEXT_RADIOSTATION:
		ret = DoRadioStationEvaluate(key);
		break;
	}

	return ret;
}



// forward declarations of comparison functions

int CompareME_MEN(DWORD key1, DWORD key2);
int CompareME_MEK(DWORD key1, DWORD key2);
int CompareME_MET(DWORD key1, DWORD key2);
int CompareME_GRN(DWORD key1, DWORD key2);
int CompareME_GRK(DWORD key1, DWORD key2);
int CompareGR_GRN(DWORD key1, DWORD key2);
int CompareGR_GRK(DWORD key1, DWORD key2);
int CompareME_ALN(DWORD key1, DWORD key2);
int CompareME_ALK(DWORD key1, DWORD key2);
int CompareAL_ALN(DWORD key1, DWORD key2);
int CompareAL_ALK(DWORD key1, DWORD key2);
int CompareME_ARN(DWORD key1, DWORD key2);
int CompareME_ARK(DWORD key1, DWORD key2);
int CompareAR_ARN(DWORD key1, DWORD key2);
int CompareAR_ARK(DWORD key1, DWORD key2);
int ComparePL_PLN(DWORD key1, DWORD key2);
int ComparePL_PLK(DWORD key1, DWORD key2);
int CompareRS_RSN(DWORD key1, DWORD key2);
int CompareRS_RSK(DWORD key1, DWORD key2);
int CompareME_PLO(DWORD key1, DWORD key2);
int CompareME_ALO(DWORD key1, DWORD key2);



// these are global so that the qsrot compare function can get at them easily

typedef int compare_func(DWORD,DWORD);

int             m_numCompareFunctions;
compare_func    *m_pCompareFunctions[5];

int				m_CompareExtraData[5];
int				dataIndex;



CMediaSorter::CMediaSorter(char *szSortOrder)
{
	m_szSortOrder = szSortOrder;
	m_iNumSortTokens = 0;
	m_numCompareFunctions = 0;
	xpath_token_t  token;
	xpath_comparator_t comparator;
	char *value;

	for (int i = 0 ; i < 5; m_CompareExtraData[i++]=-1);

	InitializeCriticalSection(&m_SortingLock);

	char * sortOrder = szSortOrder;
	char * endSortOrder = szSortOrder + strlen(szSortOrder);

	if (ParseToken(&token,&sortOrder))
	{
		m_xtSortTokens[m_iNumSortTokens++] = token;

		if (ParseComparator(&comparator,&sortOrder))
		{
			if (GetValue(&value,&sortOrder))
			{
				m_CompareExtraData[m_iNumSortTokens-1] = atoi(value);
			}
		}
	

		while (sortOrder < endSortOrder)
		{
			if (ParseToken(&token,&sortOrder))
			{
				m_xtSortTokens[m_iNumSortTokens++] = token;
				if (ParseComparator(&comparator,&sortOrder))
				{
					if (GetValue(&value,&sortOrder))
					{
						m_CompareExtraData[m_iNumSortTokens-1] = atoi(value);
					}
				}

			}
			else
			{
				sortOrder++;
			}
		}
	}
}


int CompareAscending(const void *a, const void *b)
{
	int comparelevel = 0;
	int comparison   = 0;
	while ((comparison==0) && (comparelevel < m_numCompareFunctions))
	{
		dataIndex = comparelevel;
		comparison = m_pCompareFunctions[comparelevel++](*(DWORD*)a,*(DWORD*)b);
	}
	return comparison;
}





void CMediaSorter::SortList(xpath_context_t context, CDWordArray &pdwArray)
{
	int i;
	EnterCriticalSection(&m_SortingLock);
	// build list of compare functions for the actual sorting function
	switch (context)
	{
	case CONTEXT_MEDIA:

		for (i=0; i< m_iNumSortTokens; i++)
		{
			switch (m_xtSortTokens[i])
			{
			case TOK_NONE:
				break;
			case TOK_MEN:
				m_pCompareFunctions[m_numCompareFunctions++] = CompareME_MEN;
				break;
			case TOK_MEK:
				m_pCompareFunctions[m_numCompareFunctions++] = CompareME_MEK;
				break;
			case TOK_MET:
				m_pCompareFunctions[m_numCompareFunctions++] = CompareME_MET;
				break;
			case TOK_GRN:
				m_pCompareFunctions[m_numCompareFunctions++] = CompareME_GRN;
				break;
			case TOK_GRK:
				m_pCompareFunctions[m_numCompareFunctions++] = CompareME_GRK;
				break;
			case TOK_ALN:
				m_pCompareFunctions[m_numCompareFunctions++] = CompareME_ALN;
				break;
			case TOK_ALK:
				m_pCompareFunctions[m_numCompareFunctions++] = CompareME_ALK;
				break;
			case TOK_ARN:
				m_pCompareFunctions[m_numCompareFunctions++] = CompareME_ARN;
				break;
			case TOK_ARK:
				m_pCompareFunctions[m_numCompareFunctions++] = CompareME_ARK;
				break;
			case TOK_PLO:
				m_pCompareFunctions[m_numCompareFunctions++] = CompareME_PLO;
				break;
			case TOK_ALO:
				m_pCompareFunctions[m_numCompareFunctions++] = CompareME_ALO;
				break;
			default:
				break;
			}
		}
		break;
	case CONTEXT_ARTIST:

		for (i=0; i< m_iNumSortTokens; i++)
		{
			switch (m_xtSortTokens[i])
			{
			case TOK_ARN:
				m_pCompareFunctions[m_numCompareFunctions++] = CompareAR_ARN;
				break;
			case TOK_ARK:
				m_pCompareFunctions[m_numCompareFunctions++] = CompareAR_ARK;
				break;
			default:
				break;
			}
		}


		break;
	case CONTEXT_ALBUM:

		for (i=0; i< m_iNumSortTokens; i++)
		{
			switch (m_xtSortTokens[i])
			{
			case TOK_ALN:
				m_pCompareFunctions[m_numCompareFunctions++] = CompareAL_ALN;
				break;
			case TOK_ALK:
				m_pCompareFunctions[m_numCompareFunctions++] = CompareAL_ALK;
				break;
			default:
				break;
			}
		}

		break;
	case CONTEXT_GENRE:
		for (i=0; i< m_iNumSortTokens; i++)
		{
			switch (m_xtSortTokens[i])
			{
			case TOK_GRN:
				m_pCompareFunctions[m_numCompareFunctions++] = CompareGR_GRN;
				break;
			case TOK_GRK:
				m_pCompareFunctions[m_numCompareFunctions++] = CompareGR_GRK;
				break;
			default:
				break;
			}
		}
		break;
	case CONTEXT_PLAYLIST:
		for (i=0; i< m_iNumSortTokens; i++)
		{
			switch (m_xtSortTokens[i])
			{
			case TOK_PLN:
				m_pCompareFunctions[m_numCompareFunctions++] = ComparePL_PLN;
				break;
			case TOK_PLK:
				m_pCompareFunctions[m_numCompareFunctions++] = ComparePL_PLK;
				break;
			default:
				break;
			}
		}

		break;
	case CONTEXT_RADIOSTATION:
		for (i=0; i< m_iNumSortTokens; i++)
		{
			switch (m_xtSortTokens[i])
			{
			case TOK_RSN:
				m_pCompareFunctions[m_numCompareFunctions++] = CompareRS_RSN;
				break;
			case TOK_RSK:
				m_pCompareFunctions[m_numCompareFunctions++] = CompareRS_RSK;
				break;
			default:
				break;
			}
		}
		break;

	}


	int iArraySize = pdwArray.GetSize();

	if (iArraySize > 0)
	{

		int iCSSize = sizeof (DWORD*);
		void* pArrayStart = (void *)&pdwArray[0];

		qsort (pArrayStart, iArraySize, iCSSize, CompareAscending);
	}

	LeaveCriticalSection(&m_SortingLock);

}





int CompareME_MEN(DWORD key1, DWORD key2)
{
	// comparing media name
	CMediaElement *p1, *p2;
	p1 = theMediaDatabase.mediaManager.GetMediaFromKey(key1);
	p2 = theMediaDatabase.mediaManager.GetMediaFromKey(key2);

	return p1->GetElementTitle().CompareNoCase(p2->GetElementTitle());

}


int CompareME_MEK(DWORD key1, DWORD key2)
{
	// comparing media keys
	return (key1 == key2 ? 0 : (key1 < key2 ? -1 : +1));

}

int CompareME_MET(DWORD key1, DWORD key2)
{

	CMediaElement *p1, *p2;
	p1 = theMediaDatabase.mediaManager.GetMediaFromKey(key1);
	p2 = theMediaDatabase.mediaManager.GetMediaFromKey(key2);

	return p1->GetMimeType().CompareNoCase(p2->GetMimeType());
}



int CompareME_GRN(DWORD key1, DWORD key2)
{
	// comparing genre names of the media 
	CMediaElement *p1, *p2;
	p1 = theMediaDatabase.mediaManager.GetMediaFromKey(key1);
	p2 = theMediaDatabase.mediaManager.GetMediaFromKey(key2);

	return theMediaDatabase.genreManager.GetGenreFromKey(p1->GetGenreIndex()).CompareNoCase(theMediaDatabase.genreManager.GetGenreFromKey(p2->GetGenreIndex()));
}


int CompareME_GRK(DWORD key1, DWORD key2)
{
		// comparing genre names of the media 
	CMediaElement *p1, *p2;
	p1 = theMediaDatabase.mediaManager.GetMediaFromKey(key1);
	p2 = theMediaDatabase.mediaManager.GetMediaFromKey(key2);

	DWORD gk1 = p1->GetGenreIndex();
	DWORD gk2 = p2->GetGenreIndex();

	return (gk1 == gk2 ? 0 : (gk1 < gk2 ? -1 : +1));
}


int CompareGR_GRN(DWORD key1, DWORD key2)
{
	// comparing genre names of genres
	return theMediaDatabase.genreManager.GetGenreFromKey(key1).CompareNoCase(theMediaDatabase.genreManager.GetGenreFromKey(key2));
}

int CompareGR_GRK(DWORD key1, DWORD key2)
{
	return (key1 == key2 ? 0 : (key1 < key2 ? -1 : +1));
}


int CompareME_ALN(DWORD key1, DWORD key2)
{
	CMediaElement *p1, *p2;
	p1 = theMediaDatabase.mediaManager.GetMediaFromKey(key1);
	p2 = theMediaDatabase.mediaManager.GetMediaFromKey(key2);
	return theMediaDatabase.albumManager.GetAlbumFromKey(p1->GetAlbumIndex()).CompareNoCase(theMediaDatabase.albumManager.GetAlbumFromKey(p2->GetAlbumIndex()));
}


int CompareME_ALK(DWORD key1, DWORD key2)
{
	CMediaElement *p1, *p2;
	p1 = theMediaDatabase.mediaManager.GetMediaFromKey(key1);
	p2 = theMediaDatabase.mediaManager.GetMediaFromKey(key2);

	DWORD ak1 = p1->GetAlbumIndex();
	DWORD ak2 = p2->GetAlbumIndex();

	return (ak1 == ak2 ? 0 : (ak1 < ak2 ? -1 : +1));

}

int CompareAL_ALN(DWORD key1, DWORD key2)
{
	return theMediaDatabase.albumManager.GetAlbumFromKey(key1).CompareNoCase(theMediaDatabase.albumManager.GetAlbumFromKey(key2));
}


int CompareAL_ALK(DWORD key1, DWORD key2)
{
	return (key1 == key2 ? 0 : (key1 < key2 ? -1 : +1));

}

int CompareME_ARN(DWORD key1, DWORD key2)
{
	CMediaElement *p1, *p2;
	p1 = theMediaDatabase.mediaManager.GetMediaFromKey(key1);
	p2 = theMediaDatabase.mediaManager.GetMediaFromKey(key2);

	return theMediaDatabase.artistManager.GetArtistFromKey(p1->GetArtistIndex()).CompareNoCase(theMediaDatabase.artistManager.GetArtistFromKey(p2->GetArtistIndex()));
}

int CompareME_ALO(DWORD key1, DWORD key2)
{
	CMediaElement *p1, *p2;
	p1 = theMediaDatabase.mediaManager.GetMediaFromKey(key1);
	p2 = theMediaDatabase.mediaManager.GetMediaFromKey(key2);

	DWORD pos1 = p1->GetAlbumPosition();
	DWORD pos2 = p2->GetAlbumPosition();

	return (pos1 == pos2 ? 0 : (pos1 < pos2 ? -1 : +1));

}

int CompareME_ARK(DWORD key1, DWORD key2)
{

	CMediaElement *p1, *p2;
	p1 = theMediaDatabase.mediaManager.GetMediaFromKey(key1);
	p2 = theMediaDatabase.mediaManager.GetMediaFromKey(key2);

	DWORD ak1 = p1->GetAlbumIndex();
	DWORD ak2 = p2->GetAlbumIndex();

	return (ak1 == ak2 ? 0 : (ak1 < ak2 ? -1 : +1));
}

int CompareME_PLO(DWORD key1, DWORD key2)
{
	CMediaElement *p1, *p2;
	p1 = theMediaDatabase.mediaManager.GetMediaFromKey(key1);
	p2 = theMediaDatabase.mediaManager.GetMediaFromKey(key2);

	DWORD key1Pos = 0, key2Pos = 0;

	int playlistIndex = m_CompareExtraData[dataIndex];
	
	if (playlistIndex ==-1)
	{
		return CompareME_MEN(key1,key2);
	}
	else
	{
		CPlayList * pPlaylist = theMediaDatabase.playListManager.GetPlayList((DWORD)playlistIndex-1);

		DWORD playListEntries = pPlaylist->playListEntries.GetSize();

		DWORD currPos = 0;

		while (currPos < playListEntries)
		{
			DWORD currValue = pPlaylist->playListEntries.GetAt(currPos);
			if	(currValue == key1)
			{
				key1Pos = currPos;

			}
			else if (currValue == key2)
			{
				key2Pos = currPos;

			}
			currPos++;
		}
	}
	return (key1Pos == key2Pos ? 0 : (key1Pos < key2Pos ? -1 : +1));
}

int CompareAR_ARN(DWORD key1, DWORD key2)
{
	return theMediaDatabase.artistManager.GetArtistFromKey(key1).CompareNoCase(theMediaDatabase.artistManager.GetArtistFromKey(key2));
}


int CompareAR_ARK(DWORD key1, DWORD key2)
{
	return (key1 == key2 ? 0 : (key1 < key2 ? -1 : +1));
}

int ComparePL_PLN(DWORD key1, DWORD key2)
{
	return theMediaDatabase.playListManager.GetPlayListTitle(key1-1).CompareNoCase(theMediaDatabase.playListManager.GetPlayListTitle(key2-1));
}

int ComparePL_PLK(DWORD key1, DWORD key2)
{
	return (key1 == key2 ? 0 : (key1 < key2 ? -1 : +1));
}

int CompareRS_RSN(DWORD key1, DWORD key2)
{
	CRadioStation *r1,*r2;


	r1 = theMediaDatabase.radioManager.GetStationFromKey(key1);
	r2 = theMediaDatabase.radioManager.GetStationFromKey(key2);

	return r1->GetStationTitle().CompareNoCase(r2->GetStationTitle());

}

int CompareRS_RSK(DWORD key1, DWORD key2)
{
	return (key1 == key2 ? 0 : (key1 < key2 ? -1 : +1));
}












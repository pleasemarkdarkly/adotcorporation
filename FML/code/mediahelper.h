#ifndef _MEDIA_HELPER_H_
#define _MEDIA_HELPER_H_
// helper file for filtering and sorting media items

#include "stdafx.h"



#define NUM_TOKENS 17

typedef enum
{
	TOK_NONE,
	TOK_MEN,
	TOK_MEK,
	TOK_MET,
	TOK_MEU,
	TOK_GRN,
	TOK_GRK,
	TOK_ALN,
	TOK_ALK,
	TOK_ARN,
	TOK_ARK,
	TOK_PLN,
	TOK_PLK,
	TOK_RSN,
	TOK_RSK,
	TOK_PLO,
	TOK_ALO
} xpath_token_t;


#define NUM_CONTEXTS 7

typedef enum
{
	CONTEXT_UNKNOWN,
	CONTEXT_MEDIA,
	CONTEXT_ARTIST,
	CONTEXT_ALBUM,
	CONTEXT_GENRE,
	CONTEXT_PLAYLIST,
	CONTEXT_RADIOSTATION
} xpath_context_t;



class CFilterExpression
{
public:
	CFilterExpression(char *xpath);
	~CFilterExpression();
	bool Evaluate(xpath_context_t context, DWORD key);
private:
	CObArray filterElements;
};


class CMediaFilter
{
public:
	CMediaFilter(char * xPath);
	~CMediaFilter();
	bool Evaluate(xpath_context_t context, DWORD mediaKey);

private:

	CFilterExpression *pExpression;

};

class CMediaSorter
{

public:
	CMediaSorter(char * szSortOrder);
	void SortList(xpath_context_t context, CDWordArray &pdwArray);
private:
	CString			m_szSortOrder;
	int				m_iNumSortTokens;
	xpath_token_t	m_xtSortTokens[5];

	CRITICAL_SECTION m_SortingLock;
	
};

#endif
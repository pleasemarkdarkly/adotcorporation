// Dictionary.cpp: implementation of the CDictionary class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifdef WIN32
#include <tchar.h>
#endif
#include "Dictionary.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDictionary::~CDictionary()
{
	DictionaryNode* t;
	while(m_head)
	{	
		t = m_head->m_next;
		delete m_head;
		m_head = t;
	}
}

HRESULT CDictionary::CreateInstance(IDssDictionary** pIDict)
{
	CDictionary* pDict = new CDictionary;
	return ::InternalCreateInstance(pDict,pIDict);
}

HRESULT STDCALL CDictionary::Count(DWORD* pdwNumElements)
{
	if(pdwNumElements == NULL) return E_POINTER;
	*pdwNumElements = m_dwNumElements;
	return S_OK;
}

HRESULT STDCALL CDictionary::GetEntry(DWORD dwIndex, LPCTSTR* ppszKey, LPCTSTR* ppszValue)
{
	if(ppszKey == NULL || ppszValue == NULL) return E_POINTER;
	if(dwIndex >= m_dwNumElements) return E_OUTOFRANGE;

	DictionaryNode* t = m_head;
	for(DWORD d=0; d<dwIndex; d++)
		t = t->m_next;
	
	*ppszKey = (LPCTSTR)t->m_szKey;
	*ppszValue = (LPCTSTR)t->m_szValue;
	return S_OK;
}

HRESULT STDCALL CDictionary::AddEntry(LPCTSTR szKey, LPCTSTR szValue)
{
	if(szKey == NULL || szValue == NULL) return E_POINTER;
	DictionaryNode* t = new DictionaryNode(szKey,szValue);
	if(t == NULL) return E_OUTOFMEMORY;
	
	DictionaryNode* n = m_head;

	if(!m_head)
	{
		m_head = t;
	}
	else
	{
		while(n->m_next)
			n = n->m_next;
		n->m_next = t;
	}
	
	m_dwNumElements++;

	return S_OK;
}

HRESULT STDCALL CDictionary::FindEntry(LPCTSTR szKey,LPCTSTR* ppszValue)
{
	if(szKey == NULL || ppszValue == NULL) return E_POINTER;
	DictionaryNode* t = m_head;
	while(t)
	{
		if(_tcsicmp(szKey,t->m_szKey) == 0)
		{
			*ppszValue = t->m_szValue;
			return S_OK;
		}
		t = t->m_next;
	}
	return E_FAIL;
}

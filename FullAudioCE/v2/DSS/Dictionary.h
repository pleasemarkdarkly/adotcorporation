// Dictionary.h: interface for the CDictionary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DICTIONARY_H__6E577C95_45CB_11D3_8D94_0090276149F2__INCLUDED_)
#define AFX_DICTIONARY_H__6E577C95_45CB_11D3_8D94_0090276149F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Dss.h"
#include "DssString.h"

class CDictionary : public CUnknown, public IDssDictionary
{
	struct DictionaryNode
	{
		DictionaryNode(LPCTSTR szKey, LPCTSTR szValue) : m_szKey(szKey), m_szValue(szValue), m_next(NULL) {};
		CString m_szKey;
		CString m_szValue;
		DictionaryNode* m_next;
	};

	CDictionary() : m_head(NULL), m_dwNumElements(0) {};
public:
	// Declare the delegating IUnknown.
	DECLARE_IUNKNOWN(IDssDictionary)

	virtual ~CDictionary();

	static HRESULT CreateInstance(IDssDictionary** pIDict);

//Interface Methods
	virtual HRESULT STDCALL Count(DWORD* pdwNumElements);
	virtual HRESULT STDCALL GetEntry(DWORD dwIndex, LPCTSTR* ppszKey, LPCTSTR* ppszValue);
	virtual HRESULT STDCALL AddEntry(LPCTSTR szKey, LPCTSTR szValue);
	virtual HRESULT STDCALL FindEntry(LPCTSTR szKey, LPCTSTR* ppszValue);

private:
	DWORD m_dwNumElements;
	DictionaryNode* m_head;
};

#endif // !defined(AFX_DICTIONARY_H__6E577C95_45CB_11D3_8D94_0090276149F2__INCLUDED_)

//  Copyright (C) 1996-2000 Jean-Claude Wippler.  All rights reserved.
//
//  This file contains the implementation of the universal version classes.

#include "header.h"

#if q4_UNIV // until end of source
/////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>     // malloc

#if !q4_INLINE
static char _k4univInl[] = "univ.inl";
#undef THIS_FILE
#define THIS_FILE _k4univInl
#include "univ.inl"
#endif
    
#if q4_CHECK
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Implemented in this file

    class c4_BaseArray;
    class c4_WordArray;
    class c4_DWordArray;
    class c4_PtrArray;
    class c4_StringArray;

/////////////////////////////////////////////////////////////////////////////

#if !q4_MSVC && !q4_WATC && !(q4_MWCW && defined(_WIN32))

    static char* _strdup(const char* p)
    {
        if (!p)
            return 0;
            
        char* s = (char*) malloc(strlen(p) + 1);
        return strcpy(s, p);
    }

#endif

    //  The Borland C++ RTL does not want file handle objects to cross
    //  DLL boundaries, so we add special fopen/fclose hooks to this DLL.

#if q4_BORC
    #include <stdio.h>

#if q4_WIN32
    __declspec(dllexport) FILE*
#else
    FILE* __export
#endif
    f4_FileOpenInDLL(const char* name_, const char* mode_)
    {
        return fopen(name_, mode_);
    }

#if q4_WIN32
    __declspec(dllexport)
#else
    int __export
#endif
    f4_FileCloseInDLL(FILE* file_)
    {
        return fclose(file_);
    }

#endif

/////////////////////////////////////////////////////////////////////////////
// c4_BaseArray

c4_BaseArray::c4_BaseArray ()
    : _data (0), _size (0)
{
}

c4_BaseArray::~c4_BaseArray ()
{
    d4_assertThis;

    SetLength(0);
}

int c4_BaseArray::GetLength() const
{
    d4_assertThis;

    return _size;
}

void c4_BaseArray::SetLength(int nNewSize)
{
    d4_assertThis;

/*
		// if first and fits in buffer or if grows and still fits in buffer
	if (0 < nNewSize && nNewSize <= (int) sizeof _buffer &&
			(_data == 0 || _data == _buffer && nNewSize >= _size))
		_data = _buffer;
	else if (_data == _buffer)	// get rid of temp buffer
	{
		if (nNewSize == 0)
			_data = 0;
		else
		{
			_data = (char*) malloc(nNewSize);
			memcpy(_data, _buffer, nNewSize < _size ? nNewSize : _size);
		}
	}
	else
*/
		_data = _data == 0 ? nNewSize == 0 ? (char*) 0
										   : (char*) malloc(nNewSize)
						   : nNewSize == 0 ? (free(_data), (char*) 0)
										   : (char*) realloc(_data, nNewSize);
	
	d4_assert(_data != 0 || nNewSize == 0);

    int n = _size;
    _size = nNewSize;
    
    if (nNewSize > n)
        memset(GetData(n), 0, nNewSize - n);
}

const void* c4_BaseArray::GetData(int nIndex) const
{
    d4_assertThis;
    d4_assert(nIndex < _size);

    return _data + nIndex;
}

void* c4_BaseArray::GetData(int nIndex)
{
    d4_assertThis;
    d4_assert(nIndex < _size);

    return _data + nIndex;
}

void c4_BaseArray::Grow(int nIndex)
{
    d4_assertThis;

    if (nIndex > _size)
        SetLength(nIndex);
}

void c4_BaseArray::InsertAt(int nIndex, int nCount)
{
    d4_assertThis;

    SetLength(_size + nCount);

    int to = nIndex + nCount;
    if (_size > to)
        d4_memmove(GetData(to), GetData(nIndex), _size - to);
}

void c4_BaseArray::RemoveAt(int nIndex, int nCount)
{
    d4_assertThis;

    int from = nIndex + nCount;
    if (_size > from)
        d4_memmove(GetData(nIndex), GetData(from), _size - from);

    SetLength(_size - nCount);
}

/////////////////////////////////////////////////////////////////////////////
// c4_DWordArray

int c4_DWordArray::Add(t4_u32 newElement)
{
    d4_assertThis;

    int n = GetSize();
    _vector.Grow(Off(n + 1));
    SetAt(n, newElement);
    return n;
}

void c4_DWordArray::InsertAt(int nIndex, t4_u32 newElement, int nCount)
{
    d4_assertThis;

    _vector.InsertAt(Off(nIndex), nCount * sizeof (t4_u32));

    while (--nCount >= 0)
        SetAt(nIndex++, newElement);
}

void c4_DWordArray::RemoveAt(int nIndex, int nCount)
{
    d4_assertThis;

    _vector.RemoveAt(Off(nIndex), nCount * sizeof (t4_u32));
}

/////////////////////////////////////////////////////////////////////////////
// c4_PtrArray

int c4_PtrArray::Add(void* newElement)
{
    d4_assertThis;

    int n = GetSize();
    _vector.Grow(Off(n + 1));
    SetAt(n, newElement);
    return n;
}

void c4_PtrArray::InsertAt(int nIndex, void* newElement, int nCount)
{
    d4_assertThis;

    _vector.InsertAt(Off(nIndex), nCount * sizeof (void*));

    while (--nCount >= 0)
        SetAt(nIndex++, newElement);
}

void c4_PtrArray::RemoveAt(int nIndex, int nCount)
{
    d4_assertThis;

    _vector.RemoveAt(Off(nIndex), nCount * sizeof (void*));
}

/////////////////////////////////////////////////////////////////////////////
// c4_StringArray

c4_StringArray::~c4_StringArray()
{
    d4_assertThis;

    SetSize(0);
}

void c4_StringArray::SetSize(int nNewSize, int nGrowBy)
{
    d4_assertThis;

    int i = nNewSize;
    
    while (i < GetSize())
        SetAt(i++, 0);

    _ptrs.SetSize(nNewSize);

    while (i < GetSize())
        _ptrs.SetAt(i++, "");
}

void c4_StringArray::SetAt(int nIndex, const char* newElement)
{
    d4_assertThis;

    char* s = (char*) _ptrs.GetAt(nIndex);
    if (s && *s)
        free(s);

    _ptrs.SetAt(nIndex, newElement && *newElement? _strdup(newElement) : "");
}

int c4_StringArray::Add(const char* newElement)
{
    d4_assertThis;

    int n = _ptrs.Add(0);
    SetAt(n, newElement);
    return n;
}

void c4_StringArray::InsertAt(int nIndex, const char* newElement, int nCount)
{
    d4_assertThis;

    _ptrs.InsertAt(nIndex, 0, nCount);

    while (--nCount >= 0)
        SetAt(nIndex++, newElement);
}

void c4_StringArray::RemoveAt(int nIndex, int nCount)
{
    d4_assertThis;

    for (int i = 0; i < nCount; ++i)
        SetAt(nIndex + i, 0);

    _ptrs.RemoveAt(nIndex, nCount);
}

/////////////////////////////////////////////////////////////////////////////
#endif // q4_UNIV

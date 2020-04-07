//........................................................................................
//........................................................................................
//.. File Name: univ.h
//.. Last Modified By: Donni Reitz-Pesek	donni@iobjects.com
//.. Modification date: 2/17/2000
//........................................................................................
//.. Copyright:(c) 1995-2000 Interactive Objects Inc.
//..	 All rights reserved. This code may not be redistributed in source or linkable
//.. 	 object form without the express written consent of Interactive Objects.
//.. Contact Information: www.iobjects.com
//........................................................................................
//........................................................................................
//  Copyright (C) 1996-2000 Jean-Claude Wippler.  All rights reserved.
//
//  Specific header for the universal version classes.

#define q4_UNIV 1

#include "mk4str.h"

/////////////////////////////////////////////////////////////////////////////
// Declarations in this file

    class c4_BaseArray;
    class c4_PtrArray;
    class c4_DWordArray;
    class c4_StringArray;

/////////////////////////////////////////////////////////////////////////////

class c4_BaseArray
{
public:
    c4_BaseArray ();
    ~c4_BaseArray ();

    int GetLength() const;
    void SetLength(int nNewSize);

    const void* GetData(int nIndex) const;
    void* GetData(int nIndex);

    void Grow(int nIndex);

    void InsertAt(int nIndex, int nCount);
    void RemoveAt(int nIndex, int nCount);

private:
    char* _data;
    int _size;
//	char _buffer[4];
};

class c4_PtrArray
{
public:
    c4_PtrArray ();
    ~c4_PtrArray ();

    int GetSize() const;
    void SetSize(int nNewSize, int nGrowBy = -1);

    void* GetAt(int nIndex) const;
    void SetAt(int nIndex, const void* newElement);
    void*& ElementAt(int nIndex);

    int Add(void* newElement);

    void InsertAt(int nIndex, void* newElement, int nCount = 1);
    void RemoveAt(int nIndex, int nCount = 1);

private:
    static int Off(int n_);

    c4_BaseArray _vector;
};

class c4_DWordArray
{
public:
    c4_DWordArray ();
    ~c4_DWordArray ();

    int GetSize() const;
    void SetSize(int nNewSize, int nGrowBy = -1);

    t4_u32 GetAt(int nIndex) const;
    void SetAt(int nIndex, t4_u32 newElement);
    t4_u32& ElementAt(int nIndex);

    int Add(t4_u32 newElement);

    void InsertAt(int nIndex, t4_u32 newElement, int nCount = 1);
    void RemoveAt(int nIndex, int nCount = 1);

private:
    static int Off(int n_);

    c4_BaseArray _vector;
};

class c4_StringArray
{
public:
    c4_StringArray ();
    ~c4_StringArray ();

    int GetSize() const;
    void SetSize(int nNewSize, int nGrowBy = -1);

    const char* GetAt(int nIndex) const;
    void SetAt(int nIndex, const char* newElement);
//  c4_String& ElementAt(int nIndex);

    int Add(const char* newElement);

    void InsertAt(int nIndex, const char* newElement, int nCount = 1);
    void RemoveAt(int nIndex, int nCount = 1);

private:
    c4_PtrArray _ptrs;
};

/////////////////////////////////////////////////////////////////////////////

#if q4_INLINE
    #include "univ.inl"
#endif

/////////////////////////////////////////////////////////////////////////////


//........................................................................................
//........................................................................................
//.. File Name: univ.inl
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
//  This file contains inline definitions of the MFC replacement collections.
//      
//  Only very small members are selected for inline code.
//  See the class listing for a description of these functions.

/////////////////////////////////////////////////////////////////////////////
// c4_PtrArray

d4_inline c4_PtrArray::c4_PtrArray ()
{ 
}

d4_inline c4_PtrArray::~c4_PtrArray ()
{ 
}

d4_inline int c4_PtrArray::Off(int n_)
{
    return n_ * sizeof (void*); 
}

d4_inline int c4_PtrArray::GetSize() const
{ 
    return _vector.GetLength() / sizeof (void*); 
}

d4_inline void c4_PtrArray::SetSize(int nNewSize, int)
{ 
    _vector.SetLength(Off(nNewSize)); 
}

d4_inline void* c4_PtrArray::GetAt(int nIndex) const
{ 
    return *(void* const*) _vector.GetData(Off(nIndex)); 
}

d4_inline void c4_PtrArray::SetAt(int nIndex, const void* newElement)
{ 
    *(const void**) _vector.GetData(Off(nIndex)) = newElement; 
}

d4_inline void*& c4_PtrArray::ElementAt(int nIndex)
{ 
    return *(void**) _vector.GetData(Off(nIndex)); 
}

/////////////////////////////////////////////////////////////////////////////
// c4_DWordArray

d4_inline c4_DWordArray::c4_DWordArray ()
{ 
}

d4_inline c4_DWordArray::~c4_DWordArray ()
{ 
}

d4_inline int c4_DWordArray::Off(int n_)
{
    return n_ * sizeof (t4_u32); 
}

d4_inline int c4_DWordArray::GetSize() const
{ 
    return _vector.GetLength() / sizeof (t4_u32); 
}

d4_inline void c4_DWordArray::SetSize(int nNewSize, int)
{ 
    _vector.SetLength(Off(nNewSize)); 
}

d4_inline t4_u32 c4_DWordArray::GetAt(int nIndex) const
{ 
    return *(const t4_u32*) _vector.GetData(Off(nIndex)); 
}

d4_inline void c4_DWordArray::SetAt(int nIndex, t4_u32 newElement)
{ 
    *(t4_u32*) _vector.GetData(Off(nIndex)) = newElement; 
}

d4_inline t4_u32& c4_DWordArray::ElementAt(int nIndex)
{ 
    return *(t4_u32*) _vector.GetData(Off(nIndex)); 
}

/////////////////////////////////////////////////////////////////////////////
// c4_StringArray

d4_inline c4_StringArray::c4_StringArray ()
{ 
}

d4_inline int c4_StringArray::GetSize() const
{ 
    return _ptrs.GetSize(); 
}

d4_inline const char* c4_StringArray::GetAt(int nIndex) const
{ 
    return (const char*) _ptrs.GetAt(nIndex); 
}

/////////////////////////////////////////////////////////////////////////////

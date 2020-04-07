//........................................................................................
//........................................................................................
//.. File Name: column.inl
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
//  This file contains inline definitions for the column class.
//      
//  Only very small members are selected for inline code.
//  See the class listing for a description of these functions.

/////////////////////////////////////////////////////////////////////////////
// c4_Column

d4_inline int c4_Column::fSegIndex(t4_u32 offset_)
{
        // limited by max array: 1 << (kSegBits + 15) with 16-bit ints
    return (int) (offset_ >> kSegBits);
}

d4_inline t4_u32 c4_Column::fSegOffset(int index_)
{
    return (t4_u32) index_ << kSegBits;
}

d4_inline int c4_Column::fSegRest(t4_u32 offset_)
{
    return ((int) offset_ & kSegMask);
}

d4_inline c4_Strategy* c4_Column::Strategy() const
{
    return _strategy;
}

d4_inline t4_u32 c4_Column::Position() const
{
    return _position;
}

d4_inline t4_u32 c4_Column::ColSize() const
{
    return _size;
}

d4_inline bool c4_Column::IsDirty() const
{
    return _dirty;
}

d4_inline void c4_Column::SetBuffer(t4_u32 length_)
{
    SetLocation(1, length_);
    _dirty = true;
}

d4_inline void c4_Column::SetBufferClear(t4_u32 length_)
{
    SetLocation(0, length_);
    _dirty = true;
}

d4_inline const t4_byte* c4_Column::LoadNow(t4_u32 offset_)
{
    if (_segments.GetSize() == 0)
        SetupSegments();

    if (offset_ >= _gap)
        offset_ += _slack;

    t4_byte* ptr = (t4_byte*) _segments.GetAt(fSegIndex(offset_));
    return ptr + fSegRest(offset_); 
}

/////////////////////////////////////////////////////////////////////////////
// c4_ColIter

d4_inline c4_ColIter::c4_ColIter (c4_Column& col_, t4_u32 offset_, t4_u32 limit_)
    : _column (col_), _limit (limit_), _pos (offset_), _len (0), _ptr (0)
{
}

d4_inline const t4_byte* c4_ColIter::BufLoad() const
{
    return _ptr;
}

d4_inline t4_byte* c4_ColIter::BufSave()
{
    return _column.CopyNow(_pos);
}

d4_inline int c4_ColIter::BufLen() const
{
    return _len;
}

/////////////////////////////////////////////////////////////////////////////

//........................................................................................
//........................................................................................
//.. File Name: handler.inl
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
//  This file contains inline definitions for the storage classes.
//      
//  Only very small members are selected for inline code.
//  See the class listing for a description of these functions.

/////////////////////////////////////////////////////////////////////////////
// c4_Handler

d4_inline c4_Handler::c4_Handler (const c4_Property& prop_)
    : _property (prop_)
{
    d4_assertThis;
}

d4_inline c4_Handler::~c4_Handler ()
{
    d4_assertThis;
}

d4_inline void c4_Handler::Initialize(int colNum_)
{
    d4_assertThis;
}

d4_inline void c4_Handler::Defined()
{
    d4_assertThis;
}

d4_inline void c4_Handler::FlipBytes()
{
    d4_assertThis;
}

d4_inline const c4_Property& c4_Handler::Property() const
{
    return _property;
}

d4_inline int c4_Handler::PropId() const
{
    d4_assertThis;

    return _property.GetId();
}

d4_inline c4_Column* c4_Handler::GetNthMemoCol(int index_, bool alloc_)
{
    d4_assertThis;
    d4_assert(!alloc_); // only overrides can allocate a memo item

    return 0;
}

d4_inline void c4_Handler::SetNthMemoPos(int, t4_u32, t4_u32, c4_Column*)
{
    d4_assertThis;
    d4_assert(0);   // may never be called, only if overriden
}

d4_inline bool c4_Handler::IsPersistent() const
{
    d4_assertThis;
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// c4_HandlerSeq

d4_inline int c4_HandlerSeq::Size() const
{
    d4_assertThis;
    d4_assert(_size >= 0);

    return _size;
}
    
d4_inline int c4_HandlerSeq::NumHandlers() const
{
    d4_assertThis;

    return _handlers.GetSize();
}

d4_inline c4_Handler& c4_HandlerSeq::NthHandler(int index_) const
{
    d4_assertThis;
    d4_assert(_handlers.GetAt(index_) != 0);
    
    return *(c4_Handler*) _handlers.GetAt(index_);
}

d4_inline const c4_Sequence* c4_HandlerSeq::HandlerContext(int) const
{
    d4_assertThis;
    
    return this;
}

d4_inline c4_Field& c4_HandlerSeq::Definition() const
{
    return *_field;
}

d4_inline c4_HandlerSeq& c4_HandlerSeq::Owner() const
{
    return *_owner;
}

d4_inline int c4_HandlerSeq::OwnerRow() const
{
    return _ownerRow;
}

d4_inline void c4_HandlerSeq::SetOwnerRow(int row_)
{
    _ownerRow = row_;
}

d4_inline c4_Persist* c4_HandlerSeq::Persist() const
{
    return _persist;
}

/////////////////////////////////////////////////////////////////////////////

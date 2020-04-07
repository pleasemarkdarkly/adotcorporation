//  Copyright (C) 1996-2000 Jean-Claude Wippler.  All rights reserved.
//
//  This file contains the implementation of the data handler classes.

#include "header.h"
#include "handler.h"
#include "format.h"
#include "field.h"
#include "column.h"
#include "persist.h"

#if !q4_INLINE
static char _handlerInl[] = "handler.inl";
#undef THIS_FILE
#define THIS_FILE _handlerInl
#include "handler.inl"
#endif

#if q4_CHECK
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
    
/////////////////////////////////////////////////////////////////////////////
// Implemented in this file

    class c4_Handler;

//  class c4_Sequence;
        class c4_HandlerSeq;

/////////////////////////////////////////////////////////////////////////////
// c4_Handler

    // this is how the old "Clear" was, keep it until no longer needed
void c4_Handler::ClearBytes(c4_Bytes& buf_) const
{
    int n;
    const void* p = Clear(n);
    buf_ = c4_Bytes (p, n);
}

const void* c4_Handler::Clear(int& length_) const
{
    d4_assertThis;
    
    length_ = 0;
    return "";
}

bool c4_Handler::Commit(c4_Streamer&)
{
    d4_assertThis;
	d4_assert(0);
	return false;
}

void c4_Handler::Prepare(c4_Streamer&)
{
    d4_assertThis;
	d4_assert(0);
}

    // this is how the old "Get" was, keep it until no longer needed
void c4_Handler::GetBytes(int index_, c4_Bytes& buf_, bool copySmall_)
{
    int n;
    const void* p = Get(index_, n);
    buf_ = c4_Bytes (p, n, copySmall_ && n <= 8);
}

void c4_Handler::Move(int from_, int to_)
{
    d4_assertThis;

    if (from_ != to_)
    {
        c4_Bytes data;
        GetBytes(from_, data);

        Remove(from_, 1);

        if (to_ > from_)
            --to_;

        Insert(to_, data, 1);
    }
}

/////////////////////////////////////////////////////////////////////////////
// c4_HandlerSeq

c4_HandlerSeq::c4_HandlerSeq (c4_Persist* persist_)
    : _size (0), _persist (persist_), _field (0), _owner (0), _ownerRow (0)
{                      
    d4_assertThis;
}

c4_HandlerSeq::~c4_HandlerSeq ()
{
    d4_assertThis;
    
	DetachFromParent();
    DetachFromStorage();
    
    for (int i = 0; i < NumHandlers(); ++i)
        delete & NthHandler(i);

    _handlers.SetSize(0);

    if (_propertyLimit > 0)
    {
        delete [] _propertyMap;       // property indexes may change
        _propertyLimit = 0;
    }

	if (_owner == this)
		delete _field;
}

c4_Handler* c4_HandlerSeq::CreateHandler(const c4_Property& prop_)
{
    d4_assertThis;

	return f4_CreateFormat(prop_, *this);
}

void c4_HandlerSeq::DetachFromParent()
{
    d4_assertThis;
    
	if (_field != 0)
	{
		const char* desc = "[]";
		c4_Field temp (desc);
		Restructure(temp, true);
	}

	_owner = 0;
	_ownerRow = 0; // for lack of a better value, not really needed
}

void c4_HandlerSeq::DetachFromStorage(bool partial_)
{
    d4_assertThis;

    if (_persist != 0)
	{
		int limit = partial_ ? NumFields() : 0;

			// get rid of all handlers which might do I/O
		for (int i = NumHandlers(); --i >= limit; )
		{
			c4_Handler& h = NthHandler(i);
			if (h.IsPersistent())
			{
					// all nested fields are detached recursively
				if (IsNested(i))
					for (int n = 0; n < Size(); ++n)
					{
						c4_HandlerSeq& seq = SubEntry(i, n);
						seq.DetachFromStorage();
					}

				delete &h;
				_handlers.RemoveAt(i);
			}
		}

		if (_propertyLimit > 0)
		{
			delete [] _propertyMap;       // property indexes may change
			_propertyLimit = 0;
		}

		if (!partial_)
			_persist = 0;
	}
}

void c4_HandlerSeq::SetSize(int size_)
{
    d4_assertThis;
    d4_assert(_size >= 0);

    _size = size_;
}

void c4_HandlerSeq::RemoveAt(int index_, int count_)
{
    d4_assertThis;

		// 2000-03-14, there is one case when column changes are not
		//				detected, and that's when subviews become empty
    if (_persist != 0)
		_persist->ForceCommit();

	c4_Sequence::RemoveAt(index_, count_);
}

int c4_HandlerSeq::AddHandler(c4_Handler* handler_)
{
    d4_assertThis;
    d4_assert(handler_ != 0);

    int n = _handlers.Add(handler_);
    handler_->Initialize(n);

    return n;
}

c4_Handler* c4_HandlerSeq::SwapHandler(int colNum_, c4_Handler* handler_)
{
    d4_assertThis;
    d4_assert(handler_ != 0);

    c4_Handler* h = & NthHandler(colNum_);
    _handlers.SetAt(colNum_, handler_);

    handler_->Initialize(colNum_);

    return h;
}

c4_String c4_HandlerSeq::Description() const
{
    d4_assertThis;

    return Definition().DescribeSubFields();
}

void c4_HandlerSeq::DefineRoot(const char* desc_)
{
    d4_assertThis;
    d4_assert(desc_ != 0);
    
    c4_String s = "[" + c4_String (desc_) + "]";
    desc_ = s;

    c4_Field* f = d4_new c4_Field (desc_);
    d4_assert(!*desc_);

	SetSize(1);

	delete _field;
	_field = 0;

    DefineSub(f, this, 0);
}

void c4_HandlerSeq::DefineSub(c4_Field* field_, c4_HandlerSeq* owner_, int row_)
{
    d4_assertThis;
	d4_assert(NumHandlers() == 0);
	d4_assert(_owner == 0 || _owner == owner_);
	d4_assert(_field == 0 || !_field->IsRoot());

    _owner = owner_;
    _ownerRow = row_;
    _field = field_;

    for (int i = 0; i < NumFields(); ++i)
    {
        c4_Field& field = Field(i);
        c4_Property prop (field.Type(), field.Name());
        
        int n = AddHandler(f4_CreateFormat(prop, *this));
        d4_assert(n == i);
    }
}

void c4_HandlerSeq::Restructure(c4_Field& field_, bool remove_)
{
    d4_assertThis;
    d4_assert(_field != 0);

	for (int i = 0; i < field_.NumSubFields(); ++i)
	{
        c4_Field& nf = field_.SubField(i);
        c4_Property prop (nf.Type(), nf.Name());

		int n = PropIndex(prop.GetId());
		if (n < 0)
		{
			_handlers.InsertAt(i, f4_CreateFormat(prop, *this));
		}
		else if (n != i)
		{
			d4_assert(n > i);

				// exchange the two handler positions
			c4_Handler* h = & NthHandler(n);
			h = SwapHandler(i, h);
			_handlers.SetAt(n, h);
		}
		else
			continue;

		ClearCache(); // we mess with the order of handler, keep clearing it

		d4_assert(PropIndex(prop.GetId()) == i);
	}

    c4_Field* ofld = _field;
		// special case if we're "restructuring a view out of persistence", see below
    _field = remove_ ? 0 : &field_;

        // let handler do additional init once all have been prepared
    for (int n = 0; n < NumHandlers(); ++n)
	{
		c4_Handler& h = NthHandler(n);
		h.Initialize(n);
        h.Defined();
	}

	const char* desc = "[]";
	c4_Field temp (desc);

        // all nested fields are restructured recursively
    for (int j = 0; j < NumHandlers(); ++j)
        if (IsNested(j))
            for (int n = 0; n < Size(); ++n)
			{
				c4_HandlerSeq& seq = SubEntry(j, n);
				if (j < NumFields())
					seq.Restructure(field_.SubField(j));
				else if (seq._field != 0)
					seq.Restructure(temp, true);
			}

    if (_owner == this)
        delete ofld;    // the root table owns its field structure tree
}

int c4_HandlerSeq::NumFields() const
{
    d4_assertThis;

    return _field != 0 ? _field->NumSubFields() : 0;
}

char c4_HandlerSeq::ColumnType(int index_) const
{
    d4_assertThis;

    return NthHandler(index_).Property().Type();
}

bool c4_HandlerSeq::IsNested(int index_) const
{
    d4_assertThis;

    return ColumnType(index_) == 'V';
}

c4_Field& c4_HandlerSeq::Field(int index_) const
{
    d4_assertThis;

    return Definition().SubField(index_);
}

bool c4_HandlerSeq::Commit(c4_Streamer& ar_)
{
    d4_assertThis;

    bool changed = false;

    for (int i = 0; i < NumFields(); ++i)
		if (NthHandler(i).Commit(ar_))
			changed = true;

    return changed;
}

void c4_HandlerSeq::Prepare(c4_Streamer& ar_)
{
    d4_assertThis;

    for (int i = 0; i < NumFields(); ++i)  
		NthHandler(i).Prepare(ar_);

		// let handler do additional init once all have been prepared
    for (int k = 0; k < NumFields(); ++k)
        NthHandler(k).Defined();
}

void c4_HandlerSeq::FlipAllBytes()
{
    d4_assertThis;

    for (int i = 0; i < NumHandlers(); ++i)     
    {
        c4_Handler& h = NthHandler(i);
        h.FlipBytes();
    }
}

	// New 19990903: swap rows in tables without touching the memo fields 
	// or subviews on disk.  This is used by the new c4_View::RelocateRows.

void c4_HandlerSeq::ExchangeEntries(int srcPos_, c4_HandlerSeq& dst_, int dstPos_)
{
    d4_assertThis;
	d4_assert(NumHandlers() == dst_.NumHandlers());

	c4_Bytes t1, t2;

	for (int col = 0; col < NumHandlers(); ++col)
	{
		if (IsNested(col))
		{
			d4_assert(dst_.IsNested(col));
/*
				// force both entries to exist
			SubTable(col, srcPos_);
			dst_.SubTable(col, dstPos_);

			c4_HandlerSeq*& e1 = SubEntry(col, srcPos_);
			c4_HandlerSeq*& e2 = dst_.SubEntry(col, dstPos_);
			d4_assert(e1 != 0 && e2 != 0);

				// swap the two entries
			c4_HandlerSeq* e = e1;
			e1 = e2;
			e2 = e;
*/
				// shorthand, *after* the swap
			c4_HandlerSeq& t1 = SubEntry(col, srcPos_);
			c4_HandlerSeq& t2 = dst_.SubEntry(col, dstPos_);

				// adjust the parents
			t1._owner = this;
			t2._owner = &dst_;

				// adjust the parent row numbers
			t1.SetOwnerRow(srcPos_);
			t2.SetOwnerRow(dstPos_);

				// reattach the proper field structures
			t1.Restructure(Field(col));
			t2.Restructure(dst_.Field(col));
		}
		else
		{
			d4_assert(ColumnType(col) == dst_.ColumnType(col));

			c4_Handler& h1 = NthHandler(col);
			c4_Handler& h2 = dst_.NthHandler(col);

			if (ColumnType(col) == 'M')
			{
				c4_Column* c1 = h1.GetNthMemoCol(srcPos_, true);
				c4_Column* c2 = h2.GetNthMemoCol(dstPos_, true);

				t4_u32 p1 = c1 ? c1->Position() : 0;
				t4_u32 p2 = c2 ? c2->Position() : 0;

				t4_u32 s1 = c1 ? c1->ColSize() : 0;
				t4_u32 s2 = c2 ? c2->ColSize() : 0;

				h1.SetNthMemoPos(srcPos_, p2, s2, c2);
				h2.SetNthMemoPos(dstPos_, p1, s1, c1);
			}
			else
			{
				h1.GetBytes(srcPos_, t1);
				h2.GetBytes(dstPos_, t2);
				
				h1.Set(srcPos_, t2);
				h2.Set(dstPos_, t1);
			}
		}
	}
}

c4_HandlerSeq& c4_HandlerSeq::SubEntry(int col_, int row_) const
{
    d4_assertThis;
    d4_assert(IsNested(col_));

	c4_Bytes temp;
	NthHandler(col_).GetBytes(row_, temp);

	d4_assert(temp.Size() == sizeof (c4_HandlerSeq**));
	c4_HandlerSeq** p = (c4_HandlerSeq**) temp.Contents();

	d4_assert(p != 0 && *p != 0);

	return **p;
}

/////////////////////////////////////////////////////////////////////////////

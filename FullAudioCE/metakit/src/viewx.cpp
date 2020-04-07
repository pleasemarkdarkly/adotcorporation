//  Copyright (C) 1996-2000 Jean-Claude Wippler.  All rights reserved.
//
//  This file contains the implementation of the auxiliary view classes.

#include "header.h"
#include "handler.h"
#include "store.h"
#include "column.h"

#if q4_CHECK
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Implemented in this file

    class c4_Sequence;

    class c4_Reference;
        class c4_IntRef;
        class c4_FloatRef;
        class c4_DoubleRef;
        class c4_MemoRef;
        class c4_BytesRef;
        class c4_StringRef;
        class c4_ViewRef;
   
    class c4_Stream;
    class c4_Strategy;

/////////////////////////////////////////////////////////////////////////////
// c4_Sequence

c4_Sequence::c4_Sequence ()
    : _refCount (0), _dependencies (0), _propertyLimit (0), _tempBuf (0)
{                      
    d4_assertThis;
}

c4_Sequence::~c4_Sequence ()
{
    d4_assertThis;
    d4_assert(_refCount == 0);

    d4_assert(!_dependencies); // there can be no dependencies left

    ClearCache();

	delete _tempBuf;
}

void c4_Sequence::IncRef()
{
    d4_assertThis;

    ++_refCount;
    
    d4_assert(_refCount != 0);
}

void c4_Sequence::DecRef()
{
    d4_assertThis;
    d4_assert(_refCount != 0);

    if (--_refCount == 0)
        delete this;
}

int c4_Sequence::NumRefs() const
{
    d4_assertThis;

    return _refCount;
}

int c4_Sequence::Compare(int index_, c4_Cursor cursor_) const
{
    d4_assertThis;
    d4_assert(cursor_._seq != 0);
    
    c4_Bytes data;

    for (int colNum = 0; colNum < NumHandlers(); ++colNum)
    {
        c4_Handler& h = NthHandler(colNum);

        const c4_Sequence* hc = HandlerContext(colNum);
        int i = RemapIndex(index_, hc);

        if (!cursor_._seq->Get(cursor_._index, h.PropId(), data))
            h.ClearBytes(data);
                
        int f = h.Compare(i, data);
        if (f != 0)
            return f;
    }                
    
    return 0;
}

void c4_Sequence::SetAt(int index_, c4_Cursor newElem_)
{
    d4_assertThis;
    d4_assert(newElem_._seq != 0);

    c4_Bytes data;

    c4_Notifier change (this);
    if (GetDependencies())
        change.StartSetAt(index_, newElem_);

    for (int i = 0; i < newElem_._seq->NumHandlers(); ++i)
    {
        c4_Handler& h = newElem_._seq->NthHandler(i);

			// added 06-12-1999 to do index remapping for derived seq's
        const c4_Sequence* hc = newElem_._seq->HandlerContext(i);
        int ri = newElem_._seq->RemapIndex(newElem_._index, hc);

        h.GetBytes(ri, data);

//      Set(index_, cursor._seq->NthProperty(i), data);
        int colNum = PropIndex(h.Property());
        d4_assert(colNum >= 0);

        NthHandler(colNum).Set(index_, data);
    }

        // if number of props in dest is larger after adding, clear the rest
        // this way, new props get copied and undefined props get cleared
    if (newElem_._seq->NumHandlers() < NumHandlers())
    {
        for (int i = 0; i < NumHandlers(); ++i)
        {
            c4_Handler& h = NthHandler(i);

                // if the property does not appear in the source
            if (newElem_._seq->PropIndex(h.PropId()) < 0)
            {
                h.ClearBytes(data);
                h.Set(index_, data);
            }
        }
    }
}

int c4_Sequence::RemapIndex(int index_, const c4_Sequence* seq_) const
{
    d4_assertThis;
    
    return seq_ == this ? index_ : -1;
}

c4_Bytes& c4_Sequence::Buffer()
{
	if (_tempBuf == 0)
		_tempBuf = new c4_Bytes;
	return *_tempBuf;
}

    // 1.8.5: extra buffer to hold returned description strings
const char* c4_Sequence::UseTempBuffer(const char* str_)
{
    return strcpy((char*) Buffer().SetBuffer(strlen(str_) + 1), str_);
}

const char* c4_Sequence::Describe()
{
    d4_assertThis;

    c4_String result;

    for (int i = 0; i < NumHandlers(); ++i)
    {
        const c4_Property& prop = NthHandler(i).Property();

        c4_String desc = ':';
        desc += prop.Type();

        if (prop.Type() == 'V')
        {       // if there are no rows then we cannot determine the
                // structure of a view property, so skip it completely
            if (Size() == 0)
                continue;

                // create a reference to row 0 of this sequence
            c4_Cursor cursor (* (c4_Sequence*) this, 0); // loses const
            c4_ViewRef ref = prop (*cursor);
            c4_View view = ref;

                // now recursively get a description for the subview
            desc = view.Describe();
            if (desc.IsEmpty())
                continue;
        }

        result += ",";
        result += prop.Name() + desc;
    }

    if (result.IsEmpty())
        return "";
    
    return UseTempBuffer("[" + result.Mid(1) + "]");
}

void c4_Sequence::Resize(int newSize_, int growBy_)
{
    d4_assertThis;

    if (NumHandlers() > 0)
    {
        int diff = newSize_ - Size();

        if (diff > 0)
        {
            c4_Row empty; // make sure this doesn't recurse, see below
            InsertAt(Size(), &empty, diff);
        }
        else if (diff < 0)
            RemoveAt(newSize_, - diff);
    }
    else    // need special case to avoid recursion for c4_Row allocations
        SetSize(newSize_);
}
    
void c4_Sequence::InsertAt(int index_, c4_Cursor newElem_, int count_)
{
    d4_assertThis;
    d4_assert(newElem_._seq != 0);

    c4_Notifier change (this);
    if (GetDependencies())
        change.StartInsertAt(index_, newElem_, count_);

    SetSize(Size() + count_);

    c4_Bytes data;

    for (int i = 0; i < newElem_._seq->NumHandlers(); ++i)
    {
        c4_Handler& h = newElem_._seq->NthHandler(i);

			// added 06-12-1999 to do index remapping for derived seq's
        const c4_Sequence* hc = newElem_._seq->HandlerContext(i);
        int ri = newElem_._seq->RemapIndex(newElem_._index, hc);

        h.GetBytes(ri, data);

        int colNum = PropIndex(h.Property());
        d4_assert(colNum >= 0);

		if (h.Property().Type() == 'V')
		{
				// special treatment for subviews, insert empty, then overwrite
				// changed 19990904 - probably fixes a long-standing limitation
			c4_Bytes temp;
			h.ClearBytes(temp);

			c4_Handler& h2 = NthHandler(colNum);
			h2.Insert(index_, temp, count_);
			for (int j = 0; j < count_; ++j)
				h2.Set(index_ + j, data);
		}
		else
			NthHandler(colNum).Insert(index_, data, count_);
    }

        // if number of props in dest is larger after adding, clear the rest
        // this way, new props get copied and undefined props get cleared
    if (newElem_._seq->NumHandlers() < NumHandlers())
    {
        for (int i = 0; i < NumHandlers(); ++i)
        {
            c4_Handler& h = NthHandler(i);

                // if the property does not appear in the source
            if (newElem_._seq->PropIndex(h.PropId()) < 0)
            {
                h.ClearBytes(data);
                h.Insert(index_, data, count_);
            }
        }
    }
}

void c4_Sequence::RemoveAt(int index_, int count_)
{
    d4_assertThis;

    c4_Notifier change (this);
    if (GetDependencies())
        change.StartRemoveAt(index_, count_);

    SetSize(Size() - count_);

        //! careful, this does no index remapping, wrong for derived seq's
    for (int i = 0; i < NumHandlers(); ++i)
        NthHandler(i).Remove(index_, count_);
}

void c4_Sequence::Move(int from_, int to_)
{
    d4_assertThis;

    c4_Notifier change (this);
    if (GetDependencies())
        change.StartMove(from_, to_);

        //! careful, this does no index remapping, wrong for derived seq's
    for (int i = 0; i < NumHandlers(); ++i)
        NthHandler(i).Move(from_, to_);
}

int c4_Sequence::NthPropId(int index_) const
{
    d4_assertThis;

    return NthHandler(index_).PropId();
}

void c4_Sequence::ClearCache()
{
    if (_propertyLimit > 0)
    {
        delete [] _propertyMap;       // property indexes may change
        _propertyLimit = 0;
    }
}

int c4_Sequence::PropIndex(int propId_)
{
    d4_assertThis;

//! CACHING NOTE:   derived views will fail if underlying view is restructured
//                  still, this cache is kept, since sort will fail anyway...
//  The only safe change in these cases is adding new properties at the end.

        // use the map for the fastest result once known
    if (propId_ < _propertyLimit && _propertyMap[propId_] >= 0)
        return _propertyMap[propId_];

        // locate the property using a linear search, return if not present
    int n = NumHandlers();
    do
        if (--n < 0)
            return -1;
    while (NthPropId(n) != propId_);

        // if the map is too small, resize it (with a little slack)
    if (propId_ >= _propertyLimit)
    {
        int round = (propId_ + 8) & ~0x07;
        short* vec = d4_new short [round];

        for (int i = 0; i < round; ++i)
            vec[i] = i < _propertyLimit ? _propertyMap[i] : -1;
        
        if (_propertyLimit > 0)
            delete [] _propertyMap;

        _propertyMap = vec;
        _propertyLimit = round;
    }

        // we have a map, adjust the entry and return
    return _propertyMap[propId_] = n;
}

int c4_Sequence::PropIndex(const c4_Property& prop_)
{
    d4_assertThis;

    int pos = PropIndex(prop_.GetId());
    if (pos >= 0)
    {
        d4_assert(NthHandler(pos).Property().Type() == prop_.Type());
        return pos;
    }

    c4_Handler* h = CreateHandler(prop_);
    d4_assert(h != 0);

    int i = AddHandler(h);
    if (i >= 0 && Size() > 0)
    {
        c4_Bytes data;
        h->ClearBytes(data);
        h->Insert(0, data, Size());
    }

    return i;
}

int c4_Sequence::ItemSize(int index_, int propId_)
{
    d4_assertThis;

    int colNum = PropIndex(propId_);
    return colNum >= 0 ? NthHandler(colNum).ItemSize(index_) : -1;
}

bool c4_Sequence::Get(int index_, int propId_, c4_Bytes& buf_)
{
    d4_assertThis;

    int colNum = PropIndex(propId_);
    if (colNum < 0)
        return false;

    NthHandler(colNum).GetBytes(index_, buf_);
    return true;
}

void c4_Sequence::Set(int index_, const c4_Property& prop_, const c4_Bytes& buf_)
{
    d4_assertThis;
    
    int colNum = PropIndex(prop_);
    d4_assert(colNum >= 0);

    c4_Handler& h = NthHandler(colNum);

    c4_Notifier change (this);
    if (GetDependencies())
        change.StartSet(index_, prop_.GetId(), buf_);

    if (buf_.Size())
        h.Set(index_, buf_);
    else
    {
        c4_Bytes empty;
        h.ClearBytes(empty);
        h.Set(index_, empty);
    }
}

void c4_Sequence::Attach(c4_Sequence* child_)
{
    d4_assertThis;
    
    IncRef();

    if (!_dependencies)
        _dependencies = d4_new c4_Dependencies;

    _dependencies->Add(child_);
}

void c4_Sequence::Detach(c4_Sequence* child_)
{
    d4_assertThis;
    d4_assert(_dependencies != 0);
    
    if (!_dependencies->Remove(child_))
    {
        delete _dependencies;
        _dependencies = 0;
    }

    DecRef();
}

c4_Notifier* c4_Sequence::PreChange(c4_Notifier& nf_)
{
    d4_assertThis;
    d4_assert(0); // should not be called, because it should not attach
    return 0;
}

void c4_Sequence::PostChange(c4_Notifier& nf_)
{
    d4_assertThis;
}

/////////////////////////////////////////////////////////////////////////////
// c4_Reference

c4_Reference& c4_Reference::operator= (const c4_Reference& value_)
{            
    d4_assertThis;
    
    c4_Bytes result;
    value_.GetData(result);
    SetData(result);

    return *this;
}

bool operator== (const c4_Reference& a_, const c4_Reference& b_)
{
    c4_Bytes buf1;
    bool f1 = a_.GetData(buf1);

    c4_Bytes buf2;
    bool f2 = b_.GetData(buf2);

        // if absent, fill either with zero bytes to match length
    if (!f1)
        buf1.SetBufferClear(buf2.Size());
    if (!f2)
        buf2.SetBufferClear(buf1.Size());

    return buf1 == buf2;
}

/////////////////////////////////////////////////////////////////////////////
// c4_IntRef

c4_IntRef::operator t4_i32 () const
{
    d4_assertThis;
    
    c4_Bytes result;
    if (!GetData(result))
        return 0;

    d4_assert(result.Size() == sizeof (t4_i32));
    return *(const t4_i32*) result.Contents();
}
    
c4_IntRef& c4_IntRef::operator= (t4_i32 value_)
{
    d4_assertThis;
    
    SetData(c4_Bytes (&value_, sizeof value_));
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
#if !q4_TINY
/////////////////////////////////////////////////////////////////////////////
// c4_FloatRef

c4_FloatRef::operator double () const
{
    d4_assertThis;
    
    c4_Bytes result;
    if (!GetData(result))
        return 0;

    d4_assert(result.Size() == sizeof (float));
    return *(const float*) result.Contents();
}
    
c4_FloatRef& c4_FloatRef::operator= (double value_)
{
    d4_assertThis;
    
    float v = (float) value_; // loses precision
    SetData(c4_Bytes (&v, sizeof v));
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// c4_DoubleRef

c4_DoubleRef::operator double () const
{
    d4_assertThis;
    
    c4_Bytes result;
    if (!GetData(result))
        return 0;

    d4_assert(result.Size() == sizeof (double));
    return *(const double*) result.Contents();
}
    
c4_DoubleRef& c4_DoubleRef::operator= (double value_)
{
    d4_assertThis;
    
    SetData(c4_Bytes (&value_, sizeof value_));
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// c4_MemoRef

c4_MemoRef::operator c4_Bytes () const
{
    d4_assertThis;
    
    c4_Bytes result;
    GetData(result);

        // the result must immediately be used, its lifetime may be limited
    return result;
}

c4_MemoRef& c4_MemoRef::operator= (const c4_Bytes& value_)
{
    d4_assertThis;
    
    SetData(value_);
    return *this;
}

c4_Bytes c4_MemoRef::Access(t4_u32 off_, int len_) const
{
    d4_assertThis;
    
    int colNum = _cursor._seq->PropIndex(_property.GetId());
    if (colNum >= 0)
	{
		c4_Handler& h = _cursor._seq->NthHandler(colNum);
		c4_Column* col = h.GetNthMemoCol(_cursor._index);
		if (col != 0)
		{
			if (len_ == 0 || off_ + len_ > col->ColSize())
				len_ = col->ColSize() - off_;

			if (len_ > 0) {
				c4_ColIter iter (*col, off_, off_ + len_);
				iter.Next();

				return c4_Bytes (iter.BufLoad(), iter.BufLen()); // no copying
			}
		}
	}

	return c4_Bytes ();
}

bool c4_MemoRef::Modify(const c4_Bytes& buf_, t4_u32 off_, int diff_) const
{
    d4_assertThis;
    
    int colNum = _cursor._seq->PropIndex(_property.GetId());
    if (colNum >= 0)
	{
		c4_Handler& h = _cursor._seq->NthHandler(colNum);
		c4_Column* col = h.GetNthMemoCol(_cursor._index, true);
		if (col != 0)
		{
			const t4_byte* p = buf_.Contents();
			const int n = buf_.Size();

			const t4_u32 limit = off_ + n; // past changed bytes
			const t4_i32 overshoot = limit - col->ColSize();

			if (diff_ < overshoot)
				diff_ = overshoot;

			if (diff_ < 0)
				col->Shrink(limit, - diff_);
			else if (diff_ > 0)
					// insert bytes in the highest possible spot
					// if a gap is created, it will contain garbage
				col->Grow(overshoot > 0 ? col->ColSize() :
							 diff_ > n ? off_ : limit - diff_, diff_);

			d4_assert(col->ColSize() >= off_ + n);

			c4_ColIter iter (*col, off_, limit);
			while (iter.Next())
			{
				memcpy(iter.BufSave(), p, iter.BufLen());
				p += iter.BufLen();
			}

			return true;
		}
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
#endif // !q4_TINY
/////////////////////////////////////////////////////////////////////////////
// c4_BytesRef

c4_BytesRef::operator c4_Bytes () const
{
    d4_assertThis;
    
    c4_Bytes result;
    GetData(result);

        // the result must immediately be used, its lifetime may be limited
    return result;
}

c4_BytesRef& c4_BytesRef::operator= (const c4_Bytes& value_)
{
    d4_assertThis;
    
    SetData(value_);
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// c4_StringRef

c4_StringRef::operator const char* () const
{
    d4_assertThis;
    
    c4_Bytes result;
    GetData(result);

    return result.Size() > 0 ? (const char*) result.Contents() : "";
}

c4_StringRef& c4_StringRef::operator= (const char* value_)
{
    d4_assertThis;
    
    SetData(c4_Bytes (value_, strlen(value_) + 1));
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// c4_ViewRef

c4_ViewRef::operator c4_View () const
{
    d4_assertThis;

    c4_Bytes result;
    if (!GetData(result))
        return (c4_Sequence*) 0; // resolve ambiguity

    d4_assert(result.Size() == sizeof (c4_Sequence*));
    return *(c4_Sequence* const*) result.Contents();
}

c4_ViewRef& c4_ViewRef::operator= (const c4_View& value_)
{
    d4_assertThis;

    SetData(c4_Bytes (&value_._seq, sizeof value_._seq));
    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// c4_Stream

c4_Stream::~c4_Stream ()
{
}

/////////////////////////////////////////////////////////////////////////////
// c4_Strategy

c4_Strategy::c4_Strategy ()
    : _keepAfterCommit (false), _bytesFlipped (false), _failure (0), 
      _mapStart (0), _mapLimit (0)
{
}

c4_Strategy::~c4_Strategy ()
{
    d4_assertThis;

	d4_assert(_mapStart == 0 && _mapLimit == 0);
}

void c4_Strategy::DataSeek(t4_i32 position_)
{
    d4_assertThis;
}

int c4_Strategy::DataRead(void* buffer_, int length_)
{
    d4_assertThis;

	++_failure;
	return -1;
}

bool c4_Strategy::DataWrite(const void* buffer_, int length_)
{
    d4_assertThis;

	++_failure;
	return false;
}

void c4_Strategy::DataCommit(t4_i32 newSize_)
{
    d4_assertThis;
}

void c4_Strategy::ResetFileMapping()
{
    d4_assertThis;
}

/////////////////////////////////////////////////////////////////////////////

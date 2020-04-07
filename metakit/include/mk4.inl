//........................................................................................
//........................................................................................
//.. File Name: mk4.inl
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
//  This file contains inline definitions for the view classes.
//      
//  Only very small members are selected for inline code.
//  See the class listing for a description of these functions.

/////////////////////////////////////////////////////////////////////////////
// Reordered inlines so they are always defined before their first use
 
d4_inline c4_Cursor c4_RowRef::operator& () const
{
    return _cursor;
}

d4_inline int c4_Property::GetId() const
{
    return _id;
}

//////////////////////////////////////////////////////////////////////////////////
// c4_View

    //@mfunc Returns the number of entries in this view.
    //@rdesc A non-negative integer.
d4_inline int c4_View::GetSize() const
{
    return _seq->Size();
}

    //@mfunc Returns the highest index in this view (same as: GetSize() - 1).
    //@rdesc One less than the number of entries.
    // If there are no entries, the value -1 is returned.
d4_inline int c4_View::GetUpperBound() const
{
    return GetSize() - 1;
}

    //@mfunc Changes the size of this view.
    //@parm the new size of this view
    //@parmopt int | growBy_ | -1 | the granularity of size increases (not used)
d4_inline void c4_View::SetSize(int newSize_, int growBy_)
{
    _seq->Resize(newSize_, growBy_);
}

    //@mfunc Removes all entries (sets size to zero).
d4_inline void c4_View::RemoveAll()
{
    SetSize(0);
}

    //@mfunc Changes the value of the specified entry. If the new value has
    // other properties, these will be added to the underlying view.
    //@parm the zero-based row index
    //@parm the row to copy to this view
d4_inline void c4_View::SetAt(int index_, const c4_RowRef& newElem_)
{
    _seq->SetAt(index_, &newElem_);
}

    //@mfunc Inserts one or more copies of an entry. This is identical to
    // inserting the specified number of default entries and then setting
    // each of them to the new element value passed as argument.
    //@parm the zero-based row index
    //@parm the value to insert
    //@parmopt int | count_ | 1 | the number of entries to insert
d4_inline void c4_View::InsertAt(int index_, const c4_RowRef& newElem_, int count_)
{
    _seq->InsertAt(index_, &newElem_, count_);
}

    //@mfunc Removes entries starting at the given index. Entries which have
    // other view references may cause these views to be deleted if their
    // reference counts drop to zero because of this removal.
    //@parm the zero-based row index
    //@parmopt int | count_ | 1 | the number of entries to remove
d4_inline void c4_View::RemoveAt(int index_, int count_)
{
    _seq->RemoveAt(index_, count_);
}

    //@mfunc Returns the number of properties present in this view.
    //@rdesc A non-negative integer.
d4_inline int c4_View::NumProperties() const
{
    return _seq->NumHandlers();
}

    //@mfunc Finds a property, given its id, as returned by NthProperty().
    //@parm Unique id associated to a specific propoerty
    //@rdesc The index of the property, or -1 of it was not found.
d4_inline int c4_View::FindProperty(int propId_)
{
    return _seq->PropIndex(propId_);
}

d4_inline const char* c4_View::Describe() const
{
    return _seq->Describe();
}

d4_inline void c4_View::_IncSeqRef()
{
    _seq->IncRef();
}

d4_inline void c4_View::_DecSeqRef()
{
    _seq->DecRef();
}

    //@mfunc Destructor.
d4_inline c4_View::~c4_View ()
{
    _DecSeqRef();
}

d4_inline bool operator== (const c4_View& a_, const c4_View& b_)
{
    return a_.GetSize() == b_.GetSize() && a_.Compare(b_) == 0;
}

d4_inline bool operator!= (const c4_View& a_, const c4_View& b_)
{
    return !(a_ == b_);
}

d4_inline bool operator< (const c4_View& a_, const c4_View& b_)
{
    return a_.Compare(b_) < 0;
}

d4_inline bool operator> (const c4_View& a_, const c4_View& b_)
{
    return b_ < a_;
}

d4_inline bool operator<= (const c4_View& a_, const c4_View& b_)
{
    return !(b_ < a_);
}

d4_inline bool operator>= (const c4_View& a_, const c4_View& b_)
{                     
    return !(a_ < b_);
}

/////////////////////////////////////////////////////////////////////////////
// c4_Cursor

    //@mfunc Constructs a new cursor.
    //
    // Cursor cannot be created without an underlying view, but you could
    // define a global "nullView" object and then initialize the cursor with
    // "&nullView[0]". This works because cursors need not point to a valid row.
    //
    //@parm the underlying sequence
    //@parm the position in that sequence
d4_inline c4_Cursor::c4_Cursor (c4_Sequence& seq_, int index_)
    : _seq (&seq_), _index (index_)
{
}

    //@mfunc Pre-increments the cursor.
    //@rdesc The new cursor, after having been incremented.
d4_inline c4_Cursor& c4_Cursor::operator++ ()
{
    ++_index;
    return *this;
}

    //@mfunc Post-increments the cursor.
    //@parm Dummy arg.
    //@rdesc The cursor, as it was before being incremented.
d4_inline c4_Cursor c4_Cursor::operator++ (int)
{
    return c4_Cursor (*_seq, _index++);
}

    //@mfunc Pre-decrements the cursor.
    //@rdesc The new cursor, after having been decremented.
d4_inline c4_Cursor& c4_Cursor::operator-- ()
{
    --_index;
    return *this;
}

    //@mfunc Post-decrements the cursor.
    //@parm Dummy arg.
    //@rdesc The cursor, as it was before being decremented.
d4_inline c4_Cursor c4_Cursor::operator-- (int)
{
    return c4_Cursor (*_seq, _index--);
}

    //@mfunc Advances by a given offset.
    //@rdesc The new cursor, after it has been moved forward.
d4_inline c4_Cursor& c4_Cursor::operator+= (int offset_)
{
    _index += offset_;
    return *this;
}

    //@mfunc Backs up by a given offset.
    //@rdesc The new cursor, after it has been moved back.
d4_inline c4_Cursor& c4_Cursor::operator-= (int offset_)
{
    _index -= offset_;
    return *this;
}

    //@mfunc Subtracts a specified offset.
    //@rdesc The new cursor, after it has been decreased.
d4_inline c4_Cursor c4_Cursor::operator- (int offset_) const
{
    return c4_Cursor (*_seq, _index - offset_);
}

    //@mfunc Returns the distance between two cursors.
    //@rdesc The index of the first minus the index of the second.
d4_inline int c4_Cursor::operator- (c4_Cursor cursor_) const
{
    return _index - cursor_._index;
}

    //@func Add a specified offset.
    //@rdesc A new cursor, pointing to the old plus the specified offset.
d4_inline c4_Cursor operator+ (c4_Cursor cursor_, int offset_)
{
    return c4_Cursor (*cursor_._seq, cursor_._index + offset_);
}

    //@func Adds specified offset to cursor.
    //@rdesc A new cursor, pointing to the old plus the specified offset.
d4_inline c4_Cursor operator+ (int offset_, c4_Cursor cursor_)
{
    return cursor_ + offset_;
}

d4_inline bool operator== (c4_Cursor a_, c4_Cursor b_)
{
    return a_._seq == b_._seq && a_._index == b_._index;
}

d4_inline bool operator!= (c4_Cursor a_, c4_Cursor b_)
{
    return !(a_ == b_);
}

d4_inline bool operator< (c4_Cursor a_, c4_Cursor b_)
{
    return a_._seq < b_._seq ||
            a_._seq == b_._seq && a_._index < b_._index;
}

d4_inline bool operator> (c4_Cursor a_, c4_Cursor b_)
{
    return b_ < a_;
}

d4_inline bool operator<= (c4_Cursor a_, c4_Cursor b_)
{
    return !(b_ < a_);
}

d4_inline bool operator>= (c4_Cursor a_, c4_Cursor b_)
{                     
    return !(a_ < b_);
}

/////////////////////////////////////////////////////////////////////////////
// c4_RowRef

d4_inline c4_RowRef::c4_RowRef (c4_Cursor cursor_)
    : _cursor (cursor_)
{
}

d4_inline c4_RowRef c4_RowRef::operator= (const c4_RowRef& rowRef_)
{
    if (_cursor != rowRef_._cursor)
        _cursor._seq->SetAt(_cursor._index, &rowRef_);
    
    return *this;
}

d4_inline c4_View c4_RowRef::Container() const
{
    return _cursor._seq;
}

d4_inline bool operator== (const c4_RowRef& a_, const c4_RowRef& b_)
{
    return (&a_)._seq->Compare((&a_)._index, &b_) == 0;
}               

d4_inline bool operator!= (const c4_RowRef& a_, const c4_RowRef& b_)
{
    return !(a_ == b_);
}

d4_inline bool operator< (const c4_RowRef& a_, const c4_RowRef& b_)
{
        // 25-5-1998: don't exchange a and b, this comparison is -not- symmetric
    return (&a_)._seq->Compare((&a_)._index, &b_) < 0;
}               

d4_inline bool operator> (const c4_RowRef& a_, const c4_RowRef& b_)
{
        // 25-5-1998: don't exchange a and b, this comparison is -not- symmetric
    return (&a_)._seq->Compare((&a_)._index, &b_) > 0;
}

d4_inline bool operator<= (const c4_RowRef& a_, const c4_RowRef& b_)
{
    return !(a_ > b_);
}

d4_inline bool operator>= (const c4_RowRef& a_, const c4_RowRef& b_)
{                     
    return !(a_ < b_);
}

/////////////////////////////////////////////////////////////////////////////
// c4_Bytes

d4_inline c4_Bytes::c4_Bytes ()
    : _contents (0), _size (0), _copy (false)
{
}

d4_inline c4_Bytes::c4_Bytes (const void* buf_, int len_)
    : _contents ((t4_byte*) buf_), _size (len_), _copy (false)
{
}

    //@mfunc Returns a pointer to the contents.
    //@rdesc Pointer to contents.
d4_inline const t4_byte* c4_Bytes::Contents() const
{
    return _contents;
}

    //@mfunc Returns the number of bytes of its contents.
    //@rdesc Length in bytes.
d4_inline int c4_Bytes::Size() const
{
    return _size;
}

d4_inline void c4_Bytes::_LoseCopy()
{
    if (_copy)
        delete [] (char*) _contents;
}

    //@func Returns true if the contents of both objects is not equal.
    //@rdesc False if both objects are equal (in size and byte-for-byte).
d4_inline bool operator!= (const c4_Bytes& a_, const c4_Bytes& b_)
{
    return !(a_ == b_);
}

    //@mfunc Destructor.
    // If a copy was made, it will be released here.
d4_inline c4_Bytes::~c4_Bytes ()
{
    _LoseCopy();
}
    
/////////////////////////////////////////////////////////////////////////////
// c4_Reference

d4_inline c4_Reference::c4_Reference (const c4_RowRef& rowRef_,
                                        const c4_Property& prop_)
    : _cursor (&rowRef_), _property (prop_)
{
}

d4_inline int c4_Reference::GetSize() const
{
    return _cursor._seq->ItemSize(_cursor._index, _property.GetId());
}

d4_inline bool c4_Reference::GetData(c4_Bytes& buf_) const
{
    return _cursor._seq->Get(_cursor._index, _property.GetId(), buf_);
}

d4_inline void c4_Reference::SetData(const c4_Bytes& buf_) const
{
    _cursor._seq->Set(_cursor._index, _property, buf_);
}

d4_inline bool operator!= (const c4_Reference& a_, const c4_Reference& b_)
{
    return !(a_ == b_);
}

/////////////////////////////////////////////////////////////////////////////
// c4_IntRef

d4_inline c4_IntRef::c4_IntRef (const c4_Reference& value_)
    : c4_Reference (value_)
{
}

/////////////////////////////////////////////////////////////////////////////
#if !q4_TINY
/////////////////////////////////////////////////////////////////////////////
// c4_FloatRef

d4_inline c4_FloatRef::c4_FloatRef (const c4_Reference& value_)
    : c4_Reference (value_)
{
}

/////////////////////////////////////////////////////////////////////////////
// c4_DoubleRef

d4_inline c4_DoubleRef::c4_DoubleRef (const c4_Reference& value_)
    : c4_Reference (value_)
{
}

/////////////////////////////////////////////////////////////////////////////
// c4_MemoRef

d4_inline c4_MemoRef::c4_MemoRef (const c4_Reference& value_)
    : c4_Reference (value_)
{
}

/////////////////////////////////////////////////////////////////////////////
#endif // !q4_TINY
/////////////////////////////////////////////////////////////////////////////
// c4_BytesRef

d4_inline c4_BytesRef::c4_BytesRef (const c4_Reference& value_)
    : c4_Reference (value_)
{
}

/////////////////////////////////////////////////////////////////////////////
// c4_StringRef

d4_inline c4_StringRef::c4_StringRef (const c4_Reference& value_)
    : c4_Reference (value_)
{
}

/////////////////////////////////////////////////////////////////////////////
// c4_ViewRef

d4_inline c4_ViewRef::c4_ViewRef (const c4_Reference& value_)
    : c4_Reference (value_)
{
}

/////////////////////////////////////////////////////////////////////////////
// c4_Property

d4_inline c4_Property::c4_Property (char type_, int id_)
    : _id (id_), _type (type_)
{
}

d4_inline c4_Reference c4_Property::operator() (const c4_RowRef& rowRef_) const
{
    return c4_Reference (rowRef_, *this);
}

d4_inline c4_View c4_Property::operator, (const c4_Property& prop_) const
{
    return c4_View (*this), prop_;
}

d4_inline char c4_Property::Type() const
{
    return _type;
}

/////////////////////////////////////////////////////////////////////////////
// c4_IntProp

    //@mfunc Constructs a new integer property.
    //@parm the name of this property
d4_inline c4_IntProp::c4_IntProp (const char* name_) 
    : c4_Property ('I', name_)
{
}

d4_inline c4_IntRef c4_IntProp::operator() (const c4_RowRef& rowRef_) const
{
    return c4_Reference (rowRef_, *this);
}

d4_inline t4_i32 c4_IntProp::Get(const c4_RowRef& rowRef_) const
{
    return operator() (rowRef_);
}

d4_inline void c4_IntProp::Set(const c4_RowRef& rowRef_, t4_i32 value_) const
{
    operator() (rowRef_) = value_;
}

d4_inline c4_Row c4_IntProp::AsRow(t4_i32 value_) const
{
    c4_Row row;
    operator() (row) = value_;
    return row;
}
    
d4_inline c4_Row c4_IntProp::operator[] (t4_i32 value_) const
{
    return AsRow(value_);
}
    
/////////////////////////////////////////////////////////////////////////////
#if !q4_TINY
/////////////////////////////////////////////////////////////////////////////
// c4_FloatProp

    //@mfunc Constructs a new floating point property.
    //@parm the name of this property
d4_inline c4_FloatProp::c4_FloatProp (const char* name_) 
    : c4_Property ('F', name_)
{
}

d4_inline c4_FloatRef c4_FloatProp::operator() (const c4_RowRef& rowRef_) const
{
    return c4_Reference (rowRef_, *this);
}

d4_inline double c4_FloatProp::Get(const c4_RowRef& rowRef_) const
{
    return operator() (rowRef_);
}

d4_inline void c4_FloatProp::Set(const c4_RowRef& rowRef_, double value_) const
{
    operator() (rowRef_) = value_;
}

d4_inline c4_Row c4_FloatProp::AsRow(double value_) const
{
    c4_Row row;
    operator() (row) = value_;
    return row;
}
    
d4_inline c4_Row c4_FloatProp::operator[] (double value_) const
{
    return AsRow(value_);
}
    
/////////////////////////////////////////////////////////////////////////////
// c4_DoubleProp

    //@mfunc Constructs a new double precision property.
    //@parm the name of this property
d4_inline c4_DoubleProp::c4_DoubleProp (const char* name_) 
    : c4_Property ('D', name_)
{
}

d4_inline c4_DoubleRef c4_DoubleProp::operator() (const c4_RowRef& rowRef_) const
{
    return c4_Reference (rowRef_, *this);
}

d4_inline double c4_DoubleProp::Get(const c4_RowRef& rowRef_) const
{
    return operator() (rowRef_);
}

d4_inline void c4_DoubleProp::Set(const c4_RowRef& rowRef_, double value_) const
{
    operator() (rowRef_) = value_;
}

d4_inline c4_Row c4_DoubleProp::AsRow(double value_) const
{
    c4_Row row;
    operator() (row) = value_;
    return row;
}
    
d4_inline c4_Row c4_DoubleProp::operator[] (double value_) const
{
    return AsRow(value_);
}
    
/////////////////////////////////////////////////////////////////////////////
// c4_MemoProp

    //@mfunc Constructs a new memo property.
    //@parm the name of this property
d4_inline c4_MemoProp::c4_MemoProp (const char* name_) 
    : c4_Property ('M', name_)
{
}
    
d4_inline c4_MemoRef c4_MemoProp::operator() (const c4_RowRef& rowRef_) const
{
    return c4_Reference (rowRef_, *this);
}

d4_inline c4_Bytes c4_MemoProp::Get(const c4_RowRef& rowRef_) const
{
    return operator() (rowRef_);
}

d4_inline void c4_MemoProp::Set(const c4_RowRef& rowRef_, const c4_Bytes& value_) const
{
    operator() (rowRef_) = value_;
}

d4_inline c4_Row c4_MemoProp::AsRow(const c4_Bytes& value_) const
{
    c4_Row row;
    operator() (row) = value_;
    return row;
}
    
d4_inline c4_Row c4_MemoProp::operator[] (const c4_Bytes& value_) const
{
    return AsRow(value_);
}
    
///////////////////////////////////////////////////////////////////////////// 
#endif // !q4_TINY
/////////////////////////////////////////////////////////////////////////////
// c4_BytesProp

    //@mfunc Constructs a new bytes property.
    //@parm the name of this property
d4_inline c4_BytesProp::c4_BytesProp (const char* name_) 
    : c4_Property ('B', name_)
{
}
    
d4_inline c4_BytesRef c4_BytesProp::operator() (const c4_RowRef& rowRef_) const
{
    return c4_Reference (rowRef_, *this);
}

d4_inline c4_Bytes c4_BytesProp::Get(const c4_RowRef& rowRef_) const
{
    return operator() (rowRef_);
}

d4_inline void c4_BytesProp::Set(const c4_RowRef& rowRef_, const c4_Bytes& value_) const
{
    operator() (rowRef_) = value_;
}

d4_inline c4_Row c4_BytesProp::AsRow(const c4_Bytes& value_) const
{
    c4_Row row;
    operator() (row) = value_;
    return row;
}
    
d4_inline c4_Row c4_BytesProp::operator[] (const c4_Bytes& value_) const
{
    return AsRow(value_);
}
    
/////////////////////////////////////////////////////////////////////////////
// c4_StringProp

    //@mfunc Constructs a new string property.
    //@parm the name of this property
d4_inline c4_StringProp::c4_StringProp (const char* name_) 
    : c4_Property ('S', name_)
{
}
    
d4_inline c4_StringRef c4_StringProp::operator() (const c4_RowRef& rowRef_) const
{
    return c4_Reference (rowRef_, *this);
}

d4_inline const char* c4_StringProp::Get(const c4_RowRef& rowRef_) const
{
    return operator() (rowRef_);
}

d4_inline void c4_StringProp::Set(const c4_RowRef& rowRef_, const char* value_) const
{
    operator() (rowRef_) = value_;
}

d4_inline c4_Row c4_StringProp::AsRow(const char* value_) const
{
    c4_Row row;
    operator() (row) = value_;
    return row;
}
    
d4_inline c4_Row c4_StringProp::operator[] (const char* value_) const
{
    return AsRow(value_);
}
    
/////////////////////////////////////////////////////////////////////////////
// c4_ViewProp

    //@mfunc Constructs a new view property.
    //@parm the name of this property
d4_inline c4_ViewProp::c4_ViewProp (const char* name_)
    : c4_Property ('V', name_)
{
}
    
d4_inline c4_ViewRef c4_ViewProp::operator() (const c4_RowRef& rowRef_) const
{
    return c4_Reference (rowRef_, *this);
}

d4_inline c4_View c4_ViewProp::Get(const c4_RowRef& rowRef_) const
{
    return operator() (rowRef_);
}

d4_inline void c4_ViewProp::Set(const c4_RowRef& rowRef_, const c4_View& value_) const
{
    operator() (rowRef_) = value_;
}

d4_inline c4_Row c4_ViewProp::AsRow(const c4_View& value_) const
{
    c4_Row row;
    operator() (row) = value_;
    return row;
}
    
d4_inline c4_Row c4_ViewProp::operator[] (const c4_View& value_) const
{
    return AsRow(value_);
}
    
/////////////////////////////////////////////////////////////////////////////
// c4_Strategy

d4_inline bool c4_Strategy::IsValid() const
{ 
	return false; 
}

/////////////////////////////////////////////////////////////////////////////
// c4_CustomViewer

d4_inline c4_CustomViewer::c4_CustomViewer()
{
}

/////////////////////////////////////////////////////////////////////////////
// c4_Sequence

d4_inline c4_Dependencies* c4_Sequence::GetDependencies() const
{
    return _dependencies;
}

/////////////////////////////////////////////////////////////////////////////
// Reordered inlines so they are always used after their definition
 
    //@mfunc Dereferences this cursor to "almost" a row.
    //@rdesc Returns a reference to the row to which this cursor points.
d4_inline c4_RowRef c4_Cursor::operator* () const
{
    return *(c4_RowRef*) this; // cast avoids a const problem with BCPP 4.52
}

    //@mfunc This is the same as *(cursor + offset).
    //@rdesc Returns a reference to the row to which this cursor points.
d4_inline c4_RowRef c4_Cursor::operator[] (int offset_) const
{
    return *(*this + offset_);
}

    //@mfunc Returns a reference to specified entry.
    //@parm the zero-based row index
    //@rdesc A reference to the specified row in the view.
d4_inline c4_RowRef c4_View::GetAt(int index_) const
{
    return * c4_Cursor (*_seq, index_);
}

    //@mfunc Shorthand for <mf c4_View::GetAt>.
    //@parm the zero-based row index
    //@rdesc A reference to the specified row in the view.
    // This reference can be used on either side of the assignment operator.
d4_inline c4_RowRef c4_View::operator[] (int index_) const
{
    return GetAt(index_);
}
    
    //@mfunc Element access, for use as RHS or LHS. Shorthand for GetAt().
    //@parm the zero-based row index
    //@rdesc A reference to the specified row in the view.
d4_inline c4_RowRef c4_View::ElementAt(int index_)
{
    return GetAt(index_);
}

/////////////////////////////////////////////////////////////////////////////

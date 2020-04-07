//........................................................................................
//........................................................................................
//.. File Name: store.cpp
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
//  This file contains the implementation of the storage classes.

#include "header.h"
#include "handler.h"	// 19990906
#include "store.h"
#include "field.h"
#include "column.h"
#include "persist.h"
#include "format.h"		// 19990906

#include "mk4io.h"		// 19991104

#if !q4_INLINE
static char _storeInl[] = "store.inl";
#undef THIS_FILE
#define THIS_FILE _storeInl
#include "store.inl"
#endif
    
#if q4_CHECK
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
    
/////////////////////////////////////////////////////////////////////////////
// Implemented in this file

    class c4_Dependencies;
    class c4_Notifier;
    class c4_Handler;
    class c4_Storage;

//  class c4_Sequence;
        class c4_HandlerSeq;
        class c4_DerivedSeq;

/////////////////////////////////////////////////////////////////////////////
// c4_Dependencies

c4_Dependencies::c4_Dependencies ()
{
    _refs.SetSize(0, 3);    // a little optimization
}

c4_Dependencies::~c4_Dependencies ()
{
    d4_assertThis;
}

void c4_Dependencies::Add(c4_Sequence* seq_)
{
    d4_assertThis;

    for (int i = 0; i < _refs.GetSize(); ++i)
        d4_assert(_refs.GetAt(i) != seq_);

    _refs.Add(seq_);
}

bool c4_Dependencies::Remove(c4_Sequence* seq_)
{
    d4_assertThis;

    int n = _refs.GetSize() - 1;
    d4_assert(n >= 0);

    for (int i = 0; i <= n; ++i)
        if (_refs.GetAt(i) == seq_)
        {
            _refs.SetAt(i, _refs.GetAt(n));         
            _refs.SetSize(n);
            return n > 0;
        }

    d4_assert(0);   // dependency not found
    return true;
}

/////////////////////////////////////////////////////////////////////////////
// c4_Notifier

c4_Notifier::~c4_Notifier ()
{
    d4_assertThis;

    if (_type > kNone && _origin->GetDependencies())
    {
        c4_PtrArray& refs = _origin->GetDependencies()->_refs;

        for (int i = 0; i < refs.GetSize(); ++i)
        {
            c4_Sequence* seq = (c4_Sequence*) refs.GetAt(i);
            d4_assert(seq != 0);

            seq->PostChange(*this);

            if (_chain && _chain->_origin == seq)
            {
                c4_Notifier* next = _chain->_next;
                _chain->_next = 0;

                delete _chain;
                
                _chain = next;
            }
        }
    }

    d4_assert(!_chain);
    d4_assert(!_next);
}

void c4_Notifier::StartSetAt(int index_, c4_Cursor& cursor_)
{
    d4_assertThis;

    _type = kSetAt;
    _index = index_;
    _cursor = &cursor_;
    
    Notify();
}

void c4_Notifier::StartInsertAt(int i_, c4_Cursor& cursor_, int n_)
{
    d4_assertThis;

    _type = kInsertAt;
    _index = i_;
    _cursor = &cursor_;
    _count = n_;
    
    Notify();
}

void c4_Notifier::StartRemoveAt(int index_, int count_)
{
    d4_assertThis;

    _type = kRemoveAt;
    _index = index_;
    _count = count_;
    
    Notify();
}

void c4_Notifier::StartMove(int from_, int to_)
{
    d4_assertThis;

    _type = kMove;
    _index = from_;
    _count = to_;
    
    Notify();
}

void c4_Notifier::StartSet(int i_, int propId_, const c4_Bytes& buf_)
{
    d4_assertThis;

    _type = kSet;
    _index = i_;
    _propId = propId_;
    _bytes = &buf_;
    
    Notify();
}

void c4_Notifier::Notify()
{
    d4_assertThis;

    d4_assert(_origin->GetDependencies() != 0);
    c4_PtrArray& refs = _origin->GetDependencies()->_refs;

    int n = refs.GetSize();
    d4_assert(n > 0);
    
    c4_Notifier** rover = &_chain;

    for (int i = 0; i < n; ++i)
    {
        c4_Sequence* seq = (c4_Sequence*) refs.GetAt(i);
        d4_assert(seq != 0);

        c4_Notifier* ptr = seq->PreChange(*this);
        if (ptr)
        {
            d4_assert(ptr->_origin == seq);

            d4_assert(!*rover);
            *rover = ptr;
            rover = &ptr->_next;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
// c4_Storage

    //@mfunc | c4_Storage | c4_Storage | Constructs a new storage object.
    //@syntax c4_Storage::c4_Storage (FILE* <p file_> =0);
    //@syntax c4_Storage::c4_Storage (c4_Strategy& <p strategy_>);
    //@syntax c4_Storage::c4_Storage (const char* <p fname_>, const char* <p desc_>);
    //@syntax c4_Storage::c4_Storage (const char* <p fname_>, bool <p rw_>);
    //@parmopt FILE* | file_ | 0 | pointer to the file object
    //@parm c4_Strategy& | strategy_ | the strategy object
    //@parm const char* | fname_ | the file name
    //@parm const char* | desc_ | a description of the data structure
    //@parm const char* | rw_ | true if data can be modified

c4_Storage::c4_Storage ()
    : _persist (0)
{
    d4_assertThis;
    
    _persist = d4_new c4_Persist (*d4_new c4_Strategy, true, true); // assume writable
    _persist->IncRef();

    RootTable().DefineRoot();
}

c4_Storage::c4_Storage (c4_Strategy& strategy_, bool owned_)
	: _persist (0)
{
    d4_assertThis;

    _persist = d4_new c4_Persist (strategy_, owned_, true); // assume writable
    _persist->IncRef();

    _persist->LoadAll();
}

c4_Storage::c4_Storage (const char* fname_, const char* description_)
    : _persist (0)
{
    d4_assertThis;
    d4_assert(fname_ != 0);
    d4_assert(description_ != 0);

    Initialize(fname_, true);

    if (*description_)
        SetStructure(description_);
}

c4_Storage::c4_Storage (const char* fname_, bool rw_)
    : _persist (0)
{
    d4_assertThis;
    d4_assert(fname_ != 0);

    Initialize(fname_, rw_);
}

c4_Storage::c4_Storage (const c4_Storage& storage_)
    : _persist (storage_._persist)
{
	d4_assert(_persist != 0);
    _persist->IncRef();
}

void c4_Storage::Initialize(const char* fname_, bool rw_)
{
    d4_assertThis;
    d4_assert(fname_ != 0);
    d4_assert(_persist == 0);

	c4_FileStrategy* strat = d4_new c4_FileStrategy;
	strat->DataOpen(fname_, rw_);

    _persist = d4_new c4_Persist (*strat, true, rw_);
    _persist->IncRef();

    _persist->LoadAll();
}

c4_Storage::~c4_Storage ()
{
    d4_assertThis;
    d4_assert(_persist != 0);

    _persist->DecRef();
}

c4_Storage& c4_Storage::operator= (const c4_Storage& storage_)
{
    d4_assertThis;

    storage_._persist->IncRef();
    _persist->DecRef();
    _persist = storage_._persist;

    return *this;
}

c4_ViewRef c4_Storage::View(const char* name_)
{
    /*
        The easy solution would seem to be:

            c4_ViewProp prop (name_);
            return prop (Contents());

        But this does not work, because the return value would point to
        an object allocated on the stack.

        Instead, make sure the view *has* such a property, and use the
        one inside the c4_Handler for it (since this will stay around).
    */

    c4_HandlerSeq& seq = RootTable();
//  int n = seq.PropIndex(c4_ViewProp (name_));

	c4_ViewProp prop (name_);
	int n = seq.PropIndex(prop);
    d4_assert(n >= 0);

        // the following is an expression of the form "property (rowref)"
    return (seq.NthHandler(n).Property()) (Contents());
}

c4_View c4_Storage::GetAs(const char* description_)
{
    d4_assertThis;
    d4_assert(description_ != 0);

    c4_Field* field = d4_new c4_Field (description_);
    d4_assert(field != 0);

    d4_assert(!*description_);

    c4_String name = field->Name();
    d4_assert(!name.IsEmpty());

    c4_Field& curr = RootTable().Definition();

    c4_String newField = "," + field->Description();
	bool keep = newField.Find('[') >= 0;

    c4_String newDef;

        // go through all subfields, but skip the special 1st entry
    for (int i = 0; i < curr.NumSubFields(); ++i)
    {
        c4_Field& of = curr.SubField(i);   
        if (of.Name().CompareNoCase(name) == 0)
        {
            if (field->IsRepeating())
                newDef += newField;
            // else new is not a repeating entry, so drop this entire field
            
            newField.Empty(); // don't append it later on
            continue;
        }

        newDef += "," + of.Description(); // keep original field
    }

	if (keep)				// added 19990824 ignore if deletion
		newDef += newField; // appends new definition if not found earlier

    delete field;

    const char* p = newDef;
    SetStructure(*p ? ++p : p); // skip the leading comma

	if (!keep)				// 19990916: avoid adding an empty view again
		return c4_View ();

    return View(name);
}

void c4_Storage::SetStructure(const char* description_)
{
    d4_assertThis;
    d4_assert(description_ != 0);

    if (description_ != Description())
    {
		c4_String s = "[" + c4_String (description_) + "]";
		description_ = s;

		c4_Field* field = d4_new c4_Field (description_);
        d4_assert(!*description_);

        d4_assert(field != 0);
        RootTable().Restructure(*field);

		_persist->ForceCommit();
    }
}

    //@mfunc Stores a view under a specified name in this storage object.
    //@parm const char* | name_ | the name of the view property to set
    //@parm const c4_View& | view_ | the view to be stored
#if 0
c4_View c4_Storage::Store(const char* name_, const c4_View& view_)
{
    c4_String s = name_;
    s += c4_View::Description(view_);

    GetAs(s);                           // restructure now
    View(name_) = view_;                // then copy the data into it

    return View(name_);
}
#endif

c4_Strategy& c4_Storage::Strategy() const
{
    d4_assertThis;
    d4_assert(_persist != 0);

    return _persist->Strategy();
}

c4_HandlerSeq& c4_Storage::RootTable() const
{
    d4_assertThis;
    d4_assert(_persist != 0);
    d4_assert(_persist->Root() != 0);

    return * _persist->Root();
}

const char* c4_Storage::Description(const char* name_)
{
    d4_assertThis;

	c4_Field& root = RootTable().Definition();
	c4_String desc;

	if (name_ == 0 || *name_ == 0)
		desc = root.DescribeSubFields();
	else
		for (int i = 0; i < root.NumSubFields(); ++i)
			if (root.SubField(i).Name().CompareNoCase(name_) == 0)
			{
				desc = root.SubField(i).DescribeSubFields();
				break;
			}

        // use the temporary buffer to keep the result around for a while
	return RootTable().UseTempBuffer(desc);
}

    //@mfunc Flushes any pending changes to file right now.
    //@rdesc Describes commit actions taken:
    //@flag true | Succesful, changes have been saved to file.
    //@flag false | Failed to write all data (I/O error?).
bool c4_Storage::Commit()
{
    d4_assertThis;
    d4_assert(_persist != 0);

    return Strategy().IsValid() && _persist->Commit();
}

    //@mfunc (Re)initialize for on-demand loading.
    //@rdesc Describes rollback action taken:
    //@flag true | Everything was committed, no changes were discarded.
    //@flag false | Rolled back one or more uncommitted changes.
bool c4_Storage::Rollback()
{
    d4_assertThis;
    d4_assert(_persist != 0);

    return Strategy().IsValid() && _persist->Rollback();
}

    //@mfunc Sets storage up to always call Commit in the destructor.
void c4_Storage::AutoCommit()
{
    d4_assertThis;
    d4_assert(_persist != 0);

    _persist->AutoCommit();
}

    //@mfunc Gives access to the stored data as a single row.
    //@rdesc Returns a reference to the entire row of the root table.
c4_RowRef c4_Storage::Contents() const
{
    d4_assertThis;
    d4_assert(_persist != 0);
    d4_assert(_persist->Root() != 0);

    return * c4_Cursor (RootTable(), 0);
}

    //@mfunc Loads contents from the specified input stream.
    //
    //  The argument must be of appropriate type expected by the
    //  <c c4_Stragegy> class, usually this is <c FILE> or a pointer to it.
    //
    //  When streaming in, the contents of a storage object are completely
    //  replaced with the new data.  Even the datastructure can change.
    //  All views must be re-attached with calls to <mf c4_Storage::View>.
    //
    //@ex With the MFC interface, here's how to stream into a storage object: |
    //  c4_Storage storage;
    //      <nl>
    //  {
    //      CFile file ("data", CFile::modeRead);
    //      storage.LoadFromFile(&file);
    //  }
    //      <nl>
    //  ... use contents ...
    //
    //@parm the stream to read serialized data from
void c4_Storage::LoadFrom(c4_Stream& stream_)
{
    d4_assertThis;
    d4_assert(_persist != 0);

    _persist->Load(&stream_);
	_persist->ForceCommit();
}

    //@mfunc Saves contents to the specified output stream.
    //
    //  The argument must be of appropriate type expected by the
    //  <c c4_Stragegy> class, usually this is <c FILE> or a pointer to it.
    //
    //  Streaming out does not affect the storage object, it simply takes a
    //  snapshot of the current state (whether it has been committed or not).
    //
    //@ex With the MFC interface, here's how to stream out of a storage object: |
    //  c4_Storage storage;
    //  ... contents (can be an existing datafile) ...
    //      <nl>
    //  {
    //      CFile file ("data", CFile::modeReadWrite <or> CFile::modeCreate);
    //      storage.StoreToFile(&file);
    //  }
    //
    //@parm the stream to write serialized data to
void c4_Storage::SaveTo(c4_Stream& stream_)
{
    d4_assertThis;
    d4_assert(_persist != 0);
    
    _persist->Save(&stream_);
}

/////////////////////////////////////////////////////////////////////////////
// c4_DerivedSeq

c4_DerivedSeq::c4_DerivedSeq (c4_Sequence& seq_)
    : _seq (seq_)
{
    _seq.Attach(this);
}

c4_DerivedSeq::~c4_DerivedSeq ()
{
    d4_assertThis;

    _seq.Detach(this);
}
    
int c4_DerivedSeq::RemapIndex(int index_, const c4_Sequence* seq_) const
{
    d4_assertThis;
    
    return seq_ == this ? index_ : _seq.RemapIndex(index_, seq_);
}
    
int c4_DerivedSeq::Size() const
{
    d4_assertThis;

    return _seq.Size();
}

int c4_DerivedSeq::NumHandlers() const
{
    d4_assertThis;

    return _seq.NumHandlers();
}

c4_Handler& c4_DerivedSeq::NthHandler(int colNum_) const
{
    d4_assertThis;

    return _seq.NthHandler(colNum_);
}

const c4_Sequence* c4_DerivedSeq::HandlerContext(int colNum_) const
{
    d4_assertThis;
    
    return  _seq.HandlerContext(colNum_);
}

int c4_DerivedSeq::AddHandler(c4_Handler* handler_)
{
    d4_assertThis;

    return _seq.AddHandler(handler_);
}

c4_Handler* c4_DerivedSeq::CreateHandler(const c4_Property& prop_)
{
    d4_assertThis;

    return _seq.CreateHandler(prop_);
}

void c4_DerivedSeq::SetSize(int size_)
{
    d4_assertThis;

    _seq.SetSize(size_);
}

c4_Notifier* c4_DerivedSeq::PreChange(c4_Notifier& nf_)
{
    d4_assertThis;

    if (!GetDependencies())
        return 0;

    c4_Notifier* chg = d4_new c4_Notifier (this);

    switch (nf_._type)
    {
        case c4_Notifier::kSetAt:
            chg->StartSetAt(nf_._index, *nf_._cursor);
            break;

        case c4_Notifier::kSet:
            chg->StartSet(nf_._index, nf_._propId, *nf_._bytes);
            break;

        case c4_Notifier::kInsertAt:
            chg->StartInsertAt(nf_._index, *nf_._cursor, nf_._count);
            break;

        case c4_Notifier::kRemoveAt:
            chg->StartRemoveAt(nf_._index, nf_._count);
            break;

        case c4_Notifier::kMove:
            chg->StartMove(nf_._index, nf_._count);
            break;
    }

    return chg;
}

/////////////////////////////////////////////////////////////////////////////

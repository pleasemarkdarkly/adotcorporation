//  Copyright (C) 1996-2000 Jean-Claude Wippler.  All rights reserved.
//
//  This file contains the implementation of the main view classes.
//  
//  Views act as a general interface to collections of data objects.
//  Generic "rows" of data can be added, modified, and removed from a
//  view almost like a normal C++ object with a C++ container class.
//  
//  A major difference between these views and static C++ objects, is
//  that the structure of views can be defined and altered at run time.
//  Access to entries in views is provided by instances of the class
//  c4_Row. Access to the data items in rows is managed by instances
//  of c4_Property. The views themselves are encapsulated by the
//  class c4_View. A reference-counting scheme is used to track
//  the real workhorses of this mechanism, the classes derived from
//  c4_Sequence.

#include "header.h"
#include "derived.h"
#include "custom.h"
#include "store.h"		// for RelocateRows
#include "field.h"		// for RelocateRows

#if !q4_INLINE
static char _mk4Inl[] = "mk4.inl";
#undef THIS_FILE
#define THIS_FILE _mk4Inl
#include "mk4.inl"
#endif
    
#if q4_CHECK
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Implemented in this file

    class c4_ThreadLock;
    class c4_ViewScanner;
    class c4_View;
    class c4_Cursor;
    class c4_RowRef;
    class c4_Row;
    class c4_Property;

/////////////////////////////////////////////////////////////////////////////
// c4_ThreadLock

class c4_ThreadLock
{
public:
    c4_ThreadLock ();

    class Hold
    {
    public:
        Hold ();
        ~Hold ();
    };

    static t4_i32 AddRef(t4_i32&, int);
};

/////////////////////////////////////////////////////////////////////////////

#if q4_WIN32 && q4_MULTI

/*
 *  On Win32, use a critical section to protect the global symbol table.
 *  Also uses special thread-safe calls to inc/dec all reference counts.
 *
 *  This implementation replaces the previous use of TLS, which cannot
 *  be used without special tricks in dynamically loaded DLL's, as is
 *  required for OCX/ActiveX use (which uses LoadLibrary).
 *
 *  Note:   Could have used MFC's CCriticalSection and CSingleLock classes,
 *          but the code below is so trivial that it hardly matters.
 */

#if q4_MSVC && !q4_STRICT
#pragma warning(disable: 4201) // nonstandard extension used : ...
#endif
#include <windows.h>

        static CRITICAL_SECTION gCritSect;

    c4_ThreadLock::c4_ThreadLock ()
    {
        InitializeCriticalSection(&gCritSect);
    }

    c4_ThreadLock::Hold::Hold ()
    {
        EnterCriticalSection(&gCritSect);
    }

    c4_ThreadLock::Hold::~Hold ()
    {
        LeaveCriticalSection(&gCritSect);
    }

    t4_i32 c4_ThreadLock::AddRef(t4_i32& count_, int diff_)
    {
        d4_assert(diff_ == -1 || diff_ == +1);

        return diff_ < 0 ? InterlockedDecrement(&count_)
                         : InterlockedIncrement(&count_);
    }

#else

//  All other implementations revert to the simple "thread-less" case.

    d4_inline c4_ThreadLock::c4_ThreadLock ()
    {
    }

    d4_inline c4_ThreadLock::Hold::Hold ()
    {
    }

    d4_inline c4_ThreadLock::Hold::~Hold ()
    {
    }

    d4_inline t4_i32 c4_ThreadLock::AddRef(t4_i32& count_, int diff_)
    {
        return count_ += diff_;
    }

#endif

/////////////////////////////////////////////////////////////////////////////
// c4_ViewScanner

class c4_ViewScanner
{
public:
    c4_ViewScanner ();
    ~c4_ViewScanner ();

    c4_View Traverse(const c4_View& data_, int curr =-1);
    c4_String Describe(const c4_View& meta_, int index_ =0);

private:
    c4_IntProp pParent;
    c4_IntProp pIndex;
    c4_ViewProp pView;
    c4_StringProp pName;
    c4_StringProp pType;
    c4_IntProp pChild;

    c4_View result;
};

/////////////////////////////////////////////////////////////////////////////

c4_ViewScanner::c4_ViewScanner ()
    : pParent ("parent"), pIndex ("index"), pView ("view"), 
      pName ("name"), pType ("type"), pChild ("child")
{
}

c4_ViewScanner::~c4_ViewScanner ()
{
    d4_assertThis;
}

c4_View c4_ViewScanner::Traverse(const c4_View& data_, int curr_)
{
    d4_assertThis;
    
    if (curr_ < 0)
    {
        result.SetSize(0);
        curr_ = result.Add(pView [c4_View()]);
    }

    c4_View props = pView (result[curr_]);

    for (int i = 0; i < data_.NumProperties(); ++i)
    {
        const c4_Property& p = data_.NthProperty(i);

            // the property name is the key
        c4_Row temp;
        pName (temp) = p.Name();

            // if it's not present, append a new definition for this property
        int n = props.Find(temp);
        if (n < 0)
        {
            pType (temp) = c4_String (p.Type(), 1);
            n = props.Add(temp);
        }

        c4_RowRef def = props[n];

	if (pType (def) != (c4_String) p.Type())    // flag type 
        {
            pType (def) = "?";
        }
        else if (p.Type() == 'V')       // traverse all the subviews
        {
                // get it's definition again if traversed before, else add it
            int child = pChild (def);
            if (child == 0)
            {
                child = result.Add(pParent [curr_] + pIndex [i] +
                                                        pView [c4_View()]);
                pChild (def) = child;
            }

            c4_ViewProp sub (p.Name());
            c4_View subView = pView (result[child]);

                // go through all the subviews to collect their properties
            for (int j = 0; j < data_.GetSize(); ++j)
                Traverse(sub (data_[j]), child);
        }
    }

    return result;
}

c4_String c4_ViewScanner::Describe(const c4_View& meta_, int index_)
{
    c4_String s;

    c4_View props = pView (meta_[index_]);

    for (int i = 0; i < props.GetSize(); ++i)
    {
        c4_RowRef def = props[i];

        if (!s.IsEmpty())
            s += ",";
        
        s += pName (def);

        if (strcmp(pType (def), "V") != 0)
            s += ":" + pType (def);
        else
            s += Describe(meta_, pChild (def));
    }

    if (!s.IsEmpty())
        s = "[" + s + "]";

    return s;
}

/////////////////////////////////////////////////////////////////////////////
// c4_View

    //@mfunc | c4_View | c4_View | Constructs a new view object.
    //@syntax c4_View::c4_View (c4_Sequence* <p seq_> =0);
    //@syntax c4_View::c4_View (const c4_Property& <p prop_>);
    //@syntax c4_View::c4_View (const c4_View& <p view_>);
    //@parmopt c4_Sequence* | seq_ | 0 | the sequence
    //@parm c4_Property& | prop_ | the property
    //@parm const c4_View& | view_ | source for copy constructor

c4_View::c4_View (c4_Sequence* seq_)
    : _seq (seq_)
{                  
    if (!_seq)
        _seq = d4_new c4_HandlerSeq;

    _IncSeqRef();
}

c4_View::c4_View (c4_CustomViewer* viewer_)
    : _seq (0)
{
    d4_assert(viewer_);

    _seq = d4_new c4_CustomSeq (viewer_);

    _IncSeqRef();
}

c4_View::c4_View (const c4_Property& prop_)
    : _seq (d4_new c4_HandlerSeq)
{                  
    _IncSeqRef();

    _seq->PropIndex(prop_);
}

c4_View::c4_View (const c4_View& view_)
    : _seq (view_._seq)
{
    _IncSeqRef();
}

    //@mfunc Makes this view the same as another one.
    //@parm const c4_View& | view_ | the view to use as source
c4_View& c4_View::operator= (const c4_View& view_)
{
    d4_assertThis;

    if (_seq != view_._seq)
    {
        _DecSeqRef();
        _seq = view_._seq;
        _IncSeqRef();
    }   
    return *this;
}

bool c4_View::GetItem(int row_, int col_, c4_Bytes& buf_) const
{
    d4_assertThis;

    c4_Property prop = NthProperty(col_);
    return prop (GetAt(row_)).GetData(buf_);
}

void c4_View::SetItem(int row_, int col_, const c4_Bytes& buf_) const
{
    d4_assertThis;

    c4_Property prop = NthProperty(col_);
    prop (GetAt(row_)).SetData(buf_);
}

    //@mfunc Sets an entry, growing the view if needed.
    //@parm int | index_ | the position of the new entry
    //@parm c4_RowRef | newElem_ | the element to copy into this view
void c4_View::SetAtGrow(int index_, const c4_RowRef& newElem_) 
{
    d4_assertThis;
    d4_assert(_seq != 0);
    
    if (index_ >= GetSize())
        SetSize(index_ + 1);
        
    _seq->SetAt(index_, &newElem_);
}

    //@mfunc Adds a new entry at the end.
    //@parm c4_RowRef | newElem_ | the element to add
int c4_View::Add(const c4_RowRef& newElem_) 
{
    d4_assertThis;
    
    int i = GetSize();
    SetAtGrow(i, newElem_);
    return i;
}

c4_View c4_View::Duplicate(bool deepCopy_) const
{
    d4_assertThis;
    
        // insert all rows, sharing any subviews as needed
    c4_View result = Clone();
    result.InsertAt(0, _seq);

		// 2000-03-15: replace broken Store call with normal recursion
		//
		//	Note: this will lead to infinite recursion for circular views.
		//	These are not handled, because MK is not intended for those.
		//	Then again, this maintains heterogenous subviews, Store didn't.
	if (deepCopy_)
	{
		for (int i = 0; i < NumProperties(); ++i)
		{
			const c4_Property& prop = NthProperty(i);
			if (prop.Type() == 'V')
			{
				const c4_ViewProp& pSub = (const c4_ViewProp&) prop;
				for (int j = 0; j < result.GetSize(); ++j)
				{
					c4_View v = pSub (result[j]);
					pSub (result[j]) = v.Duplicate(true);
				}
			}
		}
	}

    return result;
}

    //@mfunc Constructs a new view with the same structure but no data.
    //@rdesc The new view object.
c4_View c4_View::Clone() const
{
    d4_assertThis;
    
    c4_View view;
    
    for (int i = 0; i < NumProperties(); ++i)
        view._seq->PropIndex(NthProperty(i));

    return view;
}

int c4_View::AddProperty(const c4_Property& prop_)
{
    d4_assertThis;
    d4_assert(_seq != 0);
    
    return _seq->PropIndex(prop_);
}

    //@mfunc Returns the N-th property (using zero-based indexing).
    //@parm the zero-based property index
    //@rdesc A reference to the specified property.
const c4_Property& c4_View::NthProperty(int index_) const
{
    return _seq->NthHandler(index_).Property();
}

int c4_View::FindPropIndexByName(const char* name_) const
{
    d4_assertThis;
    d4_assert(_seq != 0);
    
        // use a slow linear scan to find the untyped property by name
    for (int i = 0; i < NumProperties(); ++i)
    {
        c4_String s = NthProperty(i).Name();
        if (s.CompareNoCase(name_) == 0)
            return i;
    }

    return -1;
}

    //@mfunc Defines a column for a property.
    //@rdesc The new view object (without any data rows).
    //@xref <c c4_Property>
    //@ex The following code defines an empty view with three properties: |
    //  c4_IntProp p1, p2, p3;
    //  c4_View myView = (p1, p2, p3);

c4_View c4_View::operator, (const c4_Property& prop_) const
{
    c4_View view = Clone();
    view.AddProperty(prop_);
    return view;
}

    //@mfunc Inserts a copy of the contents of another view.
void c4_View::InsertAt(int index_, const c4_View& view_)
{
    d4_assertThis;

    int n = view_.GetSize();
    if (n > 0)
    {
        c4_Row empty;
        
        InsertAt(index_, empty, n);

        for (int i = 0; i < n; ++i)
            SetAt(index_ + i, view_[i]);
    }
}

    //@mfunc Move attached rows to somewhere else in same storage
bool c4_View::RelocateRows(int from_, int count_, c4_View& dest_, int pos_)
{
    d4_assertThis;
	d4_assert(0 <= from_ && from_ <= GetSize());
	d4_assert(0 <= count_ && from_ + count_ <= GetSize());
	d4_assert(0 <= pos_ && pos_ <= dest_.GetSize());

    if (count_ > 0)
    {
			// the destination must not be inside the source rows
		if (&dest_ == this && from_ <= pos_ && pos_ < from_ + count_)
			return false;

			// can't determine table without handlers (and can't be a table)
		if (NumProperties() == 0 || dest_.NumProperties() == 0)
			return false;

		c4_Sequence* s1 = _seq;
		c4_Sequence* s2 = dest_._seq;
		c4_HandlerSeq* h1 = (c4_HandlerSeq*) s1->HandlerContext(0);
		c4_HandlerSeq* h2 = (c4_HandlerSeq*) s2->HandlerContext(0);

			// both must be real handler views, not derived ones
		if (h1 != s1 || h2 != s2)
			return false;

			// both must not contain any temporary handlers
		if (s1->NumHandlers() != h1->NumFields() ||
				s2->NumHandlers() != h2->NumFields())
			return false;

			// both must be in the same storage
		if (h1->Persist() == 0 || h1->Persist() != h2->Persist())
			return false;

			// both must have the same structure (is this expensive?)
		c4_String d1 = h1->Definition().Description(true);
		c4_String d2 = h1->Definition().Description(true);
		if (d1 != d2) // ignores all names
			return false;

			// now do the real work: make space, swap rows, drop originals

        c4_Row empty;
        dest_.InsertAt(pos_, empty, count_);

			// careful if insert moves origin
		if (&dest_ == this && pos_ <= from_)
			from_ += count_;

        for (int i = 0; i < count_; ++i)
			h1->ExchangeEntries(from_ + i, *h2, pos_ + i);

		RemoveAt(from_, count_);
    }

	return true;
}

    //@mfunc Scans this view to determine its structure.
    //@rdesc The meta-view which describes the structure of this view.
c4_View c4_View::Structure() const
{
    d4_assertThis;

    c4_ViewScanner scanner;
    return scanner.Traverse(*this);
}

    //@mfunc Converts a structure view to the equivalent description string.
    //@rdesc A homogenized description of this view.
const char* c4_View::Description(const c4_View& view_)
{
    c4_ViewScanner scanner;
    return view_._seq->UseTempBuffer(scanner.Describe(scanner.Traverse(view_)));
}

    //@mfunc Creates view with all rows in natural (property-wise) order.
    //@rdesc The derived view, based on the rows of the underlying one.
c4_View c4_View::Sort() const
{
    d4_assertThis;
    d4_assert(_seq != 0);

    return f4_CreateSort(*_seq);
}

    //@mfunc Creates view sorted according to the specified properties.
    //@parm const c4_View& | up_ | the view which defines the sort order
    //@rdesc The derived view, based on the rows of the underlying one.
c4_View c4_View::SortOn(const c4_View& up_) const
{
    d4_assertThis;
    d4_assert(_seq != 0);
    d4_assert(up_._seq != 0);
    
    c4_Sequence* seq = f4_CreateProject(*_seq, *up_._seq, true);

    return f4_CreateSort(*seq);
}

    //@mfunc Creates sorted view, with some properties sorted in reverse.
    //@parm const c4_View& | up_ | the view which defines the sort order
    //@parm const c4_View& | down_ | subset of <p up_>, defines reverse order
    //@rdesc The derived view, based on the rows of the underlying one.
c4_View c4_View::SortOnReverse(const c4_View& up_, const c4_View& down_) const
{
    d4_assertThis;
    d4_assert(_seq != 0);
    d4_assert(up_._seq != 0);
    d4_assert(down_._seq != 0);
    
    c4_Sequence* seq = f4_CreateProject(*_seq, *up_._seq, true);

    return f4_CreateSort(*seq, down_._seq);
}

    //@mfunc Creates a view with rows matching the specified value.
    //@parm c4_RowRef | crit_ | the values determining which row to select
    //@rdesc The derived view, based on the rows of the underlying one.
c4_View c4_View::Select(const c4_RowRef& crit_) const
{
    d4_assertThis;
    d4_assert(_seq != 0);
    
    return f4_CreateFilter(*_seq, &crit_, &crit_);
}

    //@mfunc Creates a view with row values within the specified range.
    //@parm c4_RowRef | low_ | the values of the lower bounds (inclusive)
    //@parm c4_RowRef | high_ | the values of the upper bounds (inclusive)
    //@rdesc The derived view, based on the rows of the underlying one.
c4_View c4_View::SelectRange(const c4_RowRef& low_, const c4_RowRef& high_) const
{
    d4_assertThis;
    d4_assert(_seq != 0);
    
    return f4_CreateFilter(*_seq, &low_, &high_);
}

    //@mfunc Creates a view with the specified property arrangement.
    //@parm const c4_View& | in_ | the properties included in the result
    //@rdesc The derived view, based on the rows of the underlying one.
c4_View c4_View::Project(const c4_View& in_) const
{
    d4_assertThis;
    d4_assert(_seq != 0);
    d4_assert(in_._seq != 0);
    
    return f4_CreateProject(*_seq, *in_._seq, false);
}

    //@mfunc Creates a view with the specified property arrangement.
    //@parm const c4_View& | out_ | the properties excluded from the result
    //@rdesc The derived view, based on the rows of the underlying one.
c4_View c4_View::ProjectWithout(const c4_View& out_) const
{
    d4_assertThis;
    d4_assert(_seq != 0);
    d4_assert(out_._seq != 0);
    
    return f4_CreateProject(*_seq, *_seq, false, out_._seq);
}

c4_View c4_View::Slice(int first_, int limit_, int step_) const
{
    d4_assertThis;

    return f4_CustSlice(*_seq, first_, limit_, step_);
}

c4_View c4_View::Product(const c4_View& view_) const
{
    d4_assertThis;

    return f4_CustProduct(*_seq, view_);
}

c4_View c4_View::RemapWith(const c4_View& view_) const
{
    d4_assertThis;

    return f4_CustRemapWith(*_seq, view_);
}

c4_View c4_View::Pair(const c4_View& view_) const
{
    d4_assertThis;

    return f4_CustPair(*_seq, view_);
}

c4_View c4_View::Concat(const c4_View& view_) const
{
    d4_assertThis;

    return f4_CustConcat(*_seq, view_);
}

c4_View c4_View::Rename(const c4_Property& old_, const c4_Property& new_) const
{
    d4_assertThis;

    return f4_CustRename(*_seq, old_, new_);
}

c4_View c4_View::GroupBy(const c4_View& keys_, const c4_ViewProp& result_) const
{
    d4_assertThis;

    return f4_CustGroupBy(*_seq, keys_, result_);
}

c4_View c4_View::Counts(const c4_View& keys_, const c4_IntProp& result_) const
{
    d4_assertThis;

    return f4_CustGroupBy(*_seq, keys_, result_); // third arg is c4_IntProp
}

c4_View c4_View::Unique() const
{
    d4_assertThis;

    c4_IntProp count ("#N#");
    return Counts(Clone(), count).ProjectWithout(count);
}


c4_View c4_View::Union(const c4_View& view_) const
{
    d4_assertThis;

    return Concat(view_).Unique();
}

c4_View c4_View::Intersect(const c4_View& view_) const
{
    d4_assertThis;

    c4_View v = Concat(view_);
    
        // assume neither view has any duplicates
    c4_IntProp count ("#N#");
    return v.Counts(Clone(), count).Select(count [2]).ProjectWithout(count);
}

c4_View c4_View::Different(const c4_View& view_) const
{
    d4_assertThis;

    c4_View v = Concat(view_); 

        // assume neither view has any duplicates
    c4_IntProp count ("#N#");
    return v.Counts(Clone(), count).Select(count [1]).ProjectWithout(count);
}

c4_View c4_View::Minus(const c4_View& view_) const
{
    d4_assertThis;

        // inefficient: calculate difference, then keep only those in self
    return Intersect(Different(view_));
}

c4_View c4_View::JoinProp(const c4_ViewProp& sub_, bool outer_) const
{
    d4_assertThis;

    return f4_CustJoinProp(*_seq, sub_, outer_);
}

c4_View c4_View::Join(const c4_View& keys_, const c4_View& view_, bool outer_) const
{
    d4_assertThis;

        // inefficient: calculate difference, then keep only those in self
    return f4_CustJoin(*_seq, keys_, view_, outer_);
}

    //@mfunc Returns the index of the specified row in this view (or -1).
    // This function can be used to "unmap" an index of a derived view back
    // to the original underlying view.
    //@parm c4_RowRef& | row_ | the row to relate to this view
    //@rdesc The index of the specified row in this, or -1 of not found.
int c4_View::GetIndexOf(const c4_RowRef& row_) const
{
    d4_assertThis;
    d4_assert(_seq != 0);
    
    c4_Cursor cursor = &row_;
    d4_assert(cursor._seq != 0);
    
    return cursor._seq->RemapIndex(cursor._index, _seq);
}

    //@mfunc Finds index of the next entry matching the specified key,
    // using linear search. Only the properties present in the search key
    // are used to determine whether a row matches the key.
    //@parm the value to look for
    //@parmopt the index to start with
    //@rdesc Returns the index if found, else -1.
int c4_View::Find(const c4_RowRef& crit_, int start_) const
{
    d4_assertThis;
    d4_assert(_seq != 0);
    d4_assert(start_ >= 0);

    c4_Row copy = crit_; // the lazy (and slow) solution: make a copy

    c4_View refView = copy.Container();
    c4_Sequence* refSeq = refView._seq;
    d4_assert(refSeq != 0);

    c4_Bytes data;

    while (start_ < GetSize())
    {
        int i;
        
        for (i = 0; i < refSeq->NumHandlers(); ++i)
        {
            c4_Handler& h = refSeq->NthHandler(i); // no context issues

            if (!_seq->Get(start_, h.PropId(), data))
                h.ClearBytes(data);
            
            if (h.Compare(0, data) != 0) // always row 0
                break;
        }

        if (i == refSeq->NumHandlers())
            return start_;

        ++start_;
    }

    return -1;
}

    //@mfunc Searches for a key, using the native sort order of the view.
    //@parm c4_RowRef | crit_ | the value to look for
    //@rdesc Returns index of first row >= crit_, else indexes past the end.
int c4_View::Search(const c4_RowRef& crit_) const
{
    d4_assertThis;
    d4_assert(_seq != 0);

    int l = 0;
    int u = GetSize() - 1;

    while (l <= u)
    {
        int i = (l + u) >> 1;
        int f = _seq->Compare(i, &crit_);
        if (f > 0)
            u = i - 1;
        else if (f < 0)
            l = i + 1;
        else
            return i;
    }

    return l;
}

int c4_View::Compare(const c4_View& view_) const
{
    d4_assertThis;
    d4_assert(_seq != 0);

    if (_seq == view_._seq)
        return 0;

    int na = GetSize();
    int nb = view_.GetSize();
    int i;

    for (i = 0; i < na && i < nb; ++i)
        if (GetAt(i) != view_.GetAt(i))
            return GetAt(i) < view_.GetAt(i) ? -1 : +1;

    return na == nb ? 0 : i < na ? +1 : -1;
}

/////////////////////////////////////////////////////////////////////////////
// c4_Row

    //@mfunc Default constructor.
c4_Row::c4_Row ()
    : c4_RowRef (* Allocate())
{
}

c4_Row::c4_Row (const c4_Row& row_)
    : c4_RowRef (* Allocate())
{
    operator= (row_);
}

c4_Row::c4_Row (const c4_RowRef& rowRef_)
    : c4_RowRef (* Allocate())
{
    operator= (rowRef_);
}

    //@mfunc Destructor.
c4_Row::~c4_Row ()
{
    d4_assertThis;
    
    Release(_cursor);
}
    
c4_Row& c4_Row::operator= (const c4_Row& row_)
{
    d4_assertThis;
    
    return operator= ((const c4_RowRef&) row_);
}

    //@mfunc Assignment from a reference to a row.
    //@parm const c4_RowRef& | rowRef_ | the row reference to assign from
c4_Row& c4_Row::operator= (const c4_RowRef& rowRef_) 
{
    d4_assertThis;
    d4_assert(_cursor._seq != 0);
    
    if (_cursor != &rowRef_)
    {
        d4_assert(_cursor._index == 0);
        _cursor._seq->SetAt(0, &rowRef_);
    }
        
    return *this;
}

    //@mfunc Adds all properties and values into this row.
    //@parm const c4_RowRef& | rowRef_ | the row to concatenate from
void c4_Row::ConcatRow(const c4_RowRef& rowRef_) 
{
    d4_assertThis;
    d4_assert(_cursor._seq != 0);
    
    c4_Cursor cursor = &rowRef_;    // trick to access private rowRef_._cursor
    d4_assert(cursor._seq != 0);
    
    c4_Sequence& rhSeq = * cursor._seq;
    
    c4_Bytes data;

    for (int i = 0; i < rhSeq.NumHandlers(); ++i)
    {
        c4_Handler& h = rhSeq.NthHandler(i);

        h.GetBytes(cursor._index, data);
        _cursor._seq->Set(_cursor._index, h.Property(), data);
    }
}

c4_Row operator+ (const c4_RowRef& a_, const c4_RowRef& b_)
{
    c4_Row row = a_;
    row.ConcatRow(b_);
    return row;
}

c4_Cursor c4_Row::Allocate()
{
    c4_Sequence* seq = d4_new c4_HandlerSeq;
    seq->IncRef();
    
    seq->Resize(1);
    
    return c4_Cursor (*seq, 0);
}

void c4_Row::Release(c4_Cursor row_)
{
    d4_assert(row_._seq != 0);
    d4_assert(row_._index == 0);
	d4_assert(row_._seq->NumRefs() == 1);
    
	row_._seq->DecRef();
}

/////////////////////////////////////////////////////////////////////////////
// c4_Property

    // This is a workaround for the fact that the initialization order of
    // static objects is not always adequate (implementation dependent).
    // Extremely messy solution, to allow statically declared properties.

	static c4_ThreadLock* sThreadLock = 0;
	static c4_StringArray* sPropNames = 0;
	static c4_DWordArray* sPropCounts = 0;

	void c4_Property::CleanupInternalData()
	{
		delete sPropNames;
		sPropNames = 0; // race

		delete sPropCounts;
		sPropCounts = 0; // race

		delete sThreadLock;
		sThreadLock = 0; // race
	}

c4_Property::c4_Property (char type_, const char* name_)
    : _type (type_)
{
    if (sThreadLock == 0)
		sThreadLock = new c4_ThreadLock;

    c4_ThreadLock::Hold lock; // grabs the lock until end of scope

    if (sPropNames == 0)
		sPropNames = new c4_StringArray;

    if (sPropCounts == 0)
		sPropCounts = new c4_DWordArray;

    c4_String temp = name_;

		// 20-25% faster on open if same prop in lots of subviews, 19990905
    //for (_id = 0; _id < sPropNames->GetSize(); ++_id)
    _id = sPropNames->GetSize();
	while (-- _id >= 0)
        if (temp.CompareNoCase(sPropNames->GetAt(_id)) == 0)
            break;
    
    //if (_id >= sPropNames->GetSize())
    if (_id < 0)
    {
        for (_id = 0; _id < sPropCounts->GetSize(); ++_id)
            if (sPropCounts->GetAt(_id) == 0)
                break;

        if (_id >= sPropCounts->GetSize())
        {
            sPropCounts->SetSize(_id + 1);
            sPropNames->SetSize(_id + 1);
        }

        sPropCounts->SetAt(_id, 0);
        sPropNames->SetAt(_id, name_);
    }

    Refs(+1);
}

c4_Property::c4_Property (const c4_Property& prop_)
    : _id (prop_.GetId()), _type (prop_.Type())
{
	d4_assert(sPropCounts != 0);
    d4_assert(sPropCounts->GetAt(_id) > 0);

    Refs(+1);
}

c4_Property::~c4_Property ()
{
	Refs(-1);
}

void c4_Property::operator= (const c4_Property& prop_)
{
    d4_assertThis;

    prop_.Refs(+1);
    Refs(-1);

    _id = prop_.GetId();
    _type = prop_.Type();
}

const char* c4_Property::Name() const
{
    d4_assertThis;
	d4_assert(sPropNames != 0);

    return sPropNames->GetAt(_id);
}

void c4_Property::Refs(int diff_) const
{
    d4_assert(diff_ == -1 || diff_ == +1);

	if (sPropCounts != 0) // race
		c4_ThreadLock::AddRef((t4_i32&) sPropCounts->ElementAt(_id), diff_);
	else
		d4_assert(diff_ < 0); // can only destroy properties once data is gone

#if q4_CHECK
        // get rid of the cache when the last property goes away
    static t4_i32 sPropTotals;

    if (c4_ThreadLock::AddRef(sPropTotals, diff_) == 0)
    {
        c4_ThreadLock::Hold lock; // grabs the lock until end of scope

            // be prepared for a race, so check again after locking
        if (sPropTotals == 0)
			CleanupInternalData();
    }
#endif
}

/////////////////////////////////////////////////////////////////////////////

//  Copyright (C) 1996-2000 Jean-Claude Wippler.  All rights reserved.
//
//  Main library include file

#ifndef __MK4_H__
#define __MK4_H__

/////////////////////////////////////////////////////////////////////////////
//
//  TITLE
//                                
//      The MetaKit Library, by Jean-Claude Wippler, Equi4 Software, NL.
//      
//  DESCRIPTION
//                                
//      Structured data storage with commit / rollback and on-demand loading.
//  
//  LICENSE
//
//      This software is distributed as open source software.  For details
//      on what you may and may not do with this software, see the license
//      details at the end of the "README" file in the parent directory.
//            
//  ACKNOWLEDGEMENTS
//                                                                        
//      To Liesbeth and Myra, for making this possible.
//
/////////////////////////////////////////////////////////////////////////////
//
//  NAMING CONVENTIONS        PREFIX    REMARKS
//                              
//      Compile time options    q4_     Always defined as 1 or 0, capitalized
//      Preprocessor defines    d4_     Use with "#ifdef" or "#if defined()"
//      Classes                 c4_     Classes, listed at start of headers
//      Typedefs                t4_     Type definitions, if outside classes
//      Global functions        f4_     Internal, these are rarely defined
//
//      Member functions                Start in uppercase
//      Instance variables      _       And start in lowercase
//      Static members          _       And start in uppercase
//
//      Local variable names            Start in lowercase
//      Formal parameter names          Start lowercase, end with underscore
//
/////////////////////////////////////////////////////////////////////////////

#define d4_MetaKitLibraryVersion 201    // Mar 17, 2000
    //: Current release = 100 * major + 10 * minor + maintenance

/////////////////////////////////////////////////////////////////////////////
// Declarations in this file

    class c4_View;                      // a view on underlying data
    class c4_Cursor;                    // an index into a view
    class c4_RowRef;                    // a reference to a row
        class c4_Row;                   // one row in a view
    class c4_Bytes;                     // used to pass around generic data
    class c4_Storage;                   // manages view persistence
    class c4_CustomViewer;              // used for customizable views
    class c4_Stream;                    // abstract stream class
    class c4_Strategy;                  // system and file interface

    class c4_Property;                  // for access inside rows
        class c4_IntProp;
        class c4_FloatProp;
        class c4_DoubleProp;
        class c4_StringProp;
        class c4_BytesProp;
        class c4_MemoProp;
        class c4_ViewProp;

    // Everything below is part of the implementation, not for public use

    class c4_Sequence;                  // a collection of rows

    class c4_Reference;                 // refers to the actual data values
        class c4_IntRef;
        class c4_FloatRef;
        class c4_DoubleRef;
        class c4_MemoRef;
        class c4_BytesRef;
        class c4_StringRef;
        class c4_ViewRef;

    class c4_Dependencies;              // not defined here
    class c4_Handler;                   // not defined here
    class c4_HandlerSeq;                // not defined here
    class c4_Notifier;                  // not defined here
    class c4_Persist;                   // not defined here

/////////////////////////////////////////////////////////////////////////////

    // determine whether we need to include "mk4dll.h" to link as DLL
#if defined (MKDLL_EXPORTS) && !defined (q4_KITDLL)
#define q4_KITDLL 1
#endif

    // omit floats, doubles, memos in small model 16-bit Intel builds
#if defined (_DOS) && defined (_M_I86SM) && !defined (q4_TINY)
#define q4_TINY 1
#endif

    // and here's the other end of the scale... Alpha has 64-bit longs
#if defined (__alpha) && !defined (_WIN32) && !defined (q4_LONG64)
#define q4_LONG64 1
#endif

    // release builds default to inlining for maximum performance
#if defined (NDEBUG) && !defined (q4_INLINE)
#define q4_INLINE 1
#endif

/////////////////////////////////////////////////////////////////////////////
//#ifndef __K4CONF_H__
/////////////////////////////////////////////////////////////////////////////

    // Borland C++ and C++ Builder
#if defined (__BORLANDC__)
    // by default, if runtime is linked as a DLL, then so is MetaKit
#if defined (_RTLDLL) && !defined (q4_KITDLL)
#define q4_KITDLL 1
#endif

    // Borland 5.0 supports the bool datatype
#if __BORLANDC__ >= 0x500
#define q4_BOOL 1
#endif
#endif // __BORLANDC__

    // GNU gcc/egcs
#if defined (__GNUC__)
#if __bool_true_false_are_defined && !defined (q4_BOOL)
#define q4_BOOL 1
#endif
#endif

    // Metrowerks CodeWarrior
#if defined (__MWERKS__)
#if __option(bool)
#define q4_BOOL 1       // bool datatype is optionally supported
    // undef, these conflict with c4_Storage::c4_Storage overloading
#undef bool
#undef true
#undef false
#endif
#endif

    // Microsoft Visual C++
#if defined (_MSC_VER)
    // MSVC 5.0 supports the bool datatype, MSVC 4.x has no namespaces
#if _MSC_VER >= 1100
#define q4_BOOL 1
#else
#define q4_NO_NS 1
#endif

    // a kludge to avoid having to use ugly DLL exprt defs in this header
#pragma warning(disable: 4273) // inconsistent dll linkage
#endif // _MSC_VER

/////////////////////////////////////////////////////////////////////////////
//#endif // __K4CONF_H__
/////////////////////////////////////////////////////////////////////////////
// Other definitions needed by the public MetaKit library header files

#if !q4_BOOL && !q4_STD         // define a bool datatype
#define false 0
#define true 1
#define bool int
#endif

#if q4_KITDLL                   // add declaration specifiers
#include "mk4dll.h"
#endif

#if q4_INLINE                   // enable inline expansion
#define d4_inline inline
#else
#define d4_inline
#endif

typedef unsigned char t4_byte;  // create typedefs for t4_byte, etc.
#if q4_LONG64
typedef int t4_i32;             // if longs are 64b, then int must be 32b
typedef unsigned t4_u32;
#else
typedef long t4_i32;            // if longs aren't 64b, then they are 32b
typedef unsigned long t4_u32;
#endif

/////////////////////////////////////////////////////////////////////////////
//: A collection of data rows.
//
//  Views act like pointers to the actual collections, setting a view to a new
//  value does not alter the collection to which this view pointed previously.
//
//  The protocol used for this class mimics the way many other collection
//  classes are defined. For example, when used with MFC, you will see member
//  definitions such as c4_View::GetSize, c4_View::GetAt, c4_View::InsertAt.
//
//  The elements of views can be referred to by their 0-based index, which
//  produces a row-reference of type c4_RowRef.  These row references can
//  be copied, used to get or set properties, or dereferenced (in which case
//  an object of class c4_Row is returned).  Taking the address of a row
//  reference produces a c4_Cursor, which acts very much like a pointer.
//
//  The following code creates a view with 1 row and 2 properties:
//
//      c4_StringProp pName ("Name");
//      c4_IntProp pAge ("Age");
//
//      c4_Row data;
//      pName (data) = "John Williams";
//      pAge (data) = 43;
//
//      c4_View myView;
//      myView.Add(row);

class c4_View
{
    c4_Sequence* _seq;

public:
/* Construction / destruction / assignment */
    c4_View (c4_Sequence* implementation_ =0);
        //: Construct a view based on a sequence
    c4_View (c4_CustomViewer* viewer_);
        //: Construct a view based on a custom viewer
    c4_View (const c4_Property& property_);
        //: Construct an empty view with one property
    c4_View (const c4_View& view_);
        //: Construct a view from another one
    ~c4_View ();
        //: Destructor, decrements reference count
    
    c4_View& operator= (const c4_View& source_);
        //: Make this view the same as another one

/* Getting / setting the number of rows */
    int GetSize() const;    
        //: Return the number of entries
    int GetUpperBound() const;
        //: Return highest index (size - 1)
   
    void SetSize(int newSize_, int growBy_ =-1); 
        //: Change the size of this view
        //  Since views act like dynamic arrays, you can quickly
        //  change their size.  Increasing the size will append rows
        //  with zero/empty values, while decreasing it will delete
        //  the last rows.  The growBy_ parameter is currently unused.
    void RemoveAll();
        //: Remove all entries (sets size to zero)

/*: Getting / setting individual elements */
    c4_RowRef GetAt(int index_) const;
        //: Return a reference to specified entry
    c4_RowRef operator[] (int index_) const;
        //: Shorthand for c4_View::GetAt
    
    void SetAt(int index_, const c4_RowRef& row_);
        //: Change the value of the specified entry
        //  Replace an entry with the contents of another row.  If the
        //  row contains subviews, they will be replaced as well, but
        //  there are two cases: if this is an attached view, all the
        //  subview rows will be copied (deep copy), whereas for an
        //  unattached view the new subviews will be shared with the 
        //  original (shallow copy).
    c4_RowRef ElementAt(int index_);
        //: Element access, for use as RHS or LHS

    bool GetItem(int row_, int col_, c4_Bytes& buf_) const;
        //: Get a single data item in a generic way
        //!param: row_ - row index, range [0 .. GetSize())
        //!param: col_ - property index, range [0 .. NumProperties())
        //!param: buf_ - buffer for the result
        //  This can be used to access view data in a generalized way.
        //  Useful for c4_CustomViewers which are based on other views.
    
    void SetItem(int row_, int col_, const c4_Bytes& buf_) const;
    	//: Set a single data item in a generic way

/* These can increase the number of rows */
    void SetAtGrow(int index_, const c4_RowRef& row_);   
        //: Set an entry, growing the view if needed
    int Add(const c4_RowRef& row_);
        //: Add a new entry, same as "SetAtGrow(GetSize(), ...)".

/* Insertion / deletion of rows */
    void InsertAt(int index_, const c4_RowRef& row_, int count_ =1);
        //: Insert one or more copies of an entry
    void RemoveAt(int index_, int count_ =1);
        //: Remove entries starting at the given index
    void InsertAt(int index_, const c4_View& view_);
        //: Insert a copy of the contents of another view
    bool RelocateRows(int from_, int count_, c4_View& dest_, int pos_);
        //: Move attached rows to somewhere else in same storage

/* Dealing with the properties of this view */
    int NumProperties() const; 
        //: Return the number of properties
    const c4_Property& NthProperty(int column_) const;
        //: Return the id of the N-th property
    int FindProperty(int id_);
        //: Find the index of a property, given its id
    int FindPropIndexByName(const char* name_) const;
        //: Find the index of a property, given its name

/* Structural information */
    c4_View Structure() const;
        //: Return a view which describes the structure of this view
    const char* Describe() const;
        //: Return a description of the structure
    static const char* Description(const c4_View& view_);
        //: Return a homogenized description of this view

    c4_View Duplicate(bool deepCopy_ =false) const;
        //: Construct a new view with a copy of the data
    c4_View Clone() const;
        //: Construct a new view with the same structure but no data
    int AddProperty(const c4_Property& property_);
        //: Adds a property column to a view if not already present
    c4_View operator, (const c4_Property& property_) const;
        //: Return a view like the first, with a property appended to it
    
/* Derived views */
    c4_View Sort() const;
        //: Create view with all rows in natural (property-wise) order
    c4_View SortOn(const c4_View& order_) const;
        //: Create view sorted according to the specified properties
    c4_View SortOnReverse(const c4_View& order_, const c4_View& orderDown_) const;
        //: Create sorted view, with some properties sorted in reverse

    c4_View Select(const c4_RowRef& criterium_) const;
        //: Create view with rows matching the specified value
    c4_View SelectRange(const c4_RowRef& rowLow_, const c4_RowRef& rowHigh_) const;
        //: Create view with row values within the specified range

    c4_View Project(const c4_View& order_) const;
        //: Create view with the specified property arrangement
    c4_View ProjectWithout(const c4_View& order_) const;
        //: Create derived view with some properties omitted

    c4_View Slice(int start_, int limit_ =-1, int step_ =1) const;
        //: Create view which is a segment/slice (default is up to end)
        //  Returns a view which is a subset, either a contiguous range, or
        //  a "slice" with element taken from every step_ entries.  If the
        //  step is negative, the same entries are returned, but in reverse
        //  order (start_ is still lower index, it'll then be returned last).
    c4_View Product(const c4_View& view_) const;
        //: Create view which is the cartesian product with given view
        //  The cartesian product is defined as every combination of rows
        //  in both views.  The number of entries is the product of the
        //  number of entries in the two views, properties which are present
        //  in both views will use the values defined in this view.
    c4_View RemapWith(const c4_View& order_) const;
        //: Create view which remaps another given view
        //  Remapping constructs a view with the rows indicated by another
        //  view.  The first property in the order_ view must be an int
        //  property with index values referring to this one.  The size of
        //  the resulting view is determined by the order_ view and can
        //  differ, for example to act as a subset selection (if smaller).
    c4_View Pair(const c4_View& view_) const;
        //: Create view which pairs each row with corresponding row
        //  This is like a row-by-row concatenation.  Both views must have
        //  the same number of rows, the result has all properties from
        //  this view plus any other properties from the other view.
    c4_View Concat(const c4_View& view_) const;
        //: Create view with rows from another view appended
        //  Constructs a view which has all rows of this view, and all rows
        //  of the second view appended.  The structure of the second view
        //  is assumed to be identical to this one.  This operation is a bit
        //  similar to appending all rows from the second view, but it does
        //  not actually store the result anywhere, it just looks like it.
    c4_View Rename(const c4_Property& old_, const c4_Property& new_) const;
        //: Create view with one property renamed (must be of same type)

    c4_View GroupBy(const c4_View& keys_, const c4_ViewProp& name_) const;
        //: Create view with a subview, grouped by the specified properties
        //!param: keys_ - the properties in this view determine the grouping
        //!param: name_ - name of the new subview defined in result
        //  This operation is similar to the SQL 'GROUP BY', but it takes
        //  advantage of the fact that MetaKit supports nested views.  The
        //  view returned from this member has one row per distinct group,
        //  with an extra view property holding the remaining properties.
        //  If there are N rows in the original view matching key X, then
        //  the result is a row for key X, with a subview of N rows.  The
        //  properties of the subview are all the properties not in the key.
    c4_View Counts(const c4_View& keys_, const c4_IntProp& name_) const;
        //: Create view with count of duplicates, when grouped by key
        //!param: keys_ - the properties in this view determine the grouping
        //!param: name_ - name of the new count property defined in result
        //  This is similar to c4_View::GroupBy, but it determines only the
        //  number of rows in each group and does not create a nested view.
    c4_View Unique() const;
        //: Create view with all duplicate rows omitted
        //  Returns a subset which does not contain any duplicate rows.

    c4_View Union(const c4_View& view_) const;
        //: Create view which is the set union (assumes no duplicate rows)
        //  Calculates the set union.  This will only work if both input
        //  views are sets, i.e. they have no duplicate rows in them.
    c4_View Intersect(const c4_View& view_) const;
        //: Create view with all rows also in the given view (no dups)
        //  Calculates the set intersection.  This will only work if both
        //  input views are sets, i.e. they have no duplicate rows in them.
    c4_View Different(const c4_View& view_) const;
        //: Create view with all rows not in both views (no dups)
        //  Calculates the "XOR" of two sets.  This will only work if both
        //  input views are sets, i.e. they have no duplicate rows in them.
    c4_View Minus(const c4_View& view_) const;
        //: Create view with all rows not in the given view (no dups)
        //  Calculates set-difference of this view minus arg view.  Result
        //  is a subset, unlike c4_View::Different. Will only work if both
        //  input views are sets, i.e. they have no duplicate rows in them.

    c4_View JoinProp(const c4_ViewProp& sub_, bool outer_ =false) const;
        //: Create view with a specific subview expanded, like a join
        //!param: sub_ - name of the subview to expand
        //!param: outer_ - true: keep rows with empty subviews
        //  This operation is the inverse of c4_View::GroupBy, expanding
        //  all rows in specified subview and returning a view which looks
        //  as if the rows in each subview were "expanded in place".
    c4_View Join(const c4_View& keys_, const c4_View& view_, bool outer_ =false) const;
        //: Create view which is the relational join on the given keys
        //!param: keys_ - the properties in this view determine the join
        //!param: view_ - second view participating in the join
        //!param: outer_ - true: keep rows with no match in second view

    int GetIndexOf(const c4_RowRef& row_) const;
        //: Return the index of the specified row in this view (or -1)

/* Searching */
    int Find(const c4_RowRef& key_, int start_ =0) const;
        //: Find index of the the next entry matching the specified key
    int Search(const c4_RowRef& key_) const;
        //: Search for a key, using the native sort order of the view
    
/* Comparing view contents */
    int Compare(const c4_View& view_) const;

    friend bool operator== (const c4_View& a_, const c4_View& b_);
        //: Return true if the contents of both views are equal
    friend bool operator!= (const c4_View& a_, const c4_View& b_);
        //: Return true if the contents of both views are not equal
    friend bool operator< (const c4_View& a_, const c4_View& b_);
        //: True if first view is less than second view
    friend bool operator> (const c4_View& a_, const c4_View& b_);
        //: True if first view is greater than second view
    friend bool operator<= (const c4_View& a_, const c4_View& b_);
        //: True if first view is less or equal to second view
    friend bool operator>= (const c4_View& a_, const c4_View& b_);
        //: True if first view is greater or equal to second view
    
protected:
    void _IncSeqRef();
    void _DecSeqRef();

    friend class c4_ViewRef;
};

/////////////////////////////////////////////////////////////////////////////
//: An iterator for collections of rows (views).
//
//  Cursor objects can be used to point to specific entries in a view.
//  A cursor acts very much like a pointer to a row in a view, and is 
//  returned when taking the address of a c4_RowRef.  Dereferencing
//  a cursor leads to the original row reference again.  You can construct a
//  cursor for a c4_Row, but since such rows are not part of a collection,
//  incrementing or decrementing these cursors is meaningless (and wrong). 
//
//  The usual range of pointer operations can be applied to these objects:
//  pre/post-increment and decrement, adding or subtracting integer offsets,
//  as well as the full range of comparison operators.  If two cursors
//  point to entries in the same view, their difference can be calculated.
//
//  As with regular pointers, care must be taken to avoid running off of
//  either end of a view (the debug build includes assertions to check this).

class c4_Cursor
{
    c4_Sequence* _seq;
        /* Pointer to the sequence. */
    int _index;
        /* Current index into the sequence. */

public: 
/* Construction / destruction / dereferencing */
    c4_Cursor (c4_Sequence& implementation_, int index_);
        //: Construct a new cursor
    
    c4_RowRef operator* () const;
        //: Dereference this cursor to "almost" a row
    
    c4_RowRef operator[] (int index_) const;
        //: This is the same as *(cursor + offset)

/* Stepping the iterator forwards / backwards */
    c4_Cursor& operator++ ();
        //: Pre-increment the cursor
    c4_Cursor operator++ (int);
        //: Post-increment the cursor
    c4_Cursor& operator-- ();
        //: Pre-decrement the cursor
    c4_Cursor operator-- (int);
        //: Post-decrement the cursor

    c4_Cursor& operator+= (int offset_);
        //: Advance by a given offset
    c4_Cursor& operator-= (int offset_);
        //: Back up by a given offset

    c4_Cursor operator- (int) const;
        //: Subtract a specified offset
    int operator- (c4_Cursor cursor_) const;
        //: Return the distance between two cursors
    
    friend c4_Cursor operator+ (c4_Cursor cursor_, int offset_);
        //: Add specified offset
    friend c4_Cursor operator+ (int offset_, c4_Cursor cursor_);
        //: Add specified offset to cursor

/* Comparing row positions */
    friend bool operator== (c4_Cursor a_, c4_Cursor b_);
        //: Return true if both cursors are equal
    friend bool operator!= (c4_Cursor a_, c4_Cursor b_);
        //: Return true if both cursors are not equal
    friend bool operator< (c4_Cursor a_, c4_Cursor b_);
        //: True if first cursor is less than second cursor
    friend bool operator> (c4_Cursor a_, c4_Cursor b_);
        //: True if first cursor is greater than second cursor
    friend bool operator<= (c4_Cursor a_, c4_Cursor b_);
        //: True if first cursor is less or equal to second cursor
    friend bool operator>= (c4_Cursor a_, c4_Cursor b_);
        //: True if first cursor is greater or equal to second cursor

/* Comparing row contents */
    friend bool operator== (const c4_RowRef& a_, const c4_RowRef& b_);
        //: Return true if the contents of both rows are equal
    friend bool operator!= (const c4_RowRef& a_, const c4_RowRef& b_);
        //: Return true if the contents of both rows are not equal
    friend bool operator< (const c4_RowRef& a_, const c4_RowRef& b_);
        //: True if first row is less than second row
    friend bool operator> (const c4_RowRef& a_, const c4_RowRef& b_);
        //: True if first row is greater than second row
    friend bool operator<= (const c4_RowRef& a_, const c4_RowRef& b_);
        //: True if first row is less or equal to second row
    friend bool operator>= (const c4_RowRef& a_, const c4_RowRef& b_);
        //: True if first row is greater or equal to second row


    friend class c4_MemoRef;
    friend class c4_Reference;
    friend class c4_Row;
    friend class c4_RowRef;
    friend class c4_View;

    friend class c4_Sequence;
    friend class c4_FilterSeq;
    friend class c4_SortSeq;
};

/////////////////////////////////////////////////////////////////////////////
//: Reference to a data row, can be used on either side of an assignment.
//
//  Row references are created when dereferencing a c4_Cursor or when
//  indexing an element of a c4_View.  Assignment will change the
//  corresponding item.  Rows (objects of type c4_Row) are a special
//  case of row references, consisting of a view with exactly one item.
//
//  Internally, row references are very similar to cursors, in fact they are
//  little more than a wrapper around them.  The essential difference is one
//  of semantics: comparing row references compares contents, copying row
//  references copies the contents, whereas cursor comparison and copying
//  deals with the pointer to the row, not its contents.

class c4_RowRef
{
    c4_Cursor _cursor;
        /* A row reference is a cursor in disguise. */

public: 
/* General operations */
    c4_RowRef operator= (const c4_RowRef& row_);
        //: Assign the value of another row to this one
    c4_Cursor operator& () const;
        //: Return the cursor associated to this row
    c4_View Container() const;
        //: Return the underlying container view

protected:
    c4_RowRef (c4_Cursor);
        //: Constructor, not for general use

    friend class c4_Cursor;
    friend class c4_Row;
};

/////////////////////////////////////////////////////////////////////////////
//: An entry in a collection with copy semantics.
//
//  Rows can exist by themselves and as contents of views.  Row assignment
//  implies that a copy of the contents of the originating row is made.
//
//  A row is implemented as an unattached view with exactly one element.

class c4_Row : public c4_RowRef 
{
public:
    c4_Row ();
        //: Construct a row with no properties
    c4_Row (const c4_Row& row_);
        //: Construct a row from another one
    c4_Row (const c4_RowRef& row_);
        //: Construct a row copy from a row reference
    ~c4_Row ();
        //: Destructor
    
    c4_Row& operator= (const c4_Row& row_);
        //: Assign a copy of another row to this one
    c4_Row& operator= (const c4_RowRef& row_);
        //: Copy another row to this one
    
    void ConcatRow(const c4_RowRef& row_);
        //: Add all properties and values into this row
    friend c4_Row operator+ (const c4_RowRef& a_, const c4_RowRef& b_);
        //: Return a new row which is the concatenation of two others
    
private:
    static c4_Cursor Allocate();
    static void Release(c4_Cursor);
};

/////////////////////////////////////////////////////////////////////////////
//: Generic data buffer, with optional automatic clean up.
//
//  These objects are used to pass around untyped data without concern about
//  clean-up.  They know whether the bytes need to be de-allocated when these
//  objects go out of scope.  Small amounts of data are stored in the object.
//
//  Objects of this class are used a lot within MetaKit to manipulate its own
//  data items generically.  The c4_BytesProp class allows storing binary
//  data explicitly in a file.  If such data files must be portable, then the 
//  application itself must define a generic format to deal with byte order.
//
//  How to store an object in binary form in a row (this is not portable):
//
//      struct MyStruct { ... };
//      MyStruct something;
//  
//      c4_BytesProp pData ("Data");
//      c4_Row row;
//  
//      pData (row) = c4_Bytes (&something, sizeof something);

class c4_Bytes
{
    enum { kMaxBuf = 16 };
    t4_byte _buffer [kMaxBuf];

    union {
        t4_byte* _contents;
        double _aligner; // on a Sparc, the int below wasn't enough...
    };
    int _size;
    bool _copy;

public:
    c4_Bytes ();
        //: Construct an empty binary object
    c4_Bytes (const void* buffer_, int length_);
        //: Construct an object with contents, no copy
    c4_Bytes (const void* buffer_, int length_, bool makeCopy_);
        //: Construct an object with contents, optionally as a copy
    c4_Bytes (const c4_Bytes& bytes_);
        //: Copy constructor     
    ~c4_Bytes ();
        //: Destructor
    
    c4_Bytes& operator= (const c4_Bytes& bytes_);
        //: Assignment, this may make a private copy of contents
    void Swap(c4_Bytes& bytes_);
        //: Swap the contents and ownership of two byte objects
    
    int Size() const;
        //: Return the number of bytes of its contents
    const t4_byte* Contents() const;
        //: Return a pointer to the contents
    
    t4_byte* SetBuffer(int length_);
        //: Define contents as a freshly allocated buffer of given size
    t4_byte* SetBufferClear(int length_);
        //: Allocate a buffer and fills its contents with zero bytes

    friend bool operator== (const c4_Bytes& a_, const c4_Bytes& b_);
        //: Return true if the contents of both objects are equal
    friend bool operator!= (const c4_Bytes& a_, const c4_Bytes& b_);
        //: Return true if the contents of both objects are not equal

private:
    void _MakeCopy();
    void _LoseCopy();
};

/////////////////////////////////////////////////////////////////////////////
//: Manager for persistent storage of view structures.
//
//  The storage class uses a view, with additional functionality to be able 
//  to store and reload the data it contains (including nested subviews).
//
//  By default, data is loaded on demand, i.e. whenever data which has
//  not yet been referenced is used for the first time.  Loading is limited
//  to the lifetime of this storage object, since the storage object carries
//  the file descriptor with it that is needed to access the data file.
//
//  To save changes, call the Commit member.  This is the only time
//  that data is written to file - when using a read-only file simply avoid
//  calling Commit.
//
//  The LoadFromStream and SaveToStream members can be used to
//  serialize the contents of this storage row using only sequential I/O
//  (no seeking, only read or write calls).
//
//  The data storage mechanism implementation provides fail-safe operation:
//  if anything prevents Commit from completing its task, the last
//  succesfully committed version of the saved data will be recovered on
//  the next open. This also includes changes made to the table structure. 
//
//  The following code creates a view with 1 row and stores it on file:
//
//      c4_StringProp pName ("Name");
//      c4_IntProp pAge ("Age");
//
//      c4_Storage storage ("myfile.dat", true);
//      c4_View myView = storage.GetAs("Musicians[Name:S,Age:I]");
//
//      myView.Add(pName ["John Williams"] + pAge [43]);
//
//      storage.Commit();

class c4_Storage 
{
    c4_Persist* _persist;

public:
    c4_Storage (); 
        //: Construct streaming-only storage object
    c4_Storage (c4_Strategy& strategy_, bool owned_ =false); 
        //: Construct a storage using the specified strategy handler
    c4_Storage (const char* filename_, const char* description_);
        //: Construct a storage object for given file and format
    c4_Storage (const char* filename_, bool canModify_);
        //: Construct a storage object, keeping the current structure
    c4_Storage (const c4_Storage&);
        //: Copy constructor
    ~c4_Storage ();
        //: Destructor, usually closes file, but does not commit by default
    
    c4_Storage& operator= (const c4_Storage&);
        //: Assignment of storage implements reference semantics

    void AutoCommit();
        //: Set storage up to always call Commit in the destructor
    c4_RowRef Contents() const;
        //: Give access to the stored data as a single row

    c4_Strategy& Strategy() const;
        //: Return the strategy object associated with this storage
    c4_HandlerSeq& RootTable() const;
        //: Return the root table entry
    const char* Description(const char* name_ =0);
        //: Return a description of the view structure (default is all)
    
    bool Commit();
        //: Flush pending changes to file right now
    bool Rollback();
        //: (Re)initialize for on-demand loading
        //  Calling Rollback will cancel all uncommitted changes.
    
    c4_ViewRef View(const char* name_);
        //: Get or set a named view in this storage object
    c4_View GetAs(const char* description_);
        //: Get a named view, redefining it to match the given structure

    //DROPPED: c4_View Store(const char* name_, const c4_View& view_);

    void LoadFrom(c4_Stream& stream_);
        //: Load contents from the specified input stream
    void SaveTo(c4_Stream& stream_);
        //: Save contents to the specified output stream

private:
    void Initialize(const char*, bool);
    void SetStructure(const char*);
};

/////////////////////////////////////////////////////////////////////////////
//: Base class for the basic data types.
//
//  Property objects exist independently of view, row, and storage objects.
//  They have a name and type, and can appear in any number of views.
//  You will normally only use derived classes, to maintain strong typing.

class c4_Property
{
    short _id;
    char _type;

public:
    c4_Property (char type_, int id_);
        //: Construct a new property with the give type and id
    c4_Property (char type_, const char* name_);
        //: Construct a new property with the give type and name
    ~c4_Property ();
        //: Destructor
    
    c4_Property (const c4_Property& property_);
        //: Copy constructor
    void operator= (const c4_Property& property_);
        //: Assignment

    const char* Name() const;
        //: Return the name of this property
    char Type() const;
        //: Return the type of this property

    int GetId() const;
        //: Return a unique id for this property
        //  A property object in fact merely represents an entry in a globally
        //  maintained symbol table.  Each property is assigned a unique id,
        //  which remains valid as long as some reference to that property
        //  exists.  In general, property id's remain unique as long as the
        //  application runs.  Do not store id's on file, since they are
        //  not guaranteed to remain the same across program invocations.
        //  All properties with the same name are given the same id.

    c4_Reference operator() (const c4_RowRef& row_) const;
        //: Get or set this untyped property in a row

    void Refs(int) const;
        //: Adjust the reference count
        //  This is part of the implementation and shouldn't normally be called.

    c4_View operator, (const c4_Property& prop_) const;
        //: Return a view like the first, with a property appended to it

    static void CleanupInternalData();
        //: Call this to get rid of som eonternal datastructues (on exit)
};

    //: Integer properties.
class c4_IntProp : public c4_Property 
{
public:
    c4_IntProp (const char* name_);
        //: Construct a new property

    c4_IntRef operator() (const c4_RowRef& row_) const;
        //: Get or set an integer property in a row
    t4_i32 Get(const c4_RowRef& row_) const;
        //: Get an integer property in a row
    void Set(const c4_RowRef& row_, t4_i32 value_) const;
        //: Set an integer property in a row

    c4_Row operator[] (t4_i32 value_) const;
        //: Creates a row with one integer, shorthand for AsRow.
    c4_Row AsRow(t4_i32 value_) const;
        //: Creates a row with one integer.
};

#if !q4_TINY

    //: Floating point properties.
class c4_FloatProp : public c4_Property 
{
public:
    c4_FloatProp (const char* name_);
        //: Construct a new property

    c4_FloatRef operator() (const c4_RowRef& row_) const;
        //: Get or set a floating point property in a row
    double Get(const c4_RowRef& row_) const;
        //: Get a floating point property in a row
    void Set(const c4_RowRef& row_, double value_) const;
        //: Set a floating point property in a row

    c4_Row operator[] (double value_) const;
        //: Create a row with one floating point value, shorthand for AsRow
    c4_Row AsRow(double value_) const;
        //: Create a row with one floating point value
};

    //: Double precision properties.
class c4_DoubleProp : public c4_Property 
{
public:
    c4_DoubleProp (const char* name_);
        //: Construct a new property.

    c4_DoubleRef operator() (const c4_RowRef& row_) const;
        //: Get or set a double precision property in a row
    double Get(const c4_RowRef& row_) const;
        //: Get a double precision property in a row
    void Set(const c4_RowRef& row_, double value_) const;
        //: Set a double precision property in a row

    c4_Row operator[] (double value_) const;
        //: Create a row with one double precision value, shorthand for AsRow
    c4_Row AsRow(double value_) const;
        //: Create a row with one double precision value
};

    //: Memo properties.
class c4_MemoProp : public c4_Property
{
public:
    c4_MemoProp (const char* name_);
        //: Construct a new property

    c4_MemoRef operator() (const c4_RowRef& row_) const;
        //: Get or set a memo property in a row
    c4_Bytes Get(const c4_RowRef& row_) const;
        //: Get a memo property in a row
    void Set(const c4_RowRef& row_, const c4_Bytes& value_) const;
        //: Set a memo property in a row

    c4_Row operator[] (const c4_Bytes& value_) const;
        //: Create a row with one memo object, shorthand for AsRow
    c4_Row AsRow(const c4_Bytes& value_) const;
        //: Create a row with one memo object
};
#endif // !q4_TINY

    //: String properties.
class c4_StringProp : public c4_Property
{
public:
    c4_StringProp (const char* name_);
        //: Construct a new property

    c4_StringRef operator() (const c4_RowRef& row_) const;
        //: Get or set a string property in a row
    const char* Get(const c4_RowRef& row_) const;
        //: Get a string property in a row
    void Set(const c4_RowRef& row_, const char* value_) const;
        //: Set a string property in a row

    c4_Row operator[] (const char* value_) const;
        //: Create a row with one string, shorthand for AsRow
    c4_Row AsRow(const char* value_) const;
        //: Create a row with one string
};

    //: Binary properties.
class c4_BytesProp : public c4_Property
{
public:
    c4_BytesProp (const char* name_);
        //: Construct a new property

    c4_BytesRef operator() (const c4_RowRef& row_) const;
        //: Get or set a bytes property in a row
    c4_Bytes Get(const c4_RowRef& row_) const;
        //: Get a bytes property in a row
    void Set(const c4_RowRef& row_, const c4_Bytes& value_) const;
        //: Set a bytes property in a row

    c4_Row operator[] (const c4_Bytes& value_) const;
        //: Create a row with one bytes object, shorthand for AsRow
    c4_Row AsRow(const c4_Bytes& value_) const;
        //: Create a row with one bytes object
};

    //: View properties.
class c4_ViewProp : public c4_Property
{
public:
    c4_ViewProp (const char* name_);
        //: Construct a new property

    c4_ViewRef operator() (const c4_RowRef& row_) const;
        //: Get or set a view property in a row
    c4_View Get(const c4_RowRef& row_) const;
        //: Get a view property in a row
    void Set(const c4_RowRef& row_, const c4_View& value_) const;
        //: Set a view property in a row

    c4_Row operator[] (const c4_View& value_) const;
        //: Create a row with one view, shorthand for AsRow
    c4_Row AsRow(const c4_View& value_) const;
        //: Create a row with one view
};

/////////////////////////////////////////////////////////////////////////////
//: Abstract base class for definition of custom views.
//
//  A custom view is a view which can be accessed like any other view, using
//  row and property operations, but which is fully managed by a customized
//  "viewer" class.  The viewer will eventually handle all requests for the
//  view, such as defining its structure and size, as well as providing the
//  actual data values when requested.
//
//  Custom views cannot propagate changes.
//
//  To implement a custom view, you must derive your viewer from this base
//  class and define each of the virtual members.  Then create a new object
//  of this type on the heap and pass it to the c4_View constructor.  Your
//  viewer will automatically be destroyed when the last reference to its
//  view goes away.  See the DBF2MK sample code for an example of a viewer.

class c4_CustomViewer
{
protected: 
    c4_CustomViewer ();
        //: Constructor, must be overriden in derived class
public: 
    virtual ~c4_CustomViewer ();
        //: Destructor
    
    virtual c4_View GetTemplate() = 0;
        //: Return the structure of this view (initialization, called once)
    virtual int GetSize() = 0;
        //: Return the number of rows in this view
    virtual bool GetItem(int row_, int col_, c4_Bytes& buf_) = 0;
        //: Fetch one data item, return it as a generic data value
    virtual void SetItem(int row_, int col_, const c4_Bytes& buf_);
        //: Store one data item, supplied as a generic data value
    virtual void InsertRows(int pos_, c4_Cursor value_, int count_=1);
    	//: Insert one or more copies of a row (if possible)
    virtual void RemoveRows(int pos_, int count_=1);
    	//: Remove one or more rows (this is not always possible)
};

/////////////////////////////////////////////////////////////////////////////
//: A stream is a virtual helper class to serialize in binary form.

class c4_Stream
{
public:
    virtual ~c4_Stream ();

    virtual int Read(void* buffer_, int length_) = 0;
        //: Fetch some bytes sequentially
    virtual void Write(const void* buffer_, int length_) = 0;
        //: Store some bytes sequentially
};

/////////////////////////////////////////////////////////////////////////////
//: A strategy encapsulates code dealing with the I/O system interface.

class c4_Strategy
{
public:
    c4_Strategy ();
    virtual ~c4_Strategy ();

    virtual bool IsValid() const;
        //: True if we can do I/O with this object
    virtual void DataSeek(t4_i32 position_);
        //: Set file position
    virtual int  DataRead(void* buffer_, int length_);
        //: Read a number of bytes
    virtual bool DataWrite(const void* buffer_, int length_);
        //: Write a number of bytes, return true if successful
    virtual void DataCommit(t4_i32 newSize_);
        //: Flush and truncate file
    virtual void ResetFileMapping();
        //: Override to support memory-mapped files

    bool _keepAfterCommit;
        //: Keep data in memory after each commit (default is false)
    bool _bytesFlipped;
        //: True if the storage format is not native (default is false)
    int _failure;
        //: Error code of last failed I/O operation, zero if I/O was ok

    const t4_byte* _mapStart;
        //: First byte in file mapping, zero if not active
    const t4_byte* _mapLimit;
        //: Past last byte in file mapping, zero if not active
};

/////////////////////////////////////////////////////////////////////////////
//: A sequence is an abstract base class for views on ranges of records.
//
//  Sequences represent arrays of rows (or indexed collections / tables).
//  Insertion and removal of entries is allowed, but could take linear time.
//  A reference count is maintained to decide when the object should go away.

class c4_Sequence
{
    int _refCount;
        /* Reference count. */
    c4_Dependencies* _dependencies;
        /* Pointer to dependency list, or null if nothing depends on this. */

protected:
    int _propertyLimit;
        /* Optimization: cached property index. */
    short* _propertyMap; // see c4_HandlerSeq::Reset()
        /* Optimization: property map for faster access */
    c4_Bytes* _tempBuf;
        /* allocated on first use by c4_Sequence::Buffer() */

public: 
/* General */
    c4_Sequence ();
        //: Abstract constructor
    
    virtual int Compare(int, c4_Cursor) const;
        //: Compare the specified row with another one
    void SetAt(int, c4_Cursor);
        //: Replace the contents of a specified row
    virtual int RemapIndex(int, const c4_Sequence*) const;
        //: Remap the index to an underlying view
    
    const char* Describe();
        //: Return a descriptions of the current data structure

/* Reference counting */
    void IncRef();
        //: Increment the reference count of this sequence
    void DecRef();
        //: Decrement the reference count, delete objects when last
    int NumRefs() const;
        //: Return the current number of references to this sequence

/* Adding / removing rows */
    virtual int Size() const = 0;
        //: Return the current number of rows
    void Resize(int, int =-1);
        //: Change number of rows, either by inserting or removing them
    
    virtual void InsertAt(int, c4_Cursor, int =1);
        //: Insert one or more rows into this sequence
    virtual void RemoveAt(int, int =1);
        //: Remove one or more rows from this sequence
    virtual void Move(int, int);
        //: Move a row to another position

/* Properties */
    int NthPropId(int) const;
        //: Return the id of the N-th property
    int PropIndex(int);
        //: Find the index of a property by its id
    int PropIndex(const c4_Property&);
        //: Find the index of a property, or create a new entry
    
    virtual int NumHandlers() const = 0;
        //: Return the number of data handlers in this sequence
    virtual c4_Handler& NthHandler(int) const = 0;
        //: Return a reference to the N-th handler in this sequence
    virtual const c4_Sequence* HandlerContext(int) const = 0;
        //: Return the context of the N-th handler in this sequence
    virtual int AddHandler(c4_Handler*) = 0;
        //: Add the specified data handler to this sequence
    virtual c4_Handler* CreateHandler(const c4_Property&) = 0;
        //: Create a handler of the appropriate type

/* Element access */
    virtual int ItemSize(int index_, int propId_);
        //: Return width of specified data item
    virtual bool Get(int, int, c4_Bytes&);
        //: Retrieve one data item from this sequence
    virtual void Set(int, const c4_Property&, const c4_Bytes&);
        //: Store a data item into this sequence
    
/* Dependency notification */
    void Attach(c4_Sequence* child_);
        //: Register a sequence to receive change notifications
    void Detach(c4_Sequence* child_);
        //: Unregister a sequence which received change notifications
    c4_Dependencies* GetDependencies() const;
        //: Return a pointer to the dependencies, or null

    virtual c4_Notifier* PreChange(c4_Notifier& nf_);
        //: Called just before a change is made to the sequence
    virtual void PostChange(c4_Notifier& nf_);
        //: Called after changes have been made to the sequence
    
    const char* UseTempBuffer(const char*);

protected:
    virtual ~c4_Sequence ();

    void ClearCache();

public: //! for c4_Table::Sequence setup
    virtual void SetSize(int size_) = 0;

    c4_Bytes& Buffer();
        //: Gives access to a general purpose temporary buffer
private:
    c4_Sequence (const c4_Sequence&);   // not implemented
    void operator= (const c4_Sequence&); // not implemented
};

/////////////////////////////////////////////////////////////////////////////
//: A reference is used to get or set typed data, using derived classes.
//
//  Objects of this class are only intended to be used as a temporary handle
//  while getting and setting properties in a row.  They are normally only
//  constructed as result of function overload operators: "property (row)".

class c4_Reference
{
protected:
    c4_Cursor _cursor;
        /* The cursor which points to the data. */
    const c4_Property& _property;
        /* The property associated to this reference. */

public:
    c4_Reference (const c4_RowRef&, const c4_Property&);
        //: Constructor

    c4_Reference& operator= (const c4_Reference&);
        //: Assignment of one data item

    int GetSize() const;
        //: Return width of the referenced data item
    bool GetData(c4_Bytes&) const;
        //: Retrieve the value of the referenced data item
    void SetData(const c4_Bytes&) const;
        //: Store a value into the referenced data item
    
    friend bool operator== (const c4_Reference&, const c4_Reference&);
        //: Return true if the contents of both references is equal
    friend bool operator!= (const c4_Reference&, const c4_Reference&);
        //: Return true if the contents of both references is not equal

private:
    void operator& () const;            // not implemented
};

/////////////////////////////////////////////////////////////////////////////

    //: Used to get or set integer values.
class c4_IntRef : public c4_Reference
{
public:
    c4_IntRef (const c4_Reference&);
        //: Constructor
    operator t4_i32 () const;
        //: Get the value as long integer
    c4_IntRef& operator= (t4_i32);
        //: Set the value to the specified long integer
};

#if !q4_TINY

    //: Used to get or set floating point values.
class c4_FloatRef : public c4_Reference
{
public:
    c4_FloatRef (const c4_Reference&);
        //: Constructor
    operator double () const;
        //: Get the value as floating point
    c4_FloatRef& operator= (double);
        //: Set the value to the specified floating point
};

    //: Used to get or set double precision values.
class c4_DoubleRef : public c4_Reference
{
public:
    c4_DoubleRef (const c4_Reference&);
        //: Constructor
    operator double () const;
        //: Get the value as floating point
    c4_DoubleRef& operator= (double);
        //: Set the value to the specified floating point
};

    //: Used to get or set memo values (growable large binary objects).
class c4_MemoRef : public c4_Reference
{
public:
    c4_MemoRef (const c4_Reference&);
        //: Constructor
    operator c4_Bytes () const;
        //: Get the value as binary object
    c4_MemoRef& operator= (const c4_Bytes&);
        //: Set the value to the specified binary object

    c4_Bytes Access(t4_u32 off_, int len_ =0) const;
        //: Fetch data from the memo field, up to end if length is zero
    bool Modify(const c4_Bytes& buf_, t4_u32 off_, int diff_ =0) const;
        //: Store data, resize by diff_ bytes, return true if successful
};

#endif // !q4_TINY

    //: Used to get or set binary object values.
class c4_BytesRef : public c4_Reference
{
public:
    c4_BytesRef (const c4_Reference&);
        //: Constructor
    operator c4_Bytes () const;
        //: Get the value as binary object
    c4_BytesRef& operator= (const c4_Bytes&);
        //: Set the value to the specified binary object
};

    //: Used to get or set string values.
class c4_StringRef : public c4_Reference
{
public:
    c4_StringRef (const c4_Reference&);
        //: Constructor
    operator const char* () const;
        //: Get the value as string
    c4_StringRef& operator= (const char*);
        //: Set the value to the specified string
};

    //: Used to get or set view values.
class c4_ViewRef : public c4_Reference
{
public:
    c4_ViewRef (const c4_Reference&);
        //: Constructor
    operator c4_View () const;
        //: Get the value as view
    c4_ViewRef& operator= (const c4_View&);
        //: Set the value to the specified view
};

/////////////////////////////////////////////////////////////////////////////

#if q4_INLINE
#include "mk4.inl"
#endif

/////////////////////////////////////////////////////////////////////////////

#endif // __MK4_H__

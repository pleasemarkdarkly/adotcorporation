//  Copyright (C) 1996-2000 Jean-Claude Wippler.  All rights reserved.
//
//  This file contains the declaration of the data handler classes.

#ifndef __HANDLER_H__
#define __HANDLER_H__

/////////////////////////////////////////////////////////////////////////////
// Declarations in this file

    class c4_Handler;                   // data representation handler

//  class c4_Sequence;
        class c4_HandlerSeq;            // a sequence built from handlers

    class c4_Column;                    // not defined here
    class c4_Field;						// not defined here
    class c4_Streamer;					// not defined here

/////////////////////////////////////////////////////////////////////////////
//: Handlers deal with all issues concerning data representation.

class c4_Handler
{
    c4_Property _property;

public:
    c4_Handler (const c4_Property& _prop);
        //: Constructor (this is an abstract base class).
    virtual ~c4_Handler ();

    virtual void Initialize(int);
        //: Initializes the data handler once context is known.

    virtual void Defined();
        //: Called when the corresponding table has been fully defined.
    virtual void FlipBytes();
        //: Called to reverse the internal byte order of foreign data.
	virtual bool Commit(c4_Streamer& ar_);
		//: Commit the associated column(s) to file.
	virtual void Prepare(c4_Streamer& ar_);
		//: Assign the associated column(s) to file.

    const c4_Property& Property() const;
        //: Returns the property associated with this handler.
    int PropId() const;
        //: Returns the id of the property associated with this handler.

    void ClearBytes(c4_Bytes& buf_) const;
        //: Used for backward compatibility, should probably be replaced.
    virtual const void* Clear(int& length_) const;
        //: Returns the default value for items of this type.

    virtual int ItemSize(int index_) = 0;
        //: Return width of specified data item.
    void GetBytes(int index_, c4_Bytes& buf_, bool copySmall_ =false);
        //: Used for backward compatibility, should probably be replaced.
    virtual void Set(int index_, const c4_Bytes& buf_) = 0;
        //: Stores a new data item at the specified index.

    virtual int Compare(int index_, const c4_Bytes& buf_) = 0;
        //: Compares an entry with a specified data item.

    virtual void Insert(int index_, const c4_Bytes& buf_, int count_) = 0;
        //: Inserts 1 or more data items at the specified index.
    virtual void Remove(int index_, int count_) = 0;
        //: Removes 1 or more data items at the specified index.
    void Move(int from_, int to_);
        //: Move a data item to another position.

    virtual c4_Column* GetNthMemoCol(int index_, bool alloc_ =false);
        //: Special access to underlying data of memo entries
    virtual void SetNthMemoPos(int, t4_u32, t4_u32 len_, c4_Column* =0);
        //: Special adjustment of memo entries

	virtual bool IsPersistent() const;
		//: True if this handler might do I/O to satisfy fetches

protected:
    virtual const void* Get(int index_, int& length_) = 0;
        //: Retrieves the data item at the specified index.
};

/////////////////////////////////////////////////////////////////////////////
//:(IMPL) A handler sequence uses data handlers for access to the data.
//
//  This is a type of sequence which actually stores data items (as opposed
//  to derived sequences, which rely on other sequences to deal with this).
//  Information is managed by data handlers, which are usually organized to
//  store information as column-wise vectors.

class c4_HandlerSeq : public c4_Sequence
{
    c4_PtrArray _handlers;
    c4_Persist* _persist;
    c4_Field* _field;   
    c4_HandlerSeq* _owner;
    int _ownerRow;
    int _size;

public:
    c4_HandlerSeq (c4_Persist* =0);
    
    virtual int Size() const;
    
    virtual int NumHandlers() const;
    virtual c4_Handler& NthHandler(int) const;
    virtual const c4_Sequence* HandlerContext(int) const;
    virtual int AddHandler(c4_Handler*);

    virtual void SetSize(int size_);
	virtual void RemoveAt(int index_, int count_); // added 2000-03-14

protected:
    virtual c4_Handler* CreateHandler(const c4_Property&);

    virtual ~c4_HandlerSeq ();

public:
    c4_Handler* SwapHandler(int, c4_Handler*);
        // SwapHandler is called from ~Table
//!!!!
    void DefineRoot(const char* ="");
        //: Initializes this table as a root table.
    void DefineSub(c4_Field*, c4_HandlerSeq*, int);
        //: Initializes this table as a nested table.
    void Restructure(c4_Field&, bool remove_ =false);
        //: Restructures a table to match the given field layout.
	void DetachFromParent();
		//: Breaks the tie with the owning view
    void DetachFromStorage(bool partial_ =false);
		//: Drop all temp handlers which might do I/O

/* Table properties */
    c4_Field& Definition() const;
        //: Returns the repeating field definition.
    c4_String Description() const;
        //: Returns a description of the data structure.
    c4_HandlerSeq& Owner() const;
        //: Returns a reference to the owner of this table.
    int OwnerRow() const;
        //: Returns the row index in the owning table.
    c4_Persist* Persist() const;
        //: Returns the associated persistence manager.
    
/* Access to subfields */
    c4_Field& Field(int) const;
        //: Returns the field definition of a subcolumn.
	int NumFields() const;
		//: Returns number of persistent columns.
	char ColumnType(int index_) const;
		//: Returns type code of specified column.
    bool IsNested(int) const;
        //: Returns true if a column consists of subtables.
    
/* Table persistence */
    bool Commit(c4_Streamer&);
        //: Saves current table state to file.
    void Prepare(c4_Streamer&);
        //: Loads table state from file.

/* Internal methods */
    void FlipAllBytes();
        //: Permanently switch the byte orde.
	void ExchangeEntries(int srcPos_, c4_HandlerSeq& dst_, int dstPos_);
		//: Exchange two table entries, along with the substructure
    void SetOwnerRow(int);
		// adjust the row number of this table in its parent

    c4_HandlerSeq& SubEntry(int, int) const;
};

/////////////////////////////////////////////////////////////////////////////

#if q4_INLINE
#include "handler.inl"
#endif

/////////////////////////////////////////////////////////////////////////////

#endif

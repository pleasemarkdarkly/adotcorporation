//  Copyright (C) 1996-2000 Jean-Claude Wippler.  All rights reserved.
//
//  This file contains the declaration of the storage classes.
//
//@todo     Updatable views (where appropriate, lots of tricky details).

#ifndef __STORE_H__
#define __STORE_H__

/////////////////////////////////////////////////////////////////////////////
// Declarations in this file

    class c4_Dependencies;              // list of dependent sequences
    class c4_Notifier;                  // takes care of change notification

//  class c4_Sequence;
        class c4_DerivedSeq;            // a sequence based on another one

    class c4_Handler;					// not defined here

	/////////////////////////////////////////////////////////////////////////////
//:(IMPL) These objects keep track of the set of dependent views.

class c4_Dependencies
{
    c4_PtrArray _refs;

public:
    c4_Dependencies ();
    ~c4_Dependencies ();

    void Add(c4_Sequence* seq_);
    bool Remove(c4_Sequence* seq_);

    friend class c4_Notifier;
};

/////////////////////////////////////////////////////////////////////////////
//:(IMPL) Manages the process of notifying all derived views.
//
//  Notification is a very powerful (and complex) mechanism which is used to
//  propagate changes in views to all derived views. The complexity is caused
//  by the fact that changes will cause views to be in an inconsistent state
//  during change propagation. Pre- and post-changes are used to handle this.

class c4_Notifier
{
    c4_Sequence* _origin;
    c4_Notifier* _chain;
    c4_Notifier* _next;

public:
    enum { kNone, kSetAt, kInsertAt, kRemoveAt, kMove, kSet, kLimit };

    c4_Notifier (c4_Sequence* origin_);
    ~c4_Notifier ();

    bool HasDependents() const;

    void StartSetAt(int index_, c4_Cursor& cursor_);
    void StartInsertAt(int index_, c4_Cursor& cursor_, int count_);
    void StartRemoveAt(int index_, int count_);
    void StartMove(int from_, int to_);
    void StartSet(int index_, int propId_, const c4_Bytes& buf_);

    int _type;
    int _index;
    int _propId;
    int _count;
    c4_Cursor* _cursor;
    const c4_Bytes* _bytes;

private:
    void Notify();
};

/////////////////////////////////////////////////////////////////////////////
//:(IMPL) A derived sequence delegates most work to another sequence.
//
//  This is an abstract base class, which is used by several other classes
//  including <c c4_SortSeq>, <c c4_FilterSeq>, and <c c4_ProjectSeq>.
//  The basic mechanism here is to store a pointer to an underlying sequence
//  and to funnel all requests through to it. Derived classes can adjust this
//  behavior to suit their purposes. Note that derived sequences fall into
//  roughly two categories: row mapping and property mapping. Row mapping
//  modifies the row index before calling the underlying sequence, while
//  property mapping alters the use of properties or data handlers.

class c4_DerivedSeq : public c4_Sequence
{
protected:
    c4_Sequence& _seq;

protected:
    c4_DerivedSeq (c4_Sequence& seq_);
    virtual ~c4_DerivedSeq ();
    
public:
    virtual int RemapIndex(int, const c4_Sequence*) const;

    virtual int Size() const;
    
    virtual int NumHandlers() const;
    virtual c4_Handler& NthHandler(int) const;
    virtual const c4_Sequence* HandlerContext(int) const;
    virtual int AddHandler(c4_Handler*);
    virtual c4_Handler* CreateHandler(const c4_Property&);

    virtual void SetSize(int size_);

    virtual c4_Notifier* PreChange(c4_Notifier& nf_);
};

/////////////////////////////////////////////////////////////////////////////

#if q4_INLINE
#include "store.inl"
#endif

/////////////////////////////////////////////////////////////////////////////

#endif

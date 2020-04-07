//  Copyright (C) 1996-2000 Jean-Claude Wippler.  All rights reserved.
//
//  This file contains the declaration of the customizing classes.

#ifndef __CUSTOM_H__
#define __CUSTOM_H__

#ifndef __FIELD_H__
#include "field.h"
#endif
#ifndef __STORE_H__
#include "handler.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// Declarations in this file

//  class c4_Sequence;
//      class c4_HandlerSeq;
            class c4_CustomSeq;         // a sequence for customizable views

/////////////////////////////////////////////////////////////////////////////

class c4_CustomSeq : public c4_HandlerSeq
{
    c4_CustomViewer* _viewer;
    bool _inited;

public:
    c4_CustomSeq (c4_CustomViewer* viewer_);
    virtual ~c4_CustomSeq ();

    virtual int Size() const;

    virtual void InsertAt(int, c4_Cursor, int =1);
    virtual void RemoveAt(int, int =1);
    virtual void Move(int from_, int);

    bool DoGet(int row_, int col_, c4_Bytes& buf_) const;
    void DoSet(int row_, int col_, const c4_Bytes& buf_);

private: // this *is* used, as override
    virtual c4_Handler* CreateHandler(const c4_Property&);
};

/////////////////////////////////////////////////////////////////////////////

    extern c4_CustomViewer* f4_CustSlice(c4_Sequence&, int, int , int);
    extern c4_CustomViewer* f4_CustProduct(c4_Sequence&, const c4_View&);
    extern c4_CustomViewer* f4_CustRemapWith(c4_Sequence&, const c4_View&);
    extern c4_CustomViewer* f4_CustPair(c4_Sequence&, const c4_View&);
    extern c4_CustomViewer* f4_CustConcat(c4_Sequence&, const c4_View&);
    extern c4_CustomViewer* f4_CustRename(c4_Sequence&, 
                                            const c4_Property&, const c4_Property&);
    extern c4_CustomViewer* f4_CustGroupBy(c4_Sequence&, 
                                            const c4_View&, const c4_Property&);
    extern c4_CustomViewer* f4_CustJoinProp(c4_Sequence&, 
                                            const c4_ViewProp&, bool);
    extern c4_CustomViewer* f4_CustJoin(c4_Sequence&, 
                                            const c4_View&, const c4_View&, bool);

/////////////////////////////////////////////////////////////////////////////

#endif

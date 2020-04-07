//........................................................................................
//........................................................................................
//.. File Name: field.cpp
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
//  This file contains the implementation of the field classes.

#include "header.h"
#include "field.h"

#include <stdlib.h>     // strtol

#if !q4_INLINE
static char _k4fieldInl[] = "field.inl";
#undef THIS_FILE
#define THIS_FILE _k4fieldInl
#include "field.inl"
#endif

#if q4_CHECK
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Implemented in this file

    class c4_ViewScanner;
    class c4_Field;

/////////////////////////////////////////////////////////////////////////////
// c4_Field

c4_Field::c4_Field (const char*& description_, c4_Field* parent_, int index_)
    : _index (index_), _parent (parent_), _type (0)
{
    d4_assertThis;

    if (_parent == 0)
        _parent = this; // the root field is its own parent
    
    size_t n = strcspn(description_, ",[]");
    const char* p = strchr(description_, ':');

    if (p != 0 && p < description_ + n)
    {
        _name = c4_String (description_, p - description_);
        _type = p[1];
    }
    else
	{
        _name = c4_String (description_, n);
		_type = 'S';
    }

    description_ += n;
        
    if (*description_ == '[')
    {
        ++description_;
		_type = 'V';

		if (*description_ == '^' && *++description_ == ']') // recursive subfield
            _subFields.Add(this);

        if (*description_ == ']')
            ++description_;
        else
            do
                _subFields.Add(d4_new c4_Field (description_, this, NumSubFields()));
            while (*description_++ == ',');
    }
}

c4_Field::~c4_Field ()
{
    d4_assertThis;
    
    //better? for (int i = NumSubFields(); --i >= 0 ;)
    for (int i = 0; i < NumSubFields(); ++i)
	{
		c4_Field* sf = & SubField(i);
		if (sf != this)	// careful with recusrive subfields
			delete sf;
	}
}

c4_String c4_Field::Description(bool anonymous_) const
{
    d4_assertThis;
    
    c4_String s = anonymous_ ? "?" : (const char*) Name();
    if (Type() != 'V')
    {
        s += ':';
        s += Type();
    }
	else
		s += "[" + DescribeSubFields(anonymous_) + "]";
    
    return s;
}
    
c4_String c4_Field::DescribeSubFields(bool anonymous_) const
{
    d4_assertThis;
    d4_assert(Type() == 'V');

	if (NumSubFields() == 1 && this == &SubField(0))
		return "^";

    c4_String s;
    char c = 0;
    
    for (int i = 0; i < NumSubFields(); ++i)
    {
		if (c != 0)
			s += c;
        s += SubField(i).Description();
        c = ',';
    }   
    
    return s;
}
    
/////////////////////////////////////////////////////////////////////////////

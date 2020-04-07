//........................................................................................
//........................................................................................
//.. File Name: field.h
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
//  Field declarations

#ifndef __FIELD_H__
#define __FIELD_H__

#ifndef __K4CONF_H__
#error Please include "k4conf.h" before this header file
#endif
    
/////////////////////////////////////////////////////////////////////////////
// Declarations in this file
    
    class c4_Field;                     // data structure description

/////////////////////////////////////////////////////////////////////////////
//: Fields form a tree describing the underlying data structure.
//
//  This is an internal class.

class c4_Field
{
    int _index;
    c4_PtrArray _subFields;
    c4_Field* _parent;
    c4_String _name;
    char _type;

public: 
/* Construction / destruction */
    c4_Field (const char*&, c4_Field* =0, int index_ =0);
        //: Constructs a new field.
    ~c4_Field ();

/* Structural information */
    bool IsRoot() const;
        //: Returns true if this is the root field.

/* Repeating and compound fields */
    int NumSubFields() const;   
        //: Returns the number of subfields.
    c4_Field& SubField(int) const;
        //: Returns the description of each subfield.
    bool IsRepeating() const;
        //: Returns true if this field contains subtables.
    
/* Field name and description */
    c4_String Name() const;
        //: Returns name of this field.
    char Type() const;
        //: Returns the type description of this field, if any.
    c4_String Description(bool anonymous_ =false) const;
        //: Describes the structure, omit names if anonymous.
	c4_String DescribeSubFields(bool anonymous_ =false) const;
        //: Describes just the subfields, omit names if anonymous.
    
private:
    c4_Field (const c4_Field&);         // not implemented
    void operator= (const c4_Field&);   // not implemented
};

/////////////////////////////////////////////////////////////////////////////

#if q4_INLINE
#include "field.inl"
#endif

/////////////////////////////////////////////////////////////////////////////

#endif

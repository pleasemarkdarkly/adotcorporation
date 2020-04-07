//  Copyright (C) 1996-2000 Jean-Claude Wippler.  All rights reserved.
//
//  This file contains inline definitions for the field classes.
//      
//  Only very small members are selected for inline code.
//  See the class listing for a description of these functions.

d4_inline bool c4_Field::IsRoot() const
{
    return _parent == this;
}

d4_inline bool c4_Field::IsRepeating() const
{
    return _type == 'V';
}

d4_inline int c4_Field::NumSubFields() const
{
    return _subFields.GetSize();
}

d4_inline c4_Field& c4_Field::SubField(int index_) const
{
    return *(c4_Field*) _subFields.GetAt(index_);
}

d4_inline c4_String c4_Field::Name() const
{
    return _name;
}
    
d4_inline char c4_Field::Type() const
{
    return _type;
}

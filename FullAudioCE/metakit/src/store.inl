//  Copyright (C) 1996-2000 Jean-Claude Wippler.  All rights reserved.
//
//  This file contains inline definitions for the storage classes.
//      
//  Only very small members are selected for inline code.
//  See the class listing for a description of these functions.

/////////////////////////////////////////////////////////////////////////////
// c4_Notifier

d4_inline c4_Notifier::c4_Notifier (c4_Sequence* origin_)
    : _origin (origin_), _chain (0), _next (0),
      _type (kNone), _index (0), _propId (0), _count (0), 
      _cursor (0), _bytes (0)
{
    d4_assert(_origin != 0);
}

/////////////////////////////////////////////////////////////////////////////

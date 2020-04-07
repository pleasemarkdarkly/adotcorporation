//  Copyright (C) 1996-2000 Jean-Claude Wippler.  All rights reserved.
//
//  This file contains the implementation of the table classes.
//
//@todo     Don't load data now from flat format if not necessary.
//@todo     Commit over flat format is destructive, shouldn't be.

#include "header.h"
#include "handler.h"
#include "store.h"
#include "field.h"
#include "format.h"
#include "column.h"
#include "persist.h"

#if q4_CHECK
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Implemented in this file

    class c4_Bytes;
    class c4_HandlerSeq;
    
/////////////////////////////////////////////////////////////////////////////
// c4_Bytes

    //@mfunc | c4_Bytes | c4_Bytes | Constructs a binary object.
    //@syntax c4_Bytes::c4_Bytes ();
    //@syntax c4_Bytes::c4_Bytes (const void* <p buf_>, int <p len_>);
    //@syntax c4_Bytes::c4_Bytes (const void* <p buf_>, int <p len_>, bool <p copy_>);
    //@syntax c4_Bytes::c4_Bytes (const c4_Bytes& <p src_>);
    //@parm const void* | buf_ | pointer to original data bytes
    //@parm int | len_ | number of data bytes
    //@parm bool | copy_ | true if a copy should be made
    //@parm const c4_Bytes& | src_ | source data for copy constructor

c4_Bytes::c4_Bytes (const void* buf_, int len_, bool copy_)
    : _contents ((t4_byte*) buf_), _size (len_), _copy (copy_)
{
    if (_copy)
        _MakeCopy();
}

c4_Bytes::c4_Bytes (const c4_Bytes& src_)
    : _contents (src_._contents), _size (src_._size), _copy (src_._copy)
{
    if (_copy || _contents == src_._buffer)
        _MakeCopy();
}

    //@mfunc Assignment, this may make a private copy of contents.
    //@rdesc Reference to the left hand side object.
c4_Bytes& c4_Bytes::operator= (const c4_Bytes& src_)
{
    d4_assertThis;
    
    if (&src_ != this)
    {
        _LoseCopy();

        _contents = src_._contents;
        _size = src_._size;
        _copy = src_._copy;
        
        if (_copy || _contents == src_._buffer)
            _MakeCopy();
    }
    
    return *this;
}

    //@mfunc Swaps the contents and ownership of two byte objects.
    // This operation can often be used to avoid allocation and copying.
void c4_Bytes::Swap(c4_Bytes& bytes_)
{
    d4_assertThis;
    
    t4_byte* p = _contents;
    int s = _size;
    bool c = _copy;

    _contents = bytes_._contents;
    _size = bytes_._size;
    _copy = bytes_._copy;

    bytes_._contents = p;
    bytes_._size = s;
    bytes_._copy = c;

        // if either one is using its local buffer, swap those too
    if (_contents == bytes_._buffer || p == _buffer)
    {
        t4_byte t [kMaxBuf];
        
        memcpy(t, _buffer, kMaxBuf);
        memcpy(_buffer, bytes_._buffer, kMaxBuf);
        memcpy(bytes_._buffer, t, kMaxBuf);

        if (_contents == bytes_._buffer)
            _contents = _buffer;

        if (bytes_._contents == _buffer)
            bytes_._contents = bytes_._buffer;
    }
}

    //@mfunc Defines contents as a freshly allocated buffer of given size.
    //@rdesc Modifiable pointer to the newly allocated buffer.
t4_byte* c4_Bytes::SetBuffer(int length_)
{
    d4_assertThis;

/* No substantial improvement measured:
        Perhaps keep a correctly sized c4_Bytes object in each property?
        It means c4_...Ref objects would need to store a pointer, not an id.

    if (length_ == _size)
        return _contents; // no work needed, get out fast
*/
    _LoseCopy();
    
    _size = length_;
    _copy = _size > kMaxBuf;

    return _contents = _copy ? d4_new t4_byte [_size] : _buffer;
}

    //@mfunc Allocates a buffer and fills its contents with zero bytes.
    //@rdesc Modifiable pointer to the newly allocated and cleared buffer.
t4_byte* c4_Bytes::SetBufferClear(int length_)
{
    d4_assertThis;
    
    return (t4_byte*) memset(SetBuffer(length_), 0, length_);
}

void c4_Bytes::_MakeCopy()
{
    d4_assertThis;
    d4_assert(_contents != 0);
    
    _copy = _size > kMaxBuf;

    _contents = (t4_byte*) memcpy(_copy ? d4_new t4_byte [_size] : _buffer,
                                                        _contents, _size);
}

    //@func Returns true if the contents of both objects is equal.
    //@rdesc True if both objects are equal (in size and byte-for-byte).
bool operator== (const c4_Bytes& a_, const c4_Bytes& b_)
{
    return a_._contents == b_._contents ||
                (a_._size == b_._size &&
                    memcmp(a_._contents, b_._contents, a_._size) == 0);
}

/////////////////////////////////////////////////////////////////////////////

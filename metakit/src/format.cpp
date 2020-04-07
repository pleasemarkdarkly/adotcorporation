//........................................................................................
//........................................................................................
//.. File Name: format.cpp
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
//  This file contains the implementation of the format handler classes.

#include "header.h"
#include "handler.h"
#include "column.h"
#include "format.h"
#include "persist.h"	// 19990906

#if q4_CHECK
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Implemented in this file

//  class c4_Handler;
        class c4_FormatHandler;
            class c4_FormatX;
#if !q4_TINY
                class c4_FormatF;
                class c4_FormatD;
            class c4_FormatM;
#endif
            class c4_FormatB;
            class c4_FormatS;
            class c4_FormatV;

/////////////////////////////////////////////////////////////////////////////

class c4_FormatHandler : public c4_Handler, public c4_Column
{
    int _colNum;
	c4_HandlerSeq& _sequence;

public:
    c4_FormatHandler (const c4_Property& prop_, c4_HandlerSeq& seq_);
    virtual ~c4_FormatHandler ();

    virtual void Initialize(int);
	virtual bool Commit(c4_Streamer& ar_);
	virtual void Prepare(c4_Streamer& ar_);
	virtual bool IsPersistent() const;

protected:
    c4_HandlerSeq& Sequence() const;

    int ColNum() const;
    int RowCount();
    
    t4_byte* CommonBuffer(int len_) const;

    const t4_byte* LoadAt(t4_u32 offset_);
    t4_byte* CopyAt(t4_u32 offset_);

    void InsertData(t4_u32 index_, t4_u32 count_, bool clear_);
    void RemoveData(t4_u32 index_, t4_u32 count_);
};

/////////////////////////////////////////////////////////////////////////////

d4_inline c4_HandlerSeq& c4_FormatHandler::Sequence() const
{
    d4_assertThis;
    
    return _sequence;
}

d4_inline int c4_FormatHandler::ColNum() const
{
    d4_assertThis;
    d4_assert(_colNum >= 0);
    
    return _colNum;
/* doesn't work because some handlers are embedded in others
	for (int i = 0; i < _sequence.NumHandlers(); ++i)
		if (&_sequence.NthHandler(i) == (const c4_Handler*) this)
			return i;

	d4_assert(0);
	return -1;
*/
}

d4_inline int c4_FormatHandler::RowCount()
{
    d4_assertThis;

    return _sequence.Size();
}

d4_inline const t4_byte* c4_FormatHandler::LoadAt(t4_u32 offset_)
{
    d4_assertThis;

    return LoadNow(offset_);
}

d4_inline t4_byte* c4_FormatHandler::CopyAt(t4_u32 offset_)
{
    d4_assertThis;

    return CopyNow(offset_);
}

/////////////////////////////////////////////////////////////////////////////
// c4_FormatHandler

c4_FormatHandler::c4_FormatHandler (const c4_Property& prop_, c4_HandlerSeq& seq_)
	: c4_Handler (prop_),
	  c4_Column (seq_.Persist() ? &seq_.Persist()->Strategy() : 0),
	  _sequence (seq_), _colNum (-1)
{
}

c4_FormatHandler::~c4_FormatHandler ()
{
}

t4_byte* c4_FormatHandler::CommonBuffer(int len_) const
{
    d4_assertThis;

    return Sequence().Buffer().SetBuffer(len_);
}

void c4_FormatHandler::Initialize(int colNum_)
{
    d4_assertThis;
    
    c4_Handler::Initialize(colNum_);

    _colNum = colNum_;
}

void c4_FormatHandler::InsertData(t4_u32 index_, t4_u32 count_, bool clear_)
{
    d4_assertThis;
    d4_assert(index_ <= ColSize());

    if (count_ > 0)
    {
        Grow(index_, count_);

            // clear the contents, in separate chunks if necessary
        if (clear_)
        {
            c4_ColIter iter (*this, index_, index_ + count_);
            while (iter.Next())
                memset(iter.BufSave(), 0, iter.BufLen());
        }
    }
}

void c4_FormatHandler::RemoveData(t4_u32 index_, t4_u32 count_)
{
    d4_assertThis;
    d4_assert(index_ + count_ <= ColSize());

    if (count_ > 0)
        Shrink(index_, count_);
}

bool c4_FormatHandler::Commit(c4_Streamer& ar_)
{
    d4_assertThis;

    bool changed = IsDirty();

    t4_u32 sz = ColSize();
    ar_.StoreValue(sz);

    if (sz > 0)
    {
        if (ar_.IsFlat())
            ar_.WriteColumn(*this);
        else
        {
            if (ar_.Preflighting())
                if (IsDirty())
                    SaveNow(ar_.AllocateCurr(sz));
                else
                    ar_.OccupyNext(Position(), sz);
            else if (Strategy() != 0)
            {
                if (Strategy()->_mapStart != 0 ||
                        !Strategy()->_keepAfterCommit)
                    ReleaseAllSegments();
            }

            d4_assert(!IsDirty());

            ar_.StoreValue(Position());
        }
    }

	return changed;
}

void c4_FormatHandler::Prepare(c4_Streamer& ar_)
{
    d4_assertThis;

    t4_u32 sz = ar_.LoadValue();
    if (sz)
    {
        if (ar_.IsFlat())
        {
			SetBuffer(sz);

                //! should skip and use on-demand loading if possible
            ar_.ReadColumn(*this);
        }
        else
        {
            t4_u32 pos = ar_.LoadValue();
            d4_assert(pos > 0);

			SetLocation(pos, sz);
        
            ar_.OccupyNext(pos, sz);
        }
    }
}

bool c4_FormatHandler::IsPersistent() const
{
    d4_assertThis;
	return Strategy() != 0;
}

/////////////////////////////////////////////////////////////////////////////

class c4_FormatX : public c4_FormatHandler
{
public:
    c4_FormatX (const c4_Property& prop_, c4_HandlerSeq& seq_,
					int width_ =sizeof (t4_i32));

    virtual void Defined();
    virtual void FlipBytes();

    virtual const void* Clear(int& length_) const;

    virtual int ItemSize(int index_);
    virtual const void* Get(int index_, int& length_);
    virtual void Set(int index_, const c4_Bytes& buf_);

    static int DoCompare(const c4_Bytes& b1_, const c4_Bytes& b2_);
    virtual int Compare(int index_, const c4_Bytes& buf_);

    virtual void Insert(int index_, const c4_Bytes& buf_, int count_);
    virtual void Remove(int index_, int count_);

protected:
    typedef void (c4_FormatX::*tGetter) (int);
    typedef int (c4_FormatX::*tSetter) (int, const t4_byte*);

    void Get_0b(int index_);
    void Get_1b(int index_);
    void Get_2b(int index_);
    void Get_4b(int index_);
    void Get_8i(int index_);
    void Get_16i(int index_);
    void Get_16r(int index_);
    void Get_32i(int index_);
    void Get_32r(int index_);
    void Get_64i(int index_);
    void Get_64r(int index_);

    int Set_0b(int index_, const t4_byte* item_);
    int Set_1b(int index_, const t4_byte* item_);
    int Set_2b(int index_, const t4_byte* item_);
    int Set_4b(int index_, const t4_byte* item_);
    int Set_8i(int index_, const t4_byte* item_);
    int Set_16i(int index_, const t4_byte* item_);
    int Set_16r(int index_, const t4_byte* item_);
    int Set_32i(int index_, const t4_byte* item_);
    int Set_32r(int index_, const t4_byte* item_);
    int Set_64i(int index_, const t4_byte* item_);
    int Set_64r(int index_, const t4_byte* item_);

    void SetAccessWidth(int bits_);
    void ResizeData(int index_, int count_, bool clear_ =false);
    void FixSize();

    tGetter _getter;
    tSetter _setter;

    int _currWidth;     // number of bits used for one entry (0..64)
    int _dataWidth;     // number of bytes used for passing a value along

	t4_byte _item[8];	// holds temp result
};

/////////////////////////////////////////////////////////////////////////////

void c4_FormatX::Get_0b(int)
{
    *(t4_i32*) _item = 0;
}

void c4_FormatX::Get_1b(int index_)
{
    t4_u32 off = index_ >> 3;
    d4_assert(off < ColSize());

    *(t4_i32*) _item = (*LoadAt(off) >> (index_ & 7)) & 0x01;
}

void c4_FormatX::Get_2b(int index_)
{
    t4_u32 off = index_ >> 2;
    d4_assert(off < ColSize());

    *(t4_i32*) _item = (*LoadAt(off) >> ((index_ & 3) << 1)) & 0x03;
}

void c4_FormatX::Get_4b(int index_)
{
    t4_u32 off = index_ >> 1;
    d4_assert(off < ColSize());

    *(t4_i32*) _item = (*LoadAt(off) >> ((index_ & 1) << 2)) & 0x0F;
}

void c4_FormatX::Get_8i(int index_)
{
    t4_u32 off = index_;
    d4_assert(off < ColSize());

    *(t4_i32*) _item = *(const char*) LoadAt(off);
}

void c4_FormatX::Get_16i(int index_)
{
    t4_u32 off = index_ * (t4_u32) 2;
    d4_assert(off + 2 <= ColSize());

    const t4_byte* vec = LoadAt(off);

    _item[0] = vec[0];
    _item[1] = vec[1];

    *(t4_i32*) _item = *(const short*) _item;
}

void c4_FormatX::Get_16r(int index_)
{
    t4_u32 off = index_ * (t4_u32) 2;
    d4_assert(off + 2 <= ColSize());

    const t4_byte* vec = LoadAt(off);

    _item[1] = vec[0];
    _item[0] = vec[1];

    *(t4_i32*) _item = *(const short*) _item;
}

void c4_FormatX::Get_32i(int index_)
{
    t4_u32 off = index_ * (t4_u32) 4;
    d4_assert(off + 4 <= ColSize());

    const t4_byte* vec = LoadAt(off);

    _item[0] = vec[0];
    _item[1] = vec[1];
    _item[2] = vec[2];
    _item[3] = vec[3];
}

void c4_FormatX::Get_32r(int index_)
{
    t4_u32 off = index_ * (t4_u32) 4;
    d4_assert(off + 4 <= ColSize());

    const t4_byte* vec = LoadAt(off);

    _item[3] = vec[0];
    _item[2] = vec[1];
    _item[1] = vec[2];
    _item[0] = vec[3];
}

void c4_FormatX::Get_64i(int index_)
{
    t4_u32 off = index_ * (t4_u32) 8;
    d4_assert(off + 8 <= ColSize());

    const t4_byte* vec = LoadAt(off);

    for (int i = 0; i < 8; ++i)
        _item[i] = vec[i];
}

void c4_FormatX::Get_64r(int index_)
{
    t4_u32 off = index_ * (t4_u32) 8;
    d4_assert(off + 8 <= ColSize());

    const t4_byte* vec = LoadAt(off);

    for (int i = 0; i < 8; ++i)
        _item[7-i] = vec[i];
}

/////////////////////////////////////////////////////////////////////////////

    static int fPrecision(t4_i32 v)
    {
        if ((v & ~0x0FL) == 0)
        {
            static int bits[] = { 0, 1, 2, 2, 4, 4, 4, 4, 
                                    4, 4, 4, 4, 4, 4, 4, 4 };
            return bits[(int) v];
        }

        if (v < 0)      // first flip all bits if bit 31 is set
            v = ~ v;    // ... bit 31 is now always zero

            // then check if bits 15-31 used (32b), 7-31 used (16b), else (8b)
        return v >> 15 ? 32 : v >> 7 ? 16 : 8;
    }

int c4_FormatX::Set_0b(int, const t4_byte* item_)
{
    t4_i32 v = *(const t4_i32*) item_;
    return fPrecision(v);
}

int c4_FormatX::Set_1b(int index_, const t4_byte* item_)
{
    t4_i32 v = *(const t4_i32*) item_;

    t4_u32 off = index_ >> 3;
    d4_assert(off < ColSize());

    index_ &= 7;

    t4_byte* p = CopyAt(off);
    *p = (*p & ~(1 << index_)) | (((t4_byte) v & 1) << index_);

    return v >> 1 ? fPrecision(v) : 1;
}

int c4_FormatX::Set_2b(int index_, const t4_byte* item_)
{
    t4_i32 v = *(const t4_i32*) item_;

    t4_u32 off = index_ >> 2;
    d4_assert(off < ColSize());

    const int n = (index_ & 3) << 1;

    t4_byte* p = CopyAt(off);
    *p = (*p & ~(0x03 << n)) | (((t4_byte) v & 0x03) << n);

    return v >> 2 ? fPrecision(v) : 2;
}

int c4_FormatX::Set_4b(int index_, const t4_byte* item_)
{
    t4_i32 v = *(const t4_i32*) item_;

    t4_u32 off = index_ >> 1;
    d4_assert(off < ColSize());

    const int n = (index_ & 1) << 2;

    t4_byte* p = CopyAt(off);
    *p = (*p & ~(0x0F << n)) | (((t4_byte) v & 0x0F) << n);

    return v >> 4 ? fPrecision(v) : 4;
}

int c4_FormatX::Set_8i(int index_, const t4_byte* item_)
{
    t4_i32 v = *(const t4_i32*) item_;

    t4_u32 off = index_;
    d4_assert(off < ColSize());

    *(char*) CopyAt(off) = (char) v;

    return v != (char) v ? fPrecision(v) : 8;
}

int c4_FormatX::Set_16i(int index_, const t4_byte* item_)
{
    t4_i32 v = *(const t4_i32*) item_;

    t4_u32 off = index_ * (t4_u32) 2;
    d4_assert(off + 2 <= ColSize());

    *(short*) CopyAt(off) = (short) v;

    return v != (short) v ? 32 : 16;
}

int c4_FormatX::Set_16r(int index_, const t4_byte* item_)
{
    t4_i32 v = *(const t4_i32*) item_;

    t4_byte buf [2];
    *(short*) buf = (short) v;

    t4_u32 off = index_ * (t4_u32) 2;
    d4_assert(off + 2 <= ColSize());

    t4_byte* vec = CopyAt(off);

    vec[1] = buf[0];
    vec[0] = buf[1];

    return v != (short) v ? 32 : 16;
}

int c4_FormatX::Set_32i(int index_, const t4_byte* item_)
{
    t4_i32 v = *(const t4_i32*) item_;

    t4_u32 off = index_ * (t4_u32) 4;
    d4_assert(off + 4 <= ColSize());

    *(t4_i32*) CopyAt(off) = (t4_i32) v;

    return 32;
}

int c4_FormatX::Set_32r(int index_, const t4_byte* item_)
{
    t4_u32 off = index_ * (t4_u32) 4;
    d4_assert(off + 4 <= ColSize());

    t4_byte* vec = CopyAt(off);

    vec[3] = item_[0];
    vec[2] = item_[1];
    vec[1] = item_[2];
    vec[0] = item_[3];

    return 32;
}

int c4_FormatX::Set_64i(int index_, const t4_byte* item_)
{
    t4_u32 off = index_ * (t4_u32) 8;
    d4_assert(off + 8 <= ColSize());

    t4_byte* vec = CopyAt(off);

    for (int i = 0; i < 8; ++i)
        vec[i] = item_[i];

    return 64;
}

int c4_FormatX::Set_64r(int index_, const t4_byte* item_)
{
    t4_u32 off = index_ * (t4_u32) 8;
    d4_assert(off + 8 <= ColSize());

    t4_byte* vec = CopyAt(off);

    for (int i = 0; i < 8; ++i)
        vec[7-i] = item_[i];

    return 64;
}

/////////////////////////////////////////////////////////////////////////////

c4_FormatX::c4_FormatX (const c4_Property& prop_, c4_HandlerSeq& seq_, int width_)
    : c4_FormatHandler (prop_, seq_), 
      _getter (&c4_FormatX::Get_0b), _setter (&c4_FormatX::Set_0b),
      _currWidth (0), _dataWidth (width_)
{
}

void c4_FormatX::Defined()
{
    d4_assertThis;
    
        // if this was called before and we have data, then ignore next calls
//  if (_currWidth > 0)
//      return;

    c4_FormatHandler::Defined();

	int numRows = RowCount();
    if (numRows > 0)
    {
        t4_u32 sz = ColSize();
        int w = (int) ((sz << 3) / numRows);

            // deduce sub-byte sizes for small vectors, see c4_FormatX::Set
        if (numRows <= 7 && 0 < sz && sz <= 6)
        {
            static t4_byte realWidth [][6] = {
             // sz =  1:  2:  3:  4:  5:  6:
                    {  8, 16,  1, 32,  2,  4  },            //  n = 1
                    {  4,  8,  1, 16,  2,  0  },            //  n = 2
                    {  2,  4,  8,  1,  0, 16  },            //  n = 3
                    {  2,  4,  0,  8,  1,  0  },            //  n = 4
                    {  1,  2,  4,  0,  8,  0  },            //  n = 5
                    {  1,  2,  4,  0,  0,  8  },            //  n = 6
                    {  1,  2,  0,  4,  0,  0  },            //  n = 7
            };

            w = realWidth [numRows-1] [(int) sz - 1];
            d4_assert(w > 0);
        }

        d4_assert((w & (w - 1)) == 0);
        SetAccessWidth(w);
    }
}

void c4_FormatX::FlipBytes()
{
    d4_assertThis;
    
    if (_currWidth > 8)
    {
        int step = _currWidth >> 3;

        c4_ColIter iter (*this, 0, ColSize());
        while (iter.Next(step))
        {
            t4_byte* data = iter.BufSave();
            d4_assert(data != 0);

            for (int j = 0; j < step; ++j)
            {
                t4_byte c = data[j];
                data[j] = data[step-j-1];
                data[step-j-1] = c;
            }
        }
    }
}

void c4_FormatX::SetAccessWidth(int bits_)
{
    d4_assertThis;
    d4_assert((bits_ & (bits_ - 1)) == 0);

    int l2bp1 = 0;  // "log2 bits plus one" needed to represent value
    while (bits_)
    {
        ++l2bp1;
        bits_ >>= 1;
    }
    d4_assert(0 <= l2bp1 && l2bp1 < 8);

    _currWidth = (1 << l2bp1) >> 1;

    if (l2bp1 > 4 && IsForeign())
        l2bp1 += 3; // switch to the trailing entries for byte flipping

        // Metrowerks Codewarrior 11 is dumb, it requires the "&c4_FormatX::"
        
    static tGetter gTab [] =
    {
        &c4_FormatX::Get_0b,    //  0:  0 bits/entry
        &c4_FormatX::Get_1b,    //  1:  1 bits/entry
        &c4_FormatX::Get_2b,    //  2:  2 bits/entry
        &c4_FormatX::Get_4b,    //  3:  4 bits/entry

        &c4_FormatX::Get_8i,    //  4:  8 bits/entry
        &c4_FormatX::Get_16i,   //  5: 16 bits/entry
        &c4_FormatX::Get_32i,   //  6: 32 bits/entry
        &c4_FormatX::Get_64i,   //  7: 64 bits/entry

        &c4_FormatX::Get_16r,   //  8: 16 bits/entry, reversed
        &c4_FormatX::Get_32r,   //  9: 32 bits/entry, reversed
        &c4_FormatX::Get_64r,   // 10: 64 bits/entry, reversed
    };

    static tSetter sTab [] =
    {
        &c4_FormatX::Set_0b,    //  0:  0 bits/entry
        &c4_FormatX::Set_1b,    //  1:  1 bits/entry
        &c4_FormatX::Set_2b,    //  2:  2 bits/entry
        &c4_FormatX::Set_4b,    //  3:  4 bits/entry

        &c4_FormatX::Set_8i,    //  4:  8 bits/entry
        &c4_FormatX::Set_16i,   //  5: 16 bits/entry
        &c4_FormatX::Set_32i,   //  6: 32 bits/entry
        &c4_FormatX::Set_64i,   //  7: 64 bits/entry

        &c4_FormatX::Set_16r,   //  8: 16 bits/entry, reversed
        &c4_FormatX::Set_32r,   //  9: 32 bits/entry, reversed
        &c4_FormatX::Set_64r,   // 10: 64 bits/entry, reversed
    };

    d4_assert(l2bp1 < sizeof gTab / sizeof *gTab);

    _getter = gTab[l2bp1];
    _setter = sTab[l2bp1];

    d4_assert(_getter != 0 && _setter != 0);
}

const void* c4_FormatX::Clear(int& length_) const
{
    d4_assertThis;

    static t4_i32 zero[] = {0, 0};
    d4_assert(sizeof zero >= _dataWidth);

    length_ = _dataWidth;
    return zero;
}

int c4_FormatX::ItemSize(int index_)
{
    d4_assertThis;

    return _currWidth >= 8 ? _currWidth >> 3 : - _currWidth;
}

const void* c4_FormatX::Get(int index_, int& length_)
{
    d4_assertThis;
	d4_assert(sizeof _item >= _dataWidth);

    (this->*_getter)(index_);

    length_ = _dataWidth;
    return _item;
}

void c4_FormatX::Set(int index_, const c4_Bytes& buf_)
{
    d4_assertThis;
    d4_assert(buf_.Size() == _dataWidth);

    int n = (this->*_setter)(index_, buf_.Contents());
    if (n > _currWidth)
    {
        int k = RowCount();

        t4_u32 oldEnd = ColSize();
        t4_u32 newEnd = ((t4_u32) k * n + 7) >> 3;
        
        if (newEnd > oldEnd)
            InsertData(oldEnd, newEnd - oldEnd, _currWidth == 0);

            // data value exceeds width, expand to new size and repeat
        if (_currWidth > 0)
        {
            d4_assert(n % _currWidth == 0); // must be expanding by a multiple

                // To expand, we start by inserting a new appropriate chunk
                // at the end, and expand the entries in place (last to first).

            tGetter oldGetter = _getter;
            SetAccessWidth(n);
            
            d4_assert(sizeof _item >= _dataWidth);

                // this expansion in place works because it runs backwards
            while (--k >= 0)
            {
                (this->*oldGetter)(k);
                (this->*_setter)(k, _item);
            }
        }
        else
        {
            if (_dataWidth > (int) sizeof (t4_i32))
                n = _dataWidth << 3; // don't trust setter result, use max instead

            SetAccessWidth(n);
        }

        FixSize();

            // now repeat the failed call to _setter
        int w = (this->*_setter)(index_, buf_.Contents());
        d4_assert(w <= _currWidth);
    }
}

int c4_FormatX::DoCompare(const c4_Bytes& b1_, const c4_Bytes& b2_)
{
    d4_assert(b1_.Size() == sizeof (t4_i32));
    d4_assert(b2_.Size() == sizeof (t4_i32));

    t4_i32 v1 = *(const t4_i32*) b1_.Contents();
    t4_i32 v2 = *(const t4_i32*) b2_.Contents();
    
    return v1 == v2 ? 0 : v1 < v2 ? -1 : +1;
}

int c4_FormatX::Compare(int index_, const c4_Bytes& buf_)
{
    d4_assertThis;

		// make a copy, since we probably are using the common _item buffer
	c4_Bytes copy (buf_.Contents(), buf_.Size(), true);

    c4_Bytes data;
    GetBytes(index_, data);

    return DoCompare(data, copy);
}

void c4_FormatX::Insert(int index_, const c4_Bytes& buf_, int count_)
{
    d4_assertThis;
    d4_assert(buf_.Size() == _dataWidth);
    d4_assert(count_ > 0);

    bool clear = true;
    const t4_byte* ptr = buf_.Contents();

    for (int i = 0; i < _dataWidth; ++i)
        if (*ptr++)
        {
            clear = false;
            break;
        }

    ResizeData(index_, count_, clear);

    if (!clear)
        while (--count_ >= 0)
            Set(index_++, buf_);
}

void c4_FormatX::Remove(int index_, int count_)
{
    d4_assertThis;
    d4_assert(count_ > 0);

    ResizeData(index_, - count_);
}

void c4_FormatX::ResizeData(int index_, int count_, bool clear_)
{
    d4_assertThis;

    if (!(_currWidth & 7))  // not 1, 2, or 4
    {
        const t4_u32 w = (t4_u32) (_currWidth >> 3);

        if (count_ > 0)
            InsertData(index_ * w, count_ * w, clear_);
        else
            RemoveData(index_ * w, - count_ * w);

        return;
    }

    d4_assert(_currWidth == 1 || _currWidth == 2 || _currWidth == 4);

        /*
            _currwidth      1:  2:  4:
            
            shiftPos         3   2   1      shift the offset right this much
            maskPos          7   3   1      mask the offset with this
        */

    const int shiftPos = _currWidth == 4 ? 1 : 4 - _currWidth;
    const int maskPos = (1 << shiftPos) - 1;

    // the following code is similar to c4_Column::Resize, but at bit level

        // turn insertion into deletion by inserting entire bytes
    if (count_ > 0)
    {
        unsigned off = (unsigned) index_ >> shiftPos;
        int gapBytes = (count_ + maskPos) >> shiftPos;

        InsertData(off, gapBytes, clear_);

            // oops, we might have inserted too low by a few entries
        const int bits = (index_ & maskPos) * _currWidth;
        if (bits)
        {
            const int maskLow = (1 << bits) - 1;

                // move the first few bits to start of inserted range
            t4_byte* p = CopyAt(off + gapBytes);
            t4_byte one = *p & maskLow;
            *p &= ~maskLow;

            * CopyAt(off) = one;
        }

        index_ += count_;
        count_ -= gapBytes << shiftPos;
        d4_assert(count_ <= 0);
    }

        // now perform a deletion using a forward loop to copy down
    if (count_ < 0)
    {
        int from = index_ - count_;
        int adjust = 0;

        c4_Bytes temp;
        t4_i32 n = ColSize() << shiftPos;

        while (from < n)
        {
#if 0 // no optimization: wrong & little gain expected unless exact bytes
                // if on a byte boundary, try removing full bytes
            if (0 && (index_ & maskPos) == 0 && index_ + maskPos < from)
            {
                int skipBytes = (from - index_) >> shiftPos;
                d4_assert(skipBytes);

                RemoveData(index_ >> shiftPos, skipBytes);

                    // the from position has been moved down by this amount
                adjust = - skipBytes << shiftPos;
            }
            else
#endif
            {
                GetBytes(from++ + adjust, temp);
                Set(index_++, temp);
            }
        }
    }
    else
        d4_assert(count_ == 0);

    FixSize();
}

void c4_FormatX::FixSize()
{
    int n = RowCount();
    t4_u32 needBytes = ((t4_u32) n * _currWidth + 7) >> 3;

        // use a special trick to mark sizes less than 1 byte in storage
    if (1 <= n && n <= 4 && (_currWidth & 7))
    {
        const int shiftPos = _currWidth == 4 ? 1 : 4 - _currWidth;

        static t4_byte fakeSizes [3][4] = { //  n:  1:  2:  3:  4:
            { 6, 1, 2, 2 },     //  4-bit entries:   4b  8b 12b 16b
            { 5, 5, 1, 1 },     //  2-bit entries:   2b  4b  6b  8b
            { 3, 3, 4, 5 },     //  1-bit entries:   1b  2b  3b  4b
        };

            // The idea is to use an "impossible" size (ie. 5, for n = 2)
            // to give information about the current bit packing density.
        d4_assert(needBytes <= 2);
        needBytes = fakeSizes [shiftPos-1] [n-1];
    }

    t4_u32 currSize = ColSize();

    if (needBytes < currSize)
        RemoveData(needBytes, currSize - needBytes);
    else if (needBytes > currSize)
        InsertData(currSize, needBytes - currSize, true);
}

/////////////////////////////////////////////////////////////////////////////
#if !q4_TINY
/////////////////////////////////////////////////////////////////////////////

class c4_FormatF : public c4_FormatX
{
public:
    c4_FormatF (const c4_Property& prop_, c4_HandlerSeq& seq_);
    virtual ~c4_FormatF ();

    static int DoCompare(const c4_Bytes& b1_, const c4_Bytes& b2_);
    virtual int Compare(int index_, const c4_Bytes& buf_);
};

/////////////////////////////////////////////////////////////////////////////

c4_FormatF::c4_FormatF (const c4_Property& prop_, c4_HandlerSeq& seq_)
    : c4_FormatX (prop_, seq_, sizeof (float))
{
}

c4_FormatF::~c4_FormatF ()
{
}

int c4_FormatF::DoCompare(const c4_Bytes& b1_, const c4_Bytes& b2_)
{
    d4_assert(b1_.Size() == sizeof (float));
    d4_assert(b2_.Size() == sizeof (float));

    float v1 = *(const float*) b1_.Contents();
    float v2 = *(const float*) b2_.Contents();
    
    return v1 == v2 ? 0 : v1 < v2 ? -1 : +1;
}

int c4_FormatF::Compare(int index_, const c4_Bytes& buf_)
{
    d4_assertThis;

	c4_Bytes copy (buf_.Contents(), buf_.Size(), true);

    c4_Bytes data;
    GetBytes(index_, data);

    return DoCompare(data, copy);
}

/////////////////////////////////////////////////////////////////////////////

class c4_FormatD : public c4_FormatX
{
public:
    c4_FormatD (const c4_Property& prop_, c4_HandlerSeq& seq_);
    virtual ~c4_FormatD ();

    virtual void Initialize(int);

    static int DoCompare(const c4_Bytes& b1_, const c4_Bytes& b2_);
    virtual int Compare(int index_, const c4_Bytes& buf_);
};

/////////////////////////////////////////////////////////////////////////////

c4_FormatD::c4_FormatD (const c4_Property& prop_, c4_HandlerSeq& seq_)
    : c4_FormatX (prop_, seq_, sizeof (double))
{
}

c4_FormatD::~c4_FormatD ()
{
}

void c4_FormatD::Initialize(int colNum_)
{
    d4_assertThis;
    
    c4_FormatX::Initialize(colNum_);

        // force maximum size, autosizing more than 32 bits won't work
    SetAccessWidth(8 * sizeof (double));
}

int c4_FormatD::DoCompare(const c4_Bytes& b1_, const c4_Bytes& b2_)
{
    d4_assert(b1_.Size() == sizeof (double));
    d4_assert(b2_.Size() == sizeof (double));

    double v1 = *(const double*) b1_.Contents();
    double v2 = *(const double*) b2_.Contents();
    
    return v1 == v2 ? 0 : v1 < v2 ? -1 : +1;
}

int c4_FormatD::Compare(int index_, const c4_Bytes& buf_)
{
    d4_assertThis;
    d4_assert(sizeof (double) * (index_ + 1) <= ColSize());

	c4_Bytes copy (buf_.Contents(), buf_.Size(), true);

    c4_Bytes data;
    GetBytes(index_, data);

    return DoCompare(data, copy);
}

/////////////////////////////////////////////////////////////////////////////

class c4_FormatM : public c4_FormatHandler
{
public:
    c4_FormatM (const c4_Property& prop_, c4_HandlerSeq& seq_);
    virtual ~c4_FormatM ();

    virtual void Initialize(int);
	virtual bool Commit(c4_Streamer& ar_);
	virtual void Prepare(c4_Streamer& ar_);

    virtual void Defined();

    virtual int ItemSize(int index_);
    virtual const void* Get(int index_, int& length_);
    virtual void Set(int index_, const c4_Bytes& buf_);

    static int DoCompare(const c4_Bytes& b1_, const c4_Bytes& b2_);
    virtual int Compare(int index_, const c4_Bytes& buf_);

    virtual void Insert(int index_, const c4_Bytes& buf_, int count_);
    virtual void Remove(int index_, int count_);

    virtual c4_Column* GetNthMemoCol(int index_, bool alloc_ =false);
    virtual void SetNthMemoPos(int index_, t4_u32 pos_, t4_u32 len_,
								c4_Column* newCol_ =0);

private:
    t4_u32 ItemPos(int index_);
    t4_u32 ItemLen(int index_);

    c4_FormatX _sizes;
    c4_FormatX _positions;
    c4_PtrArray _items;
};

/////////////////////////////////////////////////////////////////////////////

c4_FormatM::c4_FormatM (const c4_Property& prop_, c4_HandlerSeq& seq_)
    : c4_FormatHandler (prop_, seq_), _sizes (prop_, seq_), _positions (prop_, seq_)
{
}

c4_FormatM::~c4_FormatM ()
{
    d4_assertThis;

        // cleanup allocated columns
    //better? for (int i = _items.GetSize(); --i >= 0 ;)
    for (int i = 0; i < _items.GetSize(); ++i)
        delete (c4_Column*) _items.GetAt(i);
}

t4_u32 c4_FormatM::ItemPos(int index_)
{
    d4_assertThis;

    int length;
    const t4_u32* ptr = (const t4_u32*) _positions.Get(index_, length);
    d4_assert(length == (int) sizeof (t4_u32));

    return *ptr;
}

t4_u32 c4_FormatM::ItemLen(int index_)
{
    d4_assertThis;

    int length;
    const t4_u32* ptr = (const t4_u32*) _sizes.Get(index_, length);
    d4_assert(length == (int) sizeof (t4_u32));

    return *ptr;
}

c4_Column* c4_FormatM::GetNthMemoCol(int index_, bool alloc_)
{
    d4_assertThis;

    c4_Column* col = (c4_Column*) _items.GetAt(index_);
    if (!col && (alloc_ || ItemLen(index_) > 0))
    {
        col = d4_new c4_Column (Strategy());
        col->SetLocation(ItemPos(index_), ItemLen(index_));
        _items.SetAt(index_, col);
    }

    return col;
}

void c4_FormatM::SetNthMemoPos(int index_, t4_u32 pos_, t4_u32 len_,
							   c4_Column* newCol_)
{
    d4_assertThis;

    c4_Bytes temp (&pos_, sizeof pos_);
    _positions.Set(index_, temp);

    c4_Bytes temp2 (&len_, sizeof len_);
    _sizes.Set(index_, temp2);

	if (newCol_) // new 19990903 for c4_Table::ExchangeEntries
		_items.SetAt(index_, newCol_);
}

void c4_FormatM::Initialize(int colNum_)
{
    c4_FormatHandler::Initialize(colNum_);

    _sizes.Initialize(colNum_);
    _positions.Initialize(colNum_);
}

void c4_FormatM::Defined()
{
    d4_assertThis;
    
    _sizes.Defined();
    _positions.Defined();

    _items.SetSize(RowCount());
}

int c4_FormatM::ItemSize(int index_)
{
    d4_assertThis;

		// use the column if present, since size may have changed
    c4_Column* col = GetNthMemoCol(index_);
    return (int) col ? col->ColSize() : ItemLen(index_);
}

const void* c4_FormatM::Get(int index_, int& length_)
{
    d4_assertThis;

    int n = length_ = ItemSize(index_);

    if (n == 0)
        return "";

    d4_assert(GetNthMemoCol(index_) != 0);
    c4_ColIter iter (*GetNthMemoCol(index_), 0, n);
    iter.Next();

        // most common case, all bytes are inside the same segment
    if (iter.BufLen() == n)
        return iter.BufLoad(); // avoid copying

        // else set up a copy of the contents, in separate chunks
    t4_byte* buf = CommonBuffer(n);

    t4_byte* p = buf;
    do {
        memcpy(p, iter.BufLoad(), iter.BufLen());
        p += iter.BufLen();
    } while (iter.Next());

    length_ = n;
    return buf;
}

void c4_FormatM::Set(int index_, const c4_Bytes& buf_)
{
    d4_assertThis;

    int m = buf_.Size();
    int n = m - (int) ItemSize(index_);
    
    if (n == 0 && m == 0)
        return; // no size change and no contents

        // set the contents, in separate chunks if necessary
    const t4_byte* p = buf_.Contents();

	if (m == 0) // fixed 19990828: clearing a memo crashed 1.8.6
	{
		GetNthMemoCol(index_, true)->SetBufferClear(0);
	}
	else
	{
		GetNthMemoCol(index_, true)->SetBuffer(m);

		c4_ColIter iter (*GetNthMemoCol(index_), 0, m);
		while (iter.Next(m)) // 1999-12-20: add "m" to prevent read-ahead optim
		{
			memcpy(iter.BufSave(), p, iter.BufLen());
			p += iter.BufLen();
		}
	}

    if (n)
    {
            // adjust the stored size
        c4_Bytes dBuf;
        *(t4_i32*) dBuf.SetBuffer(sizeof (t4_i32)) = buf_.Size();
        _sizes.Set(index_, dBuf);
    
			// clear the position on file, will be reallocated later
		dBuf.SetBufferClear(sizeof (t4_i32));
		_positions.Set(index_, dBuf);
    }
}

int c4_FormatM::DoCompare(const c4_Bytes& b1_, const c4_Bytes& b2_)
{
    int n = b1_.Size();
    if (n > b2_.Size())
        n = b2_.Size();

    int f = memcmp(b1_.Contents(), b2_.Contents(), n);
    return f ? f : b1_.Size() - b2_.Size();
}

int c4_FormatM::Compare(int index_, const c4_Bytes& buf_)
{
    d4_assertThis;

    c4_Bytes data;
    GetBytes(index_, data);

    return DoCompare(data, buf_);
}

void c4_FormatM::Insert(int index_, const c4_Bytes& buf_, int count_)
{
    d4_assertThis;
    d4_assert(count_ > 0);

        // insert empty size and position entries
    c4_Bytes dBuf;
    dBuf.SetBufferClear(sizeof (t4_i32));

    _sizes.Insert(index_, dBuf, count_);
    _positions.Insert(index_, dBuf, count_);
    _items.InsertAt(index_, 0, count_);

        // now set their contents
    while (--count_ >= 0)
        Set(index_++, buf_);
}

void c4_FormatM::Remove(int index_, int count_)
{
    d4_assertThis;

        // remove the items themselves
    for (int i = 0; i < count_; ++i)
        delete GetNthMemoCol(index_ + i);

        // remove the size and position entries
    _sizes.Remove(index_, count_);
    _positions.Remove(index_, count_);

    _items.RemoveAt(index_, count_);
}

bool c4_FormatM::Commit(c4_Streamer& ar_)
{
    d4_assertThis;

    bool changed = false;

    if (!ar_.IsFlat())
    {
        for (int r = 0; r < RowCount(); ++r)
        {
            c4_Column* mc = GetNthMemoCol(r);
            if (mc)
            {
                t4_u32 msz = mc->ColSize();
                if (msz > 0)
                    if (ar_.Preflighting())
                        if (mc->IsDirty())
                        {
                            t4_u32 mpos = ar_.AllocateCurr(msz);
                            SetNthMemoPos(r, mpos, msz);

                            mc->SaveNow(mpos);
                        }
                        else
                            ar_.OccupyNext(mc->Position(), msz);
                    else
                        mc->ReleaseAllSegments();
            }
        }
    }

	if (_sizes.Commit(ar_))
		changed = true;

    if (ar_.IsFlat())
    {
        for (int r = 0; r < RowCount(); ++r)
        {
            c4_Column* mc = GetNthMemoCol(r);
            if (mc)
                ar_.WriteColumn(*mc);
        }
    }
	else if (_positions.Commit(ar_))
		changed = true;
	
	return changed;
}

void c4_FormatM::Prepare(c4_Streamer& ar_)
{
    d4_assertThis;

	_sizes.Prepare(ar_);
	if (!ar_.IsFlat())
		_positions.Prepare(ar_);

    Defined(); // special early define to allow access to size and pos

    for (int r = 0; r < RowCount(); ++r)
    {
        c4_Column* mc =  GetNthMemoCol(r);
        if (mc)
                // in flat format, read all memo fields afterwards
            if (ar_.IsFlat())
                ar_.ReadColumn(*mc);
            else // in on-demand format, track allocated storage
                ar_.OccupyNext(mc->Position(), mc->ColSize());
    }
}

/////////////////////////////////////////////////////////////////////////////
#endif // !q4_TINY
/////////////////////////////////////////////////////////////////////////////

class c4_FormatB : public c4_FormatHandler
{
public:
    c4_FormatB (const c4_Property& prop_, c4_HandlerSeq& seq_);

    virtual void Initialize(int);
	virtual bool Commit(c4_Streamer& ar_);
	virtual void Prepare(c4_Streamer& ar_);

    virtual void Defined();

    virtual int ItemSize(int index_);
    virtual const void* Get(int index_, int& length_);
    virtual void Set(int index_, const c4_Bytes& buf_);

    static int DoCompare(const c4_Bytes& b1_, const c4_Bytes& b2_);
    virtual int Compare(int index_, const c4_Bytes& buf_);

    virtual void Insert(int index_, const c4_Bytes& buf_, int count_);
    virtual void Remove(int index_, int count_);

protected:
    c4_FormatX _sizes;

private:
    t4_u32 Offset(int index_) const;

    c4_DWordArray _offsets;
};

/////////////////////////////////////////////////////////////////////////////

c4_FormatB::c4_FormatB (const c4_Property& prop_, c4_HandlerSeq& seq_)
    : c4_FormatHandler (prop_, seq_), _sizes (prop_, seq_)
{
}

d4_inline t4_u32 c4_FormatB::Offset(int index_) const
{
    d4_assertThis;
    d4_assert(_offsets.GetAt(_offsets.GetSize() - 1) == ColSize());

        // extend offset vectors for missing empty entries at end 
    int n = _offsets.GetSize();
    d4_assert(n > 0);

    if (index_ >= n)
        index_ = n - 1;

    return _offsets.GetAt(index_);
}

void c4_FormatB::Initialize(int colNum_)
{
    c4_FormatHandler::Initialize(colNum_);

    _sizes.Initialize(colNum_);

    _offsets.SetSize(1, 100);
    _offsets.SetAt(0, 0);
}

/*
    class Tally
    {
        int _id;
        long _count;
    public:
        Tally (int id_ =0) : _id (id_), _count (0) { }
        ~Tally () { printf("Tally %2d = %8ld\n", _id, _count); }
        void Step() { ++_count; }
    };
*/

void c4_FormatB::Defined()
{
    d4_assertThis;
    
    _sizes.Defined();

	int numRows = RowCount();
    _offsets.SetSize(numRows + 1, 10 + numRows / 16);       // optimization

    c4_Bytes dBuf;
    t4_i32 off = 0;

    int last = 0, length;

    for (int i = 0; i < numRows; ++i)
    {
		const t4_i32* ptr = (const t4_i32*) _sizes.Get(i, length);
        d4_assert(length == (int) sizeof (t4_i32));

        if (*ptr > 0)
            last = i + 1;

        off += *ptr;
        _offsets.SetAt(i + 1, off);
    }

        // optimization: get rid of all trailing entries that are empty
    _offsets.SetSize(last + 1);
}

int c4_FormatB::ItemSize(int index_)
{
    d4_assertThis;

    return Offset(index_ + 1) - Offset(index_);
}

const void* c4_FormatB::Get(int index_, int& length_)
{
    d4_assertThis;

    t4_u32 start = Offset(index_);
    t4_u32 limit = Offset(index_ + 1);
    
    int n = length_ = (int) (limit - start);
    d4_assert(n >= 0);

    if (n == 0)
        return "";

    c4_ColIter iter (*this, start, limit);
    iter.Next();

        // most common case, all bytes are inside the same segment
    if (iter.BufLen() == n)
        return iter.BufLoad(); // avoid copying

        // else set up a copy of the contents, in separate chunks
    t4_byte* buf = CommonBuffer(n);

    t4_byte* p = buf;
    do {
        memcpy(p, iter.BufLoad(), iter.BufLen());
        p += iter.BufLen();
    } while (iter.Next());

    length_ = n;
    return buf;
}

void c4_FormatB::Set(int index_, const c4_Bytes& buf_)
{
    d4_assertThis;

    int m = buf_.Size();
    t4_u32 start = Offset(index_);
    int n = m - (int) (Offset(index_ + 1) - start);
    
    if (n > 0)
        Grow(start, (t4_u32) n);
    else if (n < 0)
        Shrink(start, (t4_u32) - n);
    else if (m == 0)
        return; // no size change and no contents

        // set the contents, in separate chunks if necessary
    const t4_byte* p = buf_.Contents();

    c4_ColIter iter (*this, start, start + m);
    while (iter.Next(m)) // 1999-12-20: add "m" to prevent read-ahead optim
    {
        memcpy(iter.BufSave(), p, iter.BufLen());
        p += iter.BufLen();
    }

    if (n)  // if size has changed
    {
            //adjust the stored size
        c4_Bytes dBuf;
        *(t4_i32*) dBuf.SetBuffer(sizeof (t4_i32)) = buf_.Size();
        _sizes.Set(index_, dBuf);

        int k = _offsets.GetSize() - 1;

            // if filling in an empty entry at end: extend offsets first
        if (m > 0 && index_ >= k)
        {
            _offsets.InsertAt(k, _offsets.GetAt(k), index_ - k + 1);
            
            k = index_ + 1;
            d4_assert(k == _offsets.GetSize() - 1);
        }

            // adjust following entry offsets
        while (++index_ <= k)
            _offsets.ElementAt(index_) += n;
    }

    d4_assert(_offsets.GetAt(_offsets.GetSize() - 1) == ColSize());
}

int c4_FormatB::DoCompare(const c4_Bytes& b1_, const c4_Bytes& b2_)
{
    int n = b1_.Size();
    if (n > b2_.Size())
        n = b2_.Size();

    int f = memcmp(b1_.Contents(), b2_.Contents(), n);
    return f ? f : b1_.Size() - b2_.Size();
}

int c4_FormatB::Compare(int index_, const c4_Bytes& buf_)
{
    d4_assertThis;

    c4_Bytes data;
    GetBytes(index_, data);

    return DoCompare(data, buf_);
}

void c4_FormatB::Insert(int index_, const c4_Bytes& buf_, int count_)
{
    d4_assertThis;
    d4_assert(count_ > 0);

    int m = buf_.Size();
    t4_u32 off = Offset(index_);

        // insert the appropriate number of bytes
    t4_u32 n = count_ * (t4_u32) m;

    if (n > 0)
    {
        Grow(off, n);

            // store as many copies as needed, but be prepared to do it in chunks
        int spos = 0;

        c4_ColIter iter (*this, off, off + n);
        while (iter.Next(m - spos))
        {
            memcpy(iter.BufSave(), buf_.Contents() + spos, iter.BufLen());

            spos += iter.BufLen();
            if (spos >= m)
                spos = 0;
        }

        d4_assert(spos == 0); // must have copied an exact multiple of the data
    }

        // insert the size entries themselves
    c4_Bytes dBuf;
    *(t4_i32*) dBuf.SetBuffer(sizeof (t4_i32)) = m;
    _sizes.Insert(index_, dBuf, count_);

        // optimization: don't create trailing offsets for empty entries
    if (m > 0 || index_ < _offsets.GetSize() - 1)
    {
        int k = _offsets.GetSize();
        if (index_ >= k)
            _offsets.InsertAt(k, _offsets.GetAt(k - 1), index_ - k + 1);

            // define offsets of the new entries
        _offsets.InsertAt(index_, 0, count_);
        d4_assert(_offsets.GetSize() <= RowCount() + 1);

        while (--count_ >= 0)
        {
            _offsets.SetAt(index_++, off);
            off += m;
        }

        d4_assert(index_ < _offsets.GetSize());

            // adjust all following entries
        while (index_ < _offsets.GetSize())
            _offsets.ElementAt(index_++) += n;

        d4_assert(_offsets.GetAt(index_ - 1) == ColSize());
    }

    d4_assert(index_ <= RowCount() + 1);
}

void c4_FormatB::Remove(int index_, int count_)
{
    d4_assertThis;

    t4_u32 off = Offset(index_);
    t4_i32 n = Offset(index_ + count_) - off;
    d4_assert(n >= 0);

    if (n > 0)
        Shrink(off, n);

        // remove the size entries themselves
    _sizes.Remove(index_, count_);

        // and the offset entries
    if (index_ + count_ > _offsets.GetSize() - 1)
    {
            // if short offsets vector, leave the last entry in at all times
        count_ = _offsets.GetSize() - index_ - 1;
        if (count_ <= 0)
            return;
    }

    _offsets.RemoveAt(index_, count_);
    d4_assert(_offsets.GetSize() <= RowCount() + 1);

    d4_assert(index_ < _offsets.GetSize());

        // adjust all following entries
    while (index_ < _offsets.GetSize())
        _offsets.ElementAt(index_++) -= n;

    d4_assert(_offsets.GetAt(index_ - 1) == ColSize());
    d4_assert(index_ <= RowCount() + 1);
}

bool c4_FormatB::Commit(c4_Streamer& ar_)
{
    d4_assertThis;

    bool changed = c4_FormatHandler::Commit(ar_);
	return changed | _sizes.Commit(ar_); // don't uses short-circuiting here
}

void c4_FormatB::Prepare(c4_Streamer& ar_)
{
    d4_assertThis;

	c4_FormatHandler::Prepare(ar_);
	_sizes.Prepare(ar_);
}

/////////////////////////////////////////////////////////////////////////////

class c4_FormatS : public c4_FormatHandler
{
public:
    c4_FormatS (const c4_Property& prop_, c4_HandlerSeq& seq_);

    virtual const void* Clear(int& length_) const;

    virtual int ItemSize(int index_);
    virtual const void* Get(int index_, int& length_);
    virtual void Set(int index_, const c4_Bytes& buf_);

    static int DoCompare(const c4_Bytes& b1_, const c4_Bytes& b2_);
    virtual int Compare(int index_, const c4_Bytes& buf_);

    virtual void Insert(int index_, const c4_Bytes& buf_, int count_);
    virtual void Remove(int index_, int count_);

private:
    t4_u32 SetUpOffsets(int index_);
    t4_u32 Offset(int index_);
    void Realize(t4_u32 pos_);

    c4_DWordArray _offsets;
};

/////////////////////////////////////////////////////////////////////////////

c4_FormatS::c4_FormatS (const c4_Property& prop_, c4_HandlerSeq& seq_)
    : c4_FormatHandler (prop_, seq_)
{
}

t4_u32 c4_FormatS::SetUpOffsets(int index_)
{
    d4_assertThis;

    int k = _offsets.GetSize();
    if (k > 0)
	{
        --k;
            // 25-7-1999, optimize indexing off the end
        if (index_ > k && _offsets.GetAt(k) >= ColSize())
            return ColSize() + index_ - k;
    }

    _offsets.SetSize(index_ + 1, 10 + k / 16);

    t4_u32 pos = _offsets.GetAt(k);

    c4_ColIter iter (*this, pos, ColSize());
    while (iter.Next())
    {
        for (int j = 0; j < iter.BufLen(); ++j)
            if (!iter.BufLoad()[j])
            {
                if (++k >= _offsets.GetSize())
                    _offsets.SetSize(k + 1000 + k / 8); // add more spares

                _offsets.SetAt(k, pos + j + 1);
            }

        pos += iter.BufLen();
    }

    d4_assert(pos == ColSize());

    _offsets.SetSize(k + 1);                // fix the final size

        // if the end doesn't exist yet, it's one zero byte per entry
    if (index_ >= _offsets.GetSize())
        return ColSize() + index_ - k;

    return _offsets.GetAt(index_);
}

t4_u32 c4_FormatS::Offset(int index_)
{
    d4_assertThis;

    if (index_ >= _offsets.GetSize())
		return SetUpOffsets(index_);

    return _offsets.GetAt(index_);
}

void c4_FormatS::Realize(t4_u32 pos_)
{
    d4_assertThis;

    if (ColSize() < pos_)
        InsertData(ColSize(), pos_ - ColSize(), true);
}

const void* c4_FormatS::Clear(int& length_) const
{
    d4_assertThis;
    
    length_ = 1;
    return "";
}

int c4_FormatS::ItemSize(int index_)
{
    d4_assertThis;

    return Offset(index_ + 1) - Offset(index_) - 1;
}

const void* c4_FormatS::Get(int index_, int& length_)
{
    d4_assertThis;

    t4_u32 start = Offset(index_);
    t4_u32 limit = Offset(index_ + 1);
    
    int n = length_ = (int) (limit - start);
    d4_assert(n > 0);

        // get the contents, in separate chunks if necessary

    if (n <= 1) // don't go past the real end for empty/fake strings
        return "";

    c4_ColIter iter (*this, start, limit);
    iter.Next();

        // most common case, all bytes are inside the same segment
    if (iter.BufLen() == n)
        return iter.BufLoad(); // avoid copying

        // else set up a copy of the contents, in separate chunks
    t4_byte* buf = CommonBuffer(n);

    t4_byte* p = buf;
    p[n-1] = 0;

    do {
        memcpy(p, iter.BufLoad(), iter.BufLen());
        p += iter.BufLen();
    } while (iter.Next());

    length_ = n;
    return buf;
}

void c4_FormatS::Set(int index_, const c4_Bytes& buf_)
{
    d4_assertThis;

    int m = buf_.Size();
    if (m > 0)
    {
        --m;
        d4_assert(buf_.Contents()[m] == 0);
    }

    t4_u32 off = Offset(index_);
    int n = m - (int) (Offset(index_ + 1) - off) + 1;
    
    if (n > 0)
    {
        Realize(off);
        InsertData(off, (t4_u32) n, false);
    }
    else if (n < 0)
    {
        Realize(off - n);
        RemoveData(off, (t4_u32) - n);
    }
    else if (m == 0)
        return; // no size change and no contents

        // set the contents, in separate chunks if necessary
    const t4_byte* p = buf_.Contents();

    c4_ColIter iter (*this, off, off + m);
    while (iter.Next(m)) // 1999-12-20: add "m" to prevent read-ahead optim
    {
        memcpy(iter.BufSave(), p, iter.BufLen());
        p += iter.BufLen();
    }

        // adjust following entry offsets
    if (n)
        while (++index_ < _offsets.GetSize())
            _offsets.ElementAt(index_) += n;
}

int c4_FormatS::DoCompare(const c4_Bytes& b1_, const c4_Bytes& b2_)
{
    c4_String v1 ((const char*) b1_.Contents(), b1_.Size());
    c4_String v2 ((const char*) b2_.Contents(), b2_.Size());
    
    return v1.CompareNoCase(v2);
}

int c4_FormatS::Compare(int index_, const c4_Bytes& buf_)
{
    d4_assertThis;

    c4_Bytes data;
    GetBytes(index_, data);

    return DoCompare(data, buf_);
}

void c4_FormatS::Insert(int index_, const c4_Bytes& buf_, int count_)
{
    d4_assertThis;
    d4_assert(count_ > 0);

    int m = buf_.Size();
    if (m > 0)
    {
        --m;
        d4_assert(buf_.Contents()[m] == 0);
    }

    t4_u32 off = Offset(index_);

    if (m == 0 && ColSize() <= off)
        return; // ignored if only inserting empty strings past current end

        // insert the appropriate number of bytes
    t4_u32 n = count_ * ((t4_u32) m + 1);

    Realize(off); // make sure the column data exists
    InsertData(off, n, false);

        // store as many copies as needed, but be prepared to do it in chunks
    {
        t4_u32 toff = off;
        t4_u32 tlen = n;
        int spos = 0;

        while (tlen > 0)
        {
            int l = AvailAt(toff);
            if ((t4_u32) l > tlen)
                l = (int) tlen;

            t4_byte* data = CopyNow(toff);

            if (spos < m)
            {
                if (l > m - spos)
                    l = m - spos;

                memcpy(data, buf_.Contents() + spos, l);

                spos += l;
            }
            else
            {
                *data = 0;  // each string is terminated with a null byte
                spos = 0;
                l = 1;
            }

            toff += l;
            tlen -= l;
        }

        d4_assert(spos == 0); // must have copied an exact multiple of the data
    }

        // some offset entries may not yet have been set up
    if (index_ > _offsets.GetSize())
        return;

        // define offsets of the new entries
    _offsets.InsertAt(index_, off, count_);
    d4_assert(_offsets.GetSize() <= RowCount() + 1);

    for (int j = 1; j < count_; ++j)
        _offsets.SetAt(index_ + j, _offsets.GetAt(index_ + j - 1) + m + 1);
    
        // finally, adjust all following entries
    index_ += count_;
    while (index_ < _offsets.GetSize())
        _offsets.ElementAt(index_++) += n;

    d4_assert(index_ <= RowCount() + 1);
    d4_assert(_offsets.GetAt(index_ - 1) <= ColSize());
}

void c4_FormatS::Remove(int index_, int count_)
{
    d4_assertThis;

    t4_u32 off = Offset(index_);
    t4_u32 n = Offset(index_ + count_) - off;

    if (ColSize() <= off)
        return; // removing entries when none are present is trivial

    Realize(off + n);
    RemoveData(off, n);

        // some offset entries may not yet have been set up
    int k = _offsets.GetSize();
    if (index_ + count_ > k)
        count_ = k - index_;
    if (count_ > 0)
        _offsets.RemoveAt(index_, count_);

    d4_assert(_offsets.GetSize() <= RowCount() + 1);

        // adjust all following entries
    while (index_ < _offsets.GetSize())
        _offsets.ElementAt(index_++) -= n;

    d4_assert(index_ <= RowCount() + 1);
    d4_assert(index_ == 0 || _offsets.GetAt(index_ - 1) <= ColSize());
}

/////////////////////////////////////////////////////////////////////////////

class c4_FormatV : public c4_FormatHandler
{
public:
    c4_FormatV (const c4_Property& prop_, c4_HandlerSeq& seq_);
    virtual ~c4_FormatV ();

    virtual void Defined();

    virtual void FlipBytes();
	virtual bool Commit(c4_Streamer& ar_);
	virtual void Prepare(c4_Streamer& ar_);

    virtual const void* Clear(int& length_) const;

    virtual int ItemSize(int index_);
    virtual const void* Get(int index_, int& length_);
    virtual void Set(int index_, const c4_Bytes& buf_);

    static int DoCompare(const c4_Bytes& b1_, const c4_Bytes& b2_);
    virtual int Compare(int index_, const c4_Bytes& buf_);

    virtual void Insert(int index_, const c4_Bytes& buf_, int count_);
    virtual void Remove(int index_, int count_);

private:
	void FixOwnerRows();

    c4_HandlerSeq*& At(int index_);
	c4_HandlerSeq& AtSetup(int index_);
	void Replace(int index_, c4_HandlerSeq* seq_);

    c4_PtrArray _subSeqs;
};

/////////////////////////////////////////////////////////////////////////////

c4_FormatV::c4_FormatV (const c4_Property& prop_, c4_HandlerSeq& seq_)
    : c4_FormatHandler (prop_, seq_)
{
}

c4_FormatV::~c4_FormatV ()
{
    d4_assertThis;
	d4_assert(RowCount() == _subSeqs.GetSize());

	for (int i = 0; i < _subSeqs.GetSize(); ++i)
	{
		c4_HandlerSeq* p = At(i);
		if (p != 0)
			p->DecRef();
	}
}

c4_HandlerSeq*& c4_FormatV::At(int index_)
{
    d4_assertThis;

    return (c4_HandlerSeq*&) _subSeqs.ElementAt(index_);
}

c4_HandlerSeq& c4_FormatV::AtSetup(int index_)
{
    d4_assertThis;

    c4_HandlerSeq*& e = At(index_);
	if (e == 0)
	{
		c4_HandlerSeq& t = Sequence();
        e = d4_new c4_HandlerSeq (t.Persist());
		e->IncRef();

		int col = ColNum();
		e->DefineSub(col < t.NumFields() ? &t.Field(col) : 0, &t, index_);
	}

    return *e;
}

void c4_FormatV::Defined()
{
    d4_assertThis;
    
	int numRows = RowCount();
		// this can be called more than once, because Prepare calls it early
	if (numRows == _subSeqs.GetSize())
		return;

    d4_assert(_subSeqs.GetSize() == 0);
    _subSeqs.SetSize(0, 10 + numRows / 16);       // optimization

    if (numRows > 0)
		_subSeqs.InsertAt(0, 0, numRows);
}

void c4_FormatV::FlipBytes()
{
    d4_assertThis;
	d4_assert(RowCount() == _subSeqs.GetSize());
    
    for (int i = 0; i < _subSeqs.GetSize(); ++i)
    {
        c4_HandlerSeq* te = At(i);
        if (te != 0)
            te->FlipAllBytes();
    }
}

const void* c4_FormatV::Clear(int& length_) const
{
    d4_assertThis;
    
    static c4_HandlerSeq* zero = 0;

    length_ = sizeof zero;
    return &zero;
}

int c4_FormatV::ItemSize(int index_)
{
    d4_assertThis;
	d4_assert(At(index_) != 0);

    return At(index_)->Size();
}

const void* c4_FormatV::Get(int index_, int& length_)
{
    d4_assertThis;

    c4_HandlerSeq*& e = At(index_);
	if (e == 0)
	{
		AtSetup(index_);
		d4_assert(e != 0);
	}

    length_ = sizeof (c4_HandlerSeq**);
    return &e;
}

void c4_FormatV::Set(int index_, const c4_Bytes& buf_)
{
    d4_assertThis;
    d4_assert(buf_.Size() == sizeof (c4_Sequence*));

    c4_HandlerSeq* value = *(c4_HandlerSeq* const*) buf_.Contents();

    if (value != At(index_))
		Replace(index_, value);
}

void c4_FormatV::Replace(int index_, c4_HandlerSeq* seq_)
{
    d4_assertThis;

	c4_HandlerSeq*& curr = At(index_);

	if (curr != 0)
	{
		d4_assert(&curr->Owner() == &Sequence());
		curr->DetachFromParent();

		curr->DecRef();
		curr = 0;
	}

    if (seq_)
    {
        int n = seq_->Size();

        c4_HandlerSeq& t = AtSetup(index_);
        d4_assert(t.Size() == 0);

        t.Resize(n);

        c4_Bytes data;

			// this dest seq has only the persistent handlers
			// and maybe in a different order
			// create any others we need as temporary properties
        for (int i = 0; i < seq_->NumHandlers(); ++i)
        {
            c4_Handler& h1 = seq_->NthHandler(i);

			int j = t.PropIndex(h1.Property());
			d4_assert(j >= 0);

            c4_Handler& h2 = t.NthHandler(j);

            for (int k = 0; k < n; ++k)
                if (seq_->Get(k, h1.PropId(), data))
                    h2.Set(k, data);
        }
    }
}

int c4_FormatV::DoCompare(const c4_Bytes& b1_, const c4_Bytes& b2_)
{
    d4_assert(b1_.Size() == sizeof (c4_Sequence*));
    d4_assert(b2_.Size() == sizeof (c4_Sequence*));

    c4_View v1 = *(c4_Sequence* const*) b1_.Contents();
    c4_View v2 = *(c4_Sequence* const*) b2_.Contents();
    
    return v1.Compare(v2);
}

int c4_FormatV::Compare(int index_, const c4_Bytes& buf_)
{
    d4_assertThis;
    d4_assert(buf_.Size() == sizeof (c4_Sequence*));

    c4_Sequence* seq = At(index_);
    d4_assert(seq);

    c4_Bytes data (&seq, sizeof seq);

    return DoCompare(data, buf_);
}

void c4_FormatV::FixOwnerRows()
{
    d4_assertThis;
	d4_assert(RowCount() == _subSeqs.GetSize());

	for (int i = 0; i < _subSeqs.GetSize(); ++i)
	{
		c4_HandlerSeq* e = At(i);
		if (e != 0)
			e->SetOwnerRow(i);
	}
}

void c4_FormatV::Insert(int index_, const c4_Bytes& buf_, int count_)
{
    d4_assertThis;
    d4_assert(buf_.Size() == sizeof (c4_Sequence*));
    d4_assert(count_ > 0);

    c4_Sequence* value = *(c4_Sequence* const*) buf_.Contents();
    d4_assert(!value); // can only insert an empty entry!

	_subSeqs.InsertAt(index_, 0, count_);

	FixOwnerRows();
}

void c4_FormatV::Remove(int index_, int count_)
{
    d4_assertThis;
    d4_assert(count_ > 0);

    for (int i = 0; i < count_; ++i)
	{
		c4_HandlerSeq* e = At(index_ + i);
		if (e != 0)
		{
			d4_assert(&e->Owner() == &Sequence());
			e->DetachFromParent();
			e->DecRef();
		}
	}

	_subSeqs.RemoveAt(index_, count_);

	FixOwnerRows();
}

bool c4_FormatV::Commit(c4_Streamer& ar_)
{
    d4_assertThis;
	d4_assert(RowCount() == _subSeqs.GetSize());

    bool changed = false;

	for (int j = 0; j < _subSeqs.GetSize(); ++j)
    {
        c4_HandlerSeq* entry = At(j);

        int n = entry != 0 ? entry->Size() : 0;
        ar_.StoreValue((t4_u32) n);

        if (n > 0 && entry->Commit(ar_))
			changed = true;
    }

	return changed;
}

void c4_FormatV::Prepare(c4_Streamer& ar_)
{
    d4_assertThis;
	
	//d4_assert(RowCount() == _subSeqs.GetSize());
	Defined(); // force sizing now

    for (int i = 0; i < _subSeqs.GetSize(); ++i)
    {
        int rows = (int) ar_.LoadValue();
        if (rows)
        {
            c4_HandlerSeq& t = AtSetup(i); // also creates it
            t.SetSize(rows);
			t.Prepare(ar_);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////

c4_Handler* f4_CreateFormat(const c4_Property& prop_, c4_HandlerSeq& seq_)
{
    switch (prop_.Type())
    {
        case 'I':   return d4_new c4_FormatX (prop_, seq_);
#if !q4_TINY
        case 'F':   return d4_new c4_FormatF (prop_, seq_);
        case 'D':   return d4_new c4_FormatD (prop_, seq_);
        case 'M':   return d4_new c4_FormatM (prop_, seq_);
#endif
        case 'B':   return d4_new c4_FormatB (prop_, seq_);
        case 'S':   return d4_new c4_FormatS (prop_, seq_);
        case 'V':   return d4_new c4_FormatV (prop_, seq_);
    }

    d4_assert(0);
    return 0;
}

/////////////////////////////////////////////////////////////////////////////

int f4_ClearFormat(char type_)
{
    switch (type_)
    {
        case 'I':   return sizeof (t4_i32);
#if !q4_TINY
        case 'F':   return sizeof (float);
        case 'D':   return sizeof (double);
#endif
        case 'V':   return sizeof (c4_Sequence*);
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////

int f4_CompareFormat(char type_, const c4_Bytes& b1_, const c4_Bytes& b2_)
{
    switch (type_)
    {
        case 'I':   return c4_FormatX::DoCompare(b1_, b2_);
#if !q4_TINY
        case 'F':   return c4_FormatF::DoCompare(b1_, b2_);
        case 'D':   return c4_FormatD::DoCompare(b1_, b2_);
        case 'M':   return c4_FormatB::DoCompare(b1_, b2_); // B!
#endif
        case 'B':   return c4_FormatB::DoCompare(b1_, b2_);
        case 'S':   return c4_FormatS::DoCompare(b1_, b2_);
        case 'V':   return c4_FormatV::DoCompare(b1_, b2_);
    }

    d4_assert(0);
    return 0;
}

/////////////////////////////////////////////////////////////////////////////

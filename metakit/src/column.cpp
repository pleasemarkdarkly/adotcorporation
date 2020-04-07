//........................................................................................
//........................................................................................
//.. File Name: column.cpp
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
//  This file contains the implementation of the column class.

#include "header.h"
#include "column.h"

#if !q4_INLINE
static char _columnInl[] = "column.inl";
#undef THIS_FILE
#define THIS_FILE _columnInl
#include "column.inl"
#endif

#if q4_CHECK
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#if !HAVE_MEMMOVE && !HAVE_BCOPY
    // in case we have no library memmove, or one that can't handle overlap

    void f4_memmove (void* to_, const void* from_, int n_)
    {
        char* to = (char*) to_;
        const char* from = (const char*) from_;

        if (to + n_ <= from || from + n_ <= to)
            memcpy(to, from, n_);
        else if (to < from)
            while (--n_ >= 0)
                *to++ = *from++;
        else if (to > from)
            while (--n_ >= 0)
                to[n_] = from[n_];
    }
#endif

/////////////////////////////////////////////////////////////////////////////
// Implemented in this file

    class c4_Column;
    class c4_ColIter;
    class c4_ColCache;
    
/////////////////////////////////////////////////////////////////////////////
// c4_Column
//
//  As of version 1.7, the column class has been dramatically improved.
//  It is becoming a sort of central workhorse for the entire library.
//  This version handles most insertions and deletions quite efficiently,
//  especially if these are usually clustered close together.
//
//  Version 1.7 adds support for memory mapped files, and will be extremely
//  effective with it, as long as data is not modified.  Once modifications
//  are made, we still create more buffer copies than strictly necessary.
//
//  That's a feature - it means there is room for yet more improvement!

c4_Column::c4_Column (c4_Strategy* strategy_)
    : _position (0), _size (0), _strategy (strategy_), _gap (0),
      _slack (0), _dirty (false)
{
    d4_assertThis;
}

c4_Column::~c4_Column ()
{
    d4_assertThis;

    ReleaseAllSegments();

        // this is needed to remove this column from the cache
    d4_assert(_slack == 0);
    FinishSlack();
}

#if q4_CHECK

        // debugging version to verify that the internal data is consistent
    void c4_Column::Validate()
    {
        d4_assertThis;

        if (_segments.GetSize() == 0)
            return; // ok, not initialized

        d4_assert(_gap <= _size);
        d4_assert(0 <= _slack && _slack < kSegMax);

        int n = fSegIndex(_size + _slack);
        d4_assert(n == _segments.GetSize() - 1);

        t4_byte* p = (t4_byte*) _segments.GetAt(n);

        if (fSegRest(_size + _slack) == 0)
            d4_assert(p == 0);
        else
            d4_assert(p != 0);

        while (--n >= 0)
        {
            t4_byte* p = (t4_byte*) _segments.GetAt(n);
            d4_assert(p != 0);
        }
    }

#else

        // nothing, so inline this thing to avoid even the calling overhead
    d4_inline void c4_Column::Validate()
    {
    }

#endif

bool c4_Column::IsMapped() const
{
    d4_assertThis;

    return _position > 1 && _strategy && _strategy->_mapStart != 0;
}

const t4_byte* c4_Column::DataMap() const
{
    d4_assertThis;
    d4_assert(_position > 1);

    if (_strategy == 0 || _strategy->_mapStart == 0)
        return 0;

    const t4_byte* ptr = _strategy->_mapStart + _position;
    d4_assert(ptr + _size <= _strategy->_mapLimit);

    return ptr;
}

bool c4_Column::UsesMap(const t4_byte* ptr_) const
{
    d4_assertThis;

        // the most common falsifying case is checked first
    return _strategy != 0 &&
				ptr_ < _strategy->_mapLimit && ptr_ >= _strategy->_mapStart;
}

void c4_Column::ReleaseSegment(int index_)
{
    d4_assertThis;
    
    t4_byte* p = (t4_byte*) _segments.GetAt(index_);
    if (!UsesMap(p))
        delete [] p;
}

void c4_Column::ReleaseAllSegments()
{
    d4_assertThis;
    
    //better? for (int i = _segments.GetSize(); --i >= 0 ;)
    for (int i = 0; i < _segments.GetSize(); ++i)
        ReleaseSegment(i); // last one might be a null pointer

    _segments.SetSize(0);

    _gap = 0;
    _slack = 0;

    if (_size == 0)
    {
        _position = 0;
        _dirty = false;
    }
}

    //@func Define where data is on file, or setup buffers (opt cleared).
void c4_Column::SetLocation(t4_u32 pos_, t4_u32 size_)
{
    d4_assertThis;
    d4_assert(size_ > 0 || pos_ == 0);

    ReleaseAllSegments();

    _position = pos_;
    _size = size_;

        //  There are three different position settings:
        //
        //       0 = clear buffer, no file access
        //       1 = raw buffer, no file access
        //      >1 = file position from where data can be loaded on demand
        //
        //  None of this is performed here.  Buffers are set up only when
        //  LoadNow is called to get at the real data.

    _dirty = pos_ <= 1;
}

    //@func How many contiguous bytes are there at a specified position.
int c4_Column::AvailAt(t4_u32 offset_) const
{
    d4_assertThis;
    d4_assert(offset_ <= _size);
    d4_assert(_gap <= _size);

    t4_u32 limit = _gap;

    if (offset_ >= _gap)
    {
        offset_ += _slack;
        limit = _size + _slack;
    }

    int count = kSegMax - fSegRest(offset_);
    if (offset_ + count > limit)
        count = (int) (limit - offset_);

        // either some real data or it must be at the very end of all data
    d4_assert(0 < count && count <= kSegMax ||
                count == 0 && offset_ == _size + _slack);
    return count;
}

void c4_Column::SetupSegments()
{
    d4_assertThis;
    d4_assert(_segments.GetSize() == 0);
    d4_assert(_gap == 0);
    d4_assert(_slack == 0);

    //  The last entry in the _segments array is either a partial block
    //  or a null pointer, so calling "fSegIndex(_size)" is always allowed.

    int n = fSegIndex(_size) + 1;
    _segments.SetSize(n);

        // treat last block differently if it is a partial entry
    int last = n;
    if (fSegRest(_size))
        --last;     // this block is partial, size is 1 .. kSegMax-1
    else
        --n;        // the last block is left as a null pointer
    
    if (IsMapped())
    {
            // setup for mapped files is quick, just fill in the pointers
        const t4_byte* map = DataMap();
        d4_assert(map != 0);

        for (int i = 0; i < n; ++i)
        {
            _segments.SetAt(i, (t4_byte*) map); // loses const
            map += kSegMax;
        }
    }
    else
    {
        int chunk = kSegMax;

        if (_position > 1)
		{
			d4_assert(_strategy != 0);
            _strategy->DataSeek(_position);
		}

            // allocate buffers, clear or load them if necessary
        for (int i = 0; i < n; ++i)
        {
            if (i == last)
                chunk = fSegRest(_size);

            t4_byte* p = d4_new t4_byte [chunk];
            _segments.SetAt(i, p);

            if (_position == 0)
                memset(p, 0, chunk);
            else if (_position > 1)
			{
				d4_assert(_strategy != 0);
				bool f = _strategy->DataRead(p, chunk) == chunk;
				d4_assert(f);
			}
        }
    }

    Validate();
}

    //@func Makes sure the requested data is in a modifiable buffer.
t4_byte* c4_Column::CopyNow(t4_u32 offset_)
{
    d4_assertThis;
    d4_assert(offset_ <= _size);

    _dirty = true;

    const t4_byte* ptr = LoadNow(offset_);
    if (UsesMap(ptr))
    {
        if (offset_ >= _gap)
            offset_ += _slack;

            // this will only force creation of a buffer
        ptr = CopyData(offset_, offset_, 0);
        d4_assert(!UsesMap(ptr));
    }

    return (t4_byte*) ptr;
}

    //@func Copies data, creating a buffer if needed.  Must be in single segment.
t4_byte* c4_Column::CopyData(t4_u32 to_, t4_u32 from_, int count_)
{
    d4_assertThis;

    int i = fSegIndex(to_);
    t4_byte* p = (t4_byte*) _segments.GetAt(i);

    if (UsesMap(p))
    {
        int n = kSegMax;
        if (fSegOffset(i) + n > _size + _slack)
            n = (int) (_size + _slack - fSegOffset(i));

        d4_assert(n > 0);
        
        t4_byte* q = d4_new t4_byte [n];
        memcpy(q, p, n); // some copying can be avoided, overwritten below...
        _segments.SetAt(i, q);

        p = q;
    }

    p += fSegRest(to_);

    if (count_ > 0)
    {
        d4_assert(fSegIndex(to_ + count_ - 1) == i);

        const t4_byte* src = (const t4_byte*) _segments.GetAt(fSegIndex(from_));
        d4_memmove(p, src + fSegRest(from_), count_);
    }

    return p;
}

    /*
     *  Resizing a segmented vector can be a complicated operation.
     *  For now, simply making it work in all cases is the first priority.
     *
     *  A major simplification - and good performance improvement - is caused
     *  by the trick of maintaining a "gap" in the data, which can be "moved"
     *  around to allow fast insertion as well as simple (delayed) deletion.
     *  
     *  The only complexity comes from the fact that the gap must end up being 
     *  less than one full segment in size.  Therefore, insertion and removal
     *  across segment boundaries needs to handle a variety of situations.
     *
     *  Since complete segments can be inserted quickly, this approach avoids
     *  lots of copying when consecutive insertions/deletions are clustered.
     *  Even random changes move half as much (on average) as without a gap.
     *
     *  The price is the overhead of up to one segment of empty space, and the
     *  complexity of this code (all the magic is within this c4_Column class).
     */

void c4_Column::MoveGapUp(t4_u32 dest_)
{
    d4_assertThis;
    d4_assert(dest_ <= _size);
    d4_assert(_gap < dest_);
    d4_assert(_slack > 0);

        // forward loop to copy contents down, in little pieces if need be
    while (_gap < dest_)
    {
        int n = kSegMax - fSegRest(_gap);
        t4_u32 curr = _gap + n;
        if (curr > dest_)
            curr = dest_;

            // copy to [_gap..curr), which is inside one segment
        d4_assert(_gap < curr);
        d4_assert(fSegIndex(_gap) == fSegIndex(curr - 1));

            // copy from [_gap + _slack .. curr + _slack), of the same size
        t4_u32 fromBeg = _gap + _slack;
        t4_u32 fromEnd = curr + _slack;

        while (fromBeg < fromEnd)
        {
            int k = kSegMax - fSegRest(fromBeg);
            if (fromBeg + k > fromEnd)
                k = (int) (fromEnd - fromBeg);

            d4_assert(k > 0);

            CopyData(_gap, fromBeg, k);

            _gap += k;
            fromBeg += k;
        }

        _gap = curr;
    }

    d4_assert(_gap == dest_);
}

void c4_Column::MoveGapDown(t4_u32 dest_)
{
    d4_assertThis;
    d4_assert(dest_ <= _size);
    d4_assert(_gap > dest_);
    d4_assert(_slack > 0);

        // reverse loop to copy contents up, in little pieces if need be
    t4_u32 toEnd = _gap + _slack;
    t4_u32 toBeg = dest_ + _slack;

    while (toEnd > toBeg)
    {
        int n = fSegRest(toEnd);
        t4_u32 curr = toEnd - (n ? n : kSegMax);
        if (curr < toBeg)
            curr = toBeg;

            // copy to [curr..toEnd), which is inside one segment
        d4_assert(curr < toEnd);
        d4_assert(fSegIndex(curr) == fSegIndex(toEnd - 1));

            // copy from [fromBeg .. _gap), which has the same size
        t4_u32 fromBeg = _gap - (toEnd - curr);

        while (_gap > fromBeg)
        {
            int k = fSegRest(_gap);
            if (k == 0)
                k = kSegMax;
            if (_gap - k < fromBeg)
                k = (int) (_gap - fromBeg);

            d4_assert(k > 0);

            toEnd -= k;
            _gap -= k;
            
            CopyData(toEnd, _gap, k);
        }
    }

    d4_assert(_gap == dest_);
}

void c4_Column::MoveGapTo(t4_u32 pos_)
{
    d4_assertThis;
    d4_assert(pos_ <= _size);

    if (_slack == 0)            // if there is no real gap, then just move it
        _gap = pos_;
    else if (_gap < pos_)       // move the gap up, ie. some bytes down
        MoveGapUp(pos_);
    else if (_gap > pos_)       // move the gap down, ie. some bytes up
        if (_gap - pos_ > _size - _gap + fSegRest(pos_))
        {
            RemoveGap();        // it's faster to get rid of the gap instead
            _gap = pos_;
        }
        else                    // normal case, move some bytes up
            MoveGapDown(pos_);

    d4_assert(_gap == pos_);

    Validate();
}

void c4_Column::RemoveGap()
{
    d4_assertThis;

    if (_slack > 0)
    {
        if (_gap < _size)
            MoveGapUp(_size);

        d4_assert(_gap == _size); // the gap is now at the end
        d4_assert(_slack < kSegMax);

            //  Case 1: gap is at start of segment
            //  ==================================
            //
            //      G   G+S 
            //
            //      |    |
            //      :----+xx:
            //      |       |
            //
            //      i      i+1 (limit)
            //
            //  Case 2: gap is inside segment
            //  =============================
            //
            //         G G+S
            //
            //         |  |
            //      :--+--+x:
            //      |       |
            //
            //      i      i+1 (limit)
            //
            //  Case 3: gap runs to end of segment
            //  ==================================
            //
            //         G   G+S
            //
            //         |    |
            //      :--+----:0000000:
            //      |       |       |
            //
            //      i      i+1     i+2 (limit)
            //
            //  Case 4: gap is across two segments
            //  ==================================
            //
            //         G     G+S
            //
            //         |      |
            //      :--+----:-+xxxxx:
            //      |       |       |
            //
            //      i      i+1     i+2 (limit)

        int i = fSegIndex(_gap);
        int n = fSegRest(_gap);

        if (n == 0) // case 1
        {
            ReleaseSegment(i);
            _segments.SetAt(i, 0);
        }
        else
        {
            if (n + _slack > kSegMax) // case 4
                ReleaseSegment(i+1);

                // truncate rest of segment
            t4_byte* p = d4_new t4_byte [n];
            memcpy(p, _segments.GetAt(i), n);

            ReleaseSegment(i);
            _segments.SetAt(i, p);
            _segments.SetSize(i + 1);
        }

        _slack = 0;
    }

    Validate();
}

void c4_Column::Grow(t4_u32 off_, t4_u32 diff_)
{
    d4_assertThis;
    d4_assert(off_ <= _size);
    d4_assert(diff_ > 0);

    if (_segments.GetSize() == 0)
        SetupSegments();

    Validate();

    _dirty = true;

        // move the gap so it starts where we want to insert
    MoveGapTo(off_);

    t4_u32 bigSlack = (t4_u32) _slack;
    if (bigSlack < diff_)   // only do more if this isn't good enough
    {
            // number of segments to insert
        int n = fSegIndex(diff_ - _slack + kSegMax - 1);
        d4_assert(n > 0);

        int i1 = fSegIndex(_gap);
        int i2 = fSegIndex(_gap + _slack);

        bool moveBack = false;

        if (i2 > i1)                // cases 3 and 4
            ++i1;
        else if (fSegRest(_gap))    // case 2
            moveBack = true;

        _segments.InsertAt(i1, 0, n);
        for (int i = 0; i < n; ++i)
            _segments.SetAt(i1 + i, d4_new t4_byte [kSegMax]);

        bigSlack += fSegOffset(n);

        if (moveBack)
        {
            d4_assert(i1 == fSegIndex(_gap));

                //  we have inserted too low, move bytes in front of gap back
            CopyData(fSegOffset(i1), fSegOffset(i1 + n), fSegRest(_gap));
        }
    }

    d4_assert(diff_ <= bigSlack && bigSlack < diff_ + kSegMax);

    _gap += diff_;
    _slack = (int) (bigSlack - diff_);
    _size += diff_;

    FinishSlack();
}

void c4_Column::Shrink(t4_u32 off_, t4_u32 diff_)
{
    d4_assertThis;
    d4_assert(off_ <= _size);
    d4_assert(diff_ > 0);

    if (_segments.GetSize() == 0)
        SetupSegments();

    Validate();

    _dirty = true;

        // the simplification here is that we have in fact simply *two*
        // gaps and we must merge them together and end up with just one

    if (_slack > 0)
    {
        if (_gap < off_)                // if too low, move the gap up
            MoveGapTo(off_);
        else if (off_ + diff_ < _gap)   // if too high, move down to end
            MoveGapTo(off_ + diff_);

            // the gap is now inside, or adjacent to, the deleted area
        d4_assert(off_ <= _gap && _gap <= off_ + diff_);
    }

    _gap = off_;

        // check whether the merged gap would cross a segment boundary
    int i1 = fSegIndex(_gap);
    int i2 = fSegIndex(_gap + _slack + diff_);

        // drop complete segments, not a partially filled boundary
    if (fSegRest(_gap))
        ++i1;

        // moved up (was after the next if in the 1.7 May 28 build)
    _slack += diff_;
    _size -= diff_;

    int n = i2 - i1;
    if (n > 0)
    {
        for (int i = i1; i < i2; ++i)
            ReleaseSegment(i);

        _segments.RemoveAt(i1, n);

            // the logic in 1.7 of May 28 was warped (the assert "fix" was wrong)
        d4_assert((t4_u32) _slack >= fSegOffset(n));
        _slack -= fSegOffset(n);
    }

    d4_assert(0 <= _slack && _slack < 2 * kSegMax);

        // if the gap is at the end, get rid of a partial segment after it
    if (_gap == _size)
    {
        int i = fSegIndex(_size + _slack);
        if (i != fSegIndex(_gap))
        {
            d4_assert(i == fSegIndex(_gap) + 1);
            d4_assert(i == _segments.GetSize() - 1);

            ReleaseSegment(i);
            _segments.SetAt(i, 0);

            _slack -= fSegRest(_size + _slack);

            d4_assert(_slack < kSegMax);
            d4_assert(fSegRest(_gap + _slack) == 0);
        }
    }

        // the slack may still be too large to leave as is
    if (_slack >= kSegMax)
    {
            // move the bytes just after the end of the gap one segment down
        int x = fSegRest(_gap + _slack);
        int r = kSegMax - x;
        if (_gap + r > _size)
            r = (int) (_size - _gap);

        CopyData(_gap, _gap + _slack, r);

        int i = fSegIndex(_gap + kSegMax - 1);
        ReleaseSegment(i);

        if (r + x < kSegMax)
            _segments.SetAt(i, 0);
        else
            _segments.RemoveAt(i);

        _slack -= r + x;
        _gap += r;
    }

        // if we have no data anymore, make sure not to use the file map either
    if (_size == 0 && _slack > 0)
        CopyNow(0);

    FinishSlack();
}

void c4_Column::FinishSlack()
{
    Validate();

        // optimization: if partial end segment easily fits in slack, move it down
    t4_u32 gapEnd = _gap + _slack;
    if (!fSegRest(gapEnd) && gapEnd >= _size + 500)
    {
            // slack is at least 500 bytes more than the partial end segment
            // also, the gap must end exactly on a segment boundary
        int i = fSegIndex(gapEnd);
        d4_assert(i == _segments.GetSize() - 1);

        int n = _size - _gap;
        CopyData(gapEnd - n, gapEnd, n);

        ReleaseSegment(i);
        _segments.SetAt(i, 0);

        _slack -= n;
        d4_assert(_slack >= 500);

        Validate();
    }
}

bool c4_Column::SaveNow(t4_u32 pos_)
{
    d4_assertThis;
    d4_assert(pos_ > 0);
    d4_assert(IsDirty());
    d4_assert(_strategy != 0);
    
    d4_assert(_segments.GetSize() > 0);

    _position = pos_;

    _strategy->DataSeek(_position);

        // write all segments
    c4_ColIter iter (*this, 0, _size);
    while (iter.Next(kSegMax))
        if (!_strategy->DataWrite(iter.BufLoad(), iter.BufLen()))
            return false;

    _dirty = false;
    return true;
}

bool c4_Column::IsForeign() const
{
    d4_assertThis;
    
    return _strategy != 0 && _strategy->_bytesFlipped;
}

/////////////////////////////////////////////////////////////////////////////

bool c4_ColIter::Next()
{
    d4_assertThis;
    
    _pos += _len;

    _len = _column.AvailAt(_pos);
    _ptr = _column.LoadNow(_pos);

    if (!_ptr)
        _len = 0;
	else if (_pos + _len >= _limit)
		_len = _limit - _pos;
	else // 19990831 - optimization to avoid most copying
	{
			// while the end is adjacent to the next segment, extend it
		while (_ptr + _len == _column.LoadNow(_pos + _len))
		{
			int n = _column.AvailAt(_pos + _len);
			if (n == 0)
				break; // may be a short column (strings)

			_len += n;

			if (_pos + _len >= _limit)
			{
				_len = _limit - _pos;
				break;
			}
		}
	}

    return _len > 0;
}

bool c4_ColIter::Next(int max_)
{
    d4_assertThis;

    _pos += _len;

    _len = _column.AvailAt(_pos);
    _ptr = _column.LoadNow(_pos);

    if (!_ptr)
        _len = 0;
	else if (_pos + _len > _limit)
		_len = _limit - _pos;

	if (_len <= 0)
        return false;

    if (_len > max_)
        _len = max_;

    return true;
}

/////////////////////////////////////////////////////////////////////////////

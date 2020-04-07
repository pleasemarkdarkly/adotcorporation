//  Copyright (C) 1996-2000 Jean-Claude Wippler.  All rights reserved.
//
//  This file contains the implementation of the persistence classes.

#include "header.h"
#include "column.h"
#include "persist.h"
#include "handler.h"
#include "store.h"

#if q4_CHECK
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Implemented in this file

    class c4_FileHeader;
    class c4_Allocator;
    class c4_Streamer;
    class c4_Persist;
    
/////////////////////////////////////////////////////////////////////////////
// The file header stores a few basic facts in platform-independent format
    
    const short kStorageFormat = 0x4C4A; // b0 = 'J', b1 = <4C> (on Intel)
    const short kReverseFormat = 0x4A4C; // b0 = <4C>, b1 = 'J'

class c4_FileHeader
{    
public:
    c4_FileHeader (bool flip_ =false, t4_u32 pos_ =0);
        
    t4_u32 GetStart() const;
    
    short   _format;            // defines format and byte-order (4C4A/4A4C)
    t4_byte _spare;             // always 0x1A (CTRL-Z)
    t4_byte _flags;             // always 0x80 (allows future LoadValue)
    t4_byte _start [4];         // file offset, always low byte first
};

c4_FileHeader::c4_FileHeader (bool flip_, t4_u32 pos_)
    : _format (flip_ ? kReverseFormat : kStorageFormat), 
      _spare (0x1A), _flags (0x80)
{
    d4_assertThis;
        // a couple of (in)sanity checks...
    d4_assert(sizeof _start == sizeof (t4_u32));
    d4_assert(sizeof *this == 8);

    for (int i = 0; i < (int) sizeof _start; ++i)
    {
         _start[i] = (t4_byte) pos_;
         pos_ >>= 8;
    }
}
        
t4_u32 c4_FileHeader::GetStart() const
{                                    
    d4_assertThis;

    t4_u32 pos = 0;
    for (int i = sizeof _start; --i >= 0; )
        pos = (pos << 8) + _start[i];
    return pos;
}

/////////////////////////////////////////////////////////////////////////////
//@class:(IMPL) Support class tracks allocated storage in abstract container.

class c4_Allocator
{
    c4_DWordArray _walls;
    
public:
    c4_Allocator ();
    
    void Initialize(t4_u32 first_ =1);
    
    t4_u32 AllocationLimit() const; 

    t4_u32 Allocate(t4_u32 len_);
    void Occupy(t4_u32 pos_, t4_u32 len_);  
    void Release(t4_u32 pos_, t4_u32 len_);

private:
    int Locate(t4_u32 pos_) const;
    void InsertPair(int i_, t4_u32 from_, t4_u32 to_);
    t4_u32 ReduceFrags(int goal_, int sHi_, int sLo_);
};

/////////////////////////////////////////////////////////////////////////////
//
//  Allocation of blocks is maintained in a separate data structure.
//  There is no allocation overhead in the allocation arena itself.
//  
//  A single vector of "walls" is maintained, sorted by position:
//  
//      * Each transition between free and allocated is a single entry.
//        The number of entries is <num-free-ranges> + <num-used-ranges>.
//      * By definition, free areas start at the positions indicated
//        by the entries on even indices. Allocated ones use odd entries.
//      * There is an extra <0,0> free slot at the very beginning. This
//        simplifies boundary conditions at the start of the arena.
//      * Position zero cannot be allocated, first slot starts at 1.
//
//  Properties of this approach:
//
//      * No allocation overhead for adjacent allocated areas. On the
//        other hand, the allocator does not know the size of used slots.
//      * Alternate function allows marking a specific range as occupied.
//      * Allocator can be initialized as either all free or all in-use.
//      * Allocation info contains only integers, it could be stored.
//      * To extend allocated slots: "occupy" extra bytes at the end.
//      * Generic: can be used for memory, disk files, and array entries.

c4_Allocator::c4_Allocator ()
{
    Initialize();
}

void c4_Allocator::Initialize(t4_u32 first_)
{
    d4_assertThis;

    _walls.SetSize(0, 1000);        // empty, and growing in large chunks 
    _walls.Add(0);                  // fake block at start
    _walls.Add(0);                  // ... only used to avoid merging
    
        // if occupied, add a tiny free slot at the end, else add entire range
    const t4_u32 kLimit = (t4_u32) ~ 0L;
    if (first_ == 0)
        first_ = kLimit;
    
    _walls.Add(first_);             // start at a nicely aligned position
    _walls.Add(kLimit);             // ... there is no limit on file size
}

t4_u32 c4_Allocator::Allocate(t4_u32 len_)
{
    d4_assertThis;
        // zero arg is ok, it simply returns first allocatable position
    
    for (int i = 2; i < _walls.GetSize(); i += 2)
        if (_walls.GetAt(i+1) >= _walls.GetAt(i) + len_)
        {
            t4_u32 pos = _walls.GetAt(i);
            if (_walls.GetAt(i+1) > pos + len_)
                _walls.ElementAt(i) += len_;
            else
                _walls.RemoveAt(i, 2);
                
//          TRACE2("Allocated %4lu @ %06lu\n", len_, pos);
            return pos;
        }
    
//  TRACE1("Allocate %4lu failed\n", len_);
    d4_assert(0);
    return 0; // not reached
}

void c4_Allocator::Occupy(t4_u32 pos_, t4_u32 len_)
{
    d4_assertThis;
    d4_assert(pos_ > 0);
        // note that zero size simply checks if there is any space to extend
    
    int i = Locate(pos_);
    d4_assert(0 < i && i < _walls.GetSize());
    
    if (i % 2)                      // allocation is not at start of free block
    {
        d4_assert(_walls.GetAt(i-1) < pos_);
        
        if (_walls.GetAt(i) == pos_ + len_) // allocate from end of this free block
            _walls.SetAt(i, pos_);
        else                        // split free block in two
            InsertPair(i, pos_, pos_ + len_);
    }
    else if (_walls.GetAt(i) == pos_)
/*
    This side of the if used to be unconditional, but that was
    incorrect if ReduceFrags gets called (which only happens with
    severely fragmented files) - there are cases when allocation
    leads to an occupy request of which the free space list knows
    nothing about because it dropped small segments.  The solution
    is to silently "allow" such allocations - fixed 29-02-2000

    Thanks to Andrew Kuchling for his help in chasing this bug.
*/
    {       // else extend tail of allocated area
        if (_walls.GetAt(i+1) > pos_ + len_)
            _walls.ElementAt(i) += len_;        // move start of next free up
        else
            _walls.RemoveAt(i, 2);  // remove this slot
    }

//  TRACE2("Occupied %4lu @ %06lu\n", len_, pos_);
}   

void c4_Allocator::Release(t4_u32 pos, t4_u32 len)
{
    d4_assertThis;
//  TRACE2(" Released %4lu @ %06lu\n", len, pos);
    
    int i = Locate(pos + len);
    d4_assert(0 < i && i < _walls.GetSize());
    d4_assert(i % 2 == 0);          // don't release inside a free block
    
    if (_walls.GetAt(i) == pos)         // move start of next free down 
        _walls.ElementAt(i) -= len;
    else if (_walls.GetAt(i-1) == pos)  // move end of previous free up
        _walls.ElementAt(i-1) += len;
    else                            // insert a new entry
        InsertPair(i, pos, pos + len);

    if (_walls.GetAt(i-1) == _walls.GetAt(i))   // merge if adjacent free
        _walls.RemoveAt(i-1, 2);
}   

t4_u32 c4_Allocator::AllocationLimit() const
{
    d4_assertThis;
    d4_assert(_walls.GetSize() >= 2);

    return _walls.GetAt(_walls.GetSize() - 2);
}

int c4_Allocator::Locate(t4_u32 pos) const
{
    d4_assertThis;

    int lo = 0, hi = _walls.GetSize() - 1;
    
    while (lo < hi)
    {
        int i = (lo + hi) / 2;
        if (pos < _walls.GetAt(i))
            hi = i - 1;
        else if (pos > _walls.GetAt(i))
            lo = i + 1;
        else
            return i;
    }                
    
    return lo < _walls.GetSize() && pos > _walls.GetAt(lo) ? lo + 1 : lo; 
}

void c4_Allocator::InsertPair(int i_, t4_u32 from_, t4_u32 to_)
{
    d4_assertThis;
    d4_assert(0 < i_);     
    d4_assert(i_ < _walls.GetSize());
      
    d4_assert(from_ < to_);
    d4_assert(_walls.GetAt(i_-1) < from_);
    d4_assert(to_ < _walls.GetAt(i_));

    _walls.InsertAt(i_, from_, 2);
    _walls.SetAt(i_+1, to_);
    
        // it's ok to have arrays up to some 30000 bytes
    if (_walls.GetSize() > 7500)
        ReduceFrags(5000, 12, 6);
}

t4_u32 c4_Allocator::ReduceFrags(int goal_, int sHi_, int sLo_)
{       
    d4_assertThis;
    
        // drastic fail-safe measure: remove small gaps if vec gets too long
        // this will cause some lost free space but avoids array overflow
        // the lost space will most probably be re-used after the next commit
    
    int limit = _walls.GetSize() - 2;
    t4_u32 loss = 0;
    
        // go through all entries and remove gaps under the given threshold
    for (int shift = sHi_; shift >= sLo_; --shift)
    {
            // the threshold is a fraction of the current size of the arena
        t4_u32 threshold = AllocationLimit() >> shift;
        if (threshold == 0)
        {
//          TRACE2("c4_Allocator reduce skipped: shift %d, limit %d\n",
//                                                      shift, limit);
            continue;
        }
                                     
        int n = 2;
        for (int i = n; i < limit; i += 2)
            if (_walls.GetAt(i+1) - _walls.GetAt(i) > threshold)
            {
                _walls.SetAt(n++, _walls.GetAt(i));
                _walls.SetAt(n++, _walls.GetAt(i+1));
            }
            else
                loss += _walls.GetAt(i+1) - _walls.GetAt(i);
            
//      TRACE3("c4_Allocator threshold %ld: resized from %d to %d\n",
//                                              (t4_i32) threshold, limit, n);
        limit = n;
        
        // if (_walls.GetSize() < goal_) - suboptimal, fixed 29-02-2000
        if (limit < goal_)
            break;      // got rid of enough entries, that's enough
    }
    
    int n = _walls.GetSize() - 2;
//  TRACE2("c4_Allocator lost %d free block, total %ld bytes\n",
//                                      (n - limit) / 2, (t4_i32) loss);
    _walls.SetAt(limit++, _walls.GetAt(n++));
    _walls.SetAt(limit++, _walls.GetAt(n));
    _walls.SetSize(limit);
    
    return loss;
}

/////////////////////////////////////////////////////////////////////////////
// c4_Streamer

c4_Streamer::c4_Streamer (c4_Persist& persist_, c4_Stream* stream_,
							bool store_, bool trackSpace_)
    : _curr (_buffer), _limit (_buffer), _pos (0), _tally (-1), 
      _stream (stream_), _nextSpace (0), _persist (persist_), _start (0),
	  _oldStyle (false)
{
    d4_assertThis;
    d4_assert(!_persist._streamer);
    
    if (store_)
        _limit = _buffer + sizeof _buffer - 5; // slack for one StoreValue
	else
		SeekPos(0); // use mem-mapping for reads if possible

	if (trackSpace_) // added 19990905: omit space tracking on r/o files
	{
		_nextSpace = d4_new c4_Allocator;
		_nextSpace->Occupy(1, sizeof (c4_FileHeader) - 1);
	}
}

c4_Streamer::~c4_Streamer ()
{
    d4_assertThis;

    delete _persist._space;
		// if space was not tracked, just return as empty, but initialized
    _persist._space = _nextSpace ? _nextSpace : d4_new c4_Allocator;

    _persist._streamer = 0;
}

bool c4_Streamer::IsFlat() const
{
    d4_assertThis;

    return _persist._streamer != 0;
}

bool c4_Streamer::Preflighting() const
{
    d4_assertThis;
    d4_assert(!IsFlat());

    return _tally >= 0;
}

bool c4_Streamer::OldStyle() const
{
    d4_assertThis;

    return _oldStyle;
}

c4_Allocator* c4_Streamer::CurrSpace() const
{
    d4_assertThis;

    return _persist._space;
}

c4_Strategy& c4_Streamer::Strategy() const
{
    d4_assertThis;

    return _persist.Strategy();
}

void c4_Streamer::MarkAsOccupied()
{
    d4_assertThis;
    
    if (_start)
    {
        t4_u32 now = _pos - (_limit - _curr);
        d4_assert(now > _start);
        
        OccupyNext(_start, now - _start);

        _start = 0;
    }
}

int c4_Streamer::MyRead(void* buf_, int len_)
{
    d4_assertThis;
    
    int n;
	if (_stream != 0)
	{
		n = _stream->Read(buf_, len_);
	}
	else
	{
		c4_Strategy& strategy = Strategy();
		d4_assert(strategy._mapStart == 0); // will never be a mmap'ed file

		if (!IsFlat())
			strategy.DataSeek(_pos);
        n = strategy.DataRead(buf_, len_);
	}

    _pos += n;

    return n;
}

bool c4_Streamer::Read(void* buf_, int len_)
{
    d4_assertThis;
    d4_assert(len_ > 0);
    
    if (_curr < _limit)
    {
        int n = _limit - _curr;
        if (n > len_)
            n = len_;

        memcpy(buf_, _curr, n);

        _curr += n;
        buf_ = (t4_byte*) buf_ + n;
        len_ -= n; 
        
        if (len_ == 0)
            return true;
    }

    int r = MyRead(buf_, len_);
    d4_assert(r <= 0 || r == len_);

    return r == len_;
}

void c4_Streamer::ReadColumn(c4_Column& col_)
{
    c4_ColIter iter (col_, 0, col_.ColSize());
    while (iter.Next())
    {
        bool f = Read(iter.BufSave(), iter.BufLen());
        d4_assert(f);
    }
}

void c4_Streamer::MyWrite(const void* buf_, int len_)
{
    d4_assertThis;

    if (_tally >= 0)    // during preflight, we only count data written
    {
        _tally += len_;
        return;
    }

    if (_stream != 0)
	{
        _stream->Write(buf_, len_);
    }
	else
	{
		c4_Strategy& strategy = Strategy();
		if (!IsFlat())
			strategy.DataSeek(_pos);
        strategy.DataWrite(buf_, len_);
	}

    _pos += len_;
}

void c4_Streamer::Write(const void* buf_, int len_)
{
    d4_assertThis;

        // use buffering if possible
    if (_curr + len_ <= _limit)
    {
        memcpy(_curr, buf_, len_);
        _curr += len_;

        return;
    }

    int n = _curr - _buffer;

    MyWrite(_buffer, n);
    MyWrite(buf_, len_);

    _curr = _buffer;
}

void c4_Streamer::WriteColumn(c4_Column& col_)
{
    c4_ColIter iter (col_, 0, col_.ColSize());
    while (iter.Next())
        Write(iter.BufLoad(), iter.BufLen());
}

    /*
        StoreValue and LoadValue deal with variable-sized storage of
        one unsigned integer value of up to 32 bits. Depending on the
        magnitude of the integer, 1..5 bytes are used to represent it.
        Each byte holds 7 significant bits and one continuation bit.
        This saves storage, but it is also byte order independent.
        
        Nice property: StoreValue never starts by emitting a zero byte.
        This offers the chance to define a stream escape character:
        insert a zero byte before emitting a value to flag it in an
        unambiguous way.
    */
            
void c4_Streamer::StoreValue(t4_u32 v_)
{
    d4_assertThis;

    if (_curr >= _limit)
        FlushBuffer();

    int n = 0;

    do
        n += 7;
    while ((v_ >> n) && n < 32);

    while (n)
    {
        n -= 7;

        t4_byte b = (t4_byte) ((v_ >> n) & 0x7F);
        if (!n)
            b |= 0x80;      // set bit 7 on the last byte

        *_curr++ = b;
    }
}

void c4_Streamer::SeekPos(t4_u32 pos_)
{
	_pos = pos_;

		// try using memory mapped files first
	c4_Strategy& strategy = Strategy();
	if (!IsFlat() && strategy._mapStart != 0)
    {
        _curr = (t4_byte*) strategy._mapStart + _pos; // loses const
        _limit = (t4_byte*) strategy._mapLimit;
        _pos += _limit - _curr;
    }
	else
	{
		_curr = _limit; // force reading
	}
}

t4_byte c4_Streamer::NextByte()
{
    d4_assertThis;
	d4_assert (_curr >= _limit);

    _curr = _buffer;
    _limit = _curr + MyRead(_buffer, sizeof _buffer);
	d4_assert(_curr < _limit);

    return *_curr++;
}

t4_u32 c4_Streamer::LoadValue()
{
    d4_assertThis;

    t4_byte b = _curr < _limit ? *_curr++ : NextByte();

		// normal values can never start with a zero byte
	d4_assert(b != 0); // oops, this is a pre 1.5 file format!
    
    t4_u32 v = 0;
    for (;;)
    {
        v = (v << 7) + b;
        
        if (b & 0x80)
            break;        
            
        b = _curr < _limit ? *_curr++ : NextByte();
    }
    
    return v - 0x80;        // oops, last byte had bit 7 set
}

void c4_Streamer::FlushBuffer()
{
    d4_assertThis;

    int n = _curr - _buffer;
    if (n > 0)
    {
        MyWrite(_buffer, n);

        _curr = _buffer;
    }
}

t4_u32 c4_Streamer::AllocateCurr(t4_u32 len_)
{
    d4_assertThis;
    d4_assert(CurrSpace() != 0);

    t4_u32 pos = CurrSpace()->Allocate(len_);
    if (_nextSpace)
		_nextSpace->Occupy(pos, len_);

    return pos;
}

void c4_Streamer::OccupyNext(t4_u32 pos_, t4_u32 len_)
{
    d4_assertThis;

	if (_nextSpace)
		_nextSpace->Occupy(pos_, len_);
}

bool c4_Streamer::LoadDescription(t4_i32 pos_, c4_Bytes& desc_)
{
    d4_assertThis;

    if (pos_)
	{
        _start = pos_;
		SeekPos(pos_);
	}

	t4_byte b = _curr < _limit ? *_curr++ : NextByte();
	if ((b & 0x7F) != 0)
	{
		d4_assert(false); // oops, this is a pre 1.5 file format!
		return false;
	}

	if (b) // 0x80 marks a version <= 2.0 header
	{
		_oldStyle = true;

			// read length and description string
		int n = (int) LoadValue();
    
		char* desc = (char*) desc_.SetBuffer(n + 1);
		desc[n] = 0;

		if (n > 0)
		{
			bool f = Read(desc, n);
			d4_assert(f);
		}
	}
	else // 0x00 marks a new style header
	{
		//!!! work in progress
		desc_ = c4_Bytes ("#:I,_:H", 7); // version and heterogenous view
		d4_assert(_curr + 7 < _limit);
		_curr += 7;
	}

	return true;
}

t4_u32 c4_Streamer::SaveDescription(const c4_String& desc_, bool preflight_)
{
    d4_assertThis;
    d4_assert(!_start);

        // section code has been removed in 1.7, now uses two passes instead
    if (!preflight_ && !IsFlat())
    {
        d4_assert(_tally > 0);

            // now we know how much room this all takes, so allocate a slot
        _pos = _start = AllocateCurr(_tally);
        d4_assert(_start > 0);
    }

    _tally = preflight_ ? 0 : -1;

    StoreValue(0);              // always start with a zero marker

    int k = desc_.GetLength();
    StoreValue(k);              // emit the length of the description string

		// 16-7-1999: avoid write overruns in viewx.cpp, around line 818
	const char* p = desc_;
	while (k > 0) {
		int i = 4096;
		if (i > k)
			i = k;

		Write(p, i);

		p += i;
		k -= i;
	}

    return _start;
}

/////////////////////////////////////////////////////////////////////////////
// c4_Persist

c4_Persist::c4_Persist (c4_Strategy& strategy_, bool owned_,
							bool modifiable_, c4_HandlerSeq* root_)
    : _refCount (0), _strategy (strategy_), _space (0), _streamer (0),
	  _root (root_), _fCommit (0), _modifiable (modifiable_),
	  _owned (owned_), _forceCommit (false), _treewalk (0)
{
    d4_assertThis;

    Initialize();
}

c4_Persist::~c4_Persist ()
{
    d4_assertThis;

    if (_fCommit != 0)
        (this->*_fCommit)();

    Cleanup();

	if (_owned)
		delete &_strategy;
}

void c4_Persist::IncRef()
{
    d4_assertThis;

    ++_refCount;
    
    d4_assert(_refCount != 0);
}

void c4_Persist::DecRef()
{
    d4_assertThis;
    d4_assert(_refCount != 0);

    if (--_refCount == 0)
        delete this;
}

void c4_Persist::Initialize()
{
    d4_assertThis;
    d4_assert(!_space);

    if (!_root)
        _root = d4_new c4_HandlerSeq (this);
	_root->IncRef();

    _space = d4_new c4_Allocator;
    _space->Occupy(1, sizeof (c4_FileHeader) - 1);
}

void c4_Persist::Cleanup()
{
    d4_assertThis;

    if (_streamer)
    {
//      TRACE0("c4_Persist: cleaned up dangling archive\n");
        delete _streamer;
        _streamer = 0;
    }
    
    d4_assert(_space != 0);
    delete _space;
    _space = 0;

	_root->DecRef();
    _root = 0;
}

c4_HandlerSeq* c4_Persist::Root() const
{
    d4_assertThis;

    return _root;
}

c4_Strategy& c4_Persist::Strategy() const
{
    d4_assertThis;

    return _strategy;
}

void c4_Persist::AutoCommit()
{
    d4_assertThis;

    _fCommit = &c4_Persist::Commit;
}

bool c4_Persist::Commit()
{
    d4_assertThis;

		// 1-Mar-1999, new semantics! return success status of commits
    _strategy._failure = 0;
    
	if (!_modifiable) // added 14-2-2000, commit of r/o file now simply fails
	  	return false; // note that _strategy._failure is *zero* in this case

    SaveAll();

		// get rid of temp properties which still use the datafile
	_root->DetachFromStorage(true);

	_forceCommit = _strategy._failure != 0; // commit again iff it failed
    return _strategy._failure == 0;
}

bool c4_Persist::Rollback()
{
    d4_assertThis;

	_root->DetachFromParent();
    Cleanup();
    Initialize();

    return LoadAll(); //! is _forceCommit ok if this fails?
}

bool c4_Persist::LoadAll(c4_Stream* stream_)
{
    d4_assertThis;
    d4_assert(_root != 0);

    if (!stream_)
        _strategy.DataSeek(0);
    
	_forceCommit = false;

    c4_Streamer ar (*this, stream_, false, _modifiable && _strategy.IsValid());
    _streamer = &ar;
    
        // try to read the header, it may run into end of file 
    c4_FileHeader header;
    if (!ar.Read(&header, sizeof header))
    {
        _root->DefineRoot();
        return false;   // no data in file
    }

    if (header._format == kStorageFormat)
        _strategy._bytesFlipped = false;
    else if (header._format == kReverseFormat)
        _strategy._bytesFlipped = true; // not native, must flip bytes
    else
    {
        _root->DefineRoot();
        return false;   // unrecognized format
    }

    t4_i32 start = header.GetStart();
    if (start)
    {
        if (stream_)
            return false; // cannot load as stream, requires seeking

        _streamer = 0;
    }   
    
		// load the description string and deal with the new header format
	c4_Bytes desc;
	if (!ar.LoadDescription(start, desc))
		return false;	// failed to load for some reason

        // define and fill the root table 
    _root->DefineRoot((const char*) desc.Contents());
    _root->Prepare(ar);

        // mark the structural data as in use
    ar.MarkAsOccupied();

    return true;
}

void c4_Persist::SaveAll(c4_Stream* stream_)
{
    d4_assertThis;
    d4_assert(_root != 0);

    {       // see block end for an explanation of why we need a block here
        c4_Streamer ar (*this, stream_, true, _modifiable && _strategy.IsValid());
        
        c4_String desc = _root->Description();

        if (stream_)
        {
            _streamer = &ar;
            
            c4_FileHeader header (IsForeign());
            ar.Write(&header, sizeof header);

            ar.SaveDescription(desc);
        
            /* 1-Mar-1999, changed = */ _root->Commit(ar);

            ar.FlushBuffer();
            return;
        }
        
            // first pass allocates columns and measures structure size
        ar.SaveDescription(desc, true);

        bool changed = _root->Commit(ar);
        if (!changed && !_forceCommit) // avoid commit if possible, 14-2-2000
         	return;  
          
        ar.FlushBuffer();

            // second pass saves the columns and structure to disk
        t4_u32 start = ar.SaveDescription(desc, false);

        _root->Commit(ar); // writes changes and saves structure

        ar.FlushBuffer();

        c4_FileHeader head (IsForeign(), start);

        // 1-Mar-1999: don't proceed with commit if there was an I/O error
        if (_strategy._failure != 0)
            return;

        _strategy.DataCommit(0);

        _strategy.DataSeek(0);
        _strategy.DataWrite(&head, sizeof head);

        // nasty side effect of c4_Streamer is to alter _space on destruction
    }

        // commit and tell strategy object what the new file size is, this
        // may be smaller now, if old data at the end is no longer referenced
    _strategy.DataCommit(_space->AllocationLimit());
}

bool c4_Persist::Load(c4_Stream* stream_)
{
    d4_assertThis;
    d4_assert(stream_ != 0);

    c4_Persist tmpStorage (_strategy, false, false, _root);
    
    if (!tmpStorage.LoadAll(stream_))
        return false;
    
    if (IsForeign())
    {
        _root->FlipAllBytes();
        _strategy._bytesFlipped = false;
    }

    return true;
}

void c4_Persist::Save(c4_Stream* stream_)
{
    d4_assertThis;
    d4_assert(stream_ != 0);
    
    c4_Persist tmpStorage (_strategy, false, false, _root);
    
    tmpStorage.SaveAll(stream_);
}

bool c4_Persist::IsForeign() const
{
    d4_assertThis;
    
    return _strategy._bytesFlipped;
}

void c4_Persist::ForceCommit()
{
 	d4_assertThis;
 	
 	_forceCommit = true;
}

/////////////////////////////////////////////////////////////////////////////

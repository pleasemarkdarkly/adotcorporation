//  Copyright (C) 1996-2000 Jean-Claude Wippler.  All rights reserved.
//
//  This file contains the declaration of the column class.

#ifndef __COLUMN_H__
#define __COLUMN_H__

/////////////////////////////////////////////////////////////////////////////
// Declarations in this file

    class c4_Column;                    // a column in a table
    class c4_ColIter;                   // an iterator over column data
    class c4_ColCache;                  // manages a cache for columns

    class c4_Strategy;                  // not defined here

/////////////////////////////////////////////////////////////////////////////
//: Objects of this class define the column contents of tables.
//
//  This is an internal class.
//
//@xref <c c4_Table>

class c4_Column
{
    c4_PtrArray _segments;
    t4_u32 _position;
    t4_u32 _size;
    c4_Strategy* _strategy;
    t4_u32 _gap;
    int _slack;
    bool _dirty;
    
public:
    c4_Column (c4_Strategy* strategy_ =0);
        //: Constructs a column using the specified strategy.
    ~c4_Column ();

    void SetBuffer(t4_u32);
        //: Allocate a new buffer of the specified size.
    void SetBufferClear(t4_u32);
        //: Allocate and clear a new buffer of the specified size.

    c4_Strategy* Strategy() const;
        //: Returns the associated strategy pointer
    t4_u32 Position() const;
        //: Special access for the DUMP program.
    t4_u32 ColSize() const;
        //: Returns the number of bytes as stored on disk.
    bool IsDirty() const;
        //: Returns true if contents needs to be saved.

    void SetLocation(t4_u32, t4_u32);
        //: Sets the position and size of this column on file.

    int AvailAt(t4_u32 offset_) const;
        //: Returns number of bytes we can access at once.
    const t4_byte* LoadNow(t4_u32);
        //: Makes sure the data is loaded into memory.
    t4_byte* CopyNow(t4_u32);
        //: Makes sure a copy of the data is in memory.
    void Grow(t4_u32, t4_u32);
        //: Grows the buffer by inserting space.
    void Shrink(t4_u32, t4_u32);
        //: Shrinks the buffer by removing space.
    bool SaveNow(t4_u32 pos_);
        //: Makes sure the buffer is saved to file if dirty.

    bool IsForeign() const;
        //: Returns true if the file format is not native (bytes flipped).

    void RemoveGap();
    void ReleaseAllSegments();
    bool IsMapped() const; // 19990831, made public for ColIter::Next

private:
    enum { kSegBits = 12, kSegMax = 1 << kSegBits, kSegMask = kSegMax - 1 };

	static int fSegIndex(t4_u32 offset_);
	static t4_u32 fSegOffset(int index_);
	static int fSegRest(t4_u32 offset_);

    const t4_byte* DataMap() const;
    bool UsesMap(const t4_byte*) const;

    void ReleaseSegment(int);
    void SetupSegments();
    void Validate();
    void FinishSlack();

    void MoveGapUp(t4_u32 pos_);
    void MoveGapDown(t4_u32 pos_);
    void MoveGapTo(t4_u32 pos_);

    t4_byte* CopyData(t4_u32, t4_u32, int);
};

/////////////////////////////////////////////////////////////////////////////

class c4_ColIter
{
    c4_Column& _column;
    t4_u32 _limit;
    t4_u32 _pos;
    int _len;
    const t4_byte* _ptr;

public:
    c4_ColIter (c4_Column& col_, t4_u32 offset_, t4_u32 limit_);
//  ~c4_ColIter ();

    bool Next();
    bool Next(int max_);

    const t4_byte* BufLoad() const;
    t4_byte* BufSave();
    int BufLen() const;
};

/////////////////////////////////////////////////////////////////////////////

#if q4_INLINE
#include "column.inl"
#endif

/////////////////////////////////////////////////////////////////////////////

#endif

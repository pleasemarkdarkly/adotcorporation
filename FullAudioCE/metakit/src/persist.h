//  Copyright (C) 1996-2000 Jean-Claude Wippler.  All rights reserved.
//
//  This file contains the declaration of the persistence classes.

#ifndef __PERSIST_H__
#define __PERSIST_H__

/////////////////////////////////////////////////////////////////////////////
// Declarations in this file

    class c4_Streamer;                  // wraps file access
    class c4_Persist;                   // persistent table storage

    class c4_Allocator;                 // not defined here
    class c4_Column;                    // not defined here
    class c4_Strategy;                  // not defined here
    class c4_HandlerSeq;				// not defined here

/////////////////////////////////////////////////////////////////////////////
//:(IMPL) Manages loading/saving of structure (optionally also data).

class c4_Streamer
{ 
    enum { kBufMax = 1024 };
    t4_byte _buffer [kBufMax];

    t4_byte* _curr;
    t4_byte* _limit;
    t4_u32 _pos;
    t4_i32 _tally;

    c4_Stream* _stream;
    c4_Allocator* _nextSpace;
    c4_Persist& _persist;

    t4_u32 _start;
	c4_Column _treewalk;
	bool _oldStyle;

public:                   
    c4_Streamer (c4_Persist& persist_, c4_Stream* stream_,
					bool store_, bool trackSpace_);
    ~c4_Streamer ();
    
    bool IsFlat() const;
    bool Preflighting() const;
	bool OldStyle() const;

    c4_Allocator* CurrSpace() const;
    c4_Strategy& Strategy() const;
    
    void MarkAsOccupied();

    bool Read(void* buf_, int len_);
	void ReadColumn(c4_Column& col_);
    
    void Write(const void* buf_, int len_);
	void WriteColumn(c4_Column& col_);

    void StoreValue(t4_u32 v_);
    t4_u32 LoadValue();

    bool LoadDescription(t4_i32 pos_, c4_Bytes& desc_);
    t4_u32 SaveDescription(const c4_String&, bool =false);

    void FlushBuffer();

    t4_u32 AllocateCurr(t4_u32 len_);
    void OccupyNext(t4_u32 pos_, t4_u32 len_);

private:
	void SeekPos(t4_u32 pos_);
    t4_byte NextByte();
    int MyRead(void* buf_, int len_);
    void MyWrite(const void* buf_, int len_);
};

/////////////////////////////////////////////////////////////////////////////
//: These objects manage persistent storage of tables on file.
//
//  This is an internal class.

class c4_Persist
{
    int _refCount;
    c4_Strategy& _strategy;
    c4_Allocator* _space;
    c4_Streamer* _streamer;
    c4_HandlerSeq* _root;
    bool (c4_Persist::*_fCommit) ();
	bool _modifiable;
    bool _owned;
	bool _forceCommit; // tracked to help determine if commit is needed
	c4_Column* _treewalk;
  
public:
    c4_Persist (c4_Strategy&, bool owned_, bool modifiable_, c4_HandlerSeq* =0);
        //: Constructs a new object.
    
    void IncRef();
    void DecRef();

    c4_HandlerSeq* Root() const;
        //: Returns the root table of this object.
    c4_Strategy& Strategy() const;
        //: Returns the file associated with this object.

	void AutoCommit();

    bool Commit();
        //: Flushes pending changes to file right now.
    bool Rollback();
        //: (Re)initializes for on-demand loading.
        // This will cancel any uncommitted changes.
    
    bool Load(c4_Stream*);
        //: Loads contents from the specified input stream.
    void Save(c4_Stream*);
        //: Saves contents to the specified output stream.

    bool LoadAll(c4_Stream* =0);
    void SaveAll(c4_Stream* =0);

	void ForceCommit();

private:
    ~c4_Persist ();
    c4_Persist (const c4_Persist&);     // not implemented
    void operator= (const c4_Persist&); // not implemented

    void Initialize();
    void Cleanup();
    bool IsForeign() const;

    friend class c4_Streamer;
};

/////////////////////////////////////////////////////////////////////////////

#endif

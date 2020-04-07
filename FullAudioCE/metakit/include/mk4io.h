//  Copyright (C) 1996-2000 Jean-Claude Wippler.  All rights reserved.
//
//  This file contains the declaration of the file strategy class.

#ifndef __MK4IO_H__
#define __MK4IO_H__

#include <stdio.h>

/////////////////////////////////////////////////////////////////////////////
//: A file stream can be used to serialize using the stdio library.

class c4_FileStream : public c4_Stream
{
public:
	c4_FileStream (FILE* stream_, bool owned_ =false);
	virtual ~c4_FileStream ();

    virtual int Read(void* buffer_, int length_);
    virtual void Write(const void* buffer_, int length_);

	FILE* _stream;
    bool _owned;
};

/////////////////////////////////////////////////////////////////////////////
//: A file strategy encapsulates code dealing with all file I/O.

class c4_FileStrategy : public c4_Strategy
{     
public:
    c4_FileStrategy (FILE* file_ =0);
        //: Construct a new strategy object
    virtual ~c4_FileStrategy ();

	virtual bool IsValid() const;
        //: True if we can do I/O with this object
	virtual bool DataOpen(const char* fileName_, bool allowWriting_);
        //: Open a data file by name
    virtual void DataSeek(t4_i32 position_);
        //: Set file position
    virtual int  DataRead(void* buffer_, int length_);
        //: Read a number of bytes
    virtual bool DataWrite(const void* buffer_, int length_);
        //: Write a number of bytes, return true if successful
    virtual void DataCommit(t4_i32 newSize_);
        //: Flush and truncate file
    virtual void ResetFileMapping();
		//: Support for memory-mapped files

protected:

    FILE* _file;
        //: Pointer to file object
    FILE* _cleanup;
        //: Pointer to same file object, if it must be deleted at end
};

/////////////////////////////////////////////////////////////////////////////

#endif // __MK4IO_H__

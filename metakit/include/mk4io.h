//........................................................................................
//........................................................................................
//.. File Name: mk4io.h
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
//  This file contains the declaration of the file strategy class.

#ifndef __MK4IO_H__
#define __MK4IO_H__

#include <stdio.h>
#include "sdapi.h"
/////////////////////////////////////////////////////////////////////////////
//: A file stream can be used to serialize using the stdio library.

class c4_FileStream : public c4_Stream
{
public:
	c4_FileStream (PCFD stream_, bool owned_ =false);
	virtual ~c4_FileStream ();

    virtual int Read(void* buffer_, int length_);
    virtual void Write(const void* buffer_, int length_);

//	FILE* _stream;
    PCFD _stream;
    bool _owned;
};

/////////////////////////////////////////////////////////////////////////////
//: A file strategy encapsulates code dealing with all file I/O.

class c4_FileStrategy : public c4_Strategy
{     
public:
    c4_FileStrategy (PCFD file_ =0);
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

    PCFD _file; //dr was FILE*
        //: Pointer to file object
    PCFD _cleanup; //dr was FILE*
        //: Pointer to same file object, if it must be deleted at end
};

/////////////////////////////////////////////////////////////////////////////

#endif // __MK4IO_H__

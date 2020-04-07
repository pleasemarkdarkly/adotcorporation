//........................................................................................
//........................................................................................
//.. File Name: fileio.cpp
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
//  This file contains the implementation of the file io strategy class.

#include "header.h"
#include "mk4io.h"
#include "sdapi.h"

#if q4_WIN32
#if q4_MSVC && !q4_STRICT
#pragma warning(disable: 4201) // nonstandard extension used : ...
#endif
#include <windows.h>
#include <io.h>
#endif

#if q4_UNIX && HAVE_MMAP
#include <sys/mman.h>
#endif

#if q4_CHECK
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Implemented in this file

//	class c4_Stream;
		class c4_FileStream;
//  class c4_Strategy;
		class c4_FileStrategy;

/////////////////////////////////////////////////////////////////////////////
// c4_FileStream

c4_FileStream::c4_FileStream (PCFD stream_, bool owned_)
	: _stream (stream_), _owned (owned_)
{
}

c4_FileStream::~c4_FileStream ()
{
    if (_owned)
        po_close(_stream); //dr
}

int c4_FileStream::Read(void* buffer_, int length_)
{
    d4_assertThis;
    d4_assert(_stream != 0);

//    return fread(buffer_, 1, length_, _stream); dr 
      return po_read(_stream, (UCHAR*)buffer_, length_);

}

void c4_FileStream::Write(const void* buffer_, int length_)
{
    d4_assertThis;
    d4_assert(_stream != 0);

//    fwrite(buffer_, 1, length_, _stream);
    po_write(_stream, (UCHAR*)buffer_, length_);
}

/////////////////////////////////////////////////////////////////////////////
// c4_FileStrategy

c4_FileStrategy::c4_FileStrategy (PCFD file_)
    : _file (file_), _cleanup (0)
{
    ResetFileMapping();
}

c4_FileStrategy::~c4_FileStrategy ()
{
    d4_assertThis;

    _file = 0;
    ResetFileMapping();

    if (_cleanup)
//        fclose(_cleanup);
      po_close(_cleanup);
	d4_assert(_mapStart == 0 && _mapLimit == 0);
}

d4_inline bool c4_FileStrategy::IsValid() const
{ 
    d4_assertThis;

	return _file != 0; 
}

void c4_FileStrategy::ResetFileMapping()
{
   #if q4_WIN32
    if (_mapStart != 0)
    {
        BOOL g = ::UnmapViewOfFile(_mapStart);
        d4_assert(g);

        _mapStart = _mapLimit = 0;
    }

    if (_file)
    {
        po_lseek(_file, 0, 2); //dr 
        t4_i32 len = ftell(_file);

        if (len > 0)
        {
            HANDLE h = ::CreateFileMapping((HANDLE) _get_osfhandle(_fileno(_file)),
                                                0, PAGE_READONLY, 0, len, 0);
            d4_assert(h);   // check for errors, but can continue without mapping

            if (h)
            {
                _mapStart = (t4_byte*) ::MapViewOfFile(h, FILE_MAP_READ, 0, 0, len);
                d4_assert(_mapStart != 0);

                if (_mapStart != 0)
                    _mapLimit = _mapStart + len;

                BOOL f = ::CloseHandle(h);
                d4_assert(f);
            }
        }
    }
  #elif q4_UNIX && HAVE_MMAP
    if (_mapStart != 0)
    {
        munmap((char*) _mapStart, _mapLimit - _mapStart); // also loses const
        _mapStart = _mapLimit = 0;
    }

    if (_file)
    {
        po_lseek(_file, 0, 2); //dr 
        t4_i32 len = ftell(_file);

        if (len > 0)
        {
            _mapStart = (const t4_byte*) mmap(0, len, PROT_READ, MAP_PRIVATE,
                                                        fileno(_file), 0);
            if (_mapStart != (void*) -1L)
                _mapLimit = _mapStart + len;
        }
    }
  #endif
}

bool c4_FileStrategy::DataOpen(const char* fname_, bool rw_)
{
    d4_assertThis;
    d4_assert(!_file);

//    _cleanup = _file = fopen(fname_, rw_ ? "r+b" : "rb");
    _cleanup = _file = po_open((TEXT*)fname_, rw_ ? PO_RDWR : PO_RDONLY, rw_ ? PS_IWRITE : PS_IREAD);
    if (_file)
    {
        ResetFileMapping();
        return true;
    }

    if (rw_)
//        _cleanup = _file = fopen(fname_, "w+b");
      _cleanup = _file = po_open((TEXT*)fname_, PO_WRONLY, PS_IWRITE);
    d4_assert(_file != 0);
    return false;
}

void c4_FileStrategy::DataSeek(t4_i32 pos_)
{
    INT16 *err = 0; //dr
    d4_assertThis;
    d4_assert(_file != 0);

    po_lseek(_file, pos_, 0, err); //dr

}

int c4_FileStrategy::DataRead(void* buf_, int len_)
{
    d4_assertThis;
    d4_assert(_file != 0);
	d4_assert(_mapStart == 0); // 2000-03-16: never read if file is mmap'ed

//    return fread(buf_, 1, len_, _file);
    return po_read(_file, (UCHAR*)buf_, len_);
}

bool c4_FileStrategy::DataWrite(const void* buf_, int len_)
{
    d4_assertThis;
    d4_assert(_file != 0);

#if q4_WIN32
    if (buf_ >= _mapStart && buf_ <= _mapLimit - len_)
    {
        // a horrendous hack to allow file mapping for Win95 on network drive
        // must use a temp buf to avoid write from mapped file to same file
        // 
        //  6-Feb-1999  --  this workaround is not thread safe

        static char tempBuf [4096]; // 4096 is (1 << kSegBits), see column.cpp
		d4_assert(len_ <= sizeof tempBuf);
        buf_ = memcpy(tempBuf, buf_, len_);
    }
#endif

//    int n = (int) fwrite(buf_, 1, len_, _file);
    int n = (int) po_write(_file, (UCHAR*)buf_, len_);
    if (n != len_)
    {
      //*******************************************************
      //*        _failure = ferror(_file);                    *
      //*******************************************************
        d4_assert(_failure != 0);
        d4_assert(true); // always force an assertion failure in debug mode
    }

    return _failure == 0; // 1-Mar-1999: new success return value
}

void c4_FileStrategy::DataCommit(t4_i32 limit_)
{
    d4_assertThis;
    d4_assert(_file != 0);

    //fflush(_file);
    po_flush(_file);

    if (limit_ > 0)
    {
#if 0 // can't truncate file in a portable way!
            // unmap the file first, WinNT is more picky about this than Win95
        FILE* save = _file;

        _file = 0;
        ResetFileMapping();
        _file = save;

        _file->SetLength(limit_); // now we can resize the file
#endif
        ResetFileMapping(); // remap, since file length may have changed
    }
}

/////////////////////////////////////////////////////////////////////////////

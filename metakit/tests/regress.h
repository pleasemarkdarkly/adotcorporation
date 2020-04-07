//  Copyright (C) 1996-2000 Jean-Claude Wippler.  All rights reserved.
//
//  Regression test program, header file

/////////////////////////////////////////////////////////////////////////////

#include "mk4.h"
#include "mk4io.h"
#include "mk4str.h"
#include <cyg/infra/diag.h>

#define TraceAll    false

    // default for dos and unix is to assume they don't support exceptions
#if defined (_DOS) || defined (unix) || defined (__unix__) || defined (__GNUC__)
#if !defined (q4_NOTHROW)
#define q4_NOTHROW 1
#endif
#endif		      

#if _MSC_VER == 800
#pragma warning (disable: 4703) // function too large for global optimizations

// also no exceptions in MSVC 1.52 when used with a QuickWin target
#if defined (_QWINVER) && !defined (q4_NOTHROW)
#define q4_NOTHROW 1	  
#endif
#endif		      

#if q4_NOTHROW
#define try
#define catch(x)	if (0)

extern const char* msg;
#endif	   

#if defined (macintosh)  
#define TESTDIR	":tests:"
#define TEXTOUT	"wt"
#define LINESEP	"\r"
#elif defined (__VMS)
#define TESTDIR	"[.tests]"
#define TEXTOUT	"w"
#define LINESEP	"\r\n" // is this correct?
#elif defined (unix) || defined (__unix__)
#define TESTDIR	"tests/"
#define TEXTOUT	"w"
#define LINESEP	"\n"
#else
#define TESTDIR	"tests\\"
#define TEXTOUT	"wt"
#define LINESEP	"\r\n"
#endif

#include <stdio.h>

#if q4_MFC && defined(_DEBUG)
#define B(n_,d_,c_) \
    { \
	TRACE(#n_ " - " #d_ "\n"); \
	fputs(#n_ " - " #d_ "\n", stderr); \
	freopen(TESTDIR #n_ ".txt", TEXTOUT, stdout); \
	CMemoryState oldState, newState, diffState; \
	oldState.Checkpoint(); \
	afxTraceEnabled = TraceAll; \
	try \
	{ \
	    puts(">>> " #d_); \
	    {
#define E \
	    } \
	    puts("<<< done."); \
	} \
	catch (const char* msg) \
	{ \
	    fprintf(stderr, "\t%s\n", msg); \
	    printf("*** %s ***\n", msg); \
	} \
	catch (...) \
	{ \
	    fputs("\tException!\n", stderr); \
	    puts("*** Exception ***"); \
	} \
	afxTraceEnabled = true; \
	fflush(stdout); \
	newState.Checkpoint(); \
	if (diffState.Difference(oldState, newState)) \
	{ \
	    fputs("\tMemory leaked!\n", stderr); \
	    puts("*** Memory leaked ***"); \
	    TRACE("   *** Memory leaked, "); \
	    diffState.DumpAllObjectsSince(); \
	} \
	fflush(stdout); \
    }
#else
#define B(n_,d_,c_) \
    { \
	fputs(#n_ " - " #d_ "\n", stderr); \
	freopen(TESTDIR #n_ ".txt", TEXTOUT, stdout); \
	try \
	{ \
	    puts(">>> " #d_); \
	    {
#define E \
	    } \
	    puts("<<< done."); \
	} \
	catch (const char* msg) \
	{ \
	    fprintf(stderr, "\t%s\n", msg); \
	    diag_printf("*** %s ***\n", msg); \
	} \
	catch (...) \
	{ \
	    fputs("\tException!\n", stderr); \
	    puts("*** Exception ***"); \
	} \
	fflush(stdout); \
    }
#endif

//Fail("Failed: A(#e_)"); 

#define W(f_)	remove(#f_)
#define R(f_)	A(remove(#f_) == 0)
#define D(f_)	DumpFile(#f_, TESTDIR #f_ ".txt")

/////////////////////////////////////////////////////////////////////////////

extern void DumpFile(const char* in_, const char* out_);
//extern void Fail(const char* msg);

extern void TestBasics();
extern void TestCustom();
extern void TestCustom2();
extern void TestFormat();
extern void TestLimits();
extern void TestNotify();
extern void TestResize();
extern void TestStores();
extern void TestStores2();
extern void TestStores3();
extern void TestStores4();

/////////////////////////////////////////////////////////////////////////////

    //  The Borland C++ RTL does not want file handle objects to cross
    //  DLL boundaries, so we use special fopen/fclose hooks in the DLL.

#if defined (__BORLANDC__) // this assumes MetaKit is in a DLL!
extern FILE* f4_FileOpenInDLL(const char*, const char*);
extern int f4_FileCloseInDLL(FILE*);

#define fopen f4_FileOpenInDLL
#define fclose f4_FileCloseInDLL
#endif

/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_cache.h>
#include <cyg/kernel/kapi.h>
#include <cyg/io/io.h>
#include <cyg/infra/diag.h>
#include "mk4.h"
#include "regress.h"

#define STACK_SIZE (32 * 4096)
static cyg_handle_t thread;
static cyg_thread thread_obj;
static char stack[STACK_SIZE];

static void test_entry(cyg_addrword_t data); /* main entry point */

extern "C" {
void
cyg_user_start(void)
{
    cyg_thread_create(10, test_entry, (cyg_addrword_t)0, "test",
		      (void *)stack, STACK_SIZE, &thread, &thread_obj);
    cyg_thread_resume(thread);
}
}



//  Copyright (C) 1996-2000 Jean-Claude Wippler.  All rights reserved.
//
//  Regression test program, main code



#if q4_NOTHROW
    const char* msg;
#endif

/////////////////////////////////////////////////////////////////////////////

static void
test_entry(cyg_addrword_t data)
{
 //  afxMemDF |= allocMemDF | checkAlwaysMemDF;

        // The M4STRING package sometimes keeps a spare empty string around.
        // By allocating it here, we avoid a few bogus memory leak reports.
    c4_String empty;

    TestBasics();
    TestNotify();
    TestCustom();
    TestCustom2();
    TestResize();
    TestStores();
    TestStores2();
    TestStores3();
    TestStores4();
    TestFormat();
    TestLimits();

    diag_printf("regress done\n");
#if defined (_DEBUG)
    fputs("\nPress return... ", stderr);
    getchar();
#endif

    fputs("Done.\n", stderr);

   return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Recursively display the entire view contents. The results shown do not
// depend on file layout (free space, file positions, flat vs. on-demand).

static void ViewDisplay(const c4_View& v_, FILE* fp, int l_ =0)
{
    c4_String types;
    bool hasData = false, hasSubs = false;

        // display header info and collect all data types
    fprintf(fp, "%*s VIEW %5d rows =", l_, "", v_.GetSize());
    for (int n = 0; n < v_.NumProperties(); ++n)
    {
        c4_Property prop = v_.NthProperty(n);
        char t = prop.Type();

        fprintf(fp, " %s:%c", (const char*) prop.Name(), t);
        
        types += t;
    
        if (t == 'V')
            hasSubs = true;
        else
            hasData = true;
    }
    fprintf(fp, "\n");

    for (int j = 0; j < v_.GetSize(); ++j)
    {
        if (hasData)    // data properties are all shown on the same line
        {
            fprintf(fp, "%*s %4d:", l_, "", j);
            c4_RowRef r = v_[j];
            c4_Bytes data;

            for (int k = 0; k < types.GetLength(); ++k)
            {
                c4_Property p = v_.NthProperty(k);

                switch (types[k])
                {
                case 'I':
                    fprintf(fp, " %ld", (long) ((c4_IntProp&) p) (r));
                    break;

                case 'F':
                    fprintf(fp, " %g", (double) ((c4_FloatProp&) p) (r));
                    break;

                case 'D':
                    fprintf(fp, " %.12g", (double) ((c4_DoubleProp&) p) (r));
                    break;

                case 'S':
                    fprintf(fp, " '%s'", (const char*) ((c4_StringProp&) p) (r));
                    break;

                case 'B':
                    (p (r)).GetData(data);
                    fprintf(fp, " (%db)", data.Size());
                    break;

                case 'M':
                    (p (r)).GetData(data);
                    fprintf(fp, " (%db)", data.Size());
                    break;

                default:
                    if (types[k] != 'V')
                        fprintf(fp, " (%c?)", types[k]);
                }
            }

            fprintf(fp, "\n");
        }

        if (hasSubs)    // subviews are then shown, each as a separate block
        {
            for (int k = 0; k < types.GetLength(); ++k)
            {
                if (types[k] == 'V')
                {
                    c4_Property prop = v_.NthProperty(k);

                    fprintf(fp, "%*s %4d: subview '%s'\n", l_, "", j,
                            (const char*) prop.Name());

                    c4_ViewProp& vp = (c4_ViewProp&) prop;

                    ViewDisplay(vp (v_[j]), fp, l_ + 2);
                }
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////

void DumpFile(const char* in_, const char* out_)
{
    FILE* fp = fopen(out_, TEXTOUT);
    A(fp);

    c4_Storage store (in_, false);

    ViewDisplay(store.Contents().Container(), fp);

    fclose(fp);
}

void Fail(const char* msg)
{   
#if 0  
    #if q4_NOTHROW
        fprintf(stderr, "\t%s\n", msg);
        printf("*** %s ***\n", msg);
    #else
        throw msg;
    #endif
#endif
  diag_printf("\t%s\n", msg);
}

/////////////////////////////////////////////////////////////////////////////

/*=============================================================================
//
//      hal_diag.c
//
//      HAL diagnostic output code
//
//=============================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   nickg, gthomas
// Contributors:nickg, gthomas
// Date:        1998-03-02
// Purpose:     HAL diagnostic output
// Description: Implementations of HAL diagnostic output support.
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <pkgconf/hal.h>
#include <pkgconf/system.h>
#include CYGBLD_HAL_PLATFORM_H

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>           // basic machine info
#include <cyg/hal/hal_intr.h>           // interrupt macros
#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_diag.h>

#if defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT) \
    || defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT)
#include <cyg/hal/hal_stub.h>           // target_register_t
#include <cyg/hal/hal_intr.h>           // HAL_INTERRUPT_UNMASK(...)
#include <cyg/hal/hal_if.h>             // Calling interface definitions
#include <cyg/hal/hal_misc.h>           // Helper functions
#include <cyg/hal/drv_api.h>            // CYG_ISR_HANDLED
#endif

#include <cyg/hal/v850_common.h>        // hardware registers, etc.

// Assumption: all diagnostic output must be GDB packetized unless this is a ROM (i.e.
// totally stand-alone) system.

#if defined(CYG_HAL_STARTUP_ROM) && !defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)
#define HAL_DIAG_USES_HARDWARE
#else
#if defined(CYGDBG_HAL_DIAG_DISABLE_GDB_PROTOCOL)
#define HAL_DIAG_USES_HARDWARE
#endif
#endif

#if (CYGHWR_HAL_V85X_V850_CEB_DIAG_BAUD == 9600)
#define BAUD_COUNT    0xDD
#define BAUD_DIVISOR  3
#endif
#if (CYGHWR_HAL_V85X_V850_CEB_DIAG_BAUD == 19200)
#define BAUD_COUNT    0xDD
#define BAUD_DIVISOR  2
#endif
#if (CYGHWR_HAL_V85X_V850_CEB_DIAG_BAUD == 38400)
#define BAUD_COUNT    0xDD
#define BAUD_DIVISOR  1
#endif
#ifndef BAUD_COUNT
#error Unsupported BAUD rate
#endif

/*---------------------------------------------------------------------------*/
// CEB-v850

// Actually send character down the wire
void
hal_diag_write_char_serial(char c)
{
    volatile unsigned char *TxDATA = (volatile unsigned char *)V850_REG_TXS0;
    volatile unsigned char *TxSTAT = (volatile unsigned char *)V850_REG_STIC0;
    // Send character
    *TxDATA = (unsigned char)c;
    // Wait for Tx not busy
    while ((*TxSTAT & 0x80) == 0x00) ;
    *TxSTAT &= ~0x80;
}

bool
hal_diag_read_serial(char *c)
{
    volatile unsigned char *RxDATA = (volatile unsigned char *)V850_REG_RXS0;
    volatile unsigned char *RxSTAT = (volatile unsigned char *)V850_REG_SRIC0;
    volatile unsigned char *RxERR = (volatile unsigned char *)V850_REG_SERIC0;
    int timeout = 0;
    while ((*RxSTAT & 0x80) == 0x00) {
        if (++timeout == 50000) return false;
    }
    *c = (char)*RxDATA;
    *RxSTAT &= ~0x80;
    return true;
}

void hal_diag_init(void)
{
    static int init = 0;
    volatile unsigned char *mode = (volatile unsigned char *)V850_REG_ASIM0;
    volatile unsigned char *brgc = (volatile unsigned char *)V850_REG_BRGC0;
    volatile unsigned char *brgm = (volatile unsigned char *)V850_REG_BRGM0;
    volatile unsigned char *rxstat = (volatile unsigned char *)V850_REG_SRIC0;
    volatile unsigned char *rxerr = (volatile unsigned char *)V850_REG_SERIC0;
    volatile unsigned char *txstat = (volatile unsigned char *)V850_REG_STIC0;
    if (init) return;
    init++;
    *mode = 0xC8;
    *brgc = BAUD_COUNT;
    *brgm = BAUD_DIVISOR;
    *rxstat = 0x47;
    *rxerr = 0;
    *txstat = 0x47;
}

#ifdef HAL_DIAG_USES_HARDWARE

void hal_diag_write_char(char c)
{
    CYG_INTERRUPT_STATE old;
    HAL_DISABLE_INTERRUPTS(old);
    hal_diag_init();
    hal_diag_write_char_serial(c);
    HAL_RESTORE_INTERRUPTS(old);
}

void hal_diag_read_char(char *c)
{
    while (!hal_diag_read_serial(c)) ;
}

#else // HAL_DIAG relies on GDB

void 
hal_diag_read_char(char *c)
{
    while (!hal_diag_read_serial(c)) ;
}

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
static void hal_diag_write_char_break(void)
{
}
#endif

void 
hal_diag_write_char(char c)
{
    static char line[100];
    static int pos = 0;

    // No need to send CRs
    if( c == '\r' ) return;

    line[pos++] = c;

    if( c == '\n' || pos == sizeof(line) )
    {
        CYG_INTERRUPT_STATE old;

        // Disable interrupts. This prevents GDB trying to interrupt us
        // while we are in the middle of sending a packet. The serial
        // receive interrupt will be seen when we re-enable interrupts
        // later.
        
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
        CYG_HAL_GDB_ENTER_CRITICAL_IO_REGION(old);
#else
        HAL_DISABLE_INTERRUPTS(old);
#endif

        while(1)
        {
            static char hex[] = "0123456789ABCDEF";
            cyg_uint8 csum = 0;
            int i;
            char c1;
        
            hal_diag_write_char_serial('$');
            hal_diag_write_char_serial('O');
            csum += 'O';
            for( i = 0; i < pos; i++ )
            {
                char ch = line[i];
                char h = hex[(ch>>4)&0xF];
                char l = hex[ch&0xF];
                hal_diag_write_char_serial(h);
                hal_diag_write_char_serial(l);
                csum += h;
                csum += l;
            }
            hal_diag_write_char_serial('#');
            hal_diag_write_char_serial(hex[(csum>>4)&0xF]);
            hal_diag_write_char_serial(hex[csum&0xF]);

            // Wait for the ACK character '+' from GDB here and handle
            // receiving a ^C instead.  This is the reason for this clause
            // being a loop.
            if (!hal_diag_read_serial(&c1))
                continue;   // No response - try sending packet again

            if( c1 == '+' )
                break;              // a good acknowledge

#if defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS) && \
    defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT)
            cyg_drv_interrupt_acknowledge(CYGHWR_HAL_GDB_PORT_VECTOR);
            if( c1 == 3 ) {
                // Ctrl-C: breakpoint.
#if 0  // FIXME - __builtin_return_address() doesn't work
                cyg_hal_gdb_interrupt (__builtin_return_address(0));
#else
                cyg_hal_gdb_interrupt (hal_diag_write_char_break);
#endif
                break;
            }
#endif
            // otherwise, loop round again
        }
        
        pos = 0;

        // And re-enable interrupts
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
        CYG_HAL_GDB_LEAVE_CRITICAL_IO_REGION(old);
#if 1  // FIXME - __builtin_return_address() doesn't work
        hal_diag_write_char_break();
#endif
#else
        HAL_RESTORE_INTERRUPTS(old);
#endif
    }
}
#endif  // USE HARDWARE

#if defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT) \
    || defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT)

struct Hal_SavedRegisters *hal_saved_interrupt_state;

void
hal_ctrlc_isr_init(void)
{
    HAL_INTERRUPT_UNMASK(CYGHWR_HAL_GDB_PORT_VECTOR);
}

cyg_uint32
hal_ctrlc_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    volatile unsigned char *RxDATA = (volatile unsigned char *)V850_REG_RXS0;
    unsigned char ch;
    ch = (char)*RxDATA;
    if( cyg_hal_is_break( &ch , 1 ) )
        cyg_hal_user_break( (CYG_ADDRWORD *)hal_saved_interrupt_state );
    return CYG_ISR_HANDLED;
}
#endif

/*---------------------------------------------------------------------------*/
/* End of hal_diag.c */

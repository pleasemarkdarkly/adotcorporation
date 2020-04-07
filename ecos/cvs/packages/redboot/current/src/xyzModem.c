//==========================================================================
//
//      xyzModem.c
//
//      RedBoot stream handler for xyzModem protocol
//
//==========================================================================
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-07-14
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <redboot.h>
#include <xyzModem.h>

// Assumption - run xyzModem protocol over the console port

// Values magic to the protocol
#define SOH 0x01
#define STX 0x02
#define EOT 0x04
#define ACK 0x06
#define NAK 0x15
#define CAN 0x18

// Data & state local to the protocol
static struct {
    hal_virtual_comm_table_t* __chan;
    unsigned char pkt[1024], *bufp;
    unsigned char blk,cblk,crc1,crc2;
    unsigned char next_blk;  // Expected block
    int len, mode, total_retries;
    int total_SOH, total_STX, total_CAN;
    bool crc_mode, at_eof;
} xyz;

#define xyzModem_CHAR_TIMEOUT            2000  // 2 seconds
#define xyzModem_MAX_RETRIES             20
#define xyzModem_MAX_RETRIES_WITH_CRC    10

// with only one serial port, as noted below, this debugging model doesn't work.
#ifdef CYGPKG_REDBOOT_DEMOJUMPER
#undef DEBUG
#endif

#ifdef DEBUG
#ifndef USE_SPRINTF
//
// Note: this debug setup only works if the target platform has two serial ports
// available so that the other one (currently only port 1) can be used for debug
// messages.
//
static int
zm_dprintf(char *fmt, ...)
{
    int cur_console;
    va_list args;

    va_start(args, fmt);
    cur_console = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);
    CYGACC_CALL_IF_SET_CONSOLE_COMM(1);
    vprintf(fmt, args);
    CYGACC_CALL_IF_SET_CONSOLE_COMM(cur_console);
}

static void
zm_flush(void)
{
}

#else
//
// Note: this debug setup works by storing the strings in a fixed buffer
//
static char *zm_out = (char *)0x00200000;
static char *zm_out_start = (char *)0x00200000;

static int
zm_dprintf(char *fmt, ...)
{
    int len;
    va_list args;

    va_start(args, fmt);
    len = vsprintf(zm_out, fmt, args);
    zm_out += len;
}

static void
zm_flush(void)
{
    char *p = zm_out_start;
    while (*p) mon_write_char(*p++);
    zm_out = zm_out_start;
}
#endif

static void
zm_dump_buf(void *buf, int len)
{
    vdump_buf_with_offset(zm_dprintf, buf, len, 0);
}

static unsigned char zm_buf[2048];
static unsigned char *zm_bp;

static void
zm_new(void)
{
    zm_bp = zm_buf;
}

static void
zm_save(unsigned char c)
{
    *zm_bp++ = c;
}

static void
zm_dump(int line)
{
    zm_dprintf("Packet at line: %d\n", line);
    zm_dump_buf(zm_buf, zm_bp-zm_buf);
}

#define ZM_DEBUG(x) x
#else
#define ZM_DEBUG(x)
#endif

// Wait for the line to go idle
static void
xyzModem_flush(void)
{
    int res;
    char c;
    while (true) {
        res = CYGACC_COMM_IF_GETC_TIMEOUT(*xyz.__chan, &c);
        if (!res) return;
    }
}

static int
xyzModem_get_hdr(void)
{
    char c;
    int res;
    bool hdr_found = false;
    int i;
    unsigned short cksum;

    ZM_DEBUG(zm_new());
    // Find the start of a header
    while (!hdr_found) {
        res = CYGACC_COMM_IF_GETC_TIMEOUT(*xyz.__chan, &c);
        ZM_DEBUG(zm_save(c));
        if (res) {
            switch (c) {
            case SOH:
                xyz.total_SOH++;
            case STX:
                if (c == STX) xyz.total_STX++;
                hdr_found = true;
                break;
            case CAN:
                xyz.total_CAN++;
                ZM_DEBUG(zm_dump(__LINE__));
                return xyzModem_cancel;
            case EOT:
                ZM_DEBUG(zm_dump(__LINE__));
                return xyzModem_eof;
            default:
                // Ignore, waiting for start of header
            }
        } else {
            // Data stream timed out
            ZM_DEBUG(zm_dump(__LINE__));
            return xyzModem_timeout;
        }
    }

    // Header found, now read the data
    res = CYGACC_COMM_IF_GETC_TIMEOUT(*xyz.__chan, &xyz.blk);
    ZM_DEBUG(zm_save(xyz.blk));
    if (!res) {
        ZM_DEBUG(zm_dump(__LINE__));
        return xyzModem_timeout;
    }
    res = CYGACC_COMM_IF_GETC_TIMEOUT(*xyz.__chan, &xyz.cblk);
    ZM_DEBUG(zm_save(xyz.cblk));
    if (!res) {
        ZM_DEBUG(zm_dump(__LINE__));
        return xyzModem_timeout;
    }
    xyz.len = (c == SOH) ? 128 : 1024;
    xyz.bufp = xyz.pkt;
    for (i = 0;  i < xyz.len;  i++) {
        res = CYGACC_COMM_IF_GETC_TIMEOUT(*xyz.__chan, &c);
        ZM_DEBUG(zm_save(c));
        if (res) {
            xyz.pkt[i] = c;
        } else {
            ZM_DEBUG(zm_dump(__LINE__));
            return xyzModem_timeout;
        }
    }
    res = CYGACC_COMM_IF_GETC_TIMEOUT(*xyz.__chan, &xyz.crc1);
    ZM_DEBUG(zm_save(xyz.crc1));
    if (!res) {
        ZM_DEBUG(zm_dump(__LINE__));
        return xyzModem_timeout;
    }
    if (xyz.crc_mode) {
        res = CYGACC_COMM_IF_GETC_TIMEOUT(*xyz.__chan, &xyz.crc2);
        ZM_DEBUG(zm_save(xyz.crc2));
        if (!res) {
            ZM_DEBUG(zm_dump(__LINE__));
            return xyzModem_timeout;
        }
    }
    ZM_DEBUG(zm_dump(__LINE__));
    // Validate the message
    if ((xyz.blk ^ xyz.cblk) != (unsigned char)0xFF) {
        ZM_DEBUG(zm_dprintf("Framing error - blk: %x/%x/%x\n", xyz.blk, xyz.cblk, (xyz.blk ^ xyz.cblk)));
        ZM_DEBUG(zm_dump_buf(xyz.pkt, xyz.len));
        xyzModem_flush();
        return xyzModem_frame;
    }
    // Verify checksum/CRC
    if (xyz.crc_mode) {
        cksum = crc16(xyz.pkt, xyz.len);
        if (cksum != ((xyz.crc1 << 8) | xyz.crc2)) {
            ZM_DEBUG(zm_dprintf("CRC error - recvd: %02x%02x, computed: %x\n", 
                                xyz.crc1, xyz.crc2, cksum & 0xFFFF));
            return xyzModem_cksum;
        }
    } else {
        cksum = 0;
        for (i = 0;  i < xyz.len;  i++) {
            cksum += xyz.pkt[i];
        }
        if (xyz.crc1 != (cksum & 0xFF)) {
            ZM_DEBUG(zm_dprintf("Checksum error - recvd: %x, computed: %x\n", xyz.crc1, cksum & 0xFF));
            return xyzModem_cksum;
        }
    }
    // If we get here, the message passes [structural] muster
    return 0;
}

int 
xyzModem_stream_open(char *filename, int mode, int *err)
{
    int console_chan, stat;
    int retries = xyzModem_MAX_RETRIES;
    int crc_retries = xyzModem_MAX_RETRIES_WITH_CRC;

    if (mode == xyzModem_zmodem) {
        *err = xyzModem_noZmodem;
        return -1;
    }

    // Set up the I/O channel.  Note: this allows for using a different port in the future
    console_chan = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);
    CYGACC_CALL_IF_SET_CONSOLE_COMM(console_chan);
    xyz.__chan = CYGACC_CALL_IF_CONSOLE_PROCS();
    CYGACC_CALL_IF_SET_CONSOLE_COMM(console_chan);
    CYGACC_COMM_IF_CONTROL(*xyz.__chan, __COMMCTL_SET_TIMEOUT, xyzModem_CHAR_TIMEOUT);
    xyz.len = 0;
    xyz.crc_mode = true;
    xyz.at_eof = false;
    xyz.mode = mode;
    xyz.total_retries = 0;
    xyz.total_SOH = 0;
    xyz.total_STX = 0;
    xyz.total_CAN = 0;

    while (retries-- > 0) {
        stat = xyzModem_get_hdr();
        if (stat == 0) {
            if (xyz.blk == 0) {
                // Note: yModem file name data blocks quietly discarded
                CYGACC_COMM_IF_PUTC(*xyz.__chan, ACK);
            }
            xyz.next_blk = 1;
            xyz.len = 0;
            return 0;
        } else 
        if (stat == xyzModem_timeout) {
            if (--crc_retries <= 0) xyz.crc_mode = false;
            CYGACC_CALL_IF_DELAY_US(5*100000);   // Extra delay for startup
            CYGACC_COMM_IF_PUTC(*xyz.__chan, (xyz.crc_mode ? 'C' : NAK));
            xyz.total_retries++;
            ZM_DEBUG(zm_dprintf("NAK (%d)\n", __LINE__));
        }
        if (stat == xyzModem_cancel) {
            break;
        }
    }
    *err = stat;
    ZM_DEBUG(zm_flush());
    return -1;
}

int 
xyzModem_stream_read(char *buf, int size, int *err)
{
    int stat, total, len;
    int retries;

    total = 0;
    // Try and get 'size' bytes into the buffer
    while (!xyz.at_eof && (size > 0)) {
        if (xyz.len == 0) {
            retries = xyzModem_MAX_RETRIES;
            while (retries-- > 0) {
                stat = xyzModem_get_hdr();
                if (stat == 0) {
                    if (xyz.blk == xyz.next_blk) {
                        CYGACC_COMM_IF_PUTC(*xyz.__chan, ACK);
                        ZM_DEBUG(zm_dprintf("ACK (%d)\n", __LINE__));
                        xyz.next_blk = (xyz.next_blk + 1) & 0xFF;
                        break;
                    } else if (xyz.blk == ((xyz.next_blk - 1) & 0xFF)) {
                        // Just re-ACK this so sender will get on with it
                        CYGACC_COMM_IF_PUTC(*xyz.__chan, ACK);
                        continue;  // Need new header
                    } else {
                        stat = xyzModem_sequence;
                    }
                }
                if (stat == xyzModem_cancel) {
                    break;
                }
                if (stat == xyzModem_eof) {
                    CYGACC_COMM_IF_PUTC(*xyz.__chan, ACK);
                    ZM_DEBUG(zm_dprintf("ACK (%d)\n", __LINE__));
                    if (xyz.mode == xyzModem_ymodem) {
                        CYGACC_COMM_IF_PUTC(*xyz.__chan, (xyz.crc_mode ? 'C' : NAK));
                        xyz.total_retries++;
                        stat = xyzModem_get_hdr();                        
                        CYGACC_COMM_IF_PUTC(*xyz.__chan, ACK);
                        ZM_DEBUG(zm_dprintf("ACK (%d)\n", __LINE__));
                    }
                    xyz.at_eof = true;
                    break;
                }
                CYGACC_COMM_IF_PUTC(*xyz.__chan, (xyz.crc_mode ? 'C' : NAK));
                xyz.total_retries++;
                ZM_DEBUG(zm_dprintf("NAK (%d)\n", __LINE__));
            }
            if (stat < 0) {
                *err = stat;
                xyz.len = -1;
                return total;
            }            
        }
        len = xyz.len;
        if (size < len) len = size;
        memcpy(buf, xyz.bufp, len);
        size -= len;
        buf += len;
        total += len;
        xyz.len -= len;
        xyz.bufp += len;
    }
    return total;
}

void
xyzModem_stream_close(int *err)
{
    SHUNT_PRINT("xyzModem - %s mode, %d(SOH)/%d(STX)/%d(CAN) packets, %d retries\n", xyz.crc_mode ? "CRC" : "Cksum", xyz.total_SOH, xyz.total_STX, xyz.total_CAN, xyz.total_retries);
    ZM_DEBUG(zm_flush());
}

char *
xyzModem_error(int err)
{
    switch (err) {
    case xyzModem_access:
        return "Can't access file";
        break;
    case xyzModem_noZmodem:
        return "Sorry, zModem not available yet";
        break;
    case xyzModem_timeout:
        return "Timed out";
        break;
    case xyzModem_eof:
        return "End of file";
        break;
    case xyzModem_cancel:
        return "Cancelled";
        break;
    case xyzModem_frame:
        return "Invalid framing";
        break;
    case xyzModem_cksum:
        return "CRC/checksum error";
        break;
    case xyzModem_sequence:
        return "Block sequence error";
        break;
    default:
        return "Unknown error";
        break;
    }
}
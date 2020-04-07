//==========================================================================
//
//      include/netinet6_ipv6.h
//
//      
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
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//####BSDCOPYRIGHTBEGIN####
//
// -------------------------------------------
//
// Portions of this software may have been derived from OpenBSD or other sources,
// and are covered by the appropriate copyright disclaimers included herein.
//
// -------------------------------------------
//
//####BSDCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-01-10
// Purpose:      
// Description:  
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================


/*	$OpenBSD: ip6.h,v 1.1 1999/12/08 06:50:21 itojun Exp $	*/

/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Copyright (c) 1982, 1986, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)ip.h	8.1 (Berkeley) 6/10/93
 */

#ifndef _NETINET6_IPV6_H_
#define _NETINET6_IPV6_H_

/*
 * Definition for internet protocol version 6.
 * RFC 2460
 */

struct ip6_hdr {
	union {
		struct ip6_hdrctl {
			u_int32_t ip6_un1_flow;	/* 20 bits of flow-ID */
			u_int16_t ip6_un1_plen;	/* payload length */
			u_int8_t  ip6_un1_nxt;	/* next header */
			u_int8_t  ip6_un1_hlim;	/* hop limit */
		} ip6_un1;
		u_int8_t ip6_un2_vfc;	/* 4 bits version, 4 bits class */
	} ip6_ctlun;
	struct in6_addr ip6_src;	/* source address */
	struct in6_addr ip6_dst;	/* destination address */
};

#define ip6_vfc		ip6_ctlun.ip6_un2_vfc
#define ip6_flow	ip6_ctlun.ip6_un1.ip6_un1_flow
#define ip6_plen	ip6_ctlun.ip6_un1.ip6_un1_plen
#define ip6_nxt		ip6_ctlun.ip6_un1.ip6_un1_nxt
#define ip6_hlim	ip6_ctlun.ip6_un1.ip6_un1_hlim
#define ip6_hops	ip6_ctlun.ip6_un1.ip6_un1_hlim

#define IPV6_VERSION		0x60
#define IPV6_VERSION_MASK	0xf0

#if BYTE_ORDER == BIG_ENDIAN
#define IPV6_FLOWINFO_MASK	0x0fffffff	/* flow info (28 bits) */
#define IPV6_FLOWLABEL_MASK	0x000fffff	/* flow label (20 bits) */
#else
#if BYTE_ORDER == LITTLE_ENDIAN
#define IPV6_FLOWINFO_MASK	0xffffff0f	/* flow info (28 bits) */
#define IPV6_FLOWLABEL_MASK	0xffff0f00	/* flow label (20 bits) */
#endif /* LITTLE_ENDIAN */
#endif
#if 1
/* ECN bits proposed by Sally Floyd */
#define IP6TOS_CE		0x01	/* congestion experienced */
#define IP6TOS_ECT		0x02	/* ECN-capable transport */
#endif

/*
 * Extension Headers
 */

struct	ip6_ext {
	u_char	ip6e_nxt;
	u_char	ip6e_len;
};

/* Hop-by-Hop options header */
/* XXX should we pad it to force alignment on an 8-byte boundary? */
struct ip6_hbh {
	u_int8_t ip6h_nxt;	/* next header */
	u_int8_t ip6h_len;	/* length in units of 8 octets */
	/* followed by options */
};

/* Destination options header */
/* XXX should we pad it to force alignment on an 8-byte boundary? */
struct ip6_dest {
	u_int8_t ip6d_nxt;	/* next header */
	u_int8_t ip6d_len;	/* length in units of 8 octets */
	/* followed by options */
};

/* Option types and related macros */
#define IP6OPT_PAD1		0x00	/* 00 0 00000 */
#define IP6OPT_PADN		0x01	/* 00 0 00001 */
#define IP6OPT_JUMBO		0xC2	/* 11 0 00010 = 194 */
#define IP6OPT_JUMBO_LEN	6
#define IP6OPT_RTALERT		0x05	/* 00 0 00101 */
#define IP6OPT_RTALERT_LEN	4
#define IP6OPT_RTALERT_MLD	0	/* Datagram contains an MLD message */
#define IP6OPT_RTALERT_RSVP	1	/* Datagram contains an RSVP message */
#define IP6OPT_RTALERT_ACTNET	2 	/* contains an Active Networks msg */
#define IP6OPT_MINLEN		2

#define IP6OPT_TYPE(o)		((o) & 0xC0)
#define IP6OPT_TYPE_SKIP	0x00
#define IP6OPT_TYPE_DISCARD	0x40
#define IP6OPT_TYPE_FORCEICMP	0x80
#define IP6OPT_TYPE_ICMP	0xC0

#define IP6OPT_MUTABLE		0x20

/* Routing header */
struct ip6_rthdr {
	u_int8_t  ip6r_nxt;	/* next header */
	u_int8_t  ip6r_len;	/* length in units of 8 octets */
	u_int8_t  ip6r_type;	/* routing type */
	u_int8_t  ip6r_segleft;	/* segments left */
	/* followed by routing type specific data */
};

/* Type 0 Routing header */
struct ip6_rthdr0 {
	u_int8_t  ip6r0_nxt;		/* next header */
	u_int8_t  ip6r0_len;		/* length in units of 8 octets */
	u_int8_t  ip6r0_type;		/* always zero */
	u_int8_t  ip6r0_segleft;	/* segments left */
	u_int8_t  ip6r0_reserved;	/* reserved field */
	u_int8_t  ip6r0_slmap[3];	/* strict/loose bit map */
	struct in6_addr  ip6r0_addr[1];	/* up to 23 addresses */
};

/* Fragment header */
struct ip6_frag {
	u_int8_t  ip6f_nxt;		/* next header */
	u_int8_t  ip6f_reserved;	/* reserved field */
	u_int16_t ip6f_offlg;		/* offset, reserved, and flag */
	u_int32_t ip6f_ident;		/* identification */
};

#if BYTE_ORDER == BIG_ENDIAN
#define IP6F_OFF_MASK		0xfff8	/* mask out offset from _offlg */
#define IP6F_RESERVED_MASK	0x0006	/* reserved bits in ip6f_offlg */
#define IP6F_MORE_FRAG		0x0001	/* more-fragments flag */
#else /* BYTE_ORDER == LITTLE_ENDIAN */
#define IP6F_OFF_MASK		0xf8ff	/* mask out offset from _offlg */
#define IP6F_RESERVED_MASK	0x0600	/* reserved bits in ip6f_offlg */
#define IP6F_MORE_FRAG		0x0100	/* more-fragments flag */
#endif /* BYTE_ORDER == LITTLE_ENDIAN */

/*
 * Internet implementation parameters.
 */
#define IPV6_MAXHLIM	255	/* maximun hoplimit */
#define IPV6_DEFHLIM	64	/* default hlim */
#define IPV6_FRAGTTL	120	/* ttl for fragment packets, in slowtimo tick */
#define IPV6_HLIMDEC	1	/* subtracted when forwaeding */

#define IPV6_MMTU	1280	/* minimal MTU and reassembly. 1024 + 256 */
#define IPV6_MAXPACKET	65535	/* ip6 max packet size without Jumbo payload*/

/*
 * IP6_EXTHDR_CHECK ensures that region between the IP6 header and the
 * target header (including IPv6 itself, extension headers and
 * TCP/UDP/ICMP6 headers) are continuous. KAME requires drivers
 * to store incoming data into one internal mbuf or one or more external
 * mbufs(never into two or more internal mbufs). Thus, the third case is
 * supposed to never be matched but is prepared just in case.
 */

#define IP6_EXTHDR_CHECK(m, off, hlen, ret)				\
do {									\
    if ((m)->m_next != NULL) {						\
	if (((m)->m_flags & M_LOOP) &&					\
	    ((m)->m_len < (off) + (hlen)) &&				\
	    (((m) = m_pullup((m), (off) + (hlen))) == NULL)) {		\
		ip6stat.ip6s_exthdrtoolong++;				\
		return ret;						\
	} else if ((m)->m_flags & M_EXT) {				\
		if ((m)->m_len < (off) + (hlen)) {			\
			ip6stat.ip6s_exthdrtoolong++;			\
			m_freem(m);					\
			return ret;					\
		}							\
	} else {							\
		if ((m)->m_len < (off) + (hlen)) {			\
			ip6stat.ip6s_exthdrtoolong++;			\
			m_freem(m);					\
			return ret;					\
		}							\
	}								\
    }									\
    else {								\
	if ((m)->m_len < (off) + (hlen)) {				\
		ip6stat.ip6s_tooshort++;				\
		in6_ifstat_inc(m->m_pkthdr.rcvif, ifs6_in_truncated);	\
		m_freem(m);						\
		return ret;						\
	}								\
    }									\
} while (0)

#ifdef __NetBSD__
/*
 * IP6_EXTHDR_GET ensures that intermediate protocol header (from "off" to
 * "len") is located in single mbuf, on contiguous memory region.
 * The pointer to the region will be returned to pointer variable "val",
 * with type "typ".
 * IP6_EXTHDR_GET0 does the same, except that it aligns the structure at the
 * very top of mbuf.  GET0 is likely to make memory copy than GET.
 *
 * XXX we're now testing this, needs m_pulldown()
 */
#define IP6_EXTHDR_GET(val, typ, m, off, len) \
do {									\
	struct mbuf *t;							\
	int tmp;							\
	t = m_pulldown((m), (off), (len), &tmp);			\
	if (t) {							\
		if (t->m_len < tmp + (len))				\
			panic("m_pulldown malfunction");		\
		(val) = (typ)(mtod(t, caddr_t) + tmp);			\
	} else								\
		(val) = (typ)NULL;					\
} while (0)

#define IP6_EXTHDR_GET0(val, typ, m, off, len) \
do {									\
	struct mbuf *t;							\
	t = m_pulldown((m), (off), (len), NULL);			\
	if (t) {							\
		if (t->m_len < (len))					\
			panic("m_pulldown malfunction");		\
		(val) = (typ)mtod(t, caddr_t);				\
	} else								\
		(val) = (typ)NULL;					\
} while (0)

#endif /*NetBSD*/

#endif /* not _NETINET_IPV6_H_ */
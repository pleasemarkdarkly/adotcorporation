//==========================================================================
//
//      include/netinet6_ip6_var.h
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

/*	$OpenBSD: ip6_var.h,v 1.2 1999/12/10 10:04:28 angelos Exp $	*/

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
 *	@(#)ip_var.h	8.1 (Berkeley) 6/10/93
 */

#ifndef _NETINET6_IP6_VAR_H_
#define _NETINET6_IP6_VAR_H_

/*
 * IP6 reassembly queue structure.  Each fragment
 * being reassembled is attached to one of these structures.
 */
struct	ip6q {
	u_long	ip6q_head;
	u_short	ip6q_len;
	u_char	ip6q_nxt;
	u_char	ip6q_hlim;
	struct	ip6asfrag *ip6q_down;
	struct	ip6asfrag *ip6q_up;
	u_long	ip6q_ident;
	u_char	ip6q_arrive;
	u_char	ip6q_ttl;
	struct  in6_addr ip6q_src, ip6q_dst;
	struct	ip6q *ip6q_next;
	struct	ip6q *ip6q_prev;
	int	ip6q_unfrglen;
#ifdef notyet
	u_char	*ip6q_nxtp;
#endif
};

struct	ip6asfrag {
	u_long	ip6af_head;
	u_short	ip6af_len;
	u_char	ip6af_nxt;
	u_char	ip6af_hlim;
	/* must not override the above members during reassembling */
	struct	ip6asfrag *ip6af_down;
	struct	ip6asfrag *ip6af_up;
	u_short	ip6af_mff;
	u_short	ip6af_off;
	struct	mbuf *ip6af_m;
	u_long	ip6af_offset;		/* offset where next header starts */
	u_short ip6af_frglen;	/* fragmentable part length */
	u_char	ip6af_x1[10];
};

#define IP6_REASS_MBUF(ip6af) (*(struct mbuf **)&((ip6af)->ip6af_m))

struct	ip6_moptions {
	struct	ifnet *im6o_multicast_ifp; /* ifp for outgoing multicasts */
	u_char	im6o_multicast_hlim;	/* hoplimit for outgoing multicasts */
	u_char	im6o_multicast_loop;	/* 1 >= hear sends if a member */
	LIST_HEAD(, in6_multi_mship) im6o_memberships;
};

/*
 * Control options for outgoing packets
 */

/* Routing header related info */
struct	ip6po_rhinfo {
	struct	ip6_rthdr *ip6po_rhi_rthdr; /* Routing header */
	struct	route_in6 ip6po_rhi_route; /* Route to the 1st hop */
};
#define ip6po_rthdr	ip6po_rhinfo.ip6po_rhi_rthdr
#define ip6po_route	ip6po_rhinfo.ip6po_rhi_route

struct	ip6_pktopts {
	struct	mbuf *ip6po_m;	/* Pointer to mbuf storing the data */
	int	ip6po_hlim;		/* Hoplimit for outgoing packets */
	struct	in6_pktinfo *ip6po_pktinfo; /* Outgoing IF/address information */
	struct	sockaddr *ip6po_nexthop;	/* Next-hop address */
	struct	ip6_hbh *ip6po_hbh; /* Hop-by-Hop options header */
	struct	ip6_dest *ip6po_dest1; /* Destination options header(1st part) */
	struct	ip6po_rhinfo ip6po_rhinfo; /* Routing header related info. */
	struct	ip6_dest *ip6po_dest2; /* Destination options header(2nd part) */
};

struct	ip6stat {
	u_quad_t ip6s_total;		/* total packets received */
	u_quad_t ip6s_tooshort;		/* packet too short */
	u_quad_t ip6s_toosmall;		/* not enough data */
	u_quad_t ip6s_fragments;	/* fragments received */
	u_quad_t ip6s_fragdropped;	/* frags dropped(dups, out of space) */
	u_quad_t ip6s_fragtimeout;	/* fragments timed out */
	u_quad_t ip6s_fragoverflow;	/* fragments that exceeded limit */
	u_quad_t ip6s_forward;		/* packets forwarded */
	u_quad_t ip6s_cantforward;	/* packets rcvd for unreachable dest */
	u_quad_t ip6s_redirectsent;	/* packets forwarded on same net */
	u_quad_t ip6s_delivered;	/* datagrams delivered to upper level*/
	u_quad_t ip6s_localout;		/* total ip packets generated here */
	u_quad_t ip6s_odropped;		/* lost packets due to nobufs, etc. */
	u_quad_t ip6s_reassembled;	/* total packets reassembled ok */
	u_quad_t ip6s_fragmented;	/* datagrams sucessfully fragmented */
	u_quad_t ip6s_ofragments;	/* output fragments created */
	u_quad_t ip6s_cantfrag;		/* don't fragment flag was set, etc. */
	u_quad_t ip6s_badoptions;	/* error in option processing */
	u_quad_t ip6s_noroute;		/* packets discarded due to no route */
	u_quad_t ip6s_badvers;		/* ip6 version != 6 */
	u_quad_t ip6s_rawout;		/* total raw ip packets generated */
	u_quad_t ip6s_badscope;		/* scope error */
	u_quad_t ip6s_notmember;	/* don't join this multicast group */
	u_quad_t ip6s_nxthist[256];	/* next header history */
	u_quad_t ip6s_m1;		/* one mbuf */
	u_quad_t ip6s_m2m[32];		/* two or more mbuf */
	u_quad_t ip6s_mext1;		/* one ext mbuf */
	u_quad_t ip6s_mext2m;		/* two or more ext mbuf */
	u_quad_t ip6s_exthdrtoolong;	/* ext hdr are not continuous */
	u_quad_t ip6s_nogif;		/* no match gif found */
	u_quad_t ip6s_toomanyhdr;	/* discarded due to too many headers */
	/* XXX the following two items are not really AF_INET6 thing */
	u_quad_t ip6s_pulldown;		/* # of calls to m_pulldown */
	u_quad_t ip6s_pulldown_copy;	/* # of mbuf copies in m_pulldown */
	u_quad_t ip6s_pulldown_alloc;	/* # of mbuf allocs in m_pulldown */
};

#ifdef _KERNEL
/* flags passed to ip6_output as last parameter */
#define	IPV6_DADOUTPUT		0x01	/* DAD */
#define	IPV6_FORWARDING		0x02	/* most of IPv6 header exists */

extern struct	ip6stat ip6stat;	/* statistics */
extern u_int32_t ip6_id;		/* fragment identifier */
extern int	ip6_defhlim;		/* default hop limit */
extern int	ip6_defmcasthlim;	/* default multicast hop limit */
extern int	ip6_forwarding;		/* act as router? */
extern int	ip6_forward_srcrt;	/* forward src-routed? */
extern int	ip6_gif_hlim;		/* Hop limit for gif encap packet */
extern int	ip6_use_deprecated;	/* allow deprecated addr as source */
extern int	ip6_rr_prune;		/* router renumbering prefix
					 * walk list every 5 sec.    */
#ifdef MAPPED_ADDR_ENABLED
extern int	ip6_mapped_addr_on;
#endif /* MAPPED_ADDR_ENABLED */

extern struct socket *ip6_mrouter; 	/* multicast routing daemon */
extern int	ip6_sendredirects;	/* send IP redirects when forwarding? */
extern int	ip6_maxfragpackets; /* Maximum packets in reassembly queue */
extern int	ip6_sourcecheck;	/* Verify source interface */
extern int	ip6_sourcecheck_interval; /* Interval between log messages */
extern int	ip6_accept_rtadv;	/* Acts as a host not a router */
extern int	ip6_keepfaith;		/* Firewall Aided Internet Translator */
extern int	ip6_log_interval;
extern time_t	ip6_log_time;
extern int	ip6_hdrnestlimit; /* upper limit of # of extension headers */
extern int	ip6_dad_count;		/* DupAddrDetectionTransmits */

extern u_int32_t ip6_flow_seq;
extern int ip6_auto_flowlabel;

struct in6pcb;
struct inpcb;

int	icmp6_ctloutput __P((int, struct socket *, int, int, struct mbuf **));

void	ip6_init __P((void));
void	ip6intr __P((void));
void	ip6_input __P((struct mbuf *));
void	ip6_freemoptions __P((struct ip6_moptions *));
int	ip6_unknown_opt __P((u_int8_t *, struct mbuf *, int));
char *	ip6_get_prevhdr __P((struct mbuf *, int));
int	ip6_mforward __P((struct ip6_hdr *, struct ifnet *, struct mbuf *));
int	ip6_process_hopopts __P((struct mbuf *, u_int8_t *, int, u_int32_t *,
				 u_int32_t *));
void	ip6_savecontrol __P((struct inpcb *, struct mbuf **, struct ip6_hdr *,
		struct mbuf *));
int	ip6_sysctl __P((int *, u_int, void *, size_t *, void *, size_t));

void	ip6_forward __P((struct mbuf *, int));

void	ip6_mloopback __P((struct ifnet *, struct mbuf *, struct sockaddr_in6 *));
int	ip6_output __P((struct mbuf *, struct ip6_pktopts *,
			struct route_in6 *, int,
			struct ip6_moptions *, struct ifnet **));
int	ip6_ctloutput __P((int, struct socket *, int, int, struct mbuf **));
int	ip6_setpktoptions __P((struct mbuf *, struct ip6_pktopts *, int));
int	ip6_optlen __P((struct inpcb *));

int	route6_input __P((struct mbuf **, int *, int));

void	frag6_init __P((void));
int	frag6_input __P((struct mbuf **, int *, int));
void	frag6_slowtimo __P((void));
void	frag6_drain __P((void));

void	rip6_init __P((void));
int	rip6_input __P((struct mbuf **mp, int *offp, int proto));
int	rip6_ctloutput __P((int, struct socket *, int, int, struct mbuf **));
int	rip6_output __P((struct mbuf *, ...));
int	rip6_usrreq __P((struct socket *,
	    int, struct mbuf *, struct mbuf *, struct mbuf *, struct proc *));

int	dest6_input __P((struct mbuf **, int *, int));
int	none_input __P((struct mbuf **, int *, int));
#endif /* _KERNEL */

#endif /* !_NETINET6_IP6_VAR_H_ */
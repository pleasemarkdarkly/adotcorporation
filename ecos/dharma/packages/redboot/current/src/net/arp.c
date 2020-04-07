//==========================================================================
//
//      net/arp.c
//
//      Stand-alone ARP support for RedBoot
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

#include <net/net.h>

static struct {
    int      waiting;
    char     *eth;
    char     *ip;
} arp_req;

/*
 * Handle incoming ARP packets.
 */
void
__arp_handler(pktbuf_t *pkt)
{
    arp_header_t *arp = pkt->arp_hdr;
    int hw_type, protocol;

    /*
     * Only handle ethernet hardware and IP protocol.
     */
    protocol = ntohs(arp->protocol);
    hw_type = ntohs(arp->hw_type);
    if ((hw_type == ARP_HW_ETHER) && (protocol == ETH_TYPE_IP)) {
	/*
	 * Handle requests for our ethernet address.
	 */
	if (!memcmp(arp->target_ip, __local_ip_addr, 4)) {
	    if (ntohs(arp->opcode) == ARP_REQUEST) {
		/* format response. */
		arp->opcode = htons(ARP_REPLY);
		memcpy(arp->target_ip, arp->sender_ip,
		       sizeof(ip_addr_t));
		memcpy(arp->target_enet, arp->sender_enet,
		       sizeof(enet_addr_t));
		memcpy(arp->sender_ip, __local_ip_addr,
		       sizeof(ip_addr_t));
		memcpy(arp->sender_enet, __local_enet_addr,
		       sizeof(enet_addr_t));
		pkt->pkt_bytes = sizeof(arp_header_t);
		__enet_send(pkt, &arp->target_enet, ETH_TYPE_ARP);

	    } else if (ntohs(arp->opcode) == ARP_REPLY && arp_req.waiting) {
		if (!memcmp(arp_req.ip, arp->sender_ip, sizeof(ip_addr_t))) {
                    memcpy(arp_req.eth, arp->sender_enet, sizeof(enet_addr_t));
		    arp_req.waiting = 0;
		}
	    }
	}
    }
    __pktbuf_free(pkt);
}


/* 
 * Find the ethernet address of the machine with the given
 * ip address.
 * Return 0 and fills in 'eth_addr' if successful,
 *       -1 if unsuccessful.
 */
int
__arp_request(ip_addr_t *ip_addr, enet_addr_t *eth_addr)
{
    pktbuf_t *pkt;
    arp_header_t *arp;
    unsigned long retry_start;
    enet_addr_t   bcast_addr;
    int           retry;

    /* just fail if can't get a buffer */
    if ((pkt = __pktbuf_alloc(ARP_PKT_SIZE)) == NULL)
	return -1;

    arp = pkt->arp_hdr;
    arp->opcode = htons(ARP_REQUEST);
    arp->hw_type = htons(ARP_HW_ETHER);
    arp->protocol = htons(0x800);
    arp->hw_len = sizeof(enet_addr_t);
    arp->proto_len = sizeof(ip_addr_t);

    memcpy(arp->sender_ip, __local_ip_addr, sizeof(ip_addr_t));
    memcpy(arp->sender_enet, __local_enet_addr, sizeof(enet_addr_t));
    memcpy(arp->target_ip, ip_addr, sizeof(ip_addr_t));

    bcast_addr[0] = 255;
    bcast_addr[1] = 255;
    bcast_addr[2] = 255;
    bcast_addr[3] = 255;
    bcast_addr[4] = 255;
    bcast_addr[5] = 255;

    arp_req.eth = (char *)eth_addr;
    arp_req.ip = (char *)ip_addr;
    arp_req.waiting = 1;

    retry = 8;
    while (retry-- > 0) {

        /* send the packet */
	pkt->pkt_bytes = sizeof(arp_header_t);
        __enet_send(pkt, &bcast_addr, ETH_TYPE_ARP);

        retry_start = MS_TICKS();
	while ((MS_TICKS() - retry_start) < 250) {
	    __enet_poll();
	    if (!arp_req.waiting) {
		__pktbuf_free(pkt);
		return 0;
	    }
	}
    }
    __pktbuf_free(pkt);
    return -1;
}

#define NUM_ARP 16
static ip_route_t routes[NUM_ARP];

int
__arp_lookup(ip_addr_t *host, ip_route_t *rt)
{
    int i;
    static int next_arp = 0;

    for (i = 0;  i < NUM_ARP;  i++) {
        if (memcmp(host, &routes[i].ip_addr, sizeof(*host)) == 0) {
            // This is a known host
            memcpy(rt, &routes[i], sizeof(*rt));
            return 0;
        }
    }
    memcpy(&rt->ip_addr, host, sizeof(*host));
    if (__arp_request(host, &rt->enet_addr) < 0) {
        return -1;
    } else {
        memcpy(&routes[next_arp], rt, sizeof(*rt));
        if (++next_arp == NUM_ARP) next_arp = 0;
        return 0;
    }
}

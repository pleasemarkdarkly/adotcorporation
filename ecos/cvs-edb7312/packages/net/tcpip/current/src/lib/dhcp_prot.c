/*==========================================================================
//
//      dhcp_prot.c
//
//      DHCP protocol implementation for DHCP client
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
// Author(s):   hmt
// Contributors: gthomas
// Date:        2000-07-01
// Purpose:     DHCP support
// Description: 
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#define nDHCP_CHATTER // report as we go along?

#include <pkgconf/system.h>
#include <pkgconf/net.h>

#ifdef CYGPKG_NET_DHCP

#if 0
#define perror( txt ) // nothing
#endif

#include <network.h>
#include <dhcp.h>

#include <cyg/infra/cyg_ass.h>

// ------------------------------------------------------------------------
// Get the actual packet size of an initialized buffer

static int
dhcp_size( struct bootp *ppkt )
{
    unsigned char *op;
    
    op = &ppkt->bp_vend[0];
    // First check for the cookie!
    if ( op[0] !=  99 ||
         op[1] != 130 ||
         op[2] !=  83 ||
         op[3] !=  99 ) {
        CYG_FAIL( "Bad DHCP cookie" );
        return 0;
    }
    op += 4;
    while (*op != TAG_END) {
        op += *(op+1)+2;
        if ( op > &ppkt->bp_vend[BP_VEND_LEN-1] ) {
            CYG_FAIL( "Oversize DHCP packet in dhcp_size" );
            return 0;
        }
    }
    return (op - (unsigned char *)ppkt) + 1;
}

// ------------------------------------------------------------------------
// Insert/set an option value in an initialized buffer

static int
set_fixed_tag( struct bootp *ppkt,
               unsigned char tag,
               cyg_uint32 value,
               int len)
{
    unsigned char *op;

    // Initially this will only scan the options field.
    
    op = &ppkt->bp_vend[4];
    while (*op != TAG_END) {
        if ( op > &ppkt->bp_vend[BP_VEND_LEN-1] ) {
            CYG_FAIL( "Oversize DHCP packet in set_fixed_tag" );
            return false;
        }
        if (*op == tag)                 // Found it...
            break;
        op += *(op+1)+2;
    }
    
    if (*op == tag) { // Found it...
        if ( *(op+1) != len ) {
            CYG_FAIL( "Wrong size in set_fixed_tag" );
            return false;           // wrong size
        }
    }
    else { // overwrite the end tag and install a new one
        if ( op + len + 2 > &ppkt->bp_vend[BP_VEND_LEN-1] ) {
            CYG_FAIL( "Oversize DHCP packet in set_fixed_tag append" );
            return false;
        }
        *op = tag;
        *(op+1) = len;
        *(op + len + 2) = TAG_END;
    }
    // and insert the value.  Net order is BE.
    op += len + 2 - 1;              // point to end of value
    while ( len-- > 0 ) {
        *op-- = (unsigned char)(value & 255);
        value >>= 8;
    }
    return true;
}

// ------------------------------------------------------------------------
// Bring up an interface enough to broadcast, before we know who we are

static int
bring_half_up(const char *intf, struct ifreq *ifrp )
{
    int s;
    int one = 1;

    struct sockaddr_in *addrp;
    struct ecos_rtentry route;

    // Ensure clean slate
    route_reinit();  // Force any existing routes to be forgotten

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        perror("socket");
        return false;
    }

    if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &one, sizeof(one))) {
        perror("setsockopt");
        return false;
    }

    addrp = (struct sockaddr_in *) &ifrp->ifr_addr;
    memset(addrp, 0, sizeof(*addrp));
    addrp->sin_family = AF_INET;
    addrp->sin_len = sizeof(*addrp);
    addrp->sin_port = 0;
    addrp->sin_addr.s_addr = INADDR_ANY;

    strcpy(ifrp->ifr_name, intf);
    if (ioctl(s, SIOCSIFADDR, ifrp)) { /* set ifnet address */
        perror("SIOCSIFADDR");
        return false;
    }

    if (ioctl(s, SIOCSIFNETMASK, ifrp)) { /* set net addr mask */
        perror("SIOCSIFNETMASK");
        return false;
    }

    /* the broadcast address is 255.255.255.255 */
    memset(&addrp->sin_addr, 255, sizeof(addrp->sin_addr));
    if (ioctl(s, SIOCSIFBRDADDR, ifrp)) { /* set broadcast addr */
        perror("SIOCSIFBRDADDR");
        return false;
    }

    ifrp->ifr_flags = IFF_UP | IFF_BROADCAST | IFF_RUNNING;
    if (ioctl(s, SIOCSIFFLAGS, ifrp)) { /* set ifnet flags */
        perror("SIOCSIFFLAGS up");
        return false;
    }

    if (ioctl(s, SIOCGIFHWADDR, ifrp) < 0) { /* get MAC address */
        perror("SIOCGIFHWADDR 1");
        return false;
    }

    // Set up routing
    /* the broadcast address is 255.255.255.255 */
    memset(&addrp->sin_addr, 255, sizeof(addrp->sin_addr));
    memset(&route, 0, sizeof(route));
    memcpy(&route.rt_gateway, addrp, sizeof(*addrp));

    addrp->sin_family = AF_INET;
    addrp->sin_port = 0;
    addrp->sin_addr.s_addr = INADDR_ANY;
    memcpy(&route.rt_dst, addrp, sizeof(*addrp));
    memcpy(&route.rt_genmask, addrp, sizeof(*addrp));

    route.rt_dev = ifrp->ifr_name;
    route.rt_flags = RTF_UP|RTF_GATEWAY;
    route.rt_metric = 0;

    if (ioctl(s, SIOCADDRT, &route)) { /* add route */
        if (errno != EEXIST) {
            perror("SIOCADDRT 3");
            return false;
        }
    }

    close(s);

    return true;
}


// ------------------------------------------------------------------------
// DHCP retransmission timeouts and number of tries
// 
// To work better with simulated failures (or real ones!) so that the rest
// of the system is tested, rather than DHCP renewal failures pulling
// everything down, we try a little more zealously than the RFC suggests.

static unsigned char timeout_random = 0;

struct timeout_state {
    unsigned int secs;
    int countdown;
};

static inline void reset_timeout( struct timeval *ptv, struct timeout_state *pstate )
{
    timeout_random++;
    pstate->countdown = 4; // initial fast retries
    pstate->secs = 3 + (timeout_random & 3);
    ptv->tv_sec = 0;
    ptv->tv_usec = 65536 * (2 + (timeout_random & 3)); // 0.1 - 0.3S, about
}

static inline int next_timeout( struct timeval *ptv, struct timeout_state *pstate )
{
    if ( 0 < pstate->countdown-- )
        return true;
    if ( 0 == ptv->tv_sec )
        ptv->tv_sec = pstate->secs;
    else {
        timeout_random++;
        pstate->secs = ptv->tv_sec * 2 - 2 + (timeout_random & 3);
        pstate->countdown = 2; // later fast retries
        ptv->tv_sec = 0;
    }
    return pstate->secs < 100; // If longer, too many tries...
}

// ------------------------------------------------------------------------
// Lease expiry and alarms to notify it

static cyg_alarm_t alarm_function;

static void alarm_function(cyg_handle_t alarm, cyg_addrword_t data)
{
    struct dhcp_lease *lease = (struct dhcp_lease *)data;
    lease->which |= lease->next;
    cyg_semaphore_post( &dhcp_needs_attention );

    // Step the lease on into its next state of being alarmed ;-)
    if ( lease->next & DHCP_LEASE_EX ) {
        cyg_alarm_disable( alarm );
    }
    else if ( lease->next & DHCP_LEASE_T2 ) {
        cyg_alarm_initialize( lease->alarm, lease->expiry, 0 );
        cyg_alarm_enable( lease->alarm );
        lease->next = DHCP_LEASE_EX;
    }
    else if ( lease->next & DHCP_LEASE_T1 ) {
        cyg_alarm_initialize( lease->alarm, lease->t2, 0 );
        cyg_alarm_enable( lease->alarm );
        lease->next = DHCP_LEASE_T2;
    }
}

static inline void no_lease( struct dhcp_lease *lease )
{
    if ( lease->alarm ) {
        // Already set: delete this.
        cyg_alarm_disable( lease->alarm );
        cyg_alarm_delete( lease->alarm );
        lease->alarm = 0;
    }
}

static inline void new_lease( struct bootp *bootp, struct dhcp_lease *lease )
{
    cyg_tick_count_t now = cyg_current_time();
    cyg_tick_count_t then;
    cyg_uint32 tag = 0;
    cyg_resolution_t resolution = 
        cyg_clock_get_resolution(cyg_real_time_clock());
    cyg_handle_t h;

    // Silence any jabbering from past lease on this interface
    no_lease( lease );
    lease->which = lease->next = 0;
    cyg_clock_to_counter(cyg_real_time_clock(), &h);
    cyg_alarm_create( h, alarm_function, (cyg_addrword_t)lease,
                      &lease->alarm, &lease->alarm_obj );

    // extract the lease time and scale it &c to now.
    get_bootp_option( bootp, TAG_DHCP_LEASE_TIME, &tag );

    if ( 0xffffffff == tag ) {
        lease->expiry = 0xffffffffffffffff;
        lease->t2     = 0xffffffffffffffff;
        lease->t1     = 0xffffffffffffffff;
        return; // it's an infinite lease, hurrah!
    }

    then = (cyg_uint64)(ntohl(tag));

    then *= 1000000000; // into nS - we know there is room in a tick_count_t
    then = (then / resolution.dividend) * resolution.divisor; // into system ticks
    lease->expiry = now + then;

    get_bootp_option( bootp, TAG_DHCP_REBIND_TIME, &tag );
    then = (cyg_uint64)(ntohl(tag));
    then *= 1000000000; // into nS - we know there is room in a tick_count_t
    then = (then / resolution.dividend) * resolution.divisor; // into system ticks
    lease->t2 = now + then;

    get_bootp_option( bootp, TAG_DHCP_RENEWAL_TIME, &tag );
    then = (cyg_uint64)(ntohl(tag));
    then *= 1000000000; // into nS - we know there is room in a tick_count_t
    then = (then / resolution.dividend) * resolution.divisor; // into system ticks
    lease->t1 = now + then;

#if 0 // for testing this mechanism
    lease->expiry = now + 5000; // 1000 here makes for failure in the DHCP test
    lease->t2     = now + 3500;
    lease->t1     = now + 2500;
#endif

    lease->next = DHCP_LEASE_T1;

    cyg_alarm_initialize( lease->alarm, lease->t1, 0 );
    cyg_alarm_enable( lease->alarm );
}


// ------------------------------------------------------------------------
// the DHCP state machine - this does all the work

int
do_dhcp(const char *intf, struct bootp *res,
        cyg_uint8 *pstate, struct dhcp_lease *lease)
{
    struct ifreq ifr;
    struct sockaddr_in cli_addr, broadcast_addr, server_addr, rx_addr;
    int s, addrlen;
    int one = 1;
    unsigned char mincookie[] = {99,130,83,99,255} ;
    struct timeval tv;
    struct timeout_state timeout_scratch;
    cyg_uint8 oldstate = *pstate;
    cyg_uint8 msgtype = 0, seen_bootp_reply = 0;

    cyg_uint32 xid;

    // IMPORTANT: xmit is the same as res throughout this; *received is a
    // scratch buffer for reception; its contents are always copied to res
    // when we are happy with them.  So we always transmit from the
    // existing state.
    struct bootp rx_local;
    struct bootp *received = &rx_local;
    struct bootp *xmit = res;

    // First, get a socket on the interface in question.  But Zeroth, if
    // needs be, bring it to the half-up broadcast only state if needs be.
    
    if ( DHCPSTATE_INIT      == oldstate
         || DHCPSTATE_FAILED == oldstate
         || 0                == oldstate ) {
        // either explicit init state or the beginning of time or retry
        if ( ! bring_half_up( intf, &ifr ) )
            return false;
        *pstate = DHCPSTATE_INIT;
        xid = (cyg_uint32)res + (cyg_uint32)(0xffffffff & cyg_current_time());
    }
    else
        xid = res->bp_xid; // use what's there already

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        perror("socket");
        return false;
    }

    if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &one, sizeof(one))) {
        perror("setsockopt");
        return false;
    }

    memset((char *) &cli_addr, 0, sizeof(cli_addr));
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    cli_addr.sin_port = htons(IPPORT_BOOTPC);
    
    memset((char *) &broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    broadcast_addr.sin_port = htons(IPPORT_BOOTPS);

    memset((char *) &server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST); // overwrite later
    server_addr.sin_port = htons(IPPORT_BOOTPS);

    if(bind(s, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) < 0) {
        perror("bind error");
        return false;
    }
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one))) {
        perror("setsockopt SO_REUSEADDR");
        return false;
    }
    if (setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(one))) {
        perror("setsockopt SO_REUSEPORT");
        return false;
    }
    
    // Now, we can launch into the DHCP state machine.  I think this will
    // be the neatest way to do it; it returns from within the switch arms
    // when all is well, or utterly failed.

    reset_timeout( &tv, &timeout_scratch );

    while ( 1 ) {

        // If we are active rather than in the process of shutting down,
        // check for any lease expiry every time round, so that alarms
        // *can* change the course of events even when already renewing,
        // for example.
        if ( DHCPSTATE_DO_RELEASE   != *pstate
             && DHCPSTATE_NOTBOUND  != *pstate
             && DHCPSTATE_FAILED    != *pstate ) {
            cyg_uint8 lease_state;

            cyg_scheduler_lock();
            lease_state = lease->which;
            lease->which = 0; // flag that we have noticed it
            cyg_scheduler_unlock();

            if ( lease_state & DHCP_LEASE_EX ) {
                // then the lease has expired completely!
                *pstate = DHCPSTATE_NOTBOUND;
            }
            else if ( lease_state & DHCP_LEASE_T2 ) {
                // Time to renew
                reset_timeout( &tv, &timeout_scratch ); // next conversation
                *pstate = DHCPSTATE_REBINDING;
            }
            else if ( lease_state & DHCP_LEASE_T1 ) {
                // Time to renew
                reset_timeout( &tv, &timeout_scratch ); // next conversation
                *pstate = DHCPSTATE_RENEWING;
            }
        }

        switch ( *pstate ) {

        case DHCPSTATE_INIT:

            // Send the DHCPDISCOVER packet

            if (ioctl(s, SIOCGIFHWADDR, &ifr) < 0) { /* get MAC address */
                perror("SIOCGIFHWADDR 2");
                return false;
            }

            // Fill in the BOOTP request - DHCPDISCOVER packet
            bzero(xmit, sizeof(*xmit));
            xmit->bp_op = BOOTREQUEST;
            xmit->bp_htype = HTYPE_ETHERNET;
            xmit->bp_hlen = IFHWADDRLEN;
            xmit->bp_xid = xid;
            xmit->bp_secs = 0;
            xmit->bp_flags = htons(0x8000); // BROADCAST FLAG
            bcopy(ifr.ifr_hwaddr.sa_data, &xmit->bp_chaddr, xmit->bp_hlen);
            bcopy(mincookie, xmit->bp_vend, sizeof(mincookie));

            // remove the next line to test ability to handle bootp packets.
            set_fixed_tag( xmit, TAG_DHCP_MESS_TYPE, DHCPDISCOVER, 1 );

            set_fixed_tag( xmit, TAG_DHCP_MAX_MSGSZ, BP_MINPKTSZ, 2 );

#ifdef DHCP_CHATTER
            diag_printf( "---------DHCPSTATE_INIT sending:\n" );
            show_bootp( intf, xmit );
#endif            
            if(sendto(s, xmit, dhcp_size(xmit), 0, 
                      (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr)) < 0) {
                *pstate = DHCPSTATE_FAILED;
                break;
            }

            seen_bootp_reply = 0;
            *pstate = DHCPSTATE_SELECTING;
            break;

        case DHCPSTATE_SELECTING:
            // This is a separate state so that we can listen again
            // *without* retransmitting.
            
            // listen for the DHCPOFFER reply

            setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

            addrlen = sizeof(rx_addr);
            if (recvfrom(s, received, sizeof(struct bootp), 0,
                         (struct sockaddr *)&rx_addr, &addrlen) < 0) {
                // No packet arrived (this time)
                if ( seen_bootp_reply ) { // then already have a bootp reply
                    // Save the good packet in *xmit
                    bcopy( received, xmit, dhcp_size(received) );
                    *pstate = DHCPSTATE_BOOTP_FALLBACK;
                    reset_timeout( &tv, &timeout_scratch );
                    break;
                }       
                // go to the next larger timeout and re-send:
                if ( ! next_timeout( &tv, &timeout_scratch ) ) {
                    *pstate = DHCPSTATE_FAILED;
                    break;
                }
                *pstate = DHCPSTATE_INIT; // to retransmit
                break;
            }

#ifdef DHCP_CHATTER
            diag_printf( "---------DHCPSTATE_SELECTING received:\n" );
            diag_printf( "...rx_addr is family %d, addr %08x, port %d\n",
                         rx_addr.sin_family,
                         rx_addr.sin_addr.s_addr,
                         rx_addr.sin_port );
            show_bootp( intf, received );
#endif            
            if ( received->bp_xid != xid )   // not the same transaction;
                break;                  // listen again...

            if ( 0 == received->bp_siaddr.s_addr ) {
                // then fill in from the options...
                get_bootp_option( received, TAG_DHCP_SERVER_ID,
                                  &received->bp_siaddr.s_addr );
            }

            // see if it was a DHCP reply or a bootp reply; it could be
            // either.
            if ( get_bootp_option( received, TAG_DHCP_MESS_TYPE, &msgtype ) ) {
                if ( DHCPOFFER == msgtype ) { // all is well
                    // Save the good packet in *xmit
                    bcopy( received, xmit, dhcp_size(received) );
                    // we like the packet, so reset the timeout for next time
                    reset_timeout( &tv, &timeout_scratch );
                    *pstate = DHCPSTATE_REQUESTING;
                }
            }
            else // No TAG_DHCP_MESS_TYPE entry so it's a bootp reply
                seen_bootp_reply = 1; // (keep the bootp packet in received)
                
            // If none of the above state changes occurred, we got a packet
            // that "should not happen", OR we have a bootp reply in our
            // hand; so listen again with the same timeout, without
            // retrying the send, in the hope of getting a DHCP reply.
            break;

        case DHCPSTATE_REQUESTING:
            // Just send what you got with a DHCPREQUEST in the message type.
            // then wait for an ACK in DHCPSTATE_REQUEST_RECV.

            // Fill in the BOOTP request - DHCPREQUEST packet
            xmit->bp_op = BOOTREQUEST;
            xmit->bp_flags = htons(0x8000); // BROADCAST FLAG

            set_fixed_tag( xmit, TAG_DHCP_MESS_TYPE, DHCPREQUEST, 1 );
            set_fixed_tag( xmit, TAG_DHCP_MAX_MSGSZ, BP_MINPKTSZ, 2 );

            // And this will be a new one:
            set_fixed_tag( xmit, TAG_DHCP_REQ_IP, ntohl(xmit->bp_yiaddr.s_addr), 4 );
            
#ifdef DHCP_CHATTER
            diag_printf( "---------DHCPSTATE_REQUESTING sending:\n" );
            show_bootp( intf, xmit );
#endif            
            if(sendto(s, xmit, dhcp_size(xmit), 0, 
                      (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr)) < 0) {
                *pstate = DHCPSTATE_FAILED;
                break;
            }

            *pstate = DHCPSTATE_REQUEST_RECV;
            break;

        case DHCPSTATE_REQUEST_RECV:
            // wait for an ACK or a NACK - retry by going back to
            // DHCPSTATE_REQUESTING; NACK means go back to INIT.

            setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

            addrlen = sizeof(rx_addr);
            if (recvfrom(s, received, sizeof(struct bootp), 0,
                         (struct sockaddr *)&rx_addr, &addrlen) < 0) {
                // No packet arrived
                // go to the next larger timeout and re-send:
                if ( ! next_timeout( &tv, &timeout_scratch ) ) {
                    *pstate = DHCPSTATE_FAILED;
                    break;
                }
                *pstate = DHCPSTATE_REQUESTING;
                break;
            }

#ifdef DHCP_CHATTER
            diag_printf( "---------DHCPSTATE_REQUEST_RECV received:\n" );
            diag_printf( "...rx_addr is family %d, addr %08x, port %d\n",
                         rx_addr.sin_family,
                         rx_addr.sin_addr.s_addr,
                         rx_addr.sin_port );
            show_bootp( intf, received );
#endif            
            if ( received->bp_xid != xid )   // not the same transaction;
                break;                  // listen again...

            if ( 0 == received->bp_siaddr.s_addr ) {
                // then fill in from the options...
                get_bootp_option( received, TAG_DHCP_SERVER_ID,
                                  &received->bp_siaddr.s_addr );
            }

            // check it was a DHCP reply
            if ( get_bootp_option( received, TAG_DHCP_MESS_TYPE, &msgtype ) ) {
                if ( DHCPACK == msgtype // Same offer & server?
                     && received->bp_yiaddr.s_addr == xmit->bp_yiaddr.s_addr
                     && received->bp_siaddr.s_addr == xmit->bp_siaddr.s_addr) {
                    // Save the good packet in *xmit
                    bcopy( received, xmit, dhcp_size(received) );
                    // we like the packet, so reset the timeout for next time
                    reset_timeout( &tv, &timeout_scratch );
                    // Record the new lease and set up timers &c
                    new_lease( received, lease );
                    *pstate = DHCPSTATE_BOUND;
                    break;
                }
                if ( DHCPNAK == msgtype // Same server?
                     && received->bp_siaddr.s_addr == xmit->bp_siaddr.s_addr) {
                    // we're bounced!
                    *pstate = DHCPSTATE_INIT;  // So back the start of the rigmarole.
                    reset_timeout( &tv, &timeout_scratch );
                    break;
                }
                // otherwise it's something else, maybe another offer, or a bogus
                // NAK from someone we are not asking!
                // Just listen again, which implicitly discards it.
            }
            break;

        case DHCPSTATE_BOUND:

            // We are happy now, we have our address.

            // All done with socket
            close(s);

            // Re-initialize the interface with the new state
            if ( DHCPSTATE_BOUND != oldstate ) {
                // Then need to go down and up
                do_dhcp_down_net( intf, res, &oldstate, lease ); // oldstate used
                if ( 0 != oldstate ) {
                    // Then not called from init_all_network_interfaces()
                    // so we must initialize the interface ourselves
                    if (!init_net(intf, res)) {
                        do_dhcp_down_net( intf, res, pstate, lease );
                        *pstate = DHCPSTATE_FAILED;
                        return false;
                    }
                }
            }

            // Otherwise, nothing whatsoever to do...
            return true;

        case DHCPSTATE_RENEWING:
            // Just send what you got with a DHCPREQUEST in the message
            // type UNICAST straight to the server.  Then wait for an ACK.

            // Fill in the BOOTP request - DHCPREQUEST packet
            xmit->bp_op = BOOTREQUEST;
            xmit->bp_flags = htons(0); // No BROADCAST FLAG
            // Use the *client* address here:
            xmit->bp_ciaddr.s_addr = xmit->bp_yiaddr.s_addr;

            set_fixed_tag( xmit, TAG_DHCP_MESS_TYPE, DHCPREQUEST, 1 );
            set_fixed_tag( xmit, TAG_DHCP_MAX_MSGSZ, BP_MINPKTSZ, 2 );

            // And this will be a new one:
            set_fixed_tag( xmit, TAG_DHCP_REQ_IP, ntohl(xmit->bp_yiaddr.s_addr), 4 );
            
            // Set unicast address to *server*
            server_addr.sin_addr.s_addr = res->bp_siaddr.s_addr;

#ifdef DHCP_CHATTER
            diag_printf( "---------DHCPSTATE_RENEWING sending:\n" );
            diag_printf( "UNICAST to family %d, addr %08x, port %d\n",
                         server_addr.sin_family,
                         server_addr.sin_addr.s_addr,
                         server_addr.sin_port );
            show_bootp( intf, xmit );
#endif            
            
            if(sendto(s, xmit, dhcp_size(xmit), 0, 
                       // UNICAST address of the server:
                      (struct sockaddr *)&server_addr,
                      sizeof(server_addr)) < 0) {
                *pstate = DHCPSTATE_FAILED;
                break;
            }

            *pstate = DHCPSTATE_RENEW_RECV;
            break;

        case DHCPSTATE_RENEW_RECV:
            // wait for an ACK or a NACK - retry by going back to
            // DHCPSTATE_RENEWING; NACK means go to NOTBOUND.

            setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

            addrlen = sizeof(rx_addr);
            if (recvfrom(s, received, sizeof(struct bootp), 0,
                         (struct sockaddr *)&rx_addr, &addrlen) < 0) {
                // No packet arrived
                // go to the next larger timeout and re-send:
                if ( ! next_timeout( &tv, &timeout_scratch ) ) {
                    reset_timeout( &tv, &timeout_scratch );
                    *pstate = DHCPSTATE_REBINDING;
                    break;
                }
                *pstate = DHCPSTATE_RENEWING;
                break;
            }

#ifdef DHCP_CHATTER
            diag_printf( "---------DHCPSTATE_RENEW_RECV received:\n" );
            diag_printf( "...rx_addr is family %d, addr %08x, port %d\n",
                         rx_addr.sin_family,
                         rx_addr.sin_addr.s_addr,
                         rx_addr.sin_port );
            show_bootp( intf, received );
#endif            
            if ( received->bp_xid != xid )   // not the same transaction;
                break;                  // listen again...

            if ( 0 == received->bp_siaddr.s_addr ) {
                // then fill in from the options...
                get_bootp_option( received, TAG_DHCP_SERVER_ID,
                                  &received->bp_siaddr.s_addr );
            }

            // check it was a DHCP reply
            if ( get_bootp_option( received, TAG_DHCP_MESS_TYPE, &msgtype ) ) {
                if ( DHCPACK == msgtype  // Same offer?
                     && received->bp_yiaddr.s_addr == xmit->bp_yiaddr.s_addr) {
                    // Save the good packet in *xmit
                    bcopy( received, xmit, dhcp_size(received) );
                    // we like the packet, so reset the timeout for next time
                    reset_timeout( &tv, &timeout_scratch );
                    // Record the new lease and set up timers &c
                    new_lease( received, lease );
                    *pstate = DHCPSTATE_BOUND;
                    break;
                }
                if ( DHCPNAK == msgtype ) { // we're bounced!
                    *pstate = DHCPSTATE_NOTBOUND;  // So quit out.
                    reset_timeout( &tv, &timeout_scratch );
                    break;
                }
                // otherwise it's something else, maybe another offer.
                // Just listen again, which implicitly discards it.
            }
            break;

        case DHCPSTATE_REBINDING:
            // Just send what you got with a DHCPREQUEST in the message type.
            // Then wait for an ACK.  This one is BROADCAST.

            // Fill in the BOOTP request - DHCPREQUEST packet
            xmit->bp_op = BOOTREQUEST;
            xmit->bp_flags = htons(0); // no BROADCAST FLAG
            // Use the *client* address here:
            xmit->bp_ciaddr.s_addr = xmit->bp_yiaddr.s_addr;

            set_fixed_tag( xmit, TAG_DHCP_MESS_TYPE, DHCPREQUEST, 1 );
            set_fixed_tag( xmit, TAG_DHCP_MAX_MSGSZ, BP_MINPKTSZ, 2 );

            // And this will be a new one:
            set_fixed_tag( xmit, TAG_DHCP_REQ_IP, ntohl(xmit->bp_yiaddr.s_addr), 4 );
            
#ifdef DHCP_CHATTER
            diag_printf( "---------DHCPSTATE_REBINDING sending:\n" );
            show_bootp( intf, xmit );
#endif            
            if(sendto(s, xmit, dhcp_size(xmit), 0, 
                      (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr)) < 0) {
                *pstate = DHCPSTATE_FAILED;
                break;
            }

            *pstate = DHCPSTATE_REBIND_RECV;
            break;

        case DHCPSTATE_REBIND_RECV:
            // wait for an ACK or a NACK - retry by going back to
            // DHCPSTATE_REBINDING; NACK means go to NOTBOUND.

            setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

            addrlen = sizeof(rx_addr);
            if (recvfrom(s, received, sizeof(struct bootp), 0,
                         (struct sockaddr *)&rx_addr, &addrlen) < 0) {
                // No packet arrived
                // go to the next larger timeout and re-send:
                if ( ! next_timeout( &tv, &timeout_scratch ) ) {
                    reset_timeout( &tv, &timeout_scratch );
                    *pstate = DHCPSTATE_FAILED;
                    break;
                }
                *pstate = DHCPSTATE_REBINDING;
                break;
            }

#ifdef DHCP_CHATTER
            diag_printf( "---------DHCPSTATE_REBIND_RECV received:\n" );
            diag_printf( "...rx_addr is family %d, addr %08x, port %d\n",
                         rx_addr.sin_family,
                         rx_addr.sin_addr.s_addr,
                         rx_addr.sin_port );
            show_bootp( intf, received );
#endif            
            if ( received->bp_xid != xid )   // not the same transaction;
                break;                  // listen again...

            if ( 0 == received->bp_siaddr.s_addr ) {
                // then fill in from the options...
                get_bootp_option( received, TAG_DHCP_SERVER_ID,
                                  &received->bp_siaddr.s_addr );
            }

            // check it was a DHCP reply
            if ( get_bootp_option( received, TAG_DHCP_MESS_TYPE, &msgtype ) ) {
                if ( DHCPACK == msgtype  // Same offer?
                     && received->bp_yiaddr.s_addr == xmit->bp_yiaddr.s_addr) {
                    // Save the good packet in *xmit
                    bcopy( received, xmit, dhcp_size(received) );
                    // we like the packet, so reset the timeout for next time
                    reset_timeout( &tv, &timeout_scratch );
                    // Record the new lease and set up timers &c
                    new_lease( received, lease );
                    *pstate = DHCPSTATE_BOUND;
                    break;
                }
                else if ( DHCPNAK == msgtype ) { // we're bounced!
                    *pstate = DHCPSTATE_NOTBOUND;  // So back the start of the rigmarole.
                    reset_timeout( &tv, &timeout_scratch );
                    break;
                }
                // otherwise it's something else, maybe another offer.
                // Just listen again, which implicitly discards it.
            }
            break;

        case DHCPSTATE_BOOTP_FALLBACK:
            // All done with socket
            close(s);
            // And no lease should have become active, but JIC
            no_lease( lease );
            // Re-initialize the interface with the new state
            if ( DHCPSTATE_BOOTP_FALLBACK != oldstate ) {
                // Then need to go down and up
                do_dhcp_down_net( intf, res, &oldstate, lease ); // oldstate used
                if ( 0 != oldstate ) {
                    // Then not called from init_all_network_interfaces()
                    // so we must initialize the interface ourselves
                    if (!init_net(intf, res)) {
                        do_dhcp_down_net( intf, res, pstate, lease );
                        *pstate = DHCPSTATE_FAILED;
                        return false;
                    }
                }
            }

            // Otherwise, nothing whatsoever to do...
            return true;

        case DHCPSTATE_NOTBOUND:
            // All done with socket
            close(s);
            // No lease active
            no_lease( lease );
            // Leave interface up so app can tidy.
            return false;

        case DHCPSTATE_FAILED:
            // All done with socket
            close(s);
            // No lease active
            no_lease( lease );
            // Unconditionally down the interface.
            do_dhcp_down_net( intf, res, &oldstate, lease );
            return false;

        case DHCPSTATE_DO_RELEASE:
            // We have been forced here by external means, to release the
            // lease for graceful shutdown.

            // Just send what you got with a DHCPRELEASE in the message
            // type UNICAST straight to the server.  No ACK.  Then go to
            // NOTBOUND state.

            xmit->bp_op = BOOTREQUEST;
            xmit->bp_flags = htons(0); // no BROADCAST FLAG
            // Use the *client* address here:
            xmit->bp_ciaddr.s_addr = xmit->bp_yiaddr.s_addr;

            set_fixed_tag( xmit, TAG_DHCP_MESS_TYPE, DHCPRELEASE, 1 );

            // Set unicast address to *server*
            server_addr.sin_addr.s_addr = res->bp_siaddr.s_addr;

#ifdef DHCP_CHATTER
            diag_printf( "---------DHCPSTATE_DO_RELEASE sending:\n" );
            diag_printf( "UNICAST to family %d, addr %08x, port %d\n",
                         server_addr.sin_family,
                         server_addr.sin_addr.s_addr,
                         server_addr.sin_port );
            show_bootp( intf, xmit );
#endif            
            if(sendto(s, xmit, dhcp_size(xmit), 0, 
                       // UNICAST address of the server:
                      (struct sockaddr *)&server_addr,
                      sizeof(server_addr)) < 0) {
                *pstate = DHCPSTATE_FAILED;
                break;
            }

            *pstate = DHCPSTATE_NOTBOUND;
            break;

        default:
            no_lease( lease );
            close(s);
            return false;
        }
    }
    /* NOTREACHED */
    return false;
}

// ------------------------------------------------------------------------
// Bring an interface down, failed to initialize it or lease is expired
// Also part of normal startup, bring down for proper reinitialization

int
do_dhcp_down_net(const char *intf, struct bootp *res,
        cyg_uint8 *pstate, struct dhcp_lease *lease)
{
    struct sockaddr_in *addrp;
    struct ifreq ifr;
    int s;

    // Ensure clean slate
    route_reinit();  // Force any existing routes to be forgotten

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        perror("socket");
        return false;
    }

    addrp = (struct sockaddr_in *) &ifr.ifr_addr;

    // Remove any existing address
    if ( DHCPSTATE_FAILED  == *pstate
         || DHCPSTATE_INIT == *pstate
         || 0              == *pstate ) {
        // it was configured for broadcast only, "half-up"
        memset(addrp, 0, sizeof(*addrp));
        addrp->sin_family = AF_INET;
        addrp->sin_len = sizeof(*addrp);
        addrp->sin_port = 0;
        addrp->sin_addr.s_addr = INADDR_ANY;
    }
    else {
        // get the specific address that was used
        strcpy(ifr.ifr_name, intf);
        if (ioctl(s, SIOCGIFADDR, &ifr)) {
            perror("SIOCGIFADDR 1");
            return false;
        }
    }

    strcpy(ifr.ifr_name, intf);
    if (ioctl(s, SIOCDIFADDR, &ifr)) { /* delete IF addr */
        perror("SIOCDIFADDR1");
    }

    // Shut down interface so it can be reinitialized
    ifr.ifr_flags &= ~(IFF_UP | IFF_RUNNING);
    if (ioctl(s, SIOCSIFFLAGS, &ifr)) { /* set ifnet flags */
        perror("SIOCSIFFLAGS down");
        return false;
    }

    // All done with socket
    close(s);

    if ( 0 != *pstate ) // preserve initial state
        *pstate = DHCPSTATE_INIT;

    return true;
}

// ------------------------------------------------------------------------
// Release (relinquish) a leased address - if we have one - and bring down
// the interface.
int
do_dhcp_release(const char *intf, struct bootp *res,
        cyg_uint8 *pstate, struct dhcp_lease *lease)
{
    if ( 0                           != *pstate
         && DHCPSTATE_INIT           != *pstate
         && DHCPSTATE_NOTBOUND       != *pstate
         && DHCPSTATE_FAILED         != *pstate
         && DHCPSTATE_BOOTP_FALLBACK != *pstate ) {
        *pstate = DHCPSTATE_DO_RELEASE;
        do_dhcp( intf, res, pstate, lease ); // to send the release packet
        cyg_thread_delay( 100 );             // to let it leave the building
    }
    return true;
}

// ------------------------------------------------------------------------

#endif // CYGPKG_NET_DHCP

// EOF dhcp_prot.c

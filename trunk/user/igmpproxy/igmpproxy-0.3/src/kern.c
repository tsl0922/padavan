/*
**  igmpproxy - IGMP proxy based multicast router
**  Copyright (C) 2005 Johnny Egeland <johnny@rlo.org>
**
**  This program is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation; either version 2 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
**
**----------------------------------------------------------------------------
**
**  This software is derived work from the following software. The original
**  source code has been modified from it's original state by the author
**  of igmpproxy.
**
**  smcroute 0.92 - Copyright (C) 2001 Carsten Schill <carsten@cschill.de>
**  - Licensed under the GNU General Public License, either version 2 or
**    any later version.
**
**  mrouted 3.9-beta3 - Copyright (C) 2002 by The Board of Trustees of
**  Leland Stanford Junior University.
**  - Licensed under the 3-clause BSD license, see Stanford.txt file.
**
*/


#include "igmpproxy.h"

int curttl = 0;

void k_set_rcvbuf(int bufsize, int minsize) {
    int delta = bufsize / 2;
    int iter = 0;

    /*
     * Set the socket buffer.  If we can't set it as large as we
     * want, search around to try to find the highest acceptable
     * value.  The highest acceptable value being smaller than
     * minsize is a fatal error.
     */
    if (setsockopt(MRouterFD, SOL_SOCKET, SO_RCVBUF,
                   (char *)&bufsize, sizeof(bufsize)) < 0) {
        bufsize -= delta;
        while (1) {
            iter++;
            if (delta > 1)
                delta /= 2;

            if (setsockopt(MRouterFD, SOL_SOCKET, SO_RCVBUF,
                           (char *)&bufsize, sizeof(bufsize)) < 0) {
                bufsize -= delta;
            } else {
                if (delta < 1024)
                    break;
                bufsize += delta;
            }
        }
        if (bufsize < minsize) {
            my_log(LOG_ERR, 0, "OS-allowed buffer size %u < app min %u",
                bufsize, minsize);
            /*NOTREACHED*/
        }
    }
    my_log(LOG_DEBUG, 0, "Got %d byte buffer size in %d iterations", bufsize, iter);
}

void k_hdr_include(int hdrincl) {
    if (setsockopt(MRouterFD, IPPROTO_IP, IP_HDRINCL,
                   (char *)&hdrincl, sizeof(hdrincl)) < 0)
        my_log(LOG_WARNING, errno, "setsockopt IP_HDRINCL %u", hdrincl);
}


void k_set_ttl(int t) {
#ifndef RAW_OUTPUT_IS_RAW
    unsigned char ttl;

    ttl = t;
    if (setsockopt(MRouterFD, IPPROTO_IP, IP_MULTICAST_TTL,
                   (char *)&ttl, sizeof(ttl)) < 0)
        my_log(LOG_WARNING, errno, "setsockopt IP_MULTICAST_TTL %u", ttl);
#endif
    curttl = t;
}

void k_set_loop(int l) {
    unsigned char loop;

    loop = l;
    if (setsockopt(MRouterFD, IPPROTO_IP, IP_MULTICAST_LOOP,
                   (char *)&loop, sizeof(loop)) < 0)
        my_log(LOG_WARNING, errno, "setsockopt IP_MULTICAST_LOOP %u", loop);
}

void k_set_if(uint32_t ifa) {
    struct in_addr adr;

    adr.s_addr = ifa;
    if (setsockopt(MRouterFD, IPPROTO_IP, IP_MULTICAST_IF,
                   (char *)&adr, sizeof(adr)) < 0)
        my_log(LOG_WARNING, errno, "setsockopt IP_MULTICAST_IF %s",
            inetFmt(ifa, s1));
}

void k_join(struct IfDesc *ifd, uint32_t grp) {
    struct ip_mreq mreq;

    mreq.imr_multiaddr.s_addr = grp;
    mreq.imr_interface.s_addr = ifd->InAdr.s_addr;

    my_log(LOG_NOTICE, 0, "Joining group %s on interface %s", inetFmt(grp, s1), ifd->Name);

    if (setsockopt(MRouterFD, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                   (char *)&mreq, sizeof(mreq)) < 0) {
        int mcastGroupExceeded = (errno == ENOBUFS);
        my_log(LOG_WARNING, errno, "can't join group %s on interface %s",
            inetFmt(grp, s1), ifd->Name);
        if (mcastGroupExceeded) {
            my_log(LOG_WARNING, 0, "Maximum number of multicast groups were exceeded");
#ifdef __linux__
            my_log(LOG_WARNING, 0, "Check settings of '/sbin/sysctl net.ipv4.igmp_max_memberships'");
#endif
        }
    }
}

void k_leave(struct IfDesc *ifd, uint32_t grp) {
    struct ip_mreq mreq;

    mreq.imr_multiaddr.s_addr = grp;
    mreq.imr_interface.s_addr = ifd->InAdr.s_addr;

    my_log(LOG_NOTICE, 0, "Leaving group %s on interface %s", inetFmt(grp, s1), ifd->Name);

    if (setsockopt(MRouterFD, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                   (char *)&mreq, sizeof(mreq)) < 0)
        my_log(LOG_WARNING, errno, "can't leave group %s on interface %s",
            inetFmt(grp, s1), ifd->Name);
}

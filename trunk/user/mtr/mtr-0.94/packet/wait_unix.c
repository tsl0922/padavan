/*
    mtr  --  a network diagnostic tool
    Copyright (C) 2016  Matt Kimball

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as
    published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "wait.h"

#include <assert.h>
#include <errno.h>
#ifdef HAVE_ERROR_H
#include <error.h>
#else
#include "portability/error.h"
#endif
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>

/*
    Gather all the file descriptors which should wake our select call when
    they become readable.
*/
static
int gather_read_fds(
    const struct command_buffer_t *command_buffer,
    const struct net_state_t *net_state,
    fd_set * read_set,
    fd_set * write_set)
{
    int nfds;
    int probe_nfds;
    int ip4_socket;
    int ip6_socket;
    int command_stream = command_buffer->command_stream;

    FD_ZERO(read_set);
    FD_ZERO(write_set);

    FD_SET(command_stream, read_set);
    nfds = command_stream + 1;

    if (net_state->platform.ip4_socket_raw) {
        ip4_socket = net_state->platform.ip4_recv_socket;
        FD_SET(ip4_socket, read_set);
        if (ip4_socket >= nfds) {
            nfds = ip4_socket + 1;
        }
    } else {
        ip4_socket = net_state->platform.ip4_txrx_icmp_socket;
        FD_SET(ip4_socket, read_set);
        if (ip4_socket >= nfds) {
            nfds = ip4_socket + 1;
        }
        ip4_socket = net_state->platform.ip4_txrx_udp_socket;
        FD_SET(ip4_socket, read_set);
        if (ip4_socket >= nfds) {
            nfds = ip4_socket + 1;
        }
    }

    if (net_state->platform.ip6_socket_raw) {
        ip6_socket = net_state->platform.ip6_recv_socket;
        FD_SET(ip6_socket, read_set);
        if (ip6_socket >= nfds) {
            nfds = ip6_socket + 1;
        }
    } else {
        ip6_socket = net_state->platform.ip6_txrx_icmp_socket;
        FD_SET(ip6_socket, read_set);
        if (ip6_socket >= nfds) {
            nfds = ip6_socket + 1;
        }
        ip6_socket = net_state->platform.ip6_txrx_udp_socket;
        FD_SET(ip6_socket, read_set);
        if (ip6_socket >= nfds) {
            nfds = ip6_socket + 1;
        }
    }

    probe_nfds = gather_probe_sockets(net_state, write_set);
    if (probe_nfds > nfds) {
        nfds = probe_nfds;
    }

    return nfds;
}

/*
    Sleep until we receive a new probe response, a new command on the
    command stream, or a probe timeout.  On Unix systems, this means
    we use select to wait on file descriptors for the command stream
    and the raw receive socket.
*/
void wait_for_activity(
    struct command_buffer_t *command_buffer,
    struct net_state_t *net_state)
{
    int nfds;
    fd_set read_set;
    fd_set write_set;
    struct timeval probe_timeout;
    struct timeval *select_timeout;
    int ready_count;

    nfds =
        gather_read_fds(command_buffer, net_state, &read_set, &write_set);

    while (true) {
        select_timeout = NULL;

        /*  Use the soonest probe timeout time as our maximum wait time  */
        if (get_next_probe_timeout(net_state, &probe_timeout)) {
            assert(probe_timeout.tv_sec >= 0);
            select_timeout = &probe_timeout;
        }

        ready_count =
            select(nfds, &read_set, &write_set, NULL, select_timeout);

        /*
           If we didn't have an error, either one of our descriptors is
           readable, or we timed out.  So we can now return.
         */
        if (ready_count != -1) {
            break;
        }

        /*
           We will get EINTR if we received a signal during the select, so
           retry in that case.  We may get EAGAIN if "the kernel was
           (perhaps temporarily) unable to allocate the requested number of
           file descriptors."  I haven't seen this in practice, but selecting
           again seems like the right thing to do.
         */
        if (errno != EINTR && errno != EAGAIN) {
            /*  We don't expect other errors, so report them  */
            error(EXIT_FAILURE, errno, "unexpected select error");
        }
    }
}

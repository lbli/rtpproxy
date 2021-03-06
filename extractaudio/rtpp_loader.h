/*
 * Copyright (c) 2007-2014 Sippy Software, Inc., http://www.sippysoft.com
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
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id$
 *
 */

#ifndef _RTP_LOADER_H_
#define _RTP_LOADER_H_

#include <sys/types.h>
#include <sys/stat.h>

struct rtpp_loader;
struct sessions;
struct channels;
struct rtpp_session_stat;
struct eaud_crypto;

enum origin;

#if !defined(pcap_hdr_t_DEFINED)
typedef struct pcap_hdr_s pcap_hdr_t;
#define pcap_hdr_t_DEFINED 1
#endif

DEFINE_METHOD(rtpp_loader, rtpp_loader_scan, int, struct sessions *);
DEFINE_METHOD(rtpp_loader, rtpp_loader_load, int, struct channels *,
  struct rtpp_session_stat *, enum origin, struct eaud_crypto *);
DEFINE_METHOD(rtpp_loader, rtpp_loader_dtor, void);

struct rtpp_loader {
    int ifd;
    struct stat sb;
    unsigned char *ibuf;
    rtpp_loader_scan_t scan;
    rtpp_loader_load_t load;
    rtpp_loader_dtor_t destroy;

    union {
        struct {
            pcap_hdr_t *pcap_hdr;
        } pcap_data;
        struct {} adhoc_data;
    } private;
};

struct rtpp_loader *rtpp_load(const char *);

#endif

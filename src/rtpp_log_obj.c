/*
 * Copyright (c) 2004-2006 Maxim Sobolev <sobomax@FreeBSD.org>
 * Copyright (c) 2006-2015 Sippy Software, Inc., http://www.sippysoft.com
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
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "rtpp_types.h"
#include "rtpp_log.h"
#include "rtpp_log_obj.h"
#include "rtpp_log_obj_fin.h"
#include "rtpp_genuid_singlet.h"
#include "rtpp_refcnt.h"
#include "rtpp_util.h"

struct rtpp_log_priv
{
    struct rtpp_log_obj pub;
    rtpp_log_t log;
    void *rco[0];
};

#define PUB2PVT(pubp) \
  ((struct rtpp_log_priv *)((char *)(pubp) - offsetof(struct rtpp_log_priv, pub)))

static void rtpp_log_obj_dtor(struct rtpp_log_priv *);
static void rtpp_log_obj_setlevel(struct rtpp_log_obj *, int);
static void rtpp_log_obj_write(struct rtpp_log_obj *, const char *, int, const char *, ...);
static void rtpp_log_obj_ewrite(struct rtpp_log_obj *, const char *, int, const char *, ...);

struct rtpp_log_obj *
rtpp_log_obj_ctor(struct rtpp_cfg_stable *cfs, const char *app,
  const char *call_id, int flags)
{
    struct rtpp_log_priv *pvt;

    pvt = rtpp_zmalloc(sizeof(struct rtpp_log_priv) +
      rtpp_refcnt_osize());
    if (pvt == NULL) {
        return (NULL);
    }
    pvt->log = rtpp_log_open(cfs, app, call_id, flags);
    pvt->pub.rcnt = rtpp_refcnt_ctor_pa(&pvt->rco[0], pvt,
      (rtpp_refcnt_dtor_t)&rtpp_log_obj_dtor);
    if (pvt->pub.rcnt == NULL) {
        free(pvt);
        return (NULL);
    }
    rtpp_gen_uid(&pvt->pub.lguid);
    pvt->pub.setlevel = &rtpp_log_obj_setlevel;
    pvt->pub.write = rtpp_log_obj_write;
    pvt->pub.ewrite = rtpp_log_obj_ewrite;
    return (&pvt->pub);
}

static void
rtpp_log_obj_dtor(struct rtpp_log_priv *pvt)
{

    rtpp_log_obj_fin(&pvt->pub);
    rtpp_log_close(pvt->log);
    free(pvt);
}

static void
rtpp_log_obj_setlevel(struct rtpp_log_obj *self, int log_level)
{
    struct rtpp_log_priv *pvt;

    pvt = PUB2PVT(self);
    rtpp_log_setlevel(pvt->log, log_level);
}

static void
rtpp_log_obj_write(struct rtpp_log_obj *self, const char *fname, int level,
  const char *fmt, ...)
{
    va_list ap;
    struct rtpp_log_priv *pvt;

    pvt = PUB2PVT(self);
    va_start(ap, fmt);
    _rtpp_log_write_va(pvt->log, level, fname, fmt, ap);
    va_end(ap);
    return;
}

static void
rtpp_log_obj_ewrite(struct rtpp_log_obj *self, const char *fname, int level,
  const char *fmt, ...)
{
    va_list ap;
    struct rtpp_log_priv *pvt;

    pvt = PUB2PVT(self);
    va_start(ap, fmt);
    _rtpp_log_ewrite_va(pvt->log, level, fname, fmt, ap);
    va_end(ap);
    return;
}

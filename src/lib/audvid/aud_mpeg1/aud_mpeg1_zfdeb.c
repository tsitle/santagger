/******************************************************************************/
/* aud_mpeg1_zfdeb.c            [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Debugging functions for MPEG-1 streams                                     */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 06.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#define SRC_AUD_MPEG1_ZFDEB_C
#include "aud_mpeg1_zfdeb-prot.h"
/*** */
#include "../av_zfdeb-prot.h"
#undef SRC_AUD_MPEG1_ZFDEB_C

/*
// System-Includes
*/
#include <stdarg.h>      /* va_list, ... */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
st_mpeg1_d_deb(const Tst_mpeg1_opts *pOpts,
		const char *pFnc, const char *pFmt, ...)
{
	va_list args;

	ST_ASSERTN_VOID(pOpts == NULL ||
			! ST_AVFDEB_HASCBS_DEB_BD(pOpts->basOpts))

	va_start(args, pFmt);
	st_audFDeb_dbg(ST_B_TRUE, &pOpts->basOpts,
			pFnc, NULL, pFmt, args);
	va_end(args);
}

void
st_mpeg1_d_debOrAna(const Tst_mpeg1_opts *pOpts,
		const Tst_bool isNormOrWarn,
		const char *pFnc, const char *pFmt, ...)
{
	char    msg1[4096 * 2],
	        msg2[2048 * 2];
	va_list args;

	ST_ASSERTN_VOID(pOpts == NULL)

	va_start(args, pFmt);
	vsnprintf(msg2, sizeof(msg2), pFmt, args);
	if (pOpts->anlz && ST_AVFDEB_HASCB_ANAL_P(pOpts)) {
		snprintf(msg1, sizeof(msg1), "AUD: %s%s",
				(isNormOrWarn ? "" : "! "), msg2);
		pOpts->cbAnaMsg(msg1);
	} else if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
		st_mpeg1_d_deb(pOpts, pFnc, msg2);
	va_end(args);
}

/*----------------------------------------------------------------------------*/

void
st_mpeg1_d_err(const Tst_mpeg1_opts *pOpts, const char *pFnc,
		const Tst_str *pFn, const char *pFmt, ...)
{
	va_list args;

	ST_ASSERTN_VOID(pOpts == NULL ||
			! ST_AVFDEB_HASCBS_ERR_BD(pOpts->basOpts))

	va_start(args, pFmt);
	st_audFDeb_dbg(ST_B_FALSE, &pOpts->basOpts,
			pFnc, pFn, pFmt, args);
	va_end(args);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if (ST_AUD_MPEG1_DEB_ == 1)
void st_mpeg1_prf(const char *pFmt, ...)
{
	va_list args;

	va_start(args, pFmt);
	vprintf(pFmt, args);
	va_end(args);
}

void st_mpeg1_prE(const char *pFmt, ...)
{
	va_list args;

	va_start(args, pFmt);
	vfprintf(stderr, pFmt, args);
	va_end(args);
}

void st_mpeg1_prBuf(const Tst_buf *pBuf, const Tst_uint32 sz)
{
	Tst_uint32 x,
	           y = 0;

	for (x = 0; x < sz; x++) {
		printf("%c(%02x) ", (pBuf[x] >= 0x20 && pBuf[x] <= 0x7f ? pBuf[x] : '.'), pBuf[x]);
		if (++y >= 20) {
			printf("\n");
			y = 0;
		}
	}
}
#endif

/******************************************************************************/
